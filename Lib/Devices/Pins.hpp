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
                _pins("pins", 0x24, 1, convEncode_1Byte, convDecode_1Byte, 10, false)
        {
        }
            
        inline void getPins(bool pins[7])
        {
            uint8_t value = _pins.readValue().value;
            for (int i = 0; i < 7; i++) {
            	if (value & (1<<i)) {
            		pins[i] = true;
            	} else {
            		pins[i] = false;
            	}
            }
        }

        protected:
            /**
             * Register
             */
            //The following comments specify the register size and address in the hardware.
            TypedRegisterInt 	_pins;   			//1 0x24

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

