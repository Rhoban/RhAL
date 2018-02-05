#include "Manager/Manager.hpp"
#include "tests.h"
#include "Devices/ExampleDevice1.hpp"
#include "Devices/ExampleDevice2.hpp"

int main()
{
    RhAL::Manager<
        RhAL::ExampleDevice1, 
        RhAL::ExampleDevice2
    > manager;

    assertEquals(manager.devContainer().size(), (size_t)0);
    assertEquals(manager.devExistsById(1), false);
    assertEquals(manager.devExistsByName("test2"), false);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice1>(1), false);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice2>("test2"), false);

    manager.devAdd<RhAL::ExampleDevice1>(1, "test1");
    manager.devAdd<RhAL::ExampleDevice2>(2, "test2");
    
    assertEquals(manager.devContainer().size(), (size_t)2);
    assertEquals(manager.devContainer<RhAL::ExampleDevice1>().size(), (size_t)1);
    assertEquals(manager.devContainer<RhAL::ExampleDevice2>().size(), (size_t)1);
    assertEquals(manager.devExistsById(1), true);
    assertEquals(manager.devExistsById(2), true);
    assertEquals(manager.devExistsByName("test1"), true);
    assertEquals(manager.devExistsByName("test2"), true);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice1>(1), true);
    assertEquals(manager.devExistsById<RhAL::ExampleDevice2>(1), false);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice2>("test2"), true);
    assertEquals(manager.devExistsByName<RhAL::ExampleDevice1>("test2"), false);

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
    
    const RhAL::BaseExampleDevice1& b1 = manager.devById<RhAL::BaseExampleDevice1>(1);
    const RhAL::BaseExampleDevice1& b2 = manager.devByName<RhAL::BaseExampleDevice1>("test1");

    manager.devAdd<RhAL::ExampleDevice1>(10, "Dev1Test1");
    manager.devAdd<RhAL::ExampleDevice1>(20, "Dev1Test2");
    manager.devAdd<RhAL::ExampleDevice1>(3, "Dev1Test3");
    manager.devAdd<RhAL::ExampleDevice2>(4, "Dev2Test4");
    manager.devAdd<RhAL::ExampleDevice2>(5, "Dev2Test5");
    manager.devAdd<RhAL::ExampleDevice2>(6, "Dev2Test6");

    //manager.devByName<RhAL::ExampleDevice1>("Dev1Test2").
    /*
    manager.flushRead();
    manager.flushRead();
    manager.flushRead();
    manager.flushRead();
    manager.flushRead();
    */
    manager.writeConfig("/tmp/a");
    manager.readConfig("/tmp/a");

    Json::Value j = manager.saveJSON();
    std::string json_str = Json::writeString(Json::StreamWriterBuilder(), j);
    std::cout << json_str << std::endl;
    /*
    manager.loadJSON(j);
    
    RhAL::Manager<
        RhAL::ExampleDevice1, 
        RhAL::ExampleDevice2
    > manager2;
    nlohmann::json j2 = manager2.saveJSON();
    std::cout << j2.dump(4) << std::endl;
    manager2.loadJSON(j2);
    std::cout << manager2.saveJSON().dump(4) << std::endl;
    manager2.loadJSON(j);
    manager2.loadJSON(j);
    std::cout << manager2.saveJSON().dump(4) << std::endl;
    */


    return 0;
}

