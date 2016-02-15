#include <iostream>
#include <iomanip>
#include "RhAL.hpp"
#include <thread>

using namespace RhAL;

void printDevice(RhAL::Device* dev)
{
    std::cout
        << "Dev: id:" << dev->id()
        << " name:" << dev->name()
        << std::endl;
    std::cout << "    RegistersBool:" << std::endl;
    for (const auto& it : dev->registersList().containerBool()) {
        std::cout<<"    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
    }
    std::cout << "    RegistersInt:" << std::endl;
    for (const auto& it : dev->registersList().containerInt()) {
        std::cout << "    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
    }
    std::cout << "    RegistersFloat:" << std::endl;
    for (const auto& it : dev->registersList().containerFloat()) {
        std::cout << "    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
    }

}


void ReadWriteTest(const std::string dev="/dev/ttyACM0", int bauds=1000000) {
    StandardManager manager;

    std::cout<<"Opening port: "<<dev<<" at "<<bauds<<" bauds"<<std::endl;
    manager.setProtocolConfig(
        dev, bauds, "DynamixelV1");

    //Set Manager scheduling config mode
    manager.setScheduleMode(false);

    //Scan the bus
    manager.scan();
    std::cout << manager.saveJSON().dump(4) << std::endl;


    //Iterate over Manager Devices with types
    for (const auto& it : manager.devContainer<RhAL::Device>()) {
        RhAL::Device * dev = it.second;
        printDevice(dev);
    }

}



/**
 * Manager Devices manipulation example
 */
int main(int argc, char* argv[])
{
    if(argc>1){
        if(argc==2)
            ReadWriteTest(argv[1]);
        if(argc==3)
            ReadWriteTest(argv[1],atoi(argv[2]));
    }
    else
        ReadWriteTest();
    return 0;
}
