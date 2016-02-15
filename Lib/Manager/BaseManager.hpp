#pragma once

namespace RhAL {

#include <unordered_map>
#include <string>
#include "Device.hpp"
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

        /**
         * Return a Device with given id or name 
         * (all derived types are searched).
         * Throw std::logic_error if asked Device
         * with given type is not found.
         */
        //Overloaded shortcuts
        inline const Device& dev(id_t id) const
        {
            return devById(id);
        }
        inline Device& dev(id_t id)
        {
            return devById(id);
        }
        inline const Device& dev(const std::string& name) const
        {
            return devByName(name);
        }
        inline Device& dev(const std::string& name)
        {
            return devByName(name);
        }
        //Implentations
        inline const Device& devById(id_t id) const
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "BaseManager Device id not found: " 
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline Device& devById(id_t id)
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "BaseManager Device id not found: " 
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline const Device& devByName(const std::string& name) const
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "BaseManager Device name not found: " 
                    + name);
            }
            return *(_devicesByName.at(name));
        }
        inline Device& devByName(const std::string& name)
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "BaseManager Device name not found: " 
                    + name);
            }
            return *(_devicesByName.at(name));
        }
        
        /**
         * Return true if a device is already contained with
         * given name or id for all Device types
         */
        //Overload shortcuts
        inline bool devExists(id_t id) const
        {
            return devExistsById(id);
        }
        inline bool devExists(const std::string& name) const
        {
            return devExistsByName(name);
        }
        //Implementations
        inline bool devExistsById(id_t id) const
        {
            return (_devicesById.count(id) > 0);
        }
        inline bool devExistsByName(const std::string& name) const
        {
            return (_devicesByName.count(name) > 0);
        }
        
        /**
         * Access to internal map of pointers 
         * to all contained Devices for all types.
         * Device are indexed by their name.
         */
        inline const DevicesByName<Device>& devContainer() const
        {
            return _devicesByName;
        }
        
    protected:
        
        /**
         * Device container indexed by
         * their name and their id
         */
        DevicesByName<Device> _devicesByName;
        DevicesById<Device> _devicesById;
};

}

