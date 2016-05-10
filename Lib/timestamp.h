#pragma once

#include "types.h"

namespace RhAL {

/**
 * Return the current date as TimePoint
 */
TimePoint getTimePoint();

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
//Explicit template instantiation
extern template TimeDurationMilli getTimeDuration<TimeDurationMilli>(const TimePoint&, const TimePoint&);
extern template TimeDurationMicro getTimeDuration<TimeDurationMicro>(const TimePoint&, const TimePoint&);
extern template TimeDurationSec getTimeDuration<TimeDurationSec>(const TimePoint&, const TimePoint&);
extern template TimeDurationFloat getTimeDuration<TimeDurationFloat>(const TimePoint&, const TimePoint&);

/**
 * Return the duration in given template
 * Duration type of given TimePoint since epoch
 */
template <typename T>
inline T getTimeDuration(const TimePoint& p1)
{
    return std::chrono::duration_cast<T>(p1.time_since_epoch());
}
//Explicit template instantiation
extern template TimeDurationMilli getTimeDuration<TimeDurationMilli>(const TimePoint& p1);
extern template TimeDurationMicro getTimeDuration<TimeDurationMicro>(const TimePoint& p1);
extern template TimeDurationSec getTimeDuration<TimeDurationSec>(const TimePoint& p1);
extern template TimeDurationFloat getTimeDuration<TimeDurationFloat>(const TimePoint& p1);

/**
 * Return the duration tick in micro/milli/ seconds
 * of given TimePoint since epoch or between the two
 * given TimePoint.
 */
TimeDurationMicro::rep duration_us(const TimePoint& p);
TimeDurationMicro::rep duration_us(const TimePoint& p1, const TimePoint& p2);
TimeDurationMilli::rep duration_ms(const TimePoint& p);
TimeDurationMilli::rep duration_ms(const TimePoint& p1, const TimePoint& p2);
TimeDurationSec::rep duration_s(const TimePoint& p);
TimeDurationSec::rep duration_s(const TimePoint& p1, const TimePoint& p2);
TimeDurationFloat::rep duration_float(const TimePoint& p);
TimeDurationFloat::rep duration_float(const TimePoint& p1, const TimePoint& p2);

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

//Explicit template instantiation
extern template TimeDurationMicro::rep duration_us<TimeDurationMilli>(const TimeDurationMilli& d);
extern template TimeDurationMicro::rep duration_us<TimeDurationMicro>(const TimeDurationMicro& d);
extern template TimeDurationMicro::rep duration_us<TimeDurationSec>(const TimeDurationSec& d);
extern template TimeDurationMicro::rep duration_us<TimeDurationFloat>(const TimeDurationFloat& d);
extern template TimeDurationMilli::rep duration_ms<TimeDurationMilli>(const TimeDurationMilli& d);
extern template TimeDurationMilli::rep duration_ms<TimeDurationMicro>(const TimeDurationMicro& d);
extern template TimeDurationMilli::rep duration_ms<TimeDurationSec>(const TimeDurationSec& d);
extern template TimeDurationMilli::rep duration_ms<TimeDurationFloat>(const TimeDurationFloat& d);
extern template TimeDurationSec::rep duration_s<TimeDurationMilli>(const TimeDurationMilli& d);
extern template TimeDurationSec::rep duration_s<TimeDurationMicro>(const TimeDurationMicro& d);
extern template TimeDurationSec::rep duration_s<TimeDurationSec>(const TimeDurationSec& d);
extern template TimeDurationSec::rep duration_s<TimeDurationFloat>(const TimeDurationFloat& d);
extern template TimeDurationFloat::rep duration_float<TimeDurationMilli>(const TimeDurationMilli& d);
extern template TimeDurationFloat::rep duration_float<TimeDurationMicro>(const TimeDurationMicro& d);
extern template TimeDurationFloat::rep duration_float<TimeDurationSec>(const TimeDurationSec& d);
extern template TimeDurationFloat::rep duration_float<TimeDurationFloat>(const TimeDurationFloat& d);

}

