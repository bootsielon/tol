#!/bin/bash
#************************************************************
# La llamada al script no recibe argumentos:
#   ./build_instaler.sh
#************************************************************
# Durante el proceso se pregunta por cada una de las etapas.
#************************************************************

BUILD_DIR=$PWD/$(dirname $0)
source $BUILD_DIR/current_version.sh

INSTALLER_DIR=$BUILD_DIR/../../installer
if ! [ -d "$INSTALLER_DIR" ]; then 
  echo "(!) Cannot find folder: $INSTALLER_DIR"
  exit
fi

mode=Release

echo "----------------------------------------------------"
echo " Building TOL installer "
echo "----------------------------------------------------"

CMAKE_DIR=~/CMake_Build/installer/$mode
if [ -d "$CMAKE_DIR" ]; then
  echo "/!\ There is already a built folder of the installer."
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
  cmake $INSTALLER_DIR -DTOL_RUNTIME_DIR=/usr/local/tol$version -DCMAKE_BUILD_TYPE=$mode
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
