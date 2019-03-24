#include <iostream>
#include <thread>
#include "RhAL.hpp"

typedef RhAL::Manager<RhAL::ExampleDevice1, RhAL::ExampleDevice2> Manager;

bool isOver = false;
Manager manager;

void thread_NoCooperative()
{
  std::cout << "=== Start No Cooperative Thread" << std::endl;
  size_t count = 0;
  while (!isOver)
  {
    std::cout << "--- No Cooperative Thread Cycle" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    manager.waitNextFlush();
    manager.waitNextFlush();
    count++;
  }
  std::cout << "=== Stop No Cooperative Thread count=" << count << std::endl;
}

void thread_Cooperative()
{
  std::cout << "=== Enable Cooperative Thread" << std::endl;
  manager.enableCooperativeThread();
  std::cout << "=== Start Cooperative Thread" << std::endl;
  size_t count = 0;
  while (!isOver)
  {
    std::cout << "--- Cooperative Thread Cycle" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    manager.waitNextFlush();
    count++;
  }
  std::cout << "=== Disable Cooperative Thread" << std::endl;
  manager.disableCooperativeThread();
  std::cout << "=== Stop Cooperative Thread count=" << count << std::endl;
}

int main()
{
  manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
  manager.setScheduleMode(true);

  std::cout << "=== Start Manager Thread" << std::endl;
  manager.startManagerThread([]() {
    std::cout << "--- Manager Cycle" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  std::thread t1 = std::thread(thread_NoCooperative);
  std::thread t2 = std::thread(thread_Cooperative);

  std::this_thread::sleep_for(std::chrono::seconds(4));

  std::cout << "=== Wait Thread Ending" << std::endl;
  isOver = true;
  t1.join();
  t2.join();

  std::cout << "=== Stop Manager Thread" << std::endl;
  manager.stopManagerThread();

  manager.getStatistics().print();

  return 0;
}
