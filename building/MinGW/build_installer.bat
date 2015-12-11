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

set TRUNK_DIR=!BUILD_DIR!\..\..
if not exist "!TRUNK_DIR!" (
  echo.^(^^!^) Cannot find folder: !TRUNK_DIR!
  exit /b
)

set mode=release

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

rem set init_dir=%CD%
rem set tolp_trunk=%tolp_root%\trunk
rem echo.  tolp_trunk=%tolp_trunk%
rem 
rem set installer_dir=%tolp_trunk%\building\MinGW_32\installer%
rem echo.  installer_dir=%installer_dir%
rem 
rem if exist "%installer_dir%" (
rem   echo./!\ There is already a built folder of the installer
rem   set is_built=y
rem ) else (
rem   set is_built=n
rem )
rem 
rem echo.
rem set /p cont="continue? (y/n) "
rem if "%cont%"=="y" GOTO continue_ok
rem exit /b
rem :continue_ok

rem echo.
rem set /p cont="mkdir? (y/n) "
rem if "%cont%"=="n" GOTO skip_mkdir
rem cd "%tolp_trunk%"
rem if "%is_built%"=="y" rmdir /S /Q "%installer_dir%"
rem mkdir "%installer_dir%"
rem :skip_mkdir
rem 
rem cd "%installer_dir%"

echo.
set /p answer="cmake? (y/n) "
if "!answer!"=="y" (
  set TOL_RUNTIME=!USERPROFILE!\CMake_Build\tol\!mode!-!platform!\Runtime_Base
  cmake -DTOL_RUNTIME_DIR=!TOL_RUNTIME! -DCMAKE_BUILD_TYPE=!mode! -G"CodeLite - MinGW Makefiles" "!TRUNK_DIR!"
)

echo.
set /p answer="cpack? (y/n) "
if "!answer!"=="y" (
  cpack
)

cd "!BUILD_DIR!"
echo.
exit /b
