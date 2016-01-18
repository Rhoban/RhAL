#pragma once

#include <string>
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
            _registersList(id),
            _parametersList(),
            _name(name),
            _id(id),
            _manager(nullptr)
        {
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
         * Read access to Registers and
         * Parameters list
         */
        const RegistersList& registersList() const
        {
            return _registersList;
        }
        const ParametersList& parametersList() const
        {
            return _parametersList;
        }
        
    protected:
        
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
         * Read/Write access to Registers and
         * Parameters list
         * (Used for friend Manager access)
         */
        RegistersList& registersList()
        {
            return _registersList;
        }
        ParametersList& parametersList()
        {
            return _parametersList;
        }

        /**
         * Call during device initialization.
         * Registers and Parameters are supposed
         * to be declared here
         */
        virtual void onInit() = 0;

        /**
         * Manager has access to listed 
         * Parameters and Registers
         */
        template <typename ... T>
        friend class Manager;

    private:

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
};

}

