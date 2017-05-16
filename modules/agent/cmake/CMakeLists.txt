cmake_minimum_required(VERSION 3.5)
project(monitor)

set(JAVA_INCLUDE_DIR "$ENV{JAVA_INCLUDE_DIR}")
set(JAVA_INCLUDE_DIR2 "$ENV{JAVA_INCLUDE_DIR2}")
if(JAVA_INCLUDE_DIR AND JAVA_INCLUDE_DIR2)
  include(GenerateExportHeader)
  include_directories(../include ../../instrumentation/include ../../network/include ${JAVA_INCLUDE_DIR}  ${JAVA_INCLUDE_DIR2})
  if (UNIX)
    FILE(GLOB huh ../src/*.cpp ../../instrumentation/src/*.cpp ../../network/src/ConnectorUnix.cpp ../../network/src/Connector.cpp)	
    find_package(Threads)
    add_definitions("-std=c++14")
    add_definitions(-DCONNECTOR_UNIX)
#   target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})   

  endif ()

  if (WIN32)
    FILE(GLOB huh ../src/*.cpp ../../instrumentation/src/*.cpp ../../network/src/ConnectorWindows.cpp ../../network/src/Connector.cpp)	
  endif ()

  add_library(agent SHARED ${huh} )
  GENERATE_EXPORT_HEADER(agent
  BASE_NAME agent
  EXPORT_MACRO_NAME agent_EXPORT
  EXPORT_FILE_NAME agent_Export.h
  STATIC_DEFINE monitor_BUILT_AS_STATIC)
  if (WIN32)
      target_link_libraries(agent  ws2_32.lib)
  endif()      
else()
  message(FATAL_ERROR "env variables JAVA_INCLUDE_DIR and JAVA_INCLUDE_DIR2 must be defined, the dir where jvmti.h and jni.h are located and also jni_md.h")
endif()
