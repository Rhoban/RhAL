#pragma once

#include <string>
#include <mutex>
#include "Manager/TypedManager.hpp"
#include "Manager/Device.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Devices/RX.hpp"

namespace RhAL
{
/**
 * RX28
 *
 * Dynamixel RX-28 Device
 * implementation
 */
class RX28 : public RX
{
public:
  /**
   * Initialization with name and id
   */
  RX28(const std::string& name, id_t id);
};

/**
 * DeviceManager specialized for RX28
 */
template <>
class ImplManager<RX28> : public TypedManager<RX28>
{
public:
  inline static type_t typeNumber()
  {
    return 0x001C;
  }

  inline static std::string typeName()
  {
    return "RX28";
  }
};

}  // namespace RhAL
