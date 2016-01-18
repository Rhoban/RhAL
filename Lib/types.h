#pragma once

#include <chrono>

namespace RhAL {

/**
 * Device unique id
 */
typedef int id_t;

/**
 * Device register address
 */
typedef size_t addr_t;

/**
 * Static all Device address space length.
 * Use to allocate memory inside Device.
 */
constexpr size_t AddrDevLen = 0xFF;

/**
 * Device model number
 */
typedef int type_t;

/**
 * Raw data
 */
typedef uint8_t data_t;

/**
 * Timestamp
 */
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

/**
 * Return the current date as TimePoint
 */
inline TimePoint getTimePoint()
{
    return std::chrono::steady_clock::now();
}

/**
 * Returns the current system time in seconds
 */
inline double getTimeDouble()
{
    TimePoint tp = getCurrentTimePoint();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        tp.time_since_epoch()).count()/1000000000.0;
}

/**
 * A value associated with 
 * a timestamp
 */
template <typename T>
struct TimedValue {
    const TimePoint timestamp;
    const T value;

    //Simple initialization constructor
    TimedValue(const TimePoint& time, const T& val) :
        timestamp(time),
        value(val)
    {
    }
};

/**
 * Typedef for TimedValue
 */
typedef TimedValue<bool> TimedValueBool;
typedef TimedValue<int> TimedValueInt;
typedef TimedValue<float> TimedValueFloat;

}

