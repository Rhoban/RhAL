#pragma once

#include "Protocol.hpp"

namespace RhAL {

/**
 * FakeProtocol
 *
 * Simple testing mock protocol
 */
class FakeProtocol : public Protocol
{
    public:

        FakeProtocol(Bus& bus);

        /**
         * Inherit from Protocol
         */
        void writeData(id_t id, addr_t address,
                       const uint8_t *data, size_t size);
        ResponseState writeAndCheckData(id_t id, addr_t address,
	               uint8_t *data, size_t size);
        ResponseState readData(id_t id, addr_t address,
                    uint8_t *data, size_t size);
        bool ping(id_t id);
        std::vector<ResponseState> syncRead(
                const std::vector<id_t>& ids, addr_t address,
                const std::vector<uint8_t*>& datas, size_t size);
        void syncWrite(
                const std::vector<id_t>& ids, addr_t address,
                const std::vector<const uint8_t*>& datas, size_t size);
        std::vector<ResponseState> syncWriteAndCheck(
                const std::vector<id_t>& ids, addr_t address,
                const std::vector<uint8_t*>& datas, size_t size);
        virtual void emergencyStop() override;
        virtual void exitEmergencyState() override;
};

}

