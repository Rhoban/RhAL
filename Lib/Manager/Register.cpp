#include "Register.hpp"
#include "CallManager.hpp"

namespace RhAL {

Register::Register(
    const std::string& name,
    addr_t addr,
    size_t length,
    unsigned int periodPackedRead,
    bool isForceRead,
    bool isForceWrite,
    bool isSlowRegister,
    bool isReadOnly) :
    //Member init
    id(0),
    name(name),
    addr(addr),
    length(length),
    periodPackedRead(periodPackedRead),
    isForceRead(isForceRead),
    isForceWrite(isForceWrite),
    isSlowRegister(isSlowRegister),
    isReadOnly(isReadOnly),
    _dataBufferRead(nullptr),
    _dataBufferWrite(nullptr),
    _lastDevReadUser(),
    _lastDevReadManager(),
    _lastUserWrite(),
    _needRead(false),
    _needWrite(false),
    _needSwaping(false),
    _isLastError(true),
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

void Register::init(id_t tmpId, CallManager* manager,
    data_t* bufferRead, data_t* bufferWrite)
{
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

void Register::forceRead()
{
    if (_manager == nullptr) {
        throw std::logic_error(
            "Register null manager pointer:"
            + name);
    }
    _manager->forceRegisterRead(id, name);
}
void Register::forceWrite()
{
    if (_manager == nullptr) {
        throw std::logic_error(
            "Register null manager pointer:"
            + name);
    }
    _manager->forceRegisterWrite(id, name);
}

void Register::askRead()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _needRead = true;
}
void Register::askWrite()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _needWrite = true;
}

bool Register::needRead() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _needRead;
}
bool Register::needWrite() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _needWrite;
}

void Register::selectForWrite()
{
    std::lock_guard<std::mutex> lock(_mutex);
    doConvEncode();
    _needWrite = false;
}

void Register::readyForRead()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _needRead = false;
}

void Register::finishRead(TimePoint timestamp)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _needSwaping = true;
    _lastDevReadManager = timestamp;
}

void Register::readError()
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isLastError = true;
    _needRead = true;
}

void Register::swapRead()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_needSwaping) {
        return;
    }
    _needSwaping = false;
    _isLastError = false;
    doConvDecode();
    _lastDevReadUser = _lastDevReadManager;
}

template <typename T>
TypedRegister<T>::TypedRegister(
    const std::string& name,
    addr_t addr,
    size_t length,
    FuncConvEncode<T> funcConvEncode,
    FuncConvDecode<T> funcConvDecode,
    unsigned int periodPackedRead,
    bool forceRead,
    bool forceWrite,
    bool isSlowRegister) :
    //Member init
    Register(name, addr, length, periodPackedRead,
        forceRead, forceWrite, isSlowRegister),
    funcConvEncode(funcConvEncode),
    funcConvDecode(funcConvDecode),
    _minValue(T(0)),
    _maxValue(T(0)),
    _stepValue(T(0)),
    _valueRead(),
    _valueWrite(),
    _aggregationPolicy(AggregateLast),
    _callbackOnRead([](T val){(void)val;}),
    _callbackOnWrite([](T val){(void)val;})
{
}

template <typename T>
TypedRegister<T>::TypedRegister(
    const std::string& name,
    addr_t addr,
    size_t length,
    FuncConvDecode<T> funcConvDecode,
    unsigned int periodPackedRead,
    bool forceRead,
    bool forceWrite,
    bool isSlowRegister) :
    //Member init
    Register(name, addr, length, periodPackedRead,
        forceRead, forceWrite, isSlowRegister, true),
    funcConvEncode(),
    funcConvDecode(funcConvDecode),
    _minValue(T(0)),
    _maxValue(T(0)),
    _stepValue(T(0)),
    _valueRead(),
    _valueWrite(),
    _aggregationPolicy(AggregateLast),
    _callbackOnRead([](T val){(void)val;}),
    _callbackOnWrite([](T val){(void)val;})
{
}

template <typename T>
void TypedRegister<T>::setMinValue(T val)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _minValue = val;
}
template <typename T>
void TypedRegister<T>::setMaxValue(T val)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _maxValue = val;
}
template <typename T>
void TypedRegister<T>::setStepValue(T val)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _stepValue = val;
}

template <typename T>
T TypedRegister<T>::getMinValue() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _minValue;
}
template <typename T>
T TypedRegister<T>::getMaxValue() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _maxValue;
}
template <typename T>
T TypedRegister<T>::getStepValue() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _stepValue;
}

template <typename T>
void TypedRegister<T>::setAggregationPolicy(AggregationPolicy policy)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _aggregationPolicy = policy;
}

template <typename T>
void TypedRegister<T>::setCallbackRead(std::function<void(T)> func)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _callbackOnRead = func;
}
template <typename T>
void TypedRegister<T>::setCallbackWrite(std::function<void(T)> func)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _callbackOnWrite = func;
}

template <typename T>
ReadValue<T> TypedRegister<T>::readValue()
{
    //Do immediate read on the bus
    //is the register is configured to forceWrite
    //or given Manager send mode
    if (isForceRead || !_manager->isScheduleMode()) {
        forceRead();
    }
    std::lock_guard<std::mutex> lock(_mutex);
    return ReadValue<T>(_lastDevReadUser, _valueRead, _isLastError);
}

template <typename T>
void TypedRegister<T>::writeValue(T val, bool noCallback)
{
    //Check read only
    if(isReadOnly) {
        throw std::logic_error(
            "TypedRegister write to read only Register: " 
            + name);
    }

    std::unique_lock<std::mutex> lock(_mutex);

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
    //Call user callback
    if (!noCallback) {
        _callbackOnWrite(val);
    }
    //Unlock mutex
    lock.unlock();
    //Do immediate write on the bus
    //is the register is configured to forceWrite
    //or given Manager send mode
    if (isForceWrite || !_manager->isScheduleMode()) {
        forceWrite();
    }
}

template <typename T>
T TypedRegister<T>::getWrittenValue()
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _valueWrite;
}

template <typename T>
T TypedRegister<T>::getWrittenValueAfterEncode()
{
    //Check read only
    if (isReadOnly) {
        throw std::logic_error(
            "TypedRegister get write value on read only Register: " 
            + name);
    }

    std::lock_guard<std::mutex> lock(_mutex);

    //Encode and re Decode the current write value
    data_t tmpbuffer[AddrDevLen];
    funcConvEncode(tmpbuffer, _valueWrite);
    return funcConvDecode(tmpbuffer);
}

template <typename T>
void TypedRegister<T>::doConvEncode()
{
    //Check read only
    if (isReadOnly) {
        throw std::logic_error(
            "TypedRegister conv encode on read only Register: " 
            + name);
    }
    funcConvEncode(_dataBufferWrite, _valueWrite);
}
template <typename T>
void TypedRegister<T>::doConvDecode()
{
    _valueRead = funcConvDecode(_dataBufferRead);
    //Call user callback
    _callbackOnRead(_valueRead);
}

//Template explicite instantiation
template class TypedRegister<bool>;
template class TypedRegister<int>;
template class TypedRegister<float>;

}

