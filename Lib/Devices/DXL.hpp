#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include <math.h>

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
 * Write/Read to/from given 
 * data buffer given word value
 */
inline void writeWordToBuffer(data_t* buffer, uint16_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
}
inline uint16_t readWordFromBuffer(const data_t* buffer)
{
    uint16_t val;
    val = (*(buffer + 1) << 8) | (*(buffer) & 0x00FF);
    return val;
}

/**
 * Conversion from typed value to buffer (in)
 * and buffer to typed value (out) 
 * for RX position values using 
 * 1024 max representation.
 */
inline void convIn_RXPos(data_t* buffer, float value)
{
    if (value > Deg2Rad(150)) value = Deg2Rad(150);
    if (value < -Deg2Rad(150)) value = -Deg2Rad(150);
    value += Deg2Rad(150);
    value *= 1023/Deg2Rad(300);
    if (value < 0.0) value = 0.0;
    if (value > 1023.0) value = 1023.0;
    uint16_t v = std::lround(value);
    writeWordToBuffer(buffer, v);
}
inline float convOut_RXPos(const data_t* buffer)
{
    uint16_t val = readWordFromBuffer(buffer);
    float value = val;
    return value*Deg2Rad(300)/1023 - Deg2Rad(150);
}

/**
 * Default raw copy conversion
 */
template <typename T>
inline void convIn_Default(data_t* buffer, T value)
= 0;
template <typename T>
inline T convOut_Default(const data_t* buffer)
= 0;


/**
 * Conversion from typed value to buffer (in)
 * and buffer to typed value (out) 
 * for MX position values using 
 * 4096 max representation.
 */
inline void convIn_MXPos(data_t* buffer, float value)
{
    if (value > Deg2Rad(180)) value = Deg2Rad(180);
    if (value < -Deg2Rad(180)) value = -Deg2Rad(180);
    value += Deg2Rad(180);
    value *= 4095/Deg2Rad(360);
    if (value < 0.0) value = 0.0;
    if (value > 4095.0) value = 4095.0;
    uint16_t v = std::lround(value);
    writeWordToBuffer(buffer, v);
}
inline float convOut_MXPos(const data_t* buffer)
{
    uint16_t val = readWordFromBuffer(buffer);
    float value = val;
    return value*Deg2Rad(360)/4095 - Deg2Rad(180);
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
            Device(name, id)
        {
        }

        virtual int16_t getModelNumber() = 0;

        virtual int16_t getFirmwareVersion() = 0;

        virtual uint8_t getId() = 0;
        virtual void setId(uint8_t id) = 0;

        /**
		 * Returns the baudrate in BPS
		 */
		virtual float getBaudrate() = 0;
		/**
		 * Sets the baudrate in BPS.
		 */
		virtual void setBaudrate(float baudrate) = 0;
        /**
         * Returns the baudrate as encoded in the dxl datasheet
         */
        virtual uint8_t getBaudrateDxl() = 0;
        /**
         * Sets the baudrate as encoded in the dxl datasheet
         */
        virtual void setBaudrateDxl(uint8_t baudrate) = 0;

        virtual uint8_t getReturnDelayTime() = 0;
        virtual void setReturnDelayTime(uint8_t delay) = 0;

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
        virtual float getTemperatureLimit() = 0;
        /**
         * Sets the temperature limit in degrees Celcius
         */
        virtual void setTemperatatureLimit() = 0;

        /**
         * Fills an array of 2 floats with the voltage limits in volts :
         * [min voltage value, max voltage value]
         */
		virtual void getVoltageLimits(float voltageLimits[2]) = 0;
		/**
		 * Sets the voltage voltage limits. Expected format, array of 2 floats :
		 * [min voltage in volts, max voltage in volts]
		 */
		virtual void setVoltageLimits(float voltageLimits[2]) = 0;

		/**
		 * Returns the torque limit in N.m
		 */
		virtual float getTorqueLimit() = 0;
		/**
		 * Sets the torque limit in N.m
		 */
		virtual void setTorqueLimit(float torqueLimit) = 0;

		/**
		 * Returns the torque limit as a % of the max torque
		 */
		virtual float getTorqueLimitNormalized() = 0;
		/**
		 * Sets the torque limit as a % of the max torque
		 */
		virtual void setTorqueLimitNormalized(float torqueLimitNormalized) = 0;

		/**
		 * Returns the status return level without conversion
		 */
		virtual uint8_t getStatusReturnLevel() = 0;
		/**
		 * Sets the status return level without conversion
		 */
		virtual void setStatusReturnLevel(uint8_t statusReturnLevel) = 0;


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
		virtual uint8_t getAlarmShutdown() = 0;
		/**
		 * Sets the alarm led shutdown value without conversion
		 */
		virtual void setAlarmShutdown(uint8_t alarmShutdown) = 0;


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

};

}



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




