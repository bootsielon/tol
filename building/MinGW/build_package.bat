@echo off
rem ************************************************************
rem  La llamada al script recibe dos argumentos:
rem    build_package [|<package_name>] [|release|debug]
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
  set /p package="package? "
) else (
  set package=%1
)

set PACKAGE_DIR=!BUILD_DIR!\..\..\..\OfficialTolArchiveNetwork\%package%
if not exist "!PACKAGE_DIR!\CMakeLists.txt" (
  if exist !PACKAGE_DIR! (
    echo.^(^^!^) Cannot find the CMakeLists.txt
  ) else (
    echo.^(^^!^) Cannot find the package !package!.
  )
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
echo. Building !package! - !mode! ^(!platform!^)
echo.----------------------------------------------------

set CMAKE_DIR=!USERPROFILE!\CMake_Build\packages\!package!\!mode!-!platform!
if exist "!CMAKE_DIR!" (
  echo.
  echo./^^!\ There is already a built folder of !package!
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
  set TOL_RUNTIME=!USERPROFILE!\CMake_Build\tol\!mode!-!platform!\Runtime_Base  
  rem !! cmake -DTOL_PREFIX_PATH=!TOL_RUNTIME! -DCMAKE_TOOLCHAIN_FILE=!CMAKE_TOOLCHAIN! -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" ..\..\.. -DCBLAS_DIR=c:/users/toldevel/external/atlas-windows -DCLAPACK_DIR=c:/users/toldevel/external/atlas-windows
  cmake -DTOL_PREFIX_PATH=!TOL_RUNTIME! -DCMAKE_TOOLCHAIN_FILE=!CMAKE_TOOLCHAIN! -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" "!PACKAGE_DIR!"
)

echo.
set /p answer="make? (y/n) "
if "!answer!"=="y" (
  mingw32-make
)

cd "!BUILD_DIR!"
echo.
exit /b
