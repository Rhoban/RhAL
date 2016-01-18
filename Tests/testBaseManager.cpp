#include "Manager/BaseManager.hpp"
#include "Manager/Manager.hpp"
#include "tests.h"
#include "Manager/ExampleDevice1.hpp"

int main()
{
    RhAL::Manager<RhAL::ExampleDevice1> manager;
    RhAL::BaseManager<RhAL::ExampleDevice1>* baseManager = &manager;
    
    assertEquals(baseManager->devExistsById(1), false);
    assertEquals(baseManager->devExistsByName("test1"), false);
    assertEquals(baseManager->devContainer().size(), (size_t)0);

    baseManager->devAdd("test1", (RhAL::id_t)1, &manager);
    baseManager->devAdd("test2", (RhAL::id_t)2, &manager);

    assertEquals(baseManager->devExistsById(1), true);
    assertEquals(baseManager->devExistsByName("test1"), true);
    assertEquals(baseManager->devExistsById(2), true);
    assertEquals(baseManager->devExistsByName("test2"), true);
    assertEquals(baseManager->devContainer().size(), (size_t)2);
    assertEquals(baseManager->devContainer().count("test1"), (size_t)1);

    assertEquals(baseManager->devById(1).name(), "test1");
    assertEquals(baseManager->devByName("test1").name(), "test1");
    assertEquals(baseManager->devById(2).id(), (RhAL::id_t)2);
    assertEquals(baseManager->devByName("test2").id(), (RhAL::id_t)2);

    return 0;
}

