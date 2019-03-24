#pragma once

#include <map>
#include <stdexcept>
#include <json/json.h>
#include "Parameter.hpp"

namespace RhAL
{
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
  typedef std::map<std::string, ParameterBool*> ContainerBool;
  typedef std::map<std::string, ParameterNumber*> ContainerNumber;
  typedef std::map<std::string, ParameterStr*> ContainerStr;

  /**
   * Initialization
   */
  ParametersList();

  /**
   * Return true if given parameter name
   * is already contained
   */
  bool exists(const std::string& name) const;

  /**
   * Add a new boolean, number and
   * string named parameters pointer to
   * the internal container
   * Throw std::logic_error if parameter name
   * is already contained.
   */
  void add(ParameterBool* param);
  void add(ParameterNumber* param);
  void add(ParameterStr* param);

  /**
   * Access to given parameter by its name.
   * Throw std::logic_error if asked name does not exists
   */
  const ParameterBool& paramBool(const std::string& name) const;
  ParameterBool& paramBool(const std::string& name);
  const ParameterNumber& paramNumber(const std::string& name) const;
  ParameterNumber& paramNumber(const std::string& name);
  const ParameterStr& paramStr(const std::string& name) const;
  ParameterStr& paramStr(const std::string& name);

  /**
   * Direct access to Parameter container
   */
  const ContainerBool& containerBool() const;
  const ContainerNumber& containerNumber() const;
  const ContainerStr& containerStr() const;

  /**
   * Export and return all contained
   * parameters into a json object
   */
  Json::Value saveJSON() const;

  /**
   * Import parameters from given json object.
   * Throw std::runtime_error if given json is malformated.
   */
  void loadJSON(const Json::Value& j);

private:
  /**
   * Container indexed by Parameter names
   */
  ContainerBool _paramsBool;
  ContainerNumber _paramsNumber;
  ContainerStr _paramsStr;
};

}  // namespace RhAL
