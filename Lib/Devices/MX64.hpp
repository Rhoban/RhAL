#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/DXL.hpp"

namespace RhAL {

/**
 * MX64
 *
 * Dynamixel MX-64 Device 
 * implementation
 */
class MX64 : public DXL
{
    public:

        /**
         * Initialization with name and id
         */
        inline MX64(const std::string& name, id_t id) :
            DXL(name, id),
            _goal("goal", 0X1E, 2, convIn_MXPos, convOut_MXPos, 0),
            _position("pos", 0X24, 2, convIn_MXPos, convOut_MXPos, 1)
        {
        }

        /**
         * Inherit.
         * Set the target motor 
         * position in radians
         */
        virtual void setGoal(float angle) override
        {
            _goal.writeValue(angle);
        }
        
        /**
         * Inherit.
         * Retrieve the current motor 
         * position in radians
         */
        virtual float getPos() override
        {
            return _position.readValue().value;
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
        }

    private:

        /**
         * Register
         */
        TypedRegisterFloat _goal;
        TypedRegisterFloat _position;
};

/**
 * DeviceManager specialized for MX64
 */
template <>
class ImplManager<MX64> : public BaseManager<MX64>
{
    public:

        inline static type_t typeNumber() 
        {
            return 0x0136;
        }

        inline static std::string typeName()
        {
            return "MX64";
        }
};

}

