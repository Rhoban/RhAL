#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include <math.h>
#include <type_traits>

namespace RhAL {
    /**
     * This device read pins, it is basically used to read buttons from a board
     */
    class Pins : public Device
    {
        public:
            /**
             * Initialization with name and id
             */
            inline Pins(const std::string& name, id_t id) :
                Device(name, id),
                _pins("pins", 0x24, 1, convEncode_1Byte, convDecode_1Byte, 1)
        {
        }
            
        inline float getPins(int pin)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return (_pins.readValue().value >> pin) != 0;
        }

        protected:
            /**
             * Register
             */
            //The following comments specify the register size and address in the hardware.
            TypedRegisterInt 	_pins;   			//3 0x24

            /**
             * Inherit.
             * Declare Registers and parameters
             */
            inline virtual void onInit() override
            {
                Device::registersList().add(&_pins);
            }

    };

    /**
     * DeviceManager specialized for Pins
     */
    template <>
        class ImplManager<Pins> : public TypedManager<Pins>
        {
            public:

                inline static type_t typeNumber()
                {
                    return 6000;
                }

                inline static std::string typeName()
                {
                    return "Pins";
                }
        };
}

