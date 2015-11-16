#!/bin/bash

echo -n "install? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo dpkg -i installer/tol-3.2.deb
fi

echo -n "install dependences? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo apt-get -f install
fi

echo -n "uninstall? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo dpkg -r tol
fi

echo -n "uninstall dependences? (y/n) "
read answer
if [ "$answer" = "y" ]
then
  sudo apt-get autoremove
fi

echo -n "press enter to exit "
read answer











