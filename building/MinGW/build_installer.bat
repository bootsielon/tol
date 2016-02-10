@echo off
rem ************************************************************
rem  La llamada al script no recibe argumentos:
rem    build_installer
rem ************************************************************
rem  Durante el proceso se pregunta por cada una de las etapas.
rem ************************************************************
setlocal enabledelayedexpansion

set BUILD_DIR=%~dp0
set BUILD_DIR=%BUILD_DIR:~0,-1%

call !BUILD_DIR!\cmake_prepare.bat

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

set INSTALL_DIR=!BUILD_DIR!\..\..\installer
if not exist "!INSTALL_DIR!" (
  echo.^(^^!^) Cannot find folder: !INSTALL_DIR!
  exit /b
)

set mode=Release

echo.
set /p answer="include tolbase? (y/n) "
if "!answer!"=="y" (
  set cmake_opt_tolbase=-DINCLUDE_TOLBASE=ON
) else (
  set cmake_opt_tolbase=-DINCLUDE_TOLBASE=OFF
)

echo.
echo.----------------------------------------------------
echo. Building TOL Installer ^(!platform!^)
echo.----------------------------------------------------

set CMAKE_DIR=!USERPROFILE!\CMake_Build\installer\!mode!-!platform!
if exist "!CMAKE_DIR!" (
  echo.
  echo./^^!\ There is already a built folder of !project!
  set /p answer="mkdir? (y/n) "
  if "!answer!"=="y" (
    rmdir /s /Q "!CMAKE_DIR!"
    mkdir "!CMAKE_DIR!"
  )
) else (
  mkdir "!CMAKE_DIR!"
)
cd "!CMAKE_DIR!"

echo.
set /p answer="cmake? (y/n) "
if "!answer!"=="y" (
  set TOL_RUNTIME=!USERPROFILE!\CMake_Build\tol\!mode!-!platform!\Runtime_Base
  cmake !cmake_opt_tolbase! -DTOL_PLATFORM=!platform! -DTOL_RUNTIME_DIR=!TOL_RUNTIME! -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" "!INSTALL_DIR!"
)

echo.
set /p answer="cpack? (y/n) "
if "!answer!"=="y" (
  cpack
)

cd "!BUILD_DIR!"
echo.
exit /b
