@if "%PATH_CMAKE%"=="" (
  echo.Se recomienda crear la variable de entorno CMAKE_PATH.
  echo.Asi se evita que cmake falle si encuentra sh.exe en el PATH.
  echo.Vea como ejemplo el archivo "cmake_path_example.txt".
) else (
  set PATH=%PATH_CMAKE%
)
