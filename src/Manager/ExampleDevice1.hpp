#pragma once

#include "BaseManager.hpp"
#include "Device.hpp"
#include "Register.hpp"
#include "Parameter.hpp"

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
 * ExampleDevice1
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice1 : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        inline ExampleDevice1(const std::string& name, id_t id) :
            Device(name, id),
            _position("position", 0x04, 4, convIn1, convOut1, 1),
            _goal("goal", 0x08, 4, convIn1, convOut1, 0),
            _temperature("temperature", 0x10, 4, convIn1, convOut1, 4),
            _inverted("inverse", false),
            _zero("zero", 0.0)
        {
        }

        inline void setGoal(float angle)
        {
            if (_inverted.value) {
                _goal.writeValue(-angle - _zero.value);
            } else {
                _goal.writeValue(angle + _zero.value);
            }
        }
        inline TimedValueFloat getPosition() const
        {
            return _position.readValue();
        }
        inline TimedValueFloat getTemperature() const
        {
            return _temperature.readValue();
        }

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::_registersList.add(&_position);
            Device::_registersList.add(&_goal);
            Device::_registersList.add(&_temperature);
            Device::_parametersList.add(&_inverted);
            Device::_parametersList.add(&_zero);
        }
        
    private:

        /**
         * Registers
         */
        TypedRegisterFloat _position;
        TypedRegisterFloat _goal;
        TypedRegisterFloat _temperature;

        /**
         * Parameters
         */
        ParameterBool _inverted;
        ParameterNumber _zero;
};

/**
 * DeviceManager specialized for ExampleDevice1
 */
template <>
class ImplManager<ExampleDevice1> : public BaseManager<ExampleDevice1>
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

