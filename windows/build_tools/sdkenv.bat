@Echo Off
SetLocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
Set _sdkenv=
For /F "tokens=*" %%S In ('dir /ad /b "%ProgramFiles%\Microsoft*SDK*"') Do (
  For /F "tokens=*" %%B In ('dir /s /ad /b "%ProgramFiles%\%%S\*Bin"') Do (
    If Exist "%%B\SetEnv.cmd" (
      Echo %%B\SetEnv.cmd
      Set _sdkenv=%%B\SetEnv.cmd
    )
  )
)
If Defined _sdkenv (
  EndLocal
  For /D %%d In (%SystemRoot%) Do (
    If /I "%%~dd\WINDOWS"=="%SystemRoot%" (
      Set plat=/x86
    ) Else (
      Set plat=/2000jym
    )
  )
  call "%_sdkenv%" !plat! /RELEASE
  Set plat=
  SetLocal ENABLEEXTENSIONS ENABLEDELAYEDEXPANSION
) Else (
  Echo "Error: No se pudo encontrar SetEnv.cmd para habilitar el entorno de Microsoft platform SDK"
  Pause
)
EndLocal