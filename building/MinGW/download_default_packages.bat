@echo off
rem ************************************************************
rem  Download TOL Default Packages to the Runtime folder.
rem  /!\ Not necessary any more. TOL CMake already do that.
rem ************************************************************
setlocal enabledelayedexpansion

set BUILD_DIR=%~dp0
set BUILD_DIR=%BUILD_DIR:~0,-1%

if !PROCESSOR_ARCHITECTURE!==x86 (
  if "!platform!"=="" (
    set platform=32
  )
  if not "!platform!"=="32" (
    echo.^(^^!^) Current system cannot compile for !platform!-bit.
    exit /b
  )
  set platform=32
) else (
  if "!platform!"=="" (
    set /p platform="platform (32/64)? "
  )
  if not "!platform!"=="32" if not "!platform!"=="64" (
    echo.^(^^!^) Platform word size should be 32 or 64.
    exit /b
  )
)

if "!platform!"=="32" (
  set tol_platform=Win32GNU
) else (
  set tol_platform=Win64GNU
)

echo.Download default packages for !tol_platform!
echo.

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

set mode=release
set TOL_RUNTIME=!USERPROFILE!\CMake_Build\tol\!mode!-!platform!\Runtime_Base
set dest=!TOL_RUNTIME!\bin\stdlib\DefaultPackages
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
