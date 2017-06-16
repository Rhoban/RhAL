#pragma once

#include <string>
#include <mutex>
#include "types.h"
#include "timestamp.h"
#include "ConvertionUtils.h"
#include "ParametersList.hpp"
#include "RegistersList.hpp"
#include "Protocol/Protocol.hpp"

namespace RhAL {

//Forward declaration
class CallManager;

/**
 * Device
 *
 * Base class for hardware devices.
 * Hold parameters and registers pointer
 * contained in derived class.
 *
 * All parameters and registers are expected to
 * be declared in the constructor of the
 * derived Device class.
 */
class Device
{
    public:

        /**
         * Initialization with 
         * device name and id
         */
        Device(const std::string& name, id_t id);

        /**
         * Virtual destructor
         */
        virtual ~Device();

        /**
         * Copy constructor and 
         * assignement are forbidden
         */
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        /**
         * Set the Manager pointer
         */
        void setManager(CallManager* manager);

        /**
         * Run derived class registers and
         * parameters initialization
         */
        void init();

        /**
         * Return the device name
         */
        const std::string& name() const;

        /**
         * Return the device id
         */
        id_t id() const;

        /**
         * Trigger specific implementation 
         * to enforce specific Device configuration
         */
        virtual inline void setConfig()
        {
            //Empty default
        }

        /**
         * Return true if the device has 
         * been see and is supposed 
         * enabled currently on the bus
         */
        bool isPresent() const;

        /**
         * If true, the last read operations
         * with the Device set at least one 
         * warning flags (overload, overheat,
         * badvoltage, alert).
         */
        bool isWarning() const;

        /**
         * Return last warning flags (0 if no warning)
         */
        ResponseState lastFlags() const;

        /**
         * If true, the last read operation
         * with the Device set at leat one
         * non quiet error flags
         */
        bool isError() const;
        
        /**
         * Read the dontRead parameter
         */
        bool dontRead();

        /**
         * Return the number of 
         * warnings, errors ans missing 
         * responses
         */
        unsigned long countWarnings() const;
        unsigned long countErrors() const;
        unsigned long countMissings() const;

        /**
         * Read/Write access to Registers and
         * Parameters list
         */
        const RegistersList& registersList() const;
        RegistersList& registersList();
        const ParametersList& parametersList() const;
        ParametersList& parametersList();
        
    protected:

        /**
         * Mutex protecting Device state access
         */
        mutable std::mutex _mutex;

        /**
         * Set Device isPresent and warning/error status.
         * (Used for friend Manager access)
         */
        void setPresent(bool isPresent);
        void setWarning(bool isWarning);
        void setError(bool isError);
        
        /**
         * And set last warning and error flags.
         */
        void setFlags(ResponseState state);

        /**
         * Call during device initialization.
         * Registers and Parameters are supposed
         * to be declared here
         */
        virtual void onInit() = 0;

        /**
         * Callback to be override
         * called at the begin of each
         * flush() after swapRead()
         */
        virtual void onSwap()
        {
            //Empty default
        }

        /**
         * Manager have access to listed 
         * Parameters and Registers
         */
        friend class BaseManager;

    private:
        
        /**
         * Register pointers container
         */
        RegistersList _registersList;

        /**
         * Container of bool, number and 
         * string device parameters
         */
        ParametersList _parametersList;
        
        /**
         * Device unique name
         */
        const std::string _name;

        /**
         * Device unique id on the bus
         */
        const id_t _id;

        /**
         * Pointer to the base class Manager
         * that will be provided to RegistersList
         */
        CallManager* _manager;

        /**
         * If true, the device has been see
         * and is supposed enabled currently
         * on the bus.
         */
        bool _isPresent;

        /**
         * If true, the last read operations
         * with the Device set at least one 
         * warning flags (overload, overheat,
         * badvoltage, alert).
         */
        bool _isWarning;

        /**
         * Last received warning and errors flags
         */
        ResponseState _lastFlags;

        /**
         * If true, the last read operation
         * with the Device set at leat one
         * non quiet error flags.
         */
        bool _isError;

        /**
         * Count the number of warnings, errors
         * and quiet response
         */
        unsigned long _countWarnings;
        unsigned long _countErrors;
        unsigned long _countMissings;

        /**
         * Parameter to avoid reading from this device
         */
        ParameterBool _dontRead;
};

}

