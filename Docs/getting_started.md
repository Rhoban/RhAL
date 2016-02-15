# Getting started

First of all, go to https://github.com/Rhoban/deps and follow the (short) instructions.

Then, clone this repository :
    `git clone https://github.com/Rhoban/RhAL.git`

And run :
    `deps install`

So far, so good.

To use RhAL, you have to include `RhAL.hpp`, and instanciate
the manager:

```c++
#include <RhAL.hpp>

int main()
{
    RhAL::SerialBus bus("/dev/ttyACM0", 1000000);
    RhAL::DynamixelV1 protocol(bus);
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

Another example, if you want to set the angle of the servo `32` to `3.1415`:

```c++
auto servo = manger.get<RhAL::Device::Dynamixel>(32);
servo.setAngle(3.1415);
```

## Manager methods

### Getting a device

You can get devices using IDs or names using the following methods:

* `Type &get<Type>(int id)`
* `Type &get<Type>(std::string name)`

This will return a reference to the device. If it doesn't exist, an exeption
will raise.

Go back to [the home for the list of supported types](/README.md).

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

