#pragma once

#include "Protocol.hpp"

namespace RhAL
{
    class DynamixelV1 : public Protocol
    {
        enum DynamixelV1Command {
            CommandPing = 0x01,
            CommandRead = 0x02,
            CommandWrite = 0x03,
            CommandSyncWrite = 0x83,
            CommandSyncRead = 0x84
        };

        enum DynamixelV1Error {
            ErrorVoltage = 1,
            ErrorAngleLimit = 2,
            ErrorOverheat = 4,
            ErrorRange = 8,
            ErrorChecksum = 16,
            ErrorOverload = 32,
            ErrorInstruction = 64
        };
        
        class Packet {
            public:
                Packet(id_t id, DynamixelV1Command instruction, size_t parameters);
                Packet(id_t id, size_t parameters);
                virtual ~Packet();

                void append(uint8_t byte);
                void append(uint8_t *data, size_t size);
                void setError(uint8_t error);
                uint8_t getError();
                uint8_t computeChecksum();
                void prepare();
                size_t getSize();
                uint8_t *getParameters();

                size_t position;
                uint8_t *buffer;
                size_t parameters;
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
            void sendPacket(Packet &packet);
            ResponseState receivePacket(Packet* &response);
    };
}
