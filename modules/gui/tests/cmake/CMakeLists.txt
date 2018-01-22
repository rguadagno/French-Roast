cmake_minimum_required(VERSION 3.5)
project(gui_tests)

set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework.")
endif()



include_directories(
${CATCH_LOCATION}
../include
../../include
../../../monitor/include
../../../monitor/tests/include
../../../instrumentation/include
../../../network/include
/opt/Qt5/5.5/gcc/include/QtWidgets
/opt/Qt5/5.5/gcc/include
)

FILE(GLOB src_files
../src/Main.cpp
../../src/DetailViewerModel.cpp
../../src/JammedViewerModel.cpp
../../src/QUtil.cpp
../../src/CodeFont.cpp
../../../monitor/src/StackReport.cpp
../../../monitor/src/StackTrace.cpp
../../../monitor/src/StackFrame.cpp
../../../monitor/src/Descriptor.cpp
../../../monitor/src/Monitor.cpp
../../../monitor/src/MethodStats.cpp
../../../monitor/src/SignalParams.cpp
../../../monitor/src/SignalMarkers.cpp
../../../monitor/src/JammedReport.cpp
../../../network/src/FRSocket.cpp
../../../instrumentation/src/Util.cpp
../../../monitor/tests/src/helper.cpp
../src/test_DetailViewerModel.cpp
../src/test_JammedViewerModel.cpp
)

if (UNIX)
   add_definitions("-std=c++17")
   add_definitions("-fPIC")     
   add_definitions(-DCONNECTOR_UNIX)
   find_package(Threads)
endif ()

find_package(Qt5Widgets)

add_executable(runner ${src_files})
if (UNIX)
   target_link_libraries(runner ${CMAKE_THREAD_LIBS_INIT})
endif ()
 target_link_libraries(runner  Qt5::Widgets)
if (WIN32)
   target_link_libraries(runner  ws2_32.lib)
endif()      
