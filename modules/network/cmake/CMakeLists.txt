cmake_minimum_required(VERSION 3.5)
project(roast)

include_directories(../include ../../instrumentation/include)
if (UNIX)
   FILE(GLOB huh ../src/ConnectorUnix.cpp )
  add_definitions("-std=c++14")
  add_definitions("-D=CONNECTOR_UNIX")
endif (UNIX)

if (WIN32)
   FILE(GLOB huh ../src/ConnectorWindows.cpp ../../instrumentation/src/Util.cpp)
endif (WIN32)


add_library(dummy OBJECT   ${huh})
