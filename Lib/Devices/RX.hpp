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
 * Encode function for position, input in degrees [-150, 150[ (precision : 300/1024 degrees)
 */
inline void convEncode_PositionRx(data_t* buffer, float value)
{
	// "150 minus one step"
	float maxValue = 150 - 300.0/1024.0;
	if (value > maxValue) {
		value = maxValue;
	} else if (value < -150) {
		value = -150;
	}
	value = 512.0 + value * 1024/300.0;

	uint16_t position = std::lround(value)%1024;
	write2BytesToBuffer(buffer, position);
}
/**
 * Decode function for position, output in degrees [-150, 150[ (precision : 300/1024 degrees)
 */
inline float convDecode_PositionRx(const data_t* buffer)
{
	uint16_t val = read2BytesFromBuffer(buffer);
	float result = (val - 512) * 300.0 / 1024.0;
	if (result >= -150 && result < 150) {
		//We're already in the desired portion
	} else {
		//Modulating to be in [-150, 150[
		result = fmod(result + 150.0, 300) - 150;
	}
	return result;
}

/**
 * Encode function for speed, input in degrees/s [-702.42, 702.42] (precision : 0.114 rpm ~= 0.687 degrees/s)
 */
inline void convEncode_SpeedRx(data_t* buffer, float value)
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
inline float convDecode_SpeedRx(const data_t* buffer)
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
 * Decode function for the compliance margin. Same precision and unit than position but in a byte range.
 * Input in degrees [0, 74.7] (precision : 300/1024 degrees)
 */
inline void convEncode_ComplianceMargin(data_t* buffer, float value)
{
	float maxValue = 74.7;
	if (value > maxValue) {
		value = maxValue;
	} else if (value < 0) {
		value = 0.0;
	}
	value = value * 1024/300.0;

	uint8_t position = std::lround(value);
	write1ByteToBuffer(buffer, position);
}

/**
 * Encode function for the compliance margin. Output in degrees [0, 74.7] (precision : 300/1024 degrees)
 */
inline float convDecode_ComplianceMargin(const data_t* buffer)
{
	uint8_t val = read1ByteFromBuffer(buffer);
	float result = val * 300.0 / 1024.0;

	return result;
}

/**
 * Encode function for the compliance slope. Only 7 inputs are possible (cf datasheet) : 1, 2, 3, 4, 5, 6, 7.
 * No unit is given.
 */
inline void convEncode_ComplianceSlope(data_t* buffer, int value)
{
	if (value > 7) {
		value = 7;
	} else if (value < 1) {
		value = 1;
	}

	switch(value) {
	   case 1 :
		   value = 2;
	      break;
	   case 2 :
		   value = 4;
		   break;
	   case 3 :
		   value = 8;
		   break;
	   case 4 :
		   value = 16;
		   break;
	   case 5 :
		   value = 32;
		   break;
	   case 6 :
		   value = 64;
		   break;
	   case 7 :
		   value = 128;
		   break;
	   default :
		   value = 128;
	}

	uint8_t position = value;
	write1ByteToBuffer(buffer, position);
}

/**
 * Encode function for the compliance slope. No unit is given.
 * Only 7 different outputs should be possible : 1, 2, 3, 4, 5, 6, 7.
 */
inline int convDecode_ComplianceSlope(const data_t* buffer)
{
	uint8_t val = read1ByteFromBuffer(buffer);
	int index = 0;
	while (index < 7) {
		// val is a power of 2, the slope is the (power+1). If val == 00000010 then the slope is 2
		if ((val >> index)&1) {
			return index;
		}
		index++;
	}

	return 7;
}

/**
 * RX
 *
 * Robotis Dynamixel RX-XX implementation.
 */
class RX : public DXL
{
    public:
        /**
         * Initialization with name and id
         */
        inline RX(const std::string& name, id_t id) :
            DXL(name, id),
			//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
			_angleLimitCW("angleLimitCW", 0x06, 2, convEncode_PositionRx, convDecode_PositionRx, 0, false, false, true),
			_angleLimitCCW("angleLimitCCW", 0x08, 2, convEncode_PositionRx, convDecode_PositionRx, 0, false, false, true),
			_alarmLed("alarmLed", 0x11, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true),

			_torqueEnable("torqueEnable", 0x18, 1, convEncode_Bool, convDecode_Bool, 0),
			_led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0),
			_complianceMarginCW("complianceMarginCW", 0x1A, 1, convEncode_ComplianceMargin, convDecode_ComplianceMargin, 0),
			_complianceMarginCCW("complianceMarginCCW", 0x1B, 1, convEncode_ComplianceMargin, convDecode_ComplianceMargin, 0),
			_complianceSlopeCW("complianceSlopeCW", 0x1C, 1, convEncode_ComplianceSlope, convDecode_ComplianceSlope, 0),
			_complianceSlopeCCW("complianceSlopeCCW", 0x1D, 1, convEncode_ComplianceSlope, convDecode_ComplianceSlope, 0),
			_goalPosition("goalPosition", 0x1E, 2, convEncode_PositionRx, convDecode_PositionRx, 0),
			_goalSpeed("goalSpeed", 0x20, 2, convEncode_SpeedRx, convDecode_SpeedRx, 0),
			_torqueLimit("torqueLimit", 0x22, 2, convEncode_torque, convDecode_torque, 0),
			_position("position", 0x24, 2, convEncode_PositionRx, convDecode_PositionRx, 1),
			_speed("speed", 0x26, 2, convEncode_SpeedRx, convDecode_SpeedRx, 1),
			_load("load", 0x28, 2, convEncode_torque, convDecode_torque, 0),
			_voltage("voltage", 0x2A, 1, convEncode_voltage, convDecode_voltage, 0),
			_temperature("temperature", 0x2B, 1, convEncode_temperature, convDecode_temperature, 0),
			_registered("registered", 0x2C, 1, convEncode_Bool, convDecode_Bool, 0),
			_moving("moving", 0x2E, 1, convEncode_Bool, convDecode_Bool, 0),
			_lockEeprom("lockEeprom", 0x2F, 1, convEncode_Bool, convDecode_Bool, 0),
			_punch("punch", 0x30, 2, convEncode_2Bytes, convDecode_2Bytes, 0)
        {
        }


		/**
		 * Fills an array of 2 floats with the angle limits in degrees:
		 * [CW limit, CCW limit]
		 */
		virtual void getAngleLimits(float limits[2]) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			limits[0] = _angleLimitCW.readValue().value;
			limits[1] = _angleLimitCCW.readValue().value;
		}
		/**
		 * Sets the angle limits. Expected format, array of 2 floats :
		 * [CW limit in degrees, CCW limit in degrees]
		 */
		virtual void setAngleLimits(const float limits[2]) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_angleLimitCW.writeValue(limits[0]);
			_angleLimitCCW.writeValue(limits[1]);
		}

		/**
		 * Returns the alarm led value without conversion
		 */
		virtual uint8_t getAlarmLed() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _alarmLed.readValue().value;
		}
		/**
		 * Sets the alarm led value without conversion
		 */
		virtual void setAlarmLed(uint8_t alarmLed) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_alarmLed.writeValue(alarmLed);
		}


		// The methods above this point typically involve flash registers. The methods below this point typically involve RAM registers.

		virtual bool getTorqueEnable() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _torqueEnable.readValue().value;
		}
		virtual TimePoint getTorqueEnableTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _torqueEnable.readValue().timestamp;
		}
		virtual void setTorqueEnable(bool torqueEnable) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_torqueEnable.writeValue(torqueEnable);
		}
		/**
		 * Returns true if the led is ON, false otherwise
		 */
		virtual bool getLed() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _led.readValue().value;
		}
		virtual TimePoint getLedTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _led.readValue().timestamp;
		}
		/**
		 * Sets the led ON(true) or OFF(false)
		 */
		virtual void setLed(bool led) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_led.writeValue(led);
		}

		/**
		 * Returns the goal position in degrees
		 */
		virtual float getGoalPosition() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			float value = _goalPosition.readValue().value;
			if (_inverted.value == true) {
				value = value * -1;
			}
			value = value - _zero.value;
			return value;
		}
		virtual TimePoint getGoalPositionTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _goalPosition.readValue().timestamp;
		}
		/**
		 * Sets the goal position in degrees
		 */
		virtual void setGoalPosition(float goalPosition) override
		{
			std::lock_guard<std::mutex> lock(_mutex);

			float value = goalPosition + _zero.value;
			if (_inverted.value == true) {
				value = value * -1;
			}
			_goalPosition.writeValue(value);
		}

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
			std::lock_guard<std::mutex> lock(_mutex);
			int direction = 1;
			if (_inverted.value == true) {
				direction = -1;
			}
			return _goalSpeed.readValue().value * direction;
		}
		virtual TimePoint getGoalSpeedTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _goalSpeed.readValue().timestamp;
		}
		/**
		 * Sets the goal speed in degrees/s
		 */
		virtual void setGoalSpeed(float goalSpeed) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			int direction = 1;
			if (_inverted.value == true) {
				direction = -1;
			}
			_goalSpeed.writeValue(goalSpeed * direction);
		}

		/**
		 * Returns the maximum torque value in % of max
		 */
		virtual float getMaxTorque() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _torqueLimit.readValue().value;
		}
		virtual TimePoint getMaxTorqueTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _torqueLimit.readValue().timestamp;
		}
		/**
		 * Sets the maximum torque value in % of max
		 */
		virtual void setMaxTorque(float maxTorque) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_torqueLimit.writeValue(maxTorque);
		}

		/**
		 * Returns the current position in degrees
		 */
		virtual float getPosition() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			float value = _position.readValue().value;
			if (_inverted.value == true) {
				value = value * -1;
			}
			value = value - _zero.value;
			return value;
		}
		virtual TimePoint getPositionTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _position.readValue().timestamp;
		}

		/**
		 * Returns the current speed in degrees/s
		 */
		virtual float getSpeed() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			int direction = 1;
			if (_inverted.value == true) {
				direction = -1;
			}
			return _speed.readValue().value * direction;
		}
		virtual TimePoint getSpeedTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _speed.readValue().timestamp;
		}

		/**
		 * Returns the current load in % of max
		 */
		virtual float getLoad() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _load.readValue().value;
		}
		virtual TimePoint getLoadTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _load.readValue().timestamp;
		}

		/**
		 * Returns the current voltage in Volts
		 */
		virtual float getVoltage() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _voltage.readValue().value;
		}
		virtual TimePoint getVoltageTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _voltage.readValue().timestamp;
		}

		/**
		 * Returns the current temperature in degrees Celcius
		 */
		virtual float getTemperature() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _temperature.readValue().value;
		}
		virtual TimePoint getTemperatureTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _temperature.readValue().timestamp;
		}

		/**
		 * Returns true if the instruction is registered, false otherwise
		 */
		virtual bool getRegistered() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _registered.readValue().value;
		}
		virtual TimePoint getRegisteredTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _registered.readValue().timestamp;
		}

		/**
		 * Returns true if the motor is moving, false otherwise
		 */
		virtual bool getMoving() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _moving.readValue().value;
		}
		virtual TimePoint getMovingTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _moving.readValue().timestamp;
		}

		/**
		 * Returns true of the eeprom is locked, false otherwise
		 */
		virtual bool getLockEeprom() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _lockEeprom.readValue().value;
		}
		virtual TimePoint getLockEepromTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _lockEeprom.readValue().timestamp;
		}
		/**
		 * Locks (true) or unlocks (false) the eeprom
		 */
		virtual void setLockEeprom(bool lockEeprom) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_lockEeprom.writeValue(lockEeprom);
		}

		/**
		 * Returns the punch value
		 */
		virtual float getPunch() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _punch.readValue().value;
		}
		virtual TimePoint getPunchTs() override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _punch.readValue().timestamp;
		}
		/**
		 * Sets the punch value
		 */
		virtual void setPunch(float punch) override
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_punch.writeValue(punch);
		}

		/**
		 * Sets the angle limits to the maximum possible range
		 */
		virtual void setJointMode() override
		{
			float limits[2];
			limits[0] = -150;
			//Means "150 minus one step"
			limits[1] = 150 - 300.0/1024.0;
			setAngleLimits(limits);
		}
		virtual void setWheelMode() override
		{
			float limits[2];
			limits[0] = -150;
			limits[1] = -150;
			setAngleLimits(limits);
		}

		// Non inherited methods :

		/**
		 * Returns an array of 2 compliance margins [CW, CCW]
		 */
		inline void getComplianceMargins(float margins[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			margins[0] =_complianceMarginCW.readValue().value;
			margins[1] =_complianceMarginCCW.readValue().value;
		}
		/**
		 * Returns an array of 2 TimePoints [CW, CCW]
		 */
		inline void getComplianceMarginsTs(TimePoint marginsTs[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			marginsTs[0] =_complianceMarginCW.readValue().timestamp;
			marginsTs[1] =_complianceMarginCCW.readValue().timestamp;
		}
		/**
		 * Sets the compliance margins with an array of 2 floats [CW, CCW]
		 */
		inline void setComplianceMargins(float margins[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_complianceMarginCW.writeValue(margins[0]);
			_complianceMarginCCW.writeValue(margins[1]);
		}

		/**
		 * Returns an array of 2 compliance slope [CW, CCW]
		 */
		inline void getComplianceSlopes(int slopes[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			slopes[0] =_complianceSlopeCW.readValue().value;
			slopes[1] =_complianceSlopeCCW.readValue().value;
		}
		/**
		 * Returns an array of 2 TimePoints [CW, CCW]
		 */
		inline void getComplianceSlopesTs(TimePoint slopesTs[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			slopesTs[0] =_complianceSlopeCW.readValue().timestamp;
			slopesTs[1] =_complianceSlopeCCW.readValue().timestamp;
		}
		/**
		 * Sets the compliance slopes with an array of 2 floats [CW, CCW]
		 */
		inline void setComplianceSlopes(int slopes[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_complianceSlopeCW.writeValue(slopes[0]);
			_complianceSlopeCCW.writeValue(slopes[1]);
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
			Device::registersList().add(&_torqueEnable);
			Device::registersList().add(&_led);
			Device::registersList().add(&_complianceMarginCW);
			Device::registersList().add(&_complianceMarginCCW);
			Device::registersList().add(&_complianceSlopeCW);
			Device::registersList().add(&_complianceSlopeCCW);
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

        }

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the DXL children.
		TypedRegisterFloat 	_angleLimitCW;			//2 06
		TypedRegisterFloat 	_angleLimitCCW;			//2 08
		TypedRegisterInt 	_alarmLed;				//1 11

		// Flash/RAM limit (this info has no impact on the way the registers are handled)

		TypedRegisterBool 	_torqueEnable;			//1 18
		TypedRegisterBool 	_led;					//1 19
		TypedRegisterFloat	_complianceMarginCW;	//1	1A *
		TypedRegisterFloat	_complianceMarginCCW;	//1	1B *
		TypedRegisterInt	_complianceSlopeCW;		//1	1C *
		TypedRegisterInt	_complianceSlopeCCW;	//1	1D *
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
};


}


