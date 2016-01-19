#pragma once

#include <vector>
#include <stdint.h>
#include <types.h>
#include <Bus/Bus.hpp>

namespace RhAL
{
    static constexpr addr_t Broadcast = 0xfe;

    typedef unsigned int ResponseState;
    enum : ResponseState {
        // We got a correct response from the device
        ResponseOK=1,

        // The device status
        ResponseOverload=2,
        ResponseOverheat=4,
        ResponseBadVoltage=8,
        ResponseAlert=16,

        // There was an error
        ResponseQuiet=32,
        ResponseBadChecksum=64,
        ResponseDeviceBadInstruction=128,
        ResponseDeviceBadChecksum=256,
        ResponseBadSize=512,
        ResponseBadProtocol=1024,
        ResponseBadId=2048
    };        

    class Protocol
    {
        public:
            Protocol(Bus &bus);

            /**
             * Virtual destructor
             */
            virtual ~Protocol();

            /**
             * Write size bytes of data on device with id at given address
             */
            virtual void writeData(id_t id, addr_t address, 
                    uint8_t *data, size_t size)=0;

            /**
             * Reads size bytes of data on device with id at given address
             *
             * If it fails, false will be returned
             */
            virtual ResponseState readData(id_t id, addr_t address, 
                    uint8_t *data, size_t size)=0;

            /**
             * These are helpers that internally use writeData and readData
             *
             * Note that there is no response state here, this will throw exceptions
             */
            virtual uint8_t readByte(id_t id, addr_t address);
            virtual void writeByte(id_t id, addr_t address, uint8_t byte);
            virtual uint16_t readWord(id_t id, addr_t address);
            virtual void writeWord(id_t id, addr_t address, uint16_t word);

            /**
             * This will ping a servo and return true if it responded, false else
             */
            virtual bool ping(id_t id)=0;

            /**
             * Perform a synchronized read across devices
             */
            virtual std::vector<ResponseState> syncRead(std::vector<id_t> ids, addr_t address,
                    std::vector<uint8_t*> datas, size_t size)=0;
            
            /**
             * Performs a synchronized write across devices
             */
            virtual void syncWrite(std::vector<id_t> ids, addr_t address,
                    std::vector<uint8_t*> datas, size_t size)=0;

        protected:
            // Bus used for communication
            Bus &bus;
    };
}
