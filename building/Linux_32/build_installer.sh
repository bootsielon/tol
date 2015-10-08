#!/bin/bash
TRUNK_DIR=$(dirname $0)/../..
cd $TRUNK_DIR
CMAKE_DIR=building/Linux_32/installer

echo -n "mkdir? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  if [ -d "$CMAKE_DIR" ]; then 
    rm -r "$CMAKE_DIR"
  fi
  mkdir -p "$CMAKE_DIR"
fi

cd $CMAKE_DIR

echo -n "cmake? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  cmake -DTOL_RUNTIME_DIR=/usr/local/tol3.2-release -DCMAKE_BUILD_TYPE=Release ../../..
fi

echo -n "cpack -G DEB? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  cpack -G DEB
fi

echo -n "cpack -G RPM? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  cpack -G RPM
fi

echo -n "press enter to exit "
read answer











