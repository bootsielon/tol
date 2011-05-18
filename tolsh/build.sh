#!/bin/sh

tclkit=tclkitsh-8.5.8
version=2.0.2
sys=`uname`
case $sys in
Linux)
  echo Building for Linux ...
  cp Linux/$tclkit Linux/tclkit
  Linux/tclkit sdx.kit wrap tolsh -runtime Linux/$tclkit
  mv tolsh Linux/tolsh$version
  rm Linux/tclkit
  echo tolsh$version was generated on Linux/
  ;;
esac

