cmake_minimum_required(VERSION 3.5)
project(agent_tests)

set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework.")
endif()



include_directories(
${CATCH_LOCATION}
../../include
../../../instrumentation/include
../../../network/include
)

FILE(GLOB src_files
../src/*.cpp
../../src/MonitorUtil.cpp
../../src/Descriptor.cpp
../../src/Monitor.cpp
../../src/StackFrame.cpp
../../src/StackTrace.cpp
../../src/MethodStats.cpp
../../src/ClassDetail.cpp
../../src/JammedReport.cpp
../../../instrumentation/src/Util.cpp
../../../network/src/Connector.cpp
../../../network/src/FRSocket.cpp
)

if (UNIX)
   add_definitions("-std=c++11")
   add_definitions(-DCONNECTOR_UNIX)
   find_package(Threads)
endif ()

add_executable(runner ${src_files})
if (UNIX)
   target_link_libraries(runner ${CMAKE_THREAD_LIBS_INIT})
endif ()

if (WIN32)
   target_link_libraries(runner  ws2_32.lib)
endif()      
