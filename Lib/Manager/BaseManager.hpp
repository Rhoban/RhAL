#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <json.hpp>
#include <fstream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <string>
#include <condition_variable>
#include "Statistics.hpp"
#include "Device.hpp"
#include "CallManager.hpp"
#include "Bus/SerialBus.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/ProtocolFactory.hpp"

namespace RhAL {

/**
 * BaseManager
 *
 * Implement lowlevel
 * hardware device communication.
 * Implement all Device access non
 * derived Device specific
 */
class BaseManager : public CallManager
{
    public:

        /**
         * Typedef for device container
         */
        template <typename T>
        using DevicesByName =
            std::unordered_map<std::string, T*>;
        template <typename T>
        using DevicesById =
            std::unordered_map<id_t, T*>;

        /**
         * Initialization
         */
        inline BaseManager() :
            CallManager(),
            _devicesByName(),
            _devicesById(),
            _parametersList(),
            _mutexBus(),
            _sortedRegisters(),
            _readCycleCount(0),
            _managerWaitUser1(),
            _managerWaitUser2(),
            _userWaitManager1(),
            _userWaitManager2(),
            _isManagerBarrierOpen1(false),
            _isManagerBarrierOpen2(false),
            _cooperativeThreadCount(0),
            _currentThreadWaiting1(0),
            _currentThreadWaiting2(0),
            _stats(),
            _bus(nullptr),
            _protocol(nullptr),
            _paramBusPort("port", ""),
            _paramBusBaudrate("baudrate", 1000000),
            _paramProtocolName("protocol", "FakeProtocol"),
            _paramEnableSyncRead("enableSyncRead", true),
            _paramEnableSyncWrite("enableSyncWrite", true),
            _paramThrowErrorOnScan("throwErrorOnScan", true),
            _paramThrowErrorOnRead("throwErrorOnRead", true)
        {
            //Registering all parameters
            _parametersList.add(&this->_paramScheduleMode);
            _parametersList.add(&_paramBusPort);
            _parametersList.add(&_paramBusBaudrate);
            _parametersList.add(&_paramProtocolName);
            _parametersList.add(&_paramEnableSyncRead);
            _parametersList.add(&_paramEnableSyncWrite);
            _parametersList.add(&_paramThrowErrorOnScan);
            _parametersList.add(&_paramThrowErrorOnRead);
            //Initialize the low level communication
            initBus();
        }

        /**
         * Bus and Protocol deallocation
         */
        inline ~BaseManager()
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
         * Return a Device with given id or name
         * (all derived types are searched).
         * Throw std::logic_error if asked Device
         * with given type is not found.
         */
        //Overloaded shortcuts
        inline const Device& dev(id_t id) const
        {
            return devById(id);
        }
        inline Device& dev(id_t id)
        {
            return devById(id);
        }
        inline const Device& dev(const std::string& name) const
        {
            return devByName(name);
        }
        inline Device& dev(const std::string& name)
        {
            return devByName(name);
        }
        //Implentations
        inline const Device& devById(id_t id) const
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "BaseManager Device id not found: "
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline Device& devById(id_t id)
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "BaseManager Device id not found: "
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline const Device& devByName(const std::string& name) const
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "BaseManager Device name not found: "
                    + name);
            }
            return *(_devicesByName.at(name));
        }
        inline Device& devByName(const std::string& name)
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "BaseManager Device name not found: "
                    + name);
            }
            return *(_devicesByName.at(name));
        }

        /**
         * Return true if a device is already contained with
         * given name or id for all Device types
         */
        //Overload shortcuts
        inline bool devExists(id_t id) const
        {
            return devExistsById(id);
        }
        inline bool devExists(const std::string& name) const
        {
            return devExistsByName(name);
        }
        //Implementations
        inline bool devExistsById(id_t id) const
        {
            return (_devicesById.count(id) > 0);
        }
        inline bool devExistsByName(const std::string& name) const
        {
            return (_devicesByName.count(name) > 0);
        }

        /**
         * Access to internal map of pointers
         * to all contained Devices for all types.
         * Device are indexed by their name.
         */
        inline const DevicesByName<Device>& devContainer() const
        {
            return _devicesByName;
        }

        /**
         * Add or remove one cooperative
         * thread. At the beginning of each
         * main flush() operation, others
         * declared cooperative threads are waited
         * until all have called waitNextFlush() method.
         */
        inline void addCooperativeThread()
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _cooperativeThreadCount++;
        }
        inline void removeCooperativeThread()
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            if (_cooperativeThreadCount == 0) {
                throw std::logic_error(
                    "BaseManager cooperative threads add/remove mismatch");
            }
            _cooperativeThreadCount--;
        }

        /**
         * Immediately sends a broadcasted signal
         * to put all the devices in emergency mode
         */
        inline void emergencyStop()
        {
            //Check for bus inited
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            _stats.emergencyCount++;
            _protocol->emergencyStop();
        }

        /**
         * Immediately sends a broadcasted signal
         * to exit the emergency state in all the
         * devices
         */
        inline void exitEmergencyState()
        {
            //Check for inited
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            _stats.exitEmergencyCount++;
            _protocol->exitEmergencyState();
        }

        /**
         * Declared cooperative user threads
         * have to call this method at each end
         * of their cycle to wait the next Manager
         * flush() operation.
         */
        inline void waitNextFlush()
        {
            //No cooperative thread if
            //scheduling mode is disable
            if (!CallManager::isScheduleMode()) {
                //Do nothing
                return;
            }
            //Lock the shared mutex
            std::unique_lock<std::mutex> lock(CallManager::_mutex);
            //Statistics
            _stats.waitNextFlushCount++;
            TimePoint pStart = getTimePoint();
            //The lock is now acquired
            //Increment current user in waitNextFlush()
            _currentThreadWaiting1++;
            //Notify the Manager for a new thread waiting
            _managerWaitUser1.notify_all();
            //Wait for the end of the first flush() barrier.
            //(during wait, the shared mutex is released)
            _userWaitManager1.wait(lock,
                [this](){return _isManagerBarrierOpen1;});
            //The lock is re acquire. Decrease the number
            //of waiting thread.
            _currentThreadWaiting1--;
            //Increment the number of waiting thread for the 
            //second barrier
            _currentThreadWaiting2++;
            //Notify the Manager for a new thread waiting
            _managerWaitUser2.notify_all();
            //Wait for the end of the second flush() barrier.
            //(during wait, the shared mutex is released)
            _userWaitManager2.wait(lock,
                [this](){return _isManagerBarrierOpen2;});
            //The lock is re acquire. Decrease the number
            //of waiting thread.
            _currentThreadWaiting2--;
            //Statistics
            TimePoint pStop = getTimePoint();
            _stats.waitManagerDuration +=
                getTimeDuration<TimeDurationMicro>(pStart, pStop);
            //Release the shared mutex
            lock.unlock();
        }

        /**
         * Main Manager tick operation.
         * This method have to be called in
         * its own low level thread if other user
         * thread are using the manager.
         * Following operations are done:
         * - Wait for all declared cooperative user
         *   thread for having called waitNextFlush().
         *   Here, the only running thread is the Manager.
         * - Apply to all registers read value from
         *   last bus read.
         * - Select all registers needed to be read or write.
         *   Compute the batching of read and write operations.
         * - Wake up all cooperative user threads leaving
         *   waitNextFlush().
         *   Here, all threads are running.
         * - Perform all read operation.
         * - Perform all write operations.
         * - Optionnaly swap Registers (if isForceSwap is true)
         *   to apply immediatly read values.
         */
        inline void flush(bool isForceSwap = false)
        {
            //No cooperative thread if
            //scheduling mode is disabled
            if (!CallManager::isScheduleMode()) {
                //Do nothing
                return;
            }
            //Wait for all cooperative user thread to have
            //started to wait in waitNextFlush();
            //(during wait, the shared mutex is released)
            //(The lock is taken once condition is reached)
            std::unique_lock<std::mutex> lock(CallManager::_mutex);
            //Close the second barrier
            _isManagerBarrierOpen2 = false;
            //Statistics
            _stats.flushCount++;
            TimePoint pStart = getTimePoint();
            //Wait for all user thread to have reach the 
            //first barrier
            //(During wait, the lock is release)
            _managerWaitUser1.wait(lock,
                [this](){
                    return _currentThreadWaiting1 == _cooperativeThreadCount;
                });
            //The lock is then re acquire. Open the first barrier.
            _isManagerBarrierOpen1 = true;
            //Notify the users waiting on first barrier
            _userWaitManager1.notify_all();
            //Swap to apply last read change
            swapRead();
            //Call all Devices onSwap() callback
            swapCallBack();
            //Select registers for read and write
            //and compute operation batching
            std::vector<BatchedRegisters> batchsRead =
                computeBatchedRegisters(true);
            std::vector<BatchedRegisters> batchsWrite =
                computeBatchedRegisters(false);
            //Wait for all user thread to have reach the 
            //second barrier
            //(During wait, the lock is release)
            _managerWaitUser2.wait(lock,
                [this](){
                    return _currentThreadWaiting2 == _cooperativeThreadCount;
                });
            //The lock is re acquired. Open the second barrier.
            _isManagerBarrierOpen2 = true;
            //Lock the first barrier
            _isManagerBarrierOpen1 = false;
            //Statistics
            TimePoint pStop = getTimePoint();
            _stats.waitUsersDuration +=
                getTimeDuration<TimeDurationMicro>(pStart, pStop);
            //Notify the users thread waiting on the second barrier
            _userWaitManager2.notify_all();
            //Unlock the shared mutex
            lock.unlock();
            //Perform read operation on all batchs
            for (size_t i=0;i<batchsRead.size();i++) {
                readBatch(batchsRead[i]);
            }
            //Increment Read counter
            _readCycleCount++;
            //Perform write operation on all batchs
            bool needsToWait = false;
            for (size_t i=0;i<batchsWrite.size();i++) {
                writeBatch(batchsWrite[i]);
                //Check if a written register is slow
                for(size_t j=0;j<batchsWrite[i].regs.size();j++) {
                    for(size_t k=0;k<batchsWrite[i].regs[j].size();k++) {
                        if (batchsWrite[i].regs[j][k]->isSlowRegister) {
                            needsToWait = true;
                        }
                    }
                }
            }
            //Optionally force immediate swap read
            if (isForceSwap) {
                forceSwap();
            }
            //If a slow register was written, we need
            //to wait a big amount of time
            if (needsToWait) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(SlowRegisterDelayMs));
            }
        }

        /**
         * Force all Registers to swap in order to
         * apply immediately read values
         */
        inline void forceSwap()
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            swapRead();
        }

        /**
         * Ping a Device with given name or id. 
         * True is returned if the Device is present,
         * false else.
         */
        inline bool ping(const std::string& name)
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "BaseManager no Device with name:" + name);
            } else {
                return ping(_devicesByName.at(name)->id());
            }
        }
        inline bool ping(id_t id)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            bool response = _protocol->ping(id);
            if (_devicesById.count(id) == 1) {
                //If the device is register, 
                //set present flag
                _devicesById.at(id)->setPresent(response);
            }
            return response;
        }

        /**
         * Ping all possible Device Id.
         * New discovered Devices are added.
         * All responding Devices are marked as present
         * and all others are marked as non present.
         * Throw std::logic_error is non supported
         * Device type or id/name error are found.
         */
        inline void scan()
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Mark all Device as not present
            for (auto& it : _devicesByName) {
                it.second->setPresent(false);
            }
            //Iterate over all possible Id
            for (id_t i=IdDevBegin;i<=IdDevEnd;i++) {
                //If the Device exist on the bus
                if (_protocol->ping(i)) {
                    //Retrieve the model number
                    type_t type;
                    bool isSuccess = retrieveTypeNumber(i, type);
                    //Skip the Device if model number
                    //can not be retrieved
                    if (!isSuccess) {
                        continue;
                    }
                    //Check if the Device is already known
                    bool isExist = devExistsById(i);
                    if (isExist && this->devTypeNumberById(i) != type) {
                        //Throw exception if scanned Device id
                        //is already known with a different type
                        throw std::logic_error(
                            "BaseManager scan type mismatch: "
                            + std::to_string(i));
                    } else if (isExist) {
                        //If no type problem mark
                        //the Device as present
                        devById(i).setPresent(true);
                    } else {
                        //Check if the type number is supported
                        //by the manager
                        if (!this->isTypeSupported(type)) {
                            //The type is not supported
                            if (_paramThrowErrorOnScan.value) {
                                throw std::runtime_error(
                                    "BaseManager type found in scan() not supported: "
                                    + std::to_string(type));
                            } else {
                                std::cerr <<
                                    "BaseManager type found in scan() not supported: "
                                    << "id=" << i << " type="
                                    << std::to_string(type) << std::endl;
                                continue;
                            }
                        }
                        //The Device is not yet present,
                        //it is created
                        this->devAddByTypeNumber(i, type);
                        //Set it as present
                        devById(i).setPresent(true);
                    }
                }
            }
        }

        /**
         * Check if all Devices registered are
         * present on the bus.
         * True is returned if all known Devices are
         * anwsering. If at least one Device is missing,
         * false is returned.
         * (Device Present flag is set accordingly)
         */
        inline bool checkDevices()
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Iterate over all known Devices
            bool isMissing = false;
            for (auto& dev : _devicesById) {
                bool response = _protocol->ping(dev.first);
                if (!response) {
                    isMissing = true;
                    dev.second->setPresent(false);
                } else {
                    dev.second->setPresent(true);
                }
            }

            return !isMissing;
        }

        /**
         * Call setConfig on all registered Devices
         * that are present.
         */
        inline void setDevicesConfig()
        {
            for (auto& dev : _devicesById) {
                if (dev.second->isPresent()) {
                    dev.second->setConfig();
                }
            }
        }

        /**
         * Inherit.
         * Call when a register is declared
         * to the Device. Use to build up
         * in Manager the set of all sorted
         * Register pointers.
         * Register are given by its Device id
         * and its name
         * (The user must not call it)
         */
        inline virtual void onNewRegister(
            id_t id, const std::string& name) override
        {
            //Retrieve the next register and
            //add the pointer to the container
            _sortedRegisters.push_back(
                &(devById(id).registersList().reg(name)));
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
         * Inherit
         * Call the Manager to force the immediate
         * Read or Write of the Register given by its
         * Device id and name.
         * (Called by register, not by user)
         */
        inline virtual void forceRegisterRead(
            id_t id, const std::string& name) override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            _stats.forceReadCount++;
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Retrieve Register pointer
            Register* reg = &(devById(id)
                .registersList().reg(name));
            //Reset read flags
            reg->readyForRead();
            //Read single register
            unsigned int nbFails = 0;
            while (true) {
                TimePoint pStart = getTimePoint();
                ResponseState state = _protocol->readData(
                    reg->id,
                    reg->addr,
                    reg->_dataBufferRead,
                    reg->length);
                TimePoint pStop = getTimePoint();
                _stats.readCount++;
                _stats.readLength += reg->length;
                _stats.readDuration +=
                    getTimeDuration<TimeDurationMicro>(pStart, pStop);
                //Check response
                if (checkResponseState(state, _devicesById.at(id))) {
                    //Valid case
                    break;
                } else {
                    //Error case
                    reg->readError();
                    nbFails++;
                    if (nbFails >= MaxForceReadTries) {
                        if (_paramThrowErrorOnRead.value) {
                            throw std::runtime_error(
                                "BaseManager max tries reached when read error: "
                                + reg->name);
                        } else {
                            std::cerr <<
                                "BaseManager max tries reached when read error: "
                                << reg->name << std::endl;
                            return;
                        }
                    }
                }
            }
            //Retrieve the read timestamp
            TimePoint timestamp = getTimePoint();
            //Set swapping flags and set timestamp
            reg->finishRead(timestamp);
            //Do swapping
            reg->swapRead();
        }
        inline virtual void forceRegisterWrite(
            id_t id, const std::string& name) override
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            _stats.forceWriteCount++;
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Retrieve Register pointer
            Register* reg = &(devById(id)
                .registersList().reg(name));
            //Export typed value into data buffer
            reg->selectForWrite();
            //Write the register
            TimePoint pStart = getTimePoint();
            _protocol->writeData(
                reg->id,
                reg->addr,
                reg->_dataBufferWrite,
                reg->length);
            TimePoint pStop = getTimePoint();
            _stats.writeCount++;
            _stats.writeLength += reg->length;
            _stats.writeDuration +=
                getTimeDuration<TimeDurationMicro>(pStart, pStop);
            //Wait delay in case of slow register
            if (reg->isSlowRegister) {
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(SlowRegisterDelayMs));
            }
        }

        /**
         * Return by copy all Manager Statistics
         */
        inline Statistics getStatistics() const
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            return _stats;
        }

        /**
         * Read/Write access to Manager Parameters list
         */
        const ParametersList& parametersList() const
        {
            return _parametersList;
        }
        ParametersList& parametersList()
        {
            return _parametersList;
        }
        
        /**
         * Read/Write access to Protocol Parameters list
         */
        const ParametersList& protocolParametersList() const
        {
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            return _protocol->parametersList();
        }
        ParametersList& protocolParametersList()
        {
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            return _protocol->parametersList();
        }

        /**
         * Set all Bus/Protocol configuration
         * with bus system path name, bus baudrate
         * and protocol name.
         */
        inline void setProtocolConfig(
            const std::string& port,
            unsigned long baudrate,
            const std::string& protocol)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _paramBusPort.value = port;
            _paramBusBaudrate.value = baudrate;
            _paramProtocolName.value = protocol;
            //Reset low level communication (bus/protocol)
            initBus();
        }

        /**
         * Manager Parameters setters
         */
        inline void setEnableSyncRead(bool isEnable)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _paramEnableSyncRead.value = isEnable;
        }
        inline void setEnableSyncWrite(bool isEnable)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _paramEnableSyncWrite.value = isEnable;
        }
        inline void setThrowOnScan(bool isEnable)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _paramThrowErrorOnScan.value = isEnable;
        }
        inline void setThrowOnRead(bool isEnable)
        {
            std::lock_guard<std::mutex> lock(CallManager::_mutex);
            _paramThrowErrorOnRead.value = isEnable;
        }

        /**
         * The BaseManager has to call some
         * functions of AggregateManager
         */
        virtual type_t devTypeNumberById(id_t id) const = 0;
        virtual bool isTypeSupported(type_t type) const = 0;
        virtual void devAddByTypeNumber(id_t id, type_t type) = 0;
        virtual nlohmann::json saveJSON() const = 0;
        virtual void loadJSON(const nlohmann::json& j) = 0;
        virtual void writeConfig(const std::string& filename) const = 0;
        virtual void readConfig(const std::string& filename) = 0;

    protected:

        /**
         * Device container indexed by
         * their name and their id
         */
        DevicesByName<Device> _devicesByName;
        DevicesById<Device> _devicesById;

        /**
         * Container of bool, number and
         * string device parameters
         */
        ParametersList _parametersList;

        /**
         * Reset and initialize the
         * Bus and Protocol instance.
         * (Need to be called after any
         * bus/protocol parameters update)
         */
        inline void initBus()
        {
            std::lock_guard<std::mutex> lockBus(_mutexBus);
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
                _bus = new SerialBus(
                    _paramBusPort.value, _paramBusBaudrate.value);
            }
            _protocol = ProtocolFactory(_paramProtocolName.value, *_bus);
            //Check that Protocol implementation name is valid
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager invalid protocol name: "
                    + _paramProtocolName.value);
            }
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
            //registers batched for 
            //each device id sorted by Register
            //address
            std::vector<std::vector<Register*>> regs;
            //Container of all unique ids
            //of batched registers
            std::vector<id_t> ids;
        };

        /**
         * Mutex protecting the shared
         * communication bus
         */
        mutable std::mutex _mutexBus;

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
         * Condition variable for
         * the Manager waiting that
         * users call waitNextFlush()
         * and for users waiting the end of
         * flush() operation.
         * Multi turn barrier synchronisation needs
         * two waits for both the Manager and Users
         */
        mutable std::condition_variable _managerWaitUser1;
        mutable std::condition_variable _managerWaitUser2;
        mutable std::condition_variable _userWaitManager1;
        mutable std::condition_variable _userWaitManager2;

        /**
         * If fakse, the Manager is flushing
         * and all other cooperative user thread
         * have to wait on the first and second 
         * barrier.
         */
        bool _isManagerBarrierOpen1;
        bool _isManagerBarrierOpen2;

        /**
         * The number of current user cooperative
         * threads waited for their cycle end
         * (waitNextFlush()) at the beginning
         * of flush().
         */
        unsigned int _cooperativeThreadCount;

        /**
         * The number of user cooperative
         * threads currently waiting in
         * waitNextFlush() methods for the
         * first and the second barrier.
         */
        unsigned int _currentThreadWaiting1;
        unsigned int _currentThreadWaiting2;

        /**
         * Manager Statistics container
         */
        Statistics _stats;

        /**
         * Serial bus and Protocol pointers
         */
        SerialBus* _bus;
        Protocol* _protocol;

        /**
         * Bus and protocol parameters.
         * BusPort: system path to serial device.
         * BusBaudrate: serial port baudrate.
         * ProtocolName: textual name for Protocol.
         * (factory) instantiation
         */
        ParameterStr _paramBusPort;
        ParameterNumber _paramBusBaudrate;
        ParameterStr _paramProtocolName;

        /**
         * Register Batching configuration.
         * EnableSyncRead: is protocol syncRead used.
         * EnableSyncWrite: is protocol syncWrite used.
         */
        ParameterBool _paramEnableSyncRead;
        ParameterBool _paramEnableSyncWrite;

        /**
         * Exception error configuration.
         * If true, an std::runtime_error exception
         * is thrown if:
         * ThrowErrorOnScan: unknown device type
         * is found while scanning.
         * ThrowErrorOnRead: maximum read tries is
         * reached while force read fails.
         */
        ParameterBool _paramThrowErrorOnScan;
        ParameterBool _paramThrowErrorOnRead;

        /**
         * Return true if given Register pointer
         * is mark has to be read or write
         */
        inline bool isNeedRead(Register* reg)
        {
            return
                reg->needRead() ||
                (reg->periodPackedRead > 0 &&
                (_readCycleCount % reg->periodPackedRead == 0));
        }
        inline bool isNeedWrite(Register* reg)
        {
            bool isNeed = reg->needWrite();
            //If selected for write, register
            //is reset for write aggregation
            if (isNeed) {
                reg->selectForWrite();
            }
            return isNeed;
        }

        /**
         * Iterate over all registers and batch them
         * into compatible groups (address and length).
         * If isReadOrWrite is true, registers needing
         * read are selected.
         * If isReadOrWrite is false, registers needing
         * write are selected.
         */
        std::vector<BatchedRegisters> computeBatchedRegisters(
            bool isReadOrWrite)
        {
            //Batched registers container
            std::vector<BatchedRegisters> container;

            //Merge the given temporary batch to
            //final batches by merging by id
            auto mergeById = [&container, isReadOrWrite, this]
            (const BatchedRegisters& tmpBatch) {
                //SyncRead/Write is enable whenether
                //configuration boolean are set
                bool isSyncEnable =
                    (isReadOrWrite && this->_paramEnableSyncRead.value) ||
                    (!isReadOrWrite && this->_paramEnableSyncWrite.value);
                bool found = false;
                //Already added final batch are iterated
                //to find if the current batch can be merge
                //with another batch by id with constant address
                //and length.
                for (size_t j=0;j<container.size();j++) {
                    if (
                        isSyncEnable &&
                        container[j].addr == tmpBatch.addr &&
                        container[j].length == tmpBatch.length
                    ) {
                        container[j].regs.push_back(std::vector<Register*>());
                        container[j].ids.push_back(tmpBatch.ids.front());
                        for (size_t k=0;k<tmpBatch.regs.front().size();k++) {
                            container[j].regs.back().push_back(tmpBatch.regs.front()[k]);
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
            BatchedRegisters tmpBatch;
            for (size_t i=0;i<_sortedRegisters.size();i++) {
                Register* reg = _sortedRegisters[i];
                //Select batched registers according to
                //the given predicate function
                if (
                    (isReadOrWrite && isNeedRead(reg)) ||
                    (!isReadOrWrite && isNeedWrite(reg))
                ) {
                    //Initialize the temporary batch if empty
                    if (tmpBatch.regs.size() == 0) {
                        tmpBatch.addr = reg->addr;
                        tmpBatch.length = reg->length;
                        tmpBatch.regs = {{reg}};
                        tmpBatch.ids = {reg->id};
                        //And continue to next register
                        continue;
                    }
                    bool isContigious =
                        (tmpBatch.addr + tmpBatch.length == reg->addr) &&
                        (reg->id == tmpBatch.ids.front());
                    if (isContigious) {
                        //If the register is contigious to current
                        //batch, it is added to it.
                        //Registers are first batched by address
                        //with id constant.
                        tmpBatch.length += reg->length;
                        tmpBatch.regs.front().push_back(reg);
                    } else {
                        //If the register is not contiguous,
                        //the temporaty batch is added to
                        //the final container.
                        mergeById(tmpBatch);
                        //Reset the temporary batch
                        tmpBatch.regs.clear();
                        tmpBatch.ids.clear();
                        //And a new temporary batch is initialize
                        tmpBatch.addr = reg->addr;
                        tmpBatch.length = reg->length;
                        tmpBatch.regs = {{reg}};
                        tmpBatch.ids = {reg->id};
                    }
                }
            }
            //Merge the last batch if not empty
            if (tmpBatch.regs.size() > 0) {
                mergeById(tmpBatch);
            }

            return container;
        }

        /**
         * Actually Write and Read on the bus
         * given batched Registers
         */
        inline void writeBatch(BatchedRegisters& batch)
        {
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            if (batch.ids.size() == 1) {
                //Write single register
                TimePoint pStart = getTimePoint();
                _protocol->writeData(
                    batch.ids.front(),
                    batch.addr,
                    batch.regs.front().front()->_dataBufferWrite,
                    batch.length);
                TimePoint pStop = getTimePoint();
                _stats.writeCount++;
                _stats.writeLength += batch.length;
                _stats.writeDuration +=
                    getTimeDuration<TimeDurationMicro>(pStart, pStop);
            } else {
                //Synch Write multiple registers
                std::vector<const data_t*> datas;
                for (size_t i=0;i<batch.regs.size();i++) {
                    datas.push_back(batch.regs[i].front()->_dataBufferWrite);
                }
                TimePoint pStart = getTimePoint();
                _protocol->syncWrite(
                    batch.ids,
                    batch.addr,
                    datas,
                    batch.length);
                TimePoint pStop = getTimePoint();
                _stats.syncWriteCount++;
                _stats.syncWriteLength += batch.length;
                _stats.syncWriteDuration +=
                    getTimeDuration<TimeDurationMicro>(pStart, pStop);
            }
        }
        inline void readBatch(BatchedRegisters& batch)
        {
            std::lock_guard<std::mutex> lockBus(_mutexBus);
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Reset read flags for all registers
            for (size_t i=0;i<batch.regs.size();i++) {
                for (size_t j=0;j<batch.regs[i].size();j++) {
                    batch.regs[i][j]->readyForRead();
                }
            }
            if (batch.ids.size() == 1) {
                //Read single register
                TimePoint pStart = getTimePoint();
                ResponseState state = _protocol->readData(
                    batch.ids.front(),
                    batch.addr,
                    batch.regs.front().front()->_dataBufferRead,
                    batch.length);
                TimePoint pStop = getTimePoint();
                _stats.readCount++;
                _stats.readLength += batch.length;
                _stats.readDuration +=
                    getTimeDuration<TimeDurationMicro>(pStart, pStop);
                //Check for communication error
                if (!checkResponseState(state, _devicesById.at(batch.ids.front()))) {
                    //Error case
                    //Re-ask the value at next cycle
                    for (size_t j=0;j<batch.regs.front().size();j++) {
                        batch.regs.front()[j]->readError();
                    }
                } else {
                    //Valid case
                    //Retrieve the read timestamp
                    TimePoint timestamp = getTimePoint();
                    //Assign timestamp on Manager side and 
                    //mark for swapping
                    for (size_t j=0;j<batch.regs.front().size();j++) {
                        batch.regs.front()[j]->finishRead(timestamp);
                    }
                }
            } else {
                //Synch Read multiple registers
                std::vector<data_t*> datas;
                for (size_t i=0;i<batch.regs.size();i++) {
                    datas.push_back(batch.regs[i].front()->_dataBufferRead);
                }
                TimePoint pStart = getTimePoint();
                std::vector<ResponseState> states = _protocol->syncRead(
                    batch.ids,
                    batch.addr,
                    datas,
                    batch.length);
                TimePoint pStop = getTimePoint();
                _stats.syncReadCount++;
                _stats.syncReadLength += batch.length;
                _stats.syncReadDuration +=
                    getTimeDuration<TimeDurationMicro>(pStart, pStop);
                //Retrieve the read timestamp
                TimePoint timestamp = getTimePoint();
                for (size_t i=0;i<states.size();i++) {
                    //Check for communication error
                    if (!checkResponseState(states[i], _devicesById.at(batch.ids[i]))) {
                        //Error case
                        //Re-ask the value at next cycle
                        for (size_t j=0;j<batch.regs[i].size();j++) {
                            batch.regs[i][j]->readError();
                        }
                    } else {
                        //Valid case
                        //Assign timestamp on Manager side and 
                        //mark for swapping
                        for (size_t j=0;j<batch.regs[i].size();j++) {
                            batch.regs[i][j]->finishRead(timestamp);
                        }
                    }
                }
            }
        }

        /**
         * Iterate over all registers and
         * swap then to apply read change if
         * needed.
         * (No thread protection)
         */
        inline void swapRead()
        {
            for (size_t i=0;i<_sortedRegisters.size();i++) {
                _sortedRegisters[i]->swapRead();
            }
        }

        /**
         * Iterate over all Devices and
         * trigger Device onSwap() call back.
         * (No thread protection)
         */
        inline void swapCallBack()
        {
            for (auto& it : _devicesById) {
                it.second->onSwap();
            }
        }

        /**
         * Handle given response state.
         * Update statistics and Device status.
         * Device can be null.
         * Return false if the read operation
         * is a failure and the read value can
         * not be used.
         */
        inline bool checkResponseState(
            ResponseState state, Device* dev)
        {
            //Check if the device has answered
            bool isPresent = true;
            if (state & ResponseQuiet) {
                _stats.deviceQuietCount++;
                isPresent = false;
            }
            //Check for warning flags
            bool isWarning = false;
            if (state & ResponseOverload) {
                isWarning = true;
            }
            if (state & ResponseOverheat) {
                isWarning = true;
            }
            if (state & ResponseBadVoltage) {
                isWarning = true;
            }
            if (state & ResponseAlert) {
                isWarning = true;
            }
            //Check for error flags
            bool isError = false;
            if (state & ResponseBadChecksum) {
                isError = true;
            }
            if (state & ResponseDeviceBadInstruction) {
                isError = true;
            }
            if (state & ResponseDeviceBadChecksum) {
                isError = true;
            }
            if (state & ResponseBadSize) {
                isError = true;
            }
            if (state & ResponseBadProtocol) {
                isError = true;
            }
            if (state & ResponseBadId) {
                isError = true;
            }
            //Update statistics
            if (isWarning) {
                _stats.deviceWarningCount++;
            }
            if (isError) {
                _stats.deviceErrorCount++;
            }
            //Update Device state
            if (dev != nullptr) {
                dev->setPresent(isPresent);
                dev->setWarning(isWarning);
                dev->setError(isError);
            }

            if (state & ResponseOK) {
                _stats.deviceOKCount++;
                return true;
            } else {
                return false;
            }
        }

        /**
         * Try to read the Device model number
         * from given id and assign into given type.
         * True is returned if read is successful.
         * (The bus access is supposed to be locked)
         */
        inline bool retrieveTypeNumber(id_t id, type_t& type)
        {
            //Check for initBus() called
            if (_protocol == nullptr) {
                throw std::logic_error(
                    "BaseManager protocol not initialized");
            }
            //Read at static memory address
            data_t* pt = reinterpret_cast<data_t*>(&type);
            ResponseState state = _protocol
                ->readData(id, AddrDevTypeNumber, pt, 2);

            //Check response
            return checkResponseState(state, nullptr);
        }
};

}
