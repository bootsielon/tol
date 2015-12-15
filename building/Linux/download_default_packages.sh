#!/bin/bash
#************************************************************
# Download TOL Default Packages to the installation folder.
# /!\ Not necessary any more. TOL CMake already do that.
#************************************************************

MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} = 'x86_64' ]; then
  tol_platform=Linux32GNU
else
  tol_platform=Linux64GNU
fi

echo "Download default packages for $tol_platform"
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
sudo wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=TclCore%23$tol_platform"
# =======================================
 
ok=1

if ! [ -f StdLib.*.*.zip ]; then 
  ok=0
  ls $dest
  echo "/!\ Package StdLib cannot be downloaded."
fi
if ! [ -f TclCore#$tol_platform.*.*.zip ]; then 
  ok=0
  echo "/!\ Package TclCore cannot be downloaded for '$tol_platform'"
fi

if [ "$ok" = "1" ]; then
  echo "Default packages correctly downloaded."
  exit 0
fi

exit 2
