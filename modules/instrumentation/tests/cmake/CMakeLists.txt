cmake_minimum_required(VERSION 3.5)
project(hello)


set(CATCH_LOCATION "$ENV{CATCH_LOCATION}")
set(JAVA_CLASS_DIR "$ENV{JAVA_CLASS_DIR}")
set(OPCODE_FILE "$ENV{OPCODE_FILE}")

if(NOT JAVA_CLASS_DIR)
   message(FATAL_ERROR "env variable JAVA_CLASS_DIR must be defined, the dir where the class files are to test. run set_env")
endif()

if(NOT CATCH_LOCATION)
   message(FATAL_ERROR "env variable CATCH_LOCATION must be defined, the dir where the Catch test framework. run set_env")
endif()

if(NOT OPCODE_FILE)
   message(FATAL_ERROR "env variable OPCODE_FILE must be defined, full path to file with opcodes. run set_env")
endif()

include_directories(${CATCH_LOCATION}
../../../signal/include
../../../monitor/include
../../../network/include
../../include
../include       
)
  
FILE(GLOB huh
../src/Test_Main.cpp
../src/Test_Util.cpp
../src/Test.cpp
../src/Test_OpCode.cpp
../src/Test_Instruction.cpp
../src/Test_Method.cpp
../src/Test_Method_save_buffer.cpp  
../src/testing_utils.cpp
../../src/*.cpp
../../../monitor/src/*.cpp
../../../network/src/*.cpp
../../../signal/src/*.cpp
)


add_definitions("-std=c++11")
add_definitions(-DCONNECTOR_UNIX)

add_executable(runner  ${huh})




