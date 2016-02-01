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
	to do : add registers to constructor and to onInit
        /**
         * Initialization with name and id
         */
        inline MX(const std::string& name, id_t id) :
            DXL(name, id),
            _goalPos("goalPos", 0X1E, 2, convIn_MXPos, convOut_MXPos, 0),
            _position("pos", 0X24, 2, convIn_MXPos, convOut_MXPos, 1),
        	_torqueEnable("torqueEnable", 0X18, 1, convIn_Default<bool>, convOut_Default<bool>, 0)
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

