#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL {

/**
 * Convertion functions
 */
void convIn1(RhAL::data_t* buffer, float value);
float convOut1(const RhAL::data_t* buffer);

/**
 * BaseExampleDevice1
 *
 * Test base class for ExampleDevice1
 */
class BaseExampleDevice1 : public Device
{
    public:

        /**
         * Initialization
         */
        BaseExampleDevice1(const std::string& name, id_t id);

        /**
         * Register access
         */
        TypedRegisterFloat& voltage();

    protected:

        /**
         * Registers
         */
        TypedRegisterFloat _voltage;

        /**
         * Test Inherit
         */
        virtual void onSwap() override;
};

/**
 * ExampleDevice1
 *
 * Simple example device 
 * type implemetation
 */
class ExampleDevice1 : public BaseExampleDevice1
{
    public:

        /**
         * Initialization with name and id
         */
        ExampleDevice1(const std::string& name, id_t id);

        /**
         * Register access
         */
        TypedRegisterFloat& goal();
        TypedRegisterFloat& position();
        TypedRegisterFloat& temperature();

        /**
         * Parameters getter/setter
         * (thread safe)
         */
        float getInverted() const;
        void setInverted(float val);
        float getZero() const;
        void setZero(float val);

    protected:

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override;
        
    private:

        /**
         * Registers
         */
        TypedRegisterFloat _goal;
        TypedRegisterFloat _position;
        TypedRegisterFloat _temperature;

        /**
         * Parameters
         */
        ParameterBool _inverted;
        ParameterNumber _zero;

        /**
         * Mutex protecting parameters access
         */
        mutable std::mutex _mutex;
};

/**
 * DeviceManager specialized for ExampleDevice1
 */
template <>
class ImplManager<ExampleDevice1> : public TypedManager<ExampleDevice1>
{
    public:

        inline static type_t typeNumber() 
        {
            return 1;
        }

        inline static std::string typeName()
        {
            return "ExampleDevice1";
        }
};

}

