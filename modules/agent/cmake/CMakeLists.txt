cmake_minimum_required(VERSION 3.5)
project(monitor)

include(GenerateExportHeader)
include_directories(../include ../../instrumentation/include ../../network/include)

FILE(GLOB huh ../src/*.cpp ../../instrumentation/src/*.cpp ../../network/src/*.cpp)


add_library(agent SHARED ${huh} )
target_link_libraries(agent  ws2_32.lib)

GENERATE_EXPORT_HEADER(agent
BASE_NAME agent
EXPORT_MACRO_NAME agent_EXPORT
EXPORT_FILE_NAME agent_Export.h
STATIC_DEFINE monitor_BUILT_AS_STATIC)
