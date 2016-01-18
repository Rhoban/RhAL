#include <iostream>
#include "tests.h"
#include "Parameter.hpp"
#include "ParametersList.hpp"

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

    assertEquals(params.getBool("testBool").name, "testBool");
    assertEquals(params.getBool("testBool").value, false);
    assertEquals(params.getBool("testBool").defaultValue, false);
    assertEquals(params.getNumber("testNumber").name, "testNumber");
    assertEquals(params.getNumber("testNumber").defaultValue, 42.0);
    assertEquals(params.getNumber("testNumber").value, 42.0);
    assertEquals(params.getStr("testStr").name, "testStr");
    assertEquals(params.getStr("testStr").defaultValue, "is a test");
    assertEquals(params.getStr("testStr").value, "is a test");

    //Test modification
    p2.value = 3.0;
    assertEquals(params.getNumber("testNumber").value, 3.0);
    params.getNumber("testNumber").value = 2.0;
    assertEquals(params.getNumber("testNumber").value, 2.0);

    //Test copy constructor
    RhAL::ParametersList params2 = params;
    assertEquals(params2.getBool("testBool").name, "testBool");
    assertEquals(params2.getBool("testBool").value, false);
    assertEquals(params2.getBool("testBool").defaultValue, false);
    assertEquals(params2.getNumber("testNumber").name, "testNumber");
    assertEquals(params2.getNumber("testNumber").defaultValue, 42.0);
    assertEquals(params2.getNumber("testNumber").value, 2.0);
    assertEquals(params2.getStr("testStr").name, "testStr");
    assertEquals(params2.getStr("testStr").defaultValue, "is a test");
    assertEquals(params2.getStr("testStr").value, "is a test");

    //Test json import and export
    params.getBool("testBool").value = true;
    params.getNumber("testNumber").value = 3.0;
    params.getStr("testStr").value = "simple test";
    assertEquals(params.getBool("testBool").value, true);
    assertEquals(params.getNumber("testNumber").value, 3.0);
    assertEquals(params.getStr("testStr").value, "simple test");

    std::string str = params.saveJSON().dump(4);
    std::cout << str << std::endl;
    params2.loadJSON(nlohmann::json::parse(str));
    
    assertEquals(params2.getBool("testBool").value, true);
    assertEquals(params2.getNumber("testNumber").value, 3.0);
    assertEquals(params2.getStr("testStr").value, "simple test");

    return 0;
}

