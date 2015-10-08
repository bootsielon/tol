#!/bin/bash

echo -n "install? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo dpkg -i installer/tol-3.2.deb
fi

echo -n "uninstall? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo dpkg -r tol
fi

echo -n "press enter to exit "
read answer











