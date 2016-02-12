#include <Manager/Manager.hpp>
#include <Devices/DXL.hpp>
#include "RhioBinding.h"

    RhioBinding::RhioBinding(Manager *manager_, std::string node_)
: manager(manager_)
{
    // Getting RhIO node into node
    RhIO::Root.newChild(node_);
    node = *(RhIO::Root.child(node_));

    // Binding methods
    node->bindFunc("scan", "Force the scan",
            &RhioBinding::cmdScan, *this);

    // Starts the update
    update();
}

void RhioBinding::update()
{
    auto allDevices = manager->getDevices();

    for (auto device : allDevices) {
        // The device doesn't have a corresponding RhIO node, let's
        // create it and populate it
        if (!node->childExist(device->name())) {
            node->newChild(device->name());
        }
        auto deviceNode = node->getChild(device->name());

        // Iterating over all registers
        for (const auto &entry : device->registerList().container()) {
            auto name = entry.first;
            auto reg = entry.second;

            if (!deviceNode->childExist(name)) {
                // Setup the node and corresponding callbacks
                if (auto registerBool = dynamic_cast<TypedRegisterBool*>(reg)) {
                    deviceNode->newBool(name, registerBool.readValue().value);
                    deviceNode->setCallbackBool(name, [&registerBool](bool newValue) {
                        registerBool->writeValue(newValue);
                    });
                    auto callback = [deviceNode, name](bool newValue) {
                        deviceNode->setBool(name, newValue);
                    };
                    registerFloat->addReadCallback(callback);
                    registerFloat->addWriteCallback(callback);
                } else if (auto registerInt = dynamic_cast<TypedRegisterBool*>(reg)) {
                    deviceNode->newInt(name, registerBool.readValue().value);
                    deviceNode->setCallbackInt(name, [&registerInt](int newValue) {
                        registerInt->writeValue(newValue);
                    });
                    auto callback = [deviceNode, name](int newValue) {
                        deviceNode->setInt(name, newValue);
                    };
                    registerFloat->addReadCallback(callback);
                    registerFloat->addWriteCallback(callback);
                } else if (auto registerFloat = dynamic_cast<TypedRegisterFloat*>(reg)) {
                    deviceNode->newFloat(name, registerBool.readValue().value);
                    deviceNode->setCallbackFloat(name, [&registerFloat](float newValue) {
                        registerFloat->writeValue(newValue);
                    });
                    auto callback = [deviceNode, name](float newValue) {
                        deviceNode->setFloat(name, newValue);
                    };
                    registerFloat->addReadCallback(callback);
                    registerFloat->addWriteCallback(callback);
                }
            }
        }
    }
}

std::string RhioBinding::cmdScan()
{
    manager.scan();
    update();

    return "Running a scan";
}
