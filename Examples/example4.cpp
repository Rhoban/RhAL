#include <iostream>
#include <thread>
#include "RhAL.hpp"

typedef RhAL::Manager<
    RhAL::ExampleDevice1, 
    RhAL::ExampleDevice2> Manager;
    
int main()
{
    Manager manager;
    manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");

    manager.setScheduleMode(true);

    manager.addCooperativeThread();
    manager.startManagerThread([](){
        std::cout << "Manager thread wait start" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "Manager thread wait end" << std::endl;
    });

    for (size_t i=0;i<10;i++) {
        manager.waitNextFlush();
        std::cout << "User thread wait start" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::cout << "User thread wait end" << std::endl;
    }
    manager.removeCooperativeThread();
    manager.getStatistics().print();
    manager.stopManagerThread();

    return 0;
}

