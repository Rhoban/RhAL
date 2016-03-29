#include <unistd.h>
#include <vector>
#include <iostream>
#include <tclap/CmdLine.h>
#include "RhAL.hpp"
#include "Bindings/RhIOBinding.hpp"

int main(int argc, char** argv)
{
    // Reading command line
    TCLAP::CmdLine cmd("RhAL command line tool", ' ', "0.1");
    TCLAP::ValueArg<std::string> port("l", "port", "Serial port", false, "/dev/ttyACM0", "port", cmd);
    TCLAP::ValueArg<int> speed("s", "speed", "Baudrate", false, 1000000, "speed", cmd);
    TCLAP::ValueArg<std::string> protocol("p", "protocol", "Protocol", false, "DynamixelV1", "protocol", cmd);
    TCLAP::ValueArg<std::string> config("c", "config", "Config path", false, "", "filepath", cmd);
    cmd.parse(argc, argv);

    // Creating the manager
    RhAL::StandardManager manager;

    // Default configuration
    std::cout 
        << "Protocol config: port=" << port.getValue() 
        << " speed=" << speed.getValue() 
        << " protocol=" << protocol.getValue() 
        << std::endl;
    manager.setProtocolConfig(port.getValue(), speed.getValue(), protocol.getValue());
    // Schedule mode
    manager.setScheduleMode(false);
    // Try to read config file
    if (config.getValue() != "") {
        std::cout << "Reading RhAL configuration: " 
            << config.getValue() << std::endl;
        manager.readConfig(config.getValue());
    }

    // Scanning 
    std::cout << "Scanning the bus..." << std::endl;
    manager.scan();

    // Running the binding
    std::cout << "Starting RhIO binding" << std::endl;
    RhAL::RhIOBinding binding(manager);
    while (true) {
        sleep(1);
    }

    return 0;
}

