#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"
#include <cmath>

namespace RhAL
{
/**
 * Encode function for position, input
 * in degrees [-150, 150[ (precision : 300/1024 degrees)
 */
void convEncode_PositionRx(data_t* buffer, float value);
/**
 * Decode function for position, output
 * in degrees [-150, 150[ (precision : 300/1024 degrees)
 */
float convDecode_PositionRx(const data_t* buffer);

/**
 * Encode function for speed, input in
 * degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
void convEncode_SpeedRx(data_t* buffer, float value);
/**
 * Decode function for speed, input in
 * degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
float convDecode_SpeedRx(const data_t* buffer);

/**
 * Decode function for the compliance margin.
 * Same precision and unit than position but in a byte range.
 * Input in degrees [0, 74.7] (precision : 300/1024 degrees)
 */
void convEncode_ComplianceMargin(data_t* buffer, float value);
/**
 * Encode function for the compliance margin. Output
 * in degrees [0, 74.7] (precision : 300/1024 degrees)
 */
float convDecode_ComplianceMargin(const data_t* buffer);

/**
 * Encode function for the compliance slope. Only 7 inputs
 * are possible (cf datasheet) : 1, 2, 3, 4, 5, 6, 7.
 * No unit is given.
 */
void convEncode_ComplianceSlope(data_t* buffer, int value);
/**
 * Encode function for the compliance slope. No unit is given.
 * Only 7 different outputs should be possible : 1, 2, 3, 4, 5, 6, 7.
 */
int convDecode_ComplianceSlope(const data_t* buffer);

/**
 * RX
 *
 * Robotis Dynamixel RX-XX implementation.
 */
class RX : public DXL
{
public:
  /**
   * Initialization with name and id
   */
  RX(const std::string& name, id_t id);

  /**
   * Applies the configuration to the motors.
   * Used to make sure the angle limits
   * are what they should be.
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
   * Control compliance margins.
   */
  TypedRegisterFloat& complianceMarginCW();
  TypedRegisterFloat& complianceMarginCCW();
  /**
   * Control compliance slopes.
   */
  TypedRegisterInt& complianceSlopeCW();
  TypedRegisterInt& complianceSlopeCCW();
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
   * Sets the angle limits to enable joint or wheel mode
   */
  virtual void setJointMode() override;
  virtual void setWheelMode() override;

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

  TypedRegisterFloat _angleLimitCW;   // 2 06
  TypedRegisterFloat _angleLimitCCW;  // 2 08
  TypedRegisterInt _alarmLed;         // 1 11

  // Flash/RAM limit (this info has no impact on the way the registers are handled)

  TypedRegisterBool _torqueEnable;          // 1 18
  TypedRegisterBool _led;                   // 1 19
  TypedRegisterFloat _complianceMarginCW;   // 1 1A *
  TypedRegisterFloat _complianceMarginCCW;  // 1 1B *
  TypedRegisterInt _complianceSlopeCW;      // 1 1C *
  TypedRegisterInt _complianceSlopeCCW;     // 1 1D *
  TypedRegisterFloat _goalPosition;         // 2 1E
  TypedRegisterFloat _goalSpeed;            // 2 20
  TypedRegisterFloat _torqueLimit;          // 2 22
  TypedRegisterFloat _position;             // 2 24
  TypedRegisterFloat _speed;                // 2 26
  TypedRegisterFloat _load;                 // 2 28
  TypedRegisterFloat _voltage;              // 1 2A
  TypedRegisterInt _temperature;            // 1 2B
  TypedRegisterBool _registered;            // 1 2C
  TypedRegisterBool _moving;                // 1 2E
  TypedRegisterBool _lockEeprom;            // 1 2F
  TypedRegisterFloat _punch;                // 2 30
};

}  // namespace RhAL
