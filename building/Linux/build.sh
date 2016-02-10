#!/bin/bash
#************************************************************
# La llamada al script recibe dos argumentos:
#   ./build.sh [tol|toltcl|tolbase|rtol] [|Release|Debug]
#************************************************************
# Durante el proceso se pregunta por cada una de las etapas.
#************************************************************

BUILD_DIR=$PWD/$(dirname $0)
source $BUILD_DIR/current_version.sh

if [ "$1" = "" ]; then
  echo -n "project? "
  read project
else
  project=$1
fi
if !([ "$project" = "tol" ] || [ "$project" = "toltcl" ] || [ "$project" = "tolbase" ] || [ "$project" = "rtol" ]); then
  echo "(!) First argument should be: tol, toltcl, tolbase or rtol."
  exit
fi

PROJECT_DIR=$BUILD_DIR/../../$project
if ! [ -d "$PROJECT_DIR" ]; then 
  echo "(!) Cannot find folder: $PROJECT_DIR"
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
echo " Building $project - $mode"
echo "----------------------------------------------------"

CMAKE_DIR=~/CMake_Build/$project/$mode
if [ -d "$CMAKE_DIR" ]; then
  echo "/!\ There is already a built folder of $project."
  echo -n "mkdir? (y/n) "
  read answer
  if [ "$answer" = "y" ]; then
    rm -r -f "$CMAKE_DIR"
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
    cmake $PROJECT_DIR -DCMAKE_BUILD_TYPE=$mode -DCLAPACK_NAME=lapacke -DZIPARCHIVE_ROOT_DIR=/usr/local/ZipArchive$suffix -DZIPARCHIVE_AS_STATIC=TRUE -DCMAKE_INSTALL_PREFIX=/usr/local/tol$version$suffix
  else
    cmake $PROJECT_DIR -DCMAKE_BUILD_TYPE=$mode -DTOL_PREFIX_PATH=/usr/local/tol$version$suffix -DCMAKE_INSTALL_PREFIX=/usr/local/tol$version$suffix
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
