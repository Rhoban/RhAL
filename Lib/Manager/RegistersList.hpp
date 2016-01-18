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
         * Typedef for registers container
         */
        typedef std::unordered_map<std::string, Register*>
            ContainerRegisters;

        /**
         * Initialization with associated Device id
         */
        inline RegistersList(id_t id) :
            _id(id),
            _registers(),
            _manager(nullptr),
            _memorySpace{0}
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
         * Add a new Register pointer to the internal
         * container.
         * Throw std::logic_error if register name 
         * is already contained.
         */
        inline void add(Register* reg)
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
            //Check for non intersecting memory
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
            //Insert the pointer into the container
            _registers[reg->name] = reg;
            //Assign Register associated Device id,
            //the pointer to the manager instance
            //and the pointer to data buffer
            reg->init(_id, _manager, _memorySpace + reg->addr);
            //Declare the register to the Manager
            //for building the set of all Registers
            _manager->onNewRegister(reg->id, reg->name);
        }

        /**
         * Access to given register by its name.
         * Throw std::logic_error if asked name does not exists
         */
        inline const Register& get(const std::string& name) const
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registers.at(name));
        }
        inline Register& get(const std::string& name)
        {
            if (!exists(name)) {
                throw std::logic_error(
                    "RegistersList name does not exists: "
                    + name);
            }
            return *(_registers.at(name));
        }

        /**
         * Direct access to Register container
         */
        inline const ContainerRegisters& container() const
        {
            return _registers;
        }

    private:

        /**
         * Associated Device id and data buffer
         * pointer
         */
        id_t _id;
        data_t* _data;

        /**
         * Registers container indexed by their name
         */
        ContainerRegisters _registers;

        /**
         * Pointer to a base class Manager
         * that will be provided to Registers
         */
        CallManager* _manager;

        /**
         * Complete allocated Device memory 
         * space shared by all registers
         */
        data_t _memorySpace[AddrDevLen];
};

}

