#include "utils.h"

namespace RhAL {

TimePoint getTimePoint()
{
    return std::chrono::steady_clock::now();
}

TimeDurationMicro::rep duration_us(const TimePoint& p)
{
    return getTimeDuration<TimeDurationMicro>(p).count();
}
TimeDurationMicro::rep duration_us(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationMicro>(p1, p2).count();
}
TimeDurationMilli::rep duration_ms(const TimePoint& p)
{
    return getTimeDuration<TimeDurationMilli>(p).count();
}
TimeDurationMilli::rep duration_ms(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationMilli>(p1, p2).count();
}
TimeDurationSec::rep duration_s(const TimePoint& p)
{
    return getTimeDuration<TimeDurationSec>(p).count();
}
TimeDurationSec::rep duration_s(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationSec>(p1, p2).count();
}
TimeDurationFloat::rep duration_float(const TimePoint& p)
{
    return getTimeDuration<TimeDurationFloat>(p).count();
}
TimeDurationFloat::rep duration_float(const TimePoint& p1, const TimePoint& p2)
{
    return getTimeDuration<TimeDurationFloat>(p1, p2).count();
}

template TimeDurationMilli getTimeDuration<TimeDurationMilli>(const TimePoint&, const TimePoint&);
template TimeDurationMicro getTimeDuration<TimeDurationMicro>(const TimePoint&, const TimePoint&);
template TimeDurationSec getTimeDuration<TimeDurationSec>(const TimePoint&, const TimePoint&);
template TimeDurationFloat getTimeDuration<TimeDurationFloat>(const TimePoint&, const TimePoint&);

template TimeDurationMilli getTimeDuration<TimeDurationMilli>(const TimePoint& p1);
template TimeDurationMicro getTimeDuration<TimeDurationMicro>(const TimePoint& p1);
template TimeDurationSec getTimeDuration<TimeDurationSec>(const TimePoint& p1);
template TimeDurationFloat getTimeDuration<TimeDurationFloat>(const TimePoint& p1);

template TimeDurationMicro::rep duration_us<TimeDurationMilli>(const TimeDurationMilli& d);
template TimeDurationMicro::rep duration_us<TimeDurationMicro>(const TimeDurationMicro& d);
template TimeDurationMicro::rep duration_us<TimeDurationSec>(const TimeDurationSec& d);
template TimeDurationMicro::rep duration_us<TimeDurationFloat>(const TimeDurationFloat& d);
template TimeDurationMilli::rep duration_ms<TimeDurationMilli>(const TimeDurationMilli& d);
template TimeDurationMilli::rep duration_ms<TimeDurationMicro>(const TimeDurationMicro& d);
template TimeDurationMilli::rep duration_ms<TimeDurationSec>(const TimeDurationSec& d);
template TimeDurationMilli::rep duration_ms<TimeDurationFloat>(const TimeDurationFloat& d);
template TimeDurationSec::rep duration_s<TimeDurationMilli>(const TimeDurationMilli& d);
template TimeDurationSec::rep duration_s<TimeDurationMicro>(const TimeDurationMicro& d);
template TimeDurationSec::rep duration_s<TimeDurationSec>(const TimeDurationSec& d);
template TimeDurationSec::rep duration_s<TimeDurationFloat>(const TimeDurationFloat& d);
template TimeDurationFloat::rep duration_float<TimeDurationMilli>(const TimeDurationMilli& d);
template TimeDurationFloat::rep duration_float<TimeDurationMicro>(const TimeDurationMicro& d);
template TimeDurationFloat::rep duration_float<TimeDurationSec>(const TimeDurationSec& d);
template TimeDurationFloat::rep duration_float<TimeDurationFloat>(const TimeDurationFloat& d);

}

