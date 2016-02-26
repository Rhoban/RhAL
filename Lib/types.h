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
 * Number of milliseconds that will be waited
 * after writing to a slow register
 */
constexpr unsigned int SlowRegisterDelayMs = 100;

/**
 * Number of consecutive trials when trying
 * to immediate read before abording
 */
constexpr unsigned int MaxForceReadTries = 20;

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
 * Duration typedef in micro, milli
 * and seconds
 */
typedef std::chrono::milliseconds TimeDurationMilli;
typedef std::chrono::microseconds TimeDurationMicro;
typedef std::chrono::seconds TimeDurationSec;
typedef std::chrono::duration<double, std::ratio<1>> TimeDurationFloat;

/**
 * Return the current date as TimePoint
 */
inline TimePoint getTimePoint()
{
    return std::chrono::steady_clock::now();
}

/**
 * Return the duration between two given
 * TimePoint from p1 to p2 in given template
 * Duration type
 */
template <typename T>
inline T getTimeDuration(const TimePoint& p1, const TimePoint& p2)
{
    return std::chrono::duration_cast<T>(p2 - p1);
}

/**
 * Return the duration in given template
 * Duration type of given TimePoint since epoch
 */
template <typename T>
inline T getTimeDuration(const TimePoint& p1)
{
    return std::chrono::duration_cast<T>(p1.time_since_epoch());
}

/**
 * Return the duration tick in micro/milli/ seconds
 * of given TimePoint since epoch or between the two
 * given TimePoint.
 */
inline TimeDurationMicro::rep duration_us(const TimePoint& p)
{
    return getTimeDuration<TimeDurationMicro>(p).count();
}
inline TimeDurationMicro::rep duration_us(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationMicro>(p1, p2).count();
}
inline TimeDurationMilli::rep duration_ms(const TimePoint& p)
{
    return getTimeDuration<TimeDurationMilli>(p).count();
}
inline TimeDurationMilli::rep duration_ms(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationMilli>(p1, p2).count();
}
inline TimeDurationSec::rep duration_s(const TimePoint& p)
{
    return getTimeDuration<TimeDurationSec>(p).count();
}
inline TimeDurationSec::rep duration_s(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationSec>(p1, p2).count();
}
inline TimeDurationFloat::rep duration_float(const TimePoint& p)
{
    return getTimeDuration<TimeDurationFloat>(p).count();
}
inline TimeDurationFloat::rep duration_float(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationFloat>(p1, p2).count();
}

/**
 * Return the duration tick in micro/milli/ seconds
 * of given Duration
 */
template <typename T>
inline TimeDurationMicro::rep duration_us(const T& d)
{
    return std::chrono::duration_cast<TimeDurationMicro>(d).count();
}
template <typename T>
inline TimeDurationMilli::rep duration_ms(const T& d)
{
    return std::chrono::duration_cast<TimeDurationMilli>(d).count();
}
template <typename T>
inline TimeDurationSec::rep duration_s(const T& d)
{
    return std::chrono::duration_cast<TimeDurationSec>(d).count();
}
template <typename T>
inline TimeDurationFloat::rep duration_float(const T& d)
{
    return std::chrono::duration_cast<TimeDurationFloat>(d).count();
}

/**
 * A value associated with
 * a timestamp and an error flag
 */
template <typename T>
struct ReadValue {
    //Last valid read
    const TimePoint timestamp;
    //Current read value
    const T value;
    //Is last read operation has failed
    const bool isError;

    //Simple initialization constructor
    ReadValue(const TimePoint& time, const T& val, bool flag) :
        timestamp(time),
        value(val),
        isError(flag)
    {
    }
};

/**
 * Typedef for ReadValue
 */
typedef ReadValue<bool> ReadValueBool;
typedef ReadValue<int> ReadValueInt;
typedef ReadValue<float> ReadValueFloat;

}
