#include "Devices/MX64.hpp"

namespace RhAL {

void convEncode_Current(data_t* buffer, float value)
{
    float maxValue = 9.2115;
    if (value > maxValue) {
        value = maxValue;
    } else if (value < -maxValue) {
        value = -maxValue;
    }

    uint16_t current = std::lround(2048 + value*1000.0/4.5);
    write1ByteToBuffer(buffer, current);
}
float convDecode_Current(const data_t* buffer)
{
    uint16_t val = read2BytesFromBuffer(buffer);
    return val * 4.5 * (val - 2048.0);
}

void convEncode_GoalCurrent(data_t* buffer, float value)
{
    if (value > 1.0) {
        value = 1.0;
    } else if (value < -1.0) {
        value = -1.0;
    }

    if (value >= 0) {
        value = 2047 * value;
    } else {
        value = 2048 - 2047*value;
    }
    uint16_t result = std::lround(value);
    write2BytesToBuffer(buffer, result);
}
float convDecode_GoalCurrent(const data_t* buffer)
{
    uint16_t val = read2BytesFromBuffer(buffer);
    if (val < 2048) {
        return val/2047.0;
    } else {
        return -(val - 2048)/2047.0;
    }
}

MX64::MX64(const std::string& name, id_t id) :
    MX(name, id),
    //("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
    _current("current", 0x44, 2, convDecode_Current, 0, true, false),
    _torqueControlModeEnable("torqueControlModeEnable", 0x46, 1, convEncode_Bool, convDecode_Bool, 0, true, false),
    _goalCurrent("goalCurrent", 0x47, 2, convEncode_GoalCurrent, convDecode_GoalCurrent, 0, true, false)
{
    _goalCurrent.setMinValue(-1.0);
    _goalCurrent.setMaxValue(1.0);
    _goalCurrent.setStepValue(0.0001);

    _current.setMinValue(-9.2115);
    _current.setMaxValue(9.2115);
    _current.setStepValue(0.0045);
}

TypedRegisterFloat& MX64::current()
{
    return _current;
}
TypedRegisterBool& MX64::torqueControlModeEnable()
{
    return _torqueControlModeEnable;
}
TypedRegisterFloat& MX64::goalCurrent()
{
    return _goalCurrent;
}
        
void MX64::onInit()
{
    MX::onInit();
    Device::registersList().add(&_current);
    Device::registersList().add(&_torqueControlModeEnable);
    Device::registersList().add(&_goalCurrent);
}

}

