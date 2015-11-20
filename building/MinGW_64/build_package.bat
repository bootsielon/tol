@echo off
rem *********************************************************
rem  La llamada al script recibe un argumento opcional:
rem    build_package [<package_name>]
rem *********************************************************
setlocal enabledelayedexpansion

call cmake_prepare.bat

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

set init_dir=%CD%
set tolp_runtime=%tolp_root%\trunk\tol\CodeLite64\Release\Runtime_Base
set tol_cmake_dir=%tolp_root%\trunk\tol\cmake

if "%1"=="" (
  set /p package="package? "
) else (
  set package=%1
)

set package_dir=%CD%\..\..\..\OfficialTolArchiveNetwork\%package%

if exist "%package_dir%\CMakeLists.txt" (
  cd %package_dir%
  set cmake_dir=BUILD_CMAKE\m64\release
) else (
  if exist %package_dir% (
    echo No se encuentra el CMakeLists.txt
  ) else (
    echo No se encuentra el paquete
  )
  pause
  exit /b
)

echo.
set /p answer="mkdir? (y/n) "
if "!answer!"=="y" (
  if exist "%cmake_dir%" rmdir /S /Q "%cmake_dir%"
  mkdir "%cmake_dir%"
)

cd %cmake_dir%

echo.
set /p answer="cmake? (y/n) "
if "!answer!"=="y" (  
  cmake -DTOL_PREFIX_PATH=%tolp_runtime% -DCMAKE_TOOLCHAIN_FILE=%tol_cmake_dir%\mingw-m64.toolchain.cmake -DCMAKE_BUILD_TYPE=Release -G"CodeLite - MinGW Makefiles" ..\..\.. -DCBLAS_DIR=c:/users/toldevel/external/atlas-windows -DCLAPACK_DIR=c:/users/toldevel/external/atlas-windows
)

echo.
set /p answer="make? (y/n) "
if "!answer!"=="y" (
  mingw32-make
)
echo.
pause
cd %init_dir%
exit /b
