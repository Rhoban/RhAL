#include "ProtocolFactory.hpp"
#include "Protocol/DynamixelV1.hpp"
#include "Protocol/FakeProtocol.hpp"

namespace RhAL
{
Protocol* ProtocolFactory(const std::string& name, Bus& bus)
{
  if (name == "DynamixelV1")
  {
    return new DynamixelV1(bus);
  }
  else if (name == "FakeProtocol")
  {
    return new FakeProtocol(bus);
  }
  else
  {
    return nullptr;
  }
}

}  // namespace RhAL
