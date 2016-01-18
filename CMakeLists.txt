###
### Rhoban Hardware Abstraction Layer library
### Rhoban 2015
###

cmake_minimum_required(VERSION 2.8)
project(RhAL)

set(LIB_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Lib")
set(BIN_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Bin")
set(TESTS_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Tests")
set(VENDORS_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendors")

# Enable C++11
set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -std=c++11")
# Enable compiler Warning
set(CMAKE_CXX_FLAGS 
    "${CMAKE_CXX_FLAGS} -W -Wall")

# Include sources directory
include_directories(${LIB_SOURCES_DIRECTORY})

# Serial library
add_subdirectory(${VENDORS_DIRECTORY}/serial)
include_directories("${VENDORS_DIRECTORY}/serial/include")
# Json library
include_directories("${VENDORS_DIRECTORY}/json/src")

# Lib source files
set(LIB_SOURCES
    Bus/Bus.cpp
    Bus/SerialBus.cpp
    Protocol/Protocol.cpp
    Protocol/DynamixelV1.cpp
)
# Prefix with lib directory path
foreach(SOURCE ${LIB_SOURCES})
    set (PREFIXED_LIB_SOURCES 
        ${PREFIXED_LIB_SOURCES}
        ${LIB_SOURCES_DIRECTORY}/${SOURCE}
    )
endforeach()

# Tests source files
set(TESTS_SOURCES
    testRegister
    testJSON
    testParameters
    testDevice
    testBaseManager
    testManager
)

# Build the RhAL Library
add_library(LibRhAL ${PREFIXED_LIB_SOURCES})
target_link_libraries(LibRhAL serial)

# Build the utility binary
add_executable(rhal ${BIN_SOURCES_DIRECTORY}/rhal.cpp)
target_link_libraries(rhal LibRhAL)

# Build all Tests binary
foreach(TEST ${TESTS_SOURCES})
    add_executable(${TEST} "${TESTS_SOURCES_DIRECTORY}/${TEST}.cpp")
    target_link_libraries(${TEST} LibRhAL)
endforeach()

