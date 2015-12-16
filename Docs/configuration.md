# The `rhal.json` configuration file

The `rhal.json` file contains hardware configuration for your hardware.
Here is an example:

```json
{
    "bus": {
        "type": "serial",
        "port": "/dev/ttyACM0",
        "baudrate": "1000000"
    },
    "protocol": "dynamixelV1",
    "devices": [
        {
            "name": "pan",
            "id": 123,
            "model": 64,
            "min": -150,
            "max": 150,
            "zero": 12.5
        },
        {
            "name": "tilt",
            "id": 124,
            "model": 64,
            "min": -20,
            "max": 80,
            "zero": 0
        }
    ]
}
```

## Writing the configuration

You can write the configuration using the [manager](getting_started.md):

```c++
// Scan for all available devices on the bus
manager.scan();
// Write the configuration
manager.writeConfiguration("rhal.json");
```

## Loading the configuration

Configuration can be read to instanciate the manager:

```c++
auto manager = RhAL::Manager::loadConfiguration("rhal.json");
```

