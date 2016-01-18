#include <vector>
#include <iostream>
#include <Bus/SerialBus.hpp>
#include <Protocol/DynamixelV1.hpp>

using namespace std;
using namespace RhAL;

int main()
{
    RhAL::SerialBus bus("/dev/ttyACM0", 1000000);
    RhAL::DynamixelV1 protocol(bus);

    double start = getTimeDouble();
    int c = 0;
    while (getTimeDouble()-start < 60.0) {
        c++;
        protocol.ping(241);
    }
    std::cout << c << std::endl;
}
