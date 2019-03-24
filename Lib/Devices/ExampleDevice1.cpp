#include "Devices/ExampleDevice1.hpp"

#include <iostream>

namespace RhAL
{
void convIn1(RhAL::data_t* buffer, float value)
{
  *(reinterpret_cast<float*>(buffer)) = value;
}
float convOut1(const RhAL::data_t* buffer)
{
  return *(reinterpret_cast<const float*>(buffer));
}

BaseExampleDevice1::BaseExampleDevice1(const std::string& name, id_t id)
  : Device(name, id)
  ,
  // Registers configuration
  _voltage("voltage", (addr_t)20, 4, convIn1, convOut1, 0)
{
}

TypedRegisterFloat& BaseExampleDevice1::voltage()
{
  return _voltage;
}

void BaseExampleDevice1::onSwap()
{
  std::cout << "ExampleDevice1 onSwap() " << this->name() << std::endl;
}

ExampleDevice1::ExampleDevice1(const std::string& name, id_t id)
  : BaseExampleDevice1(name, id)
  ,
  // Registers configuration
  _goal("goal", (addr_t)4, 4,
        [this](data_t* data, float angle) {
          // Goal position encoder
          std::lock_guard<std::mutex> lock(this->_mutex);
          if (this->_inverted.value)
          {
            convIn1(data, -angle - this->_zero.value);
          }
          else
          {
            convIn1(data, angle + this->_zero.value);
          }
        },
        [this](const data_t* data) -> float {
          // Goal position decoder
          std::lock_guard<std::mutex> lock(this->_mutex);
          double angle = convOut1(data);
          if (this->_inverted.value)
          {
            return -(angle + this->_zero.value);
          }
          else
          {
            return angle - this->_zero.value;
          }
        },
        0)
  , _position("position", (addr_t)8, 4, convIn1, convOut1, 1)
  , _temperature("temperature", (addr_t)16, 4, convIn1, convOut1, 4)
  ,
  // Parameters configuration
  _inverted("inverse", false)
  , _zero("zero", 0.0)
  , _mutex()
{
}

TypedRegisterFloat& ExampleDevice1::goal()
{
  return _goal;
}
TypedRegisterFloat& ExampleDevice1::position()
{
  return _position;
}
TypedRegisterFloat& ExampleDevice1::temperature()
{
  return _position;
}

float ExampleDevice1::getInverted() const
{
  std::lock_guard<std::mutex> lock(_mutex);
  return _inverted.value;
}
void ExampleDevice1::setInverted(float val)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _inverted.value = val;
}
float ExampleDevice1::getZero() const
{
  std::lock_guard<std::mutex> lock(_mutex);
  return _zero.value;
}
void ExampleDevice1::setZero(float val)
{
  std::lock_guard<std::mutex> lock(_mutex);
  _zero.value = val;
}

void ExampleDevice1::onInit()
{
  Device::registersList().add(&_goal);
  Device::registersList().add(&_position);
  Device::registersList().add(&_temperature);
  Device::registersList().add(&_voltage);
  Device::parametersList().add(&_inverted);
  Device::parametersList().add(&_zero);
}

}  // namespace RhAL
