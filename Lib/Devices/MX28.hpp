#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/MX.hpp"

namespace RhAL {

/**
 * MX28
 *
 * Dynamixel MX-28 Device
 * implementation
 */
class MX28 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        inline MX28(const std::string& name, id_t id) :
            MX(name, id)
        {
        }
    protected :
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        inline virtual void onInit() override
        {
        	MX::onInit();
        }

};

/**
 * DeviceManager specialized for MX28
 */
template <>
class ImplManager<MX28> : public BaseManager<MX28>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x001D;
        }

        inline static std::string typeName()
        {
            return "MX28";
        }
};

}

