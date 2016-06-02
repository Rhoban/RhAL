#include <iostream>
#include "SerialBus.hpp"

namespace RhAL
{
    SerialBus::SerialBus(std::string port, unsigned int baudrate)
        : serial(port, baudrate, serial::Timeout::simpleTimeout(1000))
    {
    }

    bool SerialBus::sendData(uint8_t *data, size_t size)
    {
        try {
            return serial.write(data, size)==size;
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::sendData(): IOException " << e.what() << std::endl;
            return false;
        }
    }

    bool SerialBus::waitForData(double timeout)
    {
        try {
            return serial.waitReadable(timeout);
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::waitForData(): IOException " << e.what() << std::endl;
            return false;
        }
    }

    size_t SerialBus::available()
    {
        try {
            return serial.available();
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::available(): IOException " << e.what() << std::endl;
            return 0;
        }
    }

    size_t SerialBus::readData(uint8_t *data, size_t size)
    {
        try {
            return serial.read(data, size);
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::readData(): IOException " << e.what() << std::endl;
            return 0;
        }
    }

    void SerialBus::flush()
    {
        try {
            serial.flush();
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::flush(): IOException " << e.what() << std::endl;
        }
    }

    void SerialBus::clearInputBuffer() {
        try {
        	serial.flushInput();
        } catch (serial::IOException e) {
            std::cerr << "WARNING: SerialBus::clearInputBuffer(): IOException " << e.what() << std::endl;
        }
//        int n = this->available();
//        uint8_t dummy[n];
//        this->readData(dummy, n);
    }
}
