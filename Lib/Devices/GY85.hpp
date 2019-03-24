#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <cmath>
#include <type_traits>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "AHRS/Filter.hpp"
#include "types.h"
#include "timestamp.h"

namespace RhAL
{
/**
 * How many buffers there is in the IMU
 */
#define GY85_VALUES 5

/**
 * GY-85 Inertial Measurement Unit (IMU)
 */
class GY85 : public Device
{
public:
  /**
   * A value in the device
   */
  struct GY85Value
  {
    std::shared_ptr<TypedRegisterFloat> accX;
    std::shared_ptr<TypedRegisterFloat> accY;
    std::shared_ptr<TypedRegisterFloat> accZ;
    std::shared_ptr<TypedRegisterFloat> gyroX;
    std::shared_ptr<TypedRegisterFloat> gyroY;
    std::shared_ptr<TypedRegisterFloat> gyroZ;
    std::shared_ptr<TypedRegisterFloat> magnX;
    std::shared_ptr<TypedRegisterFloat> magnY;
    std::shared_ptr<TypedRegisterFloat> magnZ;
    std::shared_ptr<TypedRegisterInt> sequence;
  };

  /**
   * Initialization with name and id
   */
  GY85(const std::string& name, id_t id);

  /**
   * Sets the filter callback
   */
  void setCallback(std::function<void()> callback);

  /**
   * Getters for current values
   */
  float getAccX();
  float getAccY();
  float getAccZ();
  float getGyroX();
  float getGyroY();
  float getGyroZ();
  float getMagnX();
  float getMagnY();
  float getMagnZ();
  float getAccXRaw();
  float getAccYRaw();
  float getAccZRaw();
  float getGyroXRaw();
  float getGyroYRaw();
  float getGyroZRaw();
  float getMagnXRaw();
  float getMagnYRaw();
  float getMagnZRaw();

  /**
   * Filter matrix
   */
  Eigen::Matrix3d getMatrix();
  Eigen::Matrix3d getMatrixCompass();

  /**
   * Filter output
   */
  float getYaw();
  float getPitch();
  float getRoll();
  float getGyroYaw();
  float getYawCompass();
  float getPitchCompass();
  float getRollCompass();
  float getMagnAzimuth();
  float getMagnHeading();

  ReadValueFloat getYawValue();
  ReadValueFloat getPitchValue();
  ReadValueFloat getRollValue();
  ReadValueFloat getGyroYawValue();
  ReadValueFloat getYawCompassValue();
  ReadValueFloat getPitchCompassValue();
  ReadValueFloat getRollCompassValue();
  ReadValueFloat getMagnAzimuthValue();
  ReadValueFloat getMagnHeadingValue();

  double getMaxStdDev();

  /**
   * Calibration
   */
  void setGyroCalibration(float x, float y, float z);

protected:
  /**
   * The IMU filter
   */
  AHRS::Filter filter;
  AHRS::Filter compassFilter;

  /**
   * A callback that is invoked after a filtering
   */
  std::function<void()> callback;

  /**
   * Calibration
   */
  std::shared_ptr<ParameterNumber> _gyroXOffset, _gyroYOffset, _gyroZOffset;
  std::shared_ptr<ParameterNumber> _accXMin, _accXMax, _accYMin, _accYMax, _accZMin, _accZMax;
  std::shared_ptr<ParameterNumber> _magnXMin, _magnXMax, _magnYMin, _magnYMax, _magnZMin, _magnZMax;

  // Invert the orientation of the robot (if the IMU X axis is backward)
  std::shared_ptr<ParameterBool> _invertOrientation;
  std::shared_ptr<ParameterBool> _invertOrientationX;
  std::shared_ptr<ParameterBool> _invertOrientationY;

  // Filters gains
  std::shared_ptr<ParameterNumber> _kp_rollpitch;
  std::shared_ptr<ParameterNumber> _ki_rollpitch;

  // filter delay
  std::shared_ptr<ParameterNumber> _filterDelay;

  // Max std dev
  std::shared_ptr<ParameterNumber> _maxStdDev;

  /**
   * Last sensor values
   */
  uint32_t sequence;

  // Raw
  float accXRaw, accYRaw, accZRaw;
  float gyroXRaw, gyroYRaw, gyroZRaw;
  float magnXRaw, magnYRaw, magnZRaw;

  // Calibrated
  float accX, accY, accZ;
  float gyroX, gyroY, gyroZ;
  float magnX, magnY, magnZ;

  // the read timestamp (of the most recent)
  TimePoint timestamp;

  // was the last read an error?
  bool isError;

  /**
   * Register
   */
  struct GY85Value values[GY85_VALUES];

  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;

  /**
   * On swap
   */
  virtual void onSwap() override;
};

/**
 * DeviceManager specialized for IMU
 */
template <>
class ImplManager<GY85> : public TypedManager<GY85>
{
public:
  inline static type_t typeNumber()
  {
    return 350;
  }

  inline static std::string typeName()
  {
    return "GY85";
  }
};

}  // namespace RhAL
