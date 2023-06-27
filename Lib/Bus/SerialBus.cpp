#include <unistd.h>
#include <iostream>
#include "SerialBus.hpp"

namespace RhAL
{
SerialBus::SerialBus(std::string port, unsigned int baudrate)
  : serial(port, baudrate, serial::Timeout::simpleTimeout(1000))
{
}

#define SERIAL_CATCH(method)                                                                                           \
                                                                                                                       \
  catch (serial::IOException e)                                                                                        \
  {                                                                                                                    \
    std::cerr << "WARNING: SerialBus::" #method "(): IOException " << e.what() << std::endl;                           \
    retryOpening();                                                                                                    \
  }                                                                                                                    \
  catch (serial::SerialException e)                                                                                    \
  {                                                                                                                    \
    std::cerr << "WARNING: SerialBus::" #method "(): SerialException " << e.what() << std::endl;                       \
    retryOpening();                                                                                                    \
  }                                                                                                                    \
  catch (serial::PortNotOpenedException e)                                                                             \
  {                                                                                                                    \
    std::cerr << "WARNING: SerialBus::" #method "(): PortNotOpenedException " << e.what() << std::endl;                \
    retryOpening();                                                                                                    \
  }

void SerialBus::retryOpening()
{
  while (true)
  {
    serial.close();

    try
    {
      serial.open();
      break;
    }
    catch (serial::IOException e)
    {
      std::cerr << "WARNING: SerialBus::reopen(): IOException " << e.what() << std::endl;
    }
    catch (serial::SerialException e)
    {
      std::cerr << "WARNING: SerialBus::reopen(): SerialException " << e.what() << std::endl;
    }

    usleep(5000);
  }
};

bool SerialBus::sendData(uint8_t* data, size_t size)
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    return serial.write(data, size) == size;
  }
  SERIAL_CATCH(sendData)

  return false;
}

bool SerialBus::waitForData(double timeout)
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    return serial.waitReadable(timeout);
  }
  SERIAL_CATCH(waitForData)

  return false;
}

size_t SerialBus::available()
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    return serial.available();
  }
  SERIAL_CATCH(available)

  return 0;
}

size_t SerialBus::readData(uint8_t* data, size_t size)
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    return serial.read(data, size);
  }
  SERIAL_CATCH(readData)

  return 0;
}

void SerialBus::flush()
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    serial.flush();
  }
  SERIAL_CATCH(flush)
}

void SerialBus::clearInputBuffer()
{
  std::lock_guard<std::mutex> lock(mutex);
  try
  {
    serial.flushInput();
  }
  SERIAL_CATCH(clearInputBuffer)
}
}  // namespace RhAL
