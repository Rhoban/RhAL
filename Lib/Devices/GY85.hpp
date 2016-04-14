#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <cmath>
#include <type_traits>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "AHRS/Filter.hpp"

namespace RhAL {

/**
 * How many buffers there is in the IMU
 */
#define GY85_VALUES 5

/**
 * GY-85 Inertial Measurement Unit (IMU)
 */
class GY85 : public Device
{
    public:
        /**
         * A value in the device
         */
        struct GY85Value
        {
            std::shared_ptr<TypedRegisterFloat> accX;
            std::shared_ptr<TypedRegisterFloat> accY;
            std::shared_ptr<TypedRegisterFloat> accZ;
            std::shared_ptr<TypedRegisterFloat> gyroX;
            std::shared_ptr<TypedRegisterFloat> gyroY;
            std::shared_ptr<TypedRegisterFloat> gyroZ;
            std::shared_ptr<TypedRegisterFloat> magnX;
            std::shared_ptr<TypedRegisterFloat> magnY;
            std::shared_ptr<TypedRegisterFloat> magnZ;
            std::shared_ptr<TypedRegisterInt> sequence;
        };

        /**
         * Initialization with name and id
         */
        GY85(const std::string& name, id_t id);

        /**
         * Sets the filter callback
         */
        void setCallback(std::function<void()> callback);

        /**
         * Getters for current values
         */
        float getAccX();
        float getAccY();
        float getAccZ();
        float getGyroX();
        float getGyroY();
        float getGyroZ();
        float getMagnX();
        float getMagnY();
        float getMagnZ();

        /**
         * Filter output
         */
        float getYaw();
        float getPitch();
        float getRoll();
        float getGyroYaw();

    protected:
        /**
         * The IMU filter
         */
        AHRS::Filter filter;

        /**
         * A callback that is invoked after a filtering
         */
        std::function<void()> callback;

        /**
         * Last sensor values
         */
        uint32_t sequence;
        float accX, accY, accZ;
        float gyroX, gyroY, gyroZ;
        float magnX, magnY, magnZ;

        /**
         * Register
         */
        struct GY85Value values[GY85_VALUES];
        
        /**
         * Inherit.
         * Declare Registers and parameters
         */
        virtual void onInit() override;
        
        /**
         * On swap
         */
        virtual void onSwap() override;
};

/**
 * DeviceManager specialized for IMU
 */
template <>
class ImplManager<GY85> : public TypedManager<GY85>
{
    public:

        inline static type_t typeNumber()
        {
            return 350;
        }

        inline static std::string typeName()
        {
            return "GY85";
        }
};

}

