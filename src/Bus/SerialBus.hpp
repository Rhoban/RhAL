#pragma once

#include <string>
#include <serial/serial.h>
#include "Bus.hpp"

namespace RhAL
{
    class SerialBus : public Bus
    {
        public:
            SerialBus(std::string port, unsigned int baudrate);
            
            bool sendData(uint8_t *data, size_t size);
            bool waitForData(unsigned int timeout);
            size_t available();
            size_t readData(uint8_t *data, size_t size);
            void flush();

        protected:
            serial::Serial serial;
    };
}
