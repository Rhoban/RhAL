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

BNO055::BNO055(const std::string& name, id_t id) : Device(name, id), callback([] {})
{
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
  // XXX: TODO
  return Eigen::Matrix3d::Identity();
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
    Eigen::Matrix3d rotation = quaternions.toRotationMatrix();

    // Robot to IMU matrix
    Eigen::Matrix3d robotToImu;
    robotToImu << robotToImuX_x->value, robotToImuY_x->value, robotToImuZ_x->value, robotToImuX_y->value,
        robotToImuY_y->value, robotToImuZ_y->value, robotToImuX_z->value, robotToImuY_z->value, robotToImuZ_z->value;
    rotation = rotation * robotToImu;

    // Sanity check for asin() call
    if (rotation(2, 0) >= -1 && rotation(2, 0) <= 1)
    {
      pitch = -asin(rotation(2, 0));
      roll = atan2(rotation(2, 1), rotation(2, 2));
      yaw = atan2(rotation(1, 0), rotation(0, 0));
    }

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
