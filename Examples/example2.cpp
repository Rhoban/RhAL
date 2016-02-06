#include <iostream>
#include "RhAL.hpp"

typedef RhAL::Manager<
    RhAL::ExampleDevice1, 
    RhAL::ExampleDevice2> Manager;

int main()
{
    Manager manager;
    
    //Disable Manager scheduling mode
    //(no flush() call, immediate read/write)
    manager.setScheduleMode(false);
    
    //Add new Device with type, id and name
    manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
    manager.devAdd<RhAL::ExampleDevice1>(3, "devTest3");
    manager.devAdd<RhAL::ExampleDevice2>(5, "devTest5");

    //Get references on derived Devices
    RhAL::ExampleDevice1& dev2 = manager.dev<RhAL::ExampleDevice1>(2);
    RhAL::ExampleDevice1& dev3 = manager.dev<RhAL::ExampleDevice1>(3);
    RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);

    //Read/Write operations without flush()
    std::cout << "----" << std::endl;
    dev2.setGoal(0.1);
    
    std::cout << "----" << std::endl;
    dev2.setZero(1.5);
    dev2.setInverted(true);
    dev2.setGoal(0.0);
    
    std::cout << "----" << std::endl;
    RhAL::TimedValueFloat val1 = dev2.getPosition();
    std::cout << val1.value << std::endl;
    
    std::cout << "----" << std::endl;
    float val2 = dev3.getTemperature().value;
    std::cout << val2 << std::endl;
    
    std::cout << "----" << std::endl;
    float val3 = dev3.getVoltage();
    std::cout << val3 << std::endl;
    
    std::cout << "----" << std::endl;
    float val4 = dev5.getPitch().value;
    std::cout << val4 << std::endl;
    
    std::cout << "----" << std::endl;
    float val5 = dev5.getRoll().value;
    std::cout << val5 << std::endl;
    
    std::cout << "----" << std::endl;
    dev5.setMode(-0.3);
    
    //Enable Manager scheduling mode
    manager.setScheduleMode(true);

    //No write operation
    std::cout << "----" << std::endl;
    dev2.setGoal(0.0);
    
    //No read operation
    std::cout << "----" << std::endl;
    dev2.getPosition();

    //Write because the register is configured
    //as force write
    std::cout << "----" << std::endl;
    dev5.setMode(-0.2);
    
    //Read because the register is configured
    //as force read
    std::cout << "----" << std::endl;
    float val6 = dev5.getRoll().value;
    std::cout << val6 << std::endl;

    //Force a Register read
    std::cout << "----" << std::endl;
    float val7 = dev5.forcePitchRead().value;
    std::cout << val7 << std::endl;

    //Show Statistics
    manager.getStatistics().print();

    return 0;
}

