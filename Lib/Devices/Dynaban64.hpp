#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
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
 * Encode function for goal torque, input in bullshits (precision 1/2048 bullshits).
 * Actually expects a value between -1 and 1
 */
inline void convEncode_GoalCurrent(data_t* buffer, float value)
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
inline float convDecode_GoalCurrent(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	if (val < 2048) {
		return val/2047.0;
	} else {
		return -(val - 2048)/2047.0;
	}
}

/**
 * Dynaban64
 *
 * Dynaban64 Device
 * implementation
 */
class Dynaban64 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        inline Dynaban64(const std::string& name, id_t id) :
            MX(name, id),
			_current("current", 0x44, 2, convEncode_Current, convDecode_Current, 0),
			_torqueControlModeEnable("torqueControlModeEnable", 0x46, 1, convEncode_Bool, convDecode_Bool, 0),
			_goalCurrent("goalTorque", 0x47, 2, convEncode_GoalCurrent, convDecode_GoalCurrent, 0),
			_trajPoly1Size("trajPoly1Size", 0x4A, 1, convEncode_, convDecode_, 0),
			_traj1a0("traj1a0", 0x4B, 4, convEncode_, convDecode_, 0),
			_traj1a1("traj1a1", 0x4F, 4, convEncode_, convDecode_, 0),
			_traj1a2("traj1a2", 0x53, 4, convEncode_, convDecode_, 0),
			_traj1a3("traj1a3", 0x57, 4, convEncode_, convDecode_, 0),
			_traj1a4("traj1a4", 0x5B, 4, convEncode_, convDecode_, 0),
			_torquePoly1Size("torquePoly1Size", 0x5F, 1, convEncode_, convDecode_, 0),
			_torque1a0("torque1a0", 0x60, 4, convEncode_, convDecode_, 0),
			_torque1a1("torque1a1", 0x64, 4, convEncode_, convDecode_, 0),
			_torque1a2("torque1a2", 0x68, 4, convEncode_, convDecode_, 0),
			_torque1a3("torque1a3", 0x6C, 4, convEncode_, convDecode_, 0),
			_torque1a4("torque1a4", 0x70, 4, convEncode_, convDecode_, 0),
			_duration1("duration1", 0x75, 2, convEncode_, convDecode_, 0),
			_trajPoly2Size("trajPoly2Size", 0x76, 1, convEncode_, convDecode_, 0),
			_traj2a0("traj2a0", 0x77, 4, convEncode_, convDecode_, 0),
			_traj2a1("traj2a1", 0x7B, 4, convEncode_, convDecode_, 0),
			_traj2a2("traj2a2", 0x7F, 4, convEncode_, convDecode_, 0),
			_traj2a3("traj2a3", 0x83, 4, convEncode_, convDecode_, 0),
			_traj2a4("traj2a4", 0x87, 4, convEncode_, convDecode_, 0),
			_torquePoly2Size("torquePoly2Size", 0x8B, 1, convEncode_, convDecode_, 0),
			_torque2a0("torque2a0", 0x8C, 4, convEncode_, convDecode_, 0),
			_torque2a1("torque2a1", 0x90, 4, convEncode_, convDecode_, 0),
			_torque2a2("torque2a2", 0x94, 4, convEncode_, convDecode_, 0),
			_torque2a3("torque2a3", 0x98, 4, convEncode_, convDecode_, 0),
			_torque2a4("torque2a4", 0x9C, 4, convEncode_, convDecode_, 0),
			_duration2("duration2", 0xA0, 2, convEncode_, convDecode_, 0),
			_mode("mode", 0xA2, 1, convEncode_, convDecode_, 0),
			_copyNextBuffer("copyNextBuffer", 0xA3, 1, convEncode_, convDecode_, 0),
			_positionTrackerOn("positionTrackerOn", 0xA4, 1, convEncode_, convDecode_, 0),
			_debugOn("debugOn", 0xA5, 1, convEncode_, convDecode_, 0),
			_unused("unused", 0xA6, 2, convEncode_, convDecode_, 0),
			_i0("i0", 0xA8, 4, convEncode_, convDecode_, 0),
			_r("r", 0xAC, 4, convEncode_, convDecode_, 0),
			_ke("ke", 0xB0, 4, convEncode_, convDecode_, 0),
			_kvis("kvis", 0xB4, 4, convEncode_, convDecode_, 0),
			_kstat("kstat", 0xB8, 4, convEncode_, convDecode_, 0),
			_kcoul("kcoul", 0xBC, 4, convEncode_, convDecode_, 0),
			_linearTransition("linearTransition", 0xC0, 4, convEncode_, convDecode_, 0),
			_speedCalculationDelay("speedCalculationDelay", 0xC4, 2, convEncode_, convDecode_, 0),
			_ouputTorque("ouputTorque", 0xC6, 4, convEncode_, convDecode_, 0),
			_electricalTorque("electricalTorque", 0xCA, 4, convEncode_, convDecode_, 0),
			_frozenRamOn("frozenRamOn", 0xCE, 1, convEncode_, convDecode_, 0),
			_useValuesNow("useValuesNow", 0xCF, 1, convEncode_, convDecode_, 0),
			_torqueKp("torqueKp", 0xD0, 1, convEncode_, convDecode_, 0),
			_goalTorque("goalTorque", 0xD2, 4, convEncode_, convDecode_, 0)
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
        	Device::registersList().add(&_goalCurrent);

        	Device::registersList().add(&_trajPoly1Size);
			Device::registersList().add(&_traj1a0);
			Device::registersList().add(&_traj1a1);
			Device::registersList().add(&_traj1a2);
			Device::registersList().add(&_traj1a3);
			Device::registersList().add(&_traj1a4);
			Device::registersList().add(&_torquePoly1Size);
			Device::registersList().add(&_torque1a0);
			Device::registersList().add(&_torque1a1);
			Device::registersList().add(&_torque1a2);
			Device::registersList().add(&_torque1a3);
			Device::registersList().add(&_torque1a4);
			Device::registersList().add(&_duration1);
			Device::registersList().add(&_trajPoly2Size);
			Device::registersList().add(&_traj2a0);
			Device::registersList().add(&_traj2a1);
			Device::registersList().add(&_traj2a2);
			Device::registersList().add(&_traj2a3);
			Device::registersList().add(&_traj2a4);
			Device::registersList().add(&_torquePoly2Size);
			Device::registersList().add(&_torque2a0);
			Device::registersList().add(&_torque2a1);
			Device::registersList().add(&_torque2a2);
			Device::registersList().add(&_torque2a3);
			Device::registersList().add(&_torque2a4);
			Device::registersList().add(&_duration2);
			Device::registersList().add(&_mode);
			Device::registersList().add(&_copyNextBuffer);
			Device::registersList().add(&_positionTrackerOn);
			Device::registersList().add(&_debugOn);
			Device::registersList().add(&_unused);
			Device::registersList().add(&_i0);
			Device::registersList().add(&_r);
			Device::registersList().add(&_ke);
			Device::registersList().add(&_kvis);
			Device::registersList().add(&_kstat);
			Device::registersList().add(&_kcoul);
			Device::registersList().add(&_linearTransition);
			Device::registersList().add(&_speedCalculationDelay);
			Device::registersList().add(&_ouputTorque);
			Device::registersList().add(&_electricalTorque);
			Device::registersList().add(&_frozenRamOn);
			Device::registersList().add(&_useValuesNow);
			Device::registersList().add(&_torqueKp);
			Device::registersList().add(&_goalTorque);
        }

        /**
         * Register
         */
		TypedRegisterFloat 	_current;					//2 44
		TypedRegisterBool	_torqueControlModeEnable;	//1 46
		TypedRegisterFloat 	_goalCurrent;				//2 47

		// Dynaban specific registers :

		TypedRegisterInt	_trajPoly1Size;				//1 4A
		TypedRegisterFloat  _traj1a0;					//4 4B
		TypedRegisterFloat  _traj1a1;					//4 4F
		TypedRegisterFloat  _traj1a2;					//4 53
		TypedRegisterFloat  _traj1a3;					//4 57
		TypedRegisterFloat  _traj1a4;					//4 5B

		TypedRegisterInt	_torquePoly1Size;			//1 5F
		TypedRegisterFloat  _torque1a0;					//4 60
		TypedRegisterFloat  _torque1a1;					//4 64
		TypedRegisterFloat  _torque1a2;					//4 68
		TypedRegisterFloat  _torque1a3;					//4 6C
		TypedRegisterFloat  _torque1a4;					//4 70

		TypedRegisterInt	_duration1;					//2 75

		TypedRegisterInt	_trajPoly2Size;				//1 76
		TypedRegisterFloat  _traj2a0;					//4 77
		TypedRegisterFloat  _traj2a1;					//4 7B
		TypedRegisterFloat  _traj2a2;					//4 7F
		TypedRegisterFloat  _traj2a3;					//4 83
		TypedRegisterFloat  _traj2a4;					//4 87

		TypedRegisterInt	_torquePoly2Size;			//1 8B
		TypedRegisterFloat  _torque2a0;					//4 8C
		TypedRegisterFloat  _torque2a1;					//4 90
		TypedRegisterFloat  _torque2a2;					//4 94
		TypedRegisterFloat  _torque2a3;					//4 98
		TypedRegisterFloat  _torque2a4;					//4 9C

		TypedRegisterInt	_duration2;					//2 A0

		TypedRegisterInt	_mode;						//1 A2
		TypedRegisterInt	_copyNextBuffer;			//1 A3
		TypedRegisterBool	_positionTrackerOn;			//1 A4
		TypedRegisterBool	_debugOn;					//1 A5
		TypedRegisterInt	_unused;					//2 A6

		TypedRegisterFloat  _i0;						//4 A8
		TypedRegisterFloat  _r;							//4 AC
		TypedRegisterFloat  _ke;						//4 B0
		TypedRegisterFloat  _kvis;						//4 B4
		TypedRegisterFloat  _kstat;						//4 B8
		TypedRegisterFloat  _kcoul;						//4 BC
		TypedRegisterFloat  _linearTransition;			//4 C0
		TypedRegisterInt	_speedCalculationDelay;		//2 C4
		TypedRegisterFloat  _ouputTorque;				//4 C6
		TypedRegisterFloat  _electricalTorque;			//4 CA

		TypedRegisterInt	_frozenRamOn;				//1 CE
		TypedRegisterInt	_useValuesNow;				//1 CF
		TypedRegisterInt	_torqueKp;					//1 D0
		TypedRegisterFloat  _goalTorque;				//4 D2

};

/**
 * DeviceManager specialized for Dynaban64
 */
template <>
class ImplManager<Dynaban64> : public TypedManager<Dynaban64>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0136;
        }

        inline static std::string typeName()
        {
            return "Dynaban64";
        }
};

}

