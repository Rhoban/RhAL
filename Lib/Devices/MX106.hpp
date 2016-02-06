#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/MX.hpp"

namespace RhAL {

/**
 * Encode function for current, input in A [-9.2115, 9.2115] (precision : 4.5 mA)
 */
inline void convEncode_Current(data_t* buffer, float value)
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
/**
 * Decode function for current, output in A [-9.2115, 9.2115] (precision : 4.5 mA)
 */
inline float convDecode_Current(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	return val * 4.5 * (val - 2048.0);
}

/**
 * Encode function for goal torque, input in bullshit (precision 1/2048 bullshit).
 * Actually expects a value between -1 and 1
 */
inline void convEncode_GoalTorque(data_t* buffer, float value)
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
/**
 * Decode function for current, output in bullshit (precision 1/2048 bullshit).
 * Actually outputs a value between -1 and 1
 */
inline float convDecode_GoalTorque(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	if (val < 2048) {
		return val/2047.0;
	} else {
		return -(val - 2048)/2047.0;
	}
}

/**
 * MX106
 *
 * Dynamixel MX-106 Device
 * implementation
 */
class MX106 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        inline MX106(const std::string& name, id_t id) :
            MX(name, id),
			_current("current", 0x44, 2, convEncode_Current, convDecode_Current, 0),
			_torqueControlModeEnable("torqueControlModeEnable", 0x46, 1, convEncode_Bool, convDecode_Bool, 0),
			_goalTorque("goalTorque", 0x47, 2, convEncode_GoalTorque, convDecode_GoalTorque, 0)
        {
        }
    protected :
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
        	MX::onInit();
        	Device::registersList().add(&_current);
        	Device::registersList().add(&_torqueControlModeEnable);
        	Device::registersList().add(&_goalTorque);
        }

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the MX children.
		TypedRegisterFloat 	_current;					//2 44
		TypedRegisterBool	_torqueControlModeEnable;	//1 46
		TypedRegisterFloat 	_goalTorque;				//2 47

};

/**
 * DeviceManager specialized for MX106
 */
template <>
class ImplManager<MX106> : public BaseManager<MX106>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0164;
        }

        inline static std::string typeName()
        {
            return "MX106";
        }
};

}

