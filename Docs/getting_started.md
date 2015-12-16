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

`̀``c++
auto servos = manager.getAll<RhAL::Device::Dynamixel>();
for (auto &servo : servos) {
    std::cout << "* Servo " << servo->getId() << std::endl;
    std::cout << "  Angle: " << servo->getAngle() << std::endl;
}
```

## Supported devices

* Dynamixel
* Dynaban
* Force sensor
* IMU

