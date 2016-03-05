#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"
#include <cmath>

namespace RhAL {

/**
 * Encode function for position, input in degrees [-180, 180[ (precision : 360/4096 degrees)
 * 180 and -180 are the exact same point
 */
inline void convEncode_PositionMx(data_t* buffer, float value)
{
	value = 2048.0 + value * 4096/360.0;

	uint16_t position = std::lround(value)%4096;
	write2BytesToBuffer(buffer, position);
}
/**
 * Decode function for position, output in degrees [-180, 180[ (precision : 360/4096 degrees)
 */
inline float convDecode_PositionMx(const data_t* buffer)
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

/**
 * Encode function for speed, input in degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
inline void convEncode_SpeedMx(data_t* buffer, float value)
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
/**
 * Decode function for speed, input in degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
inline float convDecode_SpeedMx(const data_t* buffer)
{
	float conversion = 0.68662;
	uint16_t val = read2BytesFromBuffer(buffer);
	if (val < 1024) {
		return val * conversion;
	} else {
		return -(val - 1024) * conversion;
	}
}

/**
 * Encode function for acceleration, input in degrees/s^2 [0, 2180] (precision : 8.583 Degree / sec^2)
 */
inline void convEncode_AccelerationMx(data_t* buffer, float value)
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
/**
 * Decode function for acceleration, input in degrees/s^2 [0, 2180] (precision : 8.583 Degree / sec^2)
 */
inline float convDecode_AccelerationMx(const data_t* buffer)
{
	float conversion = 8.583;
	uint8_t val = read1ByteFromBuffer(buffer);
	return val * conversion;
}



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
        	//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=true, forceWrite=false, isSlow=false)
            // ReadOnly registers: ModelNumber, FirmwareVersion, PresentPosition, PresentLoad, PresentVoltage, PresentTemperature, Registered, Moving.
			_angleLimitCW("angleLimitCW", 0x06, 2, convEncode_PositionMx, convDecode_PositionMx, 0, true, false, true),
			_angleLimitCCW("angleLimitCCW", 0x08, 2, convEncode_PositionMx, convDecode_PositionMx, 0, true, false, true),
			_alarmLed("alarmLed", 0x11, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),
			_multiTurnOffset("multiTurnOffset", 0x14, 2, convEncode_2Bytes, convDecode_2Bytes, 0, true, false, true),
			_resolutionDivider("resolutionDivider", 0x16, 1, convEncode_1Byte, convDecode_1Byte, 0, true, false, true),

			_torqueEnable("torqueEnable", 0x18, 1, convEncode_Bool, convDecode_Bool, 0),
			_led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0),
			_DGain("DGain", 0x1A, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_IGain("IGain", 0x1B, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_PGain("PGain", 0x1C, 1, convEncode_1Byte, convDecode_1Byte, 0),
			_goalPosition("goalPosition", 0x1E, 2, convEncode_PositionMx, convDecode_PositionMx, 0),
			_goalSpeed("goalSpeed", 0x20, 2, convEncode_SpeedMx, convDecode_SpeedMx, 0),
			_torqueLimit("torqueLimit", 0x22, 2, convEncode_torque, convDecode_torque, 0),
			_position("position", 0x24, 2, convDecode_PositionMx, 1, false),
			_speed("speed", 0x26, 2,  convDecode_SpeedMx, 0),
			_load("load", 0x28, 2,  convDecode_torque, 0),
			_voltage("voltage", 0x2A, 1,  convDecode_voltage, 0),
			_temperature("temperature", 0x2B, 1, convDecode_temperature, 0),
			_registered("registered", 0x2C, 1, convDecode_Bool, 0),
			_moving("moving", 0x2E, 1, convDecode_Bool, 0),
			_lockEeprom("lockEeprom", 0x2F, 1, convEncode_Bool, convDecode_Bool, 0),
			_punch("punch", 0x30, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
			_goalAcceleration("goalAcceleration", 0x49, 1, convEncode_AccelerationMx, convDecode_AccelerationMx, 0)
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

        /**
         * Applies the configuration to the motors. Used to make sure the angle limits are what they should be.
         */
        virtual void setConfig() override
       	{
        	float angleLimits[2];
        	getAngleLimits(angleLimits);
        	if (abs(angleLimits[0] - _angleLimitCWParameter.value) > _angleLimitCW.getStepValue()/2.0
        			|| abs(angleLimits[1] - _angleLimitCCWParameter.value) > _angleLimitCCW.getStepValue()/2.0) {
        		// Setting the angle limits to what they should be
        		angleLimits[0] = _angleLimitCWParameter.value;
        		angleLimits[1] = _angleLimitCCWParameter.value;
        		setAngleLimits(angleLimits);
        	}
   		}

		/**
		 * Fills an array of 2 floats with the angle limits in degrees:
		 * [CW limit, CCW limit]
		 */
		virtual void getAngleLimits(float limits[2]) override
		{
			limits[0] = _angleLimitCW.readValue().value;
			limits[1] = _angleLimitCCW.readValue().value;
		}
		/**
		 * Sets the angle limits. Expected format, array of 2 floats :
		 * [CW limit in degrees, CCW limit in degrees]
		 */
		virtual void setAngleLimits(const float limits[2]) override
		{
			_angleLimitCW.writeValue(limits[0]);
			_angleLimitCCW.writeValue(limits[1]);
		}

		/**
		 * Returns the alarm led value without conversion
		 */
		virtual uint8_t getAlarmLed() override
		{
			return _alarmLed.readValue().value;
		}
		/**
		 * Sets the alarm led value without conversion
		 */
		virtual void setAlarmLed(uint8_t alarmLed) override
		{
			_alarmLed.writeValue(alarmLed);
		}


		// The methods above this point typically involve flash registers. The methods below this point typically involve RAM registers.

		virtual bool getTorqueEnable() override
		{
			return _torqueEnable.readValue().value;
		}
		virtual TimePoint getTorqueEnableTs() override
		{
			return _torqueEnable.readValue().timestamp;
		}
		virtual void setTorqueEnable(bool torqueEnable) override
		{
			_torqueEnable.writeValue(torqueEnable);
		}
		/**
		 * Returns true if the led is ON, false otherwise
		 */
		virtual bool getLed() override
		{
			return _led.readValue().value;
		}
		virtual TimePoint getLedTs() override
		{
			return _led.readValue().timestamp;
		}
		/**
		 * Sets the led ON(true) or OFF(false)
		 */
		virtual void setLed(bool led) override
		{
			std::cout << "setting to " << led << std::endl;
			_led.writeValue(led);
		}

		/**
		 * Returns the goal position in degrees
		 */
		virtual float getGoalPosition() override
		{
			float value = _goalPosition.readValue().value;
			std::lock_guard<std::mutex> lock(_mutex);
			if (_inverted.value == true) {
				value = value * -1;
			}
			value = value - _zero.value;
			return value;
		}
		virtual TimePoint getGoalPositionTs() override
		{
			return _goalPosition.readValue().timestamp;
		}
		/**
		 * Sets the goal position in degrees
		 */
		virtual void setGoalPosition(float goalPosition) override
		{
			float value = 0.0;
			{
					// Scope for lock guard
				std::lock_guard<std::mutex> lock(_mutex);
				value = goalPosition + _zero.value;

				if (_inverted.value == true) {
					value = value * -1;
				}
			}
			_goalPosition.writeValue(value);
		}

//		/**
//		 * Sets the goal position in degrees. The goal position will linearly go from the current position to goalPosition in duration seconds.
//		 */
//		inline void setGoalPosition(float goalPosition, float duration)
//		{
//			if (duration == 0.0) {
//				setGoalPosition(goalPosition);
//			} else {
//				_goalPosition.setSmoothingOngoing(true);
//				_goalPosition.setGoalValue(goalPosition);
//				_goalPosition.setRemainingDuration(duration);
//				_goalPosition.setT0(t);
//			}
//		}

		/**
		 * Sets the goal position in degrees without boundaries ('725' will be sent as is)
		 */
		//Attention ! Do I have to hack to overpass the conversion function of a register?
		//virtual void setGoalPositionMultiTurn(float goalPosition) = 0;

		/**
		 * Returns the goal speed in degrees/s
		 */
		virtual float getGoalSpeed() override
		{
			int direction = 1;
			{
				//Scope for lock_guard
				std::lock_guard<std::mutex> lock(_mutex);
				if (_inverted.value == true) {
					direction = -1;
				}
			}
			return _goalSpeed.readValue().value * direction;
		}
		virtual TimePoint getGoalSpeedTs() override
		{
			return _goalSpeed.readValue().timestamp;
		}
		/**
		 * Sets the goal speed in degrees/s
		 */
		virtual void setGoalSpeed(float goalSpeed) override
		{
			int direction = 1;
			{
			//Scope for lock_guard
				if (_inverted.value == true) {
					direction = -1;
				}
			}
			_goalSpeed.writeValue(goalSpeed * direction);
		}

		/**
		 * Returns the maximum torque value in % of max
		 */
		virtual float getMaxTorque() override
		{
			return _torqueLimit.readValue().value;
		}
		virtual TimePoint getMaxTorqueTs() override
		{
			return _torqueLimit.readValue().timestamp;
		}
		/**
		 * Sets the maximum torque value in % of max
		 */
		virtual void setMaxTorque(float maxTorque) override
		{
			_torqueLimit.writeValue(maxTorque);
		}

		/**
		 * Returns the current position in degrees
		 */
		virtual float getPosition() override
		{
			float value = _position.readValue().value;
			if (_inverted.value == true) {
				value = value * -1;
			}
			std::lock_guard<std::mutex> lock(_mutex);
			value = value - _zero.value;
			return value;
		}
		virtual TimePoint getPositionTs() override
		{
			return _position.readValue().timestamp;
		}

		/**
		 * Returns the current speed in degrees/s
		 */
		virtual float getSpeed() override
		{
			int direction = 1;
			{
				// Braces for lock_guard
				std::lock_guard<std::mutex> lock(_mutex);
				if (_inverted.value == true) {
					direction = -1;
			}
		}
			return _speed.readValue().value * direction;
		}
		virtual TimePoint getSpeedTs() override
		{
			return _speed.readValue().timestamp;
		}

		/**
		 * Returns the current load in % of max
		 */
		virtual float getLoad() override
		{
			return _load.readValue().value;
		}
		virtual TimePoint getLoadTs() override
		{
			return _load.readValue().timestamp;
		}

		/**
		 * Returns the current voltage in Volts
		 */
		virtual float getVoltage() override
		{
			return _voltage.readValue().value;
		}
		virtual TimePoint getVoltageTs() override
		{
			return _voltage.readValue().timestamp;
		}

		/**
		 * Returns the current temperature in degrees Celcius
		 */
		virtual float getTemperature() override
		{
			return _temperature.readValue().value;
		}
		virtual TimePoint getTemperatureTs() override
		{
			return _temperature.readValue().timestamp;
		}

		/**
		 * Returns true if the instruction is registered, false otherwise
		 */
		virtual bool getRegistered() override
		{
			return _registered.readValue().value;
		}
		virtual TimePoint getRegisteredTs() override
		{
			return _registered.readValue().timestamp;
		}

		/**
		 * Returns true if the motor is moving, false otherwise
		 */
		virtual bool getMoving() override
		{
			return _moving.readValue().value;
		}
		virtual TimePoint getMovingTs() override
		{
			return _moving.readValue().timestamp;
		}

		/**
		 * Returns true of the eeprom is locked, false otherwise
		 */
		virtual bool getLockEeprom() override
		{
			return _lockEeprom.readValue().value;
		}
		virtual TimePoint getLockEepromTs() override
		{
			return _lockEeprom.readValue().timestamp;
		}
		/**
		 * Locks (true) or unlocks (false) the eeprom
		 */
		virtual void setLockEeprom(bool lockEeprom) override
		{
			_lockEeprom.writeValue(lockEeprom);
		}

		/**
		 * Returns the punch value
		 */
		virtual float getPunch() override
		{
			return _punch.readValue().value;
		}
		virtual TimePoint getPunchTs() override
		{
			return _punch.readValue().timestamp;
		}
		/**
		 * Sets the punch value
		 */
		virtual void setPunch(float punch) override
		{
			_punch.writeValue(punch);
		}

		// Non inherited methods :

		/**
		 * Returns the multiturn value
		 */
		inline int getMultiturnOffset()
		{
			return _multiTurnOffset.readValue().value;
		}
		inline TimePoint getMultiturnOffsetTs()
		{
			return _multiTurnOffset.readValue().timestamp;
		}
		/**
		 * Sets the multiturn value
		 */
		inline void setMultiturnOffset(float value)
		{
			_multiTurnOffset.writeValue(value);
		}

		/**
		 * Returns the resolution divider
		 */
		inline int getResolutionDivider()
		{
			return _resolutionDivider.readValue().value;
		}
		inline TimePoint getResolutionDividerTs()
		{
			return _resolutionDivider.readValue().timestamp;
		}
		/**
		 * Sets the resolution divider value
		 */
		inline void setResolutionDivider(float value)
		{
			_resolutionDivider.writeValue(value);
		}

		inline int getPGain()
		{
			return _PGain.readValue().value;
		}
		inline TimePoint getPGainTs()
		{
			return _PGain.readValue().timestamp;
		}
		inline void setPGain(int value)
		{
			_PGain.writeValue(value);
		}

		inline int getIGain()
		{
			return _IGain.readValue().value;
		}
		inline TimePoint getIGainTs()
		{
			return _IGain.readValue().timestamp;
		}
		inline void setIGain(int value)
		{
			_IGain.writeValue(value);
		}

		inline int getDGain()
		{
			return _DGain.readValue().value;
		}
		inline TimePoint getDGainTs()
		{
			return _DGain.readValue().timestamp;
		}
		inline void setDGain(int value)
		{
			_DGain.writeValue(value);
		}

		inline float getGoalAcceleration()
		{
			int direction = 1;
			{
				// Braces for lock_guard
				std::lock_guard<std::mutex> lock(_mutex);
				if (_inverted.value == true) {
					direction = -1;
				}
			}
			return _goalAcceleration.readValue().value * direction;
		}
		inline TimePoint getGoalAccelerationTs()
		{
			return _goalAcceleration.readValue().timestamp;
		}
		inline void setGoalAcceleration(float value)
		{
			int direction = 1;
			{
				// Braces for lock_guard
				std::lock_guard<std::mutex> lock(_mutex);
				if (_inverted.value == true) {
					direction = -1;
				}
			}
			_goalAcceleration.writeValue(value * direction);
		}

		/**
		 * Sets the angle limits to the maximum possible range
		 */
		virtual void setJointMode() override
		{
			float limits[2];
			limits[0] = -180;
			//Means "180 minus one step"
			limits[1] = 180 - 360.0/4096.0;
			setAngleLimits(limits);
		}
		virtual void setWheelMode() override
		{
			float limits[2];
			limits[0] = -180;
			limits[1] = -180;
			setAngleLimits(limits);
		}
		inline void setMultiTurnMode()
		{
			float limits[2];
			limits[0] = 180 - 360.0/4096.0;
			limits[1] = 180 - 360.0/4096.0;
			setAngleLimits(limits);
		}

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
		TypedRegisterInt	_DGain;					//1 1A *
		TypedRegisterInt	_IGain;					//1 1B *
		TypedRegisterInt	_PGain;					//1	1C *
		TypedRegisterFloat 	_goalPosition;			//2	1E
		TypedRegisterFloat 	_goalSpeed;				//2	20
		TypedRegisterFloat 	_torqueLimit;			//2	22
		TypedRegisterFloat 	_position;				//2	24
		TypedRegisterFloat 	_speed;					//2	26
		TypedRegisterFloat 	_load;					//2 28
		TypedRegisterFloat 	_voltage;				//1 2A
		TypedRegisterInt 	_temperature;			//1 2B
		TypedRegisterBool 	_registered;			//1 2C
		TypedRegisterBool 	_moving;				//1 2E
		TypedRegisterBool 	_lockEeprom;			//1 2F
		TypedRegisterFloat 	_punch;					//2 30
		TypedRegisterFloat 	_goalAcceleration;		//1 49 *
};


}

// Finds getter :
// (virtual )(\w+ )(get\w+)(.*)( = 0; )(_\w+)(\s{1})
// Instanciates it :
// $1$2$3$4 override\n\t\t{\n\t\t\t
//Finds setter :
// (virtual )(\w+ )(set\w+)(.{1})(\w+)(\s{1})(\w+)(.{1})( = 0; )(_\w+)(\s{1})
// Instanciates it :
//$1$2$3$4$5$6$7$8 override\n\t\t{\n\t\t\t
