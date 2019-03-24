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
 * Conversion functions
 */

/**
 * Decode function used when the device outputs
 * values in 0.01 degrees. Output in degrees.
 */
float convDecode_100th_degrees(const data_t* buffer);

/**
 * Decode function used with the gyroscope that
 * outputs values in 0.02deg/s. Output in deg/s
 */
float convDecode_gyro(const data_t* buffer);

/**
 * Inertial Measurement Unit (IMU)
 */
class IMU : public Device
{
public:
  /**
   * Initialization with name and id
   */
  IMU(const std::string& name, id_t id);

  /**
   * Registers access
   */
  TypedRegisterFloat& yaw();
  TypedRegisterFloat& pitch();
  TypedRegisterFloat& roll();
  TypedRegisterInt& accX();
  TypedRegisterInt& accY();
  TypedRegisterInt& accZ();
  TypedRegisterFloat& gyroX();
  TypedRegisterFloat& gyroY();
  TypedRegisterFloat& gyroZ();
  TypedRegisterFloat& gyroYaw();
  TypedRegisterInt& magnX();
  TypedRegisterInt& magnY();
  TypedRegisterInt& magnZ();
  TypedRegisterFloat& magnAzimuth();

protected:
  /**
   * Register
   */
  // The following comments specify the register size and address in the hardware.
  TypedRegisterFloat _yaw;          // 2 0x24
  TypedRegisterFloat _pitch;        // 2 0x26
  TypedRegisterFloat _roll;         // 2 0x28
  TypedRegisterInt _accX;           // 2 0x2A
  TypedRegisterInt _accY;           // 2 0x2C
  TypedRegisterInt _accZ;           // 2 0x2E
  TypedRegisterFloat _gyroX;        // 2 0x30
  TypedRegisterFloat _gyroY;        // 2 0x32
  TypedRegisterFloat _gyroZ;        // 2 0x34
  TypedRegisterFloat _gyroYaw;      // 2 0x36
  TypedRegisterInt _magnX;          // 2 0x38
  TypedRegisterInt _magnY;          // 2 0x3A
  TypedRegisterInt _magnZ;          // 2 0x3C
  TypedRegisterFloat _magnAzimuth;  // 2 0x3E

  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;
};

/**
 * DeviceManager specialized for IMU
 */
template <>
class ImplManager<IMU> : public TypedManager<IMU>
{
public:
  inline static type_t typeNumber()
  {
    return 253;
  }

  inline static std::string typeName()
  {
    return "IMU";
  }
};

}  // namespace RhAL
