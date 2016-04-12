#pragma once

#include <string>
#include <mutex>
#include <cmath>
#include <type_traits>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL {

/**
 * PressureSensor
 */
class PressureSensor : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        PressureSensor(const std::string& name, id_t id);

        /**
         * Registers access
         */
        TypedRegisterInt& pressure1();
        TypedRegisterInt& pressure2();
        TypedRegisterInt& pressure3();
        TypedRegisterInt& pressure4();

        /**
         * Parameters zeros get/set
         */
        float getZero1() const;
        void setZero1(float value);
        float getZero2() const;
        void setZero2(float value);
        float getZero3() const;
        void setZero3(float value);
        float getZero4() const;
        void setZero4(float value);

    protected:

        /**
         * Registers
         */
        //The following comments specify the register 
        //size and address in the hardware.
        TypedRegisterInt _pressure1; //3 0x24
        TypedRegisterInt _pressure2; //3 0x27
        TypedRegisterInt _pressure3; //3 0x2A
        TypedRegisterInt _pressure4; //3 0x2D

        /**
         * Parameters
         */
        ParameterNumber _zero1;
        ParameterNumber _zero2;
        ParameterNumber _zero3;
        ParameterNumber _zero4;

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override;
};

/**
 * DeviceManager specialized for PressureSensor
 */
template <>
class ImplManager<PressureSensor> : public TypedManager<PressureSensor>
{
    public:

        inline static type_t typeNumber()
        {
            return 5000;
        }

        inline static std::string typeName()
        {
            return "PressureSensor";
        }
};

}

