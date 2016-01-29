#pragma once

#include <chrono>

namespace RhAL {

/**
 * Device unique id
 */
typedef int id_t;

/**
 * Static lower and upper
 * bound on Device id use to
 * iterate over all possible Device
 * while scanning
 */
constexpr id_t IdDevBegin = 1;
constexpr id_t IdDevEnd = 253;

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
 * Static register address of model 
 * number on all Devices
 */
constexpr addr_t AddrDevTypeNumber = 0x00;

/**
 * Device model number
 */
typedef uint16_t type_t;

/**
 * Raw data
 */
typedef uint8_t data_t;

/**
 * Timestamp
 */
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

/**
 * Duration in seconds represented by a double
 */
typedef std::chrono::duration<double, std::ratio<1>> DurationDouble;


/**
 * Return the current date as TimePoint
 */
inline TimePoint getTimePoint()
{
    return std::chrono::steady_clock::now();
}

///**
// * Returns the current system time in seconds
// */
//inline double getTimeDouble()
//{
//    TimePoint tp = getTimePoint();
//
//    return std::chrono::duration_cast<std::chrono::nanoseconds>(
//        tp.time_since_epoch()).count()/1000000000.0;
//
//}

inline DurationDouble getDuration(TimePoint t0, TimePoint t1)
{
	return std::chrono::duration_cast<DurationDouble>(t1 - t0);
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

