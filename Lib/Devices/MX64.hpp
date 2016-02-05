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
 * MX64
 *
 * Dynamixel MX-64 Device 
 * implementation
 */
class MX64 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        inline MX64(const std::string& name, id_t id) :
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
        }

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

