#include <Manager/Manager.hpp>
#include <Devices/DXL.hpp>
#include "RhioBinding.h"

using namespace RhIO;
using namespace RhAL;

RhioBinding::RhioBinding(BaseManager *manager_, std::string node_)
    : thread(NULL), manager(manager_), bind(node_)
{
    // Binding methods
    bind.bindFunc("scan", "Force the scan",
            &RhioBinding::cmdScan, *this);
    bind.bindFunc("readDev", "Read all registers from a device",
            &RhioBinding::cmdReadDev, *this, {"unknown"});

    // Starts the update
    update();
}

RhioBinding::~RhioBinding()
{
    if (thread) {
        over = true;
        thread->join();
        delete thread;
    }
}

void RhioBinding::runScheduler()
{
    // Runs the scheduler
    manager->setScheduleMode(true);
    thread = new std::thread([this] {
        while (!this->over) {
            manager->flush();
            update();
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    });
}

std::vector<Device*> RhioBinding::getDevices()
{
    std::vector<Device*> devices;
    auto all = manager->devContainer();

    for (auto entry : all) {
        devices.push_back(entry.second);
    }

    return devices;
}

void RhioBinding::update()
{
    auto allDevices = getDevices();

    for (auto device : allDevices) {
        // The device doesn't have a corresponding RhIO node, let's
        // create it and populate it
        auto node = &bind.node();
        if (!node->childExist(device->name())) {
            node->newChild(device->name());
        }
        auto deviceNode = &node->child(device->name());

        // Iterating over all registers
        for (const auto &entry : device->registersList().container()) {
            auto name = entry.first;
            auto reg = entry.second;

            if (deviceNode->getValueType(name) == NoValue) {
                TypedRegisterBool *registerBool;
                TypedRegisterInt *registerInt;
                TypedRegisterFloat *registerFloat;
                // Setup the node and corresponding callbacks
                if ((registerBool = dynamic_cast<TypedRegisterBool*>(reg)) != NULL) {
                    deviceNode->newBool(name);
                    deviceNode->setBool(name, registerBool->readValue().value);
                    deviceNode->setCallbackBool(name, [registerBool](bool newValue) {
                        registerBool->writeValue(newValue, true);
                    });
                    auto callback = [deviceNode, name](bool newValue) {
                        deviceNode->setBool(name, newValue, true);
                    };
                    registerBool->setCallbackRead(callback);
                    registerBool->setCallbackWrite(callback);
                } else if ((registerInt = dynamic_cast<TypedRegisterInt*>(reg)) != NULL) {
                    deviceNode->newInt(name);
                    deviceNode->setInt(name, registerInt->readValue().value);
                    deviceNode->setCallbackInt(name, [registerInt](int newValue) {
                        registerInt->writeValue(newValue, true);
                    });
                    auto callback = [deviceNode, name](int newValue) {
                        deviceNode->setInt(name, newValue, true);
                    };
                    registerInt->setCallbackRead(callback);
                    registerInt->setCallbackWrite(callback);
                } else if ((registerFloat = dynamic_cast<TypedRegisterFloat*>(reg)) != NULL) {
                    deviceNode->newFloat(name);
                    deviceNode->setFloat(name, registerFloat->readValue().value);
                    deviceNode->setCallbackFloat(name, [registerFloat](float newValue) {
                        registerFloat->writeValue(newValue, true);
                    });
                    auto callback = [deviceNode, name](float newValue) {
                        deviceNode->setFloat(name, newValue, true);
                    };
                    registerFloat->setCallbackRead(callback);
                    registerFloat->setCallbackWrite(callback);
                }
            }
       
            if (!deviceNode->childExist("parameters")) {
                deviceNode->newChild("parameters");
            }
            auto parametersNode = &deviceNode->child("parameters");

            // Iterating over all parameters and exporting to RhIO
            for (const auto &entry : device->parametersList().containerBool()) {
                auto name = entry.first;
                auto parameter = entry.second;

                if (parametersNode->getValueType(name) == NoValue) {
                    parametersNode->newBool(name);
                    parametersNode->setBool(name, parameter->value);
                    parametersNode->setCallbackBool(name, [parameter](bool newValue) {
                        parameter->value = newValue;
                    });
                } else {
                    parametersNode->setBool(name, parameter->value);
                }
            }
            for (const auto &entry : device->parametersList().containerStr()) {
                auto name = entry.first;
                auto parameter = entry.second;

                if (parametersNode->getValueType(name) == NoValue) {
                    parametersNode->newStr(name);
                    parametersNode->setStr(name, parameter->value);
                    parametersNode->setCallbackStr(name, [parameter](std::string newValue) {
                        parameter->value = newValue;
                    });
                } else {
                    parametersNode->setStr(name, parameter->value);
                }
            }
            for (const auto &entry : device->parametersList().containerNumber()) {
                auto name = entry.first;
                auto parameter = entry.second;

                if (parametersNode->getValueType(name) == NoValue) {
                    parametersNode->newFloat(name);
                    parametersNode->setFloat(name, parameter->value);
                    parametersNode->setCallbackFloat(name, [parameter](float newValue) {
                        parameter->value = newValue;
                    });
                } else {
                    parametersNode->setFloat(name, parameter->value, false);
                }
            }
        }
    }
}

std::string RhioBinding::cmdScan()
{
    manager->scan();
    update();

    return "Running a scan";
}
            
std::string RhioBinding::cmdReadDev(std::string name)
{
    auto &device = manager->devByName(name);
    for (const auto &entry : device.registersList().container()) {
        entry.second->forceRead();
    }
    update();

    std::stringstream ss;
    ss << "Reading all registers from device " << name;
    return ss.str();
}
