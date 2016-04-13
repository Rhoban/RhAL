#pragma once

#include <string>
#include <memory>
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
template <int GAUGES>
class PressureSensor : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        PressureSensor(const std::string& name, id_t id)
            : Device(name, id),
	      _led("led", 0x19, 1, convEncode_Bool, convDecode_Bool, 0)
        {
            for (unsigned int i=0; i<GAUGES; i++) {
                std::stringstream ss;
                ss << "pressure_" << i;
                _pressure.push_back(std::shared_ptr<TypedRegisterInt>(new TypedRegisterInt(ss.str(), 0x24+3*i, 3, [i, this](const data_t* data) -> int {
                    std::lock_guard<std::mutex> lock(this->_mutex);
                    int value = convDecode_3Bytes_signed(data);
                    return value - (int)this->_zero[i]->value;
                }, 1)));

                ss.clear();
                ss << "zero_" << i;
                _zero.push_back(std::shared_ptr<ParameterNumber>(new ParameterNumber(ss.str(), 0.0)));
            }
        }

        /**
         * Registers access
         */
        TypedRegisterInt& pressure(int index)
        {
            return *_pressure[index];
        }

        /**
         * Parameters zeros get/set
         */
        float getZero(int index) const
        {
            std::lock_guard<std::mutex> lock(_mutex);

            return _zero[index]->value;
        }

        void setZero(int index, float value)
        {
            std::lock_guard<std::mutex> lock(_mutex);

            _zero[index]->value = value;
        }

    protected:

        /**
         * Registers
         */
        //The following comments specify the register 
        //size and address in the hardware.
        std::vector<std::shared_ptr<TypedRegisterInt>> _pressure; // Starts at 0x24

        // Led
        TypedRegisterBool _led; // At 0x19

        /**
         * Parameters
         */
        std::vector<std::shared_ptr<ParameterNumber>> _zero;

        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override
        {
            Device::registersList().add(&_led);
            for (auto &reg : _pressure) {
                Device::registersList().add(reg.get());
            }
            for (auto &parameter : _zero) {
                Device::parametersList().add(parameter.get());
            }
        }
};

/**
 * DeviceManager specialized for PressureSensor
 */
template <int GAUGES>
class ImplManager<PressureSensor<GAUGES>> : public TypedManager<PressureSensor<GAUGES>>
{
    public:

        inline static type_t typeNumber()
        {
            return 5000+GAUGES;
        }

        inline static std::string typeName()
        {
            return "PressureSensor";
        }
};

extern template class PressureSensor<4>;
extern template class PressureSensor<8>;

typedef PressureSensor<4> PressureSensor4;
typedef PressureSensor<8> PressureSensor8;

}

