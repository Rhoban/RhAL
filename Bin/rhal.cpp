#include <vector>
#include <iostream>
#include <Bus/SerialBus.hpp>
#include <Protocol/DynamixelV1.hpp>
#include <tclap/CmdLine.h>

using namespace std;
using namespace RhAL;

int main(int argc, char *argv[])
{
    TCLAP::CmdLine cmd("RhAL command line tool", ' ', "0.1");
    TCLAP::SwitchArg scan("S", "scan", "Scan for devices", cmd, false);
    TCLAP::SwitchArg monitor("m", "monitor", "Monitors the devices", cmd, false);
    TCLAP::ValueArg<std::string> ping("p", "ping", "Pings a device", false, "", "name|id", cmd);
    TCLAP::ValueArg<std::string> dump("d", "dump", "Dumps registers of a device", false, "", "name|id", cmd);
    TCLAP::ValueArg<std::string> blink("b", "blink", "Blinks a motor", false, "", "name|id", cmd);
    TCLAP::ValueArg<std::string> set("r", "set", "Sets a register to a value", false, "", "name|id", cmd);
    TCLAP::ValueArg<std::string> value("v", "value", "Value of the register", false, "", "name|id", cmd);
    TCLAP::ValueArg<std::string> configure("c", "configure", "Configure a device", false, "", "name|id", cmd);
    TCLAP::SwitchArg configureAll("C", "configure-all", "Configures all device", cmd, false);
    TCLAP::SwitchArg save("s", "save", "Save the configuration", cmd, false);
    cmd.parse(argc, argv);

    if (scan.getValue()) {
        // XXX: Scan
    }

    if (monitor.getValue()) {
        // XXX: Monitor
    }

    if (ping.getValue() != "") {
        // XXX: Ping
    }

    if (dump.getValue() != "") {
        // XXX: Dump
    }

    if (blink.getValue() != "") {
        // XXX: Blink
    }

    if (set.getValue() != "") {
        if (value.getValue() == "") {
            std::cerr << "You should provide a value (with -v or --value)" << std::endl;
        } else {
            // XXX: Set a value
        }
    }

    if (configure.getValue() != "") {
        // XXX: Configure the servo
    }

    if (configureAll.getValue()) {
        // XXX: Configure all servos
    }

    if (save.getValue()) {
        std::cout << "Saving configuration..." << std::endl;
        /// XXX: Save
    }
}
