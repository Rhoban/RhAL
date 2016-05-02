#include "Statistics.hpp"
#include "utils.h"

namespace RhAL {
    
Statistics::Statistics() :
    managerStart(getTimePoint())
{
    reset();
}
    
void Statistics::reset()
{
    readCount = 0;
    writeCount = 0;
    syncReadCount = 0;
    syncWriteCount = 0;
    readLength = 0;
    writeLength = 0;
    syncReadLength = 0;
    syncWriteLength = 0;
    sumReadDuration = TimeDurationMicro(0);
    sumWriteDuration = TimeDurationMicro(0);
    sumSyncReadDuration = TimeDurationMicro(0);
    sumSyncWriteDuration = TimeDurationMicro(0);
    maxReadDuration = TimeDurationMicro(0);
    maxWriteDuration = TimeDurationMicro(0);
    maxSyncReadDuration = TimeDurationMicro(0);
    maxSyncWriteDuration = TimeDurationMicro(0);
    flushCount = 0;
    waitNextFlushCount = 0;
    waitNextFlushCooperativeCount = 0;
    forceReadCount = 0;
    forceWriteCount = 0;
    waitUsersDuration = TimeDurationMicro(0);
    waitManagerDuration = TimeDurationMicro(0);
    lastFlushTimePoint = TimePoint();
    maxFlushPeriod = TimeDurationMicro(0);
    sumFlushPeriod = TimeDurationMicro(0);
    emergencyCount = 0;
    exitEmergencyCount = 0;
    deviceOKCount = 0;
    deviceWarningCount = 0;
    deviceQuietCount = 0;
    deviceErrorCount = 0;
}

void Statistics::print(std::ostream& os) const
{
    TimePoint now = getTimePoint();
    os << "RhAL Manager Statistics:" << std::endl;
    os << "------------------------" << std::endl;
    os << "Time since Manager creation: " << 
        duration_float(managerStart, now) << "s" << std::endl;
    os << "Flush() calls: " << flushCount << std::endl;
    if (flushCount > 1) {
        os << "Mean Manager flush() period: " << 
            duration_float(sumFlushPeriod)/(flushCount-1.0) << "s" << std::endl;
    }
    os << "Max Manager flush() period: " << 
        duration_float(maxFlushPeriod) << "s" << std::endl;
    os << "WaitNextFlush() cooperative calls: " << waitNextFlushCooperativeCount << std::endl;
    os << "WaitNextFlush() not cooperative calls: " << waitNextFlushCount << std::endl;
    os << "Waiting in flush() spent time: " << 
        duration_float(waitUsersDuration) << "s" << std::endl;
    os << "Waiting in WaitNextFlush() spent time: " << 
        duration_float(waitManagerDuration) << "s" << std::endl;
    os << "ForceRead() calls: " << forceReadCount << std::endl;
    os << "ForceWrite() calls: " << forceWriteCount << std::endl;
    os << "EmergencyStop() calls: " << emergencyCount << std::endl;
    os << "ExitEmergencyState() calls: " << exitEmergencyCount << std::endl;
    os << "Read() calls: " << readCount << std::endl;
    os << "Read() bytes length: " << readLength << std::endl;
    os << "Read() sum spent time: " << 
        duration_float(sumReadDuration) << "s" << std::endl;
    if (readCount > 0) {
        os << "Read() mean spent time: " << 
            duration_float(sumReadDuration)/readCount << "s" << std::endl;
    }
    os << "Read() max spent time: " << 
        duration_float(maxReadDuration) << "s" << std::endl;
    os << "Write() calls: " << writeCount << std::endl;
    os << "Write() bytes length: " << writeLength << std::endl;
    os << "Write() sum spent time: " << 
        duration_float(sumWriteDuration) << "s" << std::endl;
    if (writeCount > 0) {
        os << "Write() mean spent time: " << 
            duration_float(sumWriteDuration)/writeCount << "s" << std::endl;
    }
    os << "Write() max spent time: " << 
        duration_float(maxWriteDuration) << "s" << std::endl;
    os << "SyncRead() calls: " << syncReadCount << std::endl;
    os << "SyncRead() bytes length: " << syncReadLength << std::endl;
    os << "SyncRead() sum spent time: " << 
        duration_float(sumSyncReadDuration) << "s" << std::endl;
    if (syncReadCount > 0) {
        os << "SyncRead() mean spent time: " << 
            duration_float(sumSyncReadDuration)/syncReadCount << "s" << std::endl;
    }
    os << "SyncRead() max spent time: " << 
        duration_float(maxSyncReadDuration) << "s" << std::endl;
    os << "SyncWrite() calls: " << syncWriteCount << std::endl;
    os << "SyncWrite() bytes length: " << syncWriteLength << std::endl;
    os << "SyncWrite() sum spent time: " << 
        duration_float(sumSyncWriteDuration) << "s" << std::endl;
    if (syncWriteCount > 0) {
        os << "SyncWrite() mean spent time: " << 
            duration_float(sumSyncWriteDuration)/syncWriteCount << "s" << std::endl;
    }
    os << "SyncWrite() max spent time: " << 
        duration_float(maxSyncWriteDuration) << "s" << std::endl;
    os << "Devices valid responses: " << deviceOKCount << std::endl;
    os << "Devices warning responses: " << deviceWarningCount << std::endl;
    os << "Devices quiet responses: " << deviceQuietCount << std::endl;
    os << "Devices error responses: " << deviceErrorCount << std::endl;
}

}

