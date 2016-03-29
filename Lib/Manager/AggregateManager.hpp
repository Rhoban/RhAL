#pragma once

#include <json.hpp>
#include <type_traits>
#include "Device.hpp"
#include "TypedManager.hpp"
#include "BaseManager.hpp"

namespace RhAL {

/**
 * Trait type used to check at
 * compile time if the specific 
 * type T is listed in the variadic 
 * type list Types. 
 */
//Trait structure is declared
template <typename T, typename ... Types>
struct is_type_in_pack : std::true_type {};
//Specialization to iterate over Types pack
template <typename T, typename U, typename ... Types>
struct is_type_in_pack<T, U, Types...> : 
    //Conditional inheritance
    std::conditional<
        //If type T and U are the same
        std::is_same<T, U>::value,
        //Inherit from true_type
        std::true_type,
        //Else inherit from recursive case
        is_type_in_pack<T, Types...>
    >::type {};
//Final case if all types have been listed
//without finding T
template <typename T>
struct is_type_in_pack<T> : std::false_type {};

/**
 * AggregateManager
 *
 * Gather all derived Device container.
 * All suported derived Device types are given
 * by the variadic template parameters.
 * Add, check, access, iterate over Devices.
 */
template <typename ... Types> 
class AggregateManager : public BaseManager, public ImplManager<Types>...
{
    //Assert that variadic template given Types
    //is not empty
    static_assert(sizeof...(Types) != 0, 
        "AggregateManager empty variatic template types");

    public:
        
        /**
         * Typedef for devById/Name() methods return type. 
         * Function return type is U given type.
         * Methods overload are selected whenether T is part
         * (or not) of Types (variadic) list.
         */
        template <typename T, typename U>
        using TypeInPack = typename std::enable_if<
            is_type_in_pack<T, Types...>::value, 
            U
        >;
        template <typename T, typename U>
        using TypeNotInPack = typename std::enable_if<
            !is_type_in_pack<T, Types...>::value, 
            U
        >;

        /**
         * Import BaseManager overloaded methods 
         * because of compiler deduction issues
         */
        using BaseManager::dev;
        using BaseManager::devById;
        using BaseManager::devByName;
        using BaseManager::devExists;
        using BaseManager::devExistsById;
        using BaseManager::devExistsByName;
        using BaseManager::devContainer;

        /**
         * Add and initialize a new derived Device 
         * of given template type with given name and id.
         * DevAdd() have to be called by Manager thread.
         * Throw std::logic_error if given name or id
         * is already contained.
         */
        template <typename T>
        inline void devAdd(id_t id, const std::string& name)
        {
            if (devExistsByName(name) || devExistsById(id)) {
                throw std::logic_error(
                    "AggregateManager device name or id already added: " 
                    + name);
            } else {
                //Add and initialize the new device.
                //Registers are supposed to be initialized
                //in the constructor.
                ImplManager<T>::devAdd(name, id);
                //Retrive added Device pointer
                Device* dev = &(devById<T>(id));
                //Add to AggregateManager container
                //for fast id/name retrieving
                _devicesById[id] = dev;
                _devicesByName[name] = dev;
                //Inject Manager pointer dependancy
                dev->setManager(this);
                //Run Parameters and Registers initialization
                dev->init();
            }
        }

        /**
         * Get access to a specific Manager
         * of given template derived Device type.
         */
        template <typename T>
        const ImplManager<T>& manager() const
        {
            return *this;
        }
        template <typename T>
        ImplManager<T>& manager()
        {
            return *this;
        }

        /**
         * Add and initialize a new Device 
         * with given id. Name is generated.
         * The type of the Device is defined 
         * by given type model number.
         * Throw std::logic_error if the Device
         * id/name already exists or if given type
         * is not supported by the manager
         */
        inline virtual void devAddByTypeNumber(
            id_t id, type_t type) override
        {
            Impl<Types...>::runAddByType(this, id, type);
        }

        /**
         * Return true if given Device type model
         * number is supported by the manager
         */
        inline virtual bool isTypeSupported(type_t type) const override
        {
            return Impl<Types...>::runIsTypeSupported(type);
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
        //Overloaded shortcuts
        inline type_t devTypeNumber(id_t id) const
        {
            return devTypeNumberById(id);
        }
        inline type_t devTypeNumber(const std::string& name) const
        {
            return devTypeNumberByName(name);
        }
        inline std::string devTypeName(id_t id) const
        {
            return devTypeNameById(id);
        }
        inline std::string devTypeName(const std::string& name) const
        {
            return devTypeNameByName(name);
        }
        //Implementations
        inline virtual type_t devTypeNumberById(id_t id) const override
        {
            return Impl<Types...>::runTypeNumberById(this, id);
        }
        inline type_t devTypeNumberByName(const std::string& name) const
        {
            return Impl<Types...>::runTypeNumberByName(this, name);
        }
        inline virtual std::string devTypeNameById(id_t id) const override
        {
            return Impl<Types...>::runTypeNameById(this, id);
        }
        inline std::string devTypeNameByName(const std::string& name) const
        {
            return Impl<Types...>::runTypeNameByName(this, name);
        }

        /**
         * Return a derived Device of given 
         * template type T by its id.
         * Throw std::logic_error if asked Device
         * with given type is not found.
         *
         * If given template type is one of declared
         * derived Device type in Manager instantiation,
         * direct access is done.
         * Else, dynamic cast is tryed in case of
         * given type T is a base class of actual
         * derived Device type.
         *
         * Throw std::logic_error if dynamic cast fails.
         */
        //Overloaded shortcuts
        template <typename T>
        inline const T& dev(id_t id) const
        {
            return devById<T>(id);
        }
        template <typename T>
        inline T& dev(id_t id)
        {
            return devById<T>(id);
        }
        //Implementations
        //Return const T&
        template <typename T>
        inline typename TypeInPack<T, const T&>::type devById(id_t id) const
        {
            return ImplManager<T>::devById(id);
        }
        //Return T&
        template <typename T>
        inline typename TypeInPack<T, T&>::type devById(id_t id)
        {
            return ImplManager<T>::devById(id);
        }
        //Return const T&
        template <typename T>
        inline typename TypeNotInPack<T, const T&>::type devById(id_t id) const
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "AggregateManager Device id not found: " 
                    + std::to_string(id));
            }
            try {
                const T& ref = dynamic_cast<const T&>(*_devicesById.at(id));
                return ref;
            } catch (const std::bad_cast& e) {
                throw std::logic_error(
                    "AggregateManager bad Device cast: " 
                    + std::to_string(id));
            }
        }
        //Return T&
        template <typename T>
        inline typename TypeNotInPack<T, T&>::type devById(id_t id)
        {
            if (_devicesById.count(id) == 0) {
                throw std::logic_error(
                    "AggregateManager Device id not found: " 
                    + std::to_string(id));
            }
            try {
                T& ref = dynamic_cast<T&>(*_devicesById.at(id));
                return ref;
            } catch (const std::bad_cast& e) {
                throw std::logic_error(
                    "AggregateManager bad Device cast: " 
                    + std::to_string(id));
            }
        }
        
        /**
         * Return a derived Device of given 
         * template type T by its name.
         * Throw std::logic_error if asked Device
         * with given type is not found.
         *
         * If given template type is one of declared
         * derived Device type in Manager instantiation,
         * direct access is done.
         * Else, dynamic cast is tryed in case of
         * given type T is a base class of actual
         * derived Device type.
         *
         * Throw std::logic_error if dynamic cast fails.
         */
        //Overloaded shortcuts
        template <typename T>
        inline const T& dev(const std::string& name) const
        {
            return devByName<T>(name);
        }
        template <typename T>
        inline T& dev(const std::string& name)
        {
            return devByName<T>(name);
        }
        //Implementations
        //Return const T&
        template <typename T>
        inline typename TypeInPack<T, const T&>::type devByName(
            const std::string& name) const
        {
            return ImplManager<T>::devByName(name);
        }
        //Return T&
        template <typename T>
        inline typename TypeInPack<T, T&>::type devByName(
            const std::string& name)
        {
            return ImplManager<T>::devByName(name);
        }
        //Return const T&
        template <typename T>
        inline typename TypeNotInPack<T, const T&>::type devByName(
            const std::string& name) const
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "AggregateManager Device name not found: " 
                    + name);
            }
            try {
                const T& ref = dynamic_cast<const T&>(*_devicesByName.at(name));
                return ref;
            } catch (const std::bad_cast& e) {
                throw std::logic_error(
                    "AggregateManager bad Device cast: " 
                    + name);
            }
        }
        //Return T&
        template <typename T>
        inline typename TypeNotInPack<T, T&>::type devByName(
            const std::string& name)
        {
            if (_devicesByName.count(name) == 0) {
                throw std::logic_error(
                    "AggregateManager Device name not found: " 
                    + name);
            }
            try {
                T& ref = dynamic_cast<T&>(*_devicesByName.at(name));
                return ref;
            } catch (const std::bad_cast& e) {
                throw std::logic_error(
                    "AggregateManager bad Device cast: " 
                    + name);
            }
        }

        /**
         * Return true if a device of given template type T 
         * is already contained with given name or id
         */
        //Overloaded shortcuts
        template <typename T>
        inline bool devExists(id_t id) const
        {
            return devExistsById<T>(id);
        }
        template <typename T>
        inline bool devExists(const std::string& name) const
        {
            return devExistsByName<T>(name);
        }
        //Implementation
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
         * Access to internal map of derived Device pointers
         * of given template type indexed by their name.
         * (This method is choosen only if given template type T
         * is part of Manager variadic type list of supported Devices
         * implementation)
         */
        template <typename T>
        inline typename TypeInPack<T, const DevicesByName<T>&>::type devContainer() const
        {
            return ImplManager<T>::devContainer();
        }

        /**
         * Build and return a map of derived Device 
         * of given type T indexed by their name 
         * using dynamic cast.
         * (only compatible Devices are selected)
         * (Not efficient)
         * (This method is choosen only if given template type T
         * is NOT part of Manager variadic type list of supported 
         * Devices implementation)
         */
        //Return const DevicesByName<T> (no reference)
        template <typename T>
        inline typename TypeNotInPack<T, DevicesByName<T>>::type devContainer() const
        {
            DevicesByName<T> map;
            for (const auto& it : _devicesByName) {
                T* ptr = dynamic_cast<T*>(it.second);
                if (ptr != nullptr) {
                    map[it.first] = ptr;
                }
            }
            return map;
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
            //Add by type number
            inline static void runAddByType(
                AggregateManager<Types...>* ptr, 
                id_t id, type_t type)
            {
                if (ImplManager<T>::typeNumber() == type) {
                    std::string name = 
                        ImplManager<T>::typeName() 
                        + "_" 
                        + std::to_string(
                            ptr->ImplManager<T>::devContainer().size()+1);
                    ptr->devAdd<T>(id, name);
                } else {
                    throw std::logic_error(
                        "AggregateManager add Device of type not supported: " 
                        + std::to_string(id));
                }
            }
            //Is type supported
            inline static bool runIsTypeSupported(type_t type)
            {
                return (ImplManager<T>::typeNumber() == type);
            }
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
            //Save JSON
            inline static void runSaveJSON(
                const AggregateManager<Types...>* ptr, 
                nlohmann::json& j)
            {
                nlohmann::json tmpJ = ptr->ImplManager<T>::saveJSON();
                if (tmpJ["devices"].size() > 0) {
                    j[ImplManager<T>::typeName()] = tmpJ;
                }
            }
            //Load JSON
            inline static void runLoadJSON(
                AggregateManager<Types...>* ptr, 
                const nlohmann::json& j)
            {
                if (j.count(ImplManager<T>::typeName()) == 1) {
                    //Check json format and 
                    //add non existing devices
                    ptr->checkDevicesJSON<T>(
                        j.at(ImplManager<T>::typeName()));
                    //Load parameters
                    ptr->ImplManager<T>::loadJSON(
                        j.at(ImplManager<T>::typeName()));
                }
            }
        };
        //General iteration case
        template <typename T, typename ... Ts>
        struct Impl<T, Ts...> {
            //Add by type number
            inline static void runAddByType(
                AggregateManager<Types...>* ptr, 
                id_t id, type_t type)
            {
                if (ImplManager<T>::typeNumber() == type) {
                    std::string name = 
                        ImplManager<T>::typeName() 
                        + "_" 
                        + std::to_string(
                            ptr->ImplManager<T>::devContainer().size()+1);
                    ptr->devAdd<T>(id, name);
                } else {
                    Impl<Ts...>::runAddByType(ptr, id, type);
                }
            }
            //Is type supported
            inline static bool runIsTypeSupported(type_t type)
            {
                if (ImplManager<T>::typeNumber() == type) {
                    return true;
                } else {
                    return Impl<Ts...>::runIsTypeSupported(type);
                }
            }
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
            //Save JSON
            inline static void runSaveJSON(
                const AggregateManager<Types...>* ptr, 
                nlohmann::json& j)
            {
                nlohmann::json tmpJ = ptr->ImplManager<T>::saveJSON();
                if (tmpJ["devices"].size() > 0) {
                    j[ImplManager<T>::typeName()] = tmpJ;
                }
                Impl<Ts...>::runSaveJSON(ptr, j);
            }
            //Load JSON
            inline static void runLoadJSON(
                AggregateManager<Types...>* ptr, 
                const nlohmann::json& j)
            {
                if (j.count(ImplManager<T>::typeName()) == 1) {
                    //Check json format and 
                    //add non existing devices
                    ptr->checkDevicesJSON<T>(
                        j.at(ImplManager<T>::typeName()));
                    //Load parameters
                    ptr->ImplManager<T>::loadJSON(
                        j.at(ImplManager<T>::typeName()));
                }
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
            //Load TypedManager parameters
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
                    devAdd<T>(id, name);
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

