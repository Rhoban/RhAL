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
 * MX12
 *
 * Dynamixel MX-28 Device
 * implementation
 */
class MX12 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        inline MX12(const std::string& name, id_t id) :
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
 * DeviceManager specialized for MX12
 */
template <>
class ImplManager<MX12> : public BaseManager<MX12>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x168;
        }

        inline static std::string typeName()
        {
            return "MX12";
        }
};

}

