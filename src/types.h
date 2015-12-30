#pragma once

#include <chrono>

namespace RhAL {

/**
 * Device unique id
 */
typedef int id_t;

/**
 * Device register address
 */
typedef size_t addr_t;

/**
 * Device model number
 */
typedef int type_t;

/**
 * Raw data
 */
typedef unsigned char data_t;

/**
 * Timestamp
 */
typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;

}

