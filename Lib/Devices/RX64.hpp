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
            _goalPos("goalPos", 0X1E, 2, convEncode_RXPos, convDecode_RXPos, 0),
            _position("pos", 0X24, 2, convEncode_RXPos, convDecode_RXPos, 1),
			_torqueEnable("torqueEnable", 0X18, 1, convIn_Default<bool>, convOut_Default<bool>, 0)
        {
        }

        /**
         * Inherit.
         * Set the target motor 
         * position in radians
         */
        virtual void setGoalPos(float angle) override
        {
            _goalPos.writeValue(angle);
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

        virtual void enableTorque() override
		{
			_torqueEnable.writeValue(true);
		}

		virtual void disableTorque() override
		{
			_torqueEnable.writeValue(false);
		}
    
    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
            Device::registersList().add(&_goalPos);
            Device::registersList().add(&_position);
            Device::registersList().add(&_torqueEnable);
        }

        /**
         * Encoding for RX position values using
         * 1024 max representation.
         */
        inline void convEncode_RXPos(data_t* buffer, float value)
        {
            if (value > Deg2Rad(150)) value = Deg2Rad(150);
            if (value < -Deg2Rad(150)) value = -Deg2Rad(150);
            value += Deg2Rad(150);
            value *= 1023/Deg2Rad(300);
            if (value < 0.0) value = 0.0;
            if (value > 1023.0) value = 1023.0;
            uint16_t v = std::lround(value);
            write2BytesToBuffer(buffer, v);
        }
        inline float convDecode_RXPos(const data_t* buffer)
        {
            uint16_t val = read2BytesFromBuffer(buffer);
            float value = val;
            return value*Deg2Rad(300)/1023 - Deg2Rad(150);
        }

    private:

        /**
         * Register
         */
        TypedRegisterFloat _goalPos;
        TypedRegisterFloat _position;
        TypedRegisterFloat _torqueEnable;
};

/**
 * DeviceManager specialized for RX64
 */
template <>
class ImplManager<RX64> : public BaseManager<RX64>
{
    public:

        inline static type_t typeNumber() 
        {
            return 0x0040;
        }

        inline static std::string typeName()
        {
            return "RX64";
        }
};

}

