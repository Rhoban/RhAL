#include "Devices/Pins.hpp"

namespace RhAL
{
static float voltageDecode(const data_t* data)
{
  return ((float)convDecode_2Bytes(data)) / 1000.0;
}

Pins::Pins(const std::string& name, id_t id)
  : Device(name, id)
  ,
  // Read only registers
  //(name, address, size, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
  _pins("pins", 0x24, 2, convDecode_2Bytes, 10)
  , _voltage("voltage", 0x26, 2, voltageDecode, 10)
{
}

TypedRegisterInt& Pins::pins()
{
  return _pins;
}

bool Pins::getPin(unsigned int number)
{
  uint8_t value = _pins.readValue().value;
  return (value & (1 << number));
}

void Pins::getPins(bool pins[7])
{
  uint8_t value = _pins.readValue().value;
  for (int i = 0; i < 7; i++)
  {
    if (value & (1 << i))
    {
      pins[i] = true;
    }
    else
    {
      pins[i] = false;
    }
  }
}

void Pins::onInit()
{
  Device::registersList().add(&_pins);
  Device::registersList().add(&_voltage);
}

}  // namespace RhAL
