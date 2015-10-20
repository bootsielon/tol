#!/bin/bash

if [ "$1" = "" ]; then
  echo -n "package? "
  read package
else
  package=$1
fi

PACKAGE_DIR=$(dirname $0)/../../../OfficialTolArchiveNetwork/$package
if [ -f "$PACKAGE_DIR/CMakeLists.txt" ]; then 
  cd $PACKAGE_DIR
  CMAKE_DIR=BUILD_CMAKE/m32/release
else
  if [ -d "$PACKAGE_DIR" ]; then 
    echo "No se encuentra el CMakeLists.txt"
  else
    echo "No se encuentra el paquete"
  fi
  echo -n "press enter to exit "
  read answer
  exit
fi

echo -n "mkdir? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  if [ -d "$CMAKE_DIR" ]; then 
    rm -r "$CMAKE_DIR"
  fi
  mkdir -p "$CMAKE_DIR"
fi

cd $CMAKE_DIR

echo -n "cmake? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  cmake ../../.. -DTOL_PREFIX_PATH=/usr/local/tol3.2-release -DCMAKE_BUILD_TYPE=Release
fi

echo -n "make? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  make
fi

echo -n "press enter to continue "
read answer











