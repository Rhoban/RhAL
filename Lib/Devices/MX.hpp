#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"
#include <cmath>

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
        	//_register("name", adress, size, encodeFunction, decodeFunction, updateFreq)
			_angleLimitCW("angleLimitCW", 0x06, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_angleLimitCCW("angleLimitCCW", 0x08, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_alarmLed("alarmLed", 0x11, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_multiTurnOffset("multiTurnOffset", 0x14, 2, convEncode_2Bytes, convDecode_2Bytes, 0),

			_resolutionDivider("resolutionDivider", 0x16, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_torqueEnable("torqueEnable", 0x18, 1, convEncode_Bool, convDecode_Bool, 0),
			_led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0),
			_DGain("DGain", 0x1A, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_IGain("IGain", 0x1B, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_PGain("PGain", 0x1C, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_goalPosition("goalPosition", 0x1E, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_goalSpeed("goalSpeed", 0x20, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_torqueLimit("torqueLimit", 0x22, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_position("position", 0x24, 2, convEncode_Position, convDecode_Position, 1),
			_speed("speed", 0x26, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_load("load", 0x28, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_voltage("voltage", 0x2A, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_temperature("temperature", 0x2B, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_registered("registered", 0x2C, 1, convEncode_Bool, convDecode_Bool, 0),
			_moving("moving", 0x2E, 1, convEncode_Bool, convDecode_Bool, 0),
			_lockEeprom("lockEeprom", 0x2F, 1, convEncode_Bool, convDecode_Bool, 0),
			_punch("punch", 0x30, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_goalAcceleration("goalAcceleration", 0x49, 1, convEncode_1Byte, convDecode_1Byte, 0)
        {
        }


		/**
		 * Fills an array of 2 floats with the angle limits in degrees:
		 * [CW limit, CCW limit]
		 */
		virtual void getAngleLimits(float limits[2]) = 0;
		/**
		 * Sets the angle limits. Expected format, array of 2 floats :
		 * [CW limit in degrees, CCW limit in degrees]
		 */
		virtual void setAngleLimits(float limits[2]) = 0;

		/**
		 * Returns the alarm led value without conversion
		 */
		virtual uint8_t getAlarmLed() = 0;
		/**
		 * Sets the alarm led value without conversion
		 */
		virtual void setAlarmLed(uint8_t alarmLed) = 0;


		// The methods above this point typically involve flash registers. The methods below this point typically involve RAM registers.


		virtual bool getTorqueEnable() = 0;
		virtual void setTorqueEnable(bool torqueEnable) = 0;

		/**
		 * Returns true if the led is ON, false otherwise
		 */
		virtual bool getLed() = 0;
		/**
		 * Sets the led ON(true) or OFF(false)
		 */
		virtual void setLed(bool led) = 0;

		/**
		 * Returns the goal position in degrees
		 */
		virtual float getGoalPosition() = 0;
		/**
		 * Sets the goal position in degrees
		 */
		virtual void setGoalPosition(float goalPosition) = 0;
		/**
		 * Sets the goal position in degrees without boundaries ('725' will be sent as is)
		 */
		virtual void setGoalPositionMultiTurn(float goalPosition) = 0;

		/**
		 * Returns the goal speed in degrees/s
		 */
		virtual float getGoalSpeed() = 0;
		/**
		 * Sets the goal speed in degrees/s
		 */
		virtual void setGoalSpeed(float goalSpeed) = 0;

		/**
		 * Returns the maximum torque value in N.m
		 */
		virtual float getMaxTorque() = 0;
		/**
		 * Sets the maximum torque value in N.m
		 */
		virtual void setMaxTorque(float maxTorque) = 0;

		/**
		 * Returns the current position in degrees
		 */
		virtual float getPosition() = 0;

		/**
		 * Returns the current speed in degrees/s
		 */
		virtual float getSpeed() = 0;

		/**
		 * Returns the current load in N.m
		 */
		virtual float getLoad() = 0;

		/**
		 * Returns the current load as a percentage of the maximum value
		 */
		virtual float getLoadNormalized() = 0;


		/**
		 * Returns the current voltage in Volts
		 */
		virtual float getVoltage() = 0;

		/**
		 * Returns the current temperature in degrees Celcius
		 */
		virtual float getTemperature() = 0;

		/**
		 * Returns true if the instruction is registered, false otherwise
		 */
		virtual bool getRegistered() = 0;

		/**
		 * Returns true if the motor is moving, false otherwise
		 */
		virtual bool getMoving() = 0;

		/**
		 * Returns true of the eeprom is locked, false otherwise
		 */
		virtual bool getLockEeprom() = 0;
		/**
		 * Locks (true) or unlocks (false) the eeprom
		 */
		virtual void setLockEeprom(bool lockEeprom) = 0;

		/**
		 * Returns the punch value
		 */
		virtual float getPunch() = 0;
		/**
		 * Sets the punch value
		 */
		virtual void setPunch(float punch) = 0;

//		_multiTurnOffset;		//2 14 *
//		TypedRegisterInt	_resolutionDivider;		//1 16 *
//		TypedRegisterFloat	_DGain;					//1 1A *
//		TypedRegisterFloat	_IGain;					//1 1B *
//		TypedRegisterFloat	_PGain;					//1	1C *
//		TypedRegisterFloat 	_goalAcceleration;		//1 49 *

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
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
        To do : check this conversion function and get a compilable version
        /**
         * Encode function for position, input in degrees [-180, 180] (precision : 360/4096 degrees)
         */
        inline void convEncode_Position(data_t* buffer, float value)
        {
        	if (value > 180) {
        		value = 180;
        	} else if (value < -180) {
        		value = -180;
        	}
            if (value > 0) {
                value = value * 4096 / 360;
            } else {
            	value = 2048 + (180 + value) * 4096 / 360;
            }

            uint16_t position = std::lround(value)%4096;
            write2BytesToBuffer(buffer, position);
            // works in pypot : 4095 * ((360 / 2 + value) / 360)
        }
        /**
         * Decode function for position, output in degrees [-180, 180] (precision : 360/4096 degrees)
         */
        inline float convDecode_Position(const data_t* buffer)
        {
            uint16_t val = read2BytesFromBuffer(buffer);
            if (val <= 2048) {
            	return val * 360 / 4096.0;
            } else {
            	return -(4096 - val) * 360 / 4096.0;
            }
        }

        // Values are not restricted to a specific range (e.g [-180, 180]) because of the multi-turn option
        sgf

    private:

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the DXL children.
		TypedRegisterFloat 	_angleLimitCW;			//2 06
		TypedRegisterFloat 	_angleLimitCCW;			//2 08
		TypedRegisterInt 	_alarmLed;				//1 11
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

