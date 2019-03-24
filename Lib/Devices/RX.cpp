#include "Devices/RX.hpp"

namespace RhAL
{
void convEncode_PositionRx(data_t* buffer, float value)
{
  // "150 minus one step"
  float maxValue = 150 - 300.0 / 1024.0;
  if (value > maxValue)
  {
    value = maxValue;
  }
  else if (value < -150)
  {
    value = -150;
  }
  value = 512.0 + value * 1024 / 300.0;

  uint16_t position = std::lround(value) % 1024;
  write2BytesToBuffer(buffer, position);
}
float convDecode_PositionRx(const data_t* buffer)
{
  uint16_t val = read2BytesFromBuffer(buffer);
  float result = (val - 512) * 300.0 / 1024.0;
  if (result >= -150 && result < 150)
  {
    // We're already in the desired portion
  }
  else
  {
    // Modulating to be in [-150, 150[
    result = fmod(result + 150.0, 300) - 150;
  }
  return result;
}

void convEncode_SpeedRx(data_t* buffer, float value)
{
  float maxSpeed = 702.42;
  float conversion = 0.68662;
  if (value > maxSpeed)
  {
    value = maxSpeed;
  }
  else if (value < -maxSpeed)
  {
    value = -maxSpeed;
  }
  if (value > 0)
  {
    value = value / conversion;
  }
  else
  {
    value = 1024 - (value / conversion);
  }

  uint16_t speed = std::lround(value) % 2048;
  write2BytesToBuffer(buffer, speed);
}
float convDecode_SpeedRx(const data_t* buffer)
{
  float conversion = 0.68662;
  uint16_t val = read2BytesFromBuffer(buffer);
  if (val < 1024)
  {
    return val * conversion;
  }
  else
  {
    return -(val - 1024) * conversion;
  }
}

void convEncode_ComplianceMargin(data_t* buffer, float value)
{
  float maxValue = 74.7;
  if (value > maxValue)
  {
    value = maxValue;
  }
  else if (value < 0)
  {
    value = 0.0;
  }
  value = value * 1024 / 300.0;

  uint8_t position = std::lround(value);
  write1ByteToBuffer(buffer, position);
}
float convDecode_ComplianceMargin(const data_t* buffer)
{
  uint8_t val = read1ByteFromBuffer(buffer);
  float result = val * 300.0 / 1024.0;

  return result;
}

void convEncode_ComplianceSlope(data_t* buffer, int value)
{
  if (value > 7)
  {
    value = 7;
  }
  else if (value < 1)
  {
    value = 1;
  }

  switch (value)
  {
    case 1:
      value = 2;
      break;
    case 2:
      value = 4;
      break;
    case 3:
      value = 8;
      break;
    case 4:
      value = 16;
      break;
    case 5:
      value = 32;
      break;
    case 6:
      value = 64;
      break;
    case 7:
      value = 128;
      break;
    default:
      value = 128;
  }

  uint8_t position = value;
  write1ByteToBuffer(buffer, position);
}

int convDecode_ComplianceSlope(const data_t* buffer)
{
  uint8_t val = read1ByteFromBuffer(buffer);
  int index = 0;
  while (index < 7)
  {
    // val is a power of 2, the slope is the (power+1).
    // If val == 00000010 then the slope is 2
    if ((val >> index) & 1)
    {
      return index;
    }
    index++;
  }

  return 7;
}

RX::RX(const std::string& name, id_t id)
  : DXL(name, id)
  ,
  // ReadOnly registers: ModelNumber, FirmwareVersion, PresentPosition,
  // PresentLoad, PresentVoltage, PresentTemperature, Registered, Moving.
  //("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
  _angleLimitCW("angleLimitCW", 0x06, 2, convEncode_PositionRx, convDecode_PositionRx, 0, true, false, true)
  , _angleLimitCCW("angleLimitCCW", 0x08, 2, convEncode_PositionRx, convDecode_PositionRx, 0, true, false, true)
  , _alarmLed("alarmLed", 0x11, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true)
  ,

  _torqueEnable("torqueEnable", 0x18, 1, convEncode_Bool, convDecode_Bool, 0, true)
  , _led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0, true)
  , _complianceMarginCW("complianceMarginCW", 0x1A, 1, convEncode_ComplianceMargin, convDecode_ComplianceMargin, 0,
                        true)
  , _complianceMarginCCW("complianceMarginCCW", 0x1B, 1, convEncode_ComplianceMargin, convDecode_ComplianceMargin, 0,
                         true)
  , _complianceSlopeCW("complianceSlopeCW", 0x1C, 1, convEncode_ComplianceSlope, convDecode_ComplianceSlope, 0, true)
  , _complianceSlopeCCW("complianceSlopeCCW", 0x1D, 1, convEncode_ComplianceSlope, convDecode_ComplianceSlope, 0, true)
  , _goalPosition("goalPosition", 0x1E, 2,
                  [this](data_t* data, float value) {
                    std::lock_guard<std::mutex> lock(this->_mutex);
                    value = value + this->_zero.value;
                    if (this->_inverted.value == true)
                    {
                      value = value * -1.0;
                    }
                    convEncode_PositionRx(data, value);
                  },
                  [this](const data_t* data) -> float {
                    float value = convDecode_PositionRx(data);
                    std::lock_guard<std::mutex> lock(this->_mutex);
                    if (this->_inverted.value == true)
                    {
                      value = value * -1.0;
                    }
                    value = value - this->_zero.value;
                    return value;
                  },
                  0, true)
  , _goalSpeed("goalSpeed", 0x20, 2,
               [this](data_t* data, float value) {
                 std::lock_guard<std::mutex> lock(this->_mutex);
                 int direction = 1;
                 if (this->_inverted.value == true)
                 {
                   direction = -1;
                 }
                 convEncode_SpeedRx(data, value * direction);
               },
               [this](const data_t* data) -> float {
                 std::lock_guard<std::mutex> lock(_mutex);
                 float value = convDecode_SpeedRx(data);
                 int direction = 1;
                 if (this->_inverted.value == true)
                 {
                   direction = -1;
                 }
                 return value * direction;
               },
               0, true)
  , _torqueLimit("torqueLimit", 0x22, 2, convEncode_torque, convDecode_torque, 0, true)
  , _position("position", 0x24, 2,
              [this](const data_t* data) -> float {
                std::lock_guard<std::mutex> lock(this->_mutex);
                float value = convDecode_PositionRx(data);
                if (this->_inverted.value == true)
                {
                  value = value * -1.0;
                }
                value = value - this->_zero.value;
                return value;
              },
              1, false)
  , _speed("speed", 0x26, 2,
           [this](const data_t* data) -> float {
             std::lock_guard<std::mutex> lock(this->_mutex);
             float value = convDecode_SpeedRx(data);
             int direction = 1;
             if (this->_inverted.value == true)
             {
               direction = -1;
             }
             return value * direction;
           },
           0, true)
  , _load("load", 0x28, 2, convDecode_torque, 0, true)
  , _voltage("voltage", 0x2A, 1, convDecode_voltage, 100)
  , _temperature("temperature", 0x2B, 1, convDecode_temperature, 100)
  , _registered("registered", 0x2C, 1, convDecode_Bool, 0, true)
  , _moving("moving", 0x2E, 1, convDecode_Bool, 0, true)
  , _lockEeprom("lockEeprom", 0x2F, 1, convEncode_Bool, convDecode_Bool, 0, true)
  , _punch("punch", 0x30, 2, convEncode_2Bytes, convDecode_2Bytes, 0, true)
{
  _angleLimitCW.setMinValue(-150.0);
  _angleLimitCW.setMaxValue(150.0 - 0.29296875);
  _angleLimitCW.setStepValue(0.29296875);  // 300/1024

  _angleLimitCCW.setMinValue(-150.0);
  _angleLimitCCW.setMaxValue(150.0 - 0.29296875);
  _angleLimitCCW.setStepValue(0.29296875);  // 300/1024

  _complianceMarginCW.setMinValue(0.0);
  _complianceMarginCW.setMaxValue(74.7);
  _complianceMarginCW.setStepValue(0.29296875);  // 300/1024

  _complianceMarginCCW.setMinValue(0.0);
  _complianceMarginCCW.setMaxValue(74.7);
  _complianceMarginCCW.setStepValue(0.29296875);  // 300/1024

  _complianceSlopeCW.setMinValue(0);
  _complianceSlopeCW.setMaxValue(7);
  _complianceSlopeCW.setStepValue(1);

  _complianceSlopeCCW.setMinValue(0);
  _complianceSlopeCCW.setMaxValue(7);
  _complianceSlopeCCW.setStepValue(1);

  _goalPosition.setMinValue(-150.0);
  _goalPosition.setMaxValue(150.0 - 0.29296875);
  _goalPosition.setStepValue(0.29296875);

  _goalSpeed.setMinValue(-702.42);
  _goalSpeed.setMaxValue(702.42);
  _goalSpeed.setStepValue(0.68662);

  _position.setMinValue(-150.0);
  _position.setMaxValue(150.0 - 0.29296875);
  _position.setStepValue(0.29296875);

  _speed.setMinValue(-702.42);
  _speed.setMaxValue(702.42);
  _speed.setStepValue(0.68662);

  _punch.setMinValue(32);
  _punch.setMaxValue(1023);
  _punch.setStepValue(1);

  _torqueLimit.setMinValue(0.0);
  _torqueLimit.setMaxValue(1.0);
  _torqueLimit.setStepValue(0.000977517);  // 1.0/1023
}

void RX::setConfig()
{
  float angleLimit1 = _angleLimitCW.readValue().value;
  float angleLimit2 = _angleLimitCCW.readValue().value;
  if (abs(angleLimit1 - _angleLimitCWParameter.value) > _angleLimitCW.getStepValue() / 2.0 ||
      abs(angleLimit2 - _angleLimitCCWParameter.value) > _angleLimitCCW.getStepValue() / 2.0)
  {
    // Setting the angle limits to what they should be
    angleLimit1 = _angleLimitCWParameter.value;
    angleLimit2 = _angleLimitCCWParameter.value;
    _angleLimitCW.writeValue(angleLimit1);
    _angleLimitCCW.writeValue(angleLimit2);
  }
}

TypedRegisterFloat& RX::angleLimitCW()
{
  return _angleLimitCW;
}
TypedRegisterFloat& RX::angleLimitCCW()
{
  return _angleLimitCCW;
}
TypedRegisterInt& RX::alarmLed()
{
  return _alarmLed;
}
TypedRegisterBool& RX::torqueEnable()
{
  return _torqueEnable;
}
TypedRegisterBool& RX::led()
{
  return _led;
}
TypedRegisterFloat& RX::complianceMarginCW()
{
  return _complianceMarginCW;
}
TypedRegisterFloat& RX::complianceMarginCCW()
{
  return _complianceMarginCCW;
}
TypedRegisterInt& RX::complianceSlopeCW()
{
  return _complianceSlopeCW;
}
TypedRegisterInt& RX::complianceSlopeCCW()
{
  return _complianceSlopeCCW;
}
TypedRegisterFloat& RX::goalPosition()
{
  return _goalPosition;
}
TypedRegisterFloat& RX::goalSpeed()
{
  return _goalSpeed;
}
TypedRegisterFloat& RX::torqueLimit()
{
  return _torqueLimit;
}
TypedRegisterFloat& RX::position()
{
  return _position;
}
TypedRegisterFloat& RX::speed()
{
  return _speed;
}
TypedRegisterFloat& RX::load()
{
  return _load;
}
TypedRegisterFloat& RX::voltage()
{
  return _voltage;
}
TypedRegisterInt& RX::temperature()
{
  return _temperature;
}
TypedRegisterBool& RX::registered()
{
  return _registered;
}
TypedRegisterBool& RX::moving()
{
  return _moving;
}
TypedRegisterBool& RX::lockEeprom()
{
  return _lockEeprom;
}
TypedRegisterFloat& RX::punch()
{
  return _punch;
}

void RX::setJointMode()
{
  float limit1 = -150;
  // Means "150 minus one step"
  float limit2 = 150 - 300.0 / 1024.0;
  _angleLimitCW.writeValue(limit1);
  _angleLimitCCW.writeValue(limit2);
}
void RX::setWheelMode()
{
  float limit1 = -150;
  float limit2 = -150;
  _angleLimitCW.writeValue(limit1);
  _angleLimitCCW.writeValue(limit2);
}

void RX::onInit()
{
  DXL::onInit();

  Device::registersList().add(&_angleLimitCW);
  Device::registersList().add(&_angleLimitCCW);
  Device::registersList().add(&_alarmLed);
  Device::registersList().add(&_torqueEnable);
  Device::registersList().add(&_led);
  Device::registersList().add(&_complianceMarginCW);
  Device::registersList().add(&_complianceMarginCCW);
  Device::registersList().add(&_complianceSlopeCW);
  Device::registersList().add(&_complianceSlopeCCW);
  Device::registersList().add(&_goalPosition);
  // Setting the aggregation method (sum for the goal position)
  _goalPosition.setAggregationPolicy(AggregateSum);
  Device::registersList().add(&_goalSpeed);
  Device::registersList().add(&_torqueLimit);
  Device::registersList().add(&_position);
  Device::registersList().add(&_speed);
  Device::registersList().add(&_load);
  Device::registersList().add(&_voltage);
  Device::registersList().add(&_temperature);
  Device::registersList().add(&_registered);
  Device::registersList().add(&_moving);
  Device::registersList().add(&_lockEeprom);
  Device::registersList().add(&_punch);
}

}  // namespace RhAL
