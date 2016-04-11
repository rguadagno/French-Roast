cmake_minimum_required(VERSION 3.5)
project(hello)

include_directories(../include)
FILE(GLOB huh ../src/*.cpp)

#add_executable(hello  ${huh})
add_library(dummy OBJECT   ${huh})
