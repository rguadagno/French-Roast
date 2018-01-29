cmake_minimum_required(VERSION 3.5)
project(monitor)

set(JAVA_INCLUDE_DIR "$ENV{JAVA_INCLUDE_DIR}")
set(JAVA_INCLUDE_DIR2 "$ENV{JAVA_INCLUDE_DIR2}")
set(BOOST_ROOT "$ENV{BOOST_ROOT}")
if(NOT BOOST_ROOT)
  message(FATAL_ERROR "env variables BOOST_ROOT must be defined.")
endif()
if(JAVA_INCLUDE_DIR AND JAVA_INCLUDE_DIR2)
  include(GenerateExportHeader)
  include_directories(
  ../include
  ../../instrumentation/include
  ../../network/include
  ../../monitor/include
  ../../signal/include
  ${BOOST_ROOT}
  ${JAVA_INCLUDE_DIR}
  ${JAVA_INCLUDE_DIR2})
    FILE(GLOB huh
    ../src/*.cpp
    ../../instrumentation/src/*.cpp
    ../../monitor/src/ClassDetail.cpp
    ../../monitor/src/Descriptor.cpp
    ../../monitor/src/Monitor.cpp
    ../../monitor/src/StackTrace.cpp
    ../../monitor/src/StackFrame.cpp
    ../../monitor/src/StackReport.cpp
    ../../monitor/src/SignalParams.cpp
    ../../monitor/src/SignalMarkers.cpp
    ../../monitor/src/Signal.cpp            
    ../../monitor/src/JammedReport.cpp    
    ../../network/src/FRSocket.cpp
    ../../signal/src/fr_signals.cpp
    ../../signal/src/SignalValidator.cpp
    ../../monitor/src/TimerReport.cpp
    )	

if (UNIX)
    find_package(Threads)
    add_definitions("-std=c++17")
    add_definitions(-DCONNECTOR_UNIX)
#   target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})   

  endif ()

  if (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2")
#    add_definitions(${CMAKE_CXX_FLAGS})
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
