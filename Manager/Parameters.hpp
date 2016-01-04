#pragma once

#include <unordered_map>
#include <string>
#include <stdexcept>
#include "json.hpp"

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
 * ParametersContainer
 *
 * Container for boolean, float 
 * and string parameters
 */
class ParametersContainer
{
    public:

        /**
         * Typedef for container
         */
        typedef std::unordered_map<std::string, Parameter<bool>*> 
            ContainerBool;
        typedef std::unordered_map<std::string, Parameter<double>*> 
            ContainerNumber;
        typedef std::unordered_map<std::string, Parameter<std::string>*> 
            ContainerStr;

        /**
         * Initialization
         */
        inline ParametersContainer() :
            _paramsBool(),
            _paramsNumber(),
            _paramsStr()
        {
        }

        /**
         * Parameters desallocation
         */
        inline ~ParametersContainer()
        {
            for (auto& it : _paramsBool) {
                delete it.second;
                it.second = nullptr;
            }
            for (auto& it : _paramsNumber) {
                delete it.second;
                it.second = nullptr;
            }
            for (auto& it : _paramsStr) {
                delete it.second;
                it.second = nullptr;
            }
        }

        /**
         * Copy contructor
         */
        inline ParametersContainer(const ParametersContainer& container) :
            _paramsBool(),
            _paramsNumber(),
            _paramsStr()
        {
            for (auto& it : container._paramsBool) {
                _paramsBool[it.first] = new Parameter<bool>(*(it.second));
                _paramsBool[it.first]->value = it.second->value;
            }
            for (auto& it : container._paramsNumber) {
                _paramsNumber[it.first] = new Parameter<double>(*(it.second));
                _paramsNumber[it.first]->value = it.second->value;
            }
            for (auto& it : container._paramsStr) {
                _paramsStr[it.first] = new Parameter<std::string>(*(it.second));
                _paramsStr[it.first]->value = it.second->value;
            }
        }

        /**
         * Deleted assignment operator
         */
        ParametersContainer& operator=(
            const ParametersContainer&) const = delete;

        /**
         * Return true if given parameter name 
         * is already declared
         */
        inline bool exists(const std::string& name) const
        {
            return 
                (_paramsBool.count(name) != 0) ||
                (_paramsNumber.count(name) != 0) ||
                (_paramsStr.count(name) != 0);
        }

        /**
         * Create new boolean, number and
         * string named parameters with given default value.
         * Throw std::logic_error if given name is already declared.
         */
        inline void addBool(const std::string& name, bool value)
        {
            if (exists(name)) {
                throw std::logic_error(
                    "ParametersContainer bool name already exists: " + name);
            }
            _paramsBool[name] = new Parameter<bool>(name, value);
        }
        inline void addNumber(const std::string& name, double value)
        {
            if (exists(name)) {
                throw std::logic_error(
                    "ParametersContainer number name already exists: " + name);
            }
            _paramsNumber[name] = new Parameter<double>(name, value);
        }
        inline void addStr(const std::string& name, const std::string& value)
        {
            if (exists(name)) {
                throw std::logic_error(
                    "ParametersContainer str name already exists: " + name);
            }
            _paramsStr[name] = new Parameter<std::string>(name, value);
        }

        /**
         * Access to given parameter by its name.
         * Throw std::logic_error if asked name does not exists
         */
        Parameter<bool>& getBool(const std::string& name)
        {
            if (_paramsBool.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer bool name does not exist: " + name);
            }
            return *(_paramsBool.at(name));
        }
        Parameter<double>& getNumber(const std::string& name)
        {
            if (_paramsNumber.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer number name does not exist: " + name);
            }
            return *(_paramsNumber.at(name));
        }
        Parameter<std::string>& getStr(const std::string& name)
        {
            if (_paramsStr.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer str name does not exist: " + name);
            }
            return *(_paramsStr.at(name));
        }

        /**
         * Direct access to parameter container
         */
        const ContainerBool& containerBool() const
        {
            return _paramsBool;
        }
        const ContainerNumber& containerNumber() const
        {
            return _paramsNumber;
        }
        const ContainerStr& containerStr() const
        {
            return _paramsStr;
        }
    
        /**
         * Export and return all contained 
         * parameters into a json object
         */
        nlohmann::json saveJSON() const
        {
            nlohmann::json j;
            for (const auto& it : _paramsBool) {
                j[it.first] = it.second->value;
            }
            for (const auto& it : _paramsNumber) {
                j[it.first] = it.second->value;
            }
            for (const auto& it : _paramsStr) {
                j[it.first] = it.second->value;
            }

            return j;
        }

        /**
         * Import parameters from given json object.
         * Throw std::runtime_error if given json is malformated.
         */
        void loadJSON(const nlohmann::json& j)
        {
            if (!j.is_object()) {
                throw std::runtime_error(
                    "ParametersContainer load parameters json not object");
            }
            //Iterate on json entries
            for (nlohmann::json::const_iterator it=j.begin();it!=j.end();it++) {
                if (it.value().is_boolean()) {
                    //Boolean
                    if (_paramsBool.count(it.key()) == 0) {
                        throw std::runtime_error(
                            "ParametersContainer load parameters json bool does not exist: " 
                            + it.key());
                    } else {
                        getBool(it.key()).value = it.value();
                    }
                } else if (it.value().is_number()) {
                    //Number
                    if (_paramsNumber.count(it.key()) == 0) {
                        throw std::runtime_error(
                            "ParametersContainer load parameters json number does not exist: " 
                            + it.key());
                    } else {
                        getNumber(it.key()).value = it.value();
                    }
                } else if (it.value().is_string()) {
                    //String
                    if (_paramsStr.count(it.key()) == 0) {
                        throw std::runtime_error(
                            "ParametersContainer load parameters json str does not exist: " 
                            + it.key());
                    } else {
                        getStr(it.key()).value = it.value();
                    }
                } else {
                    throw std::runtime_error(
                        "ParametersContainer load parameters json malformated");
                }
            }
        }

    private:

        /**
         * Container indexed by parameter names
         */
        ContainerBool _paramsBool;
        ContainerNumber _paramsNumber;
        ContainerStr _paramsStr;
};

}

