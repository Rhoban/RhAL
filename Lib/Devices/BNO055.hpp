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
 * BNO055 Inertial Measurement Unit (IMU)
 */
class BNO055 : public Device
{
public:
  BNO055(const std::string& name, id_t id);

  /**
   * Filter matrix
   */
  Eigen::Matrix3d getMatrix();

  /**
   * Filter output
   */
  float getYaw();
  float getPitch();
  float getRoll();

  /**
   * A callback that is invoked after a filtering
   */
  std::function<void()> callback;

  /**
   * Sets the filter callback
   */
  void setCallback(std::function<void()> callback);

protected:
  /**
   *  Euler angles values
   */
  double yaw, pitch, roll;

  // Invert the orientation of the robot (if the IMU X axis is backward)
  std::shared_ptr<ParameterNumber> robotToImuX_x;
  std::shared_ptr<ParameterNumber> robotToImuX_y;
  std::shared_ptr<ParameterNumber> robotToImuX_z;
  std::shared_ptr<ParameterNumber> robotToImuY_x;
  std::shared_ptr<ParameterNumber> robotToImuY_y;
  std::shared_ptr<ParameterNumber> robotToImuY_z;
  std::shared_ptr<ParameterNumber> robotToImuZ_x;
  std::shared_ptr<ParameterNumber> robotToImuZ_y;
  std::shared_ptr<ParameterNumber> robotToImuZ_z;

  // the read timestamp (of the most recent)
  TimePoint timestamp;

  // was the last read an error?
  bool isError;

  /**
   * Values
   */
  std::shared_ptr<TypedRegisterFloat> quatW;
  std::shared_ptr<TypedRegisterFloat> quatX;
  std::shared_ptr<TypedRegisterFloat> quatY;
  std::shared_ptr<TypedRegisterFloat> quatZ;
  std::shared_ptr<TypedRegisterBool> gyroCalibrated;
  std::shared_ptr<TypedRegisterBool> accCalibrated;
  std::shared_ptr<TypedRegisterInt> samples;
  std::shared_ptr<TypedRegisterInt> inits;
  std::shared_ptr<TypedRegisterInt> imu_errors;
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
class ImplManager<BNO055> : public TypedManager<BNO055>
{
public:
  inline static type_t typeNumber()
  {
    return 8055;
  }

  inline static std::string typeName()
  {
    return "BNO055";
  }
};

}  // namespace RhAL
