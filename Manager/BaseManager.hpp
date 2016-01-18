#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "types.h"
#include "CallManager.hpp"

namespace RhAL {

/**
 * BaseManager
 *
 * Base template class for Device 
 * type specialized manager.
 * T is the Device derived class
 */
template <typename T>
class BaseManager
{
    public:

        /**
         * Typedef for device container
         */
        typedef std::unordered_map<std::string, T*> 
            DevicesByName;
        typedef std::unordered_map<id_t, T*> 
            DevicesById;

        /**
         * Initialization
         */
        inline BaseManager() :
            _devicesByName(),
            _devicesById()
        {
        }

        /**
         * Devices deallocation
         */
        inline virtual ~BaseManager()
        {
            for (auto& dev : _devicesByName) {
                delete dev.second;
                dev.second = nullptr;
            }
            for (auto& dev : _devicesById) {
                dev.second = nullptr;
            }
        }

        /**
         * Copy and assignment are forbidden
         */
        BaseManager(const BaseManager&) = delete;
        BaseManager& operator=(const BaseManager&) = delete;

        /**
         * Add and initialize a new Device with given
         * name, id and manager pointer instance.
         * Throw std::logic_error if given name or id
         * is already contained.
         */
        inline void devAdd(const std::string& name, id_t id, 
            CallManager* manager)
        {
            if (
                _devicesByName.count(name) != 0 ||
                _devicesById.count(id) != 0
            ) {
                throw std::logic_error(
                    "BaseManager device name or id already added: " 
                    + name);
            }
            T* dev = new T(name, id);
            _devicesByName[name] = dev;
            _devicesById[id] = dev;
            //Inject Manager pointer dependancy
            dev->setManager(manager);
            //Run Parameters and Registers initialization
            dev->init();
        }

        /**
         * Return a derided Device by its
         * id or name.
         * Throw std::logic_error if asked Device
         * is not contained.
         */
        inline const T& devById(id_t id) const
        {
            if (!devExistsById(id)) {
                throw std::logic_error(
                    "BaseManager device id does not exists: " 
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline T& devById(id_t id)
        {
            if (!devExistsById(id)) {
                throw std::logic_error(
                    "BaseManager device id does not exists: " 
                    + std::to_string(id));
            }
            return *(_devicesById.at(id));
        }
        inline const T& devByName(const std::string& name) const
        {
            if (!devExistsByName(name)) {
                throw std::logic_error(
                    "BaseManager device name does not exists: " 
                    + name);
            }
            return *(_devicesByName.at(name));
        }
        inline T& devByName(const std::string& name)
        {
            if (!devExistsByName(name)) {
                throw std::logic_error(
                    "BaseManager device name does not exists: " 
                    + name);
            }
            return *(_devicesByName.at(name));
        }

        /**
         * Return true if asked name or id 
         * Device is contained
         */
        inline bool devExistsById(id_t id) const
        {
            return _devicesById.count(id) != 0;
        }
        inline bool devExistsByName(const std::string& name) const
        {
            return _devicesByName.count(name) != 0;
        }

        /**
         * Direct read access to Device container
         * by indexed name for Device iteration
         */
        inline const DevicesByName& devContainer() const
        {
            return _devicesByName;
        }

    private:

        /**
         * Device container indexed by
         * their name and their id
         */
        DevicesByName _devicesByName;
        DevicesById _devicesById;
};

/**
 * ImplManager
 *
 * This template is specialized for each
 * implemented Device type
 */
template <typename T>
class ImplManager;

}

