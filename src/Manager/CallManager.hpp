#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include "types.h"

namespace RhAL {

/**
 * CallManager
 *
 * Base class of manager use by
 * Register class to trigger
 * force Read/Write and implement
 * register double buffer.
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
            _bufferMode(false),
            _isPackedMode(true),
            _mutex(),
            _countUsers(0),
            _isBufferSwapping(false),
            _waitUserEnd(),
            _waitSwappingEnd()
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
        /* TODO
        virtual void forceRegisterRead(
            id_t id, const std::string& name) = 0;
        virtual void forceRegisterWrite(
            id_t id, const std::string& name) = 0;
        */

        /**
         * Return the current Manager send mode.
         * If true, all Registers Read and Write 
         * operation are immediatly send on the bus.
         */
        /* TODO
        inline bool isPackedMode() const
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _isPackedMode;
        }
        */

        /**
         * Before any operation overs registers double
         * buffers, preUserOperations() has to be called.
         * Current buffer mode owns by the user is then
         * returned.
         * If false, the buffer 2 is owned by the user.
         * Tf true, the buffer 1 is owned by the user.
         * After any operations, postUserOperations()
         * has to be called.
         */
        inline bool preUserOperations() const
        {
            //Lock the shared mutex
            std::unique_lock<std::mutex> lock(_mutex);
            //If swapping buffer has begun
            if (_isBufferSwapping) {
                //Wait for the end of swapping
                //(during wait, the shared mutex is released)
                _waitSwappingEnd.wait(lock, 
                    [this](){return !_isBufferSwapping;});
            }
            //The lock is now acquired
            //Increment current user count
            _countUsers++;
            //Release the shared mutex
            lock.unlock();
            //Current buffer mode is returned
            return _bufferMode;
        }
        inline void postUserOperations() const
        {
            //Lock the shared mutex
            std::unique_lock<std::mutex> lock(_mutex);
            //Decrement current user count
            _countUsers--;
            //Release the shared mutex
            lock.unlock();
            //Wake up swapping buffer thread
            //waiting for user end
            _waitUserEnd.notify_all();
        }

    protected:
        
        /**
         * If false, the buffer 1 is owned by
         * the manager and the buffer 2 is owned by
         * the user. If true, manager owns buffer 2
         * and use buffer 1.
         */
        bool _bufferMode;
        
        /**
         * Exchange all data buffers
         * used by all registers
         */
        inline void swapDataBuffers()
        {
            //Lock the mutex 
            std::unique_lock<std::mutex> lock(_mutex);
            //Mark the manager as currently swapping
            _isBufferSwapping = true;
            //Wait for all users to finish buffer operations
            //(during wait, the shared mutex is released)
            _waitUserEnd.wait(lock, 
                [this](){return _countUsers == 0;});
            //The lock is now acquired
            //Swap double buffer
            _bufferMode = !_bufferMode;
            //Unmark the manager as swapping
            _isBufferSwapping = false;
            //Release the shared mutex
            lock.unlock();
            //Wake up user thread waiting for
            //swapping end
            _waitSwappingEnd.notify_all();
        }

        /**
         * Assign the current send mode.
         * If true (default), register operation
         * are packed.
         */
        /* TODO
        inline void setSendMode(bool mode)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _isPackedMode = mode;
        }
        */
        
    private:

        /**
         * Send mode. If false (default behaviour is true),
         * register read and write operation are immediately 
         * done on the bus.
         */
        bool _isPackedMode;


        /**
         * Mutex protecting the double
         * data buffer swapping shared by
         * the manager and all registers
         */
        mutable std::mutex _mutex;

        /**
         * The number of currently
         * user operation on double 
         * buffers
         */
        mutable int _countUsers;

        /**
         * True if the double buffer
         * swapping has been stared and 
         * is not finished
         */
        bool _isBufferSwapping;

        /**
         * Condition variable for
         * manager waiting that user
         * operations end and for
         * user waiting current buffer
         * swapping end
         */
        mutable std::condition_variable _waitUserEnd;
        mutable std::condition_variable _waitSwappingEnd;
};

}

