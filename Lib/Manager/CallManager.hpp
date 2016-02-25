#pragma once

#include <string>
#include <mutex>
#include "types.h"
#include "Parameter.hpp"

namespace RhAL {

/**
 * CallManager
 *
 * Base class of manager use by
 * Register class to trigger
 * force Read/Write 
 * This class is used to prevent 
 * circular dependency between 
 * Register and Manager.
 */
class CallManager
{
    public:

        /**
         * Initialization
         */
        CallManager() :
            _paramScheduleMode("scheduleMode", true)
        {
        }

        /**
         * Virtual destructor
         */
        inline virtual ~CallManager()
        {
        }

        /**
         * Call when a register is declared 
         * to the Device. Use to build up
         * in Manager the set of all sorted 
         * Register pointers.
         * Register are given by its Device id
         * and its name
         */
        virtual void onNewRegister(
            id_t id, const std::string& name) = 0;

        /**
         * Call the Manager to force the immediate 
         * Read or Write of the Register given by its
         * Device id and name.
         */
        virtual void forceRegisterRead(
            id_t id, const std::string& name) = 0;
        virtual void forceRegisterWrite(
            id_t id, const std::string& name) = 0;

        /**
         * Return the current Manager send mode.
         * If true, all Registers Read and Write 
         * operation are immediatly send on the bus.
         */
        inline bool isScheduleMode() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _paramScheduleMode.value;
        }

        /**
         * Set current Manager schedule mode.
         * If true, default schedule mode is enable
         * and read/write operations are trigger by
         * Manager flush() call.
         * If false, all read/write operations
         * are immediately done on the bus.
         * flush() do nothing.
         */
        inline void setScheduleMode(bool mode)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _paramScheduleMode.value = mode;
        }

    protected:

        /**
         * Send mode. If false (default behaviour is true),
         * register read and write operation are immediately 
         * done on the bus.
         */
        ParameterBool _paramScheduleMode;

        /**
         * Mutex protecting threaded Manager state access,
         * cooperative threads synchronisation
         * but not protecting the bus access
         */
        mutable std::mutex _mutex;
};

}

