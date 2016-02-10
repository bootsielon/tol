#!/bin/bash
#************************************************************
# La llamada al script recibe dos argumentos:
#   ./build_package.sh [|<package_name>] [|Release|Debug]
#************************************************************
# Durante el proceso se pregunta por cada una de las etapas.
#************************************************************

BUILD_DIR=$PWD/$(dirname $0)
source $BUILD_DIR/current_version.sh

if [ "$1" = "" ]; then
  echo -n "package? "
  read package
else
  package=$1
fi

PACKAGE_DIR=$BUILD_DIR/../../../OfficialTolArchiveNetwork/$package
if ! [ -f "$PACKAGE_DIR/CMakeLists.txt" ]; then 
  if [ -d "$PACKAGE_DIR" ]; then 
    echo "(!) Cannot find the CMakeLists.txt"
  else
    echo "(!) Cannot find the package $package."
  fi
  exit
fi

if [ "$2" = "" ]; then
  mode=Release
else
  mode=$2
fi
if !([ "$mode" = "Release" ] || [ "$mode" = "Debug" ]); then
  echo "(!) Second argument should be: Release, Debug or should be avoided."
  exit
fi
suffix=-$mode
if [ "$mode" = "Release" ]; then
  suffix=
fi

echo "----------------------------------------------------"
echo " Building package $package - $mode"
echo "----------------------------------------------------"

CMAKE_DIR=~/CMake_Build/packages/$package/$mode
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
if [ "$answer" = "y" ]; then
  cmake $PACKAGE_DIR -DTOL_PREFIX_PATH=/usr/local/tol$version$suffix -DCMAKE_BUILD_TYPE=$mode
fi

echo -n "make? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  make
fi
