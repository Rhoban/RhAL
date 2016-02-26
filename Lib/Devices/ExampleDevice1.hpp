#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL {

inline void convIn1(RhAL::data_t* buffer, float value)
{
    *(reinterpret_cast<float*>(buffer)) = value;
}
inline float convOut1(const RhAL::data_t* buffer)
{
   return  *(reinterpret_cast<const float*>(buffer));
}

/**
 * BaseExampleDevice1
 *
 * Test base class for ExampleDevice1
 */
class BaseExampleDevice1 : public Device
{
    public:

        inline BaseExampleDevice1(const std::string& name, id_t id) :
            Device(name, id),
            //Registers configuration
            _voltage("voltage", (addr_t)20, 4, convIn1, convOut1, 0)
        {
        }

    protected:

        /**
         * Registers
         */
        TypedRegisterFloat _voltage;

        /**
         * Test Inherit
         */
        virtual inline void onSwap() override
        {
            std::cout << "ExampleDevice1 onSwap() " << this->name() << std::endl;
        }
};

/**
 * ExampleDevice1
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice1 : public BaseExampleDevice1
{
    public:

        /**
         * Initialization with name and id
         */
        inline ExampleDevice1(const std::string& name, id_t id) :
            BaseExampleDevice1(name, id),
            //Registers configuration
            _goal("goal", (addr_t)4, 4, convIn1, convOut1, 0),
            _position("position", (addr_t)8, 4, convIn1, convOut1, 1),
            _temperature("temperature", (addr_t)16, 4, convIn1, convOut1, 4),
            //Parameters configuration
            _inverted("inverse", false),
            _zero("zero", 0.0),
            _mutex()
        {
        }

        /**
         * Assign target using current parameters
         */
        inline void setGoal(float angle)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_inverted.value) {
                _goal.writeValue(-angle - _zero.value);
            } else {
                _goal.writeValue(angle + _zero.value);
            }
        }

        /**
         * Return read value of position and temperature
         */
        inline ReadValueFloat getPosition()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _position.readValue();
        }
        inline ReadValueFloat getTemperature()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _temperature.readValue();
        }

        /**
         * Return read value of voltage
         */
        inline float getVoltage()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _voltage.readValue().value;
        }
        inline TimePoint getVoltageTimestamp()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _voltage.readValue().timestamp;
        }

        /**
         * Ask for read voltage at next flush()
         */
        inline void askVoltageRead()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _voltage.askRead();
        }

        /**
         * Parameters getter/setter
         * (thread safe)
         */
        inline float getInverted() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _inverted.value;
        }
        inline void setInverted(float val)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _inverted.value = val;
        }
        inline float getZero() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _zero.value;
        }
        inline void setZero(float val)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _zero.value = val;
        }

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::registersList().add(&_goal);
            Device::registersList().add(&_position);
            Device::registersList().add(&_temperature);
            Device::registersList().add(&_voltage);
            Device::parametersList().add(&_inverted);
            Device::parametersList().add(&_zero);
        }
        
    private:

        /**
         * Registers
         */
        TypedRegisterFloat _goal;
        TypedRegisterFloat _position;
        TypedRegisterFloat _temperature;

        /**
         * Parameters
         */
        ParameterBool _inverted;
        ParameterNumber _zero;

        /**
         * Mutex protecting parameters access
         */
        mutable std::mutex _mutex;
};

/**
 * DeviceManager specialized for ExampleDevice1
 */
template <>
class ImplManager<ExampleDevice1> : public TypedManager<ExampleDevice1>
{
    public:

        inline static type_t typeNumber() 
        {
            return 1;
        }

        inline static std::string typeName()
        {
            return "ExampleDevice1";
        }
};

}

