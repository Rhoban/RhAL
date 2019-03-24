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
 * AX18
 *
 * Dynamixel AX-18 Device
 * implementation
 */
class AX18 : public RX
{
public:
  /**
   * Initialization with name and id
   */
  AX18(const std::string& name, id_t id);
};

/**
 * DeviceManager specialized for AX18
 */
template <>
class ImplManager<AX18> : public TypedManager<AX18>
{
public:
  inline static type_t typeNumber()
  {
    return 0x0012;
  }

  inline static std::string typeName()
  {
    return "AX18";
  }
};

}  // namespace RhAL
