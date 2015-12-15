@echo off
rem ************************************************************
rem  La llamada al script recibe un argumento:
rem    produce_package [|<package_name>]
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
if exist "!PACKAGE_DIR!" (
  if exist "!PACKAGE_DIR!\CMakeLists.txt" (

    echo.
    set /p answer="build? (y/n) "
    if "!answer!"=="y" (
      call !BUILD_DIR!\build_package.bat !package!
    )

    echo.
    set /p answer="svn commit? (y/n) "
    if "!answer!"=="y" (
      if "!platform!"=="32" (
        svn commit "!PACKAGE_DIR!\CppTools\MinGW_32" --message "Archivos .dll del paquete !package! recompilados para Windows32 (MinGW)"
      ) else (
        svn commit "!PACKAGE_DIR!\CppTools\Windows_64_GNU" --message "Archivos .dll del paquete !package! recompilados para Windows64 (MinGW)"
      )
    )
    
  ) else (
    echo./^^!\ The package !package! has not a CMakeLists.txt.    
    exit /b
  )
) else (
  echo.^(^^!^) Cannot find the package !package!.
  exit /b
)

echo.
set /p answer="produce? (y/n) "
if "!answer!"=="y" (
  set tol_file=!BUILD_DIR!\..\produce_package.tol
  if "!platform!"=="32" (
    cd "!TOLGNU_ROOT!\bin"
  ) else (
    cd "!TOLGNU_ROOT_64!\bin"
  )
  call tolsh.exe -c"Text Package=\"!package!\"" !tol_file!
)

cd "!BUILD_DIR!"
echo.
exit /b
