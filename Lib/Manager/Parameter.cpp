#include "Parameter.hpp"

namespace RhAL {

template <typename T>
Parameter<T>::Parameter(const std::string& name, const T& value) :
    name(name),
    defaultValue(value),
    value(value)
{
}

//Template explicite declaration
template class Parameter<bool>;
template class Parameter<double>;
template class Parameter<std::string>;

}

