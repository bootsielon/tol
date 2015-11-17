@echo off

set platform=Win64GNU
echo.Download default packages for %platform%
echo.

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

set dest=%tolp_root%\trunk\tol\CodeLite64\Release\Runtime_Base\bin\stdlib\DefaultPackages
if not exist "%dest%" (
  echo./!\ Cannot find '%dest%'
  exit /b
)

rem Deletes previous packages if they exist.
del "%dest%\*.zip*"

rem == WGET ===============================
set init_dir=%CD%
cd %dest%

rem Adds wget path to variable path.
rem wget version should have option --content-disposition.
set path=C:\Program Files (x86)\GnuWin32\bin;%path%

echo.### Downloading StdLib ###
wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=StdLib" 

echo.### Downloading TclCore ###
wget --content-disposition "http://packages.tol-project.org/OfficialTolArchiveNetwork/repository.php?action=download&format=attachment&tol_package_version=4&package=TclCore%%23%platform%"

cd %init_dir%
rem =======================================

set ok=1

if not exist "%dest%\StdLib.*.*.zip" (
  set ok=0
  echo./!\ Package StdLib cannot be downloaded.
)
if not exist "%dest%\TclCore#%platform%.*.*.zip" (
  set ok=0
  echo./!\ Package TclCore cannot be downloaded for '%platform%'.
)

if "%ok%"=="1" (
  echo.Default packages correctly downloaded.
)

exit /b
