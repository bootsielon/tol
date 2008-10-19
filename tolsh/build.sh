#!/bin/sh

sys=`uname`
case $sys in
Linux)
  echo Building for Linux ...
  cp Linux/tclkit-linux-x86.upx.bin Linux/tclkit
  Linux/tclkit sdx.kit wrap tolsh -runtime Linux/tclkit-linux-x86.upx.bin
  mv tolsh Linux/.
  rm Linux/tclkit
  echo tolsh was generated on Linux/
  ;;
esac

