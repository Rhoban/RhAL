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

