#pragma once

#include <vector>
#include <algorithm>
#include <functional>
#include "AggregateManager.hpp"

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
        Manager() :
            AggregateManager<Types...>(),
            _sortedRegisters(),
            _readCycleCount(0)
        {
        }

        /**
         *
         */
        inline void flushRead()
        {
            std::cout << "---------- Start flushRead" << std::endl;
            for (size_t i=0;i<_sortedRegisters.size();i++) {
                if (isNeedRead(_sortedRegisters[i])) {
                    //std::cout << _sortedRegisters[i]->name << " " << _sortedRegisters[i]->id << " " << _sortedRegisters[i]->addr << std::endl;
                    std::cout 
                        << "id=" << _sortedRegisters[i]->id
                        << " addr=" << _sortedRegisters[i]->addr
                        << " len=" << _sortedRegisters[i]->length 
                        << " name=" << _sortedRegisters[i]->name << std::endl;
                }
            }
            std::vector<BatchedRegisters> container = computeBatchedRegisters(
                [this](const Register* reg) -> bool {
                    return this->isNeedRead(reg);
                });

            for (size_t i=0;i<container.size();i++) {
                std::cout << "Batched: addr=" << container[i].addr << " len=" << container[i].length << " IDS:";
                for (size_t j=0;j<container[i].regs.size();j++) {
                    std::cout << container[i].regs[j]->name << "(" << container[i].regs[j]->id << "), ";
                }
                std::cout << std::endl;
            }


            CallManager::swapDataBuffers();
            _readCycleCount++;
        }

        /**
         *
         */
        inline void flushWrite()
        {
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
         *
         */
        inline void writeBatch(BatchedRegisters& batch)
        {
            for (size_t i=0;i<batch.regs.size();i++) {
                batch.regs[i].doConvIn(CallManager::_bufferMode);
            }
            if (batch.regs.size() == 1) {
            } else {
            }
            for (size_t i=0;i<batch.regs.size();i++) {
                resetRegForWrite(batch.regs[i]);
            }
        }
        inline void readBatch(BatchedRegisters& batch)
        {
            TimePoint timestamp;
            if (batch.regs.size() == 1) {
                timestamp = getCurrentTimePoint();
            } else {
                timestamp = getCurrentTimePoint();
            }
            for (size_t i=0;i<batch.regs.size();i++) {
                resetRegForRead(batch.regs[i], timestamp);
                batch.regs[i].doConvIn(CallManager::_bufferMode);
            }
        }
};

}

