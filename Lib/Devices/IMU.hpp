#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include <math.h>
#include <type_traits>

namespace RhAL {

/**
 * Conversion functions
 */

/**
 * Decode function used when the device outputs values in 0.01 degrees. Output in degrees.
 */
inline float convDecode_100th_degrees(const data_t* buffer)
{
	// Beware, casting an unsigned into float and casting a signed into float is not the same thing...
	return (int16_t)read2BytesFromBuffer(buffer)/100.0;
}

/**
 * Decode function used with the gyroscope that outputs values in 0.02deg/s. Output in deg/s
 */
inline float convDecode_gyro(const data_t* buffer)
{
	return (int16_t)read2BytesFromBuffer(buffer)/50.0;
}


/**
 * Inertial Measurement Unit (IMU)
 *
 */
class IMU : public Device
{
    public:
        /**
         * Initialization with name and id
         */
        inline IMU(const std::string& name, id_t id) :
            Device(name, id),

			//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
			_yaw("yaw", 0x24, 2, convEncode_2Bytes, convDecode_100th_degrees, 1),
			_pitch("pitch", 0x26, 2, convEncode_2Bytes, convDecode_100th_degrees, 1),
			_roll("roll", 0x28, 2, convEncode_2Bytes, convDecode_100th_degrees, 1),
			_accX("accX", 0x2A, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_accY("accY", 0x2C, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_accZ("accZ", 0x2E, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_gyroX("gyroX", 0x30, 2, convEncode_2Bytes, convDecode_gyro, 1),
			_gyroY("gyroY", 0x32, 2, convEncode_2Bytes, convDecode_gyro, 1),
			_gyroZ("gyroZ", 0x34, 2, convEncode_2Bytes, convDecode_gyro, 1),
			_gyroYaw("gyroYaw", 0x36, 2, convEncode_2Bytes, convDecode_100th_degrees, 1),
			_magnX("magnX", 0x38, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_magnY("magnY", 0x3A, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_magnZ("magnZ", 0x3C, 2, convEncode_2Bytes, convDecode_2Bytes, 1),
			_magnAzimuth("magnAzimuth", 0x3E, 2, convEncode_2Bytes, convDecode_100th_degrees, 1)
        {
		}

        inline float getYaw()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _yaw.readValue().value;
        }
        inline TimePoint getYawTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _yaw.readValue().timestamp;
        }

        inline float getPitch()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _pitch.readValue().value;
        }
        inline TimePoint getPitchTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _pitch.readValue().timestamp;
        }

        inline float getRoll()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _roll.readValue().value;
        }
        inline TimePoint getRollTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _roll.readValue().timestamp;
        }

        inline int32_t getAccX()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accX.readValue().value;
        }
        inline TimePoint getAccXTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accX.readValue().timestamp;
        }

        inline int32_t getAccY()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accY.readValue().value;
        }
        inline TimePoint getAccYTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accY.readValue().timestamp;
        }

        inline int32_t getAccZ()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accZ.readValue().value;
        }
        inline TimePoint getAccZTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _accZ.readValue().timestamp;
        }

        inline float getGyroX()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroX.readValue().value;
        }
        inline TimePoint getGyroXTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroX.readValue().timestamp;
        }

        inline float getGyroY()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroY.readValue().value;
        }
        inline TimePoint getGyroYTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroY.readValue().timestamp;
        }

        inline float getGyroZ()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroZ.readValue().value;
        }
        inline TimePoint getGyroZTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroZ.readValue().timestamp;
        }

        inline float getGyroYaw()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroZ.readValue().value;
        }
        inline TimePoint getGyroYawTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _gyroZ.readValue().timestamp;
        }

        inline int32_t getMagnX()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnX.readValue().value;
        }
        inline TimePoint getMagnXTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnX.readValue().timestamp;
        }

        inline int32_t getMagnY()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnY.readValue().value;
        }
        inline TimePoint getMagnYTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnY.readValue().timestamp;
        }

        inline int32_t getMagnZ()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnZ.readValue().value;
        }
        inline TimePoint getMagnZTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnZ.readValue().timestamp;
        }

        inline float getMagnAzimuth()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnAzimuth.readValue().value;
        }
        inline TimePoint getMagnAzimuthTs()
        {
        	std::lock_guard<std::mutex> lock(_mutex);

        	return _magnAzimuth.readValue().timestamp;
        }

    protected:
		/**
		 * Register
		 */
		//The following comments specify the register size and address in the hardware.
		TypedRegisterFloat 	_yaw;		 			//2 0x24
		TypedRegisterFloat 	_pitch;		 			//2 0x26
		TypedRegisterFloat 	_roll;		 			//2 0x28
		TypedRegisterInt 	_accX;		 			//2 0x2A
		TypedRegisterInt 	_accY;		 			//2 0x2C
		TypedRegisterInt 	_accZ;		 			//2 0x2E
		TypedRegisterFloat 	_gyroX;		 			//2 0x30
		TypedRegisterFloat 	_gyroY;		 			//2 0x32
		TypedRegisterFloat 	_gyroZ;		 			//2 0x34
		TypedRegisterFloat 	_gyroYaw;		 		//2 0x36
		TypedRegisterInt 	_magnX;		 			//2 0x38
		TypedRegisterInt 	_magnY;		 			//2 0x3A
		TypedRegisterInt 	_magnZ;		 			//2 0x3C
		TypedRegisterFloat 	_magnAzimuth; 			//2 0x3E

		/**
		 * Inherit.
		 * Declare Registers and parameters
		 */
		inline virtual void onInit() override
		{
			Device::registersList().add(&_yaw);
			Device::registersList().add(&_pitch);
			Device::registersList().add(&_roll);
			Device::registersList().add(&_accX);
			Device::registersList().add(&_accY);
			Device::registersList().add(&_accZ);
			Device::registersList().add(&_gyroX);
			Device::registersList().add(&_gyroY);
			Device::registersList().add(&_gyroZ);
			Device::registersList().add(&_gyroYaw);
			Device::registersList().add(&_magnX);
			Device::registersList().add(&_magnY);
			Device::registersList().add(&_magnZ);
			Device::registersList().add(&_magnAzimuth);

		}

};

/**
 * DeviceManager specialized for IMU
 */
template <>
class ImplManager<IMU> : public TypedManager<IMU>
{
    public:

        inline static type_t typeNumber()
        {
            return 253;
        }

        inline static std::string typeName()
        {
            return "IMU";
        }
};

}


