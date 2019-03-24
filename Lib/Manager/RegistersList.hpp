#pragma once

#include <map>
#include <stdexcept>
#include "Register.hpp"

namespace RhAL
{
// Forward declaration
class CallManager;

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
  typedef std::map<std::string, Register*> ContainerRegisters;
  typedef std::map<std::string, TypedRegisterBool*> ContainerRegistersBool;
  typedef std::map<std::string, TypedRegisterInt*> ContainerRegistersInt;
  typedef std::map<std::string, TypedRegisterFloat*> ContainerRegistersFloat;

  /**
   * Initialization with associated Device id
   */
  RegistersList(id_t id);

  /**
   * Set Manager instance pointer
   */
  void setManager(CallManager* manager);

  /**
   * Return true if given register name
   * is already contained
   */
  bool exists(const std::string& name) const;

  /**
   * Add a new Typed Register pointer to
   * the internal container.
   * No Thread protection.
   * Throw std::logic_error if register name
   * is already contained.
   */
  void add(TypedRegisterBool* reg);
  void add(TypedRegisterInt* reg);
  void add(TypedRegisterFloat* reg);

  /**
   * Access to given untuyped and Typed register by its name.
   * Throw std::logic_error if asked name does not exists
   */
  // Register
  const Register& reg(const std::string& name) const;
  Register& reg(const std::string& name);
  // TypedRegisterBool
  const TypedRegisterBool& regBool(const std::string& name) const;
  TypedRegisterBool& regBool(const std::string& name);
  // TypedRegisterInt
  const TypedRegisterInt& regInt(const std::string& name) const;
  TypedRegisterInt& regInt(const std::string& name);
  // TypedRegisterFloat
  const TypedRegisterFloat& regFloat(const std::string& name) const;
  TypedRegisterFloat& regFloat(const std::string& name);

  /**
   * Direct access to Typed and untyped Register container
   */
  const ContainerRegisters& container() const;
  const ContainerRegistersBool& containerBool() const;
  const ContainerRegistersInt& containerInt() const;
  const ContainerRegistersFloat& containerFloat() const;

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
  void checkMemorySpace(const Register* reg) const;

  /**
   * Initialize new added given register pointer
   */
  void initRegister(Register* reg);
};

}  // namespace RhAL
