#pragma once

#include <iostream>
#include "types.h"

namespace RhAL {

/**
 * Statistics
 *
 * Hold and accumulate
 * all Manager Statistics
 */
struct Statistics
{
    //Date at Manager instantiation
    TimePoint managerStart;
    //Number of calls to Protocol
    //read/write/syncRead/syncWrite
    unsigned long readCount;
    unsigned long writeCount;
    unsigned long syncReadCount;
    unsigned long syncWriteCount;
    //Total length of data read or write using
    //Protocol read/write/syncRead/syncWrite calls
    unsigned long readLength;
    unsigned long writeLength;
    unsigned long syncReadLength;
    unsigned long syncWriteLength;
    //Total time duration spent during Protocol
    //read/write/syncRead/syncWrite calls
    TimeDurationMicro readDuration;
    TimeDurationMicro writeDuration;
    TimeDurationMicro syncReadDuration;
    TimeDurationMicro syncWriteDuration;
    //Number of calls to Manager flush(),
    //waitNextFlush(), forceRead() and forceWrite()
    unsigned long flushCount;
    unsigned long waitNextFlushCount;
    unsigned long waitNextFlushCooperativeCount;
    unsigned long forceReadCount;
    unsigned long forceWriteCount;
    //Total time duration spent during
    //Manager waiting for cooperative users
    //threads at flush() begining and
    //during users waiting for Manager
    //at nextFlush()
    TimeDurationMicro waitUsersDuration;
    TimeDurationMicro waitManagerDuration;
    //Number of calls to Manager
    //emergencyStop(), exitEmergencyState()
    unsigned long emergencyCount;
    unsigned long exitEmergencyCount;
    //Number of valid (usable) receivned packets
    unsigned long deviceOKCount;
    //Number of read() warnings
    unsigned long deviceWarningCount;
    //Number of read() quiet errors
    unsigned long deviceQuietCount;
    //Number of read() non quiet errors
    unsigned long deviceErrorCount;

    /**
     * Initialization
     */
    Statistics();

    /**
     * Display on given output stream the
     * textual statitics summary.
     * (default is std::cout)
     */
    void print(std::ostream& os = std::cout) const;
};

}

