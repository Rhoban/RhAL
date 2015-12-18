#include <vector>
#include <iostream>
#include <utils.h>
#include <Bus/SerialBus.hpp>
#include <Protocol/DynamixelV1.hpp>

using namespace std;
using namespace RhAL;

int main()
{
    RhAL::SerialBus bus("/dev/ttyACM0", 1000000);
    RhAL::DynamixelV1 protocol(bus);

    double start = getTime();
    int c = 0;
    while (getTime()-start < 60.0) {
        c++;
        protocol.ping(241);
    }
    std::cout << c << std::endl;
}
