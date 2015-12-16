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
    "devices:" [
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
