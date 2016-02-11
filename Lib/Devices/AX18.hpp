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
 * AX18
 *
 * Dynamixel AX-18 Device
 * implementation
 */
class AX18 : public RX
{
    public:

        /**
         * Initialization with name and id
         */
        inline AX18(const std::string& name, id_t id) :
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
 * DeviceManager specialized for AX18
 */
template <>
class ImplManager<AX18> : public BaseManager<AX18>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0012;
        }

        inline static std::string typeName()
        {
            return "AX18";
        }
};

}

