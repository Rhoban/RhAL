#include "Devices/IMU.hpp"

namespace RhAL
{
float convDecode_100th_degrees(const data_t* buffer)
{
  // Beware, casting an unsigned into float and
  // casting a signed into float is not the same thing...
  return (int16_t)read2BytesFromBuffer(buffer) / 100.0;
}

float convDecode_gyro(const data_t* buffer)
{
  return (int16_t)read2BytesFromBuffer(buffer) / 50.0;
}

IMU::IMU(const std::string& name, id_t id)
  : Device(name, id)
  ,
  // Read only registers
  //(name, address, size, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
  _yaw("yaw", 0x24, 2, convDecode_100th_degrees, 1)
  , _pitch("pitch", 0x26, 2, convDecode_100th_degrees, 1)
  , _roll("roll", 0x28, 2, convDecode_100th_degrees, 1)
  , _accX("accX", 0x2A, 2, convDecode_2Bytes, 1)
  , _accY("accY", 0x2C, 2, convDecode_2Bytes, 1)
  , _accZ("accZ", 0x2E, 2, convDecode_2Bytes, 1)
  , _gyroX("gyroX", 0x30, 2, convDecode_gyro, 1)
  , _gyroY("gyroY", 0x32, 2, convDecode_gyro, 1)
  , _gyroZ("gyroZ", 0x34, 2, convDecode_gyro, 1)
  , _gyroYaw("gyroYaw", 0x36, 2, convDecode_100th_degrees, 1)
  , _magnX("magnX", 0x38, 2, convDecode_2Bytes, 1)
  , _magnY("magnY", 0x3A, 2, convDecode_2Bytes, 1)
  , _magnZ("magnZ", 0x3C, 2, convDecode_2Bytes, 1)
  , _magnAzimuth("magnAzimuth", 0x3E, 2, convDecode_100th_degrees, 1)
{
}

TypedRegisterFloat& IMU::yaw()
{
  return _yaw;
}
TypedRegisterFloat& IMU::pitch()
{
  return _pitch;
}
TypedRegisterFloat& IMU::roll()
{
  return _roll;
}
TypedRegisterInt& IMU::accX()
{
  return _accX;
}
TypedRegisterInt& IMU::accY()
{
  return _accY;
}
TypedRegisterInt& IMU::accZ()
{
  return _accZ;
}
TypedRegisterFloat& IMU::gyroX()
{
  return _gyroX;
}
TypedRegisterFloat& IMU::gyroY()
{
  return _gyroY;
}
TypedRegisterFloat& IMU::gyroZ()
{
  return _gyroZ;
}
TypedRegisterFloat& IMU::gyroYaw()
{
  return _gyroYaw;
}
TypedRegisterInt& IMU::magnX()
{
  return _magnX;
}
TypedRegisterInt& IMU::magnY()
{
  return _magnY;
}
TypedRegisterInt& IMU::magnZ()
{
  return _magnZ;
}
TypedRegisterFloat& IMU::magnAzimuth()
{
  return _magnAzimuth;
}

void IMU::onInit()
{
  Device::registersList().add(&_yaw);
  Device::registersList().add(&_pitch);
  Device::registersList().add(&_roll);
  Device::registersList().add(&_accX);
  Device::registersList().add(&_accY);
  Device::registersList().add(&_accZ);
  Device::registersList().add(&_gyroX);
  Device::registersList().add(&_gyroY);
  Device::registersList().add(&_gyroZ);
  Device::registersList().add(&_gyroYaw);
  Device::registersList().add(&_magnX);
  Device::registersList().add(&_magnY);
  Device::registersList().add(&_magnZ);
  Device::registersList().add(&_magnAzimuth);
}

}  // namespace RhAL
