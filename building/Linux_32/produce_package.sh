#!/bin/bash

if [ "$1" = "" ]; then
  echo -n "package? "
  read package
else
  package=$1
fi

PACKAGE_DIR=$(dirname $0)/../../../OfficialTolArchiveNetwork/$package
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
      svn commit "$PACKAGE_DIR/CppTools/Linux_x86_32" --message "Archivos .so del paquete $package recompilados para Linux32"
    fi

  else
    echo "El paquete no tiene parte binaria"
  fi
else
  echo "No se encuentra el paquete"
  echo -n "press enter to exit "
  read answer
  exit
fi

echo -n "produce? (y/n) "
read answer
if [ "$answer" = "y" ]; then
  tolsh -c"Text Package=\"$package\"" ../produce_package.tol
fi

echo -n "press enter to continue "
read answer











