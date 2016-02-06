#include <iostream>
#include "tests.h"
#include "Manager/Parameter.hpp"
#include "Manager/ParametersList.hpp"

int main()
{
    RhAL::ParametersList params;

    //Test insersion and retrieval
    assertEquals(params.exists("testBool"), false);
    assertEquals(params.exists("testNumber"), false);
    assertEquals(params.exists("testStr"), false);
    assertEquals(params.containerBool().size(), (size_t)0);
    assertEquals(params.containerNumber().size(), (size_t)0);
    assertEquals(params.containerStr().size(), (size_t)0);

    RhAL::ParameterBool p1("testBool", false);
    RhAL::ParameterNumber p2("testNumber", 42.0);
    RhAL::ParameterStr p3("testStr", "is a test");
    params.add(&p1);
    params.add(&p2);
    params.add(&p3);

    assertEquals(params.exists("testBool"), true);
    assertEquals(params.exists("testNumber"), true);
    assertEquals(params.exists("testStr"), true);
    assertEquals(params.containerBool().size(), (size_t)1);
    assertEquals(params.containerNumber().size(), (size_t)1);
    assertEquals(params.containerStr().size(), (size_t)1);

    assertEquals(params.paramBool("testBool").name, "testBool");
    assertEquals(params.paramBool("testBool").value, false);
    assertEquals(params.paramBool("testBool").defaultValue, false);
    assertEquals(params.paramNumber("testNumber").name, "testNumber");
    assertEquals(params.paramNumber("testNumber").defaultValue, 42.0);
    assertEquals(params.paramNumber("testNumber").value, 42.0);
    assertEquals(params.paramStr("testStr").name, "testStr");
    assertEquals(params.paramStr("testStr").defaultValue, "is a test");
    assertEquals(params.paramStr("testStr").value, "is a test");

    //Test modification
    p2.value = 3.0;
    assertEquals(params.paramNumber("testNumber").value, 3.0);
    params.paramNumber("testNumber").value = 2.0;
    assertEquals(params.paramNumber("testNumber").value, 2.0);

    //Test copy constructor
    RhAL::ParametersList params2 = params;
    assertEquals(params2.paramBool("testBool").name, "testBool");
    assertEquals(params2.paramBool("testBool").value, false);
    assertEquals(params2.paramBool("testBool").defaultValue, false);
    assertEquals(params2.paramNumber("testNumber").name, "testNumber");
    assertEquals(params2.paramNumber("testNumber").defaultValue, 42.0);
    assertEquals(params2.paramNumber("testNumber").value, 2.0);
    assertEquals(params2.paramStr("testStr").name, "testStr");
    assertEquals(params2.paramStr("testStr").defaultValue, "is a test");
    assertEquals(params2.paramStr("testStr").value, "is a test");

    //Test json import and export
    params.paramBool("testBool").value = true;
    params.paramNumber("testNumber").value = 3.0;
    params.paramStr("testStr").value = "simple test";
    assertEquals(params.paramBool("testBool").value, true);
    assertEquals(params.paramNumber("testNumber").value, 3.0);
    assertEquals(params.paramStr("testStr").value, "simple test");

    std::string str = params.saveJSON().dump(4);
    std::cout << str << std::endl;
    params2.loadJSON(nlohmann::json::parse(str));
    
    assertEquals(params2.paramBool("testBool").value, true);
    assertEquals(params2.paramNumber("testNumber").value, 3.0);
    assertEquals(params2.paramStr("testStr").value, "simple test");

    return 0;
}

