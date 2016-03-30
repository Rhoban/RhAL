#include <functional>
#include <sstream>
#include "Bindings/RhIOBinding.hpp"
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"

namespace RhAL {

RhIOBinding::RhIOBinding(
    BaseManager& manager, 
    const std::string& nodeName,
    bool isUpdateThread) : 
    _thread(nullptr),
    _isOver(true),
    _manager(&manager),
    _node(nullptr)
{
    //Create RhIO Node and retrieve it
    RhIO::Root.newChild(nodeName);
    _node = &(RhIO::Root.child(nodeName));

    //Create RhIO command
    _node->newCommand("rhalScan", "Force the bus scan", 
        std::bind(&RhIOBinding::cmdScan, this, std::placeholders::_1));
    _node->newCommand("rhalStats", "Display RhAL statistics", 
        std::bind(&RhIOBinding::cmdStats, this, std::placeholders::_1));
    _node->newCommand("rhalForceDevRead", "Re-read all registers of given device name", 
        std::bind(&RhIOBinding::cmdForceDevRead, this, std::placeholders::_1));
    _node->newCommand("rhalPing", "Ping given device name or id", 
        std::bind(&RhIOBinding::cmdPing, this, std::placeholders::_1));
    _node->newCommand("rhalStatus", "Display RhAL devices status", 
        std::bind(&RhIOBinding::cmdStatus, this, std::placeholders::_1));
    _node->newCommand("rhalCheck", "Check all known devices", 
        std::bind(&RhIOBinding::cmdCheck, this, std::placeholders::_1));
    _node->newCommand("rhalSaveConf", "Export RhAL conf to given path", 
        std::bind(&RhIOBinding::cmdSaveConf, this, std::placeholders::_1));
    _node->newCommand("rhalLoadConf", "Import RhAL conf from given path", 
        std::bind(&RhIOBinding::cmdLoadConf, this, std::placeholders::_1));
    _node->newCommand("rhalEM", "Send emergencu stop", 
        std::bind(&RhIOBinding::cmdEmergency, this, std::placeholders::_1));
    _node->newCommand("rhalExitEmergency", "Exit emergency state", 
        std::bind(&RhIOBinding::cmdEmergencyExit, this, std::placeholders::_1));

    //First RhIO/RhAL synchronisation
    update();

    //Start Binding update thread
    if (isUpdateThread) {
        _isOver = false;
        _thread = new std::thread([this](){
            while (!this->_isOver) {
                this->update();
                std::this_thread::sleep_for(
                    std::chrono::milliseconds(500));
            }
        });
    }
}

RhIOBinding::~RhIOBinding()
{
    //Wait for update thread and quit
    if (_thread != nullptr) {
        _isOver = true;
        _thread->join();
        delete _thread;
    }
}

void RhIOBinding::update()
{
    //Retrive all contained Devices indexed by name
    auto allDevices = _manager->devContainer();

    //Iterate over all Devices
    for (const auto& device : allDevices) {
        //Create a RhIO node if it not exists associated
        //with each Device
        if (!_node->childExist(device.first)) {
            _node->newChild(device.first);
        }
        RhIO::IONode* deviceNode = &(_node->child(device.first));
        //Iterate over all Registers Bool
        for (const auto& reg : device.second->registersList().containerBool()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (deviceNode->getValueType(reg.first) == RhIO::NoValue) {
                deviceNode->newBool(reg.first);
                deviceNode->setBool(reg.first, reg.second->readValue().value);
                deviceNode->setCallbackBool(reg.first, [reg](bool newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [deviceNode, reg](bool newValue) {
                    deviceNode->setBool(reg.first, newValue, true);
                };
                reg.second->setCallbackRead(callback);
                reg.second->setCallbackWrite(callback);
            }
        }
        //Iterate over all Registers Int
        for (const auto& reg : device.second->registersList().containerInt()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (deviceNode->getValueType(reg.first) == RhIO::NoValue) {
                deviceNode->newInt(reg.first);
                deviceNode->setInt(reg.first, reg.second->readValue().value);
                deviceNode->setCallbackInt(reg.first, [reg](long newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [deviceNode, reg](long newValue) {
                    deviceNode->setInt(reg.first, newValue, true);
                };
                reg.second->setCallbackRead(callback);
                reg.second->setCallbackWrite(callback);
            }
        }
        //Iterate over all Registers Float
        for (const auto& reg : device.second->registersList().containerFloat()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (deviceNode->getValueType(reg.first) == RhIO::NoValue) {
                deviceNode->newFloat(reg.first);
                deviceNode->setFloat(reg.first, reg.second->readValue().value);
                deviceNode->setCallbackFloat(reg.first, [reg](double newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [deviceNode, reg](double newValue) {
                    deviceNode->setFloat(reg.first, newValue, true);
                };
                reg.second->setCallbackRead(callback);
                reg.second->setCallbackWrite(callback);
            }
        }
        //Create parameters RhIO node if not exists
        if (!deviceNode->childExist("parameters")) {
            deviceNode->newChild("parameters");
        }
        RhIO::IONode* parametersNode = &(deviceNode->child("parameters"));
        //Update all parameters
        updateParameters(device.second->parametersList(), parametersNode);
    }

    //Create a RhIO node for Manager parameters
    if (!_node->childExist("Manager")) {
        _node->newChild("Manager");
    }
    RhIO::IONode* parametersManagerNode = &(_node->child("Manager"));
    //Update all parameters
    updateParameters(_manager->parametersList(), parametersManagerNode);
    
    //Create a RhIO node for Protocol parameters
    if (!_node->childExist("Protocol")) {
        _node->newChild("Protocol");
    }
    RhIO::IONode* parametersProtocolNode = &(_node->child("Protocol"));
    //Update all parameters
    updateParameters(_manager->protocolParametersList(), parametersProtocolNode);
}
        
void RhIOBinding::updateParameters(
    ParametersList& params,
    RhIO::IONode* node)
{
    //Iterate over all parameters of type Bool and exportation to RhIO
    for (const auto& param : params.containerBool()) {
        if (node->getValueType(param.first) == RhIO::NoValue) {
            node->newBool(param.first);
            node->setBool(param.first, param.second->value);
            node->setCallbackBool(param.first, [param](bool newValue) {
                param.second->value = newValue;
            });
        } else {
            node->setBool(param.first, param.second->value);
        }
    }
    //Iterate over all parameters of type Number and exportation to RhIO
    for (const auto& param : params.containerNumber()) {
        if (node->getValueType(param.first) == RhIO::NoValue) {
            node->newFloat(param.first);
            node->setFloat(param.first, param.second->value);
            node->setCallbackFloat(param.first, [param](double newValue) {
                param.second->value = newValue;
            });
        } else {
            node->setFloat(param.first, param.second->value);
        }
    }
    //Iterate over all parameters of type Str and exportation to RhIO
    for (const auto& param : params.containerStr()) {
        if (node->getValueType(param.first) == RhIO::NoValue) {
            node->newStr(param.first);
            node->setStr(param.first, param.second->value);
            node->setCallbackStr(param.first, [param](std::string newValue) {
                param.second->value = newValue;
            });
        } else {
            node->setStr(param.first, param.second->value);
        }
    }
}

std::string RhIOBinding::cmdScan(
    std::vector<std::string> argv)
{
    (void)argv;
    _manager->scan();
    update();

    return "Running a scan";
}

std::string RhIOBinding::cmdStats(
    std::vector<std::string> argv)
{
    (void)argv;
    std::ostringstream oss;
    _manager->getStatistics().print(oss);

    return oss.str();
}
            
std::string RhIOBinding::cmdForceDevRead(
    std::vector<std::string> argv)
{
    if (argv.size() != 1) {
        return "Invalid Argument. Usage:\nforceDevRead [dev_name]";
    }
    if (!_manager->devExists(argv[0])) {
        return "Unknown device name: " + argv[0];
    }
    auto &device = _manager->devByName(argv[0]);
    for (auto& reg : device.registersList().container()) {
        reg.second->forceRead();
    }
    update();

    return "Read all registers from device: " + argv[0];
}

std::string RhIOBinding::cmdPing(
    std::vector<std::string> argv)
{
    if (argv.size() != 1) {
        return "Invalid Argument. Usage:\nping [dev_name|dev_id]";
    }

    bool isPresent = false;
    if (_manager->devExists(argv[0])) {
        isPresent = _manager->ping(argv[0]);
    } else {
        try {
            isPresent = _manager->ping(std::stoi(argv[0]));
        } catch (const std::exception& e) {
            return "Unknown or invalid device: " + argv[0];
        }
    }

    if (isPresent) {
        return "Device is present: " + argv[0];
    } else {
        return "Device not responding: " + argv[0];
    }
}

std::string RhIOBinding::cmdStatus(
    std::vector<std::string> argv)
{
    (void)argv;
    auto allDevices = _manager->devContainer();

    std::string msg;
    for (const auto& dev : allDevices) {
        msg += "id=" + std::to_string(dev.second->id());
        msg += " name=" + dev.first;
        msg += " type=" + _manager->devTypeNameById(dev.second->id());
        if (dev.second->isPresent()) {
            msg += " isPresent";
        } else {
            msg += " missing";
        }
        if (dev.second->isWarning()) {
            msg += " isWarning";
        }
        if (dev.second->isError()) {
            msg += " isError";
        }
        msg += "\n";
    }

    return msg;
}

std::string RhIOBinding::cmdCheck(
    std::vector<std::string> argv)
{
    (void)argv;
    bool isOk = _manager->checkDevices();
    if (isOk) {
        return "All devices are present";
    } else {
        std::string msg;
        auto allDevices = _manager->devContainer();
        for (const auto& dev : allDevices) {
            if (!dev.second->isPresent()) {
                msg += "Missing " + dev.first + "\n";
            }
        }
        return msg;
    }
}

std::string RhIOBinding::cmdSaveConf(
    std::vector<std::string> argv)
{
    if (argv.size() != 1) {
        return "Invalid Argument. Usage:\nsaveConf [filepath]";
    }
    _manager->writeConfig(argv[0]);

    return "RhAL configuration exported to: " + argv[0];
}

std::string RhIOBinding::cmdLoadConf(
    std::vector<std::string> argv)
{
    if (argv.size() != 1) {
        return "Invalid Argument. Usage:\nloadConf [filepath]";
    }
    _manager->readConfig(argv[0]);
    update();

    return "RhAL configuration imported from: " + argv[0];
}
        
std::string RhIOBinding::cmdEmergency(
    std::vector<std::string> argv)
{
    (void)argv;
    _manager->emergencyStop();
    return "Broadcast Emergency Stop";
}

std::string RhIOBinding::cmdEmergencyExit(
    std::vector<std::string> argv)
{
    (void)argv;
    _manager->exitEmergencyState();
    return "Exit Emergency State";
}

}

