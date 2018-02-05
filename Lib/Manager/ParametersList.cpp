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

Json::Value ParametersList::saveJSON() const
{
    Json::Value j;
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

void ParametersList::loadJSON(const Json::Value& j)
{
    //Empty case
    if (j.isNull()) {
        return;
    }
    //Check json type
    if (!j.isObject()) {
        throw std::runtime_error(
            "ParametersContainer load parameters json not object");
    }
    //Iterate on json entries
    for (Json::Value::const_iterator it = j.begin(); it != j.end(); it++) {
        const std::string & key = it.name();
        const Json::Value & v = j[key];
        if (v.isBool()) {
            //Boolean
            if (_paramsBool.count(key) == 0) {
                throw std::runtime_error(
                    "ParametersContainer load parameters json bool does not exist: " 
                    + key);
            } else {
                paramBool(key).value = v.asBool();
            }
        } else if (v.isDouble()) {
            //Number
            if (_paramsNumber.count(key) == 0) {
                throw std::runtime_error(
                    "ParametersContainer load parameters json number does not exist: " 
                    + key);
            } else {
                paramNumber(key).value = v.asDouble();
            }
        } else if (v.isString()) {
            //String
            if (_paramsStr.count(key) == 0) {
                throw std::runtime_error(
                    "ParametersContainer load parameters json str does not exist: " 
                    + key + " with value: " + v.asString());
            } else {
                paramStr(key).value = v.asString();
            }
        } else {
            throw std::runtime_error(
                "ParametersContainer load parameters json malformated");
        }
    }
}

}

