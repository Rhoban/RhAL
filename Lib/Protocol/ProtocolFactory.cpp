#include "ProtocolFactory.hpp"

namespace RhAL {

Protocol* ProtocolFactory(
    const std::string& name,
    Bus& bus)
{
    if (name == "DynamixelV1") {
        return new DynamixelV1(bus);
    } else {
        return nullptr;
    }
}

}

