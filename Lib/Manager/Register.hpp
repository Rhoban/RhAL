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
using FuncConvEncode = std::function<void(data_t*, T)>;
template <typename T>
using FuncConvDecode = std::function<T(const data_t*)>;

/**
 * Typedef for conversion function working
 * with type bool, int and float
 */
typedef FuncConvEncode<bool> FuncConvEncodeBool;
typedef FuncConvEncode<int> FuncConvEncodeInt;
typedef FuncConvEncode<float> FuncConvEncodeFloat;
typedef FuncConvDecode<bool> FuncConvDecodeBool;
typedef FuncConvDecode<int> FuncConvDecodeInt;
typedef FuncConvDecode<float> FuncConvDecodeFloat;

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
            _dataBufferRead(nullptr),
            _dataBufferWrite(nullptr),
            _lastDevReadUser(),
            _lastDevReadManager(),
            _lastUserWrite(),
            _needRead(false),
            _needWrite(false),
            _needSwaping(false),
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
         * Device id, the Manager pointer and pointer to
         * Device memory space buffer for read and write.
         */
        inline void init(id_t tmpId, CallManager* manager, 
            data_t* bufferRead, data_t* bufferWrite)
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if (manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer:" 
                    + name);
            }
            if (bufferRead == nullptr || bufferWrite == nullptr) {
                throw std::logic_error(
                    "Register null buffer pointer:" 
                    + name);
            }
            id = tmpId;
            _manager = manager;
            _dataBufferRead = bufferRead;
            _dataBufferWrite = bufferWrite;
        }

        /**
         * Perform immediate read or write 
         * operation on the bus
         */
        inline void forceRead()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer:" 
                    + name);
            }
            _manager->forceRegisterRead(id, name);
        }
        inline void forceWrite()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if (_manager == nullptr) {
                throw std::logic_error(
                    "Register null manager pointer:" 
                    + name);
            }
            _manager->forceRegisterWrite(id, name);
        }

        /**
         * Mark the register to be Read or Write
         */
        inline void askRead()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _needRead = true;
        }
        inline void askWrite()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _needWrite = true;
        }

        /**
         * Return true if the register
         * has been mark to be Read or Write
         */
        inline bool needRead() const
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _needRead;
        }
        inline bool needWrite() const
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return _needWrite;
        }

    protected:

        /**
         * Raw data buffer pointer in
         * Device memory for read and write 
         * operation.
         */
        data_t* _dataBufferRead;
        data_t* _dataBufferWrite;

        /**
         * Timestamp of last hardware read from device.
         * ReadUser is timestamp of current register typed value.
         * ReadManager is timestamp of (possibly newer) data
         * in read buffer.
         */
        TimePoint _lastDevReadUser;
        TimePoint _lastDevReadManager;

        /**
         * Last user write timestamp
         */
        TimePoint _lastUserWrite;

        /**
         * Dirty flags.
         * If true, the Register need 
         * to be selected for Read or Write 
         * on the bus.
         */
        bool _needRead;
        bool _needWrite;

        /**
         * If true, the data in read buffer are newer
         * than current typed read value. 
         * The Register has to be swap.
         */
        bool _needSwaping;

        /**
         * Pointer to a base class 
         * used to call the main manager
         */
        CallManager* _manager;

        /**
         * Mutex protecting Register member
         */
        mutable std::recursive_mutex _mutex;

        /**
         * Request conversion by derived TypedRegister
         * from typed written value to data buffer and from
         * data buffer to typed read value.
         * No thread protection.
         */
        virtual void doConvEncode() = 0;
        virtual void doConvDecode() = 0;

        /**
         * Manager has access to 
         * private members
         */
        template <typename ... T>
        friend class Manager;
        
        /**
         * Mark the register as selected for write.
         * Current write typed value is converted into
         * the write data buffer.
         * Set needWrite to false (reset aggregation).
         * (Call by Manager)
         */
        inline void selectForWrite()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            doConvEncode();
            _needWrite = false;
        }

        /**
         * Mark the register as read operation
         * has begins. Reset read dirty flag.
         */
        inline void readyForRead()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _needRead = false;
        }

        /**
         * Mark the register as read end and need
         * swapping.
         * Given timestamp is the date at read receive.
         * (Call by manager so doesn't need for thread protection)
         */
        inline void finishRead(TimePoint timestamp)
        {
            _needSwaping = true;
            _lastDevReadManager = timestamp;
        }

        /**
         * If the register swap is needed,
         * convert the read data buffer into
         * typed read value and assign the new timestamp.
         * (Call by Manager)
         */
        inline void swapRead()
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            if (!_needSwaping) {
                return;
            }
            _needSwaping = false;
            doConvDecode();
            _lastDevReadUser = _lastDevReadManager;
        }
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
        const FuncConvEncode<T> funcConvEncode;
        
        /**
         * Conversion functions from
         * data buffer to typed value
         */
        const FuncConvDecode<T> funcConvDecode;

        /**
         * Initialization with Register 
         * configuration and:
         * funcConvEncode: convertion function
         * from typed value to data buffer.
         * funcConvDecode: convertion function
         * from data buffer to typed value.
         */
        TypedRegister(
            const std::string& name, 
            addr_t addr, 
            size_t length, 
            FuncConvEncode<T> funcConvEncode,
            FuncConvDecode<T> funcConvDecode,
            unsigned int periodPackedRead = 0,
            bool forceRead = false,
            bool forceWrite = false) :
            //Member init
            Register(name, addr, length, 
                periodPackedRead, forceRead, forceWrite),
            funcConvEncode(funcConvEncode),
            funcConvDecode(funcConvDecode),
            _valueRead(),
            _valueWrite(),
            _aggregationPolicy(AggregateLast)
        {
        }

        /**
         * Set the register
         * aggregation policy
         */
        inline void setAggregationPolicy(AggregationPolicy policy)
        {
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            _aggregationPolicy = policy;
        }

        /**
         * Return the last read value from
         * the hardware. The returned timestamp
         * is the time when data are received from the bus.
         */
        inline TimedValue<T> readValue()
        {
            //Do immediate read on the bus
            //is the register is configured to forceWrite
            //or given Manager send mode
            if (isForceRead || !_manager->isScheduleMode()) {
                forceRead();
            }
            std::lock_guard<std::recursive_mutex> lock(_mutex);
            return TimedValue<T>(_lastDevReadUser, _valueRead);
        }

        /**
         * Set the current contained typed value. 
         * If the register is written multiple times
         * between write operations, values are aggregated
         * according with current aggregation policy.
         * The register is marked to be written.
         */
        inline void writeValue(T val)
        {
            std::unique_lock<std::recursive_mutex> lock(_mutex);
            
            //Compute aggregation if the value
            //has already been written
            if (_needWrite) {
                _valueWrite = aggregateValue(
                    _aggregationPolicy, _valueWrite, val);
            } else {
                _valueWrite = val;
            }
            //Assign the timestamp
            _lastUserWrite = getTimePoint();
            //Mark as dirty
            _needWrite = true;
            //Unlock mutex
            lock.unlock();
            //Do immediate write on the bus
            //is the register is configured to forceWrite
            //or given Manager send mode
            if (isForceWrite || !_manager->isScheduleMode()) {
                forceWrite();
            }
        }

    protected:

        /**
         * Inherit.
         * Request conversion by derived TypedRegister
         * from typed written value to data buffer and from
         * data buffer to typed read value.
         * No thread protection.
         */
        inline virtual void doConvEncode() override
        {
            funcConvEncode(_dataBufferWrite, _valueWrite);
        }
        inline virtual void doConvDecode() override
        {
            _valueRead = funcConvDecode(_dataBufferRead);
        }

    private:

        /**
         * Typed Register value.
         * ValueWrite is the user aggregated 
         * last write.
         * ValueRead is the current hardware 
         * register read value. Possible newer value
         * is in data read buffer waiting for swapping.
         */
        T _valueRead;
        T _valueWrite;
        
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

