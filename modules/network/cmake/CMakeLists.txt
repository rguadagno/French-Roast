cmake_minimum_required(VERSION 3.5)
project(roast)

include_directories(../include)
FILE(GLOB huh ../src/*.cpp)

#add_executable(roast  ${huh})
add_library(dummy OBJECT   ${huh})
