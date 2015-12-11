# Créese una variable de entorno PATH_CMAKE con las rutas necesarias separadas por ";".
# Estas rutas pueden variar de una máquina a otra o con las verrsiones instaladas.
# Esta variable sustituirá a la variable PATH antes de comenzar a compilar.
# A continuación incorporamos una lista de rutas candidatas:

%SystemRoot%\system32
%SystemRoot%
C:\Program Files\CMake\bin || C:\Program Files (x86)\CMake\bin
C:\Rtools\gcc-4.6.3\bin
C:\Program Files\R\R-3.2.2\bin
C:\Program Files\SlikSvn\bin\
C:\Program Files\NSIS
C:\Program Files\GnuWin32\bin || C:\Program Files (x86)\GnuWin32\bin
