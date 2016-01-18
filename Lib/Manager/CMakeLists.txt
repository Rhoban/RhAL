###
### TODO manager tests
###

cmake_minimum_required(VERSION 2.8)
project(RhAL)

# Include sources directory
set(SOURCES_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SOURCES_DIRECTORY})
include_directories("${SOURCES_DIRECTORY}/Vendors/json/src")

# Enable C++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Enable compiler Warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall")

#Manager files source
set(SOURCES
)

#Manager files source
set(TESTS
    testRegister
    testJSON
    testParameters
    testDevice
    testBaseManager
    testManager
)

#Build Manager as a library
#add_library(LibRhAL ${SOURCES})

#Build all tests binaries
foreach(TEST ${TESTS})
    add_executable(${TEST} "Tests/${TEST}.cpp")
    #target_link_libraries(${TEST} LibRhAL)
endforeach()

