#include <unistd.h>
#include <vector>
#include <iostream>
#include <RhAL.hpp>
#include <Bindings/RhioBinding.h>
#include <tclap/CmdLine.h>

using namespace std;
using namespace RhAL;

int main(int argc, char *argv[])
{
    // Reading command line
    TCLAP::CmdLine cmd("RhAL command line tool", ' ', "0.1");
    TCLAP::ValueArg<std::string> port("l", "port", "Serial port", false, "/dev/ttyACM0", "port", cmd);
    TCLAP::ValueArg<int> speed("s", "speed", "Baudrate", false, 1000000, "speed", cmd);
    TCLAP::ValueArg<std::string> protocol("p", "protocol", "Protocol", false, "DynamixelV1", "protocol", cmd);
    cmd.parse(argc, argv);

    // Creating the manager
    StandardManager manager;
    // Default configuration
    manager.setProtocolConfig(port.getValue(), speed.getValue(), protocol.getValue());
    // Try to read config file
    try {
        manager.readConfig("rhal.json");
    } catch (...) {
    }

    // Running the binding
    RhioBinding binding(&manager);
    binding.runScheduler();
    while (1) sleep(1);
}
