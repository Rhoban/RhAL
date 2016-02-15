#include <unistd.h>
#include <vector>
#include <iostream>
#include <RhAL.hpp>
#include <Bindings/RhioBinding.h>

using namespace std;
using namespace RhAL;

int main(int argc, char *argv[])
{
    StandardManager manager;
    // Default configuration
    manager.setProtocolConfig("/dev/ttyACM0", 1000000, "DynamixelV1");
    // Try to read config file
    try {
        manager.readConfig("rhal.json");
    } catch (...) {
    }
    RhioBinding binding(&manager);
    binding.runScheduler();

    while (1) sleep(1);
}
