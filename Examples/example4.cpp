#include <iostream>
#include <thread>
#include "RhAL.hpp"

typedef RhAL::Manager<RhAL::ExampleDevice1, RhAL::ExampleDevice2> Manager;

int main()
{
  Manager manager;
  manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");

  manager.setScheduleMode(true);

  manager.startManagerThread([]() {
    std::cout << "Manager thread wait start" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5));
    std::cout << "Manager thread wait end" << std::endl;
  });

  manager.enableCooperativeThread();
  for (size_t i = 0; i < 10; i++)
  {
    manager.waitNextFlush();
    std::cout << "User thread wait start" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "User thread wait end" << std::endl;
  }
  manager.disableCooperativeThread();

  manager.stopManagerThread();
  manager.getStatistics().print();

  return 0;
}
