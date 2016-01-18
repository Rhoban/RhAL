#pragma once

#include "BaseManager.hpp"
#include "Device.hpp"
#include "Register.hpp"
#include "Parameter.hpp"

namespace RhAL {

inline void convIn2(RhAL::data_t* buffer, float value)
{
    *(reinterpret_cast<float*>(buffer)) = value;
}
inline float convOut2(const RhAL::data_t* buffer)
{
   return  *(reinterpret_cast<const float*>(buffer));
}

/**
 * ExampleDevice2
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice2 : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        inline ExampleDevice2(const std::string& name, id_t id) :
            Device(name, id),
            _pitch("pitch", 0x08, 4, convIn2, convOut2, 2),
            _roll("roll", 0x0C, 4, convIn2, convOut2, 2),
            _mode("mode", 0x10, 4, convIn2, convOut2, 0)
        {
        }

        inline void setMode(float mode)
        {
            _mode.writeValue(mode);
        }
        inline TimedValueFloat getPitch() const
        {
            return _pitch.readValue();
        }
        inline TimedValueFloat getRoll() const
        {
            return _roll.readValue();
        }

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::_registersList.add(&_pitch);
            Device::_registersList.add(&_roll);
            Device::_registersList.add(&_mode);
        }
        
    private:

        /**
         * Registers
         */
        TypedRegisterFloat _pitch;
        TypedRegisterFloat _roll;
        TypedRegisterFloat _mode;
};

/**
 * DeviceManager specialized for ExampleDevice2
 */
template <>
class ImplManager<ExampleDevice2> : public BaseManager<ExampleDevice2>
{
    public:
        
        inline static type_t typeNumber() 
        {
            return 2;
        }

        inline static std::string typeName()
        {
            return "ExampleDevice2";
        }
};

}

