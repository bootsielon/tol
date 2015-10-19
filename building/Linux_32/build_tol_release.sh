#!/bin/bash
TOL_DIR=$(dirname $0)/../../tol
cd $TOL_DIR
CMAKE_DIR=BUILD_CMAKE/m32/release

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
  cmake ../../.. -DCMAKE_BUILD_TYPE=Release -DCLAPACK_NAME=lapacke -DZIPARCHIVE_ROOT_DIR=/usr/local/ZipArchive -DZIPARCHIVE_AS_STATIC=TRUE -DCMAKE_INSTALL_PREFIX=/usr/local/tol3.2-release
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











