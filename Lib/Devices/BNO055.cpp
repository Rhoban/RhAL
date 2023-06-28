#include <iostream>
#include <unsupported/Eigen/EulerAngles>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/BNO055.hpp"

namespace RhAL
{
static float quaternionDecode(const data_t* data)
{
  return convDecode_2Bytes_signed(data) / 16384.0;
}

static float gyroDecode(const data_t* data)
{
  return convDecode_2Bytes_signed(data) * M_PI / (180.0 * 16.0);
}

BNO055::BNO055(const std::string& name, id_t id) : Device(name, id), callback([] {})
{
  R_world_robot.setIdentity();

  quatW = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatW", 0x20, 2, quaternionDecode, 1));
  quatX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatX", 0x22, 2, quaternionDecode, 1));
  quatY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatY", 0x24, 2, quaternionDecode, 1));
  quatZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatZ", 0x26, 2, quaternionDecode, 1));
  gyroCalibrated =
      std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt("gyroCalibrated", 0x28, 1, convDecode_1Byte, 1));
  accCalibrated =
      std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt("accCalibrated", 0x29, 1, convDecode_1Byte, 1));

  samples = std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt("samples", 0x2a, 1, convDecode_1Byte, 1));
  inits = std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt("inits", 0x2b, 1, convDecode_1Byte, 1));
  imu_errors = std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt("imu_errors", 0x2c, 1, convDecode_1Byte, 1));

  store_calibration = std::shared_ptr<TypedRegisterInt>(
      new TypedRegisterInt("store_calibration", 0x2d, 1, convEncode_1Byte, convDecode_1Byte, 1));

  gyroX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("gyroX", 0x2e, 2, gyroDecode, 1));
  gyroY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("gyroY", 0x30, 2, gyroDecode, 1));
  gyroZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("gyroZ", 0x32, 2, gyroDecode, 1));

  robotToImuX_x = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuX_x", 1.0));
  robotToImuX_y = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuX_y", 0.0));
  robotToImuX_z = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuX_z", 0.0));
  robotToImuY_x = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuY_x", 0.0));
  robotToImuY_y = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuY_y", 1.0));
  robotToImuY_z = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuY_z", 0.0));
  robotToImuZ_x = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuZ_x", 0.0));
  robotToImuZ_y = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuZ_y", 0.0));
  robotToImuZ_z = std::shared_ptr<ParameterNumber>(new ParameterNumber("robotToImuZ_z", 1.0));
}

void BNO055::onInit()
{
  Device::registersList().add(quatW.get());
  Device::registersList().add(quatX.get());
  Device::registersList().add(quatY.get());
  Device::registersList().add(quatZ.get());
  Device::registersList().add(gyroCalibrated.get());
  Device::registersList().add(accCalibrated.get());

  Device::registersList().add(samples.get());
  Device::registersList().add(inits.get());
  Device::registersList().add(imu_errors.get());
  Device::registersList().add(store_calibration.get());

  Device::registersList().add(gyroX.get());
  Device::registersList().add(gyroY.get());
  Device::registersList().add(gyroZ.get());

  Device::parametersList().add(robotToImuX_x.get());
  Device::parametersList().add(robotToImuX_y.get());
  Device::parametersList().add(robotToImuX_z.get());
  Device::parametersList().add(robotToImuY_x.get());
  Device::parametersList().add(robotToImuY_y.get());
  Device::parametersList().add(robotToImuY_z.get());
  Device::parametersList().add(robotToImuZ_x.get());
  Device::parametersList().add(robotToImuZ_y.get());
  Device::parametersList().add(robotToImuZ_z.get());
}

// Filters
void BNO055::setCallback(std::function<void()> callback_)
{
  callback = callback_;
}

float BNO055::getYaw()
{
  std::lock_guard<std::mutex> lock(_mutex);
  return yaw;
}
float BNO055::getPitch()
{
  std::lock_guard<std::mutex> lock(_mutex);
  return pitch;
}
float BNO055::getRoll()
{
  std::lock_guard<std::mutex> lock(_mutex);
  return roll;
}
Eigen::Vector3d BNO055::getGyro()
{
  std::lock_guard<std::mutex> lock(_mutex);
  return gyro;
}

bool BNO055::isGyroCalibrated()
{
  return gyroCalibrated.get()->readValue().value == 3;
}

bool BNO055::isAccCalibrated()
{
  return accCalibrated.get()->readValue().value == 3;
}

Eigen::Matrix3d BNO055::getMatrix()
{
  std::lock_guard<std::mutex> lock(_mutex);
  return R_world_robot;
}

void BNO055::onSwap()
{
  _mutex.lock();

  if (!quatW.get()->readValue().isError && !quatX.get()->readValue().isError && !quatY.get()->readValue().isError &&
      !quatZ.get()->readValue().isError)
  {
    double qW = quatW.get()->readValue().value;
    double qX = quatX.get()->readValue().value;
    double qY = quatY.get()->readValue().value;
    double qZ = quatZ.get()->readValue().value;

    Eigen::Quaterniond quaternions(qW, qX, qY, qZ);

    // We obtain roll, pitch and yaw
    Eigen::Matrix3d R_world_imu = quaternions.toRotationMatrix();

    // Robot to IMU matrix
    Eigen::Matrix3d R_imu_robot;
    R_imu_robot << robotToImuX_x->value, robotToImuY_x->value, robotToImuZ_x->value, robotToImuX_y->value,
        robotToImuY_y->value, robotToImuZ_y->value, robotToImuX_z->value, robotToImuY_z->value, robotToImuZ_z->value;

    R_world_robot = R_world_imu * R_imu_robot;

    // Sanity check for asin() call
    if (R_world_robot(2, 0) >= -1 && R_world_robot(2, 0) <= 1)
    {
      pitch = -asin(R_world_robot(2, 0));
      roll = atan2(R_world_robot(2, 1), R_world_robot(2, 2));
      yaw = atan2(R_world_robot(1, 0), R_world_robot(0, 0));
    }

    gyro.x() = gyroX.get()->readValue().value;
    gyro.y() = gyroY.get()->readValue().value;
    gyro.z() = gyroZ.get()->readValue().value;

    gyro = R_imu_robot * gyro;

    // Note, IMU to world without yaw can be computed as follow:
    // Eigen::Matrix3d imuToWorld = Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()).toRotationMatrix() *
    //                       Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitX()).toRotationMatrix();
  }

  _mutex.unlock();

  if (callback)
  {
    callback();
  }
}

}  // namespace RhAL
