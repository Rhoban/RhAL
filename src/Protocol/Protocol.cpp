#include <stdexcept>
#include "Protocol.hpp"

namespace RhAL
{
    uint8_t Protocol::readByte(id_t id, addr_t address)
    {
        uint8_t byte;
        if (readData(id, address, &byte, 1)&ResponseOK) {
            return byte;
        } else {
            throw runtime_error("Read error");
        }
    }

    void Protocol::writeByte(id_t id, addr_t address, uint8_t byte)
    {
        writeData(id, address, &byte, 1);
    }

    uint16_t Protocol::readWord(id_t id, addr_t address)
    {
        uint8_t bytes[2];
        if (readData(id, address, bytes, 2)&ResponseOK) {
            return (bytes[1]<<8)|(bytes[0];
        } else {
            throw runtime_error("Read error");
        }
    }

    void Protocol::writeWord(id_t id, addr_t address, uint16_t word)
    {
        uint8_t bytes[2];
        bytes[0] = word&0xff;
        bytes[1] = (word>>8)&0xff;

        writeData(id, address, bytes, 2);
    }

    ResponseState Protocol::sendCommandResponse(id_t id, uint8_t *commandData,
            size_t commandSize, uint8_t *responseData, size_t responseSize)
    {
        sendCommand(id, commandData, commandSize);

        return receiveResponse(id, responseData, responseSize);
    }
}
