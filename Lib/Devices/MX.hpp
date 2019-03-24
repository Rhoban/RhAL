#pragma once

#include <string>
#include <mutex>
#include <cmath>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"

namespace RhAL
{
/**
 * Encode function for position, input
 * in degrees [-180, 180[ (precision : 360/4096 degrees)
 * 180 and -180 are the exact same point
 */
void convEncode_PositionMx(data_t* buffer, float value);
/**
 * Decode function for position, output
 * in degrees [-180, 180[ (precision : 360/4096 degrees)
 */
float convDecode_PositionMx(const data_t* buffer);

/**
 * Encode function for speed, input
 * in degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
void convEncode_SpeedMx(data_t* buffer, float value);
/**
 * Decode function for speed, input in
 * degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
float convDecode_SpeedMx(const data_t* buffer);

/**
 * Encode function for acceleration, input in
 * degrees/s^2 [0, 2180] (precision : 8.583 Degree / sec^2)
 */
void convEncode_AccelerationMx(data_t* buffer, float value);
/**
 * Decode function for acceleration, input in
 * degrees/s^2 [0, 2180] (precision : 8.583 Degree / sec^2)
 */
float convDecode_AccelerationMx(const data_t* buffer);

/**
 * MX
 *
 * Robotis Dynamixel MX-XX implementation.
 * This class entirely covers the MX-12 and MX-28.
 * The MX-64 and MX-106 have 3 registers that are covered
 * in their specific classes.
 * The Dynaban versions have lots of other registers.
 */
class MX : public DXL
{
public:
  /**
   * Initialization with name and id
   */
  MX(const std::string& name, id_t id);

  /**
   * Applies the configuration to the motors.
   * Used to make sure the angle limits are what they should be.
   */
  virtual void setConfig() override;

  /**
   * Registers access
   */

  /**
   * Position angle limits.
   */
  virtual TypedRegisterFloat& angleLimitCW() override;
  virtual TypedRegisterFloat& angleLimitCCW() override;
  /**
   * Alarm led configuration.
   */
  virtual TypedRegisterInt& alarmLed() override;
  /**
   * Boolean if the torque is enabled.
   */
  virtual TypedRegisterBool& torqueEnable() override;
  /**
   * Boolean if the led is ON.
   */
  virtual TypedRegisterBool& led() override;
  /**
   * Goal position in degrees.
   */
  virtual TypedRegisterFloat& goalPosition() override;
  /**
   * Goal speed in degrees/s.
   */
  virtual TypedRegisterFloat& goalSpeed() override;
  /**
   * Maximum torque value in percentage of max.
   */
  virtual TypedRegisterFloat& torqueLimit() override;
  /**
   * Current position in degrees.
   * Read only.
   */
  virtual TypedRegisterFloat& position() override;
  /**
   * Current speed in degrees/s.
   */
  virtual TypedRegisterFloat& speed() override;
  /**
   * Current load in percentage of max.
   * Read only.
   */
  virtual TypedRegisterFloat& load() override;
  /**
   * Current voltage in Volts.
   * Read only.
   */
  virtual TypedRegisterFloat& voltage() override;
  /**
   * Current temperature in degrees Celcius.
   * Read only.
   */
  virtual TypedRegisterInt& temperature() override;
  /**
   * Boolean if the last write command is registered.
   * Read only.
   */
  virtual TypedRegisterBool& registered() override;
  /**
   * Boolean if the motor is i moving.
   * Read only.
   */
  virtual TypedRegisterBool& moving() override;
  /**
   * Boolean if the eeprom is locked.
   */
  virtual TypedRegisterBool& lockEeprom() override;
  /**
   * Control punch value.
   */
  virtual TypedRegisterFloat& punch() override;

  /**
   * Non inherited registers access
   */

  TypedRegisterInt& multiTurnOffset();
  TypedRegisterInt& resolutionDivider();
  TypedRegisterInt& PGain();
  TypedRegisterInt& IGain();
  TypedRegisterInt& DGain();
  TypedRegisterFloat& goalAcceleration();

  /**
   * Sets the angle limits to enable joint,
   * wheel mode or multiturn mode
   */
  virtual void setJointMode() override;
  virtual void setWheelMode() override;
  void setMultiTurnMode();

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
  // in the all of the DXL children.
  TypedRegisterFloat _angleLimitCW;     // 2 06
  TypedRegisterFloat _angleLimitCCW;    // 2 08
  TypedRegisterInt _alarmLed;           // 1 11
  TypedRegisterInt _multiTurnOffset;    // 2 14 *
  TypedRegisterInt _resolutionDivider;  // 1 16 *

  // Flash/RAM limit (this info has no impact on the way the
  // registers are handled)

  TypedRegisterBool _torqueEnable;       // 1 18
  TypedRegisterBool _led;                // 1 19
  TypedRegisterInt _DGain;               // 1 1A *
  TypedRegisterInt _IGain;               // 1 1B *
  TypedRegisterInt _PGain;               // 1 1C *
  TypedRegisterFloat _goalPosition;      // 2 1E
  TypedRegisterFloat _goalSpeed;         // 2 20
  TypedRegisterFloat _torqueLimit;       // 2 22
  TypedRegisterFloat _position;          // 2 24
  TypedRegisterFloat _speed;             // 2 26
  TypedRegisterFloat _load;              // 2 28
  TypedRegisterFloat _voltage;           // 1 2A
  TypedRegisterInt _temperature;         // 1 2B
  TypedRegisterBool _registered;         // 1 2C
  TypedRegisterBool _moving;             // 1 2E
  TypedRegisterBool _lockEeprom;         // 1 2F
  TypedRegisterFloat _punch;             // 2 30
  TypedRegisterFloat _goalAcceleration;  // 1 49 *
};

}  // namespace RhAL
