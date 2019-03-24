#include <iostream>
#include <random>
#include <thread>
#include "FakeProtocol.hpp"

static std::random_device generator;
static std::uniform_real_distribution<> dist(0.0, 1.0);

namespace RhAL
{
FakeProtocol::FakeProtocol(Bus& bus) : Protocol(bus), _verbose("verbose", false)
{
}

void FakeProtocol::writeData(id_t id, addr_t address, const uint8_t* data, size_t size)
{
  (void)data;
  if (_verbose.value)
  {
    std::cout << "WriteData id=" << id << " addr=" << address << " size=" << size;
    if (size == 4)
    {
      std::cout << " valFloat=" << *(reinterpret_cast<const float*>(data));
    }
    std::cout << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

ResponseState FakeProtocol::readData(id_t id, addr_t address, uint8_t* data, size_t size)
{
  (void)data;
  if (_verbose.value)
  {
    std::cout << "ReadData id=" << id << " addr=" << address << " size=" << size;
  }
  if (size == 4)
  {
    float val = dist(generator);
    *(reinterpret_cast<float*>(data)) = val;
    if (_verbose.value)
      std::cout << " valFloat=" << val;
  }
  if (_verbose.value)
    std::cout << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  return (ResponseOK | ResponseOverload | ResponseOverheat);
}

ResponseState FakeProtocol::writeAndCheckData(id_t id, addr_t address, const uint8_t* data, size_t size)
{
  (void)id;
  (void)address;
  (void)data;
  (void)size;
  if (_verbose.value)
  {
    std::cout << "Not implemented yet in fakeProtocol" << std::endl;
  }
  return ResponseOK;
}

bool FakeProtocol::ping(id_t id)
{
  if (_verbose.value)
  {
    std::cout << "Ping id=" << id << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  return false;
}

std::vector<ResponseState> FakeProtocol::syncRead(const std::vector<id_t>& ids, addr_t address,
                                                  const std::vector<uint8_t*>& datas, size_t size)
{
  (void)datas;
  std::vector<ResponseState> states;
  if (_verbose.value)
    std::cout << "SyncRead ids={";
  for (size_t i = 0; i < ids.size(); i++)
  {
    if (_verbose.value)
      std::cout << ids[i] << ",";
    states.push_back(ResponseOK);
  }
  if (_verbose.value)
  {
    std::cout << "} addr=" << address << " size=" << size;
  }
  if (size == 4)
  {
    if (_verbose.value)
      std::cout << " valFoat: ";
    for (size_t i = 0; i < datas.size(); i++)
    {
      float val = dist(generator);
      if (_verbose.value)
        std::cout << val << ", ";
      *(reinterpret_cast<float*>(datas[i])) = val;
    }
  }
  if (_verbose.value)
    std::cout << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  return states;
}

void FakeProtocol::syncWrite(const std::vector<id_t>& ids, addr_t address, const std::vector<const uint8_t*>& datas,
                             size_t size)
{
  (void)datas;
  if (_verbose.value)
  {
    std::cout << "SyncWrite ids={";
    for (size_t i = 0; i < ids.size(); i++)
    {
      std::cout << ids[i] << ",";
    }
    std::cout << "} addr=" << address << " size=" << size;
    if (size == 4)
    {
      std::cout << " valFoat: ";
      for (size_t i = 0; i < datas.size(); i++)
      {
        std::cout << *(reinterpret_cast<const float*>(datas[i])) << ", ";
      }
    }
    std::cout << std::endl;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

std::vector<ResponseState> FakeProtocol::syncWriteAndCheck(const std::vector<id_t>& ids, addr_t address,
                                                           const std::vector<const uint8_t*>& datas, size_t size)
{
  (void)address;
  (void)datas;
  (void)size;
  if (_verbose.value)
  {
    std::cout << "Not implemented yet in fakeProtocol" << std::endl;
  }
  std::vector<ResponseState> output;
  for (unsigned int i = 0; i < ids.size(); i++)
  {
    output.push_back(ResponseOK);
  }
  return output;
}

void FakeProtocol::emergencyStop()
{
  if (_verbose.value)
  {
    std::cout << "Emergency stop not implemented" << std::endl;
  }
}
void FakeProtocol::exitEmergencyState()
{
  if (_verbose.value)
  {
    std::cout << "Exit emergency state not implemented" << std::endl;
  }
}

}  // namespace RhAL
