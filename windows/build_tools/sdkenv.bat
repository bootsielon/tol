@Echo Off
SetLocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
Set _sdkenv=
For /D %%d In ("%ProgramFiles%\Microsoft Platform SDK*") Do (
  Echo %%d
  If Exist "%%d\SetEnv.cmd" (
    Set _sdkenv=%%d\SetEnv.cmd
  )
)
If Defined _sdkenv (
  EndLocal
  For /D %%d In (%SystemRoot%) Do (
    If /I "%%~dd\WINDOWS"=="%SystemRoot%" (
      Set plat=/XP32
    ) Else (
      Set plat=/2000jym
    )
  )
  call "%_sdkenv%" !plat! /RETAIL
  Set plat=
  SetLocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
) Else (
  Echo "Error: No se pudo encontrar SetEnv.cmd para habilitar el entorno de Microsoft platform SDK"
  Pause
)
EndLocal