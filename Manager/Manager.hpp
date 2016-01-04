#pragma once

#include "BaseManager.hpp"
#include "Device.hpp"

namespace RhAL {

/**
 * Manager
 *
 * Main interface class for lowlevel
 * hardware device communication.
 */
template <typename ... Types> 
class Manager : public ImplManager<Types>...
{
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
                    "Manager device name or id already added: " 
                    + name);
            } else {
                ImplManager<T>::devAdd(name, id);
            }
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
            //Get by Id
            inline static const Device& runById(
                const Manager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "Manager Device id not found: " 
                        + std::to_string(id));
                }
                return ptr->devById<T>(id);
            }
            inline static Device& runById(
                Manager<Types...>* ptr, id_t id)
            {
                if (!ptr->devExistsById<T>(id)) {
                    throw std::logic_error(
                        "Manager Device id not found: " 
                        + std::to_string(id));
                }
                return ptr->devById<T>(id);
            }
            //Get by Name
            inline static const Device& runByName(
                const Manager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "Manager Device name not found: " 
                        + name);
                }
                return ptr->devByName<T>(name);
            }
            inline static Device& runByName(
                Manager<Types...>* ptr, const std::string& name)
            {
                if (!ptr->devExistsByName<T>(name)) {
                    throw std::logic_error(
                        "Manager Device name not found: " 
                        + name);
                }
                return ptr->devByName<T>(name);
            }
            //Exists by Id
            inline static bool runExistsById(
                const Manager<Types...>* ptr, id_t id)
            {
                return ptr->devExistsById<T>(id);
            }
            //Exists by Name
            inline static bool runExistsByName(
                const Manager<Types...>* ptr, const std::string& name)
            {
                return ptr->devExistsByName<T>(name);
            }
            //List
            inline static void runList(
                const Manager<Types...>* ptr, 
                std::vector<const Device*>& vect)
            {
                for (const auto& it : ptr->ImplManager<T>::devContainer()) {
                    vect.push_back(it.second);
                }
            }
        };
        //General iteration case
        template <typename T, typename ... Ts>
        struct Impl<T, Ts...> {
            //Get by Id
            inline static const Device& runById(
                const Manager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ptr->devById<T>(id);
                } else {
                    return Impl<Ts...>::runById(ptr, id);
                }
            }
            inline static Device& runById(
                Manager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return ptr->devById<T>(id);
                } else {
                    return Impl<Ts...>::runById(ptr, id);
                }
            }
            //Get by Name
            inline static const Device& runByName(
                const Manager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ptr->devByName<T>(name);
                } else {
                    return Impl<Ts...>::runByName(ptr, name);
                }
            }
            inline static Device& runByName(
                Manager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return ptr->devByName<T>(name);
                } else {
                    return Impl<Ts...>::runByName(ptr, name);
                }
            }
            //Exists by Id
            inline static bool runExistsById(
                const Manager<Types...>* ptr, id_t id)
            {
                if (ptr->devExistsById<T>(id)) {
                    return true;
                } else {
                    return Impl<Ts...>::runExistsById(ptr, id);
                }
            }
            //Exists by Name
            inline static bool runExistsByName(
                const Manager<Types...>* ptr, const std::string& name)
            {
                if (ptr->devExistsByName<T>(name)) {
                    return true;
                } else {
                    return Impl<Ts...>::runExistsByName(ptr, name);
                }
            }
            //List
            inline static void runList(
                const Manager<Types...>* ptr, 
                std::vector<const Device*>& vect)
            {
                for (const auto& it : ptr->ImplManager<T>::devContainer()) {
                    vect.push_back(it.second);
                }
                Impl<Ts...>::runList(ptr, vect);
            }
        };
};

}

