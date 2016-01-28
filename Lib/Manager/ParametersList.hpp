#pragma once

#include <unordered_map>
#include <stdexcept>
#include <json.hpp>
#include "Parameter.hpp"

namespace RhAL {

/**
 * ParametersList
 *
 * Container for boolean, float 
 * and string parameter pointers.
 * Pointer are neither allocated or
 * freed by the container.
 */
class ParametersList
{
    public:

        /**
         * Typedef for container
         */
        typedef std::unordered_map<std::string, ParameterBool*> 
            ContainerBool;
        typedef std::unordered_map<std::string, ParameterNumber*> 
            ContainerNumber;
        typedef std::unordered_map<std::string, ParameterStr*> 
            ContainerStr;

        /**
         * Initialization
         */
        inline ParametersList() :
            _paramsBool(),
            _paramsNumber(),
            _paramsStr()
        {
        }

        /**
         * Return true if given parameter name 
         * is already contained
         */
        inline bool exists(const std::string& name) const
        {
            return 
                (_paramsBool.count(name) != 0) ||
                (_paramsNumber.count(name) != 0) ||
                (_paramsStr.count(name) != 0);
        }

        /**
         * Add a new boolean, number and
         * string named parameters pointer to 
         * the internal container
         * Throw std::logic_error if parameter name 
         * is already contained.
         */
        inline void add(ParameterBool* param)
        {
            if (param == nullptr) {
                throw std::logic_error(
                    "ParametersList null pointer");
            }
            if (exists(param->name)) {
                throw std::logic_error(
                    "ParametersContainer bool name already exists: " 
                    + param->name);
            }
            _paramsBool[param->name] = param;
        }
        inline void add(ParameterNumber* param)
        {
            if (param == nullptr) {
                throw std::logic_error("ParametersList null pointer");
            }
            if (exists(param->name)) {
                throw std::logic_error(
                    "ParametersContainer number name already exists: " 
                    + param->name);
            }
            _paramsNumber[param->name] = param;
        }
        inline void add(ParameterStr* param)
        {
            if (param == nullptr) {
                throw std::logic_error("ParametersList null pointer");
            }
            if (exists(param->name)) {
                throw std::logic_error(
                    "ParametersContainer string name already exists: " 
                    + param->name);
            }
            _paramsStr[param->name] = param;
        }

        /**
         * Access to given parameter by its name.
         * Throw std::logic_error if asked name does not exists
         */
        inline const ParameterBool& paramBool(const std::string& name) const
        {
            if (_paramsBool.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer bool name does not exist: " 
                    + name);
            }
            return *(_paramsBool.at(name));
        }
        inline ParameterBool& paramBool(const std::string& name)
        {
            if (_paramsBool.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer bool name does not exist: " 
                    + name);
            }
            return *(_paramsBool.at(name));
        }
        inline const ParameterNumber& paramNumber(const std::string& name) const
        {
            if (_paramsNumber.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer number name does not exist: " 
                    + name);
            }
            return *(_paramsNumber.at(name));
        }
        inline ParameterNumber& paramNumber(const std::string& name)
        {
            if (_paramsNumber.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer number name does not exist: " 
                    + name);
            }
            return *(_paramsNumber.at(name));
        }
        inline const ParameterStr& paramStr(const std::string& name) const
        {
            if (_paramsStr.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer str name does not exist: " 
                    + name);
            }
            return *(_paramsStr.at(name));
        }
        inline ParameterStr& paramStr(const std::string& name)
        {
            if (_paramsStr.count(name) == 0) {
                throw std::logic_error(
                    "ParametersContainer str name does not exist: " 
                    + name);
            }
            return *(_paramsStr.at(name));
        }

        /**
         * Direct access to Parameter container
         */
        inline const ContainerBool& containerBool() const
        {
            return _paramsBool;
        }
        inline const ContainerNumber& containerNumber() const
        {
            return _paramsNumber;
        }
        inline const ContainerStr& containerStr() const
        {
            return _paramsStr;
        }
    
        /**
         * Export and return all contained 
         * parameters into a json object
         */
        inline nlohmann::json saveJSON() const
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
        inline void loadJSON(const nlohmann::json& j)
        {
            //Empty case
            if (j.is_null()) {
                return;
            }
            //Check json type
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
                        paramBool(it.key()).value = it.value();
                    }
                } else if (it.value().is_number()) {
                    //Number
                    if (_paramsNumber.count(it.key()) == 0) {
                        throw std::runtime_error(
                            "ParametersContainer load parameters json number does not exist: " 
                            + it.key());
                    } else {
                        paramNumber(it.key()).value = it.value();
                    }
                } else if (it.value().is_string()) {
                    //String
                    if (_paramsStr.count(it.key()) == 0) {
                        throw std::runtime_error(
                            "ParametersContainer load parameters json str does not exist: " 
                            + it.key());
                    } else {
                        paramStr(it.key()).value = it.value();
                    }
                } else {
                    throw std::runtime_error(
                        "ParametersContainer load parameters json malformated");
                }
            }
        }

    private:

        /**
         * Container indexed by Parameter names
         */
        ContainerBool _paramsBool;
        ContainerNumber _paramsNumber;
        ContainerStr _paramsStr;
};

}

