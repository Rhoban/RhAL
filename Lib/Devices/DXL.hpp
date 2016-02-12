#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include <math.h>
#include <type_traits>

namespace RhAL {

/**
 * Radian to Degree and
 * Degree to Radian conversion
 */
constexpr inline float Rad2Deg(float a)
{
	return a*180.0/M_PI;
}
constexpr inline float Deg2Rad(float a)
{
	return a*M_PI/180.0;
}

/*
 * Conversion functions
 */

/**
 * Write to given data buffer
 */
inline void write1ByteToBuffer(data_t* buffer, uint8_t value)
{
    *(buffer) = (value & 0xFF);
}
inline void write2BytesToBuffer(data_t* buffer, uint16_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
}
inline void writeFloatToBuffer(data_t* buffer, float value)
{
	// Attention ! This compile time assert checks the size of the float but not its endianness. This implementation might fail depending on the platform.
	static_assert(sizeof(value) == 4, "Float is not 32 bit on this platform, I'm done !");
	// We need to do this in order to access the bits from our float
	unsigned char *c = reinterpret_cast<unsigned char *>(&value);
    *(buffer) = c[0];
    *(buffer + 1) = c[1];
    *(buffer + 2) = c[2];
    *(buffer + 3) = c[3];
}

/**
 * Read from buffer
 */
inline uint8_t read1ByteFromBuffer(const data_t* buffer)
{
    uint8_t val;
    val = *(buffer) & 0xFF;
    return val;
}
inline uint16_t read2BytesFromBuffer(const data_t* buffer)
{
    uint16_t val;
    val = (*(buffer + 1) << 8) | (*(buffer) & 0x00FF);
    return val;
}
inline float readFloatFromBuffer(const data_t* buffer)
{
    // Attention ! To be tested
    uint32_t temp = (*(buffer + 3) << 24) | (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | (*(buffer));
    float* val = reinterpret_cast<float*>(&temp);

    return *val;
}

/**
 * Default raw copy conversions. Since the raw value is contained in the hardware :
 * - "encode" is the conversion from the user to the hardware
 * - "decode" is the conversion from the hardware to the user
 */
/**
 * Default bool encode (raw copy)
 */
inline void convEncode_Bool(data_t* buffer, bool value)
{
	if (value) {
		write1ByteToBuffer(buffer, (uint8_t)1);
	} else {
		write1ByteToBuffer(buffer, (uint8_t)0);
	}
}
/**
 * Default 1Byte encode (raw copy)
 */
inline void convEncode_1Byte(data_t* buffer, uint8_t value)
{
	write1ByteToBuffer(buffer, value);
}
/**
 * Default 2Bytes encode (raw copy)
 */
inline void convEncode_2Bytes(data_t* buffer, uint16_t value)
{
	write2BytesToBuffer(buffer, value);
}
/**
 * Default float encode (raw copy)
 */
inline void convEncode_float(data_t* buffer, float value)
{
	write1ByteToBuffer(buffer, value);
}

/**
 * Default bool decode (raw copy)
 */
inline bool convDecode_Bool(const data_t* buffer)
{
	uint8_t value = read1ByteFromBuffer(buffer);
	bool result = true;
	if (value == 0) {
		result = false;
	}

	return result;
}
/**
 * Default 1Byte decode (raw copy)
 */
inline uint8_t convDecode_1Byte(const data_t* buffer)
{
	return read1ByteFromBuffer(buffer);
}
/**
 * Default 2Bytes decode (raw copy)
 */
inline uint16_t convDecode_2Bytes(const data_t* buffer)
{
	return read2BytesFromBuffer(buffer);
}
/**
 * Default float decode (raw copy)
 */
inline float convDecode_float(const data_t* buffer)
{
	return readFloatFromBuffer(buffer);
}

/**
 * Encode function for dxl baudrate, input in BPS
 */
inline void convEncode_baudrate(data_t* buffer, int value)
{
	int8_t result = (2000000 / value) - 1;
	if (result < 1) {
		// result must be signed for this 'if' to have a meaning
		result = 1;
	}
	write1ByteToBuffer(buffer, (uint8_t) result);
}
/**
 * Decode function for dxl baudrate, output in BPS
 */
inline int convDecode_baudrate(const data_t* buffer)
{
	return 2000000 / (read1ByteFromBuffer(buffer) + 1);
}

/**
 * Encode function for the return delay time, input in us
 */
inline void convEncode_returnDelayTime(data_t* buffer, int value)
{
	write1ByteToBuffer(buffer, (uint8_t) (value/2));
}
/**
 * Decode function for dxl baudrate, output in us
 */
inline int convDecode_returnDelayTime(const data_t* buffer)
{
	return read1ByteFromBuffer(buffer) * 2;
}

/**
 * Encode function for the temperature, input in degrees Celsius
 */
inline void convEncode_temperature(data_t* buffer, float value)
{
	write1ByteToBuffer(buffer, (uint8_t) value);
}
/**
 * Decode function for the temperature, output in degrees Celsius
 */
inline float convDecode_temperature(const data_t* buffer)
{
	return read1ByteFromBuffer(buffer);
}

/**
 * Encode function for the voltage, input in V
 */
inline void convEncode_voltage(data_t* buffer, float value)
{
	if (value > 25.5) {
		value = 25.5;
	} else if (value < 0) {
		value = 0.0;
	}
	uint8_t result = value*10;

	write1ByteToBuffer(buffer, result);
}
/**
 * Decode function for the voltage, output in V
 */
inline float convDecode_voltage(const data_t* buffer)
{
	return read1ByteFromBuffer(buffer)/10.0;
}

/**
 * Encode function for the torque, input in % of max
 */
inline void convEncode_torque(data_t* buffer, float value)
{
	uint16_t result = value * 1023;
	if (result > 1023) {
		result = 1023;
	}
	write2BytesToBuffer(buffer, result);
}
/**
 * Decode function for the torque, output in % of max
 */
inline float convDecode_torque(const data_t* buffer)
{
	return read2BytesFromBuffer(buffer)/1023.0;
}

/**
 * DXL
 *
 * Robotis Dynamixel servo motor
 * abstract base class
 */
class DXL : public Device
{
    public:
        /**
         * Initialization with name and id
         */
        inline DXL(const std::string& name, id_t id) :
            Device(name, id),
			/*
			 * Registers that are common to all the dxl devices should be present here.
			 * Unfortunately, the XL-320 has different addresses starting from the 'goalTorque' register.
			 * This messes up with the genericity of dxl devices and disables the elegant solution.
			 * Therefore, we decided to declare here only the Eeprom/flash registers that are common among all the dxl devices.
			 */
			//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
			_modelNumber("modelNumber", 0x00, 2, convEncode_2Bytes, convDecode_2Bytes, 0, false, false, true),
			_firmwareVersion("firmwareVersion", 0x02, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true),
			_id("id", 0x03, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true),
			_baudrate("baudrate", 0x04, 1, convEncode_baudrate, convDecode_baudrate, 0, false, false, true),
			_returnDelayTime("returnDelayTime", 0x05, 1, convEncode_returnDelayTime, convDecode_returnDelayTime, 0, false, false, true),
			_temperatureLimit("temperatureLimit", 0x0B, 1, convEncode_temperature, convDecode_temperature, 0, false, false, true),
			_voltageLowLimit("voltageLowLimit", 0x0C, 1, convEncode_voltage, convDecode_voltage, 0, false, false, true),
			_voltageHighLimit("voltageHighLimit", 0x0D, 1, convEncode_voltage, convDecode_voltage, 0, false, false, true),
			_maxTorque("maxTorque", 0x0E, 2, convEncode_torque, convDecode_torque, 0, false, false, true),
			_statusReturnLevel("statusReturnLevel", 0x10, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true),
			_alarmShutdown("alarmShutdown", 0x12, 1, convEncode_1Byte, convDecode_1Byte, 0, false, false, true),
			//Parameters configuration
			_inverted("inverse", false),
			_zero("zero", 0.0)
        {
		}

        /**
         * Returns true if the servo's angular convention is inverted, false otherwise
         */
        inline bool getInverted()
        {
        	return _inverted.value;
        }
        /**
         * Sets (true) or resets (false) the inversion of the angular convention
         */
        inline void setInverted(bool value) {
        	_inverted.value = value;
        }

        /**
         * Returns the angle that will be considered as 0 when talking to the servo
         */
        inline float getZero()
        {
        	return _zero.value;
        }
        /**
         * Sets the angle that will be considered as 0 when talking to the servo
         */
        inline void setZero(float value)
        {
        	_zero.value = value;
        }

        virtual uint16_t getModelNumber()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _modelNumber.readValue().value;
        }

        virtual int16_t getFirmwareVersion()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _firmwareVersion.readValue().value;
        }

        virtual uint8_t getId()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _id.readValue().value;
        }
        virtual void setId(uint8_t id)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_id.writeValue(id);
        }

        /**
		 * Returns the baudrate in BPS
		 */
		virtual float getBaudrate()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _baudrate.readValue().value;
		}
		/**
		 * Sets the baudrate in BPS.
		 */
		virtual void setBaudrate(float baudrate)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_baudrate.writeValue(baudrate);
		}

		/**
		 * Returns the return delay time in us
		 */
        virtual uint8_t getReturnDelayTime()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _returnDelayTime.readValue().value;
        }
        /**
         * Sets the return delay time in us
         */
        virtual void setReturnDelayTime(uint8_t delay)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_returnDelayTime.writeValue(delay);
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
        virtual void setAngleLimits(const float limits[2]) = 0;


        /**
         * Returns the temperature limit in degrees Celcius
         */
        virtual float getTemperatureLimit()
        {
        	return _temperatureLimit.readValue().value;
        }
        /**
         * Sets the temperature limit in degrees Celcius
         */
        virtual void setTemperatatureLimit(float value)
        {
        	_temperatureLimit.writeValue(value);
        }

        /**
         * Fills an array of 2 floats with the voltage limits in volts :
         * [min voltage value, max voltage value]
         */
		virtual void getVoltageLimits(float voltageLimits[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			voltageLimits[0] = _voltageLowLimit.readValue().value;
			voltageLimits[1] = _voltageHighLimit.readValue().value;
		}

		/**
		 * Sets the voltage voltage limits. Expected format, array of 2 floats :
		 * [min voltage in volts, max voltage in volts]
		 */
		virtual void setVoltageLimits(float voltageLimits[2])
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_voltageLowLimit.writeValue(voltageLimits[0]);
			_voltageHighLimit.writeValue(voltageLimits[1]);
		}
		/**
		 * Returns the torque limit in % of max
		 */
		virtual float getTorqueLimit()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _maxTorque.readValue().value;
		}
		/**
		 * Sets the torque limit in % of max
		 */
		virtual void setTorqueLimit(float torqueLimit)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_maxTorque.writeValue(torqueLimit);
		}

		/**
		 * Returns the status return level without conversion
		 */
		virtual uint8_t getStatusReturnLevel()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _statusReturnLevel.readValue().value;
		}
		/**
		 * Sets the status return level without conversion
		 */
		virtual void setStatusReturnLevel(uint8_t statusReturnLevel)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_statusReturnLevel.writeValue(statusReturnLevel);
		}

		/**
		 * Returns the alarm led value without conversion
		 */
        virtual uint8_t getAlarmLed() = 0;
        /**
         * Sets the alarm led value without conversion
         */
        virtual void setAlarmLed(uint8_t alarmLed) = 0;

        /**
		 * Returns the alarm shutdown value without conversion
		 */
		virtual uint8_t getAlarmShutdown()
		{
			std::lock_guard<std::mutex> lock(_mutex);
			return _alarmShutdown.readValue().value;
		}
		/**
		 * Sets the alarm led shutdown value without conversion
		 */
		virtual void setAlarmShutdown(uint8_t alarmShutdown)
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_alarmShutdown.writeValue(alarmShutdown);
		}


        // The methods above this point typically involve flash registers. The methods below this point typically involve RAM registers.


		virtual bool getTorqueEnable() = 0;
		virtual TimePoint getTorqueEnableTs() = 0;
		virtual void setTorqueEnable(bool torqueEnable) = 0;

        inline void enableTorque()
        {
        	setTorqueEnable(true);
        }
        inline void disableTorque()
        {
        	setTorqueEnable(false);
        }

		/**
		 * Returns true if the led is ON, false otherwise
		 */
		virtual bool getLed() = 0;
		virtual TimePoint getLedTs() = 0;
		/**
		 * Sets the led ON(true) or OFF(false)
		 */
		virtual void setLed(bool led) = 0;

        /**
         * Returns the goal position in degrees
         */
        virtual float getGoalPosition() = 0;
		virtual TimePoint getGoalPositionTs() = 0;
        /**
         * Sets the goal position in degrees
         */
        virtual void setGoalPosition(float goalPosition) = 0;

        /**
         * Returns the goal position in rads
         */
		inline float getGoalPositionRad()
		{
			return Deg2Rad(getGoalPosition());
		}

		/**
		 * Sets the goal position in degrees without boundaries ('725' will be sent as is)
		 */
		//Attention ! Do I have to hack to overpass the conversion function of a register?
		//virtual void setGoalPositionMultiTurn(float goalPosition) = 0;
		/**
		 * Sets the goal position in rads
		 */
		inline void setGoalPositionRad(float goalPositionRad)
		{
			setGoalPosition(Rad2Deg(goalPositionRad));
		}

		/**
		 * Returns the goal speed in degrees/s
		 */
		virtual float getGoalSpeed() = 0;
		virtual TimePoint getGoalSpeedTs() = 0;
		/**
		 * Sets the goal speed in degrees/s
		 */
		virtual void setGoalSpeed(float goalSpeed) = 0;

		/**
		 * Returns the goal speed in rads/s
		 */
		inline float getGoalSpeedRad() {
			return Deg2Rad(getGoalSpeed());
		}
		/**
		 * Sets the goal speed in rads/s
		 */
		inline void setGoalSpeedRad(float goalSpeedRad) {
			setGoalSpeed(Rad2Deg(goalSpeedRad));
		}

		/**
		 * Returns the maximum torque value in % of max
		 */
		virtual float getMaxTorque() = 0;
		virtual TimePoint getMaxTorqueTs() = 0;
		/**
		 * Sets the maximum torque value in % of max
		 */
		virtual void setMaxTorque(float maxTorque) = 0;

		/**
		 * Returns the current position in degrees
		 */
		virtual float getPosition() = 0;
		virtual TimePoint getPositionTs() = 0;

		/**
		 * Returns the current position in rads
		 */
		inline float getPositionRad() {
			return Deg2Rad(getPosition());
		}

		/**
		 * Returns the current speed in degrees/s
		 */
		virtual float getSpeed() = 0;
		virtual TimePoint getSpeedTs() = 0;
		/**
		 * Returns the currents speed in rad/s
		 */
		inline float getSpeedRad() {
			return Deg2Rad(getSpeed());
		}

		/**
		 * Returns the current load in % of max
		 */
		virtual float getLoad() = 0;
		virtual TimePoint getLoadTs() = 0;


		/**
		 * Returns the current voltage in Volts
		 */
		virtual float getVoltage() = 0;
		virtual TimePoint getVoltageTs() = 0;

		/**
		 * Returns the current temperature in degrees Celcius
		 */
		virtual float getTemperature() = 0;
		virtual TimePoint getTemperatureTs() = 0;

		/**
		 * Returns true if the instruction is registered, false otherwise
		 */
		virtual bool getRegistered() = 0;
		virtual TimePoint getRegisteredTs() = 0;

		/**
		 * Returns true if the motor is moving, false otherwise
		 */
		virtual bool getMoving() = 0;
		virtual TimePoint getMovingTs() = 0;

		/**
		 * Returns true of the eeprom is locked, false otherwise
		 */
		virtual bool getLockEeprom() = 0;
		virtual TimePoint getLockEepromTs() = 0;
		/**
		 * Locks (true) or unlocks (false) the eeprom
		 */
		virtual void setLockEeprom(bool lockEeprom) = 0;

		/**
		 * Returns the punch value
		 */
		virtual float getPunch() = 0;
		virtual TimePoint getPunchTs() = 0;
		/**
		 * Sets the punch value
		 */
		virtual void setPunch(float punch) = 0;

		virtual void setJointMode() = 0;
		virtual void setWheelMode() = 0;

    protected:
		/**
		 * Register
		 */
		//The following comments specify the register size and address in the hardware.
		TypedRegisterInt 	_modelNumber; 			//2 00
		TypedRegisterInt	_firmwareVersion; 		//1	02
		TypedRegisterInt 	_id;					//1 03
		TypedRegisterInt 	_baudrate;				//1 04
		TypedRegisterInt 	_returnDelayTime;		//1 05
		TypedRegisterFloat 	_temperatureLimit;		//1 0B
		TypedRegisterFloat 	_voltageLowLimit;		//1 0C
		TypedRegisterFloat 	_voltageHighLimit;		//1 0D
		TypedRegisterFloat 	_maxTorque;				//2 0E
		TypedRegisterInt 	_statusReturnLevel;		//1 10
		TypedRegisterInt	_alarmShutdown;			//1	12

		/**
		 * Parameters
		 */
		ParameterBool _inverted;
		ParameterNumber _zero;

		/**
		 * Inherit.
		 * Declare Registers and parameters
		 */
		inline virtual void onInit() override
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
			Device::parametersList().add(&_inverted);
			Device::parametersList().add(&_zero);
		}

};

}


/*
 * Registers present in all DXLs. Beware though, some of them are present at different addresses depending of the device, forcing their implementation below the DXL class.
 * I'm looking at you xl320...
 */
//		int16_t modelNumber;
//		uint8_t firmwareVersion;
//		uint8_t id;
//		uint8_t baudrate;
//		uint8_t returnDelayTime;
//		float angleLimits[2];
//		float temperatureLimit;
//        float voltageLimits[2];
//        float maxTorque;
//        uint8_t statusReturnLevel;
//        uint8_t alarmLed;
//		uint8_t alarmShutdown
//        bool torqueEnable;
//        bool led;
//        float goalPosition;
//        float goalPositionRad;
//        float goalSpeed;
//        float goalSpeedRad;
//        float torqueLimit;
//        float torqueLimitNormalized;
//        float position;
//        float positionRad;
//        float speed;
//        float speedRad;
//        float load;
//        float loadNormalized;
//        float voltage;
//        float temperature;
//        bool registered;
//        bool moving;
//        bool lockEeprom;
//		float punch;

/**
 * Regex
 */
// Find virtual getters :
// (virtual )(\w+ )(get\w+)(.*;{1}[\n\r]{1})
//Copy it and add a timestamp getter :
// $1$2$3$4\t\t$1TimePoint $3Ts$4

