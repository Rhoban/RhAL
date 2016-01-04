#include "tests.h"
#include "Register.hpp"

int main()
{
    //Test initialization
    RhAL::Register reg("regName", 0xFF, 4, false);
    assertEquals(reg.name, "regName");
    assertEquals(reg.addr, (RhAL::addr_t)0xFF);
    assertEquals(reg.length, (size_t)4);
    assertEquals(reg.lastRead, RhAL::TimePoint());
    assertEquals(reg.lastWrite, RhAL::TimePoint());
    assertEquals(reg.isFetched, false);
    *((uint32_t*)reg.data) = (uint32_t)42;
    assertEquals(*((uint32_t*)reg.data), (uint32_t)42);

    RhAL::Register reg2 = reg;
    assertEquals(reg2.name, "regName");
    assertEquals(reg2.addr, (RhAL::addr_t)0xFF);
    assertEquals(reg2.length, (size_t)4);
    assertEquals(reg2.lastRead, RhAL::TimePoint());
    assertEquals(reg2.lastWrite, RhAL::TimePoint());
    assertEquals(reg2.isFetched, false);
    assertEquals(*((uint32_t*)reg2.data), (uint32_t)42);

    //Test container insersion
    std::vector<RhAL::Register> container;
    container.push_back(reg);
    container.push_back(reg);

    return 0;
}

