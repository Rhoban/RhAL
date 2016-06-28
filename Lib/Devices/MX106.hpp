#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/MX.hpp"
#include "Devices/MX64.hpp"

namespace RhAL {

/**
 * MX106
 *
 * Dynamixel MX-106 Device
 * implementation
 */
class MX106 : public MX
{
    public:

        /**
         * Initialization with name and id
         */
        MX106(const std::string& name, id_t id);

        /**
         * Registers access
         */
        TypedRegisterFloat& current();
        TypedRegisterBool& torqueControlModeEnable();
        TypedRegisterFloat& goalTorque();

    protected :

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override;

        /**
         * Register
         */
        //The following comments specify the register size and address 
        //in the hardware. A '*' means that the register is not present 
        //in the all of the MX children.
        TypedRegisterFloat 	_current;                    //2 44
        TypedRegisterBool	_torqueControlModeEnable;    //1 46
        TypedRegisterFloat 	_goalTorque;                 //2 47
        TypedRegisterInt 	_driveMode;                 //1 0x0A

};

/**
 * DeviceManager specialized for MX106
 */
template <>
class ImplManager<MX106> : public TypedManager<MX106>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0140;
        }

        inline static std::string typeName()
        {
            return "MX106";
        }
};

}

