@echo off
rem *********************************************************
rem  La llamada al script recibe dos argumentos:
rem    build [tol|toltcl|tolbase|all] [|Release|Debug]
rem *********************************************************
rem  Se ha de ejecutar desde el propio directorio MinGW_32.
rem  Durante el proceso por cada una de las etapas.
rem *********************************************************

call cmake_prepare.bat

set project=%1
if "%project%"=="all" (
  build tol %2
  build toltcl %2
  build tolbase %2
  exit /b
)
if "%project%"=="tol" goto project_ok
if "%project%"=="toltcl" goto project_ok
if "%project%"=="tolbase" goto project_ok
echo.First argument should be: tol, toltcl, tolbase or all.
exit /b
:project_ok

set mode=%2
if "%mode%"=="" set mode=Release
if "%mode%"=="Release" goto type_ok
if "%mode%"=="Debug" goto type_ok
echo.Second argument should be: Release, Debug or should be avoided.
exit /b
:type_ok

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

echo.
echo.----------------------------------------------------
echo. Building %project% - %mode%
echo.----------------------------------------------------

set init_dir=%CD%
set tolp_trunk=%tolp_root%\trunk
echo.  tolp_trunk=%tolp_trunk%
if "%project%"=="tol" (
  set tol_contribs=%tolp_root%\toldevel-rtools-m32
  echo.  tol_contribs=%tol_contribs%
)
if exist "%tolp_trunk%\%project%\CodeLite\%mode%" (
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
if "%is_built%"=="y" rmdir /S /Q "%tolp_trunk%\%project%\CodeLite\%mode%"
mkdir "%tolp_trunk%\%project%\CodeLite\%mode%"
:skip_mkdir

cd "%tolp_trunk%\%project%\CodeLite\%mode%"

echo.
set /p cont="cmake? (y/n) "
if "%cont%"=="n" GOTO skip_cmake
if "%project%"=="tol" (
  cmake -DTOL_EXTERNAL_DEVEL_DIR="%tol_contribs%" -DCMAKE_TOOLCHAIN_FILE="%tolp_trunk%\tol\cmake\mingw-m32.toolchain.cmake" -DCMAKE_BUILD_TYPE=%mode% -G"CodeLite - MinGW Makefiles" "%tolp_trunk%\%project%"
) else (
  cmake -DTOL_PREFIX_PATH="%tolp_trunk%\tol\CodeLite\Release\Runtime_Base" -DCMAKE_TOOLCHAIN_FILE="%tolp_trunk%\tol\cmake\mingw-m32.toolchain.cmake" -DCMAKE_BUILD_TYPE=%mode% -G"CodeLite - MinGW Makefiles" "%tolp_trunk%\%project%"
)
:skip_cmake

echo.
set /p cont="make? (y/n) "
if "%cont%"=="n" GOTO skip_make
echo %CD%
mingw32-make
:skip_make

echo.
set /p cont="install? (y/n) "
if "%cont%"=="n" GOTO skip_install
mingw32-make install
:skip_install

cd "%init_dir%"
echo.
pause
exit /b


