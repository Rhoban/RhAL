#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/RX.hpp"

namespace RhAL {


/**
 * RX28
 *
 * Dynamixel RX-28 Device
 * implementation
 */
class RX28 : public RX
{
    public:

        /**
         * Initialization with name and id
         */
        inline RX28(const std::string& name, id_t id) :
            RX(name, id)
        {
        }
    protected :
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
        	RX::onInit();
        }

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the RX children.

};

/**
 * DeviceManager specialized for RX28
 */
template <>
class ImplManager<RX28> : public BaseManager<RX28>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x001C;
        }

        inline static std::string typeName()
        {
            return "RX28";
        }
};

}

