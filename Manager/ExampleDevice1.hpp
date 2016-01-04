#pragma once

#include "BaseManager.hpp"
#include "Device.hpp"
#include "Register.hpp"

namespace RhAL {

/**
 * ExampleDevice1
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice1 : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        inline ExampleDevice1(const std::string& name, id_t id) :
            Device(name, id)
        {
        }
        
        /**
         * Return the device model number
         * and textual name
         */
        virtual type_t typeNumber() const override
        {
            return 0x01;
        }
        virtual std::string typeName() const override
        {
            return "ExampleDevice1";
        }

    protected:
        
        /**
         * Declare all parameters
         */
        virtual void initParameters() override
        {
            Device::parameters().addBool("inverse", false);
            Device::parameters().addNumber("zero", 2.0);
        }


        /**
         * Declare all registers
         */
        virtual void initRegisters() override
        {
            Device::addRegister(new Register("pos", 0x02, 4, false));
            Device::addRegister(new Register("goal", 0x06, 4, true));
        }

    private:

};

/**
 * DeviceManager specialized for ExampleDevice1
 */
template <>
class ImplManager<ExampleDevice1> : public BaseManager<ExampleDevice1>
{
    public:

    private:
};

}

