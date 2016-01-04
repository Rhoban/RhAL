#include "Manager.hpp"
#include "tests.h"
#include "ExampleDevice1.hpp"
#include "ExampleDevice2.hpp"

int main()
{
    RhAL::Manager<
        RhAL::ExampleDevice1, 
        RhAL::ExampleDevice2
    > manager;

    assertEquals(manager.devAll().size(), (size_t)0);
    assertEquals(manager.devExistsById(1), false);
    assertEquals(manager.devExistsByName("test2"), false);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice1>(1), false);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice2>("test2"), false);

    manager.devAdd<RhAL::ExampleDevice1>("test1", 1);
    manager.devAdd<RhAL::ExampleDevice2>("test2", 2);
    
    assertEquals(manager.devAll().size(), (size_t)2);
    assertEquals(manager.devAll<RhAL::ExampleDevice1>().size(), (size_t)1);
    assertEquals(manager.devAll<RhAL::ExampleDevice2>().size(), (size_t)1);
    assertEquals(manager.devExistsById(1), true);
    assertEquals(manager.devExistsById(2), true);
    assertEquals(manager.devExistsByName("test1"), true);
    assertEquals(manager.devExistsByName("test2"), true);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice1>(1), true);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice2>(1), false);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice2>("test2"), true);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice1>("test2"), false);

    assertEquals(manager.devAll<RhAL::ExampleDevice1>().front()->name(), "test1");
    assertEquals(manager.devAll<RhAL::ExampleDevice2>().front()->name(), "test2");

    const RhAL::Device& d1 = manager.devById(1);
    const RhAL::Device& d2 = manager.devById(2);
    const RhAL::Device& d3 = manager.devByName("test1");
    const RhAL::Device& d4 = manager.devByName("test2");
    assertEquals(d1.name(), "test1");
    assertEquals(d2.name(), "test2");
    assertEquals(d3.id(), (RhAL::id_t)1);
    assertEquals(d4.id(), (RhAL::id_t)2);
    
    const RhAL::ExampleDevice1& e1 = manager.devById<RhAL::ExampleDevice1>(1);
    const RhAL::ExampleDevice2& e2 = manager.devById<RhAL::ExampleDevice2>(2);
    const RhAL::ExampleDevice1& e3 = manager.devByName<RhAL::ExampleDevice1>("test1");
    const RhAL::ExampleDevice2& e4 = manager.devByName<RhAL::ExampleDevice2>("test2");
    assertEquals(e1.name(), "test1");
    assertEquals(e2.name(), "test2");
    assertEquals(e3.id(), (RhAL::id_t)1);
    assertEquals(e4.id(), (RhAL::id_t)2);

    return 0;
}

