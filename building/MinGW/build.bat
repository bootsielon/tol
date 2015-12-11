@echo off
rem ************************************************************
rem  La llamada al script recibe dos argumentos:
rem    build [tol|toltcl|tolbase|rtol] [|release|debug]
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

if "%1"=="" (
  set /p project="project? "
) else (
  set project=%1
)
if "!project!"=="tol" goto project_ok
if "!project!"=="toltcl" goto project_ok
if "!project!"=="tolbase" goto project_ok
if "!project!"=="rtol" goto project_ok
echo.^(^^!^) First argument should be: tol, toltcl, tolbase or rtol.
exit /b
:project_ok

set PROJECT_DIR=!BUILD_DIR!\..\..\!project!
if not exist "!PROJECT_DIR!" (
  echo.^(^^!^) Cannot find folder: !PROJECT_DIR!
  exit /b
)

set mode=%2
if "!mode!"=="" set mode=release
if "!mode!"=="release" goto mode_ok
if "!mode!"=="debug" goto mode_ok
echo.^(^^!^) Second argument should be: release, debug or should be avoided.
exit /b
:mode_ok

echo.
echo.----------------------------------------------------
echo. Building !project! - !mode! ^(!platform!^)
echo.----------------------------------------------------

if "!project!"=="tol" (
  set TOL_CONTRIBS=!BUILD_DIR!\..\..\..\toldevel-rtools-m!platform! 
  if not exist "!TOL_CONTRIBS!" (
    echo.^(^^!^) Cannot find folder: !TOL_CONTRIBS!
    exit /b
  )
)

set CMAKE_DIR=!USERPROFILE!\CMake_Build\!project!\!mode!-!platform!
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
  set CMAKE_TOOLCHAIN=!BUILD_DIR!\..\..\tol\cmake\mingw-m!platform!.toolchain.cmake
  if "!project!"=="tol" (
    cmake -DCLAPACK_NAME=lapacke -DTOL_EXTERNAL_DEVEL_DIR="!TOL_CONTRIBS!" -DCMAKE_TOOLCHAIN_FILE="!CMAKE_TOOLCHAIN!" -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" "!PROJECT_DIR!"
  ) else (
    set TOL_RUNTIME=!USERPROFILE!\CMake_Build\tol\!mode!-!platform!\Runtime_Base
    cmake -DTOL_PREFIX_PATH="!TOL_RUNTIME!" -DCMAKE_TOOLCHAIN_FILE="!CMAKE_TOOLCHAIN!" -DCMAKE_INSTALL_PREFIX= -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" "!PROJECT_DIR!"
  )
)

echo.
set /p answer="make? (y/n) "
if "!answer!"=="y" (
  mingw32-make
)

echo.
set /p answer="install? (y/n) "
if "!answer!"=="y" (
  mingw32-make install
)

cd "!BUILD_DIR!"
echo.
exit /b
