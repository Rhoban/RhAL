#include "PressureSensor.hpp"

namespace RhAL {

PressureSensor::PressureSensor(const std::string& name, id_t id) :
    Device(name, id),
    //Read only registers
    //(name, address, size, decodeFunction, updateFreq, forceRead=false, forceWrite=false, isSlow=false)
    _pressure1("pressure1", 0x24, 3, [this](const data_t* data) -> int {
        std::lock_guard<std::mutex> lock(this->_mutex);
        int value = convDecode_3Bytes(data);
        return value - (int)this->_zero1.value;
    }, 1),
    _pressure2("pressure2", 0x27, 3, [this](const data_t* data) -> int {
        std::lock_guard<std::mutex> lock(this->_mutex);
        int value = convDecode_3Bytes(data);
        return value - (int)this->_zero2.value;
    }, 1),
    _pressure3("pressure3", 0x2A, 3, [this](const data_t* data) -> int {
        std::lock_guard<std::mutex> lock(this->_mutex);
        int value = convDecode_3Bytes(data);
        return value - (int)this->_zero3.value;
    }, 1),
    _pressure4("pressure4", 0x2D, 3, [this](const data_t* data) -> int {
        std::lock_guard<std::mutex> lock(this->_mutex);
        int value = convDecode_3Bytes(data);
        return value - (int)this->_zero4.value;
    }, 1),
    //Parameters configuration
    _zero1("zero1", 0.0),
    _zero2("zero2", 0.0),
    _zero3("zero3", 0.0),
    _zero4("zero4", 0.0)
{
}

TypedRegisterInt& PressureSensor::pressure1()
{
    return _pressure1;
}
TypedRegisterInt& PressureSensor::pressure2()
{
    return _pressure2;
}
TypedRegisterInt& PressureSensor::pressure3()
{
    return _pressure3;
}
TypedRegisterInt& PressureSensor::pressure4()
{
    return _pressure4;
}

float PressureSensor::getZero1() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _zero1.value;
}
void PressureSensor::setZero1(float value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _zero1.value = value;
}
float PressureSensor::getZero2() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _zero2.value;
}
void PressureSensor::setZero2(float value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _zero2.value = value;
}
float PressureSensor::getZero3() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _zero3.value;
}
void PressureSensor::setZero3(float value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _zero3.value = value;
}
float PressureSensor::getZero4() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _zero4.value;
}
void PressureSensor::setZero4(float value)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _zero4.value = value;
}

void PressureSensor::onInit()
{
    Device::registersList().add(&_pressure1);
    Device::registersList().add(&_pressure2);
    Device::registersList().add(&_pressure3);
    Device::registersList().add(&_pressure4);
    Device::parametersList().add(&_zero1);
    Device::parametersList().add(&_zero2);
    Device::parametersList().add(&_zero3);
    Device::parametersList().add(&_zero4);
}

}

