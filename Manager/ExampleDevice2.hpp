#pragma once

#include "BaseManager.hpp"
#include "Device.hpp"
#include "Register.hpp"

namespace RhAL {

/**
 * ExampleDevice2
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice2 : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        inline ExampleDevice2(const std::string& name, id_t id) :
            Device(name, id)
        {
        }
        
        /**
         * Return the device model number
         * and textual name
         */
        virtual type_t typeNumber() const override
        {
            return 0x02;
        }
        virtual std::string typeName() const override
        {
            return "ExampleDevice2";
        }

    protected:
        
        /**
         * Declare all parameters
         */
        virtual void initParameters() override
        {
            Device::parameters().addNumber("offset", 0.0);
            Device::parameters().addStr("prefix", "");
        }


        /**
         * Declare all registers
         */
        virtual void initRegisters() override
        {
            Device::addRegister(new Register("force", 0x02, 4, true));
            Device::addRegister(new Register("enable", 0x08, 1, true));
        }

    private:

};

/**
 * DeviceManager specialized for ExampleDevice2
 */
template <>
class ImplManager<ExampleDevice2> : public BaseManager<ExampleDevice2>
{
    public:

    private:
};

}

