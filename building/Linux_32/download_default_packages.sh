#!/bin/bash

platform=Linux32GNU
echo "Download default packages for $platform"
echo 

dest=/usr/local/tol3.2-release/bin/stdlib/DefaultPackages
if ! [ -d "$dest" ]; then 
  echo "/!\ Cannot find '$dest'"
  exit 1
fi

# == WGET ===============================
# wget version should have option --content-disposition.
cd $dest

# Deletes previous packages if they exist.
sudo rm -f *.zip*

echo "### Downloading StdLib ###"
sudo wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=StdLib" 

echo "### Downloading TclCore ###"
sudo wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=TclCore%23$platform"
# =======================================
 
ok=1

if ! [ -f StdLib.*.*.zip ]; then 
  ok=0
  ls $dest
  echo "/!\ Package StdLib cannot be downloaded."
fi
if ! [ -f TclCore#$platform.*.*.zip ]; then 
  ok=0
  echo "/!\ Package TclCore cannot be downloaded for '$platform'"
fi

if [ "$ok" = "1" ]; then
  echo "Default packages correctly downloaded."
  exit 0
fi

exit 2
