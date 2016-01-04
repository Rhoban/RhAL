#include "ExampleDevice1.hpp"
#include "tests.h"

int main()
{
    RhAL::ExampleDevice1 dev("example", 2);
    dev.init();

    assertEquals(dev.name(), "example");
    assertEquals(dev.id(), (RhAL::id_t)2);
    assertEquals(dev.typeNumber(), (RhAL::type_t)0x01);
    assertEquals(dev.typeName(), "ExampleDevice1");

    return 0;
}

