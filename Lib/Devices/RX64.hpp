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
 * RX64
 *
 * Dynamixel RX-64 Device 
 * implementation
 */
class RX64 : public DXL
{
    public:

        /**
         * Initialization with name and id
         */
        inline RX64(const std::string& name, id_t id) :
            DXL(name, id),
            _goal("goal", 0X1E, 2, convIn_RXPos, convOut_RXPos, 0),
            _position("pos", 0X24, 2, convIn_RXPos, convOut_RXPos, 1)
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

}

