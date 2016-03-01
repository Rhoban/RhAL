#pragma once

#include <string>
#include <mutex>
#include "types.h"
#include "ConvertionUtils.h"
#include "Parameter.hpp"
#include "ParametersList.hpp"
#include "Register.hpp"
#include "RegistersList.hpp"
#include "CallManager.hpp"

namespace RhAL {

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
        inline Device(const std::string& name, id_t id) :
            _mutex(),
            _registersList(id),
            _parametersList(),
            _name(name),
            _id(id),
            _manager(nullptr),
            _isPresent(false)
        {
            if (id < IdDevBegin || id > IdDevEnd) {
                throw std::logic_error(
                    "Device id is outside static range: " 
                    + name);
            }
        }

        /**
         * Virtual destructor
         */
        inline virtual ~Device()
        {
        }

        /**
         * Copy constructor and 
         * assignement are forbidden
         */
        Device(const Device&) = delete;
        Device& operator=(const Device&) = delete;

        /**
         * Set the Manager pointer
         */
        inline void setManager(CallManager* manager)
        {
            if (manager == nullptr) {
                throw std::logic_error(
                    "Device null manager pointer: "
                    + _name);
            }
            _manager = manager;
            _registersList.setManager(_manager);
        }

        /**
         * Run derived class registers and
         * parameters initialization
         */
        inline void init()
        {
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Device null manager pointer: "
                    + _name);
            }
            //Call 
            onInit();
        }

        /**
         * Return the device name
         */
        inline const std::string& name() const
        {
            return _name;
        }

        /**
         * Return the device id
         */
        inline id_t id() const
        {
            return _id;
        }

        /**
         * Trigger specific implementation 
         * to enforce specific Device configuration
         */
        inline virtual void setConfig()
        {
            //Empty default
        }

        /**
         * Return true if the device has 
         * been see and is supposed 
         * enabled currently on the bus
         */
        inline bool isPresent() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isPresent;
        }

        /**
         * If true, the last read operations
         * with the Device set at least one 
         * warning flags (overload, overheat,
         * badvoltage, alert).
         */
        inline bool isWarning() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isWarning;
        }

        /**
         * If true, the last read operation
         * with the Device set at leat one
         * non quiet error flags
         */
        inline bool isError() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isError;
        }

        /**
         * Read/Write access to Registers and
         * Parameters list
         */
        const RegistersList& registersList() const
        {
            return _registersList;
        }
        RegistersList& registersList()
        {
            return _registersList;
        }
        const ParametersList& parametersList() const
        {
            return _parametersList;
        }
        ParametersList& parametersList()
        {
            return _parametersList;
        }
        
    protected:

        /**
         * Mutex protecting Device state access
         */
        mutable std::mutex _mutex;

        /**
         * Set Device isPresent and warning/error status.
         * (Used for friend Manager access)
         */
        inline void setPresent(bool isPresent)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isPresent = isPresent;
        }
        inline void setWarning(bool isWarning)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isWarning = isWarning;
        }
        inline void setError(bool isError)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isError = isError;
        }

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
        virtual inline void onSwap()
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
         * If true, the last read operation
         * with the Device set at leat one
         * non quiet error flags
         */
        bool _isError;
};

}

