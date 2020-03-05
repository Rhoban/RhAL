###
### Rhoban Hardware Abstraction Layer library
### Rhoban 2015
###

cmake_minimum_required(VERSION 2.8)
project(RhAL)

find_package(OpenCV 3.2.0 REQUIRED)

find_package(catkin REQUIRED COMPONENTS
  RhIOServer
  jsoncpp
  serial
  tclap
  eigen
  rhoban_utils
  )

catkin_package(
  INCLUDE_DIRS Lib
  LIBRARIES RhAL
  CATKIN_DEPENDS RhIOServer jsoncpp serial tclap eigen rhoban_utils
)

set(LIB_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Lib")
set(BIN_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Bin")
set(TESTS_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Tests")
set(EXAMPLES_SOURCES_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/Examples")

# Build option
option(BUILD_RHAL_TESTS "Build RhAL Tests" OFF)
option(BUILD_RHAL_EXAMPLES "Build RhAL Examples" OFF)
option(BUILD_RHAL_BINARY "Build RhAL Utility Binary" OFF)

# Enable C++17
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -std=c++17")
# Enable compiler Warning
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -W -Wall")

# Include sources directory
include_directories(${LIB_SOURCES_DIRECTORY} ${catkin_INCLUDE_DIRS})

# Lib source files
set(LIB_SOURCES
    Bus/Bus.cpp
    Bus/SerialBus.cpp
    Protocol/Protocol.cpp
    Protocol/DynamixelV1.cpp
    Protocol/FakeProtocol.cpp
    Protocol/ProtocolFactory.cpp
    timestamp.cpp
    Manager/Statistics.cpp
    Manager/RegistersList.cpp
    Manager/ParametersList.cpp
    Manager/Register.cpp
    Manager/Parameter.cpp
    Manager/Device.cpp
    Manager/CallManager.cpp
    Manager/ConvertionUtils.cpp
    Manager/Aggregation.cpp
    Manager/BaseManager.cpp
    RhAL.cpp
    Devices/ExampleDevice1.cpp
    Devices/ExampleDevice2.cpp
    Devices/IMU.cpp
    Devices/Pins.cpp
    Devices/PressureSensor.cpp
    Devices/DXL.cpp
    Devices/RX.cpp
    Devices/AX12.cpp
    Devices/AX18.cpp
    Devices/RX24.cpp
    Devices/RX28.cpp
    Devices/RX64.cpp
    Devices/Dynaban64.cpp
    Devices/MX.cpp
    Devices/MX12.cpp
    Devices/MX28.cpp
    Devices/MX64.cpp
    Devices/MX106.cpp
    Devices/GY85.cpp
    Devices/BNO055.cpp
    Devices/AHRS/Filter.cpp
    Bindings/RhIOBinding.cpp
)

# Tests source files
set(TESTS_SOURCES
    testRegister
    testJSON
    testParameters
    testDevice
    testBaseManager
    testManager
    testReadWrite
    testSync
    #test1
    testDynaban
    testBinding
)

# Examples source files
set(EXAMPLES_SOURCES
    example1
    example2
    example3
    example4
    example5
)

# Link Library List
set (LIBRARIES
    pthread
    ${catkin_LIBRARIES}
)

# Prefix with lib directory path
foreach(SOURCE ${LIB_SOURCES})
    set (PREFIXED_LIB_SOURCES
        ${PREFIXED_LIB_SOURCES}
        ${LIB_SOURCES_DIRECTORY}/${SOURCE}
    )
endforeach()

# Build the RhAL Library
add_library(RhAL SHARED ${PREFIXED_LIB_SOURCES})
target_link_libraries(RhAL ${LIBRARIES})

# Build all Tests binary
if (BUILD_RHAL_TESTS)
    foreach(TEST ${TESTS_SOURCES})
        add_executable(${TEST} "${TESTS_SOURCES_DIRECTORY}/${TEST}.cpp")
        target_link_libraries(${TEST} RhAL ${LIBRARIES})
    endforeach()
endif (BUILD_RHAL_TESTS)

# Build all Examples binary
if (BUILD_RHAL_EXAMPLES)
    foreach(EXAMPLE ${EXAMPLES_SOURCES})
        add_executable(${EXAMPLE} "${EXAMPLES_SOURCES_DIRECTORY}/${EXAMPLE}.cpp")
        target_link_libraries(${EXAMPLE} RhAL ${LIBRARIES})
    endforeach()
endif (BUILD_RHAL_EXAMPLES)

# Build the utility binary
if (BUILD_RHAL_BINARY)
    add_executable(rhal ${BIN_SOURCES_DIRECTORY}/rhal.cpp)
    target_link_libraries(rhal RhAL ${LIBRARIES} ${OpenCV_LIBS})
endif (BUILD_RHAL_BINARY)

