#include <iostream>
#include <thread>
#include "RhAL.hpp"

typedef RhAL::Manager<
    RhAL::ExampleDevice1, 
    RhAL::ExampleDevice2> Manager;
    
static Manager manager;

void userThread()
{
    //Get references on derived Devices
    RhAL::ExampleDevice1& dev2 = manager.dev<RhAL::ExampleDevice1>(2);
    RhAL::ExampleDevice1& dev3 = manager.dev<RhAL::ExampleDevice1>(3);
    RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);

    for (size_t k=0;k<3;k++) {
        std::cout << "---- User cycle " << k << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Dev2Position: " << dev2.getPosition().value << std::endl;
        std::cout << "Dev2Temperature: " << dev2.getTemperature().value << std::endl;
        dev2.setGoal(0.2);
        dev3.setGoal(0.3);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //Wait the next Manager cycle 
        //(wait for swap and selection of register for read/write)
        manager.waitNextFlush();
    }

    manager.removeCooperativeThread();
}

int main()
{
    //Add new Device with type, id and name
    manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
    manager.devAdd<RhAL::ExampleDevice1>(3, "devTest3");
    manager.devAdd<RhAL::ExampleDevice2>(5, "devTest5");
    
    //Get references on derived Devices
    RhAL::ExampleDevice1& dev2 = manager.dev<RhAL::ExampleDevice1>(2);
    RhAL::ExampleDevice1& dev3 = manager.dev<RhAL::ExampleDevice1>(3);
    RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);

    //Display Device 2 state
    std::cout << "Dev2Position: " << dev2.getPosition().value << std::endl;
    std::cout << "Dev2Temperature: " << dev2.getTemperature().value << std::endl;

    //Read/Write operations then swapping
    manager.flush(true);

    //Display Device 2 state
    std::cout << "Dev2Position: " << dev2.getPosition().value << std::endl;
    std::cout << "Dev2Temperature: " << dev2.getTemperature().value << std::endl;
    //Write Goal
    dev2.setGoal(0.1);
    
    //Read/Write operations then swapping
    manager.flush(true);
    
    //Display Device 2 state
    //(only position have been updated)
    std::cout << "Dev2Position: " << dev2.getPosition().value << std::endl;
    std::cout << "Dev2Temperature: " << dev2.getTemperature().value << std::endl;
    std::cout << std::endl;

    //Declare user threads
    manager.addCooperativeThread();
    manager.addCooperativeThread();
    std::thread t1(userThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::thread t2(userThread);

    for (size_t k=0;k<3;k++) {
        std::cout << "---- Manager cycle " << k << std::endl;
        manager.flush();
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }

    t1.join();
    t2.join();
    
    return 0;
}

