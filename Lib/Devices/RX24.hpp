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
 * RX24
 *
 * Dynamixel RX-24 Device
 * implementation
 */
class RX24 : public RX
{
    public:

        /**
         * Initialization with name and id
         */
        RX24(const std::string& name, id_t id);
};

/**
 * DeviceManager specialized for RX24
 */
template <>
class ImplManager<RX24> : public TypedManager<RX24>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x18;
        }

        inline static std::string typeName()
        {
            return "RX24";
        }
};

}

