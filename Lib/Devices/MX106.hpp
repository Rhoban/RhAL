#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
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
        inline MX106(const std::string& name, id_t id) :
            MX(name, id),
			_current("current", 0x44, 2, convEncode_Current, convDecode_Current, 0),
			_torqueControlModeEnable("torqueControlModeEnable", 0x46, 1, convEncode_Bool, convDecode_Bool, 0),
			_goalTorque("goalTorque", 0x47, 2, convEncode_GoalTorque, convDecode_GoalTorque, 0)
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
        	Device::registersList().add(&_current);
        	Device::registersList().add(&_torqueControlModeEnable);
        	Device::registersList().add(&_goalTorque);
        }

        /**
         * Register
         */
        //The following comments specify the register size and address in the hardware. A '*' means that the register is not present in the all of the MX children.
		TypedRegisterFloat 	_current;					//2 44
		TypedRegisterBool	_torqueControlModeEnable;	//1 46
		TypedRegisterFloat 	_goalTorque;				//2 47

};

/**
 * DeviceManager specialized for MX106
 */
template <>
class ImplManager<MX106> : public BaseManager<MX106>
{
    public:

        inline static type_t typeNumber()
        {
            return 0x0164;
        }

        inline static std::string typeName()
        {
            return "MX106";
        }
};

}

