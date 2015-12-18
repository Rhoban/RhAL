#include "DynamixelV1.hpp"

namespace RhAL
{
    DynamixelV1::DynamixelV1(Bus &bus)
        : Protocol(bus)
    {
    }

    void DynamixelV1::writeData(id_t id, addr_t address, 
            uint8_t *data, size_t size)
    {
        uint8_t buffer[packetSize(size)];
    }

    ResponseState DynamixelV1::readData(id_t id, addr_t address, 
            uint8_t *data, size_t size)
    {
    }

    bool DynamixelV1::ping(id_t id)
    {
    }

    std::vector<ResponseState> DynamixelV1::syncRead(std::vector<id_t> ids, addr_t address,
            std::vector<uint8_t*> datas, size_t size)
    {
    }

    void DynamixelV1::syncWrite(std::vector<id_t> ids, addr_t address,
            std::vector<uint8_t*> datas, size_t size)
    {
    }

    size_t DynamixelV1::packetSize(size_t size)
    {
    }
}
