@Echo off
REM Copia el archivo que se le da como argumento %1 hacia el bin de ActiveTol
REM Se usa en:
REM   * el postbuild event de tollbi.vcproj para copiar tollib.dll
REM   * Sel postbuild event de tolcon.vcproj para copiar tol.exe
 
If "%1" == "" (
  Echo Se requiere argumento 1
  Goto Uso
)
If Not Exist %1 (
  Echo %1 no existe
  Goto End
)

If "%2" == "" ( 
  Echo Se requiere argumento 2
  Goto Uso
)
If Not Exist %2\nul (
  Echo Directorio %2 no existe
  Goto End
)
Goto Cont

:Uso
Echo uso -- postbuild bin_origen destino
Pause
Goto End

:Cont

xcopy /F /Y %1 %2
If errorlevel 1 (
  Echo Error en la copia
  Pause
)

:End