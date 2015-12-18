#pragma once

#include "Protocol.hpp"

namespace RhAL
{
    class DynamixelV1 : public Protocol
    {
        enum DynamixelV1Commands {
            CommandPing = 0x01,
            CommandRead = 0x02,
            CommandWrite = 0x03,
            CommandSyncWrite = 0x83,
            CommandSyncRead = 0x84
        };

        public:
            DynamixelV1(Bus &bus);

            void writeData(id_t id, addr_t address, 
                    uint8_t *data, size_t size);

            ResponseState readData(id_t id, addr_t address, 
                    uint8_t *data, size_t size);
            
            bool ping(id_t id);

            std::vector<ResponseState> syncRead(std::vector<id_t> ids, addr_t address,
                    std::vector<uint8_t*> datas, size_t size);
            
            void syncWrite(std::vector<id_t> ids, addr_t address,
                    std::vector<uint8_t*> datas, size_t size);

        protected:
            size_t packetSize(size_t size);
    };
}
