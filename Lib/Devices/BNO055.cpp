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
  quatW = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatW", 0x24, 2, quaternionDecode, 1));
  quatX = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatX", 0x26, 2, quaternionDecode, 1));
  quatY = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatY", 0x28, 2, quaternionDecode, 1));
  quatZ = std::shared_ptr<TypedRegisterFloat>(new TypedRegisterFloat("quatZ", 0x2a, 2, quaternionDecode, 1));

  _invertOrientation = std::shared_ptr<ParameterBool>(new ParameterBool("invertOrientation", false));
  _invertOrientationX = std::shared_ptr<ParameterBool>(new ParameterBool("invertOrientationX", false));
  _invertOrientationY = std::shared_ptr<ParameterBool>(new ParameterBool("invertOrientationY", false));
}

void BNO055::onInit()
{
  Device::registersList().add(quatW.get());
  Device::registersList().add(quatX.get());
  Device::registersList().add(quatY.get());
  Device::registersList().add(quatZ.get());

  Device::parametersList().add(_invertOrientation.get());
  Device::parametersList().add(_invertOrientationX.get());
  Device::parametersList().add(_invertOrientationY.get());
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

Eigen::Matrix3d BNO055::getMatrix()
{
  std::lock_guard<std::mutex> lock(_mutex);
  // XXX: TODO
  return Eigen::Matrix3d::Identity();
}

void BNO055::onSwap()
{
  _mutex.lock();

  double qW = quatW.get()->readValue().value;
  double qX = quatX.get()->readValue().value;
  double qY = quatY.get()->readValue().value;
  double qZ = quatZ.get()->readValue().value;

  Eigen::Quaterniond quaternions(qW, qX, qY, qZ);

  // We obtain roll, pitch and yaw
  Eigen::Matrix3d rotation = quaternions.toRotationMatrix();

  pitch = -asin(rotation(2, 0));
  roll = atan2(rotation(2, 1), rotation(2, 2));
  yaw = atan2(rotation(1, 0), rotation(0, 0));

  // Note, IMU to world without yaw can be computed as follow:
  // Eigen::Matrix3d imuToWorld = Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitY()).toRotationMatrix() *
  //                       Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitX()).toRotationMatrix();

  _mutex.unlock();

  if (callback)
  {
    callback();
  }
}

}  // namespace RhAL
