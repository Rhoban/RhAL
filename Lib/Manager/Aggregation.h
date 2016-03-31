#pragma once

namespace RhAL {

/**
 * All value Aggregation policy
 */
enum AggregationPolicy {
    //Keep the last written value
    AggregateLast,
    //Keep the first written value
    AggregateFirst,
    //Sum all values
    AggregateSum,
    //Keep the maximum value
    AggregateMax,
    //Keep the minimum value
    AggregateMin,
};

/**
 * Policies implementation
 */
namespace Impl {
    template <typename T>
    inline T implAggregateLast(T oldValue, T newValue)
    {
        (void)oldValue;
        return newValue;
    }
    template <typename T>
    inline T implAggregateFirst(T oldValue, T newValue)
    {
        (void)newValue;
        return oldValue;
    }
    template <typename T>
    inline T implAggregateSum(T oldValue, T newValue)
    {
        return oldValue + newValue;
    }
    template <typename T>
    inline T implAggregateMax(T oldValue, T newValue)
    {
        return (oldValue > newValue) ? oldValue : newValue;
    }
    template <typename T>
    inline T implAggregateMin(T oldValue, T newValue)
    {
        return (oldValue < newValue) ? oldValue : newValue;
    }
    /**
     * Specialization for boolean
     */
    template <>
    inline bool implAggregateSum(bool oldValue, bool newValue)
    {
        return oldValue || newValue;
    }
    template <>
    inline bool implAggregateMax(bool oldValue, bool newValue)
    {
        return oldValue || newValue;
    }
    template <>
    inline bool implAggregateMin(bool oldValue, bool newValue)
    {
        return oldValue && newValue;
    }
}

/**
 * Return the aggregated value for given policy 
 * with given old aggregated value and 
 * new written value
 */
template <typename T>
T aggregateValue(AggregationPolicy policy, T oldValue, T newValue);

}

