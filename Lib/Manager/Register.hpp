#pragma once

#include <functional>
#include <stdexcept>
#include <mutex>
#include "types.h"
#include "timestamp.h"
#include "Aggregation.h"

namespace RhAL
{
// Forward declaration
class CallManager;

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
   * If true, a sleep delay will be added after
   * writing to the register
   */
  const bool isSlowRegister;

  /**
   * If true, the register is read only.
   * Write operations are disallowed.
   */
  const bool isReadOnly;

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
   * isSlowRegister : If true, marks the buffer as slow.
   * Some registers are slow to write on the hardware
   * (flash memory takes 20-40 ms to write).
   * isReadOnly: if true, write operation
   * are disallowed on this register.
   */
  Register(const std::string& name, addr_t addr, size_t length, unsigned int periodPackedRead = 0,
           bool isForceRead = false, bool isForceWrite = false, bool isSlowRegister = false, bool isReadOnly = false);

  /**
   * Initialize the Register with the associated
   * Device id, the Manager pointer and pointer to
   * Device memory space buffer for read and write.
   */
  void init(id_t tmpId, CallManager* manager, data_t* bufferRead, data_t* bufferWrite);

  /**
   * Perform immediate read or write
   * operation on the bus
   */
  void forceRead();
  void forceWrite();

  /**
   * Mark the register to be Read or Write
   */
  void askRead();
  void askWrite();

  /**
   * Return true if the register
   * has been mark to be Read or Write
   */
  bool needRead() const;
  bool needWrite() const;

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
   * If true, the last read attempt
   * on this register has failed
   * (read error).
   * If false, the current user
   * read value has not been updated
   */
  bool _isLastReadError;

  /**
   * If true, the last write attempt
   * on this register has failed
   * (write error) and have to be re send
   * or replace with aggregation reseted.
   */
  bool _isLastWriteError;

  /**
   * Pointer to a base class
   * used to call the main manager
   */
  CallManager* _manager;

  /**
   * Mutex protecting Register member
   */
  mutable std::mutex _mutex;

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
  friend class BaseManager;

  /**
   * Mark the register as selected for write.
   * Current write typed value is converted into
   * the write data buffer.
   * Set needWrite to false.
   * (Call by Manager)
   */
  void selectForWrite();

  /**
   * Mark the register as read operation
   * has begins. Reset read dirty flag.
   * (Call by manager)
   */
  void readyForRead();

  /**
   * Mark the register as read end and need
   * swapping.
   * Given timestamp is the date at read receive.
   * (Call by manager)
   */
  void finishRead(TimePoint timestamp);

  /**
   * Mark the register as last read
   * operation has failed.
   * Re mark the register to be read again.
   * (Call by Manager)
   */
  void readError();

  /**
   * Mark the register as last write
   * operatio failed.
   * Re mark the regiser as needed
   * to be write again.
   */
  void writeError();

  /**
   * If the register swap is needed,
   * convert the read data buffer into
   * typed read value and assign the new timestamp.
   * (Call by Manager)
   */
  void swapRead();
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
  TypedRegister(const std::string& name, addr_t addr, size_t length, FuncConvEncode<T> funcConvEncode,
                FuncConvDecode<T> funcConvDecode, unsigned int periodPackedRead = 0, bool forceRead = false,
                bool forceWrite = false, bool isSlowRegister = false);

  /**
   * Initialization for ReadOnly Register and
   * configuration.
   * funcConvEncode is not initialized and bool
   * isReadOnly is true.
   */
  TypedRegister(const std::string& name, addr_t addr, size_t length, FuncConvDecode<T> funcConvDecode,
                unsigned int periodPackedRead = 0, bool forceRead = false, bool forceWrite = false,
                bool isSlowRegister = false);

  /**
   * Read and write access for range
   * and step values.
   * Values are considered non defined if
   * they are all equals to 0 (T(0)).
   */
  void setMinValue(T val);
  void setMaxValue(T val);
  void setStepValue(T val);
  T getMinValue() const;
  T getMaxValue() const;
  T getStepValue() const;

  /**
   * Set the register
   * aggregation policy
   */
  void setAggregationPolicy(AggregationPolicy policy);

  /**
   * Set the on user write and on
   * manager read callback. The updated
   * value is given as calback argument.
   */
  void setCallbackRead(std::function<void(T)> func);
  void setCallbackWrite(std::function<void(T)> func);

  /**
   * Return the last read value from
   * the hardware. The returned timestamp
   * is the time when data are received from the bus.
   */
  ReadValue<T> readValue();

  /**
   * Set the current contained typed value.
   * If the register is written multiple times
   * between write operations, values are aggregated
   * according with current aggregation policy.
   * The register is marked to be written.
   * If noCallback is true, the on write callback
   * is not called.
   */
  void writeValue(T val, bool noCallback = false);

  /**
   * Return the current write value
   * that has been written by writeValue()
   * and aggregation policy.
   */
  T getWrittenValue() const;

  /**
   * Return the value set by user
   * Encode and Decode really send
   * on the bus.
   */
  T getWrittenValueAfterEncode() const;

  /**
   * Operator overload assignement
   * and static cast shortcut
   * for writeValue() and readValue()
   */
  void operator=(const T& val);
  operator T();

protected:
  /**
   * Inherit.
   * Request conversion by derived TypedRegister
   * from typed written value to data buffer and from
   * data buffer to typed read value.
   * No thread protection.
   */
  virtual void doConvEncode() override;
  virtual void doConvDecode() override;

private:
  /**
   * Additional optional range values and minimum
   * step value for the register.
   * Useful mainly for the Float Register type.
   * The values are supposed non defined if they are
   * all equals to 0 (T(0)).
   */
  T _minValue;
  T _maxValue;
  T _stepValue;

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

  /**
   * User callback called on user write
   * and on successfull manager read
   * (during swapRead).
   * Written or read value is given
   * as callback argument
   */
  std::function<void(T)> _callbackOnRead;
  std::function<void(T)> _callbackOnWrite;
};

/**
 * Typedef for TypedRegister
 */
typedef TypedRegister<bool> TypedRegisterBool;
typedef TypedRegister<int> TypedRegisterInt;
typedef TypedRegister<float> TypedRegisterFloat;

}  // namespace RhAL
