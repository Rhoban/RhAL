#pragma once

#include <string>
#include <stdexcept>

namespace std {
    /**
     * Overload std::to_string for
     * non printable types
     */
    template <typename T>
    string to_string(T a) {
        (void)a;
        return "[non-printable]";
    }
    /**
     * Overload std::to_string for
     * boolean types
     */
    template <>
    string to_string(bool a) {
        if (a) {
            return "true";
        } else {
            return "false";
        }
    }
}

/**
 * Utils assert functions
 */
inline void assertEquals(const std::string& a, const char* b)
{
    if (a != b) {
        throw std::logic_error("Assert equals fail: " 
            + a 
            + " != " 
            + std::string(b));
    }
}
template <typename T>
inline void assertEquals(T a, T b)
{
    if (a != b) {
        throw std::logic_error("Assert equals fail: " 
            + std::to_string(a) 
            + " != " 
            + std::to_string(b));
    }
}

