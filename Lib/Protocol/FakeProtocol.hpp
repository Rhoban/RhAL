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
        virtual void writeData(
            id_t id, addr_t address, 
            const uint8_t *data, size_t size) override;
        virtual ResponseState readData(
            id_t id, addr_t address, 
            uint8_t *data, size_t size) override;
        virtual bool ping(id_t id) override;
        virtual std::vector<ResponseState> syncRead(
            const std::vector<id_t>& ids, addr_t address,
            const std::vector<uint8_t*>& datas, size_t size) override;
        virtual void syncWrite(
            const std::vector<id_t>& ids, addr_t address,
            const std::vector<const uint8_t*>& datas, size_t size) override;
        virtual void emergencyStop() override;
        virtual void exitEmergencyState() override;
};

}

