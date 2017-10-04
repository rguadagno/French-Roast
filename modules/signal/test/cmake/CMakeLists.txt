cmake_minimum_required(VERSION 3.5)
project(frenchroast)


set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework.")
endif()

include_directories(
${CATCH_LOCATION}
../../include
../include
../../../instrumentation/include
)
  
FILE(GLOB src_list
../src/*.cpp
../../src/*.cpp
../../../instrumentation/src/Util.cpp
)


add_definitions("-std=c++11")
add_executable(runner  ${src_list})
