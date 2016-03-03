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
 * PressureSensor
 *
 */
class PressureSensor : public Device
{
    public:
        /**
         * Initialization with name and id
         */
        inline PressureSensor(const std::string& name, id_t id) :
            Device(name, id),

			//_register("name", address, size, encodeFunction, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
			_pressure1("pressure1", 0x24, 3, convEncode_3Bytes, convDecode_3Bytes, 1, true, false, false),
			_pressure2("pressure2", 0x27, 3, convEncode_3Bytes, convDecode_3Bytes, 1, true, false, false),
			_pressure3("pressure3", 0x2A, 3, convEncode_3Bytes, convDecode_3Bytes, 1, true, false, false),
			_pressure4("pressure4", 0x2D, 3, convEncode_3Bytes, convDecode_3Bytes, 1, true, false, false),
			//Parameters configuration
			_zero1("zero1", 0.0),
			_zero2("zero2", 0.0),
			_zero3("zero3", 0.0),
			_zero4("zero4", 0.0)
        {
		}

        inline int32_t getPressure1()
        {
        	//Implicit unsigned to signed cast
        	int32_t value = _pressure1.readValue().value;

        	std::lock_guard<std::mutex> lock(_mutex);
        	return (value - (int32_t)_zero1.value);
        }
        inline TimePoint getPressure1Ts()
        {
        	return _pressure1.readValue().timestamp;
        }

        inline int32_t getPressure2()
        {
        	//Implicit unsigned to signed cast
        	int32_t value = _pressure2.readValue().value;

        	std::lock_guard<std::mutex> lock(_mutex);
        	return (value - (int32_t)_zero2.value);
        }
        inline TimePoint getPressure2Ts()
        {
        	return _pressure2.readValue().timestamp;
        }

        inline int32_t getPressure3()
        {
        	//Implicit unsigned to signed cast
        	int32_t value = _pressure3.readValue().value;

        	std::lock_guard<std::mutex> lock(_mutex);
        	return (value - (int32_t)_zero3.value);
        }
        inline TimePoint getPressure3Ts()
        {
        	return _pressure3.readValue().timestamp;
        }

        inline int32_t getPressure4()
        {
        	//Implicit unsigned to signed cast
        	int32_t value = _pressure4.readValue().value;

        	std::lock_guard<std::mutex> lock(_mutex);
        	return (value - (int32_t)_zero4.value);
        }
        inline TimePoint getPressure4Ts()
        {
        	return _pressure4.readValue().timestamp;
        }


        inline float getZero1()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _zero1.value;
        }
        inline void setZero1(float value)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_zero1.value = value;
        }

        inline float getZero2()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _zero2.value;
        }
        inline void setZero2(float value)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_zero2.value = value;
        }

        inline float getZero3()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _zero3.value;
        }
        inline void setZero3(float value)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_zero3.value = value;
        }

        inline float getZero4()
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	return _zero4.value;
        }
        inline void setZero4(float value)
        {
        	std::lock_guard<std::mutex> lock(_mutex);
        	_zero4.value = value;
        }

    protected:
		/**
		 * Register
		 */
		//The following comments specify the register size and address in the hardware.
		TypedRegisterInt 	_pressure1; 			//3 0x24
		TypedRegisterInt 	_pressure2; 			//3 0x27
		TypedRegisterInt 	_pressure3; 			//3 0x2A
		TypedRegisterInt 	_pressure4; 			//3 0x2D


		/**
		 * Parameters
		 */
		ParameterNumber _zero1;
		ParameterNumber _zero2;
		ParameterNumber _zero3;
		ParameterNumber _zero4;


		/**
		 * Inherit.
		 * Declare Registers and parameters
		 */
		inline virtual void onInit() override
		{
			Device::registersList().add(&_pressure1);
			Device::registersList().add(&_pressure2);
			Device::registersList().add(&_pressure3);
			Device::registersList().add(&_pressure4);

			Device::parametersList().add(&_zero1);
			Device::parametersList().add(&_zero2);
			Device::parametersList().add(&_zero3);
			Device::parametersList().add(&_zero4);
		}

};

/**
 * DeviceManager specialized for PressureSensor
 */
template <>
class ImplManager<PressureSensor> : public TypedManager<PressureSensor>
{
    public:

        inline static type_t typeNumber()
        {
            return 5000;
        }

        inline static std::string typeName()
        {
            return "PressureSensor";
        }
};

}


