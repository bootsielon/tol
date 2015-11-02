@echo off
rem *********************************************************
rem  La llamada al script recibe un argumento opcional:
rem    produce_package [<package_name>]
rem *********************************************************
setlocal enabledelayedexpansion

call cmake_prepare.bat

set tolp_root=%CD%\..\..\..
if not exist "%tolp_root%\trunk" (
  echo./!\ Cannot find 'trunk' at %tolp_root%
  exit /b
)

set init_dir=%CD%

if "%1"=="" (
  set /p package="package? "
) else (
  set package=%1
)

set package_dir=%CD%\..\..\..\OfficialTolArchiveNetwork\%package%

if exist "%package_dir%" (
  if exist "%package_dir%\CMakeLists.txt" (

    echo.
    set /p answer="build? (y/n) "
    if "!answer!"=="y" (
      call build_package.bat %package%
    )

    echo.
    set /p answer="svn commit? (y/n) "
    if "!answer!"=="y" (
      svn commit "%package_dir%\CppTools\Windows_64_GNU" --message "Archivos .dll del paquete %package% recompilados para Windows64 (MinGW)"
    )
    
  ) else (
    echo El paquete no tiene parte binaria
    pause
    exit /b
  )
) else (
  echo "No se encuentra el paquete"
  pause
  exit /b
)

echo.
set /p answer="produce? (y/n) "
if "!answer!"=="y" (
  set tol_file=%CD%\..\produce_package.tol
  cd "%TOLGNU_ROOT_64%\bin"
  call tolsh.exe -c"Text Package=\"%package%\"" !tol_file!
)

echo.
pause
cd %init_dir%
exit /b
