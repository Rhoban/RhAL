#include <iostream>
#include "FakeProtocol.hpp"

namespace RhAL
{
        
FakeProtocol::FakeProtocol(Bus& bus) :
    Protocol(bus)
{
}

void FakeProtocol::writeData(
    id_t id, addr_t address, 
    uint8_t *data, size_t size) 
{
    (void)data;
    std::cout << "WriteData id=" << id 
        << " addr=" << address
        << " size=" << size 
        << std::endl;
}

ResponseState FakeProtocol::readData(
    id_t id, addr_t address, 
    uint8_t *data, size_t size) 
{
    (void)data;
    std::cout << "ReadData id=" << id 
        << " addr=" << address
        << " size=" << size 
        << std::endl;
    return ResponseOK;
}

bool FakeProtocol::ping(id_t id) 
{
    std::cout << "Ping id=" << id << std::endl;
    return true;
}

std::vector<ResponseState> FakeProtocol::syncRead(
    std::vector<id_t> ids, addr_t address,
    std::vector<uint8_t*> datas, size_t size) 
{
    (void)datas;
    std::vector<ResponseState> states;
    std::cout << "SyncRead ids={";
    for (size_t i=0;i<ids.size();i++) {
        std::cout << ids[i] << ",";
        states.push_back(ResponseOK);
    }
    std::cout << "} addr=" << address 
        << " size=" << size << std::endl;

    return states;
}

void FakeProtocol::syncWrite(
    std::vector<id_t> ids, addr_t address,
    std::vector<uint8_t*> datas, size_t size) 
{
    (void)datas;
    std::cout << "SyncWrite ids={";
    for (size_t i=0;i<ids.size();i++) {
        std::cout << ids[i] << ",";
    }
    std::cout << "} addr=" << address 
        << " size=" << size << std::endl;
}

}

