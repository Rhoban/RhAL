#include "Bus/SerialBus.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/DynamixelV1.hpp"
#include <iostream>
#include "types.h"
#include <vector>
#include <iomanip>
#include "Devices/RX.hpp"
#include <chrono>
#include <unistd.h>

using namespace RhAL;

int main(int argc, char* argv[])
{

    if(argc!=2)
    {
        std::cout<<"Need the dev"<<std::endl;
        return -1;
    }
    SerialBus* bus= new SerialBus(argv[1], 1000000);
    Protocol* protocol=new DynamixelV1(*bus);

    /*
    //ping
    for(int i=0;i<253;i++)
    {
    bool ret=protocol->ping(i);
    if(ret)
    std::cout<<"PING: "<<i<<std::endl;
    }
    */


    std::vector<RhAL::id_t> ids={6,38};
    addr_t addr_pos=0x24;
    addr_t addr_goal=0x1e;

    std::vector<uint8_t*> datas;
    size_t size=2;
    // uint8_t d[size];

    uint8_t testw[2]={0,0};
    std::vector<const uint8_t*> dataswrite;

    for(size_t i=0;i<ids.size();i++)
    {
        datas.push_back(new uint8_t(size));
        // dataswrite.push_back(new uint8_t(size));
        dataswrite.push_back(testw);

        for(size_t j=0;j<size;j++)
        {
            datas[i][j]=0;
            // dataswrite[i][j]=0;
        }
    }
    int nberrors=0;

    TimePoint tp1=getTimePoint();

    for(int j=0;j<1000;j++)
    {

        TimePoint tpr1;
        TimePoint tpr2;

        // tpr1=getTimePoint();
        // protocol->syncWrite((const std::vector<RhAL::id_t>)ids,
        //                     addr_goal,
        //                     dataswrite,
        //                     size);

        // tpr2=getTimePoint();
        // usleep(100);
        // std::cout<<"WRITE ("<<getTimeDuration<TimeDurationMicro>(tpr1,tpr2).count()<<"us)"<<std::endl;

        tpr1=getTimePoint();
        std::vector<ResponseState> states = protocol->syncRead(ids,
                                                               addr_pos,
                                                               datas,
                                                               size);
        tpr2=getTimePoint();

        std::cout<<"RESPONSE ("<<getTimeDuration<TimeDurationMicro>(tpr1,tpr2).count()<<"us)"<<std::endl;
        for(size_t i=0;i<states.size();i++)
        {
            if(states[i]!=ResponseOK)
                nberrors++;
            // printf("Err: 0x%x data: 0x%x 0x%x %f",states[i],(uint8_t) *datas[i],(uint8_t) *(datas[i]+1),convDecode_PositionRx(datas[i]));
            printf("id: %d RetCode: 0x%x data: 0x%x 0x%x\n",(int)ids[i],states[i],(uint8_t) *datas[i],(uint8_t) *(datas[i]+1));
        }
        std::cout<<std::endl;

        // usleep(100);
        tpr1=getTimePoint();
        protocol->syncWrite((const std::vector<RhAL::id_t>)ids,
                            addr_goal,
                            dataswrite,
                            size);

        tpr2=getTimePoint();
        std::cout<<"WRITE ("<<getTimeDuration<TimeDurationMicro>(tpr1,tpr2).count()<<"us)"<<std::endl;
        // usleep(50);
        // tpr1=getTimePoint();
        // protocol->syncWrite((const std::vector<RhAL::id_t>)ids,
        //                     addr_goal,
        //                     dataswrite,
        //                     size);

        // tpr2=getTimePoint();
        // std::cout<<"WRITE ("<<getTimeDuration<TimeDurationMicro>(tpr1,tpr2).count()<<"us)"<<std::endl;
        usleep(100);
    }

    TimePoint tp2=getTimePoint();

    std::cout<<"nbErrors: "<<nberrors<<std::endl;
    std::cout<<"Total Time: "<<getTimeDuration<TimeDurationMicro>(tp1,tp2).count()<<"us"<<std::endl;
    std::cout<<"Average Time: "<<getTimeDuration<TimeDurationMicro>(tp1,tp2).count()/1000.0<<"us"<<std::endl;
}
