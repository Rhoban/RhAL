#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include <json.hpp>
#include "AggregateManager.hpp"
#include "Bus/SerialBus.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/ProtocolFactory.hpp"

namespace RhAL {

/**
 * Manager
 *
 * Main interface class for lowlevel
 * hardware device communication.
 * All suported derived Device types are given
 * by the variadic template parameters.
 */
template <typename ... Types> 
class Manager : public AggregateManager<Types...>
{
    public:

        /**
         * Initialization
         */
        inline Manager() :
            AggregateManager<Types...>(),
            _sortedRegisters(),
            _readCycleCount(0),
            _bus(nullptr),
            _protocol(nullptr),
            _parametersList(),
            _paramBusPort("port", ""),
            _paramBusBaudrate("baudrate", 1000000),
            _paramProtocolName("protocol", "FakeProtocol")
        {
            //Registering all parameters
            _parametersList.add(&_paramBusPort);
            _parametersList.add(&_paramBusBaudrate);
            _parametersList.add(&_paramProtocolName);
        }

        /**
         * Bus and Protocol deallocation
         */
        inline ~Manager()
        {
            if (_protocol != nullptr) {
                delete _protocol;
                _protocol = nullptr;
            }
            if (_bus != nullptr) {
                delete _bus;
                _bus = nullptr;
            }
        }

        /**
         * TODO
         */
        inline void loadParameters(const std::string& filename)
        {
        }
        inline void writeParameters(const std::string& filename)
        {
        }

        /**
         * Reset and initialize the
         * Bus and Protocol instance.
         * (Need to be call before any flushRead() 
         * or flushWrite() or after any bus/protocol 
         * parameters update)
         */
        inline void initBus()
        {
            //Free existing instance
            if (_protocol != nullptr) {
                delete _protocol;
                _protocol = nullptr;
            }
            if (_bus != nullptr) {
                delete _bus;
                _bus = nullptr;
            }
            //Allocate Bus and Protocol
            if (_paramBusPort.value != "") {
                _bus = new SerialBus(_paramBusPort.value, _paramBusBaudrate.value);
            }
            _protocol = ProtocolFactory(_paramProtocolName.value, *_bus);
            //Check that Protocol implementation name is valid
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "Manager invalid protocol name: " 
                    + _paramProtocolName.value);
            }
        }

        /**
         * TODO
         */
        inline void flushRead()
        {
            /*
            std::cout << "---------- Start flushRead" << std::endl;
            for (size_t i=0;i<_sortedRegisters.size();i++) {
                if (isNeedRead(_sortedRegisters[i])) {
                    std::cout 
                        << "id=" << _sortedRegisters[i]->id
                        << " addr=" << _sortedRegisters[i]->addr
                        << " len=" << _sortedRegisters[i]->length 
                        << " name=" << _sortedRegisters[i]->name << std::endl;
                }
            }
            */
            
            //Compute Read batching
            std::vector<BatchedRegisters> batchs = computeBatchedRegisters(
                [this](const Register* reg) -> bool {
                    return this->isNeedRead(reg);
                });
            //Read all batchs
            for (size_t i=0;i<batchs.size();i++) {
                readBatch(batchs[i]);
            }
            //Increment Read counter
            _readCycleCount++;

            /*
            for (size_t i=0;i<container.size();i++) {
                std::cout << "Batched: addr=" << container[i].addr << " len=" << container[i].length << " IDS:";
                for (size_t j=0;j<container[i].regs.size();j++) {
                    std::cout << container[i].regs[j]->name << "(" << container[i].regs[j]->id << "), ";
                }
                std::cout << std::endl;
            }
            */

        }

        /**
         * TODO
         */
        inline void flushWrite()
        {
            //Compute Write batching
            std::vector<BatchedRegisters> batchs = computeBatchedRegisters(
                [this](const Register* reg) -> bool {
                    return this->isNeedWrite(reg);
                });
            //Send all batchs
            for (size_t i=0;i<batchs.size();i++) {
                writeBatch(batchs[i]);
            }
        }

        /**
         * Swap all Registers double buffers.
         * (Has to be called before/after
         * Read and Write operations)
         * (This method must be called by 
         * the same thread as flush*() methods)
         */
        inline void swapBuffers()
        {
            CallManager::swapDataBuffers();
        }

        /**
         * Shorthand for flushRead(),
         * flushWrite() then swapBuffers().
         */
        inline void flush()
        {
            flushRead();
            flushWrite();
            swapBuffers();
        }
        
        /**
         * Inherit.
         * Call when a register is declared 
         * to the Device. Use to build up
         * in Manager the set of all sorted 
         * Register pointers.
         * Register are given by its Device id
         * and its name
         */
        inline virtual void onNewRegister(
            id_t id, const std::string& name) override
        {
            //Retrieve the nex register and 
            //add the pointer to the container
            _sortedRegisters.push_back(
                &(this->devById(id)
                .registersList()
                .get(name)));
            //Re sort the container by id and then by address
            std::sort(_sortedRegisters.begin(), _sortedRegisters.end(),
                [](const Register* pt1, const Register* pt2) -> bool
                {
                    if (pt1->id == pt2->id) {
                        return pt1->addr < pt2->addr;
                    } else {
                        return pt1->id < pt2->id;
                    }
                });
        }

        /**
         * TODO
         */
        inline nlohmann::json saveJSON() const
        {
            nlohmann::json j = AggregateManager<Types...>::saveAggregatedJSON();
            j["Manager"] = _parametersList.saveJSON();
            return j;
        }
        /*
        inline void loadJSON(const nlohmann::json& j)
        {
        }
        */

    private:

        /**
         * Internal structure 
         * for a batch of registers
         */
        struct BatchedRegisters {
            //Start address
            addr_t addr;
            //Batch length
            size_t length;
            //Container of 
            //registers batched
            std::vector<Register*> regs;
        };
        
        /**
         * Container of all Register pointers
         * sorted by their id and then by address 
         * for fast packets batching
         */
        std::vector<Register*> _sortedRegisters;

        /**
         * Count all readFlush() calls
         */
        unsigned long _readCycleCount;

        /**
         * Serial bus and Protocol pointers
         */
        SerialBus* _bus;
        Protocol* _protocol;
        
        /**
         * Container of bool, number and 
         * string device parameters
         */
        ParametersList _parametersList;

        /**
         * Bus and protocol parameters.
         * BusPort: system path to serial device
         * BusBaudrate: serial port baudrate
         * ProtocolName: textual name for Protocol 
         * (factory) instantiation
         */
        ParameterStr _paramBusPort;
        ParameterNumber _paramBusBaudrate;
        ParameterStr _paramProtocolName;

        /**
         * Return true if given Register pointer
         * is mark has to be read or write
         */
        inline bool isNeedRead(const Register* reg) const
        {
            if (CallManager::_bufferMode) {
                return 
                    reg->_needRead2 || 
                    (reg->periodPackedRead > 0 &&
                    (_readCycleCount % reg->periodPackedRead == 0));
            } else {
                return
                    reg->_needRead1 || 
                    (reg->periodPackedRead > 0 &&
                    (_readCycleCount % reg->periodPackedRead == 0));
            }
        }
        inline bool isNeedWrite(const Register* reg) const
        {
            if (CallManager::_bufferMode) {
                return reg->_needWrite2;
            } else {
                return reg->_needWrite1;
            }
        }

        /**
         * Reset given Register pointer for
         * read or write operation.
         * Set timestamp with given timepoint.
         */
        inline void resetRegForRead(Register* reg, 
            const TimePoint& timestamp)
        {
            if (CallManager::_bufferMode) {
                reg->_needRead2 = false;
                reg->_lastDevRead2 = timestamp;
            } else {
                reg->_needRead1 = false;
                reg->_lastDevRead1 = timestamp;
            }
        }
        inline void resetRegForWrite(Register* reg)
        {
            if (CallManager::_bufferMode) {
                reg->_needWrite2 = false;
            } else {
                reg->_needWrite1 = false;
            }
        }

        /**
         * Iterate over all registers and batch them 
         * into compatible groups (address and length).
         * A register is selected for batching if
         * given predicate function selectRegister()
         * return true.
         */
        std::vector<BatchedRegisters> computeBatchedRegisters(
            std::function<bool(const Register*)> selectRegister)
        {
            //Batched registers container
            std::vector<BatchedRegisters> container;

            //Initialize the for the first register
            BatchedRegisters tmpBatch;
            if (_sortedRegisters.size() > 0) {
                tmpBatch.addr = _sortedRegisters.front()->addr;
                tmpBatch.length = _sortedRegisters.front()->length;
                tmpBatch.regs = {_sortedRegisters.front()};
            }

            //Merge the given temporary batch to 
            //final batches by merging by id
            auto mergeById = [&container](const BatchedRegisters& tmpBatch) {
                bool found = false;
                //Already added final batch are iterated
                //to find if the current batch can be merge
                //with another batch by id with constant address 
                //and length.
                for (size_t j=0;j<container.size();j++) {
                    if (container[j].addr == tmpBatch.addr && container[j].length == tmpBatch.length) {
                        for (size_t k=0;k<tmpBatch.regs.size();k++) {
                            container[j].regs.push_back(tmpBatch.regs[k]);
                        }
                        found = true;
                        break;
                    }
                }
                //If no compatible final batch are 
                //found a new one is created
                if (!found) {
                    container.push_back(tmpBatch);
                }
            };

            //Iterate over all sorted Registers 
            //by id and then by address
            size_t index = 1;
            while (index < _sortedRegisters.size()) {
                Register* reg = _sortedRegisters[index];
                //Select batched registers according to
                //the given predicate function
                if (selectRegister(reg)) {
                    bool isContigious = 
                        (tmpBatch.addr + tmpBatch.length == reg->addr) &&
                        (reg->id == tmpBatch.regs.front()->id);
                    if (isContigious) {
                        //If the register is contigious to current
                        //batch, it is added to it.
                        //Registers are first batched by address
                        //with id constant.
                        tmpBatch.length += reg->length;
                        tmpBatch.regs.push_back(reg);
                    } else {
                        //If the register is not contiguous,
                        //the temporaty batch is added to
                        //the final container.
                        mergeById(tmpBatch);
                        //And a new temporary batch is initialize
                        tmpBatch.addr = reg->addr;
                        tmpBatch.length = reg->length;
                        tmpBatch.regs = {reg};
                    }
                }
                index++;
            }
            //Merge the last batch
            mergeById(tmpBatch);

            return container;
        }

        /**
         * Actually Write and Read on the bus 
         * given batched Registers
         * TODO handle error.........XXX
         */
        inline void writeBatch(BatchedRegisters& batch)
        {
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "Manager protocol not initialized");
            }
            //Converted and write the typed 
            //value into the data buffer
            for (size_t i=0;i<batch.regs.size();i++) {
                batch.regs[i]->doConvIn(CallManager::_bufferMode);
            }
            if (batch.regs.size() == 1) {
                //Write single register
                _protocol->writeData(
                    batch.regs.front()->id, 
                    batch.addr, 
                    batch.regs.front()->_dataBuffer, 
                    batch.length);
            } else {
                //Synch Write multiple registers
                std::vector<id_t> ids;
                std::vector<data_t*> datas;
                for (size_t i=0;i<batch.regs.size();i++) {
                    if (ids.size() == 0 || 
                        batch.regs[i]->id != ids.back()
                    ) {
                        //Do not insert an id twice
                        ids.push_back(batch.regs[i]->id);
                    }
                    datas.push_back(batch.regs[i]->_dataBuffer);
                }
                _protocol->syncWrite(
                    ids, 
                    batch.addr,
                    datas,
                    batch.length);
            }
            //Reset dirty flags
            for (size_t i=0;i<batch.regs.size();i++) {
                resetRegForWrite(batch.regs[i]);
            }
        }
        inline void readBatch(BatchedRegisters& batch)
        {
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "Manager protocol not initialized");
            }
            if (batch.regs.size() == 1) {
                //Read single register
                ResponseState state = _protocol->readData(
                    batch.regs.front()->id, 
                    batch.addr, 
                    batch.regs.front()->_dataBuffer, 
                    batch.length);
                //Check for communication error
                if (state != ResponseOK) {
                    //TODO XXX XXX XXX handle response code
                }
            } else {
                //Synch Read multiple registers
                std::vector<id_t> ids;
                std::vector<data_t*> datas;
                for (size_t i=0;i<batch.regs.size();i++) {
                    if (ids.size() == 0 || 
                        batch.regs[i]->id != ids.back()
                    ) {
                        //Do not insert an id twice
                        ids.push_back(batch.regs[i]->id);
                    }
                    datas.push_back(batch.regs[i]->_dataBuffer);
                }
                std::vector<ResponseState> states = _protocol->syncRead(
                    ids, 
                    batch.addr,
                    datas,
                    batch.length);
                //Check for communication error
                    //TODO XXX XXX XXX handle response code
            }
            //Retrieve the read timestamp
            TimePoint timestamp = getTimePoint();
            //Reset dirty flags, set read timestamp
            //and convert the data buffer into typed value
            for (size_t i=0;i<batch.regs.size();i++) {
                resetRegForRead(batch.regs[i], timestamp);
                batch.regs[i]->doConvOut(CallManager::_bufferMode);
            }
        }
};

}

