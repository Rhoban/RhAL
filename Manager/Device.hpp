#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include "types.h"
#include "Parameters.hpp"
#include "Register.hpp"

namespace RhAL {

/**
 * Device
 *
 * Base class for hardware devices.
 * Hold parameters and registers.
 */
class Device
{
    public:

        /**
         * Typedef for registers container
         */
        typedef std::unordered_map<std::string, Register*> 
            RegisterContainer;

        /**
         * Initialization with 
         * device name and id
         */
        inline Device(const std::string& name, id_t id) :
            _name(name),
            _id(id),
            _registers(),
            _params()
        {
        }

        /**
         * Free all allocated registers
         */
        inline virtual ~Device()
        {
            for (auto& reg : _registers) {
                delete reg.second;
                reg.second = nullptr;
            }
        }

        /**
         * Call Device derived implementation and
         * initialize all registers and parameters.
         * Throw std::logic_error if init() has already
         * been called.
         */
        inline void init()
        {
            if (
                _registers.size() != 0 ||
                _params.containerBool().size() != 0 ||
                _params.containerNumber().size() != 0 ||
                _params.containerStr().size() != 0
            ) {
                throw std::logic_error(
                    "Device device already init: " 
                    + _name);
            }
            initParameters();
            initRegisters();
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
         * Return the device model number
         * and textual name
         */
        virtual type_t typeNumber() const = 0;
        virtual std::string typeName() const = 0;

    protected:

        /**
         * Access to registers and 
         * parameters container
         */
        inline const RegisterContainer& registers() const
        {
            return _registers;
        }
        inline RegisterContainer& registers()
        {
            return _registers;
        }
        inline const ParametersContainer& parameters() const
        {
            return _params;
        }
        inline ParametersContainer& parameters()
        {
            return _params;
        }

        /**
         * Add given allocated register.
         * Desallocation is done by Device destructor.
         * std::logic_error is throw if given register name
         * already exists.
         */
        inline void addRegister(Register* reg)
        {
            if (_registers.count(reg->name) != 0) {
                throw std::logic_error(
                    "Device register already added: " 
                    + reg->name);
            }
            _registers[reg->name] = reg;
        }

        /**
         * Initialize and declare all parameters
         */
        virtual void initParameters() = 0;
        
        /**
         * Initialize and declare all registers
         */
        virtual void initRegisters() = 0;

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
         * Allocated register container
         */
        RegisterContainer _registers;

        /**
         * Container of bool, number and 
         * string device parameters
         */
        ParametersContainer _params;
};

}

