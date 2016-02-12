#pragma once

#include <string>
#include <mutex>
#include "types.h"
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
            std::lock_guard<std::mutex> lock(_mutex);
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
         * Return true if the device has 
         * been see and is supposed 
         * enable currently on the bus
         */
        inline bool isPresent() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isPresent;
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
         * Set Device isPresent state.
         * (Used for friend Manager access)
         */
        inline void setPresent(bool isPresent)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isPresent = isPresent;
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
        template <typename ... T>
        friend class Manager;

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
        std::string _name;

        /**
         * Device unique id on the bus
         */
        id_t _id;

        /**
         * Pointer to the base class Manager
         * that will be provided to RegistersList
         */
        CallManager* _manager;

        /**
         * If true, the device has been see
         * and is supposed enable currently
         * on the bus
         */
        bool _isPresent;
};

}

