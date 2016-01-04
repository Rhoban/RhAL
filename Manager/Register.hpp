#pragma once

#include <algorithm>
#include "types.h"

namespace RhAL {

/**
 * Register
 *
 * Named data register
 * in device memory used to
 * communicate with.
 */
struct Register
{
    //Textual name
    const std::string name;
    //Address in device memory
    const addr_t addr;
    //Register length in bytes
    const size_t length;
    //Timestamp at last read from device and
    //write from user
    TimePoint lastRead;
    TimePoint lastWrite;
    //When true, the register is marked to
    //be read during flushRead
    const bool isFetched;
    //Allocated data
    data_t* data;

    /**
     * Initialize and allocate a new register 
     * with given name, address and data length.
     * If isFetched is true, the register data will be fetch
     * at each flushRead of the manager.
     */
    inline Register(
        const std::string& name, 
        addr_t addr, 
        size_t length, 
        bool isFetched) :
        name(name),
        addr(addr),
        length(length),
        lastRead(),
        lastWrite(),
        isFetched(isFetched),
        data(nullptr)
    {
        data = new data_t[length];
    }

    /**
     * Data buffer desaloccation
     */
    inline ~Register()
    {
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
    }

    /**
     * Copy constructor
     */
    inline Register(const Register& reg) :
        name(reg.name),
        addr(reg.addr),
        length(reg.length),
        lastRead(reg.lastRead),
        lastWrite(reg.lastWrite),
        isFetched(reg.isFetched),
        data(nullptr)
    {
        data = new data_t[length];
        std::copy(reg.data, reg.data+length, data);
    }

    /**
     * Assignement operator
     */
    Register& operator=(const Register&) = delete;
};

}

