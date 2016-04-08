#include <iostream>
#include <thread>
#include <chrono>
#include "RhAL.hpp"
#include "Bindings/RhIOBinding.hpp"

typedef RhAL::Manager<
    RhAL::ExampleDevice1, 
    RhAL::RX64, 
    RhAL::ExampleDevice2> Manager;

int main()
{
    //Initialize the Manager
    Manager manager;
    manager.setProtocolConfig(
        "", 1000000, "FakeProtocol");
    manager.setScheduleMode(true);
    
    //Add new Device with type, id and name
    manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
    manager.devAdd<RhAL::ExampleDevice1>(3, "devTest3");
    manager.devAdd<RhAL::ExampleDevice2>(5, "devTest5");
    manager.devAdd<RhAL::RX64>(10, "devDXL");

    manager.startManagerThread();

    //Start RhIO Binding
    RhAL::RhIOBinding binding(manager);
    std::this_thread::sleep_for(std::chrono::seconds(60));

    manager.stopManagerThread();

    return 0;
}

