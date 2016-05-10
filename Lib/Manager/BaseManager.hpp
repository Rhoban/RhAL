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
#include <unordered_set>
#include <string>
#include <condition_variable>
#include <exception>
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
        BaseManager();

        /**
         * Bus and Protocol deallocation
         */
        ~BaseManager();

        /**
         * Return a Device with given id or name
         * (all derived types are searched).
         * Throw std::logic_error if asked Device
         * with given type is not found.
         */
        //Overloaded shortcuts
        const Device& dev(id_t id) const;
        Device& dev(id_t id);
        const Device& dev(const std::string& name) const;
        Device& dev(const std::string& name);
        //Implentations
        const Device& devById(id_t id) const;
        Device& devById(id_t id);
        const Device& devByName(const std::string& name) const;
        Device& devByName(const std::string& name);

        /**
         * Return true if a device is already contained with
         * given name or id for all Device types
         */
        //Overload shortcuts
        bool devExists(id_t id) const;
        bool devExists(const std::string& name) const;
        //Implementations
        bool devExistsById(id_t id) const;
        bool devExistsByName(const std::string& name) const;

        /**
         * Access to internal map of pointers
         * to all contained Devices for all types.
         * Device are indexed by their name.
         */
        const DevicesByName<Device>& devContainer() const;

        /**
         * Enable or disable the calling thread as 
         * cooperative thread. At the beginning of each
         * main flush() calls, all others
         * declared cooperative threads are waited
         * until all have called waitNextFlush() method.
         */
        void enableCooperativeThread();
        void disableCooperativeThread();

        /**
         * Immediately sends a broadcasted signal
         * to put all the devices in emergency mode
         */
        void emergencyStop();

        /**
         * Immediately sends a broadcasted signal
         * to exit the emergency state in all the
         * devices
         */
        void exitEmergencyState();

        /**
         * Declared cooperative user threads
         * have to call this method at each end
         * of their cycle to wait the next Manager
         * flush() operation.
         */
        void waitNextFlush();

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
         * - Perform all write operations.
         * - Perform all read operations.
         * - Optionnaly swap Registers (if isForceSwap is true)
         *   to apply immediatly read values.
         */
        void flush(bool isForceSwap = true);

        /**
         * Force all Registers to swap in order to
         * apply immediately read values
         */
        void forceSwap();

        /**
         * Ping a Device with given name or id. 
         * True is returned if the Device is present,
         * false else.
         */
        bool ping(const std::string& name);
        bool ping(id_t id);

        /**
         * Update the given Device id to
         * new given id.
         * Warning: due to internal structure
         * coheerence, the processus is stopped
         * (exit()) after the update.
         * No return.
         */
        void changeDeviceId(id_t oldId, id_t newId);

        /**
         * Ping all possible Device Id.
         * New discovered Devices are added.
         * All responding Devices are marked as present
         * and all others are marked as non present.
         * Throw std::logic_error is non supported
         * Device type or id/name error are found.
         */
        void scan();

        /**
         * Check if all Devices registered are
         * present on the bus.
         * True is returned if all known Devices are
         * anwsering. If at least one Device is missing,
         * false is returned.
         * (Device Present flag is set accordingly)
         */
        bool checkDevices();

        /**
         * Call setConfig on all registered Devices
         * that are present.
         */
        void setDevicesConfig();

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
        virtual void onNewRegister(
            id_t id, const std::string& name) override;

        /**
         * Inherit
         * Call the Manager to force the immediate
         * Read or Write of the Register given by its
         * Device id and name.
         * (Called by register, not by user)
         */
        virtual void forceRegisterRead(
            id_t id, const std::string& name) override;
        virtual void forceRegisterWrite(
            id_t id, const std::string& name) override;

        /**
         * Return by copy all Manager Statistics
         */
        Statistics getStatistics() const;

        /**
         * Reset all Manager Statistics
         */
        void resetStatistics();

        /**
         * Read/Write access to Manager Parameters list
         */
        const ParametersList& parametersList() const;
        ParametersList& parametersList();
        
        /**
         * Read/Write access to Protocol Parameters list
         */
        const ParametersList& protocolParametersList() const;
        ParametersList& protocolParametersList();

        /**
         * Set all Bus/Protocol configuration
         * with bus system path name, bus baudrate
         * and protocol name.
         */
        void setProtocolConfig(
            const std::string& port,
            unsigned long baudrate,
            const std::string& protocol);

        /**
         * Manager Parameters setters
         */
        void setEnableSyncRead(bool isEnable);
        void setEnableSyncWrite(bool isEnable);
        void setWaitWriteCheckResponse(bool isEnable);
        void setThrowOnScan(bool isEnable);
        void setThrowOnRead(bool isEnable);

        /**
         * The BaseManager has to call some
         * functions of AggregateManager
         */
        virtual type_t devTypeNumberById(id_t id) const = 0;
        virtual std::string devTypeNameById(id_t id) const = 0;
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
        void initBus();

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
         * The set of thread id  currently
         * declared as cooperative to be waited
         * in flush() by the manager.
         */
        std::unordered_set<std::thread::id> _cooperativeThread;

        /**
         * The number of user cooperative
         * and not cooperative threads currently 
         * waiting in waitNextFlush() methods 
         * for the first and the second barrier.
         */
        unsigned int _currentThreadWaiting1;
        unsigned int _currentThreadWaiting2;
        unsigned int _currentThreadCooperativeWaiting1;
        unsigned int _currentThreadCooperativeWaiting2;

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
         * Write check behaviour. If false, the Manager
         * assume that write protocol command does not
         * return a acknoledgement message and does wait
         * for it. If true, the Manager wait after each
         * write for a check response from devices.
         */
        ParameterBool _paramWaitWriteCheckResponse;

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
        bool isNeedRead(Register* reg);
        bool isNeedWrite(Register* reg);

        /**
         * Iterate over all registers and batch them
         * into compatible groups (address and length).
         * If isReadOrWrite is true, registers needing
         * read are selected.
         * If isReadOrWrite is false, registers needing
         * write are selected.
         */
        std::vector<BatchedRegisters> computeBatchedRegisters(
            bool isReadOrWrite);

        /**
         * Actually Write and Read on the bus
         * given batched Registers
         */
        void writeBatch(BatchedRegisters& batch);
        void readBatch(BatchedRegisters& batch);

        /**
         * Iterate over all registers and
         * swap then to apply read change if
         * needed.
         * (No thread protection)
         */
        void swapRead();

        /**
         * Iterate over all Devices and
         * trigger Device onSwap() call back.
         * (No thread protection)
         */
        void swapCallBack();

        /**
         * Handle given response state.
         * Update statistics and Device status.
         * Device can be null.
         * Return false if the read operation
         * is a failure and the read value can
         * not be used.
         */
        bool checkResponseState(
            ResponseState state, Device* dev);

        /**
         * Try to read the Device model number
         * from given id and assign into given type.
         * True is returned if read is successful.
         * (The bus access is supposed to be locked)
         */
        bool retrieveTypeNumber(id_t id, type_t& type);
};

}
