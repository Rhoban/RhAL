#pragma once

namespace RhAL {

#include <unordered_map>
#include "CallManager.hpp"

/**
 * BaseManager
 *
 * Implement all Device access non 
 * derived Device specific
 */
class BaseManager : public CallManager
{
    public:
        
        /**
         * Typedef for device container
         */
        template <typename T>
        using DevicesByName = 
            std::unordered_map<std::string, T*>;
        template <typename T>
        using DevicesById = 
            std::unordered_map<id_t, T*>; 

    protected:

    private:
};

}

