#include "Aggregation.h"

namespace RhAL {

template <typename T>
T aggregateValue(AggregationPolicy policy, T oldValue, T newValue)
{
    switch (policy) {
        case AggregateLast: 
            return Impl::implAggregateLast(oldValue, newValue); 
            break;
        case AggregateFirst: 
            return Impl::implAggregateFirst(oldValue, newValue); 
            break;
        case AggregateSum: 
            return Impl::implAggregateSum(oldValue, newValue); 
            break;
        case AggregateMax: 
            return Impl::implAggregateMax(oldValue, newValue); 
            break;
        case AggregateMin: 
            return Impl::implAggregateMin(oldValue, newValue); 
            break;
        default: 
            return T();
            break;
    }
}

//Template explicite instantiation
template bool aggregateValue<bool>(AggregationPolicy, bool, bool);
template int aggregateValue<int>(AggregationPolicy, int, int);
template float aggregateValue<float>(AggregationPolicy, float, float);

}

