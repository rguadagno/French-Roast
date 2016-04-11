cmake_minimum_required(VERSION 3.5)
project(roast)

include_directories(../include ../../instrumentation/include)
FILE(GLOB huh ../src/*.cpp ../../instrumentation/src/Util.cpp)

add_library(dummy OBJECT   ${huh})
