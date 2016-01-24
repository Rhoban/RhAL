#pragma once

#include <mutex>
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
            //Registers configuration
            _pitch("pitch", (addr_t)4, 4, convIn2, convOut2, 2),
            _roll("roll", (addr_t)8, 4, convIn2, convOut2, 0, true, false),
            _mode("mode", (addr_t)12, 4, convIn2, convOut2, 0, false, true)
        {
        }

        /**
         * Force immediate read of pitch and returned it
         */
        inline TimedValueFloat forcePitchRead()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _pitch.forceRead();
            return _pitch.readValue();
        }

        /**
         * Return current pitch value
         */
        inline TimedValueFloat getPitch()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _pitch.readValue();
        }

        /**
         * Read roll (immediate roll)
         */
        inline TimedValueFloat getRoll()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _roll.readValue();
        }
        
        /**
         * Set mode (imediate write)
         */
        inline void setMode(float mode)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _mode.writeValue(mode);
        }

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::registersList().add(&_pitch);
            Device::registersList().add(&_roll);
            Device::registersList().add(&_mode);
        }
        
    private:

        /**
         * Registers
         */
        TypedRegisterFloat _pitch;
        TypedRegisterFloat _roll;
        TypedRegisterFloat _mode;

        /**
         * Mutex protecting access
         */
        mutable std::mutex _mutex;
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

