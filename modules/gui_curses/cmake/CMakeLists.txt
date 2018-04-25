cmake_minimum_required(VERSION 3.5)
project(gui_curses)


  include_directories(
  ../include
  ../../network/include
  ../../monitor/include
  ../../instrumentation/include
  ../../signal/include
  )

  if (UNIX)
    FILE(GLOB huh
    ../src/*.cpp
    ../../network/src/*.cpp
    ../../monitor/src/*.cpp
    ../../instrumentation/src/Util.cpp
    ../../signal/src/*.cpp
    )	
    find_package(Threads)
    add_definitions("-std=c++14")
    add_definitions(-DCONNECTOR_UNIX)

    add_executable(roaster ${huh})
   target_link_libraries(roaster curses)
   target_link_libraries(roaster ${CMAKE_THREAD_LIBS_INIT})
endif ()
