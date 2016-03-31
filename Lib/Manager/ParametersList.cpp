#include "ParametersList.hpp"

namespace RhAL {

ParametersList::ParametersList() :
    _paramsBool(),
    _paramsNumber(),
    _paramsStr()
{
}

bool ParametersList::exists(const std::string& name) const
{
    return 
        (_paramsBool.count(name) != 0) ||
        (_paramsNumber.count(name) != 0) ||
        (_paramsStr.count(name) != 0);
}

void ParametersList::add(ParameterBool* param)
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
void ParametersList::add(ParameterNumber* param)
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
void ParametersList::add(ParameterStr* param)
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

const ParameterBool& ParametersList::paramBool(const std::string& name) const
{
    if (_paramsBool.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer bool name does not exist: " 
            + name);
    }
    return *(_paramsBool.at(name));
}
ParameterBool& ParametersList::paramBool(const std::string& name)
{
    if (_paramsBool.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer bool name does not exist: " 
            + name);
    }
    return *(_paramsBool.at(name));
}
const ParameterNumber& ParametersList::paramNumber(const std::string& name) const
{
    if (_paramsNumber.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer number name does not exist: " 
            + name);
    }
    return *(_paramsNumber.at(name));
}
ParameterNumber& ParametersList::paramNumber(const std::string& name)
{
    if (_paramsNumber.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer number name does not exist: " 
            + name);
    }
    return *(_paramsNumber.at(name));
}
const ParameterStr& ParametersList::paramStr(const std::string& name) const
{
    if (_paramsStr.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer str name does not exist: " 
            + name);
    }
    return *(_paramsStr.at(name));
}
ParameterStr& ParametersList::paramStr(const std::string& name)
{
    if (_paramsStr.count(name) == 0) {
        throw std::logic_error(
            "ParametersContainer str name does not exist: " 
            + name);
    }
    return *(_paramsStr.at(name));
}

const ParametersList::ContainerBool& ParametersList::containerBool() const
{
    return _paramsBool;
}
const ParametersList::ContainerNumber& ParametersList::containerNumber() const
{
    return _paramsNumber;
}
const ParametersList::ContainerStr& ParametersList::containerStr() const
{
    return _paramsStr;
}

nlohmann::json ParametersList::saveJSON() const
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

void ParametersList::loadJSON(const nlohmann::json& j)
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

}

