#pragma once

#include "types.h"
#include "timestamp.h"

#include "Manager/Aggregation.h"
#include "Manager/Statistics.hpp"
#include "Manager/CallManager.hpp"
#include "Manager/Register.hpp"
#include "Manager/Parameter.hpp"
#include "Manager/RegistersList.hpp"
#include "Manager/ParametersList.hpp"
#include "Manager/Device.hpp"
#include "Manager/TypedManager.hpp"
#include "Manager/BaseManager.hpp"
#include "Manager/AggregateManager.hpp"
#include "Manager/Manager.hpp"
#include "Bindings/RhIOBinding.hpp"

#include "Devices/ExampleDevice1.hpp"
#include "Devices/ExampleDevice2.hpp"
#include "Devices/DXL.hpp"
#include "Devices/MX106.hpp"
#include "Devices/MX64.hpp"
#include "Devices/Dynaban64.hpp"
#include "Devices/MX28.hpp"
#include "Devices/MX12.hpp"
#include "Devices/RX64.hpp"
#include "Devices/RX28.hpp"
#include "Devices/RX24.hpp"
#include "Devices/AX18.hpp"
#include "Devices/AX12.hpp"
#include "Devices/IMU.hpp"
#include "Devices/GY85.hpp"
#include "Devices/PressureSensor.hpp"
#include "Devices/Pins.hpp"

/**
 * Define classic Manager typedef
 * with all implemented Devices
 */
namespace RhAL
{
#define RHAL_STANDARD_MANAGER_TYPES                                                                                    \
  AX12, AX18, RX24, RX28, RX64, MX12, MX28, Dynaban64, MX106, IMU, PressureSensor4, PressureSensor8, Pins, GY85

/**
 * StandardManager typedef
 */
typedef Manager<RHAL_STANDARD_MANAGER_TYPES> StandardManager;

/**
 * Template explicit instatiation
 */
extern template class Manager<RHAL_STANDARD_MANAGER_TYPES>;

}  // namespace RhAL
