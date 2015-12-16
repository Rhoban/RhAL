# Getting started

To use RhAL, you have to include `RhAL.hpp`, and instanciate
the manager:

```c++
#include <RhAL.hpp>

int main()
{
    RhAL::Bus::SerialBus bus("/dev/ttyACM0", 1000000);
    RhAL::Protocol::DynamixelV1 protocol(bus);
    RhAL::Manager manager(protocol);
}
```

Note: you can also use a [`rhal.json` configuration file](configuration.md)

You can then scan for available devices:

```c++
manager.scan();
```

And access all devices you want to:

```c++
auto servos = manager.getAll<RhAL::Device::Dynamixel>();
for (auto &servo : servos) {
    std::cout << "* Servo " << servo.getId() << std::endl;
    std::cout << "  Angle: " << servo.getAngle() << std::endl;
}
```

## Manager methods

### Getting a device

You can get devices using IDs or names using the following methods:

* `Type &get<Type>(int id)`
* `Type &get<Type>(std::string name)`

This will return a reference to the device. If it doesn't exist, an exeption
will raise.

See below for supported types.

### Getting all devices of a type

You can also get all devices of a specific types using:

* `std::vector<Type> getAll<Type>()`

### Using grouped read/write

You can use grouped read and write, enabling it with:

* `enableGroupedWrite(bool enable, int startAddress, int endAddress)`
* `enableGroupedRead(bool enable, int startAddress, int endAddress)`

Then, one can flush the read or write with:

* `flushRead()`
* `flushWrite()`

## Supported devices 

* Dynamixel
* Dynaban
* Force sensor
* IMU

