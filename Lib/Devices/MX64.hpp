#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/MX.hpp"

namespace RhAL
{
/**
 * Encode function for current, input in
 * A [-9.2115, 9.2115] (precision : 4.5 mA)
 */
void convEncode_Current(data_t* buffer, float value);
/**
 * Decode function for current, output
 * in A [-9.2115, 9.2115] (precision : 4.5 mA)
 */
float convDecode_Current(const data_t* buffer);

/**
 * Encode function for goal torque, input in
 * bullshits (precision 1/2048 bullshits).
 * Actually expects a value between -1 and 1
 */
void convEncode_GoalCurrent(data_t* buffer, float value);
/**
 * Decode function for current, output
 * in bullshit (precision 1/2048 bullshit).
 * Actually outputs a value between -1 and 1
 */
float convDecode_GoalCurrent(const data_t* buffer);

/**
 * MX64
 *
 * Dynamixel MX-64 Device
 * implementation
 */
class MX64 : public MX
{
public:
  /**
   * Initialization with name and id
   */
  MX64(const std::string& name, id_t id);

  /**
   * Registers access
   */
  TypedRegisterFloat& current();
  TypedRegisterBool& torqueControlModeEnable();
  TypedRegisterFloat& goalCurrent();

protected:
  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;

  /**
   * Register
   */
  // The following comments specify the register size and address
  // in the hardware. A '*' means that the register is not present
  // in the all of the MX children.
  TypedRegisterFloat _current;                 // 2 44
  TypedRegisterBool _torqueControlModeEnable;  // 1 46
  TypedRegisterFloat _goalCurrent;             // 2 47
};

/**
 * DeviceManager specialized for MX64
 */
template <>
class ImplManager<MX64> : public TypedManager<MX64>
{
public:
  inline static type_t typeNumber()
  {
    return 0x0136;
  }

  inline static std::string typeName()
  {
    return "MX64";
  }
};

}  // namespace RhAL
