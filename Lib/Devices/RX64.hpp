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
 * RX64
 *
 * Dynamixel RX-64 Device
 * implementation
 */
class RX64 : public RX
{
public:
  /**
   * Initialization with name and id
   */
  RX64(const std::string& name, id_t id);
};

/**
 * DeviceManager specialized for RX64
 */
template <>
class ImplManager<RX64> : public TypedManager<RX64>
{
public:
  inline static type_t typeNumber()
  {
    return 0x0040;
  }

  inline static std::string typeName()
  {
    return "RX64";
  }
};

}  // namespace RhAL
