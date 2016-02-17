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

    /**
     * Initialization
     */
    Statistics() :
        managerStart(getTimePoint()),
        readCount(0),
        writeCount(0),
        syncReadCount(0),
        syncWriteCount(0),
        readLength(0),
        writeLength(0),
        syncReadLength(0),
        syncWriteLength(0),
        readDuration(0),
        writeDuration(0),
        syncReadDuration(0),
        syncWriteDuration(0),
        flushCount(0),
        waitNextFlushCount(0),
        forceReadCount(0),
        forceWriteCount(0),
        waitUsersDuration(0),
        waitManagerDuration(0),
        emergencyCount(0),
        exitEmergencyCount(0)
    {
    }

    /**
     * Display on given output stream the
     * textual statitics summary.
     * (default is std::cout)
     */
    inline void print(std::ostream& os = std::cout) const
    {
        TimePoint now = getTimePoint();
        os << "RhAL Manager Statistics:" << std::endl;
        os << "Time since Manager creation: " << getTimeDuration
            <TimeDurationDouble>(managerStart, now).count() <<
            "s" << std::endl;
        os << "Read() calls: " << readCount << std::endl;
        os << "Write() calls: " << writeCount << std::endl;
        os << "SyncRead() calls: " << syncReadCount << std::endl;
        os << "SyncWrite() calls: " << syncWriteCount << std::endl;
        os << "Read() bytes length: " << readLength << std::endl;
        os << "Write() bytes length: " << writeLength << std::endl;
        os << "SyncRead() bytes length: " << syncReadLength << std::endl;
        os << "SyncWrite() bytes length: " << syncWriteLength << std::endl;
        os << "Flush() calls: " << flushCount << std::endl;
        os << "WaitNextFlush() calls: " << waitNextFlushCount << std::endl;
        os << "ForceRead() calls: " << forceReadCount << std::endl;
        os << "ForceWrite() calls: " << forceWriteCount << std::endl;
        os << "EmergencyStop() calls: " << emergencyCount << std::endl;
        os << "ExitEmergencyState() calls: " << exitEmergencyCount << std::endl;
        os << "Read() spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(readDuration).count() <<
            "s" << std::endl;
        os << "Write() spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(writeDuration).count() <<
            "s" << std::endl;
        os << "SyncRead() spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(syncReadDuration).count() <<
            "s" << std::endl;
        os << "SyncWrite() spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(syncWriteDuration).count() <<
            "s" << std::endl;
        os << "Waiting Users spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(waitUsersDuration).count() <<
            "s" << std::endl;
        os << "Waiting Manager spent time: " << std::chrono::duration_cast
            <TimeDurationDouble>(waitManagerDuration).count() <<
            "s" << std::endl;
    }
};

}
