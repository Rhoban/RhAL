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
         * Therefore, we decided to declare here only the Eeprom registers that are common among all the dxl devices.
         *
         */
        _modelNumber("modelNumber", 0x00, 2, convEncode_2Bytes, convDecode_2Bytes, 0),
        _firmwareVersion("firmwareVersion", 0x02, 1, convEncode_1Byte, convDecode_1Byte, 0),
        _id("id", 0x03, 1, convEncode_1Byte, convDecode_1Byte, 0),
        _baudrate("baudrate", 0x04, 1, convEncode_baudrate, convDecode_baudrate, 0),
        _returnDelayTime("returnDelayTime", 0x05, 1, convEncode_returnDelayTime, convDecode_returnDelayTime, 0),
        _temperatureLimit("temperatureLimit", 0x0B, 1, convEncode_temperature, convDecode_temperature, 0),
        _voltageLowLimit("voltageLowLimit", 0x0C, 1, convEncode_voltage, convDecode_voltage, 0),
        _voltageHighLimit("voltageHighLimit", 0x0D, 1, convEncode_voltage, convDecode_voltage, 0),
        _maxTorque("maxTorque", 0x0E, 2, convEncode_torque, convDecode_torque, 0),
        _statusReturnLevel("statusReturnLevel", 0x10, 1, convEncode_1Byte, convDecode_1Byte, 0),
        _alarmShutdown("alarmShutdown", 0x12, 1, convEncode_1Byte, convDecode_1Byte, 0),
		//Parameters configuration
		_inverted("inverse", false),
		_zero("zero", 0.0),
		_stallTorque("stallTorque", 1000)
        {
        }

        virtual uint16_t getModelNumber() {
        	return _modelNumber.readValue().value;
        }

        virtual int16_t getFirmwareVersion() {
        	return _firmwareVersion.readValue().value;
        }

        virtual uint8_t getId() {
        	return _id.readValue().value;
        }
        virtual void setId(uint8_t id) {
        	_id.writeValue(id);
        }

        /**
		 * Returns the baudrate in BPS
		 */
		virtual float getBaudrate() {
			return _baudrate.readValue().value;
		}
		/**
		 * Sets the baudrate in BPS.
		 */
		virtual void setBaudrate(float baudrate) {
			_baudrate.writeValue(baudrate);
		}

		/**
		 * Returns the return delay time in us
		 */
        virtual uint8_t getReturnDelayTime() {
        	return _returnDelayTime.readValue().value;
        }
        /**
         * Sets the return delay time in us
         */
        virtual void setReturnDelayTime(uint8_t delay) {
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
        virtual void setAngleLimits(float limits[2]) = 0;


        /**
         * Returns the temperature limit in degrees Celcius
         */
        virtual float getTemperatureLimit() {
        	return _temperatureLimit.readValue().value;
        }
        /**
         * Sets the temperature limit in degrees Celcius
         */
        virtual void setTemperatatureLimit(float value) {
        	_temperatureLimit.writeValue(value);
        }

        /**
         * Fills an array of 2 floats with the voltage limits in volts :
         * [min voltage value, max voltage value]
         */
		virtual void getVoltageLimits(float voltageLimits[2]) {
			voltageLimits[0] = _voltageLowLimit.readValue().value;
			voltageLimits[1] = _voltageHighLimit.readValue().value;
		}

		/**
		 * Sets the voltage voltage limits. Expected format, array of 2 floats :
		 * [min voltage in volts, max voltage in volts]
		 */
		virtual void setVoltageLimits(float voltageLimits[2]) {
			_voltageLowLimit.writeValue(voltageLimits[0]);
			_voltageHighLimit.writeValue(voltageLimits[1]);
		}
		/**
		 * Returns the torque limit in N.m
		 */
		virtual float getTorqueLimit() {
			return _maxTorque.readValue().value;
		}
		/**
		 * Sets the torque limit in N.m
		 */
		virtual void setTorqueLimit(float torqueLimit) {
			_maxTorque.writeValue(torqueLimit);
		}

		/**
		 * Returns the torque limit as a % of the max torque
		 */
		inline float getTorqueLimitNormalized() {
			return getTorqueLimit()/_stallTorque.value;
		}
		/**
		 * Sets the torque limit as a % of the max torque
		 */
		inline void setTorqueLimitNormalized(float torqueLimitNormalized) {
			setTorqueLimit(torqueLimitNormalized * _stallTorque.value);
		}

		/**
		 * Returns the status return level without conversion
		 */
		virtual uint8_t getStatusReturnLevel() {
			return _statusReturnLevel.readValue().value;
		}
		/**
		 * Sets the status return level without conversion
		 */
		virtual void setStatusReturnLevel(uint8_t statusReturnLevel) {
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
		virtual uint8_t getAlarmShutdown() {
			return _alarmShutdown.readValue().value;
		}
		/**
		 * Sets the alarm led shutdown value without conversion
		 */
		virtual void setAlarmShutdown(uint8_t alarmShutdown) {
			_alarmShutdown.writeValue(alarmShutdown);
		}


        // The methods above this point typically involve flash registers. The methods below this point typically involve RAM registers.


		virtual bool getTorqueEnable() = 0;
		virtual void setTorqueEnable(bool torqueEnable) = 0;

        inline void enableTorque() {
        	setTorqueEnable(true);
        }
        inline void disableTorque() {
        	setTorqueEnable(false);
        }

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
         * Returns the goal position in rads
         */
		inline float getGoalPositionRad() {
			return Deg2Rad(getGoalPosition());
		}

		/**
		 * Sets the goal position in degrees without boundaries ('725' will be sent as is)
		 */
		virtual void setGoalPositionMultiTurn(float goalPosition) = 0;
		/**
		 * Sets the goal position in rads
		 */
		inline void setGoalPositionRad(float goalPositionRad) {
			setGoalPosition(Rad2Deg(goalPositionRad));
		}

		/**
		 * Returns the goal speed in degrees/s
		 */
		virtual float getGoalSpeed() = 0;
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
		 * Returns the current position in rads
		 */
		inline float getPositionRad() {
			return Deg2Rad(getPosition());
		}

		/**
		 * Returns the current speed in degrees/s
		 */
		virtual float getSpeed() = 0;
		/**
		 * Returns the currents speed in rad/s
		 */
		inline float getSpeedRad() {
			return Deg2Rad(getSpeed());
		}

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

    protected :
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

            *(buffer) = (value & 0xFF);
            *(buffer + 1) = ((value >> 8) & 0xFF);
            *(buffer + 2) = ((value >> 16) & 0xFF);
            *(buffer + 3) = ((value >> 24) & 0xFF);
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
            float val;
            // Attention ! To be tested
            val = (*(buffer + 3) << 8) | (*(buffer + 2) << 8) | (*(buffer + 1) << 8) | (*(buffer) & 0x00FF);
            return val;
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
        inline bool convDecode_Bool(data_t* buffer)
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
        inline uint8_t convDecode_1Byte(data_t* buffer)
        {
        	return read1ByteFromBuffer(buffer);
        }
        /**
         * Default 2Bytes decode (raw copy)
         */
        inline uint16_t convDecode_2Bytes(data_t* buffer)
        {
        	return read2BytesFromBuffer(buffer);
        }
        /**
         * Default float decode (raw copy)
         */
        inline float convDecode_float(data_t* buffer)
        {
        	return readFloatFromBuffer(buffer);
        }

        /**
         * Encode function for dxl baudrate, input in BPS
         */
        inline void convEncode_baudrate(data_t* buffer, int value) {
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
        inline int convDecode_baudrate(data_t* buffer) {
        	return 2000000 / (read1ByteFromBuffer(buffer) + 1);
        }

        /**
         * Encode function for the return delay time, input in us
         */
        inline void convEncode_returnDelayTime(data_t* buffer, int value) {
        	write1ByteToBuffer(buffer, (uint8_t) (value/2));
        }
        /**
         * Decode function for dxl baudrate, output in us
         */
        inline int convDecode_returnDelayTime(data_t* buffer) {
        	return read1ByteFromBuffer(buffer) * 2;
        }

        /**
         * Encode function for the temperature, input in degrees Celsius
         */
        inline void convEncode_temperature(data_t* buffer, float value) {
        	write1ByteToBuffer(buffer, (uint8_t) value);
        }
        /**
         * Decode function for the temperature, output in degrees Celsius
         */
        inline float convDecode_temperature(data_t* buffer) {
        	return read1ByteFromBuffer(buffer);
        }

        /**
         * Encode function for the voltage, input in V
         */
        inline void convEncode_voltage(data_t* buffer, float value) {
        	uint8_t result = value*10;
        	if (result < 0) {
        		result = 0;
        	}
        	write1ByteToBuffer(buffer, result);
        }
        /**
         * Decode function for the voltage, output in V
         */
        inline float convDecode_voltage(data_t* buffer) {
        	return read1ByteFromBuffer(buffer)/10.0;
        }

        /**
         * Encode function for the torque, input in N.m
         */
        inline void convEncode_torque(data_t* buffer, float value) {
        	float result = value / _stallTorque.value();
        	if (result > 1) {
        		result = 1;
        	}
        	write2BytesToBuffer(buffer, uint16_t(result*1023));
        }
        /**
         * Decode function for the torque, output in N.m
         */
        inline float convDecode_torque(data_t* buffer) {
        	float result = (read2BytesFromBuffer(buffer)/1023.0)*_stallTorque.value();
        	return result;
        }
    private:
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
			ParameterNumber _stallTorque;

};

}


/* Registers present in all DXLs. Beware though, some of them are present at different addresses depending of the device, forcing their implementation below the DXL class...
 *
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



