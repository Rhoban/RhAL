#include "RegistersList.hpp"
#include "CallManager.hpp"

namespace RhAL {

RegistersList::RegistersList(id_t id) :
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

void RegistersList::setManager(CallManager* manager)
{
    _manager = manager;
}
        
bool RegistersList::exists(const std::string& name) const
{
    return (_registers.count(name) != 0);
}

void RegistersList::add(TypedRegisterBool* reg)
{
    //Check for non intersecting memory
    checkMemorySpace(reg);
    //Insert the pointer into the container
    _registers[reg->name] = reg;
    _registersBool[reg->name] = reg;
    //Register initialization
    initRegister(reg);
}
void RegistersList::add(TypedRegisterInt* reg)
{
    //Check for non intersecting memory
    checkMemorySpace(reg);
    //Insert the pointer into the container
    _registers[reg->name] = reg;
    _registersInt[reg->name] = reg;
    //Register initialization
    initRegister(reg);
}
void RegistersList::add(TypedRegisterFloat* reg)
{
    //Check for non intersecting memory
    checkMemorySpace(reg);
    //Insert the pointer into the container
    _registers[reg->name] = reg;
    _registersFloat[reg->name] = reg;
    //Register initialization
    initRegister(reg);
}

const Register& RegistersList::reg(const std::string& name) const
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registers.at(name));
}
Register& RegistersList::reg(const std::string& name)
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registers.at(name));
}
const TypedRegisterBool& RegistersList::regBool(const std::string& name) const
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersBool.at(name));
}
TypedRegisterBool& RegistersList::regBool(const std::string& name)
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersBool.at(name));
}
const TypedRegisterInt& RegistersList::regInt(const std::string& name) const
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersInt.at(name));
}
TypedRegisterInt& RegistersList::regInt(const std::string& name)
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersInt.at(name));
}
const TypedRegisterFloat& RegistersList::regFloat(const std::string& name) const
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersFloat.at(name));
}
TypedRegisterFloat& RegistersList::regFloat(const std::string& name)
{
    if (!exists(name)) {
        throw std::logic_error(
            "RegistersList name does not exists: "
            + name);
    }
    return *(_registersFloat.at(name));
}

const RegistersList::ContainerRegisters& RegistersList::container() const
{
    return _registers;
}
const RegistersList::ContainerRegistersBool& RegistersList::containerBool() const
{
    return _registersBool;
}
const RegistersList::ContainerRegistersInt& RegistersList::containerInt() const
{
    return _registersInt;
}
const RegistersList::ContainerRegistersFloat& RegistersList::containerFloat() const
{
    return _registersFloat;
}

void RegistersList::checkMemorySpace(const Register* reg) const
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
                + reg->name + " and " + reg2->name);
        }
    }
}

void RegistersList::initRegister(Register* reg)
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

}

