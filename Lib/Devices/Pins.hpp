#pragma once

#include <string>
#include <mutex>
#include <cmath>
#include <type_traits>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL
{
/**
 * Pins
 *
 * This device read pins, it is basically
 * used to read buttons from a board
 */
class Pins : public Device
{
public:
  /**
   * Initialization with name and id
   */
  Pins(const std::string& name, id_t id);

  /**
   * Register access
   */
  TypedRegisterInt& pins();

  /**
   * Individual pin value access
   */
  bool getPin(unsigned int number);

  /**
   * Get all pings value in an array
   */
  void getPins(bool pins[7]);

protected:
  /**
   * Registers
   */
  // The following comments specify the register size and address in the hardware.
  TypedRegisterInt _pins;  // 1 0x24
  TypedRegisterFloat _voltage;

  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;
};

/**
 * DeviceManager specialized for Pins
 */
template <>
class ImplManager<Pins> : public TypedManager<Pins>
{
public:
  inline static type_t typeNumber()
  {
    return 6000;
  }

  inline static std::string typeName()
  {
    return "Pins";
  }
};

}  // namespace RhAL
