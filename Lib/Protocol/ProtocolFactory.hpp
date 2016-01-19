#pragma once

#include <string>
#include "Bus/Bus.hpp"
#include "Protocol/Protocol.hpp"

namespace RhAL {

/**
 * Allocate and return a derived
 * Protocol instance from given textual
 * name and bus instance.
 * Nullptr is returned if given name
 * does not match any implementation.
 * Returned instance have to be deallocated.
 */
Protocol* ProtocolFactory(
    const std::string& name,
    Bus& bus);

}

