#include <iostream>
#include "RhAL.hpp"

typedef RhAL::Manager<
    RhAL::ExampleDevice1, 
    RhAL::RX64, 
    RhAL::ExampleDevice2> Manager;

void printDevice(const RhAL::Device& dev)
{
    std::cout 
        << "Dev: id:" << dev.id() 
        << " name:" << dev.name()
        << std::endl;
    std::cout << "    RegistersBool:" << std::endl;
    for (const auto& it : dev.registersList().containerBool()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    RegistersInt:" << std::endl;
    for (const auto& it : dev.registersList().containerInt()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    RegistersFloat:" << std::endl;
    for (const auto& it : dev.registersList().containerFloat()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersBool:" << std::endl;
    for (const auto& it : dev.parametersList().containerBool()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersNumber:" << std::endl;
    for (const auto& it : dev.parametersList().containerNumber()) {
        std::cout << "    --" << it.first << std::endl;
    }
    std::cout << "    ParametersStr:" << std::endl;
    for (const auto& it : dev.parametersList().containerStr()) {
        std::cout << "    --" << it.first << std::endl;
    }
}

/**
 * Manager Devices manipulation example
 */
int main()
{
    Manager manager;

    //manager.setProtocolConfig(
    //    "/dev/ttyUSB0", 1000000, "DynamixelV1");
    manager.setProtocolConfig(
        "", 1000000, "FakeProtocol");

    //Scan the bus
    //(no response with FakeProtocol)
    manager.scan();

    //Export configuration in file
    manager.writeConfig("/tmp/rhal.json");
    //Import configuration in file
    manager.readConfig("/tmp/rhal.json");

    //Set Manager scheduling config mode
    //(default is true)
    manager.setScheduleMode(true);

    //Add new Device with type, id and name
    manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
    manager.devAdd<RhAL::ExampleDevice1>(3, "devTest3");
    manager.devAdd<RhAL::ExampleDevice2>(5, "devTest5");

    //Iterate over Manager Devices with types
    for (const auto& it : manager.devContainer()) {
        //Retrieve Device model number and model name
        RhAL::type_t type = manager.devTypeNumberById(it.second->id());
        RhAL::type_t typebis = manager.devTypeNumberByName(it.first);
        std::string name = manager.devTypeName(it.second->name());
        (void)typebis;
        std::cout << "Device: name:" << it.first 
            << " id:" << it.second->id() 
            << " typeName:" << name
            << " typeNumber:" << type
            << std::endl;
    }

    //Device access
    const RhAL::Device& dev2 = manager.dev(2);
    const RhAL::Device& dev3 = manager.dev("devTest3");
    //Derived Device access
    const RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);
    const RhAL::ExampleDevice2& dev5bis = manager.dev<RhAL::ExampleDevice2>("devTest5");
    //Base class of derived Device access (dynamic cast)
    const RhAL::BaseExampleDevice1& dev2bis = manager.dev<RhAL::BaseExampleDevice1>(2);

    //Check if a Device or Derived Device exists
    bool isDev4 = manager.devExists(4);
    bool isDev2 = manager.devExists<RhAL::ExampleDevice2>("devTest2");
    std::cout << isDev4 << " " << isDev2 << std::endl;

    //Iterate over all Device ExampleDevice1
    for (const auto& it : manager.devContainer<RhAL::ExampleDevice1>()) {
        const RhAL::ExampleDevice1* pt = it.second;
        std::cout << "ExampleDevice1: " << pt->name() << std::endl;
    }
    std::map<std::string, RhAL::BaseExampleDevice1*> container = 
        manager.devContainer<RhAL::BaseExampleDevice1>();
    //Iterate over all Device BaseExampleDevice1
    for (const auto& it : container) {
        std::cout << "BaseExampleDevice1: " << it.second->name() << std::endl;
    }

    //Display Device registers and parameters
    printDevice(dev2);

    //Print Manager config
    std::cout << manager.saveJSON().dump(4) << std::endl;

    return 0;
}

