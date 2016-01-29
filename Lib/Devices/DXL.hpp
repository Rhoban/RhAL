#pragma once

#include <string>
#include <mutex>
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL {

/**
 * Radian to Degree and
 * Degree to Radian conversion
 */
constexpr inline float Rad2Deg(float a)
{
    return a*180.0/M_PI;
}
constexpr inline float Deg2Rad(float a)
{
    return a*M_PI/180.0;
}

/**
 * Write/Read to/from given 
 * data buffer given word value
 */
inline void writeWordToBuffer(data_t* buffer, uint16_t value)
{
    *(buffer) = (value & 0xFF);
    *(buffer + 1) = ((value >> 8) & 0xFF);
}
inline uint16_t readWordFromBuffer(const data_t* buffer)
{
    uint16_t val;
    val = (*(buffer + 1) << 8) | (*(buffer) & 0x00FF);
    return val;
}

/**
 * Conversion from typed value to buffer (in)
 * and buffer to typed value (out) 
 * for RX position values using 
 * 1024 max representation.
 */
inline void convIn_RXPos(data_t* buffer, float value)
{
    if (value > Deg2Rad(150)) value = Deg2Rad(150);
    if (value < -Deg2Rad(150)) value = -Deg2Rad(150);
    value += Deg2Rad(150);
    value *= 1023/Deg2Rad(300);
    if (value < 0.0) value = 0.0;
    if (value > 1023.0) value = 1023.0;
    uint16_t v = std::lround(value);
    writeWordToBuffer(buffer, v);
}
inline float convOut_RXPos(const data_t* buffer)
{
    uint16_t val = readWordFromBuffer(buffer);
    float value = val;
    return value*Deg2Rad(300)/1023 - Deg2Rad(150);
}

/**
 * Default raw copy conversion
 */
template <typename T>
inline void convIn_Default(data_t* buffer, T value)
{
	*(reinterpret_cast<T*>(buffer)) = value;
}
template <typename T>
inline T convOut_Default(const data_t* buffer)
{
	return *(reinterpret_cast<const T*>(buffer));
}


/**
 * Conversion from typed value to buffer (in)
 * and buffer to typed value (out) 
 * for MX position values using 
 * 4096 max representation.
 */
inline void convIn_MXPos(data_t* buffer, float value)
{
    if (value > Deg2Rad(180)) value = Deg2Rad(180);
    if (value < -Deg2Rad(180)) value = -Deg2Rad(180);
    value += Deg2Rad(180);
    value *= 4095/Deg2Rad(360);
    if (value < 0.0) value = 0.0;
    if (value > 4095.0) value = 4095.0;
    uint16_t v = std::lround(value);
    writeWordToBuffer(buffer, v);
}
inline float convOut_MXPos(const data_t* buffer)
{
    uint16_t val = readWordFromBuffer(buffer);
    float value = val;
    return value*Deg2Rad(360)/4095 - Deg2Rad(180);
}

/**
 * DXL
 *
 * Robotis Dynamixel servo motor
 * abstract base class
 */
class DXL : public Device
{
    public:

        /**
         * Initialization with name and id
         */
        inline DXL(const std::string& name, id_t id) :
            Device(name, id)
        {
        }

        /**
         * Set the target motor 
         * position in radians
         */
        virtual void setGoalPos(float angle) = 0;

        /**
         * Set the target motor 
         * position in degrees
         */
        inline void setGoalPosDegree(float angle)
        {
            setGoalPos(Deg2Rad(angle));
        }

        /**
         * Retrieve the current motor 
         * position in radians
         */
        virtual float getPos() = 0;

        /**
         * Retrieve the current motor 
         * position in degrees
         */
        inline float getPosDegree()
        {
            return Rad2Deg(getPos());
        }

        virtual void enableTorque() = 0;
        virtual void disableTorque() = 0;
};

}

