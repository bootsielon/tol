#!/bin/bash
#*********************************************************
# La llamada al script no recibe argumentos:
#   ./build_instaler.sh
#*********************************************************
# Durante el proceso se pregunta por cada una de las etapas.

BUILD_DIR=$PWD/$(dirname $0)

TRUNK_DIR=$BUILD_DIR/../..

CMAKE_DIR=~/CMake_Build/installer

echo "----------------------------------------------------"
echo " Building TOL installer "
echo "----------------------------------------------------"

if [ -d "$CMAKE_DIR" ]; then
  echo "/!\ There is already a built folder of $package."
  echo -n "mkdir? (y/n) "
  read answer
  if [ "$answer" = "y" ]; then
    rm -r "$CMAKE_DIR"
    mkdir -p "$CMAKE_DIR"
  fi
else
  mkdir -p "$CMAKE_DIR"
fi

cd $CMAKE_DIR

echo -n "cmake? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  cmake $TRUNK_DIR -DTOL_RUNTIME_DIR=/usr/local/tol3.2 -DCMAKE_BUILD_TYPE=release
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
