#include "SerialBus.hpp"

namespace RhAL
{
    SerialBus::SerialBus(std::string port, unsigned int baudrate)
        : serial(port, baudrate, serial::Timeout::simpleTimeout(1000))
    {
    }

    bool SerialBus::sendData(uint8_t *data, size_t size)
    {
        return serial.write(data, size)==size;
    }

    bool SerialBus::waitForData(unsigned int timeout)
    {
        return serial.waitReadable(timeout);
    }

    size_t SerialBus::readData(uint8_t *data, size_t size)
    {
        return serial.read(data, size);
    }

    void SerialBus::flush()
    {
        serial.flush();
    }
}
