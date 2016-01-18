#pragma once

#include <string>

namespace RhAL {

/**
 * Parameter
 *
 * Device named parameter
 * expected to be used with types
 * bool, float and string
 */
template <typename T>
struct Parameter
{
    const std::string name;
    const T defaultValue;
    T value;

    /**
     * Initialization with parameter 
     * name and default value
     */
    inline Parameter(const std::string& name, const T& value) :
        name(name),
        defaultValue(value),
        value(value)
    {
    }
};

/**
 * Typedef for Parameters
 */
typedef Parameter<bool> ParameterBool;
typedef Parameter<double> ParameterNumber;
typedef Parameter<std::string> ParameterStr;

}

