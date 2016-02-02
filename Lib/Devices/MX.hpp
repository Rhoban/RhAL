#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"

namespace RhAL {

/**
 * MX
 *
 * Robotis Dynamixel MX-XX implementation.
 * This class entirely covers the MX-12 and MX-28.
 * The MX-64 and MX-106 have 3 registers that are covered in their specific classes.
 * The Dynaban versions have lots of other registers.
 */
class MX : public DXL
{
    public:
        /**
         * Initialization with name and id
         */
        inline MX(const std::string& name, id_t id) :
            DXL(name, id),
            _goalPos("goalPos", 0X1E, 2, convIn_MXPos, convOut_MXPos, 0),
            _position("pos", 0X24, 2, convIn_MXPos, convOut_MXPos, 1),
        	_torqueEnable("torqueEnable", 0X18, 1, convIn_Default<bool>, convOut_Default<bool>, 0)

			_modelNumber("modelNumber", 0x00, 2, in, out, 0),
			_firmwareVersion("firmwareVersion", 0x02, 1, in, out, 0),
			_id("id", 0x03, 1, in, out, 0),
			_baudrate("baudrate", 0x04, 1, in, out, 0),
			_returnDelayTime("returnDelayTime", 0x05, 1, in, out, 0),
			_angleLimitCW("angleLimitCW", 0x06, 2, in, out, 0),
			_angleLimitCCW("angleLimitCCW", 0x08, 2, in, out, 0),
			_temperatureLimit("temperatureLimit", 0x0B, 1, in, out, 0),
			_voltageLowLimit("voltageLowLimit", 0x0C, 1, in, out, 0),
			_voltageHighLimit("voltageHighLimit", 0x0D, 1, in, out, 0),
			_maxTorque("maxTorque", 0x0E, 2, in, out, 0),
			_statusReturnLevel("statusReturnLevel", 0x10, 1, in, out, 0),
			_alarmLed("alarmLed", 0x11, 1, in, out, 0),
			_alarmShutdown("alarmShutdown", 0x12, 1, in, out, 0),
			_multiTurnOffset("multiTurnOffset", 0x14, 2, in, out, 0),
			_resolutionDivider("resolutionDivider", 0x16, 1, in, out, 0),
			_torqueEnable("torqueEnable", 0x18, 1, in, out, 0),
			_led("led", 0x19, 1, in, out, 0),
			_DGain("DGain", 0x1A, 1, in, out, 0),
			_IGain("IGain", 0x1B, 1, in, out, 0),
			_PGain("PGain", 0x1C, 1, in, out, 0),
			_goalPosition("goalPosition", 0x1E, 2, in, out, 0),
			_goalSpeed("goalSpeed", 0x20, 2, in, out, 0),
			_torqueLimit("torqueLimit", 0x22, 2, in, out, 0),
			_position("position", 0x24, 2, in, out, 0),
			_speed("speed", 0x26, 2, in, out, 0),
			_load("load", 0x28, 2, in, out, 0),
			_voltage("voltage", 0x2A, 1, in, out, 0),
			_temperature("temperature", 0x2B, 1, in, out, 0),
			_registered("registered", 0x2C, 1, in, out, 0),
			_moving("moving", 0x2E, 1, in, out, 0),
			_lockEeprom("lockEeprom", 0x2F, 1, in, out, 0),
			_punch("punch", 0x30, 2, in, out, 0),
			_goalAcceleration("goalAcceleration", 0x49, 1, in, out, 0)
        {
        }

        /**
         * Inherit.
         * Set the target motor
         * position in radians
         */


    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::registersList().add(&_goalPos);
            Device::registersList().add(&_position);
            //Setting the aggregation method (sum for the goal position)
            _position.setAggregationPolicy(AggregateSum);
            Device::registersList().add(&_torqueEnable);
        }

    private:

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the DXL children.
		TypedRegisterInt 	_modelNumber; 			//2 00
		TypedRegisterInt	_firmwareVersion; 		//1	02
		TypedRegisterInt 	_id;					//1 03
		TypedRegisterInt 	_baudrate;				//1 04
		TypedRegisterInt 	_returnDelayTime;		//1 05
		TypedRegisterFloat 	_angleLimitCW;			//2 06
		TypedRegisterFloat 	_angleLimitCCW;			//2 08
		TypedRegisterFloat 	_temperatureLimit;		//1 0B
		TypedRegisterFloat 	_voltageLowLimit;		//1 0C
		TypedRegisterFloat 	_voltageHighLimit;		//1 0D
		TypedRegisterFloat 	_maxTorque;				//2 0E
		TypedRegisterInt 	_statusReturnLevel;		//1 10
		TypedRegisterInt 	_alarmLed;				//1 11
		TypedRegisterInt	_alarmShutdown;			//1	12
		TypedRegisterInt	_multiTurnOffset;		//2 14 *
		TypedRegisterInt	_resolutionDivider;		//1 16 *

		// Flash/RAM limit (this info has no impact on the way the registers are handled)

		TypedRegisterBool 	_torqueEnable;			//1 18
		TypedRegisterBool 	_led;					//1 19
		TypedRegisterFloat	_DGain;					//1 1A *
		TypedRegisterFloat	_IGain;					//1 1B *
		TypedRegisterFloat	_PGain;					//1	1C *
		TypedRegisterFloat 	_goalPosition;			//2	1E
		TypedRegisterFloat 	_goalSpeed;				//2	20
		TypedRegisterFloat 	_torqueLimit;			//2	22
		TypedRegisterFloat 	_position;				//2	24
		TypedRegisterFloat 	_speed;					//2	26
		TypedRegisterFloat 	_load;					//2 28
		TypedRegisterFloat 	_voltage;				//1 2A
		TypedRegisterFloat 	_temperature;			//1 2B
		TypedRegisterBool 	_registered;			//1 2C
		TypedRegisterBool 	_moving;				//1 2E
		TypedRegisterBool 	_lockEeprom;			//1 2F
		TypedRegisterFloat 	_punch;					//2 30
		TypedRegisterFloat 	_goalAcceleration;		//1 49 *


};


}

