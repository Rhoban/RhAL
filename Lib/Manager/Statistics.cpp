#include "Statistics.hpp"
#include "utils.h"

namespace RhAL {
    
Statistics::Statistics() :
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
    waitNextFlushCooperativeCount(0),
    forceReadCount(0),
    forceWriteCount(0),
    waitUsersDuration(0),
    waitManagerDuration(0),
    emergencyCount(0),
    exitEmergencyCount(0),
    deviceOKCount(0),
    deviceWarningCount(0),
    deviceQuietCount(0),
    deviceErrorCount(0)
{
}

void Statistics::print(std::ostream& os) const
{
    TimePoint now = getTimePoint();
    os << "RhAL Manager Statistics:" << std::endl;
    os << "------------------------" << std::endl;
    os << "Time since Manager creation: " << 
        duration_float(managerStart, now) << "s" << std::endl;
    os << "Read() calls: " << readCount << std::endl;
    os << "Write() calls: " << writeCount << std::endl;
    os << "SyncRead() calls: " << syncReadCount << std::endl;
    os << "SyncWrite() calls: " << syncWriteCount << std::endl;
    os << "Read() bytes length: " << readLength << std::endl;
    os << "Write() bytes length: " << writeLength << std::endl;
    os << "SyncRead() bytes length: " << syncReadLength << std::endl;
    os << "SyncWrite() bytes length: " << syncWriteLength << std::endl;
    os << "Flush() calls: " << flushCount << std::endl;
    os << "WaitNextFlush() cooperative calls: " << waitNextFlushCooperativeCount << std::endl;
    os << "WaitNextFlush() not cooperative calls: " << waitNextFlushCount << std::endl;
    os << "ForceRead() calls: " << forceReadCount << std::endl;
    os << "ForceWrite() calls: " << forceWriteCount << std::endl;
    os << "EmergencyStop() calls: " << emergencyCount << std::endl;
    os << "ExitEmergencyState() calls: " << exitEmergencyCount << std::endl;
    os << "Read() spent time: " << 
        duration_float(readDuration) << "s" << std::endl;
    os << "Write() spent time: " << 
        duration_float(writeDuration) << "s" << std::endl;
    os << "SyncRead() spent time: " << 
        duration_float(syncReadDuration) << "s" << std::endl;
    os << "SyncWrite() spent time: " << 
        duration_float(syncWriteDuration) << "s" << std::endl;
    os << "Waiting in flush() spent time: " << 
        duration_float(waitUsersDuration) << "s" << std::endl;
    os << "Waiting in WaitNextFlush() spent time: " << 
        duration_float(waitManagerDuration) << "s" << std::endl;
    os << "Devices valid responses: " << deviceOKCount << std::endl;
    os << "Devices warning responses: " << deviceWarningCount << std::endl;
    os << "Devices quiet responses: " << deviceQuietCount << std::endl;
    os << "Devices error responses: " << deviceErrorCount << std::endl;
}

}

