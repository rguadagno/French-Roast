#!/bin/bash

# Sample script to build Rench Roast

set -e

if [ -z "$FR_PATH" ] ; then
 DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
 export FR_BASE=$DIR
fi


if [ -n ${JAVA_INCLUDE_DIR} ]; then 
 export JAVA_INCLUDE_DIR=/usr/lib/jvm/default-java/include
fi

if [ -n ${JAVA_INCLUDE_DIR2=} ]; then
 export JAVA_INCLUDE_DIR2=${JAVA_INCLUDE_DIR}/linux
fi

if [ -n ${BOOST_ROOT} ]; then
 export BOOST_ROOT=/usr/include/boost
fi

if [ -n ${OPCODE_FILE} ]; then
 export OPCODE_FILE=${FR_BASE}/modules/instrumentation/config/opcodes.txt
fi




# Make one directory using cmake. 
function makeit()
{
  if [ -d "$1" ] ; then
      echo "making $1"
      rm -f "$1/CMakeCache.txt"
      cd "$1" && cmake . && make
  fi
}

cd $FR_BASE

# Make all cmake projects.
for d in modules/*; do
 makeit $d/cmake
 makeit $d/tests/cmake
done

# Make gui 
cd $FR_BASE/modules/gui/profile && qmake && make
cd $FR_BASE

exit 0
