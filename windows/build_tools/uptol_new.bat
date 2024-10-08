@Rem Author: Jorge <josp.jorge@gmail.com>

@Echo Off
Rem Actualiza desde el SVN el contenido de build_tools (este directorio)
Rem Luego procede a ejecutar _uptol.bat

SetLocal enableextensions enabledelayedexpansion

ECHO Actualizando el codigo desde el repositorio

svn update ..\..

ECHO Codigo actualizado

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
  Echo y ya podr� ejecutar UPTOL sin problemas
  Echo ############################################################################
  
  Goto END
)

CALL _uptol_new.bat %*

:END
