#include <functional>
#include <iomanip>
#include <sstream>
#include <rhoban_utils/stats/stats.h>
#include "Bindings/RhIOBinding.hpp"
#include "Manager/BaseManager.hpp"
#include "Manager/Device.hpp"
#include "Devices/DXL.hpp"
#include "Devices/GY85.hpp"
#include "Devices/PressureSensor.hpp"

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
    _node->newCommand("rhalResetStats", "Reset all RhAL statistics", 
        std::bind(&RhIOBinding::cmdResetStats, this, std::placeholders::_1));
    _node->newCommand("rhalReadDev", "Re-read all registers of given device name", 
        std::bind(&RhIOBinding::cmdReadDev, this, std::placeholders::_1));
    _node->newCommand("rhalReadReg", "Re-read in the given device name the given register name", 
        std::bind(&RhIOBinding::cmdReadReg, this, std::placeholders::_1));
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
    _node->newCommand("rhalInit", "Enable all servos in zero position", 
        std::bind(&RhIOBinding::cmdInit, this, std::placeholders::_1));
    _node->newCommand("rhalChangeId", "Chaneg the Device id of given id to given id", 
        std::bind(&RhIOBinding::cmdChangeId, this, std::placeholders::_1));
    _node->newCommand("rhalTare", "Tare all pressure devices", 
        std::bind(&RhIOBinding::cmdTare, this, std::placeholders::_1));
    _node->newCommand("rhalGyroTare", "Tare all gyro devices", 
        std::bind(&RhIOBinding::cmdGyroTare, this, std::placeholders::_1));

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
    const auto& allDevices = _manager->devContainer();

    //Iterate over all Devices
    for (const auto& device : allDevices) {
        //Create a RhIO node if it not exists associated
        //with each Device
        if (!_node->childExist(device.first)) {
            _node->newChild(device.first);
        }
        RhIO::IONode* deviceNode = &(_node->child(device.first));

        // In the case of the GY85, move registers to the registers/ sub node
        RhIO::IONode *registersNode = deviceNode;
        if (dynamic_cast<RhAL::GY85*>(device.second) != nullptr) {
            if (!registersNode->childExist("registers")) {
                registersNode->newChild("registers");
            }
            registersNode = &registersNode->child("registers");
        }

        //Iterate over all Registers Bool
        for (const auto& reg : device.second->registersList().containerBool()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (registersNode->getValueType(reg.first) == RhIO::NoValue) {
                registersNode->newBool(reg.first);
                registersNode->setBool(reg.first, reg.second->readValue().value);
                registersNode->setCallbackBool(reg.first, [reg](bool newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [registersNode, reg](bool newValue) {
                    registersNode->setBool(reg.first, newValue, true);
                };
                reg.second->setCallbackRead(callback);
                reg.second->setCallbackWrite(callback);
            }
        }
        //Iterate over all Registers Int
        for (const auto& reg : device.second->registersList().containerInt()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (registersNode->getValueType(reg.first) == RhIO::NoValue) {
                registersNode->newInt(reg.first);
                registersNode->setInt(reg.first, reg.second->readValue().value);
                registersNode->setCallbackInt(reg.first, [reg](long newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [registersNode, reg](long newValue) {
                    registersNode->setInt(reg.first, newValue, true);
                };
                reg.second->setCallbackRead(callback);
                reg.second->setCallbackWrite(callback);
            }
        }
        //Iterate over all Registers Float
        for (const auto& reg : device.second->registersList().containerFloat()) {
            //Create a RhIO Node if it not exists associated
            //with the register
            if (registersNode->getValueType(reg.first) == RhIO::NoValue) {
                registersNode->newFloat(reg.first);
                registersNode->setFloat(reg.first, reg.second->readValue().value);
                registersNode->setCallbackFloat(reg.first, [reg](double newValue) {
                    reg.second->writeValue(newValue, true);
                });
                auto callback = [registersNode, reg](double newValue) {
                    registersNode->setFloat(reg.first, newValue, true);
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
        //Specific updates
        specificUpdate(deviceNode, device.second);
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

void RhIOBinding::specificUpdate(RhIO::IONode *deviceNode, RhAL::Device *device)
{
    //If the device is a GY85
    if (RhAL::GY85* gy85 = dynamic_cast<RhAL::GY85*>(device)) {
        if (deviceNode->getValueType("accX") == RhIO::NoValue) {
            deviceNode->newFloat("accX");
            deviceNode->newFloat("accY");
            deviceNode->newFloat("accZ");
            deviceNode->newFloat("gyroX");
            deviceNode->newFloat("gyroY");
            deviceNode->newFloat("gyroZ");
            deviceNode->newFloat("magnX");
            deviceNode->newFloat("magnY");
            deviceNode->newFloat("magnZ");
            deviceNode->newFloat("accXRaw");
            deviceNode->newFloat("accYRaw");
            deviceNode->newFloat("accZRaw");
            deviceNode->newFloat("gyroXRaw");
            deviceNode->newFloat("gyroYRaw");
            deviceNode->newFloat("gyroZRaw");
            deviceNode->newFloat("magnXRaw");
            deviceNode->newFloat("magnYRaw");
            deviceNode->newFloat("magnZRaw");
            deviceNode->newFloat("yaw");
            deviceNode->newFloat("pitch");
            deviceNode->newFloat("roll");
            deviceNode->newFloat("gyroYaw");
            deviceNode->newFloat("magnHeading");
            deviceNode->newFloat("magnAzimuth");
            deviceNode->newFloat("matrix_0_0");
            deviceNode->newFloat("matrix_0_1");
            deviceNode->newFloat("matrix_0_2");
            deviceNode->newFloat("matrix_1_0");
            deviceNode->newFloat("matrix_1_1");
            deviceNode->newFloat("matrix_1_2");
            deviceNode->newFloat("matrix_2_0");
            deviceNode->newFloat("matrix_2_1");
            deviceNode->newFloat("matrix_2_2");

            std::function<void()> update = [deviceNode, gy85] {
                deviceNode->setFloat("accX", gy85->getAccX());
                deviceNode->setFloat("accY", gy85->getAccY());
                deviceNode->setFloat("accZ", gy85->getAccZ());
                deviceNode->setFloat("gyroX", gy85->getGyroX());
                deviceNode->setFloat("gyroY", gy85->getGyroY());
                deviceNode->setFloat("gyroZ", gy85->getGyroZ());
                deviceNode->setFloat("magnX", gy85->getMagnX());
                deviceNode->setFloat("magnY", gy85->getMagnY());
                deviceNode->setFloat("magnZ", gy85->getMagnZ());
                deviceNode->setFloat("accXRaw", gy85->getAccXRaw());
                deviceNode->setFloat("accYRaw", gy85->getAccYRaw());
                deviceNode->setFloat("accZRaw", gy85->getAccZRaw());
                deviceNode->setFloat("gyroXRaw", gy85->getGyroXRaw());
                deviceNode->setFloat("gyroYRaw", gy85->getGyroYRaw());
                deviceNode->setFloat("gyroZRaw", gy85->getGyroZRaw());
                deviceNode->setFloat("magnXRaw", gy85->getMagnXRaw());
                deviceNode->setFloat("magnYRaw", gy85->getMagnYRaw());
                deviceNode->setFloat("magnZRaw", gy85->getMagnZRaw());
                deviceNode->setFloat("magnHeading", 180*gy85->getMagnHeading()/M_PI);
                deviceNode->setFloat("magnAzimuth", 180*gy85->getMagnAzimuth()/M_PI);
                deviceNode->setFloat("gyroYaw", 180*gy85->getGyroYaw()/M_PI);
                deviceNode->setFloat("yaw", 180*gy85->getYaw()/M_PI);
                deviceNode->setFloat("pitch", 180*gy85->getPitch()/M_PI);
                deviceNode->setFloat("roll", 180*gy85->getRoll()/M_PI);
                deviceNode->setFloat("matrix_0_0", gy85->getMatrix()(0,0));
                deviceNode->setFloat("matrix_0_1", gy85->getMatrix()(0,1));
                deviceNode->setFloat("matrix_0_2", gy85->getMatrix()(0,2));
                deviceNode->setFloat("matrix_1_0", gy85->getMatrix()(1,0));
                deviceNode->setFloat("matrix_1_1", gy85->getMatrix()(1,1));
                deviceNode->setFloat("matrix_1_2", gy85->getMatrix()(1,2));
                deviceNode->setFloat("matrix_2_0", gy85->getMatrix()(2,0));
                deviceNode->setFloat("matrix_2_1", gy85->getMatrix()(2,1));
                deviceNode->setFloat("matrix_2_2", gy85->getMatrix()(2,2));
            };
            update();
            gy85->setCallback(update);
        }
    }
    // Pressure sensors
    if (RhAL::PressureSensorBase* ps = dynamic_cast<RhAL::PressureSensorBase*>(device)) {
        if (deviceNode->getValueType("x") == RhIO::NoValue) {
            deviceNode->newFloat("x");
            deviceNode->newFloat("y");
            deviceNode->newFloat("weight");

            std::function<void()> update = [deviceNode, ps] {
                deviceNode->setFloat("x", ps->getX());
                deviceNode->setFloat("y", ps->getY());
                deviceNode->setFloat("weight", ps->getWeight());
            };
            update();
            ps->setCallback(update);
        }
    }
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

std::string RhIOBinding::cmdResetStats(
    std::vector<std::string> argv)
{
    (void)argv;
    std::ostringstream oss;
    _manager->resetStatistics();

    return "RhAL Statistics Reseted";
}
            
std::string RhIOBinding::cmdReadDev(
    std::vector<std::string> argv)
{
    if (argv.size() != 1) {
        return "Invalid Argument. Usage:\nforceDevRead [dev_name]";
    }
    if (!_manager->devExists(argv[0])) {
        return "Unknown device name: " + argv[0];
    }
    Device& device = _manager->devByName(argv[0]);
    for (const auto& reg : device.registersList().container()) {
        reg.second->forceRead();
    }
    update();

    return "Read all registers from device: " + argv[0];
}

std::string RhIOBinding::cmdReadReg(
    std::vector<std::string> argv)
{
    if (argv.size() != 2) {
        return "Invalid Argument. Usage:\nforceDevRead [dev_name] [reg_name]";
    }
    if (!_manager->devExists(argv[0])) {
        return "Unknown device name: " + argv[0];
    }
    if (!_manager->devByName(argv[0]).registersList().exists(argv[1])) {
        return "Unknown parameter name: " + argv[0];
    }
    _manager->devByName(argv[0]).registersList().reg(argv[1]).forceRead();
    update();

    return "Read one register from device: " + argv[0] + ":" + argv[1];
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
    const auto& allDevices = _manager->devContainer();

    std::ostringstream os;
    os << std::setfill(' ') << std::setw(4) << "id";
    os << std::setfill(' ') << std::setw(30) << "name";
    os << std::setfill(' ') << std::setw(20) << "type";
    os << std::setfill(' ') << std::setw(15) << "status";
    os << std::setfill(' ') << std::setw(10) << "warnings";
    os << std::setfill(' ') << std::setw(10) << "errors";
    os << std::setfill(' ') << std::setw(10) << "missings";
    os << std::setfill(' ') << std::setw(20) << "last flags";
    os << std::endl;
    os << std::setfill('-') << std::setw(4+30+20+15+10+10+10+20) << "-" << std::endl;
    for (const auto& dev : allDevices) {
        os << std::setfill(' ') << std::setw(4) << std::to_string(dev.second->id());
        os << std::setfill(' ') << std::setw(30) << dev.first;
        os << std::setfill(' ') << std::setw(20) << _manager->devTypeNameById(dev.second->id());
        if (dev.second->isPresent()) {
            if (dev.second->isError()) {
                os << std::setw(15) << "error";
            } else if (dev.second->isWarning()) {
                os << std::setw(15) << "warning";
            } else {
                os << std::setw(15) << "present";
            }
        } else {
            os << std::setw(15) << "MISSING!";
        }
        os << std::setw(10) << dev.second->countWarnings();
        os << std::setw(10) << dev.second->countErrors();
        os << std::setw(10) << dev.second->countMissings();
        if (dev.second->countWarnings() > 0 || dev.second->countErrors() > 0) {
            if (dev.second->lastFlags() & ResponseOverload) os << " Overload";
            if (dev.second->lastFlags() & ResponseOverheat) os << " Overheat";
            if (dev.second->lastFlags() & ResponseBadVoltage) os << " BadVoltage";
            if (dev.second->lastFlags() & ResponseQuiet) os << " Quiet";
            if (dev.second->lastFlags() & ResponseBadChecksum) os << " BadChecksum";
            if (dev.second->lastFlags() & ResponseDeviceBadInstruction) os << " DeviceBadInstruction";
            if (dev.second->lastFlags() & ResponseDeviceBadChecksum) os << " DeviceBadChecksum";
            if (dev.second->lastFlags() & ResponseBadSize) os << " BadSize";
            if (dev.second->lastFlags() & ResponseBadProtocol) os << " BadProtocol";
            if (dev.second->lastFlags() & ResponseBadId) os << " BadId";
        } else {
            os << std::setw(20) << " ";
        }
        os << std::endl;
    }

    return os.str();
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
        const auto& allDevices = _manager->devContainer();
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
        
std::string RhIOBinding::cmdInit(
    std::vector<std::string> argv)
{
    (void)argv;
    const auto& allDevices = _manager->devContainer();
    //Iterate over all DXL Devices
    for (const auto& dev : allDevices) {
        DXL* pt = dynamic_cast<DXL*>(dev.second);
        if (pt != nullptr) {
            pt->setGoalPositionSmooth(0.0, 1.0);
        }
    }
    _manager->exitEmergencyState();

    return "Init all servos";
}

std::string RhIOBinding::cmdChangeId(
    std::vector<std::string> argv)
{
    if (argv.size() != 2) {
        return "Invalid Argument. Usage:\nchangeId [old_id] [new_id]";
    }
    size_t oldId = std::stoi(argv[0]);
    size_t newId = std::stoi(argv[1]);
    _manager->changeDeviceId(oldId, newId);
    return "Change id from" + argv[0] 
        + " to " + argv[1] + ". RhAL Exit.";
}

std::string RhIOBinding::cmdTare(
    std::vector<std::string> argv)
{
    (void)argv;
    std::vector<PressureSensorBase*> sensors;
    std::map<PressureSensorBase*, std::vector<std::vector<double>>> zeros;
    const auto& allDevices = _manager->devContainer();
    for (const auto& dev : allDevices) {
        PressureSensorBase* ps = dynamic_cast<PressureSensorBase*>(dev.second);
        if (ps != nullptr) {
            sensors.push_back(ps);
            zeros[ps] = std::vector<std::vector<double>>();
            for (int g=0; g<ps->gauges(); g++) {
                // Reseting the zeros
                ps->setZero(g, 0);
                zeros[ps].push_back(std::vector<double>());
            }
        }
    }

    if (sensors.size() == 0) {
        return "No pressure devices found";
    } else {
        _manager->waitNextFlush();
        std::stringstream ss;
        int samples = 250;
        for (int k=0; k<samples; k++) {
            for (const auto& ps : sensors) {
                for (int g=0; g<ps->gauges(); g++) {
                    zeros[ps][g].push_back(ps->gain(g)*ps->pressure(g));
                }
            }
            _manager->waitNextFlush();
        }
        std::vector<std::string> errors;
        for (const auto& ps : sensors) {
            ss.precision(30);
            for (int g=0; g<ps->gauges(); g++) {
                double avg;
                double dev = rhoban_utils::standardDeviation(zeros[ps][g], &avg);
                if (dev > ps->getMaxStdDev()) {
                    std::stringstream tmp;
                    tmp << "Too high deviation for " << ps->name() << " #" << g;
                    errors.push_back(tmp.str());
                }
                if (dev < ps->getMinStdDev()) {
                    std::stringstream tmp;
                    tmp << "Too low deviation  for " << ps->name() << " #" << g;
                    errors.push_back(tmp.str());
                }
                ps->setZero(g, avg);
            }
        }
        for (auto &err : errors) {
            ss << "Error: " << err << std::endl;
        }
        ss << "Tare on " << sensors.size() << " devices.";
        return ss.str();
    }
}

std::string RhIOBinding::cmdGyroTare(
    std::vector<std::string> argv)
{
    (void)argv;
    std::vector<GY85*> sensors;
    std::map<GY85*, std::map<std::string, std::vector<double>>> zeros;

    auto allDevices = _manager->devContainer();
    for (auto& dev : allDevices) {
        GY85 *gy85 = dynamic_cast<GY85*>(dev.second);
        if (gy85 != nullptr) {
            sensors.push_back(gy85);
            zeros[gy85] = std::map<std::string, std::vector<double>>();
            zeros[gy85]["x"] = std::vector<double>();
            zeros[gy85]["y"] = std::vector<double>();
            zeros[gy85]["z"] = std::vector<double>();
        }
    }

    if (!sensors.size()) {
        return "No sensor found";
    } else {
        std::stringstream ss;
        int samples = 300;
        for (int k=0; k<samples; k++) {
            for (auto &gy85 : sensors) {
                zeros[gy85]["x"].push_back(gy85->getGyroXRaw()/(float)samples);
                zeros[gy85]["y"].push_back(gy85->getGyroYRaw()/(float)samples);
                zeros[gy85]["z"].push_back(gy85->getGyroZRaw()/(float)samples);
            }
            _manager->waitNextFlush();
        }
        for (auto &gy85 : sensors) {
            double xAvg, xDev;
            xDev = rhoban_utils::standardDeviation(zeros[gy85]["x"], &xAvg);
            double yAvg, yDev;
            yDev = rhoban_utils::standardDeviation(zeros[gy85]["y"], &yAvg);
            double zAvg, zDev;
            zDev = rhoban_utils::standardDeviation(zeros[gy85]["z"], &zAvg);
            
            if (xDev > gy85->getMaxStdDev()) {
                ss << "Error: too high deviation for X" << std::endl;
            }
            if (yDev > gy85->getMaxStdDev()) {
                ss << "Error: too high deviation for X" << std::endl;
            }
            if (zDev > gy85->getMaxStdDev()) {
                ss << "Error: too high deviation for X" << std::endl;
            }
            
            gy85->setGyroCalibration(xAvg, yAvg, zAvg);
        }
        ss << "Done.";
        return ss.str();
    }
}

}

