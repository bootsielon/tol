@Rem Author: Jorge <josp.jorge@gmail.com>


..\..\tol\win-VC8\release\build_date_gen.exe

touch ..\..\tol\init.cpp

@Echo Off
Rem Actualiza desde el SVN el contenido de build_tools (este directorio)
Rem Luego procede a ejecutar _uptol.bat

SetLocal enableextensions enabledelayedexpansion


If not "%1"=="/no_update" (
ECHO Actualizando el codigo desde el repositorio
svn update ..\..
ECHO Codigo actualizado
)

:DO

If Not Exist ..\ActiveTOL\nul (
  ..\ActiveTOL_Base.exe -y
)

If Not Exist ..\tol-contrib\nul (
  ..\tol-contrib.exe -y
  cd ..\tol-contrib
  call setup.bat
  cd ..\build_tools
  Echo ############################################################################
  Echo El paquete win-tol-contrib ha sido instalado
  Echo Por favor, cierre esta ventana de comandos y abra de nuevo otra con STARTENV
  Echo y ya podrá ejecutar UPTOL sin problemas
  Echo ############################################################################
  
  Goto END
)

CALL _uptol.bat %*

:END
