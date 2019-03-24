#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"

namespace RhAL
{
/**
 * Convertion function
 */
void convIn2(RhAL::data_t* buffer, float value);
float convOut2(const RhAL::data_t* buffer);

/**
 * BaseExampleDevice2
 */
class BaseExampleDevice2 : public Device
{
public:
  /**
   * Initilization
   */
  BaseExampleDevice2(const std::string& name, id_t id);
};

/**
 * ExampleDevice2
 *
 * Simple example device
 * type implemetation
 */
class ExampleDevice2 : public BaseExampleDevice2
{
public:
  /**
   * Initialization with name and id
   */
  ExampleDevice2(const std::string& name, id_t id);

  /**
   * Register access
   */
  TypedRegisterFloat& pitch();
  TypedRegisterFloat& roll();
  TypedRegisterFloat& mode();

protected:
  /**
   * Inherit.
   * Declare Registers and parameters
   */
  virtual void onInit() override;

private:
  /**
   * Registers
   */
  TypedRegisterFloat _pitch;
  TypedRegisterFloat _roll;
  TypedRegisterFloat _mode;

  /**
   * Mutex protecting access
   */
  mutable std::mutex _mutex;
};

/**
 * DeviceManager specialized for ExampleDevice2
 */
template <>
class ImplManager<ExampleDevice2> : public TypedManager<ExampleDevice2>
{
public:
  inline static type_t typeNumber()
  {
    return 2;
  }

  inline static std::string typeName()
  {
    return "ExampleDevice2";
  }
};

}  // namespace RhAL
