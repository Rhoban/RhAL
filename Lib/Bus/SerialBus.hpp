#pragma once

#include <string>
#include <serial/serial.h>
#include <mutex>
#include "Bus.hpp"

namespace RhAL
{
class SerialBus : public Bus
{
public:
  SerialBus(std::string port, unsigned int baudrate);

  bool sendData(uint8_t* data, size_t size);
  bool waitForData(double timeout);
  size_t available();
  size_t readData(uint8_t* data, size_t size);
  void flush();
  void clearInputBuffer();
  void retryOpening();

protected:
  serial::Serial serial;
  std::mutex mutex;
};
}  // namespace RhAL
