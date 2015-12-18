###
### Rhoban Hardware Abstraction Layer library
### Rhoban 2015
###

cmake_minimum_required(VERSION 2.8)
project(RhAL)

set (SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

set (VENDORS_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendors")

# Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Enable compiler Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

# Include sources directory
include_directories(${SOURCES_DIRECTORY})

# Serial library
add_subdirectory(${VENDORS_DIRECTORY}/serial)
include_directories("${VENDORS_DIRECTORY}/serial/include")

set (SOURCES
    Bus/Bus.cpp
    Bus/SerialBus.cpp
    Protocol/Protocol.cpp
    )

foreach (SOURCE ${SOURCES})
    set (ALL_SOURCES ${ALL_SOURCES} 
        ${SOURCES_DIRECTORY}/${SOURCE})
endforeach ()

add_library (_rhal ${ALL_SOURCES})
target_link_libraries (_rhal serial)

add_executable (rhal ${SOURCES_DIRECTORY}/rhal.cpp)
target_link_libraries (rhal _rhal)
