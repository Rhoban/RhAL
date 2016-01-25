#include "tests.h"
#include "Manager/Register.hpp"
#include "Manager/Manager.hpp"
#include "Manager/ExampleDevice1.hpp"

static void convIn(RhAL::data_t* buffer, long value)
{
    *(reinterpret_cast<long*>(buffer)) = value;
}
static long convOut(const RhAL::data_t* buffer)
{
   return  *(reinterpret_cast<const long*>(buffer));
}

int main()
{
    RhAL::Manager<RhAL::ExampleDevice1> manager;

    //Test Register
    RhAL::data_t buffer1[256];
    RhAL::data_t buffer2[256];
    RhAL::TypedRegisterBool reg("regName", 0xFF, 4, convIn, convOut, 0);
    reg.init(1, &manager, buffer1, buffer2);
    assertEquals(reg.id, (RhAL::id_t)1);
    assertEquals(reg.name, "regName");
    assertEquals(reg.addr, (RhAL::addr_t)0xFF);
    assertEquals(reg.length, (size_t)4);
    assertEquals(reg.periodPackedRead, (unsigned int)0);

    assertEquals(reg.needRead(), false);
    assertEquals(reg.needWrite(), false);
    reg.askRead();
    assertEquals(reg.needRead(), true);
    assertEquals(reg.needWrite(), false);
    reg.askWrite();
    assertEquals(reg.needRead(), true);
    assertEquals(reg.needWrite(), true);

    //Test TypedRegister
    RhAL::data_t buffer3[256];
    RhAL::data_t buffer4[256];
    RhAL::TypedRegisterInt reg2("reg2Name", 0x42, 4, convIn, convOut, 1);
    assertEquals(reg2.name, "reg2Name");
    assertEquals(reg2.addr, (RhAL::addr_t)0x42);
    assertEquals(reg2.length, (size_t)4);
    assertEquals(reg2.periodPackedRead, (unsigned int)1);
    
    reg2.init(2, &manager, buffer3, buffer4);
    assertEquals(reg2.id, (RhAL::id_t)2);
    assertEquals(reg2.needRead(), false);
    assertEquals(reg2.needWrite(), false);
    reg2.writeValue(4);
    assertEquals(reg2.needRead(), false);
    assertEquals(reg2.needWrite(), true);

    return 0;
}

