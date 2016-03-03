#include "Bus/SerialBus.hpp"
#include "Protocol/Protocol.hpp"
#include "Protocol/DynamixelV1.hpp"
#include <iostream>
#include "types.h"
#include <vector>
#include <iomanip>
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

    //ping
    std::cout<<"PING: "<<protocol->ping(1)<<std::endl;


    std::vector<RhAL::id_t> ids={1,2,3};
    addr_t addr=0x12;
    std::vector<uint8_t*> datas;
    size_t size=2;
    // uint8_t d[size];


    for(size_t i=0;i<ids.size();i++)
    {
        datas.push_back(new uint8_t(size));

        for(size_t j=0;j<size;j++)
            datas[i][j]=0;

    }
    std::vector<ResponseState> states = protocol->syncRead(
        ids,
        addr,
        datas,
        size);

    std::cout<<"RESPONSE ";
    for(size_t i=0;i<states.size();i++)
    {
        printf("Err: 0x%x data: 0x%x 0x%x ",states[i],(uint8_t) *datas[i],(uint8_t) *(datas[i]+1));
    }
    std::cout<<std::endl;
}
