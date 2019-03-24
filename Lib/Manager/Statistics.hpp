#pragma once

#include <iostream>
#include "types.h"

namespace RhAL
{
/**
 * Statistics
 *
 * Hold and accumulate
 * all Manager Statistics
 */
struct Statistics
{
  // Date at Manager instantiation
  TimePoint managerStart;
  // Number of register read or write
  // per flush current accumulator,
  // mean and max
  unsigned long regReadPerFlushAccu;
  unsigned long regReadPerFlushMax;
  double regReadPerFlushMean;
  unsigned long regWrittenPerFlushAccu;
  unsigned long regWrittenPerFlushMax;
  double regWrittenPerFlushMean;
  // Number of calls to Protocol
  // read/write/syncRead/syncWrite
  unsigned long readCount;
  unsigned long writeCount;
  unsigned long syncReadCount;
  unsigned long syncWriteCount;
  // Total length of data read or write using
  // Protocol read/write/syncRead/syncWrite calls
  unsigned long readLength;
  unsigned long writeLength;
  unsigned long syncReadLength;
  unsigned long syncWriteLength;
  // Total time duration spent during Protocol
  // read/write/syncRead/syncWrite calls
  // and maximum duration
  TimeDurationMicro sumReadDuration;
  TimeDurationMicro sumWriteDuration;
  TimeDurationMicro sumSyncReadDuration;
  TimeDurationMicro sumSyncWriteDuration;
  TimeDurationMicro maxReadDuration;
  TimeDurationMicro maxWriteDuration;
  TimeDurationMicro maxSyncReadDuration;
  TimeDurationMicro maxSyncWriteDuration;
  // Number of calls to Manager flush(),
  // waitNextFlush(), forceRead() and forceWrite()
  unsigned long flushCount;
  unsigned long waitNextFlushCount;
  unsigned long waitNextFlushCooperativeCount;
  unsigned long forceReadCount;
  unsigned long forceWriteCount;
  // Total time duration spent during
  // Manager waiting for cooperative users
  // threads at flush() begining and
  // during users waiting for Manager
  // at nextFlush()
  TimeDurationMicro waitUsersDuration;
  TimeDurationMicro waitManagerDuration;
  // Time Point of last flush() begin
  TimePoint lastFlushTimePoint;
  // Maximum abd sum duration between
  // two consecutives flush() call
  TimeDurationMicro maxFlushPeriod;
  TimeDurationMicro sumFlushPeriod;
  // Number of calls to Manager
  // emergencyStop(), exitEmergencyState()
  unsigned long emergencyCount;
  unsigned long exitEmergencyCount;
  // Number of valid (usable) receivned packets
  unsigned long deviceOKCount;
  // Number of read() warnings
  unsigned long deviceWarningCount;
  // Number of read() quiet errors
  unsigned long deviceQuietCount;
  // Number of read() non quiet errors
  unsigned long deviceErrorCount;
  // Number of detected write errors
  unsigned long writeErrorCount;

  /**
   * Initialization
   */
  Statistics();

  /**
   * Reset to zero all statistics
   */
  void reset();

  /**
   * Display on given output stream the
   * textual statitics summary.
   * (default is std::cout)
   */
  void print(std::ostream& os = std::cout) const;
};

}  // namespace RhAL
