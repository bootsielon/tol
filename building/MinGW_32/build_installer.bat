@echo off
rem *********************************************************
rem  Se ha de ejecutar desde el propio directorio MinGW_32.
rem  Durante el proceso por cada una de las etapas.
rem *********************************************************

call cmake_prepare.bat

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

echo.
echo.----------------------------------------------------
echo. Building TOL Installer
echo.----------------------------------------------------

set init_dir=%CD%
set tolp_trunk=%tolp_root%\trunk
echo.  tolp_trunk=%tolp_trunk%

set installer_dir=%tolp_trunk%\building\installer%1
echo.  installer_dir=%installer_dir%

if exist "%installer_dir%" (
  echo./!\ There is already a built folder of %project%
  set is_built=y
) else (
  set is_built=n
)

echo.
set /p cont="continue? (y/n) "
if "%cont%"=="y" GOTO continue_ok
exit /b
:continue_ok

echo.
set /p cont="mkdir? (y/n) "
if "%cont%"=="n" GOTO skip_mkdir
cd "%tolp_trunk%"
if "%is_built%"=="y" rmdir /S /Q "%installer_dir%"
mkdir "%installer_dir%"
:skip_mkdir

cd "%installer_dir%"

echo.
set /p cont="cmake? (y/n) "
if "%cont%"=="n" GOTO skip_cmake
cmake -DTOL_RUNTIME_DIR=%tolp_trunk%\tol\CodeLite\Release\Runtime_Base -DCMAKE_BUILD_TYPE=Release -G"CodeLite - MinGW Makefiles" "%tolp_trunk%"
:skip_cmake

echo.
set /p cont="cpack? (y/n) "
if "%cont%"=="n" GOTO skip_cpack
echo %CD%
cpack
:skip_cpack

cd %init_dir%
echo.
pause
exit /b
