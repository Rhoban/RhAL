#include "BaseManager.hpp"
#include "tests.h"
#include "ExampleDevice1.hpp"

int main()
{
    RhAL::BaseManager<RhAL::ExampleDevice1> manager;
    
    assertEquals(manager.devExistsById(1), false);
    assertEquals(manager.devExistsByName("test1"), false);
    assertEquals(manager.devContainer().size(), (size_t)0);

    manager.devAdd("test1", (RhAL::id_t)1);
    manager.devAdd("test2", (RhAL::id_t)2);

    assertEquals(manager.devExistsById(1), true);
    assertEquals(manager.devExistsByName("test1"), true);
    assertEquals(manager.devExistsById(2), true);
    assertEquals(manager.devExistsByName("test2"), true);
    assertEquals(manager.devContainer().size(), (size_t)2);
    assertEquals(manager.devContainer().count("test1"), (size_t)1);

    assertEquals(manager.devById(1).name(), "test1");
    assertEquals(manager.devByName("test1").name(), "test1");
    assertEquals(manager.devById(2).id(), (RhAL::id_t)2);
    assertEquals(manager.devByName("test2").id(), (RhAL::id_t)2);

    return 0;
}

