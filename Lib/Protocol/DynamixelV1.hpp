#pragma once

#include "Protocol.hpp"
#include "Manager/Parameter.hpp"

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

            /**
             * Append data to the buffer
             */
            void append(uint8_t byte);
            void append(const uint8_t *data, size_t size);

            /**
             * Sets the status packet error
             */
            void setError(uint8_t error);

            /**
             * Gets the status packet error
             */
            uint8_t getError();

            /**
             * Compute the checksum
             */
            uint8_t computeChecksum();

            /**
             * Prepare the packet for the network (adds headers and checksum in
             * the buffer)
             */
            void prepare();

            /**
             * Gets the total size of the packet (including header and checksum)
             */
            size_t getSize();

            /**
             * Gets a pointer to the parameters
             */
            uint8_t *getParameters();

            /**
             * Buffer and number of parameters
             */
            uint8_t *buffer;
            size_t parameters;

          protected:
            size_t position;
        };

      public:
        DynamixelV1(Bus &bus);

        /**
         * Implementations from Protocol
         */
        void writeData(id_t id, addr_t address,
                       const uint8_t *data, size_t size);
            ResponseState readData(id_t id, addr_t address,
                    uint8_t *data, size_t size);
            bool ping(id_t id);
            std::vector<ResponseState> syncRead(
                const std::vector<id_t>& ids, addr_t address,
                const std::vector<uint8_t*>& datas, size_t size);
        void syncWrite(
            const std::vector<id_t>& ids, addr_t address,
            const std::vector<const uint8_t*>& datas, size_t size);
        /**
         * Broadcasts a "disable torque" command
         */
        void emergencyStop();

        /**
         * Broadcasts an "enable torque" command
         */
        void exitEmergencyState();

      protected:
        /**
         * This sends a packet over the bus
         */
        void sendPacket(Packet &packet);

        /**
         * Waits to receive a packet over the bus
        */
        ResponseState receivePacket(Packet* &response, id_t id);

      private:

        /**
         * Parameters
         * timeout: wait for receive packet in secondes
         * waitAfterWrite: a delay in seconds to wait
         * after each write
         */
        ParameterNumber _timeout;
        ParameterNumber _waitAfterWrite;
    };
}
