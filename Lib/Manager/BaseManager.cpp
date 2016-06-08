#include "BaseManager.hpp"

namespace RhAL {

BaseManager::BaseManager() :
    CallManager(),
    _devicesByName(),
    _devicesById(),
    _parametersList(),
    _mutexBus(),
    _sortedRegisters(),
    _readCycleCount(0),
    _managerWaitUser1(),
    _managerWaitUser2(),
    _userWaitManager1(),
    _userWaitManager2(),
    _isManagerBarrierOpen1(false),
    _isManagerBarrierOpen2(false),
    _cooperativeThread(0),
    _currentThreadWaiting1(0),
    _currentThreadWaiting2(0),
    _currentThreadCooperativeWaiting1(0),
    _currentThreadCooperativeWaiting2(0),
    _stats(),
    _bus(nullptr),
    _protocol(nullptr),
    _paramBusPort("port", ""),
    _paramBusBaudrate("baudrate", 1000000),
    _paramProtocolName("protocol", "FakeProtocol"),
    _paramEnableSyncRead("enableSyncRead", true),
    _paramEnableSyncWrite("enableSyncWrite", true),
    _paramWaitWriteCheckResponse("waitWriteCheckResponse", true),
    _paramThrowErrorOnScan("throwErrorOnScan", true),
    _paramThrowErrorOnRead("throwErrorOnRead", true)
{
    //Registering all parameters
    _parametersList.add(&this->_paramScheduleMode);
    _parametersList.add(&_paramBusPort);
    _parametersList.add(&_paramBusBaudrate);
    _parametersList.add(&_paramProtocolName);
    _parametersList.add(&_paramEnableSyncRead);
    _parametersList.add(&_paramEnableSyncWrite);
    _parametersList.add(&_paramWaitWriteCheckResponse);
    _parametersList.add(&_paramThrowErrorOnScan);
    _parametersList.add(&_paramThrowErrorOnRead);
    //Initialize the low level communication
    initBus();
}

BaseManager::~BaseManager()
{
    if (_protocol != nullptr) {
        delete _protocol;
        _protocol = nullptr;
    }
    if (_bus != nullptr) {
        delete _bus;
        _bus = nullptr;
    }
}

const Device& BaseManager::dev(id_t id) const
{
    return devById(id);
}
Device& BaseManager::dev(id_t id)
{
    return devById(id);
}
const Device& BaseManager::dev(const std::string& name) const
{
    return devByName(name);
}
Device& BaseManager::dev(const std::string& name)
{
    return devByName(name);
}

const Device& BaseManager::devById(id_t id) const
{
    if (_devicesById.count(id) == 0) {
        throw std::logic_error(
            "BaseManager Device id not found: "
            + std::to_string(id));
    }
    return *(_devicesById.at(id));
}
Device& BaseManager::devById(id_t id)
{
    if (_devicesById.count(id) == 0) {
        throw std::logic_error(
            "BaseManager Device id not found: "
            + std::to_string(id));
    }
    return *(_devicesById.at(id));
}
const Device& BaseManager::devByName(const std::string& name) const
{
    if (_devicesByName.count(name) == 0) {
        throw std::logic_error(
            "BaseManager Device name not found: "
            + name);
    }
    return *(_devicesByName.at(name));
}
Device& BaseManager::devByName(const std::string& name)
{
    if (_devicesByName.count(name) == 0) {
        throw std::logic_error(
            "BaseManager Device name not found: "
            + name);
    }
    return *(_devicesByName.at(name));
}

bool BaseManager::devExists(id_t id) const
{
    return devExistsById(id);
}
bool BaseManager::devExists(const std::string& name) const
{
    return devExistsByName(name);
}

bool BaseManager::devExistsById(id_t id) const
{
    return (_devicesById.count(id) > 0);
}
bool BaseManager::devExistsByName(const std::string& name) const
{
    return (_devicesByName.count(name) > 0);
}

const BaseManager::DevicesByName<Device>& BaseManager::devContainer() const
{
    return _devicesByName;
}

void BaseManager::enableCooperativeThread()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::thread::id id = std::this_thread::get_id();
    if (_cooperativeThread.count(id)) {
        throw std::logic_error(
            "BaseManager enable already active cooperative thread");
    } else {
        _cooperativeThread.insert(id);
    }
}
void BaseManager::disableCooperativeThread()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::thread::id id = std::this_thread::get_id();
    if (!_cooperativeThread.count(id)) {
        throw std::logic_error(
            "BaseManager disable not active cooperative thread");
    } else {
        _cooperativeThread.erase(id);
        //Notify up the Manager which may be
        //waiting at the first barrier for
        //the disabled coorepative thread
        _managerWaitUser1.notify_all();
    }
}

void BaseManager::emergencyStop()
{
    //Check for bus inited
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    _stats.emergencyCount++;
    _protocol->emergencyStop();
}

void BaseManager::exitEmergencyState()
{
    //Check for inited
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    _stats.exitEmergencyCount++;
    _protocol->exitEmergencyState();
}

void BaseManager::waitNextFlush()
{
    //No cooperative thread if
    //scheduling mode is disable
    if (!CallManager::isScheduleMode()) {
        //Do nothing
        return;
    }

    //Lock the shared mutex
    std::unique_lock<std::mutex> lock(CallManager::_mutex);
    //Statistics
    TimePoint pStart = getTimePoint();

    //Retrieve cooperative state
    std::thread::id id = std::this_thread::get_id();
    bool isCooperative = (_cooperativeThread.count(id) != 0);

    //The lock is now acquired
    if (isCooperative) {
        //Statistics
        _stats.waitNextFlushCooperativeCount++;
        //Increment current cooperative 
        //user in waitNextFlush()
        _currentThreadCooperativeWaiting1++;
        //Notify the Manager for a 
        //new thread waiting
        _managerWaitUser1.notify_all();
    } else {
        //Statistics
        _stats.waitNextFlushCount++;
        if (_isManagerBarrierOpen1) {
            //If the thread is not cooperative and if
            //the first barrier is already open, a flush
            //operation has already begun. To be sure that
            //registers are selected, the next Manager cycle
            //is waited.
            //Wait for barrier 2 opens force to wait for
            //the next flush() call.
            _userWaitManager2.wait(lock,
                [this](){return _isManagerBarrierOpen2;});
        } 
        if (_isManagerBarrierOpen1) {
            throw std::logic_error(
                "BaseManager this should never happen waitNextFlush()");
        }

        //Increment current user 
        //in waitNextFlush()
        _currentThreadWaiting1++;
    }

    //Wait for the end of the first flush() barrier.
    //(during wait, the shared mutex is released)
    _userWaitManager1.wait(lock,
        [this](){return _isManagerBarrierOpen1;});

    //The lock is re acquire 
    if (isCooperative) {
        //Decrease the number
        //of waiting cooperative thread.
        _currentThreadCooperativeWaiting1--;
        //Increment the number of cooperative 
        //waiting thread for the second barrier
        _currentThreadCooperativeWaiting2++;
    } else {
        //Increment the number of waiting thread for the 
        //second barrier
        _currentThreadWaiting2++;
    }
    //Notify the Manager for a new thread waiting
    _managerWaitUser2.notify_all();

    //Wait for the end of the second flush() barrier.
    //(during wait, the shared mutex is released)
    _userWaitManager2.wait(lock,
        [this](){return _isManagerBarrierOpen2;});

    //The lock is re acquire
    if (isCooperative) {
        //Decrease the number
        //of waiting thread.
        _currentThreadCooperativeWaiting2--;
    } else {
        //Decrease the number
        //of waiting thread.
        _currentThreadWaiting1--;
        //Decrease the number
        //of waiting thread.
        _currentThreadWaiting2--;
    }

    //Statistics
    TimePoint pStop = getTimePoint();
    _stats.waitManagerDuration +=
        getTimeDuration<TimeDurationMicro>(pStart, pStop);

    //Release the shared mutex
    lock.unlock();
}

void BaseManager::flush(bool isForceSwap)
{
    //No cooperative thread if
    //scheduling mode is disabled
    if (!CallManager::isScheduleMode()) {
        //Do nothing
        return;
    }

    //Check if the Manager is defined as cooperative thread
    if (_cooperativeThread.count(std::this_thread::get_id())) {
        throw std::logic_error(
            "BaseManager Manager thread declared as cooperative");
    }

    //Wait for all cooperative user thread to have
    //started to wait in waitNextFlush();
    //(during wait, the shared mutex is released)
    //(The lock is taken once condition is reached)
    std::unique_lock<std::mutex> lock(CallManager::_mutex);
    //Statistics
    _stats.flushCount++;
    if (_stats.regReadPerFlushMax == 0) {
        _stats.regReadPerFlushMean = _stats.regReadPerFlushAccu;
    } else {
        _stats.regReadPerFlushMean = 
            0.99*_stats.regReadPerFlushMean 
            + 0.01*_stats.regReadPerFlushAccu;
    }
    if (_stats.regReadPerFlushAccu > _stats.regReadPerFlushMax) {
        _stats.regReadPerFlushMax = _stats.regReadPerFlushAccu;
    }
    _stats.regReadPerFlushAccu = 0;
    if (_stats.regWrittenPerFlushMax == 0) {
        _stats.regWrittenPerFlushMean = _stats.regWrittenPerFlushAccu;
    } else {
        _stats.regWrittenPerFlushMean = 
            0.99*_stats.regWrittenPerFlushMean 
            + 0.01*_stats.regWrittenPerFlushAccu;
    }
    if (_stats.regWrittenPerFlushAccu > _stats.regWrittenPerFlushMax) {
        _stats.regWrittenPerFlushMax = _stats.regWrittenPerFlushAccu;
    }
    _stats.regWrittenPerFlushAccu = 0;
    TimePoint pStart = getTimePoint();
    if (_stats.lastFlushTimePoint != TimePoint()) {
        TimeDurationMicro d = getTimeDuration<TimeDurationMicro>(
            _stats.lastFlushTimePoint, pStart);
        if (d > _stats.maxFlushPeriod) {
            _stats.maxFlushPeriod = d;
        }
        _stats.sumFlushPeriod += d;
    }
    _stats.lastFlushTimePoint = pStart;
    //Wait for all user thread to have reach the 
    //first barrier
    //(During wait, the lock is release)
    _managerWaitUser1.wait(lock,
        [this](){
            return (_currentThreadCooperativeWaiting1 
                == _cooperativeThread.size());
        });

    //Close the second barrier
    _isManagerBarrierOpen2 = false;
    //The lock is then re acquire. Open the first barrier.
    _isManagerBarrierOpen1 = true;
    //Notify the users waiting on first barrier
    _userWaitManager1.notify_all();
    //Swap to apply last read change
    swapRead();
    //Call all Devices onSwap() callback
    swapCallBack();
    //Select registers for read and write
    //and compute operation batching
    std::vector<BatchedRegisters> batchsRead =
        computeBatchedRegisters(true);
    std::vector<BatchedRegisters> batchsWrite =
        computeBatchedRegisters(false);

    //Wait for all user thread to have reach the 
    //second barrier
    //(During wait, the lock is release)
    _managerWaitUser2.wait(lock,
        [this](){
            return 
                _currentThreadCooperativeWaiting2 == _cooperativeThread.size() &&
                _currentThreadWaiting2 == _currentThreadWaiting1;
        });

    //The lock is re acquired. Lock the first barrier
    _isManagerBarrierOpen1 = false;
    //Open the second barrier.
    _isManagerBarrierOpen2 = true;
    //Statistics
    TimePoint pStop = getTimePoint();
    _stats.waitUsersDuration +=
        getTimeDuration<TimeDurationMicro>(pStart, pStop);
    //Notify the users thread waiting on the second barrier.
    //Users are exiting from waitNextFlush().
    _userWaitManager2.notify_all();
    //Unlock the shared mutex
    lock.unlock();

    //Perform write operation on all batchs
    bool needsToWait = false;
    for (size_t i=0;i<batchsWrite.size();i++) {
        writeBatch(batchsWrite[i]);
        //Check if a written register is slow
        for(size_t j=0;j<batchsWrite[i].regs.size();j++) {
            for(size_t k=0;k<batchsWrite[i].regs[j].size();k++) {
                if (batchsWrite[i].regs[j][k]->isSlowRegister) {
                    needsToWait = true;
                }
            }
        }
    }
    
    //If a slow register was written, we need
    //to wait a big amount of time
    if (needsToWait) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SlowRegisterDelayMs));
    }

    //Perform read operation on all batchs
    for (size_t i=0;i<batchsRead.size();i++) {
        readBatch(batchsRead[i]);
    }

    //Increment Read counter
    _readCycleCount++;

    //Optionally force immediate swap read
    if (isForceSwap) {
        forceSwap();
    }
}

void BaseManager::forceSwap()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    swapRead();
}

bool BaseManager::ping(const std::string& name)
{
    if (_devicesByName.count(name) == 0) {
        throw std::logic_error(
            "BaseManager no Device with name:" + name);
    } else {
        return ping(_devicesByName.at(name)->id());
    }
}
bool BaseManager::ping(id_t id)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    bool response = _protocol->ping(id);
    if (_devicesById.count(id) == 1) {
        //If the device is register, 
        //set present flag
        _devicesById.at(id)->setPresent(response);
    }
    return response;
}
        
void BaseManager::changeDeviceId(id_t oldId, id_t newId)
{
    if (!ping(oldId)) {
        throw std::logic_error(
            "BaseManager device not responding: " 
            + std::to_string(oldId));
    }
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Write to standard id register at address 3
    uint8_t data = newId;
    if (_paramWaitWriteCheckResponse.value) {
        ResponseState state = _protocol->writeAndCheckData(
            oldId,
            0x03,
            &data,
            1);
        if (checkResponseState(state, nullptr)) {
            std::cerr 
                << "Changing Device id from " << oldId 
                << " to " << newId << std::endl;
            //Sucessfully stop the processus
            exit(0);
        } else {
            _stats.writeErrorCount++;
            std::cerr 
                << "Error: Fail changing Device id from " << oldId 
                << " to " << newId << std::endl;
        }
    } else {
        _protocol->writeData(
            oldId,
            0x03,
            &data,
            1);
        std::cerr 
            << "Changing Device id from " << oldId 
            << " to " << newId << std::endl;
        //Sucessfully stop the processus
        exit(0);
    }
}

void BaseManager::scan()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Mark all Device as not present
    for (auto& it : _devicesByName) {
        it.second->setPresent(false);
    }
    //Iterate over all possible Id
    for (id_t i=IdDevBegin;i<=IdDevEnd;i++) {
        //If the Device exist on the bus
        if (_protocol->ping(i)) {
            //Retrieve the model number
            type_t type;
            bool isSuccess = retrieveTypeNumber(i, type);
            //Skip the Device if model number
            //can not be retrieved
            if (!isSuccess) {
                continue;
            }
            //Check if the Device is already known
            bool isExist = devExistsById(i);
            if (isExist && this->devTypeNumberById(i) != type) {
                //Throw exception if scanned Device id
                //is already known with a different type
                throw std::logic_error(
                    "BaseManager scan type mismatch: "
                    + std::string("id=") + std::to_string(i)
                    + std::string(" type=") + std::to_string(type)
                    + std::string(" is alreay known as ") + devById(i).name()
                    + std::string(" with type ") + devTypeNameById(i));
            } else if (isExist) {
                //If no type problem mark
                //the Device as present
                devById(i).setPresent(true);
            } else {
                //Check if the type number is supported
                //by the manager
                if (!this->isTypeSupported(type)) {
                    //The type is not supported
                    if (_paramThrowErrorOnScan.value) {
                        throw std::runtime_error(
                            "BaseManager type found in scan() not supported: "
                            + std::to_string(type));
                    } else {
                        std::cerr <<
                            "BaseManager type found in scan() not supported: "
                            << "id=" << i << " type="
                            << std::to_string(type) << std::endl;
                        continue;
                    }
                }
                //The Device is not yet present,
                //it is created
                this->devAddByTypeNumber(i, type);
                //Set it as present
                devById(i).setPresent(true);
            }
        }
    }
}

bool BaseManager::checkDevices()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Iterate over all known Devices
    bool isMissing = false;
    for (auto& dev : _devicesById) {
        bool response = _protocol->ping(dev.first);
        if (!response) {
            isMissing = true;
            dev.second->setPresent(false);
        } else {
            dev.second->setPresent(true);
        }
    }

    return !isMissing;
}

void BaseManager::setDevicesConfig()
{
    for (auto& dev : _devicesById) {
        if (dev.second->isPresent()) {
            dev.second->setConfig();
        }
    }
}

void BaseManager::onNewRegister(
    id_t id, const std::string& name)
{
    //Retrieve the next register and
    //add the pointer to the container
    _sortedRegisters.push_back(
        &(devById(id).registersList().reg(name)));
    //Re sort the container by id and then by address
    std::sort(_sortedRegisters.begin(), _sortedRegisters.end(),
        [](const Register* pt1, const Register* pt2) -> bool
        {
            if (pt1->id == pt2->id) {
                return pt1->addr < pt2->addr;
            } else {
                return pt1->id < pt2->id;
            }
        });
}

void BaseManager::forceRegisterRead(
    id_t id, const std::string& name)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    _stats.regReadPerFlushAccu++;
    _stats.forceReadCount++;
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Retrieve Register pointer
    Register* reg = &(devById(id)
        .registersList().reg(name));
    //Reset read flags
    reg->readyForRead();
    //Read single register
    unsigned int nbFails = 0;
    while (true) {
        TimePoint pStart = getTimePoint();
        ResponseState state = _protocol->readData(
            reg->id,
            reg->addr,
            reg->_dataBufferRead,
            reg->length);
        TimePoint pStop = getTimePoint();
        _stats.readCount++;
        _stats.readLength += reg->length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumReadDuration += duration;
        if (_stats.maxReadDuration < duration) {
            _stats.maxReadDuration = duration;
        }
        //Check response
        if (checkResponseState(state, _devicesById.at(id))) {
            //Valid case
            break;
        } else {
            //Error case
            reg->readError();
            nbFails++;
            if (nbFails >= MaxForceReTries) {
                if (_paramThrowErrorOnRead.value) {
                    throw std::runtime_error(
                        "BaseManager max tries reached when read error: "
                        + reg->name 
                        + ", on device id : " + std::to_string(id));
                } else {
                    std::cerr <<
                        "BaseManager max tries reached when read error: "
                        << reg->name 
                        << ", on device id : " << id << std::endl;
                    return;
                }
            }
        }
    }
    //Retrieve the read timestamp
    TimePoint timestamp = getTimePoint();
    //Set swapping flags and set timestamp
    reg->finishRead(timestamp);
    //Do swapping
    reg->swapRead();
}
void BaseManager::forceRegisterWrite(
    id_t id, const std::string& name)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    _stats.regWrittenPerFlushAccu++;
    _stats.forceWriteCount++;
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Retrieve Register pointer
    Register* reg = &(devById(id)
        .registersList().reg(name));
    //Export typed value into data buffer
    reg->selectForWrite();
    //Write the register
    unsigned int nbFails = 0;
    bool isContinue = true;
    while (isContinue) {
        TimePoint pStart = getTimePoint();
        if (_paramWaitWriteCheckResponse.value) {
            //Write and check response state
            ResponseState state = _protocol->writeAndCheckData(
                reg->id,
                reg->addr,
                reg->_dataBufferWrite,
                reg->length);
            //Check for communication error
            if (checkResponseState(state, &(devById(id)))) {
                isContinue = false;
            } else {
                //If error, retries until max limit is reached.
                _stats.writeErrorCount++;
                nbFails++;
                if (nbFails >= MaxForceReTries) {
                    reg->writeError();
                    if (_paramThrowErrorOnRead.value) {
                        throw std::runtime_error(
                            "BaseManager max tries reached when write error: "
                            + reg->name 
                            + ", on device id : " + std::to_string(id));
                    } else {
                        std::cerr <<
                            "BaseManager max tries reached when write error: "
                            << reg->name 
                            << ", on device id : " << id << std::endl;
                        return;
                    }
                }
            }
        } else {
            //Direct no write check case
            _protocol->writeData(
                reg->id,
                reg->addr,
                reg->_dataBufferWrite,
                reg->length);
            isContinue = false;
        }
        TimePoint pStop = getTimePoint();
        _stats.writeCount++;
        _stats.writeLength += reg->length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumWriteDuration += duration;
        if (_stats.maxWriteDuration < duration) {
            _stats.maxWriteDuration = duration;
        }
    }
    //Wait delay in case of slow register
    if (reg->isSlowRegister) {
        std::this_thread::sleep_for(
            std::chrono::milliseconds(SlowRegisterDelayMs));
    }
}

Statistics BaseManager::getStatistics() const
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    return _stats;
}

void BaseManager::resetStatistics()
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _stats.reset();
}

const ParametersList& BaseManager::parametersList() const
{
    return _parametersList;
}
ParametersList& BaseManager::parametersList()
{
    return _parametersList;
}

const ParametersList& BaseManager::protocolParametersList() const
{
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    return _protocol->parametersList();
}
ParametersList& BaseManager::protocolParametersList()
{
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    return _protocol->parametersList();
}

void BaseManager::setProtocolConfig(
    const std::string& port,
    unsigned long baudrate,
    const std::string& protocol)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramBusPort.value = port;
    _paramBusBaudrate.value = baudrate;
    _paramProtocolName.value = protocol;
    //Reset low level communication (bus/protocol)
    initBus();
}

void BaseManager::setEnableSyncRead(bool isEnable)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramEnableSyncRead.value = isEnable;
}
void BaseManager::setEnableSyncWrite(bool isEnable)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramEnableSyncWrite.value = isEnable;
}
void BaseManager::setWaitWriteCheckResponse(bool isEnable)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramWaitWriteCheckResponse.value = isEnable;
}
void BaseManager::setThrowOnScan(bool isEnable)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramThrowErrorOnScan.value = isEnable;
}
void BaseManager::setThrowOnRead(bool isEnable)
{
    std::lock_guard<std::mutex> lock(CallManager::_mutex);
    _paramThrowErrorOnRead.value = isEnable;
}

void BaseManager::initBus()
{
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Free existing instance
    if (_protocol != nullptr) {
        delete _protocol;
        _protocol = nullptr;
    }
    if (_bus != nullptr) {
        delete _bus;
        _bus = nullptr;
    }
    //Allocate Bus and Protocol
    if (_paramBusPort.value != "") {
        try {
        _bus = new SerialBus(
            _paramBusPort.value, _paramBusBaudrate.value);
        } catch (const std::exception& e) {
            throw std::runtime_error(
                "SerialBus initialization failed:" 
                + std::string(" port:") 
                + std::string(_paramBusPort.value)
                + std::string(" exception: ")
                + std::string(e.what()));
        }
    }
    _protocol = ProtocolFactory(_paramProtocolName.value, *_bus);
    //Check that Protocol implementation name is valid
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager invalid protocol name: "
            + _paramProtocolName.value);
    }
}

bool BaseManager::isNeedRead(Register* reg)
{
    return
        reg->needRead() ||
        (reg->periodPackedRead > 0 &&
        (_readCycleCount % reg->periodPackedRead == 0));
}
bool BaseManager::isNeedWrite(Register* reg)
{
    bool isNeed = reg->needWrite();
    //If selected for write, register
    //is reset for write aggregation
    if (isNeed) {
        reg->selectForWrite();
    }
    return isNeed;
}

std::vector<BaseManager::BatchedRegisters> BaseManager::computeBatchedRegisters(
    bool isReadOrWrite)
{
    //Batched registers container
    std::vector<BatchedRegisters> container;

    //Merge the given temporary batch to
    //final batches by merging by id
    auto mergeById = [&container, isReadOrWrite, this]
    (const BatchedRegisters& tmpBatch) {
        //SyncRead/Write is enable whenether
        //configuration boolean are set
        bool isSyncEnable =
            (isReadOrWrite && this->_paramEnableSyncRead.value) ||
            (!isReadOrWrite && this->_paramEnableSyncWrite.value);
        bool found = false;
        //Already added final batch are iterated
        //to find if the current batch can be merge
        //with another batch by id with constant address
        //and length.
        for (size_t j=0;j<container.size();j++) {
            if (
                isSyncEnable &&
                container[j].addr == tmpBatch.addr &&
                container[j].length == tmpBatch.length
            ) {
                container[j].regs.push_back(std::vector<Register*>());
                container[j].ids.push_back(tmpBatch.ids.front());
                for (size_t k=0;k<tmpBatch.regs.front().size();k++) {
                    container[j].regs.back().push_back(tmpBatch.regs.front()[k]);
                }
                found = true;
                break;
            }
        }
        //If no compatible final batch are
        //found a new one is created
        if (!found) {
            container.push_back(tmpBatch);
        }
    };

    //Iterate over all sorted Registers
    //by id and then by address
    BatchedRegisters tmpBatch;
    for (size_t i=0;i<_sortedRegisters.size();i++) {
        Register* reg = _sortedRegisters[i];
        //Select batched registers according to
        //the given predicate function
        if (
            (isReadOrWrite && isNeedRead(reg)) ||
            (!isReadOrWrite && isNeedWrite(reg))
        ) {
            //Initialize the temporary batch if empty
            if (tmpBatch.regs.size() == 0) {
                tmpBatch.addr = reg->addr;
                tmpBatch.length = reg->length;
                tmpBatch.regs = {{reg}};
                tmpBatch.ids = {reg->id};
                //And continue to next register
                continue;
            }
            bool isContigious =
                (tmpBatch.addr + tmpBatch.length == reg->addr) &&
                (reg->id == tmpBatch.ids.front());
            if (isContigious) {
                //If the register is contigious to current
                //batch, it is added to it.
                //Registers are first batched by address
                //with id constant.
                tmpBatch.length += reg->length;
                tmpBatch.regs.front().push_back(reg);
            } else {
                //If the register is not contiguous,
                //the temporaty batch is added to
                //the final container.
                mergeById(tmpBatch);
                //Reset the temporary batch
                tmpBatch.regs.clear();
                tmpBatch.ids.clear();
                //And a new temporary batch is initialize
                tmpBatch.addr = reg->addr;
                tmpBatch.length = reg->length;
                tmpBatch.regs = {{reg}};
                tmpBatch.ids = {reg->id};
            }
        }
    }
    //Merge the last batch if not empty
    if (tmpBatch.regs.size() > 0) {
        mergeById(tmpBatch);
    }

    return container;
}

void BaseManager::writeBatch(BatchedRegisters& batch)
{
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Registers stats
    for (size_t i=0;i<batch.regs.size();i++) {
        _stats.regWrittenPerFlushAccu += batch.regs[i].size();
    }
    if (batch.ids.size() == 1) {
        //Write single register
        TimePoint pStart = getTimePoint();
        if (_paramWaitWriteCheckResponse.value) {
            //Write and check response state
            ResponseState state = _protocol->writeAndCheckData(
                batch.ids.front(),
                batch.addr,
                batch.regs.front().front()->_dataBufferWrite,
                batch.length);
            //Check for communication error
            if (!checkResponseState(state, _devicesById.at(batch.ids.front()))) {
                //Error case
                _stats.writeErrorCount++;
                //Re-write the value at next cycle
                for (size_t j=0;j<batch.regs.front().size();j++) {
                    batch.regs.front()[j]->writeError();
                }
            } 
        } else {
            //Direct write no check case
            _protocol->writeData(
                batch.ids.front(),
                batch.addr,
                batch.regs.front().front()->_dataBufferWrite,
                batch.length);
        }
        TimePoint pStop = getTimePoint();
        _stats.writeCount++;
        _stats.writeLength += batch.length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumWriteDuration += duration;
        if (_stats.maxWriteDuration < duration) {
            _stats.maxWriteDuration = duration;
        }
    } else {
        //Synch Write multiple registers
        std::vector<const data_t*> datas;
        for (size_t i=0;i<batch.regs.size();i++) {
            datas.push_back(batch.regs[i].front()->_dataBufferWrite);
        }
        TimePoint pStart = getTimePoint();
        if (_paramWaitWriteCheckResponse.value) {
            //Write and check response state
            std::vector<ResponseState> states = _protocol->syncWriteAndCheck(
                batch.ids,
                batch.addr,
                datas,
                batch.length);
            for (size_t i=0;i<states.size();i++) {
                //Check for communication error
                if (!checkResponseState(states[i], _devicesById.at(batch.ids[i]))) {
                    //Error case
                    _stats.writeErrorCount++;
                    //Re-write the value at next cycle
                    for (size_t j=0;j<batch.regs[i].size();j++) {
                        batch.regs[i][j]->writeError();
                    }
                }
            }
        } else {
            //Direct write no check case
            _protocol->syncWrite(
                batch.ids,
                batch.addr,
                datas,
                batch.length);
        }
        TimePoint pStop = getTimePoint();
        _stats.syncWriteCount++;
        _stats.syncWriteLength += batch.length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumSyncWriteDuration += duration;
        if (_stats.maxSyncWriteDuration < duration) {
            _stats.maxSyncWriteDuration = duration;
        }
    }
}
void BaseManager::readBatch(BatchedRegisters& batch)
{
    std::lock_guard<std::mutex> lockBus(_mutexBus);
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Reset read flags for all registers
    for (size_t i=0;i<batch.regs.size();i++) {
        for (size_t j=0;j<batch.regs[i].size();j++) {
            batch.regs[i][j]->readyForRead();
        }
    }
    //Registers stats
    for (size_t i=0;i<batch.regs.size();i++) {
        _stats.regReadPerFlushAccu += batch.regs[i].size();
    }
    if (batch.ids.size() == 1) {
        //Read single register
        TimePoint pStart = getTimePoint();
        ResponseState state = _protocol->readData(
            batch.ids.front(),
            batch.addr,
            batch.regs.front().front()->_dataBufferRead,
            batch.length);
        TimePoint pStop = getTimePoint();
        _stats.readCount++;
        _stats.readLength += batch.length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumReadDuration += duration;
        if (_stats.maxReadDuration < duration) {
            _stats.maxReadDuration = duration;
        }
        //Check for communication error
        if (!checkResponseState(state, _devicesById.at(batch.ids.front()))) {
            //Error case
            //Re-ask the value at next cycle
            for (size_t j=0;j<batch.regs.front().size();j++) {
                batch.regs.front()[j]->readError();
            }
        } else {
            //Valid case
            //Retrieve the read timestamp
            TimePoint timestamp = getTimePoint();
            //Assign timestamp on Manager side and 
            //mark for swapping
            for (size_t j=0;j<batch.regs.front().size();j++) {
                batch.regs.front()[j]->finishRead(timestamp);
            }
        }
    } else {
        //Synch Read multiple registers
        std::vector<data_t*> datas;
        for (size_t i=0;i<batch.regs.size();i++) {
            datas.push_back(batch.regs[i].front()->_dataBufferRead);
        }
        TimePoint pStart = getTimePoint();
        std::vector<ResponseState> states = _protocol->syncRead(
            batch.ids,
            batch.addr,
            datas,
            batch.length);
        TimePoint pStop = getTimePoint();
        _stats.syncReadCount++;
        _stats.syncReadLength += batch.length;
        TimeDurationMicro duration = 
            getTimeDuration<TimeDurationMicro>(pStart, pStop);
        _stats.sumSyncReadDuration += duration;
        if (_stats.maxSyncReadDuration < duration) {
            _stats.maxSyncReadDuration = duration;
        }
        //Retrieve the read timestamp
        TimePoint timestamp = getTimePoint();
        for (size_t i=0;i<states.size();i++) {
            //Check for communication error
            if (!checkResponseState(states[i], _devicesById.at(batch.ids[i]))) {
                //Error case
                //Re-ask the value at next cycle
                for (size_t j=0;j<batch.regs[i].size();j++) {
                    batch.regs[i][j]->readError();
                }
            } else {
                //Valid case
                //Assign timestamp on Manager side and 
                //mark for swapping
                for (size_t j=0;j<batch.regs[i].size();j++) {
                    batch.regs[i][j]->finishRead(timestamp);
                }
            }
        }
    }
}

void BaseManager::swapRead()
{
    for (size_t i=0;i<_sortedRegisters.size();i++) {
        _sortedRegisters[i]->swapRead();
    }
}

void BaseManager::swapCallBack()
{
    for (auto& it : _devicesById) {
        it.second->onSwap();
    }
}

bool BaseManager::checkResponseState(
    ResponseState state, Device* dev)
{
    //Check if the device has answered
    bool isPresent = true;
    if (state & ResponseQuiet) {
        _stats.deviceQuietCount++;
        isPresent = false;
    }
    //Check for warning flags
    bool isWarning = false;
    if (state & ResponseOverload) {
        isWarning = true;
    }
    if (state & ResponseOverheat) {
        isWarning = true;
    }
    if (state & ResponseBadVoltage) {
        isWarning = true;
    }
    if (state & ResponseAlert) {
        isWarning = true;
    }
    //Check for error flags
    bool isError = false;
    if (state & ResponseBadChecksum) {
        isError = true;
    }
    if (state & ResponseDeviceBadInstruction) {
        isError = true;
    }
    if (state & ResponseDeviceBadChecksum) {
        isError = true;
    }
    if (state & ResponseBadSize) {
        isError = true;
    }
    if (state & ResponseBadProtocol) {
        isError = true;
    }
    if (state & ResponseBadId) {
        isError = true;
    }
    //Update statistics
    if (isWarning) {
        _stats.deviceWarningCount++;
    }
    if (isError) {
        _stats.deviceErrorCount++;
    }
    //Update Device state
    if (dev != nullptr) {
        dev->setPresent(isPresent);
        dev->setWarning(isWarning);
        dev->setError(isError);
        if (isWarning || isError) {
            dev->setFlags(state);
        }
    }

    if (state & ResponseOK) {
        _stats.deviceOKCount++;
        return true;
    } else {
        return false;
    }
}

bool BaseManager::retrieveTypeNumber(id_t id, type_t& type)
{
    //Check for initBus() called
    if (_protocol == nullptr) {
        throw std::logic_error(
            "BaseManager protocol not initialized");
    }
    //Read at static memory address
    data_t* pt = reinterpret_cast<data_t*>(&type);
    ResponseState state = _protocol
        ->readData(id, AddrDevTypeNumber, pt, 2);

    //Check response
    return checkResponseState(state, nullptr);
}

}

