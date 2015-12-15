#!/bin/bash
#************************************************************
# La llamada al script recibe un argumento:
#   ./produce_package.sh [|<package_name>]
#************************************************************
# Durante el proceso se pregunta por cada una de las etapas.
#************************************************************

BUILD_DIR=$PWD/$(dirname $0)

if [ "$1" = "" ]; then
  echo -n "package? "
  read package
else
  package=$1
fi

PACKAGE_DIR=$BUILD_DIR/../../../OfficialTolArchiveNetwork/$package

if [ -d "$PACKAGE_DIR" ]; then 
  if [ -f "$PACKAGE_DIR/CMakeLists.txt" ]; then
    echo -n "build? (y/n) "
    read answer
    if [ "$answer" = "y" ]; then
      ./build_package.sh $package
    fi
    echo -n "svn commit? (y/n) "
    read answer
    if [ "$answer" = "y" ]; then
      MACHINE_TYPE=`uname -m`
      if [ ${MACHINE_TYPE} = 'x86_64' ]; then
        svn commit "$PACKAGE_DIR/CppTools/Linux_64_GNU" --message "Archivos .so del paquete $package recompilados para Linux 64"
      else
        svn commit "$PACKAGE_DIR/CppTools/Linux_x86_32" --message "Archivos .so del paquete $package recompilados para Linux 32"
      fi
    fi
  else
    echo "/!\ The package $package has not a CMakeLists.txt."
  fi
else
  echo "(!) Cannot find the package $package."
  exit
fi

echo -n "produce? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  tolsh -c"Text Package=\"$package\"" ../produce_package.tol
fi
