
@if "%PATH_CMAKE%"=="" (
  echo.Se recomienda crear la variable de entorno PATH_CMAKE.
  echo.Asi se evita que cmake falle si encuentra sh.exe en el PATH.
  echo.Vea como ejemplo el archivo "path_cmake_example.txt".
) else (
  set "PATH=%PATH_CMAKE%"
)