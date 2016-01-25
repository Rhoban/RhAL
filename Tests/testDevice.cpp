#include "Manager/ExampleDevice1.hpp"
#include "tests.h"
#include "Manager/Manager.hpp"

int main()
{
    RhAL::Manager<RhAL::ExampleDevice1> manager;
    RhAL::ExampleDevice1 dev("example", 1);

    assertEquals(dev.name(), "example");
    assertEquals(dev.id(), (RhAL::id_t)1);
    assertEquals(manager.typeNumber<RhAL::ExampleDevice1>(), (RhAL::type_t)1);
    assertEquals(manager.typeName<RhAL::ExampleDevice1>(), "ExampleDevice1");

    return 0;
}

