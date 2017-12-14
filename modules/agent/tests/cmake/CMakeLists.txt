cmake_minimum_required(VERSION 3.5)
project(agent_tests)

set(JAVA_INCLUDE_DIR "$ENV{JAVA_INCLUDE_DIR}")
set(JAVA_INCLUDE_DIR2 "$ENV{JAVA_INCLUDE_DIR2}")
set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework.")
endif()

if(NOT JAVA_INCLUDE_DIR)
   message(FATAL_ERROR "env variable JAVA_INCLUDE_DIR must be defined, the dir where the jvmti.h exists.")
endif()



include_directories(
${CATCH_LOCATION}
../include
../../include
../../../network/include
../../../signal/include
../../../instrumentation/include
../../../instrumentation/tests/include
../../../monitor/include
${JAVA_INCLUDE_DIR}
${JAVA_INCLUDE_DIR2}
)

FILE(GLOB src_files
../src/Main.cpp
../src/Test_Agent.cpp
../src/Test_AgentSignalReporting.cpp
../src/test_reporter.cpp
../src/MockJVMTI.cpp
../src/MockJNI.cpp
../../src/AgentHooks.cpp
../../src/AgentUtil.cpp
../../src/AgentSignalReporting.cpp
../../src/Agent.cpp
../../src/Config.cpp
../../src/ServerTransport.cpp
../../src/Reporter.cpp
../../../network/src/Connector.cpp
../../../network/src/FRSocket.cpp
../../..//instrumentation/src/*.cpp
../../..//instrumentation/tests/src/testing_utils.cpp
../../../monitor/src/ClassDetail.cpp
../../../signal/src/fr_signals.cpp
../../../signal/src/SignalValidator.cpp
)

if (UNIX)
   add_definitions("-std=c++14")
   add_definitions(-DCONNECTOR_UNIX)
   find_package(Threads)
endif ()

add_executable(runner ${src_files})

if (WIN32)
   target_link_libraries(runner  ws2_32.lib)
endif()      
if (UNIX)
   target_link_libraries(runner ${CMAKE_THREAD_LIBS_INIT})
endif ()
