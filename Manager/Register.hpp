#pragma once

#include <functional>
#include <stdexcept>
#include <mutex>
#include "types.h"
#include "CallManager.hpp"
#include "Aggregation.h"

namespace RhAL {

/**
 * Compile time constante for
 * register data buffer maximum 
 * length in bytes
 */
constexpr size_t MaxRegisterLength = 4;

/**
 * Template alias for conversion function
 * from value to raw data buffer and inverse
 */
template <typename T>
using FuncConvIn = std::function<void(data_t*, T)>;
template <typename T>
using FuncConvOut = std::function<T(const data_t*)>;

/**
 * Typedef for conversion function working
 * with type bool, int and float
 */
typedef FuncConvIn<bool> FuncConvInBool;
typedef FuncConvIn<int> FuncConvInInt;
typedef FuncConvIn<float> FuncConvInFloat;
typedef FuncConvOut<bool> FuncConvOutBool;
typedef FuncConvOut<int> FuncConvOutInt;
typedef FuncConvOut<float> FuncConvOutFloat;

/**
 * Register
 *
 * Named data register
 * in device memory used to
 * communicate with.
 */
class Register
{
    public:

        /**
         * Associated Device id
         * Set by RegistersList
         */
        id_t id;
        
        /**
         * Textual name
         */
        const std::string name;

        /**
         * Address in device memory
         */
        const addr_t addr;

        /**
         * Register length in bytes
         */
        const size_t length;

        /**
         * Register Read period in flush cycle.
         * If 0, the register is not Read.
         * If 1, the register is Read every flush Read.
         * If n, the register is Real every n flush Read.
         */
        const unsigned int periodPackedRead;

        /**
         * If true, Read or Write
         * are immediatly sent to the bus
         */
        const bool isForceRead;
        const bool isForceWrite;

        /**
         * Initialize and allocate a new register 
         * with given:
         * name: textual name unique to the device.
         * addr: address in device memory.
         * length: data buffer length in bytes.
         * periodPackedRead: Disable or enable the register
         * read from hardware every given readFlush().
         * forceRead: Do not packed read operation.
         * forceWrite: Do not packed write operation.
         */
        inline Register(
            const std::string& name, 
            addr_t addr, 
            size_t length, 
            unsigned int periodPackedRead = 0,
            bool isForceRead = false,
            bool isForceWrite = false) :
            //Member init
            id(0),
            name(name),
            addr(addr),
            length(length),
            periodPackedRead(periodPackedRead),
            isForceRead(isForceRead),
            isForceWrite(isForceWrite),
            _dataBuffer(nullptr),
            _lastDevRead1(),
            _lastDevRead2(),
            _lastUserWrite(),
            _needRead1(false),
            _needRead2(false),
            _needWrite1(false),
            _needWrite2(false),
            _manager(nullptr),
            _mutex()
        {
            if (length > MaxRegisterLength) {
                throw std::logic_error(
                    "Register length invalid with static max length: " 
                    + name);
            }
            if (addr+length >= AddrDevLen) {
                throw std::logic_error(
                    "Register address/length is outside static memory range: "
                    + name);
            }
        }

        /**
         * Initialize the Register with the associated 
         * Device id and the Manager pointer
         */
        inline void init(id_t tmpId, CallManager* manager, data_t* data)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer:" 
                    + name);
            }
            if (data == nullptr) {
                throw std::logic_error(
                    "Register null data pointer:" 
                    + name);
            }
            id = tmpId;
            _manager = manager;
            _dataBuffer = data;
        }

        /**
         * Performe immediate Read or Write
         * operation on the bus
         */
        /* TODO
        inline void forceRead()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer: " 
                    + name);
            }
            //Call the Manager with coordinate 
            //of this Register
            _manager->forceRegisterRead(id, name);
        }
        inline void forceWrite()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer: " 
                    + name);
            }
            //Call the Manager with coordinate 
            //of this Register
            _manager->forceRegisterWrite(id, name);
        }
        */

        /**
         * Mark the register to be Read or Write
         * at next flushRead() or flushWrite()
         */
        inline void askRead()
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();

            std::unique_lock<std::mutex> lock(_mutex);
            //Assign double buffered needRead
            if (bufferMode) {
                _needRead1 = true;
            } else {
                _needRead2 = true;
            }
            lock.unlock();
            
            //Notify the end of double buffer use
            _manager->postUserOperations();
        }
        inline void askWrite()
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();

            std::unique_lock<std::mutex> lock(_mutex);
            //Assign double buffered needWrite
            if (bufferMode) {
                _needWrite1 = true;
            } else {
                _needWrite2 = true;
            }
            lock.unlock();
            
            //Notify the end of double buffer use
            _manager->postUserOperations();
        }

        /**
         * Return true if the register
         * has been mark to be Read or Write
         */
        inline bool needRead() const
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();

            std::unique_lock<std::mutex> lock(_mutex);
            //Retrieve double buffered needRead
            bool val;
            if (bufferMode) {
                val = _needRead1;
            } else {
                val = _needRead2;
            }
            lock.unlock();
            
            //Notify the end of double buffer use
            _manager->postUserOperations();

            return val;
        }
        inline bool needWrite() const
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();

            std::unique_lock<std::mutex> lock(_mutex);
            //Retrieve double buffered needWrite
            bool val;
            if (bufferMode) {
                val = _needWrite1;
            } else {
                val = _needWrite2;
            }
            lock.unlock();
            
            //Notify the end of double buffer use
            _manager->postUserOperations();

            return val;
        }

    protected:

        /**
         * Raw data buffer pointer in
         * Device memory.
         */
        data_t* _dataBuffer;

        /**
         * Double buffered Timestamp at last read 
         * from device and last write from user
         */
        TimePoint _lastDevRead1;
        TimePoint _lastDevRead2;
        TimePoint _lastUserWrite;


        /**
         * Double buffered dirty flags.
         * If true, the Register need 
         * to be Read or Write on the bus.
         */
        bool _needRead1;
        bool _needRead2;
        bool _needWrite1;
        bool _needWrite2;

        /**
         * Pointer to a base class 
         * used to call the main manager
         */
        CallManager* _manager;

        /**
         * Mutex protecting Register meta data
         */
        mutable std::mutex _mutex;

        /**
         * Request conversion by derived TypedRegister
         * from typed written value to data buffer and from
         * data buffer to typed read value.
         * Used given buffer mode for double buffered value.
         * No thread protection.
         */
        virtual void doConvIn(bool bufferMode) = 0;
        virtual void doConvOut(bool bufferMode) = 0;

        /**
         * Manager has access to 
         * private members
         */
        template <typename ... T>
        friend class Manager;
};

/**
 * TypedRegister
 *
 * Register view as a typed register
 * of template type bool, int or float.
 * Allow get and set typed value in and out 
 * of the register through conversion functions.
 */
template <typename T>
class TypedRegister : public Register
{
    public:

        /**
         * Conversion functions from
         * typed value to data buffer
         */
        const FuncConvIn<T> funcConvIn;
        
        /**
         * Conversion functions from
         * data buffer to typed value
         */
        const FuncConvOut<T> funcConvOut;

        /**
         * Initialization with Register 
         * configuration and:
         * funcConvIn: convertion function 
         * from typed value to data buffer.
         * funcConvOut: convertion function 
         * from data buffer to typed value.
         */
        TypedRegister(
            const std::string& name, 
            addr_t addr, 
            size_t length, 
            FuncConvIn<T> funcConvIn,
            FuncConvOut<T> funcConvOut,
            unsigned int periodPackedRead = 0,
            bool forceRead = false,
            bool forceWrite = false) :
            //Member init
            Register(name, addr, length, 
                periodPackedRead, forceRead, forceWrite),
            funcConvIn(funcConvIn),
            funcConvOut(funcConvOut),
            _valueRead1(),
            _valueRead2(),
            _valueWrite1(),
            _valueWrite2(),
            _aggregationPolicy(AggregateLast)
        {
        }

        /**
         * Set the register
         * aggregation policy
         */
        inline void setAggregationPolicy(AggregationPolicy policy)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _aggregationPolicy = policy;
        }

        /**
         * Return the last read value from
         * the hardware. The returned timestamp
         * is the time when data are received from the bus.
         */
        inline TimedValue<T> readValue() const
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();
            
            std::unique_lock<std::mutex> lock(_mutex);
            //Retrieve last read value and TimePoint
            TimePoint timestamp;
            T value;
            if (bufferMode) {
                timestamp = _lastDevRead1;
                value = _valueRead1;
            } else {
                timestamp = _lastDevRead2;
                value = _valueRead2;
            }
            lock.unlock();

            //Notify the end of double buffer use
            _manager->postUserOperations();
            
            return TimedValue<T>(timestamp, value);
        }

        /**
         * Set the current contained
         * typed value. 
         * The register is marked to
         * be written.
         * If the register configuration
         * is set as immediate Write or if
         * the manager is in immediante send mode,
         * the register Write operation
         * is immediatly done on the bus.
         */
        inline void writeValue(T val)
        {
            //Wait for double buffer swapping
            bool bufferMode = _manager->preUserOperations();

            std::unique_lock<std::mutex> lock(_mutex);
            T aggregated = val;
            if (bufferMode) {
                //Compute aggregation if the value
                //has already been written
                if (_needWrite1) {
                    aggregated = aggregateValue(
                        _aggregationPolicy, _valueWrite1, val);
                }
                //Assign the value and timestamp
                _valueWrite1 = aggregated;
                _lastUserWrite = getCurrentTimePoint();
                //Mark as dirty
                _needWrite1 = true;
            } else {
                //Compute aggregation if the value
                //has already been written
                if (_needWrite2) {
                    aggregated = aggregateValue(
                        _aggregationPolicy, _valueWrite2, val);
                }
                //Assign the value and timestamp
                _valueWrite2 = aggregated;
                _lastUserWrite = getCurrentTimePoint();
                //Mark as dirty
                _needWrite2 = true;
            }
            lock.unlock();

            //Notify the end of double buffer use
            _manager->postUserOperations();
        }

    protected:

        /**
         * Inherit.
         * Request conversion by derived TypedRegister
         * from typed written value to data buffer and from
         * data buffer to typed read value.
         * Used given buffer mode for double buffered value.
         * No thread protection.
         */
        inline virtual void doConvIn(bool bufferMode)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (bufferMode) {
                funcConvIn(_dataBuffer, _valueWrite1);
            } else {
                funcConvIn(_dataBuffer, _valueWrite2);
            }
        }
        inline virtual void doConvOut(bool bufferMode)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            if (bufferMode) {
                _valueRead1 = funcConvOut(_dataBuffer);
            } else {
                _valueRead2 = funcConvOut(_dataBuffer);
            }
        }

    private:

        /**
         * Double buffered Typed Register value 
         * of last aggregated user write and 
         * last hardware read value
         */
        T _valueRead1;
        T _valueRead2;
        T _valueWrite1;
        T _valueWrite2;
        
        /**
         * Value Aggregation policy
         */
        AggregationPolicy _aggregationPolicy;
};

/**
 * Typedef for TypedRegister
 */
typedef TypedRegister<bool> TypedRegisterBool;
typedef TypedRegister<int> TypedRegisterInt;
typedef TypedRegister<float> TypedRegisterFloat;

}

