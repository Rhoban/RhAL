#include "Devices/ExampleDevice2.hpp"

namespace RhAL
{
void convIn2(RhAL::data_t* buffer, float value)
{
  *(reinterpret_cast<float*>(buffer)) = value;
}
float convOut2(const RhAL::data_t* buffer)
{
  return *(reinterpret_cast<const float*>(buffer));
}

BaseExampleDevice2::BaseExampleDevice2(const std::string& name, id_t id) : Device(name, id)
{
}

ExampleDevice2::ExampleDevice2(const std::string& name, id_t id)
  : BaseExampleDevice2(name, id)
  ,
  // Registers configuration
  _pitch("pitch", (addr_t)4, 4, convIn2, convOut2, 2)
  , _roll("roll", (addr_t)8, 4, convIn2, convOut2, 0, true, false)
  , _mode("mode", (addr_t)12, 4, convIn2, convOut2, 0, false, true)
{
}

TypedRegisterFloat& ExampleDevice2::pitch()
{
  return _pitch;
}
TypedRegisterFloat& ExampleDevice2::roll()
{
  return _roll;
}
TypedRegisterFloat& ExampleDevice2::mode()
{
  return _mode;
}

void ExampleDevice2::onInit()
{
  Device::registersList().add(&_pitch);
  Device::registersList().add(&_roll);
  Device::registersList().add(&_mode);
}

}  // namespace RhAL
