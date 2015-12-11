
@if "%PATH_CMAKE%"=="" (
  echo.Se recomienda crear la variable de entorno PATH_CMAKE.
  echo.Asi se evita que cmake falle si encuentra sh.exe en el PATH.
  echo.Vea las instrucciones del archivo "path_cmake_readme.txt".
) else (
  set "PATH=%PATH_CMAKE%"
)