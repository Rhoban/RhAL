#pragma once

#include "types.h"
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

#include "Devices/ExampleDevice1.hpp"
#include "Devices/ExampleDevice2.hpp"
#include "Devices/DXL.hpp"
#include "Devices/MX106.hpp"
#include "Devices/MX64.hpp"
#include "Devices/MX28.hpp"
#include "Devices/MX12.hpp"
#include "Devices/RX64.hpp"
#include "Devices/RX28.hpp"
#include "Devices/AX18.hpp"
#include "Devices/AX12.hpp"
#include "Devices/IMU.hpp"
#include "Devices/PressureSensor.hpp"
#include "Devices/Pins.hpp"

/**
 * Define classic Manager typedef
 * with all implemented Device
 */
namespace RhAL {
typedef Manager<
    MX106,
    MX64,
    MX28,
    MX12,
    RX64,
    RX28,
    AX18,
    AX12,
    IMU,
    PressureSensor,
    Pins
> StandardManager;
}

