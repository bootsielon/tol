#!/bin/bash
#*********************************************************
# La llamada al script recibe dos argumentos:
#   ./build.sh [tol|toltcl|tolbase|rtol] [|release|debug]
#*********************************************************
# Durante el proceso se pregunta por cada una de las etapas.

BUILD_DIR=$PWD/$(dirname $0)

if [ "$1" = "" ]; then
  echo -n "project? "
  read project
else
  project=$1
fi
if !([ "$project" = "tol" ] || [ "$project" = "toltcl" ] || [ "$project" = "tolbase" ] || [ "$project" = "rtol" ]); then
  echo "(!) First argument should be: tol, toltcl, tolbase or rtol."
  echo -n "press enter to exit "
  read answer
  exit
fi

PROJECT_DIR=$BUILD_DIR/../../$project

if [ "$2" = "" ]; then
  mode=release
else
  mode=$2
fi
if !([ "$mode" = "release" ] || [ "$mode" = "debug" ]); then
  echo "(!) Second argument should be: release, debug or should be avoided."
  echo -n "press enter to exit "
  read answer  
  exit
fi
suffix=-$mode
if [ "$mode" = "release" ]; then
  suffix=
fi

CMAKE_DIR=~/CMake_Build/$project/$mode

echo "----------------------------------------------------"
echo " Building $project - $mode"
echo "----------------------------------------------------"

if [ -d "$CMAKE_DIR" ]; then
  echo "/!\ There is already a built folder of $project."
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
  if [ "$project" = "tol" ]; then 
    cmake $PROJECT_DIR/. -DCMAKE_BUILD_TYPE=$mode -DCLAPACK_NAME=lapacke -DZIPARCHIVE_ROOT_DIR=/usr/local/ZipArchive$suffix -DZIPARCHIVE_AS_STATIC=TRUE -DCMAKE_INSTALL_PREFIX=/usr/local/tol3.2$suffix
  else
    cmake $PROJECT_DIR -DCMAKE_BUILD_TYPE=$mode -DTOL_PREFIX_PATH=/usr/local/tol3.2$suffix -DCMAKE_INSTALL_PREFIX=/usr/local/tol3.2$suffix
  fi  
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

echo -n "press enter to exit "
read answer
