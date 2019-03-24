#include <iostream>
#include <thread>
#include "RhAL.hpp"

typedef RhAL::Manager<RhAL::ExampleDevice1, RhAL::ExampleDevice2> Manager;

static Manager manager;

void userThread()
{
  manager.enableCooperativeThread();

  // Get references on derived Devices
  RhAL::ExampleDevice1& dev2 = manager.dev<RhAL::ExampleDevice1>(2);
  RhAL::ExampleDevice1& dev3 = manager.dev<RhAL::ExampleDevice1>(3);
  RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);

  for (size_t k = 0; k < 3; k++)
  {
    std::cout << "---- User cycle " << k << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "Dev2Position: " << dev2.position() << std::endl;
    std::cout << "Dev2Temperature: " << dev2.temperature() << std::endl;
    dev2.goal() = 0.2;
    dev3.goal() = 0.3;
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    // Wait the next Manager cycle
    //(wait for swap and selection of register for read/write)
    manager.waitNextFlush();
  }

  manager.disableCooperativeThread();
}

int main()
{
  // Add new Device with type, id and name
  manager.devAdd<RhAL::ExampleDevice1>(2, "devTest2");
  manager.devAdd<RhAL::ExampleDevice1>(3, "devTest3");
  manager.devAdd<RhAL::ExampleDevice2>(5, "devTest5");

  // Get references on derived Devices
  RhAL::ExampleDevice1& dev2 = manager.dev<RhAL::ExampleDevice1>(2);
  RhAL::ExampleDevice1& dev3 = manager.dev<RhAL::ExampleDevice1>(3);
  RhAL::ExampleDevice2& dev5 = manager.dev<RhAL::ExampleDevice2>(5);

  //
  manager.dev(2).registersList().regFloat("goal").setCallbackWrite(
      [](float val) { std::cout << "Write callback: " << val << std::endl; });
  manager.dev(2).registersList().regFloat("position").setCallbackRead([](float val) {
    std::cout << "Read callback: " << val << std::endl;
  });

  // Display Device 2 state
  std::cout << "Dev2Position: " << dev2.position() << std::endl;
  std::cout << "Dev2Temperature: " << dev2.temperature() << std::endl;

  // Read/Write operations then swapping
  manager.flush(true);

  // Display Device 2 state
  std::cout << "Dev2Position: " << dev2.position() << std::endl;
  std::cout << "Dev2Temperature: " << dev2.temperature() << std::endl;
  // Write Goal
  dev2.goal() = 0.1;

  // Read/Write operations then swapping
  manager.flush(true);

  // Display Device 2 state
  //(only position have been updated)
  std::cout << "Dev2Position: " << dev2.position() << std::endl;
  std::cout << "Dev2Temperature: " << dev2.temperature() << std::endl;
  std::cout << std::endl;

  // Declare user threads
  std::thread t1(userThread);
  std::this_thread::sleep_for(std::chrono::milliseconds(200));
  std::thread t2(userThread);

  manager.startManagerThread([]() {
    std::cout << "---- Manager cycle " << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  });
  std::this_thread::sleep_for(std::chrono::milliseconds(4 * 2000));

  t1.join();
  t2.join();

  manager.stopManagerThread();

  // Display Manager statistics
  manager.getStatistics().print();

  return 0;
}
