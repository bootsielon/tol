@echo off
rem ************************************************************
rem  Download TOL Default Packages to the Runtime folder.
rem  /!\ Not necessary any more. TOL CMake already do that.
rem ************************************************************
setlocal enabledelayedexpansion

set BUILD_DIR=%~dp0
set BUILD_DIR=%BUILD_DIR:~0,-1%

set tol_platform=Win32MSVC

echo.Download default packages for !tol_platform!
echo.

set dest=%CD%\..\ActiveTOL\bin\stdlib\DefaultPackages
if not exist "!dest!" (
  echo.^(^^!^) Cannot find '!dest!'
  exit /b
)

rem Deletes previous packages if they exist.
del "!dest!\*.zip*"

rem == WGET ===============================
cd !dest!

rem Adds wget path to variable path.
rem wget version should have option --content-disposition.
set path=C:\Program Files (x86)\GnuWin32\bin;C:\Program Files\GnuWin32\bin;%path%

echo.### Downloading StdLib ###
wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=StdLib" 

echo.### Downloading TclCore ###
wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=TclCore%%23!tol_platform!"

cd !BUILD_DIR!
rem =======================================

set ok=1

if not exist "!dest!\StdLib.*.*.zip" (
  set ok=0
  echo./!\ Package StdLib cannot be downloaded.
)
if not exist "!dest!\TclCore#!tol_platform!.*.*.zip" (
  set ok=0
  echo./!\ Package TclCore cannot be downloaded for '!tol_platform!'.
)

if "!ok!"=="1" (
  echo.Default packages correctly downloaded.
)

exit /b
