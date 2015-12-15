#!/bin/bash
#*********************************************************
# La llamada al script recibe dos argumentos:
#   ./build_contrib.sh [|<contrib_name>] [|release|debug]
#*********************************************************
# Durante el proceso se pregunta por cada una de las etapas.

BUILD_DIR=$PWD/$(dirname $0)

if [ "$1" = "" ]; then
  echo -n "contrib? "
  read contrib
else
  contrib=$1
fi

CONTRIB_DIR=$BUILD_DIR/../../../contribs/$contrib

if ! [ -d "$CONTRIB_DIR" ]; then 
  echo "(!) Cannot find the contrib $contrib."
  exit
fi

if [ "$2" = "" ]; then
  mode=release
else
  mode=$2
fi
if !([ "$mode" = "release" ] || [ "$mode" = "debug" ]); then
  echo "(!) Second argument should be: release, debug or should be avoided."
  exit
fi
suffix=-$mode
if [ "$mode" = "release" ]; then
  suffix=
fi

CMAKE_DIR=~/CMake_Build/contrib/$contrib/$mode

echo "----------------------------------------------------"
echo " Building contrib $contrib - $mode"
echo "----------------------------------------------------"

if [ -d "$CMAKE_DIR" ]; then
  echo "/!\ There is already a built folder of $contrib."
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
  #cmake para ZipArchive
  cmake $CONTRIB_DIR -DCMAKE_BUILD_TYPE=$mode -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=/usr/local/$contrib$suffix
fi

echo -n "make? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  make
fi

echo -n "install? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  sudo make install
fi
