#include "Devices/DXL.hpp"

namespace RhAL {

void convEncode_baudrate(data_t* buffer, int value)
{
    int8_t result = (2000000 / value) - 1;
    if (result < 1) {
        //result must be signed for this 'if' to have a meaning
        result = 1;
    }
    write1ByteToBuffer(buffer, (uint8_t) result);
}
int convDecode_baudrate(const data_t* buffer)
{
    return 2000000 / (read1ByteFromBuffer(buffer) + 1);
}

void convEncode_returnDelayTime(data_t* buffer, int value)
{
    write1ByteToBuffer(buffer, (uint8_t) (value/2));
}
int convDecode_returnDelayTime(const data_t* buffer)
{
    return read1ByteFromBuffer(buffer) * 2;
}

void convEncode_temperature(data_t* buffer, unsigned int value)
{
    write1ByteToBuffer(buffer, (uint8_t) value);
}
unsigned int convDecode_temperature(const data_t* buffer)
{
    return read1ByteFromBuffer(buffer);
}

void convEncode_voltage(data_t* buffer, float value)
{
    //The max is in fact 15V or 16V depending on the motor.
    //Lazy solution...
    if (value > 16.0) {
        value = 16.0;
    } else if (value < 0) {
        value = 0.0;
    }
    uint8_t result = value*10;

    write1ByteToBuffer(buffer, result);
}
float convDecode_voltage(const data_t* buffer)
{
    return read1ByteFromBuffer(buffer)/10.0;
}

void convEncode_torque(data_t* buffer, float value)
{
    if(value<0.0)
        value=0.0;
    uint16_t result = value * 1023;
    if (result > 1023) {
        result = 1023;
    }
    write2BytesToBuffer(buffer, result);
}
float convDecode_torque(const data_t* buffer)
{
    return read2BytesFromBuffer(buffer)/1023.0;
}

DXL::DXL(const std::string& name, id_t id) :
    Device(name, id),
    /*
     * Registers that are common to all the dxl devices should be present here.
     * Unfortunately, the XL-320 has different addresses starting from the 'goalTorque' register.
     * This messes up with the genericity of dxl devices and disables the elegant solution.
     * Therefore, we decided to declare here only the Eeprom/flash registers that are common among all the dxl devices.
     * ReadOnly registers: ModelNumber, FirmwareVersion, PresentPosition,
     * PresentLoad, PresentVoltage, PresentTemperature, Registered, Moving.
     */
    //("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=true, forceWrite=false, isSlow=false)
    //Our current policy is as follows : if the register has an updateFreq of 0,
    //the it should be in forceRead mode. Otherwise, it really shouldn't be in forceRead mode.
    _modelNumber("modelNumber", 0x00, 2, convDecode_2Bytes, 0, true, false, true),
    _firmwareVersion("firmwareVersion", 0x02, 1, convDecode_1Byte, 0, true, false, true),
    _id("id", 0x03, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),
    _baudrate("baudrate", 0x04, 1, convEncode_baudrate, convDecode_baudrate, 0, true, false, true),
    _returnDelayTime("returnDelayTime", 0x05, 1, convEncode_returnDelayTime, convDecode_returnDelayTime, 0, true, false, true),
    _temperatureLimit("temperatureLimit", 0x0B, 1, convEncode_temperature, convDecode_temperature, 0, true, false, true),
    _voltageLowLimit("voltageLowLimit", 0x0C, 1, convEncode_voltage, convDecode_voltage, 0, true, false, true),
    _voltageHighLimit("voltageHighLimit", 0x0D, 1, convEncode_voltage, convDecode_voltage, 0, true, false, true),
    _maxTorque("maxTorque", 0x0E, 2, convEncode_torque, convDecode_torque, 0, true, false, true),
    _statusReturnLevel("statusReturnLevel", 0x10, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),
    _alarmShutdown("alarmShutdown", 0x12, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),
    //Parameters configuration
    _angleLimitCWParameter("angleLimitCWParameter", 0.0),
    _angleLimitCCWParameter("angleLimitCCWParameter", 0.0),
    _inverted("inverse", false),
    _zero("zero", 0.0),
    _isSmoothingActive(false)
{
    _temperatureLimit.setMinValue(0);
    _temperatureLimit.setMaxValue(255); //uint8 but you should not go to 255°!!
    _temperatureLimit.setStepValue(1);

    _voltageLowLimit.setMinValue(5.0);
    _voltageLowLimit.setMaxValue(16.0);
    _voltageLowLimit.setStepValue(0.1);

    _voltageHighLimit.setMinValue(5.0);
    _voltageHighLimit.setMaxValue(16.0);
    _voltageHighLimit.setStepValue(0.1);

    _maxTorque.setMinValue(0.0);
    _maxTorque.setMaxValue(1.0);
    _maxTorque.setStepValue(0.000977517); // 1.0/1023
}

TypedRegisterInt& DXL::firmwareVersion()
{
    return _firmwareVersion;
}
TypedRegisterInt& DXL::baudrate()
{
    return _baudrate;
}
TypedRegisterInt& DXL::returnDelayTime()
{
    return _returnDelayTime;
}
TypedRegisterInt& DXL::temperatureLimit()
{
    return _temperatureLimit;
}
TypedRegisterFloat& DXL::voltageLowLimit()
{
    return _voltageLowLimit;
}
TypedRegisterFloat& DXL::voltageHightLimit()
{
    return _voltageHighLimit;
}
TypedRegisterFloat& DXL::maxTorque()
{
    return _maxTorque;
}
TypedRegisterInt& DXL::statusReturnLevel()
{
    return _statusReturnLevel;
}
TypedRegisterInt& DXL::alarmShutdown()
{
    return _alarmShutdown;
}

void DXL::enableTorque()
{
    torqueEnable().writeValue(true);
}
void DXL::disableTorque()
{
    torqueEnable().writeValue(false);
}

void DXL::setGoalPositionSmooth(float angle, float delay)
{
    _isSmoothingActive = true;
    _smoothingStartGoal = position().readValue().value;
    _smoothingEndGoal = angle;
    _smoothingCurrentTime = 0.0;
    _smoothingEndTime = delay;
    goalPosition().writeValue(_smoothingStartGoal);
}
bool DXL::isSmoothingActive() const
{
    return _isSmoothingActive;
}

bool DXL::getInverted()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _inverted.value;
}
void DXL::setInverted(bool value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _inverted.value = value;
}
float DXL::getZero()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _zero.value;
}
void DXL::setZero(float value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _zero.value = value;
}

void DXL::onSwap()
{
    TimePoint tp = getTimePoint();
    t = duration_float(tp);
    float step = duration_float(_lastTp, tp);
    if (_lastTp == TimePoint()) {
        _lastTp = tp;
        return;
    }
    _lastTp = tp;

    if (_isSmoothingActive) {
        if(_smoothingEndTime==0.0) //FIXME: is it necessary?
            _smoothingEndTime=0.001;
        double goal =
                ((_smoothingEndGoal - _smoothingStartGoal)/_smoothingEndTime)
                * _smoothingCurrentTime
                + _smoothingStartGoal;
        goalPosition().writeValue(goal);
        _smoothingCurrentTime += step;
        if (_smoothingCurrentTime >= _smoothingEndTime) {
            _isSmoothingActive = false;
        }
    }
}

void DXL::onInit()
{
    Device::registersList().add(&_modelNumber);
    Device::registersList().add(&_firmwareVersion);
    Device::registersList().add(&_id);
    Device::registersList().add(&_baudrate);
    Device::registersList().add(&_returnDelayTime);
    Device::registersList().add(&_temperatureLimit);
    Device::registersList().add(&_voltageLowLimit);
    Device::registersList().add(&_voltageHighLimit);
    Device::registersList().add(&_maxTorque);
    Device::registersList().add(&_statusReturnLevel);
    Device::registersList().add(&_alarmShutdown);

    Device::parametersList().add(&_angleLimitCWParameter);
    Device::parametersList().add(&_angleLimitCCWParameter);
    Device::parametersList().add(&_inverted);
    Device::parametersList().add(&_zero);
}

}
