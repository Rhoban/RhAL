#include "Devices/MX.hpp"

namespace RhAL {

void convEncode_PositionMx(data_t* buffer, float value)
{
    value = 2048.0 + value * 4096/360.0;
    uint16_t position = std::lround(value)%4096;
    write2BytesToBuffer(buffer, position);
}
float convDecode_PositionMx(const data_t* buffer)
{
    uint16_t val = read2BytesFromBuffer(buffer);
    float result = (val - 2048) * 360.0 / 4096.0;
    if (result >= -180 && result < 180) {
        //We're already in the desired portion
    } else {
        //Modulating to be in [-180, 180[
        result = fmod(result + 180.0, 360) - 180;
    }

    return result;
}
void convEncode_SpeedMx(data_t* buffer, float value)
{
    float maxSpeed = 702.42;
    float conversion = 0.68662;
    if (value > maxSpeed) {
        value = maxSpeed;
    } else if (value < -maxSpeed) {
        value = -maxSpeed;
    }
    if (value > 0) {
        value = value / conversion;
    } else {
        value = 1024 - (value / conversion);
    }

    uint16_t speed = std::lround(value)%2048;
    write2BytesToBuffer(buffer, speed);
}
float convDecode_SpeedMx(const data_t* buffer)
{
    float conversion = 0.68662;
    uint16_t val = read2BytesFromBuffer(buffer);
    if (val < 1024) {
        return val * conversion;
    } else {
        return -(val - 1024) * conversion;
    }
}
void convEncode_AccelerationMx(data_t* buffer, float value)
{
    float maxAccel = 2180;
    float conversion = 8.583;
    if (value > maxAccel) {
        value = maxAccel;
    } else if (value < 0) {
        value = 0;
    }
    value = value / conversion;

    uint8_t accel = std::lround(value)%256;
    write1ByteToBuffer(buffer, accel);
}
float convDecode_AccelerationMx(const data_t* buffer)
{
    float conversion = 8.583;
    uint8_t val = read1ByteFromBuffer(buffer);
    return val * conversion;
}

MX::MX(const std::string& name, id_t id) :
    DXL(name, id),
    //ReadOnly registers: ModelNumber, FirmwareVersion, PresentPosition, PresentLoad, 
    //PresentVoltage, PresentTemperature, Registered, Moving.
    //("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
    _angleLimitCW("angleLimitCW", 0x06, 2, convEncode_PositionMx, convDecode_PositionMx, 0, true, false, true),
    _angleLimitCCW("angleLimitCCW", 0x08, 2, convEncode_PositionMx, convDecode_PositionMx, 0, true, false, true),
    _alarmLed("alarmLed", 0x11, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),
    _multiTurnOffset("multiTurnOffset", 0x14, 2, convEncode_2Bytes, convDecode_2Bytes, 0, true, false, true),
    _resolutionDivider("resolutionDivider", 0x16, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),

    _torqueEnable("torqueEnable", 0x18, 1, convEncode_Bool, convDecode_Bool, 0, true),
    _led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0, true),
    _DGain("DGain", 0x1A, 1, convEncode_1Byte, convDecode_1Byte, 0, true),
    _IGain("IGain", 0x1B, 1, convEncode_1Byte, convDecode_1Byte, 0, true),
    _PGain("PGain", 0x1C, 1, convEncode_1Byte, convDecode_1Byte, 0, true),
    _goalPosition("goalPosition", 0x1E, 2, 
        [this](data_t* data, float value) {
            std::lock_guard<std::mutex> lock(this->_mutex);
            value = value + this->_zero.value;
            if (this->_inverted.value == true) {
                value = value * -1.0;
            }
            convEncode_PositionMx(data, value);
        }, [this](const data_t* data) -> float {
            std::lock_guard<std::mutex> lock(_mutex);
            float value = convDecode_PositionMx(data);
            if (this->_inverted.value == true) {
                value = value * -1.0;
            }
            value = value - this->_zero.value;
            return value;
        }, 0, true),
    _goalSpeed("goalSpeed", 0x20, 2, 
        [this](data_t* data, float value) {
            std::lock_guard<std::mutex> lock(this->_mutex);
            int direction = 1;
            if (this->_inverted.value == true) {
                direction = -1;
            }
            convEncode_SpeedMx(data, value * direction);
        }, [this](const data_t* data) -> float {
            std::lock_guard<std::mutex> lock(this->_mutex);
            float value = convDecode_SpeedMx(data);
            int direction = 1;
            if (this->_inverted.value == true) {
                direction = -1;
            }
            return value * direction;
        }, 0, true),
    _torqueLimit("torqueLimit", 0x22, 2, convEncode_torque, convDecode_torque, 0, true),
    _position("position", 0x24, 2, 
        [this](const data_t* data) -> float {
            std::lock_guard<std::mutex> lock(this->_mutex);
            float value = convDecode_PositionMx(data);
            if (this->_inverted.value == true) {
                value = value * -1.0;
            }
            value = value - this->_zero.value;
            return value;
        }, 1),
    _speed("speed", 0x26, 2, 
        [this](const data_t* data) -> float {
            std::lock_guard<std::mutex> lock(this->_mutex);
            float value = convDecode_SpeedMx(data);
            int direction = 1;
            if (this->_inverted.value == true) {
                direction = -1;
            }
            return value * direction;
        }, 0, true),
    _load("load", 0x28, 2,  convDecode_torque, 0, true),
    _voltage("voltage", 0x2A, 1,  convDecode_voltage, 100),
    _temperature("temperature", 0x2B, 1, convDecode_temperature, 100),
    _registered("registered", 0x2C, 1, convDecode_Bool, 0, true),
    _moving("moving", 0x2E, 1, convDecode_Bool, 0, true),
    _lockEeprom("lockEeprom", 0x2F, 1, convEncode_Bool, convDecode_Bool, 0, true),
    _punch("punch", 0x30, 2, convEncode_2Bytes, convDecode_2Bytes, 0, true),
    _goalAcceleration("goalAcceleration", 0x49, 1, 
        [this](data_t* data, float value) {
            std::lock_guard<std::mutex> lock(this->_mutex);
            int direction = 1;
            if (this->_inverted.value == true) {
                direction = -1;
            }
            convEncode_AccelerationMx(data, value * direction);
        }, [this](const data_t* data) -> float {
            std::lock_guard<std::mutex> lock(this->_mutex);
            float value = convDecode_AccelerationMx(data);
            int direction = 1;
            if (this->_inverted.value == true) {
                direction = -1;
            }
            return value * direction;
        }, 0, true)
{

    _angleLimitCW.setMinValue(-180.0);
    _angleLimitCW.setMaxValue(180.0-0.087890625);
    _angleLimitCW.setStepValue(0.087890625);

    _angleLimitCCW.setMinValue(-180.0);
    _angleLimitCCW.setMaxValue(180.0-0.087890625);
    _angleLimitCCW.setStepValue(0.087890625);

    _goalPosition.setMinValue(-180.0);
    _goalPosition.setMaxValue(180.0-0.087890625);
    _goalPosition.setStepValue(0.087890625);

    _goalSpeed.setMinValue(-702.42);
    _goalSpeed.setMaxValue(702.42);
    _goalSpeed.setStepValue(0.68662);

    _position.setMinValue(-180.0);
    _position.setMaxValue(180.0-0.087890625);
    _position.setStepValue(0.087890625);

    _speed.setMinValue(-702.42);
    _speed.setMaxValue(702.42);
    _speed.setStepValue(0.68662);

    _goalAcceleration.setMinValue(0.0);
    _goalAcceleration.setMaxValue(2800.0);
    _goalAcceleration.setStepValue(8.583);

    _punch.setMinValue(0);
    _punch.setMaxValue(1023);
    _punch.setStepValue(1);

    _torqueLimit.setMinValue(0.0);
    _torqueLimit.setMaxValue(1.0);
    _torqueLimit.setStepValue(0.000977517); // 1.0/1023
}

void MX::setConfig()
{
    float angleLimit1 = _angleLimitCW.readValue().value;
    float angleLimit2 = _angleLimitCCW.readValue().value;
    if (
            abs(angleLimit1 - _angleLimitCWParameter.value) > _angleLimitCW.getStepValue()/2.0 ||
            abs(angleLimit2 - _angleLimitCCWParameter.value) > _angleLimitCCW.getStepValue()/2.0
       ) {
        // Setting the angle limits to what they should be
        angleLimit1 = _angleLimitCWParameter.value;
        angleLimit2 = _angleLimitCCWParameter.value;
        _angleLimitCW.writeValue(angleLimit1);
        _angleLimitCCW.writeValue(angleLimit2);
    }
}

TypedRegisterFloat& MX::angleLimitCW()
{
    return _angleLimitCW;
}
TypedRegisterFloat& MX::angleLimitCCW()
{
    return _angleLimitCCW;
}
TypedRegisterInt& MX::alarmLed()
{
    return _alarmLed;
}
TypedRegisterBool& MX::torqueEnable()
{
    return _torqueEnable;
}
TypedRegisterBool& MX::led()
{
    return _led;
}
TypedRegisterFloat& MX::goalPosition()
{
    return _goalPosition;
}
TypedRegisterFloat& MX::goalSpeed()
{
    return _goalSpeed;
}
TypedRegisterFloat& MX::torqueLimit()
{
    return _torqueLimit;
}
TypedRegisterFloat& MX::position()
{
    return _position;
}
TypedRegisterFloat& MX::speed()
{
    return _speed;
}
TypedRegisterFloat& MX::load()
{
    return _load;
}
TypedRegisterFloat& MX::voltage()
{
    return _voltage;
}
TypedRegisterInt& MX::temperature()
{
    return _temperature;
}
TypedRegisterBool& MX::registered()
{
    return _registered;
}
TypedRegisterBool& MX::moving()
{
    return _moving;
}
TypedRegisterBool& MX::lockEeprom()
{
    return _lockEeprom;
}
TypedRegisterFloat& MX::punch()
{
    return _punch;
}

TypedRegisterInt& MX::multiTurnOffset()
{
    return _multiTurnOffset;
}
TypedRegisterInt& MX::resolutionDivider()
{
    return _resolutionDivider;
}
TypedRegisterInt& MX::PGain()
{
    return _PGain;
}
TypedRegisterInt& MX::IGain()
{
    return _IGain;
}
TypedRegisterInt& MX::DGain()
{
    return _DGain;
}
TypedRegisterFloat& MX::goalAcceleration()
{
    return _goalAcceleration;
}

void MX::setJointMode()
{
    float limit1 = -180;
    //Means "180 minus one step"
    float limit2 = 180 - 360.0/4096.0;
    _angleLimitCW.writeValue(limit1);
    _angleLimitCCW.writeValue(limit2);
}
void MX::setWheelMode()
{
    float limit1 = -180;
    float limit2 = -180;
    _angleLimitCW.writeValue(limit1);
    _angleLimitCCW.writeValue(limit2);
}
void MX::setMultiTurnMode()
{
    float limit1 = 180 - 360.0/4096.0;
    float limit2 = 180 - 360.0/4096.0;
    _angleLimitCW.writeValue(limit1);
    _angleLimitCCW.writeValue(limit2);
}

void MX::onInit()
{
    DXL::onInit();

    Device::registersList().add(&_angleLimitCW);
    Device::registersList().add(&_angleLimitCCW);
    Device::registersList().add(&_alarmLed);
    Device::registersList().add(&_multiTurnOffset);
    Device::registersList().add(&_resolutionDivider);
    Device::registersList().add(&_torqueEnable);
    Device::registersList().add(&_led);
    Device::registersList().add(&_DGain);
    Device::registersList().add(&_IGain);
    Device::registersList().add(&_PGain);
    Device::registersList().add(&_goalPosition);
    //Setting the aggregation method (sum for the goal position)
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
    Device::registersList().add(&_goalAcceleration);
}

}

