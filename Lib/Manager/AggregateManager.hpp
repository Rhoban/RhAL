#pragma once

#include <vector>
#include <json.hpp>
#include "CallManager.hpp"
#include "BaseManager.hpp"
#include "Device.hpp"

namespace RhAL {

/**
 * AggregateManager
 *
 * Gather all derived Device container.
 * All suported derived Device types are given
 * by the variadic template parameters.
 * Add, check, access, iterate over Devices.
 */
template <typename ... Types> 
class AggregateManager : public CallManager, public ImplManager<Types>...
{
    //Assert that variadic template given Types
    //is not empty
    static_assert(sizeof...(Types) != 0, 
        "AggregateManager empty variatic template types");

    public:

        /**
         * Add and initialize a new derived Device 
         * of given template type with given name and id.
         * Throw std::logic_error if given name or id
         * is already contained.
         */
        template <typename T>
        inline void devAdd(const std::string& name, id_t id)
        {
            if (devExistsByName(name) || devExistsById(id)) {
                throw std::logic_error(
                    "AggregateManager device name or id already added: " 
                    + name);
            } else {
                //Add and initialize the new device.
                //Registers are supposed to be initialized
                //in the constructor.
                ImplManager<T>::devAdd(name, id, this);
            }
        }

        /**
         * Return the model number or the 
         * model name of given template Device
         * derived type
         */
        template <typename T>
        inline type_t typeNumber() const
        {
            return ImplManager<T>::typeNumber();
        }
        template <typename T>
        inline std::string typeName() const
        {
            return ImplManager<T>::typeName();
        }

        /**
         * Return the model number or model name
         * of given derived Device by its id or name.
         * Throw std::logic_error if asked Device
         * is not found.
         */
        inline type_t typeNumberById(id_t id) const
        {
            return Impl<Types...>::runTypeNumberById(this, id);
        }
        inline type_t typeNumberByName(const std::string& name) const
        {
            return Impl<Types...>::runTypeNumberByName(this, name);
        }
        inline std::string typeNameById(id_t id) const
        {
            return Impl<Types...>::runTypeNameById(this, id);
        }
        inline std::string typeNameByName(const std::string& name) const
        {
            return Impl<Types...>::runTypeNameByName(this, name);
        }

        /**
         * Return a derived Device of given 
         * template type by its id or name.
         * Throw std::logic_error if asked Device
         * with given type is not found.
         */
        template <typename T>
        inline const T& devById(id_t id) const
        {
            return ImplManager<T>::devById(id);
        }
        template <typename T>
        inline T& devById(id_t id)
        {
            return ImplManager<T>::devById(id);
        }
        template <typename T>
        inline const T& devByName(const std::string& name) const
        {
            return ImplManager<T>::devByName(name);
        }
        template <typename T>
        inline T& devByName(const std::string& name)
        {
            return ImplManager<T>::devByName(name);
        }

        /**
         * Return a Device with given id or name 
         * (all derived types are searched).
         * Throw std::logic_error if asked Device
         * with given type is not found.
         */
        inline const Device& devById(id_t id) const
        {
            return Impl<Types...>::runById(this, id);
        }
        inline Device& devById(id_t id)
        {
            return Impl<Types...>::runById(this, id);
        }
        inline const Device& devByName(const std::string& name) const
        {
            return Impl<Types...>::runByName(this, name);
        }
        inline Device& devByName(const std::string& name)
        {
            return Impl<Types...>::runByName(this, name);
        }
        
        /**
         * Return true if a device of given template type T 
         * is already contained with given name or id
         */
        template <typename T>
        inline bool devExistsById(id_t id) const
        {
            return ImplManager<T>::devExistsById(id);
        }
        template <typename T>
        inline bool devExistsByName(const std::string& name) const
        {
            return ImplManager<T>::devExistsByName(name);
        }

        /**
         * Return true if a device is already contained with
         * given name or id for all Device types
         */
        inline bool devExistsById(id_t id) const
        {
            return Impl<Types...>::runExistsById(this, id);
        }
        inline bool devExistsByName(const std::string& name) const
        {
            return Impl<Types...>::runExistsByName(this, name);
        }

        /**
         * Return a Vector of pointer to all contained 
         * derived Device of given template type.
         * Note that this method is not greatly efficient.
         */
        template <typename T>
        inline std::vector<const T*> devAll() const
        {
            std::vector<const T*> ptrs;
            for (const auto& it : ImplManager<T>::devContainer()) {
                ptrs.push_back(it.second);
            }
            return ptrs;
        }

        /**
         * Return a Vector of pointer to all contained
         * Device for all types.
         * Note that this method is not greatly efficient.
         */
        inline std::vector<const Device*> devAll() const
        {
            std::vector<const Device*> ptrs;
            Impl<Types...>::runList(this, ptrs);
            return ptrs;
        }

    protected:
        
        /**
         * Export and return all aggregated 
         * derived Device container parameters
         * into a json object
         */
        inline nlohmann::json saveAggregatedJSON() const
        {
            nlohmann::json j;
            Impl<Types...>::runSaveJSON(this, j);
            return j;
        }

        /**
         * Import parameters and Devices from given
         * json object.
         * Throw std::runtime_error if given json is malformated.
         */
        inline void loadAggregatedJSON(const nlohmann::json& j)
        {
            Impl<Types...>::runLoadJSON(this, j);
        }

    private:

        /**
         * Implementation of iteration over contained
         * types using template variadic parameters and
         * specialization of structure
         */
        //Structure declaration
        template <typename ... Ts>
        struct Impl;
        //Final case single element
        template <typename T>
        struct Impl<T> {
            //Type number by Id
            inline static type_t runTypeNumberById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "AggregateManager type Device id not found: " 
                        + std::to_string(id));
                }
                return ImplManager<T>::typeNumber();
            }
            //Type name by Id
            inline static std::string runTypeNameById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "AggregateManager type Device id not found: " 
                        + std::to_string(id));
                }
                return ImplManager<T>::typeName();
            }
            //Type number by Name
            inline static type_t runTypeNumberByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "AggregateManager type Device name not found: " 
                        + name);
                }
                return ImplManager<T>::typeNumber();
            }
            //Type name by Name
            inline static std::string runTypeNameByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "AggregateManager type Device name not found: " 
                        + name);
                }
                return ImplManager<T>::typeName();
            }
            //Get by Id
            inline static const Device& runById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "AggregateManager Device id not found: " 
                        + std::to_string(id));
                }
                return ptr->devById<T>(id);
            }
            inline static Device& runById(
                AggregateManager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "AggregateManager Device id not found: " 
                        + std::to_string(id));
                }
                return ptr->devById<T>(id);
            }
            //Get by Name
            inline static const Device& runByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "AggregateManager Device name not found: " 
                        + name);
                }
                return ptr->devByName<T>(name);
            }
            inline static Device& runByName(
                AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "AggregateManager Device name not found: " 
                        + name);
                }
                return ptr->devByName<T>(name);
            }
            //Exists by Id
            inline static bool runExistsById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                return ptr->devExistsById<T>(id);
            }
            //Exists by Name
            inline static bool runExistsByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                return ptr->devExistsByName<T>(name);
            }
            //List
            inline static void runList(
                const AggregateManager<Types...>* ptr, 
                std::vector<const Device*>& vect)
            {
                for (const auto& it : ptr->ImplManager<T>::devContainer()) {
                    vect.push_back(it.second);
                }
            }
            //Save JSON
            inline static void runSaveJSON(
                const AggregateManager<Types...>* ptr, 
                nlohmann::json& j)
            {
                j[ImplManager<T>::typeName()] = 
                    ptr->ImplManager<T>::saveJSON();
            }
            //Load JSON
            inline static void runLoadJSON(
                AggregateManager<Types...>* ptr, 
                const nlohmann::json& j)
            {
                if (j.count(ImplManager<T>::typeName()) != 1) {
                    throw std::runtime_error(
                        "AggregateManager load parameters no Device type: "
                        + ImplManager<T>::typeName());
                }
                //Check json format and 
                //add non existing devices
                ptr->checkDevicesJSON<T>(
                    j.at(ImplManager<T>::typeName()));
                //Load parameters
                ptr->ImplManager<T>::loadJSON(
                    j.at(ImplManager<T>::typeName()));
            }
        };
        //General iteration case
        template <typename T, typename ... Ts>
        struct Impl<T, Ts...> {
            //Type number by Id
            inline static type_t runTypeNumberById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ImplManager<T>::typeNumber();
                } else {
                    return Impl<Ts...>::runTypeNumberById(ptr, id);
                }
            }
            //Type name by Id
            inline static std::string runTypeNameById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ImplManager<T>::typeName();
                } else {
                    return Impl<Ts...>::runTypeNameById(ptr, id);
                }
            }
            //Type number by Name
            inline static type_t runTypeNumberByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ImplManager<T>::typeNumber();
                } else {
                    return Impl<Ts...>::runTypeNumberByName(ptr, name);
                }
            }
            //Type name by Name
            inline static std::string runTypeNameByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ImplManager<T>::typeName();
                } else {
                    return Impl<Ts...>::runTypeNameByName(ptr, name);
                }
            }
            //Get by Id
            inline static const Device& runById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ptr->devById<T>(id);
                } else {
                    return Impl<Ts...>::runById(ptr, id);
                }
            }
            inline static Device& runById(
                AggregateManager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ptr->devById<T>(id);
                } else {
                    return Impl<Ts...>::runById(ptr, id);
                }
            }
            //Get by Name
            inline static const Device& runByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ptr->devByName<T>(name);
                } else {
                    return Impl<Ts...>::runByName(ptr, name);
                }
            }
            inline static Device& runByName(
                AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ptr->devByName<T>(name);
                } else {
                    return Impl<Ts...>::runByName(ptr, name);
                }
            }
            //Exists by Id
            inline static bool runExistsById(
                const AggregateManager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return true;
                } else {
                    return Impl<Ts...>::runExistsById(ptr, id);
                }
            }
            //Exists by Name
            inline static bool runExistsByName(
                const AggregateManager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return true;
                } else {
                    return Impl<Ts...>::runExistsByName(ptr, name);
                }
            }
            //List
            inline static void runList(
                const AggregateManager<Types...>* ptr, 
                std::vector<const Device*>& vect)
            {
                for (const auto& it : ptr->ImplManager<T>::devContainer()) {
                    vect.push_back(it.second);
                }
                Impl<Ts...>::runList(ptr, vect);
            }
            //Save JSON
            inline static void runSaveJSON(
                const AggregateManager<Types...>* ptr, 
                nlohmann::json& j)
            {
                j[ImplManager<T>::typeName()] = 
                    ptr->ImplManager<T>::saveJSON();
                Impl<Ts...>::runSaveJSON(ptr, j);
            }
            //Load JSON
            inline static void runLoadJSON(
                AggregateManager<Types...>* ptr, 
                const nlohmann::json& j)
            {
                if (j.count(ImplManager<T>::typeName()) != 1) {
                    throw std::runtime_error(
                        "AggregateManager load parameters no Device type: "
                        + ImplManager<T>::typeName());
                }
                //Check json format and 
                //add non existing devices
                ptr->checkDevicesJSON<T>(
                    j.at(ImplManager<T>::typeName()));
                //Load parameters
                ptr->ImplManager<T>::loadJSON(
                    j.at(ImplManager<T>::typeName()));
                //Continue on other types
                Impl<Ts...>::runLoadJSON(ptr, j);
            }
        };

        /**
         * Check in given json object configuration
         * of a derived (template) Device type that json
         * is well formated and add or check
         * listed devices
         */
        template <typename T>
        inline void checkDevicesJSON(const nlohmann::json& j)
        {
            //Check json type
            if (!j.is_object()) {
                throw std::runtime_error(
                    "AggregateManager load parameters json not object");
            }
            //Load BaseManager parameters
            if (j.count("parameters") != 1) {
                throw std::runtime_error(
                    "AggregateManager load parameters no parameters key");
            }
            //Check device exists and type
            if (j.count("devices") != 1) {
                throw std::runtime_error(
                    "AggregateManager load parameters no devices key");
            }
            if (!j.at("devices").is_array()) {
                throw std::runtime_error(
                    "AggregateManager load parameters devices not array");
            }
            //Check no other key exists
            if (j.size() != 2) {
                throw std::runtime_error(
                    "AggregateManager load parameters json malformed");
            }
            //Iterate over devices
            for (size_t i=0;i<j.at("devices").size();i++) {
                const nlohmann::json& dev = j.at("devices").at(i);
                //Check json formated
                if (!dev.is_object()) {
                    throw std::runtime_error(
                        "AggregateManager load parameters device not object");
                }
                if (
                    dev.count("id") != 1 || 
                    dev.count("name") != 1 ||
                    dev.count("parameters") != 1 ||
                    !dev.at("id").is_number() ||
                    !dev.at("name").is_string() ||
                    dev.size() != 3
                ) {
                    throw std::runtime_error(
                        "AggregateManager load parameters device json malformated");
                }
                //Retrieve device id and name
                id_t id = dev.at("id");
                std::string name = dev.at("name");
                //Check device exist
                bool isById = devExistsById(id);
                bool isByName = devExistsByName(name);
                bool isByIdTyped = devExistsById<T>(id);
                bool isByNameTyped = devExistsByName<T>(name);
                if (!isById && !isByName) {
                    //Create it if not exist
                    devAdd<T>(name, id);
                } else if (
                    //Else, device with same id/name exists in 
                    //other derived type or there is an id/name mismatch
                    (isById && !isByName) || (!isById && isByName) ||
                    !isByIdTyped || !isByNameTyped
                ) {
                    throw std::runtime_error(
                        "AggregateManager load parameters device id/name problem: "
                        + name);
                } 
            }
        }
};

}

