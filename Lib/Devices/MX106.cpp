#include "Devices/MX106.hpp"

namespace RhAL {

MX106::MX106(const std::string& name, id_t id) :
    MX(name, id),
    //("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
    _current("current", 0x44, 2, convEncode_Current, convDecode_Current, 0, true, false),
    _torqueControlModeEnable("torqueControlModeEnable", 0x46, 1, convEncode_Bool, convDecode_Bool, 0, true, false),
    _goalTorque("goalTorque", 0x47, 2, convEncode_GoalCurrent, convDecode_GoalCurrent, 0, true, false),
    _driveMode("driveMode", 0x0A, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true)
{
}

TypedRegisterFloat& MX106::current()
{
    return _current;
}
TypedRegisterBool& MX106::torqueControlModeEnable()
{
    return _torqueControlModeEnable;
}
TypedRegisterFloat& MX106::goalTorque()
{
    return _goalTorque;
}        
        
void MX106::onInit()
{
    MX::onInit();
    Device::registersList().add(&_current);
    Device::registersList().add(&_torqueControlModeEnable);
    Device::registersList().add(&_goalTorque);
}

}

