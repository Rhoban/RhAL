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
 * Radian to Degree and
 * Degree to Radian conversion
 */
constexpr inline float Rad2Deg(float a)
{
  return a * 180.0 / M_PI;
}
constexpr inline float Deg2Rad(float a)
{
  return a * M_PI / 180.0;
}

/*
 * Conversion functions
 */

/**
 * Encode function for dxl baudrate, input in BPS
 */
void convEncode_baudrate(data_t* buffer, int value);

/**
 * Decode function for dxl baudrate, output in BPS
 */
int convDecode_baudrate(const data_t* buffer);

/**
 * Encode function for the return delay time, input in us
 */
void convEncode_returnDelayTime(data_t* buffer, int value);

/**
 * Decode function for dxl baudrate, output in us
 */
int convDecode_returnDelayTime(const data_t* buffer);

/**
 * Encode function for the temperature, input in degrees Celsius
 */
void convEncode_temperature(data_t* buffer, unsigned int value);

/**
 * Decode function for the temperature, output in degrees Celsius
 */
unsigned int convDecode_temperature(const data_t* buffer);

/**
 * Encode function for the voltage, input in V
 */
void convEncode_voltage(data_t* buffer, float value);

/**
 * Decode function for the voltage, output in V
 */
float convDecode_voltage(const data_t* buffer);

/**
 * Encode function for the torque, input in % of max
 */
void convEncode_torque(data_t* buffer, float value);

/**
 * Decode function for the torque, output in % of max
 */
float convDecode_torque(const data_t* buffer);

/**
 * DXL
 *
 * Robotis Dynamixel servo motor
 * abstract base class
 */
class DXL : public Device
{
public:
  /**
   * Initialization with name and id
   */
  DXL(const std::string& name, id_t id);

  /**
   * Registers access
   */

  /**
   * Firmware version number.
   * Read only.
   */
  TypedRegisterInt& firmwareVersion();
  /**
   * Baudrate in BSP.
   */
  TypedRegisterInt& baudrate();
  /**
   * Return delay time in micro seconds.
   */
  TypedRegisterInt& returnDelayTime();
  /**
   * Temperature limit in celcius degrees.
   */
  TypedRegisterInt& temperatureLimit();
  /**
   * Min and max voltage limit in volts.
   */
  TypedRegisterFloat& voltageLowLimit();
  TypedRegisterFloat& voltageHightLimit();
  /**
   * Torque limit in percentage of maximum.
   */
  TypedRegisterFloat& maxTorque();
  /**
   * Status return level.
   */
  TypedRegisterInt& statusReturnLevel();
  /**
   * Alarm shutdown.
   */
  TypedRegisterInt& alarmShutdown();

  /**
   * Position angle limits.
   */
  virtual TypedRegisterFloat& angleLimitCW() = 0;
  virtual TypedRegisterFloat& angleLimitCCW() = 0;
  /**
   * Alarm led configuration.
   */
  virtual TypedRegisterInt& alarmLed() = 0;
  /**
   * Boolean if the torque is enabled.
   */
  virtual TypedRegisterBool& torqueEnable() = 0;
  /**
   * Boolean if the led is ON.
   */
  virtual TypedRegisterBool& led() = 0;
  /**
   * Goal position in degrees.
   */
  virtual TypedRegisterFloat& goalPosition() = 0;
  /**
   * Goal speed in degrees/s.
   */
  virtual TypedRegisterFloat& goalSpeed() = 0;
  /**
   * Maximum torque value in percentage of max.
   */
  virtual TypedRegisterFloat& torqueLimit() = 0;
  /**
   * Current position in degrees.
   * Read only.
   */
  virtual TypedRegisterFloat& position() = 0;
  /**
   * Current speed in degrees/s.
   */
  virtual TypedRegisterFloat& speed() = 0;
  /**
   * Current load in percentage of max.
   * Read only.
   */
  virtual TypedRegisterFloat& load() = 0;
  /**
   * Current voltage in Volts.
   * Read only.
   */
  virtual TypedRegisterFloat& voltage() = 0;
  /**
   * Current temperature in degrees Celcius.
   * Read only.
   */
  virtual TypedRegisterInt& temperature() = 0;
  /**
   * Boolean if the last write command is registered.
   * Read only.
   */
  virtual TypedRegisterBool& registered() = 0;
  /**
   * Boolean if the motor is i moving.
   * Read only.
   */
  virtual TypedRegisterBool& moving() = 0;
  /**
   * Boolean if the eeprom is locked.
   */
  virtual TypedRegisterBool& lockEeprom() = 0;
  /**
   * Control punch value.
   */
  virtual TypedRegisterFloat& punch() = 0;

  /**
   * Sets the angle limits to enable joint or wheel mode
   */
  virtual void setJointMode() = 0;
  virtual void setWheelMode() = 0;

  /**
   * Shortcut for torque enable and disable
   */
  void enableTorque();
  void disableTorque();

  /**
   * Go to given goal position (degree or radian) smoothed
   * with given delay in seconds.
   */
  void setGoalPositionSmooth(float angle, float delay);
  /**
   * Return true if smoothing if currently enabled or
   * is finished
   */
  bool isSmoothingActive() const;

  /**
   * Parameters get/set
   */
  /**
   * Returns true if the servo's angular
   * convention is inverted, false otherwise
   */
  bool getInverted();
  /**
   * Sets (true) or resets (false) the inversion
   * of the angular convention
   */
  void setInverted(bool value);
  /**
   * Returns the angle that will be considered
   * as 0 when talking to the servo
   */
  float getZero();
  /**
   * Sets the angle that will be considered
   * as 0 when talking to the servo
   */
  void setZero(float value);

protected:
  /**
   * Registers
   */
  // The following comments specify the register size and address in the hardware.
  TypedRegisterInt _modelNumber;         // 2 00
  TypedRegisterInt _firmwareVersion;     // 1 02
  TypedRegisterInt _id;                  // 1 03
  TypedRegisterInt _baudrate;            // 1 04
  TypedRegisterInt _returnDelayTime;     // 1 05
  TypedRegisterInt _temperatureLimit;    // 1 0B
  TypedRegisterFloat _voltageLowLimit;   // 1 0C
  TypedRegisterFloat _voltageHighLimit;  // 1 0D
  TypedRegisterFloat _maxTorque;         // 2 0E
  TypedRegisterInt _statusReturnLevel;   // 1 10
  TypedRegisterInt _alarmShutdown;       // 1 12

  /**
   * Parameters
   */
  ParameterNumber _angleLimitCWParameter;
  ParameterNumber _angleLimitCCWParameter;
  ParameterBool _inverted;
  ParameterNumber _zero;

  /**
   * Time measure
   */
  TimePoint _lastTp;
  double t;

  /**
   * Goal position smoothing state
   */
  bool _isSmoothingActive;
  double _smoothingStartGoal;
  double _smoothingEndGoal;
  double _smoothingCurrentTime;
  double _smoothingEndTime;

  /**
   * Inherit.
   * Flush() callback.
   * Implement position smoothing.
   */
  virtual void onSwap() override;

  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;
};

}  // namespace RhAL

/*
 * Registers present in all DXLs. Beware though, some of them are present at different addresses depending of the
 * device, forcing their implementation below the DXL class. I'm looking at you xl320...
 */
//		int16_t modelNumber;
//		uint8_t firmwareVersion;
//		uint8_t id;
//		uint8_t baudrate;
//		uint8_t returnDelayTime;
//		float angleLimits[2];
//		float temperatureLimit;
//        float voltageLimits[2];
//        float maxTorque;
//        uint8_t statusReturnLevel;
//        uint8_t alarmLed;
//		uint8_t alarmShutdown
//        bool torqueEnable;
//        bool led;
//        float goalPosition;
//        float goalPositionRad;
//        float goalSpeed;
//        float goalSpeedRad;
//        float torqueLimit;
//        float torqueLimitNormalized;
//        float position;
//        float positionRad;
//        float speed;
//        float speedRad;
//        float load;
//        float loadNormalized;
//        float voltage;
//        float temperature;
//        bool registered;
//        bool moving;
//        bool lockEeprom;
//		float punch;
