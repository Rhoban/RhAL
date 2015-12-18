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

    std::vector<RhAL::id_t> ids;
    std::vector<uint8_t *> datas;
    uint8_t byte = 77;
    ids.push_back(241);
    datas.push_back(&byte);
    protocol.syncWrite(ids, 1, datas, 1);
    
    for (int k=0; k<25; k++) {
        std::cout << "Reg " << k << ": " << (int)protocol.readByte(241, k) << std::endl;
    }
}
