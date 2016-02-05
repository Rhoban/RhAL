# RhAL

Rhoban hardware abstraction library. You can use it to interract
with the supported devices.

## Documentation

* [Getting started](/Docs/getting_started.md)
* Supported buses
    * Serial
* Supported protocol
    * [Dynamixel V1](/Docs/dynamixel_v1.md)
    * [Dynamixel V2](/Docs/dynamixel_v2.md)
* Supported devices
    * Dynamixel
    * Dynaban
    * IMU
    * Force sensor
* [The `rhal.json` configuration](/Docs/configuration.md)
* [The `rhal` command line tool](/Docs/command_line.md)

## TO DO
- The timeout value has to be fixed by the user (currently it has a fixed constant value). It should be chosen depending on the servo configuration (e.g. "return delay time" for a dxl servo) and baudrate.
- Check endianness in the writeFloatToBuffer method
- Add a method "getStallTorque" instead of the stallTorqueAt12V, and add the "smart" function that calculates the new stall torque with a voltage measure
