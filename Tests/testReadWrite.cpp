#include <iostream>
#include <iomanip>
#include "RhAL.hpp"
#include <thread>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define DEFAULT "\033[39m"

using namespace RhAL;

void printDevice(RhAL::Device* dev)
{
    std::cout
        << "Dev: id:" << dev->id()
        << " name:" << dev->name()
        << std::endl;
    // std::cout<<std::setfill('*');
    std::cout << "    RegistersBool:" << std::endl;
    for (const auto& it : dev->registersList().containerBool()) {
        // std::cout<<"    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
        std::cout<<std::right<<std::setw(25)<<it.first<<": "<<it.second->readValue().value<<" ";
        std::cout<<std::right<<std::setw(25)<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;

    }
    std::cout << "\n    RegistersInt:" << std::endl;
    for (const auto& it : dev->registersList().containerInt()) {
        // std::cout << "    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
        std::cout<<std::right<<std::setw(25)<<it.first<<": "<<it.second->readValue().value<<" ";
        std::cout<<std::right<<std::setw(25)<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
    }
    std::cout << "\n    RegistersFloat:" << std::endl;
    for (const auto& it : dev->registersList().containerFloat()) {
        // std::cout << "    --" << it.first <<": "<<it.second->readValue().value<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
        std::cout<<std::right<<std::setw(25)<<it.first<<": "<<it.second->readValue().value<<" ";
        std::cout<<std::right<<std::setw(25)<<" ReadOnly: "<<it.second->isReadOnly<< std::endl;
    }

}

void testDevice(RhAL::Device* dev)
{
    std::cout
        << "Dev: id:" << dev->id()
        << " name:" << dev->name()
        << std::endl;

    std::cout << "    RegistersBool:" << std::endl;
    for (const auto& it : dev->registersList().containerBool()) {

        if(!it.second->isReadOnly && it.first.compare("lockEeprom"))
        {

            // for(int i=0;i<10;i++)
            {
            bool res=false;
            auto tmpval=it.second->readValue();

            it.second->writeValue(!(tmpval.value));

            auto tmpval2=it.second->readValue();
            if(tmpval2.value!=tmpval.value)
                res=true;

            std::cout<<std::right<<std::setw(25)<<it.first<<": wrote "<<(tmpval.value?"(false)":"(true)")<<" read "<<(tmpval2.value?"(true)":"(false)")<<" time ("<<getTimeDuration<TimeDurationMicro>(tmpval.timestamp,tmpval2.timestamp).count()<<"us) ";
            // std::cout<<(res?"PASS":"FAIL")<<std::endl;
            if(res)
                std::cout<<GREEN<<"PASS"<<DEFAULT<<std::endl;
            else
                std::cout<<RED<<"FAIL"<<DEFAULT<<std::endl;

            //try to restore
            it.second->writeValue(tmpval.value);
            }
        }

    }


    std::cout << "\n    RegistersInt:" << std::endl;
    for (const auto& it : dev->registersList().containerInt()) {
        if(!it.second->isReadOnly && it.first.compare("statusReturnLevel") && it.first.compare("baudrate") && it.first.compare("returnDelayTime")&& it.first.compare("id"))
        {

            // for(int i=0;i<10;i++)
            {
            bool res=false;
            auto tmpval=it.second->readValue();
            int delta=1;
            it.second->writeValue((tmpval.value)+delta);

            auto tmpval2=it.second->readValue();
            if(tmpval2.value==(tmpval.value+delta))
                res=true;

            std::cout<<std::right<<std::setw(25)<<it.first<<": wrote ("<<tmpval.value+delta<<") read ("<<tmpval2.value<<") duration ("<<getTimeDuration<TimeDurationMicro>(tmpval.timestamp,tmpval2.timestamp).count()<<"us) ";//<<(res?"PASS":"FAIL")<<std::endl;

            if(res)
                std::cout<<GREEN<<"PASS"<<DEFAULT<<std::endl;
            else
                std::cout<<RED<<"FAIL"<<DEFAULT<<std::endl;

            //try to restore
            it.second->writeValue(tmpval.value);
            }
        }

    }

    std::cout << "\n    RegistersFloat:" << std::endl;
    for (const auto& it : dev->registersList().containerFloat()) {

        if(!it.second->isReadOnly)
        {

            // for(int i=0;i<10;i++)
            {
            bool res=false;
            float delta=0.1;
            auto tmpval=it.second->readValue();

            it.second->writeValue(tmpval.value+delta);

            auto tmpval2=it.second->readValue();
            if(tmpval2.value==(tmpval.value+delta))
                res=true;


            std::cout<<std::right<<std::setw(25)<<it.first<<": wrote ("<<tmpval.value+delta<<") written ("<<it.second->getWrittenValue()<<") read ("<<tmpval2.value<<") duration ("<<getTimeDuration<TimeDurationMicro>(tmpval.timestamp,tmpval2.timestamp).count()<<"us) ";//<<(res?"PASS":"FAIL")<<std::endl;
            if(res)
                std::cout<<GREEN<<"PASS"<<DEFAULT<<std::endl;
            else
                std::cout<<RED<<"FAIL"<<DEFAULT<<std::endl;

            //try to restore
            it.second->writeValue(tmpval.value);
            }
        }

    }

    std::cout<<std::endl;

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
        testDevice(dev);
    }
    manager.getStatistics().print();

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
