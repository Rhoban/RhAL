#pragma once

#include <map>
#include <string>
#include <stdexcept>
#include <json/json.h>
#include "types.h"
#include "timestamp.h"
#include "CallManager.hpp"
#include "Parameter.hpp"
#include "ParametersList.hpp"

namespace RhAL
{
/**
 * TypedManager
 *
 * Base template class for Device
 * type specialized manager.
 * T is the Device derived class
 */
template <typename T>
class TypedManager
{
public:
  /**
   * Typedef for device container
   */
  typedef std::map<std::string, T*> DevsByName;
  typedef std::map<id_t, T*> DevsById;

  /**
   * Initialization
   */
  inline TypedManager() : _paramsList(), _devsByName(), _devsById()
  {
  }

  /**
   * Devices deallocation
   */
  inline virtual ~TypedManager()
  {
    for (auto& dev : _devsByName)
    {
      delete dev.second;
      dev.second = nullptr;
    }
    for (auto& dev : _devsById)
    {
      dev.second = nullptr;
    }
  }

  /**
   * Copy and assignment are forbidden
   */
  TypedManager(const TypedManager&) = delete;
  TypedManager& operator=(const TypedManager&) = delete;

  /**
   * Add a new Device with given
   * name and id.
   * Only Manaegr thread must call this method.
   * Throw std::logic_error if given name or id
   * is already contained.
   */
  inline void devAdd(const std::string& name, id_t id)
  {
    if (_devsByName.count(name) != 0 || _devsById.count(id) != 0)
    {
      throw std::logic_error("TypedManager device name or id already added: " + name);
    }
    T* dev = new T(name, id);
    _devsByName[name] = dev;
    _devsById[id] = dev;
  }

  /**
   * Return a derided Device by its
   * id or name.
   * Throw std::logic_error if asked Device
   * is not contained.
   */
  inline const T& devById(id_t id) const
  {
    if (!devExistsById(id))
    {
      throw std::logic_error("TypedManager device id does not exists: " + std::to_string(id));
    }
    return *(_devsById.at(id));
  }
  inline T& devById(id_t id)
  {
    if (!devExistsById(id))
    {
      throw std::logic_error("TypedManager device id does not exists: " + std::to_string(id));
    }
    return *(_devsById.at(id));
  }
  inline const T& devByName(const std::string& name) const
  {
    if (!devExistsByName(name))
    {
      throw std::logic_error("TypedManager device name does not exists: " + name);
    }
    return *(_devsByName.at(name));
  }
  inline T& devByName(const std::string& name)
  {
    if (!devExistsByName(name))
    {
      throw std::logic_error("TypedManager device name does not exists: " + name);
    }
    return *(_devsByName.at(name));
  }

  /**
   * Return true if asked name or id
   * Device is contained
   */
  inline bool devExistsById(id_t id) const
  {
    return _devsById.count(id) != 0;
  }
  inline bool devExistsByName(const std::string& name) const
  {
    return _devsByName.count(name) != 0;
  }

  /**
   * Direct read access to Device container
   * by indexed name for Device iteration
   */
  inline const DevsByName& devContainer() const
  {
    return _devsByName;
  }

  /**
   * Export and return the base container
   * parameters and all devices parameters
   * into a json object
   */
  inline Json::Value saveJSON() const
  {
    Json::Value j;
    j["parameters"] = _paramsList.saveJSON();
    j["devices"] = Json::Value();
    for (const auto& it : _devsById)
    {
      Json::Value obj;
      obj["id"] = it.second->id();
      obj["name"] = it.second->name();
      obj["parameters"] = it.second->parametersList().saveJSON();
      j["devices"].append(obj);
    }

    return j;
  }

  /**
   * Import parameters from given json object.
   * Devices listed in json not already present
   * are added.
   * All listed Devices are expected to exist.
   * Throw std::runtime_error if given json is malformated.
   */
  inline void loadJSON(const Json::Value& j)
  {
    const Json::Value& parameters = j["parameters"];
    if (!parameters.isNull())
    {
      _paramsList.loadJSON(parameters);
    }
    // Iterate over devices
    const Json::Value& devices = j["devices"];
    if (devices.isNull())
    {
      throw std::out_of_range("RhAL::TypedManager::loadJson: Could not find value 'devices'");
    }
    if (!devices.isArray())
    {
      throw std::runtime_error("RhAL::TypedManager::loadJson: 'devices' is not an array");
    }
    for (Json::Value::ArrayIndex i = 0; i < devices.size(); i++)
    {
      const Json::Value& dev = devices[i];
      const Json::Value& id_v = dev["id"];
      if (id_v.isNull())
      {
        throw std::out_of_range("RhAL::TypedManager::loadJson: Could not find 'id' in device '" + std::to_string(i) +
                                "'");
      }
      if (!id_v.isInt())
      {
        throw std::out_of_range("RhAL::TypedManager::loadJson: 'id' in device '" + std::to_string(i) +
                                "' is not an int");
      }
      // Retrieve device id
      id_t id = id_v.asInt();
      // Load parameters
      const Json::Value& dev_parameters = dev["parameters"];
      if (!dev_parameters.isNull())
      {
        _devsById.at(id)->parametersList().loadJSON(dev_parameters);
      }
    }
  }

  /**
   * Read/Write access to Parameters list
   */
  const ParametersList& parametersList() const
  {
    return _paramsList;
  }
  ParametersList& parametersList()
  {
    return _paramsList;
  }

private:
  /**
   * Container of bool, number and
   * string device parameters
   */
  ParametersList _paramsList;

  /**
   * Device container indexed by
   * their name and their id
   */
  DevsByName _devsByName;
  DevsById _devsById;
};

/**
 * ImplManager
 *
 * This template is specialized for each
 * implemented Device type
 */
template <typename T>
class ImplManager;

}  // namespace RhAL
