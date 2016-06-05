#include "Device.hpp"
#include "CallManager.hpp"

namespace RhAL {

Device::Device(const std::string& name, id_t id) :
    _mutex(),
    _registersList(id),
    _parametersList(),
    _name(name),
    _id(id),
    _manager(nullptr),
    _isPresent(false),
    _isWarning(false),
    _isError(false),
    _countWarnings(0),
    _countErrors(0)
{
    if (id < IdDevBegin || id > IdDevEnd) {
        throw std::logic_error(
            "Device id is outside static range: " 
            + name);
    }
}

Device::~Device()
{
}

void Device::setManager(CallManager* manager)
{
    if (manager == nullptr) {
        throw std::logic_error(
            "Device null manager pointer: "
            + _name);
    }
    _manager = manager;
    _registersList.setManager(_manager);
}

void Device::init()
{
    if (_manager == nullptr) {
        throw std::logic_error(
            "Device null manager pointer: "
            + _name);
    }
    //Call 
    onInit();
}

const std::string& Device::name() const
{
    return _name;
}

id_t Device::id() const
{
    return _id;
}

bool Device::isPresent() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _isPresent;
}

bool Device::isWarning() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _isWarning;
}

bool Device::isError() const
{
    std::lock_guard<std::mutex> lock(_mutex);
    return _isError;
}

unsigned long Device::countWarnings() const
{
    return _countWarnings;
}
unsigned long Device::countErrors() const
{
    return _countErrors;
}

const RegistersList& Device::registersList() const
{
    return _registersList;
}
RegistersList& Device::registersList()
{
    return _registersList;
}
const ParametersList& Device::parametersList() const
{
    return _parametersList;
}
ParametersList& Device::parametersList()
{
    return _parametersList;
}

void Device::setPresent(bool isPresent)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isPresent = isPresent;
}
void Device::setWarning(bool isWarning)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isWarning = isWarning;
    if (isWarning) {
        _countWarnings++;
    }
}
void Device::setError(bool isError)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _isError = isError;
    if (isError) {
        _countErrors++;
    }
}

}

