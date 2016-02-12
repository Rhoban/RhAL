#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/RX.hpp"

namespace RhAL {


/**
 * AX12
 *
 * Dynamixel AX-12 Device
 * implementation
 */
class AX12 : public RX
{
    public:

        /**
         * Initialization with name and id
         */
        inline AX12(const std::string& name, id_t id) :
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
 * DeviceManager specialized for AX12
 */
template <>
class ImplManager<AX12> : public TypedManager<AX12>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x000C;
        }

        inline static std::string typeName()
        {
            return "AX12";
        }
};

}

