#pragma once

#include <unordered_map>
#include <stdexcept>
#include "Register.hpp"
#include "CallManager.hpp"

namespace RhAL {

/**
 * RegistersList
 *
 * Container for Register pointers.
 * Pointer are neither allocated or
 * freed by the container.
 */
class RegistersList
{
    public:

        /**
         * Typedef for typed registers container
         */
        typedef std::unordered_map<std::string, Register*> 
            ContainerRegisters;
        typedef std::unordered_map<std::string, TypedRegisterBool*> 
            ContainerRegistersBool;
        typedef std::unordered_map<std::string, TypedRegisterInt*> 
            ContainerRegistersInt;
        typedef std::unordered_map<std::string, TypedRegisterFloat*> 
            ContainerRegistersFloat;

        /**
         * Initialization with associated Device id
         */
        inline RegistersList(id_t id) :
            _id(id),
            _registers(),
            _registersBool(),
            _registersInt(),
            _registersFloat(),
            _manager(nullptr),
            _memorySpaceRead{0},
            _memorySpaceWrite{0}
        {
        }

        /**
         * Set Manager instance pointer
         */
        inline void setManager(CallManager* manager)
        {
            _manager = manager;
        }

        /**
         * Return true if given register name 
         * is already contained
         */
        inline bool exists(const std::string& name) const
        {
            return (_registers.count(name) != 0);
        }

        /**
         * Add a new Typed Register pointer to 
         * the internal container.
         * No Thread protection.
         * Throw std::logic_error if register name 
         * is already contained.
         */
        inline void add(TypedRegisterBool* reg)
        {
            //Check for non intersecting memory
            checkMemorySpace(reg);
            //Insert the pointer into the container
            _registers[reg->name] = reg;
            _registersBool[reg->name] = reg;
            //Register initialization
            initRegister(reg);
        }
        inline void add(TypedRegisterInt* reg)
        {
            //Check for non intersecting memory
            checkMemorySpace(reg);
            //Insert the pointer into the container
            _registers[reg->name] = reg;
            _registersInt[reg->name] = reg;
            //Register initialization
            initRegister(reg);
        }
        inline void add(TypedRegisterFloat* reg)
        {
            //Check for non intersecting memory
            checkMemorySpace(reg);
            //Insert the pointer into the container
            _registers[reg->name] = reg;
            _registersFloat[reg->name] = reg;
            //Register initialization
            initRegister(reg);
        }

        /**
         * Access to given untuyped and Typed register by its name.
         * Throw std::logic_error if asked name does not exists
         */
        //Register
        inline const Register& reg(const std::string& name) const
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registers.at(name));
        }
        inline Register& reg(const std::string& name)
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registers.at(name));
        }
        //TypedRegisterBool
        inline const TypedRegisterBool& regBool(const std::string& name) const
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersBool.at(name));
        }
        inline TypedRegisterBool& regBool(const std::string& name)
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersBool.at(name));
        }
        //TypedRegisterInt
        inline const TypedRegisterInt& regInt(const std::string& name) const
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersInt.at(name));
        }
        inline TypedRegisterInt& regInt(const std::string& name)
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersInt.at(name));
        }
        //TypedRegisterFloat
        inline const TypedRegisterFloat& regFloat(const std::string& name) const
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersFloat.at(name));
        }
        inline TypedRegisterFloat& regFloat(const std::string& name)
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registersFloat.at(name));
        }

        /**
         * Direct access to Typed and untyped Register container
         */
        inline const ContainerRegisters& container() const
        {
            return _registers;
        }
        inline const ContainerRegistersBool& containerBool() const
        {
            return _registersBool;
        }
        inline const ContainerRegistersInt& containerInt() const
        {
            return _registersInt;
        }
        inline const ContainerRegistersFloat& containerFloat() const
        {
            return _registersFloat;
        }

    private:

        /**
         * Associated Device id 
         */
        id_t _id;

        /**
         * Typed and untyped Registers container 
         * indexed by their name
         */
        ContainerRegisters _registers;
        ContainerRegistersBool _registersBool;
        ContainerRegistersInt _registersInt;
        ContainerRegistersFloat _registersFloat;

        /**
         * Pointer to a base class Manager
         * that will be provided to Registers
         */
        CallManager* _manager;

        /**
         * Complete allocated Device memory 
         * space for read and write shared 
         * by all contained registers
         */
        data_t _memorySpaceRead[AddrDevLen];
        data_t _memorySpaceWrite[AddrDevLen];

        /**
         * Throw std::logic_error if given Register pointer
         * address/len is intersecting another contained
         * Register memory space
         * Check also for register exists and pointer validity.
         */
        inline void checkMemorySpace(const Register* reg) const
        {
            if (reg == nullptr) {
                throw std::logic_error(
                    "RegistersList null pointer");
            }
            if (exists(reg->name)) {
                throw std::logic_error(
                    "RegistersList name already added: "
                    + reg->name);
            }
            if (_manager == nullptr) {
                throw std::logic_error(
                    "RegistersList manager pointer not initialized");
            }
            for (const auto& it : _registers) {
                const Register* reg2 = it.second;
                if (
                    (reg->addr > reg2->addr &&
                    reg->addr < reg2->addr+reg2->length) ||
                    (reg->addr+reg->length > reg2->addr &&
                    reg->addr+reg->length < reg2->addr+reg2->length) 
                ) {
                    throw std::logic_error(
                        "RegistersList register memory intersection: "
                        + reg->name);
                }
            }
        }

        /**
         * Initialize new added given register pointer
         */
        inline void initRegister(Register* reg)
        {
            //Assign Register associated Device id,
            //the pointer to the manager instance
            //and the pointer to data buffer read and write
            reg->init(
                _id, _manager, 
                _memorySpaceRead + reg->addr,
                _memorySpaceWrite + reg->addr);
            //Declare the register to the Manager
            //for building the set of all Registers
            _manager->onNewRegister(reg->id, reg->name);
        }
};

}

