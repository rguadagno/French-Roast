cmake_minimum_required(VERSION 3.5)
project(agent_tests)

set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework.")
endif()



include_directories(
${CATCH_LOCATION}
../include
../../include
../../../network/include
)

FILE(GLOB src_files
../src/*.cpp
)

if (UNIX)
   add_definitions("-std=c++11")
endif ()

add_executable(runner ${src_files})

if (WIN32)
   target_link_libraries(runner  ws2_32.lib)
endif()      
