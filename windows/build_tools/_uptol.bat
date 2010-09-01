@Rem Author: Jorge <josp.jorge@gmail.com>

rem @Echo Off
Rem Compila tol y toltcl, el postbuild se encarga de copiar los binarios
Rem Copia stdlib hacia ActiveTOL
Rem Copia tolbase\toltklib\byswidget, tolbase\lib\ hacia ActiveTOL

rem SetLocal enableextensions enabledelayedexpansion
Rem Validacion de argumentos

Rem procesamiento de opciones de linea de comandos
Set _rebuild=0
Set _jup=0
Set _pnsis=0
Set _dotests=0
Set _dodist=0

Set _plog=!CD!\log2clog.exe
Set _fbody=!CD!\test_status\BODY.txt

If Not Exist test_status\nul (
  Md test_status
)

Rem !!!!!!!!!!!!!!!!!!!!!!!

Set _url_bin_=https://www.tol-project.org/win32

Set tdt_subject="UPTOL works fine"
Set tdt_body=%_fbody%
Set tdt_attach=""

      
:READ_OPTS
If "%1"=="" Goto END_OPTS
If "%1"=="/?" Goto SHOW_HELP
If "%1"=="/no_update" (
  Goto NEXT_OPTS
)
If "%1"=="/smartbuild" (
  Set _rebuild=0
  Goto NEXT_OPTS
)
If "%1"=="/rebuild" (
  Set _rebuild=1
  Goto NEXT_OPTS
)
If "%1"=="/jup" (
  Set _jup=1
  Goto NEXT_OPTS
)
If "%1"=="/pnsis" (
  Set _pnsis=1
  Goto NEXT_OPTS
)
If "%1"=="/tests" (
  Set _dotests=1
  Goto NEXT_OPTS
)
If "%1"=="/no_tests" (
  Set _dotests=0
  Goto NEXT_OPTS
)
If "%1"=="/dist" (
  Set _dodist=1
  Goto NEXT_OPTS
)
Set _msg_error=Opcion invalida %1
Goto ERROR
:NEXT_OPTS
Shift
Goto READ_OPTS
:END_OPTS

Rem Goto TMP
Rem Goto DODIST


If "%_rebuild%"=="1" (
  Set _rbopt=/rebuild
)

Set ATOLDIR=!CD!\..\ActiveTol\bin

Goto BEGIN

Rem Ayuda de uso
:SHOW_HELP
@more README
Goto END

Rem Codigo de error
:ERROR
If Not Defined _msg_error (
  Echo Error invocado sin mensage
) else (
  Echo %_msg_error%
  Set tdt_subject="UPTOL works badly"
  Echo %_msg_error% >> %_fbody%
)
Pause
Goto END

Rem Codigo de retorno generico
:RET
If Defined _ret (
  Goto %_ret%
) Else (
  Set _msg_error=Error interno! Punto de retorno no definido.
  Goto ERROR
)

Rem Actualiza scripts en ActiveTOL, recibe como argumento
Rem   - _dir_src,  directorio origen
Rem   - _dir_dest,  directorio destino
:UP_SCRIPTS
If Exist %_dir_dest%\nul (
  Echo Borrando scripts antiguos de %_dir_dest%
  Rmdir /S /Q %_dir_dest% 
)
Echo Copiando scripts nuevos de %_dir_src% a %_dir_dest%
xcopy %_dir_src% %_dir_dest% /S /F /Y /I /EXCLUDE:exclude.lst
If errorlevel 1  (
  Set _msg_error=Error copiando %_dir_src% hacia %_dir_dest%
  Goto ERROR
)
Goto RET

Rem Codigo generico de compilacion
:COMPILE
Rem Verifico que ejecutemos desde el SDK
If Not Defined MSSdk (
  Set _msg_error=Solo se puede compilar desde una consola Microsoft Platform SDK
  Goto ERROR
)
If Not Defined _mod (
  Set _msg_error=Error interno! variable _mod no definida en COMPILE
  Goto ERROR
)
If Not Exist ..\..\%_mod%\nul (
  Set _msg_error=Error interno! el directorio %_mod% debia existir cuando se ha solicitado su compilacion.
  Goto ERROR
)
If Not Defined _sln (
  Set _msg_error=Error interno variable _sln no definida en COMPILE
  Goto ERROR
)
If Not Exist ..\..\%_mod%\win-VC9\%_sln% (
  Set _msg_error=No se puede compilar, el archivo %_mod%\win-VC9\%_sln% no existe
  Goto ERROR
)
If "%_vsvars32%"=="" (
  If Defined VS90COMNTOOLS (
    call "%VS90COMNTOOLS%vsvars32"
    If errorlevel 1 (
      Set _msg_error=No se pudo ejecutar vsvars32
      Goto ERROR
    )
    Set _vsvars32=1
  ) else (
    Echo VS90COMNTOOLS no esta definida intentanto vsvars32 desde el PATH
    call vsvars32
    If errorlevel 1 (
      Set _msg_error=No se pudo ejecutar vsvars32
      Goto ERROR
    )
  )
) else  (
  Echo vsvars32 ya ha sido invocado
)
Rem Verifico postbuild.bat: aseguro que existe. postbuild copia la compilacion a ActiveTOL
If Not Exist ..\..\%_mod%\win-VC9\postbuild.bat (
  xcopy /F /Y postbuild.bat %_mod%\win-VC9
)
vcbuild %_rbopt% /useenv ..\..\%_mod%\win-VC9\%_sln% "Release|Win32"
If errorlevel 1 (
  Set _msg_error=Fallo en vcbuild con la solucion %_sln%
  Goto ERROR
)
Goto RET

:CHECK_VARS
For %%v in %_vars% do (
  If Not defined %%v (
    Echo La variable %%v debe estar definida para compilar %_mod%
    If Not Defined _msg_extra Goto END
    If "%_msg_extra%"=="" Goto END
    Echo %_msg_extra%
    Pause
    Goto END
  )
)
Goto RET

:BEGIN

Rem Compilacion de TOL
Set _mod=tol
Set _sln=gnutol.sln
Set _ret=RET_TOLCC
Goto COMPILE
:RET_TOLCC

Rem Compilacion de TOLTCL
Rem Verifico que las variable de entorno de TOLTCL esten definidas
Set _vars=(TCL_INCLUDE TCL_LIB)
Set _msg_extra=TCL_INCLUDE debe apuntar a los include de Tcl. TCL_LIB debe apuntar a las librerias de Tcl.
Set _ret=RET_VARTT
Goto CHECK_VARS
:RET_VARTT
Set _mod=toltcl
Set _sln=toltcl.sln
Set _ret=RET_TTCC
Goto COMPILE
:RET_TTCC

Rem Compilacion de VBTOL
If "%_jup%"=="1" Goto RET_VBCC
Set _mod=vbtol
Set _sln=vbtol.sln
Set _ret=RET_VBCC
Goto COMPILE
:RET_VBCC

Rem Compilacion de TOLSH
If "%_jup%"=="1" Goto RET_TSHCC
PushD ..\..\tolsh
call build
xcopy /F /Y tolsh.exe !ATOLDIR!
PopD
:RET_TSHCC

If "%_jup%"=="1" Goto END

Rem Copio scripts TOL y TCL hacia ActiveTOL
If Not Exist ..\ActiveTOL\nul (
  Echo Directorio ActiveTOL no existente. No se copiaran scripts.
  Pause
  Goto END
)

Rem Copia de stdlib
Set _dir_src=..\..\tol\stdlib
Set _dir_dest=..\ActiveTOL\bin\stdlib
Set _ret=RET_CPTL
Goto UP_SCRIPTS
:RET_CPTL
Rem copiamos pkgIndex.tcl de toltcl
xcopy ..\..\toltcl\pkgIndex.tcl ..\ActiveTOL\lib\toltcl\ /F /Y /I
If errorlevel 1 (
  Set _msg_error=Error copiando toltcl\pkgIndex.tcl hacia ActiveTOL
  Goto ERROR
)
Rem Copia de scripts tcl, byswidget, toltk y rmtps_client
Rem byswidget
Set _dir_src=..\..\tolbase\lib\byswidget
Set _dir_dest=..\ActiveTOL\lib\byswidget
Set  _ret=RET_CPBYS
Goto UP_SCRIPTS
:RET_CPBYS
Rem toltk
Set _dir_src=..\..\tolbase\lib\toltk
Set _dir_dest=..\ActiveTOL\lib\toltk
Set  _ret=RET_CPTTK
Goto UP_SCRIPTS
:RET_CPTTK
Rem rmtps_client
Set _dir_src=..\..\tolbase\lib\rmtps_client
Set _dir_dest=..\ActiveTOL\lib\rmtps_client
Set  _ret=RET_CPRMT
Goto UP_SCRIPTS
:RET_CPRMT

Rem Recupero ficheros perdidos por cualquier causa
If Not Exist ..\ActiveTOL\bin\tol.exe (
  Echo Recuperando ejecutable perdido ..\ActiveTOL\bin\tol.exe
  copy ..\..\tol\win-VC9\release\tol.exe ..\ActiveTOL\bin\tol.exe
)
If Not Exist ..\ActiveTOL\bin\tol.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\bin\tol.dll
  copy ..\..\tol\win-VC9\release\tol.dll ..\ActiveTOL\bin\tol.dll
)
If Not Exist ..\ActiveTOL\bin\tolodbc*.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\bin\tolodbc*.dll
  copy ..\..\tol\win-VC9\libtolodbc\Release\*.dll ..\ActiveTOL\bin
)  
If Not Exist ..\ActiveTOL\bin\tolmysql*.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\bin\tolmysql*.dll
  copy ..\..\tol\win-VC9\libtolmysql\Release\*.dll ..\ActiveTOL\bin
)  
If Not Exist ..\ActiveTOL\bin\tolpgsql*.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\bin\tolpgsql*.dll
  copy ..\..\tol\win-VC9\libtolpgsql\Release\*.dll ..\ActiveTOL\bin
)  
If Not Exist ..\ActiveTOL\lib\toltcl\toltcl.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\lib\toltcl\toltcl.dll
  copy ..\..\toltcl\win-VC9\Release\toltcl.dll ..\ActiveTOL\lib\toltcl\toltcl.dll
)
If Not Exist ..\ActiveTOL\bin\vbtol.dll (
  Echo Recuperando librería perdida ..\ActiveTOL\bin\vbtol.dll
  copy ..\..\vbtol\win-VC9\Release\vbtol.dll ..\ActiveTOL\bin\vbtol.dll
)

Rem Ejecuto los tests si me lo han pedido
:DO_TESTS
If "%_dodist%"=="1" (
  If "%_dotests%"=="0" (
    Echo "Forzando los test de tol debido a que se ha solicitado /dist"
    Set _dotests=1
  )
)

If "%_dotests%"=="1" (
  If Exist ..\ActiveTOL\bin\tol.exe (
    If Not Exist test_status (
      Md test_status
    ) Else (
      Del /Q test_status\*.*
    )
    Set _tst_cd=!CD!\test_status
    PushD ..\ActiveTOL\bin
    Echo Ejecutando Tol standard tests ...
	echo %CD%\\test_status\
	
    tol.exe -v -c"Text DupTestIndicatorPath=\"%CD%\\test_status\"" "..\\..\\..\\tol_tests\\tol\\_tolTester.tol">!_tst_cd!\init.log
    PopD
    If Exist test_status\FAIL.txt (
      Set test_status=FAIL
      Set tdt_subject="ERROR ejecutando TOL_TESTS"
      Set tdt_body="La ejecucion de TOL_TESTS genero el archivo de estado FAIL.txt. No se ha actualizado Tolbase en %_url_bin_%."
      Set tdt_attach=test_status\FAIL.txt
    ) Else If Exist test_status\WARNING.txt (
      Set test_status=WARNING
      Set tdt_subject="Version %_branchid% actualizada, WARNING ejecutando TOL_TESTS"
      Set tdt_body=test_status\BODY.txt
      Set tdt_attach=test_status\WARNING.txt
    ) Else If Exist test_status\OK.txt (
      Set test_status=OK
      Set tdt_subject="Version %_branchid% actualizada, OK ejecutando TOL_TESTS"
      Set tdt_body=test_status\BODY.txt
      Set tdt_attach=test_status\OK.txt
    ) Else (
      Set test_status=FAIL
      Set tdt_subject="ERROR ejecutando TOL_TEST"
      Echo TOL_TEST no genero archivo de estado, verifique manualmente /tests>>%_fbody%
      Echo.>>%_fbody%
      Echo COMPUTERNAME  = %COMPUTERNAME%>>%_fbody%
      Echo USERNAME      = %USERNAME%>>%_fbody%
      Echo BRANCHID      = ""RAMA SVN"
      Echo.>>%_fbody%
      Echo Se ha adjuntado el log de inicializacion de TOL.>>%_fbody%
      Set tdt_body=test_status\BODY.txt
      Set tdt_attach=test_status\init.log
    )
  ) Else (
    Set _msg_error=No se encuentra ..ºActiveTOL\bin\tol.exe necesario para ejecutar los tests de TOL
    Goto ERROR
  )
)
If "%test_status%"=="FAIL" (
  Echo TOL_TESTS finalizo con errores, chequee %CD%\test_status
  Goto SENDMAIL 
)


Rem empaco la distribucion de tolbase si me lo han pedido
If "%_dodist%"=="1" (
  If Not Exist ..\NSIS\Tolbase*.exe (
    Echo ..\NSIS\Tolbase*.exe no existe
    If Not "%_pnsis%"=="1" (
      Echo Habilitando /pnsis para poder hacer /dist
      Set _pnsis=1
    )
  )
)
Set _cwd=%CD%
If "%_pnsis%"=="1" (

  Echo Generando el archivo de creación del instalador de tolbase ...
  ..\ActiveTOL\bin\tol -v ..\NSIS\Tolbase.nsi.generate.tol
  If Not Exist ..\NSIS\Tolbase.nsi (
    Set _msg_error=El empaquetado de tolbase ..ºNSIS\Tolbase.nsi no existe.
    Goto ERROR
  )
  If Exist ..\NSIS\Tolbase*.exe (
    Del /Q ..\NSIS\Tolbase*.exe
  )
  Echo Creando instalador de tolbase ...
  makensis /V2 /X"SetCompressor /FINAL /SOLID lzma" ..\NSIS\Tolbase.nsi
  If Exist ..\NSIS\Tolbase*.exe (
    Echo El instalador de Tolbase ha sido creado en "%_cwd%\..\NSIS"
  ) Else (
    Set _msg_error=No se ha encontrado el resultado de ejecutar makensis
    Goto ERROR
  )
)

:DODIST
Rem envio el empaquetado hacia www.tol-project.org si se ha pedido
Echo status es %test_status%
If "%_dodist%"=="1" (
  If Exist ..\NSIS\Tolbase*.exe (
    For %%f In (..\NSIS\Tolbase*.exe) Do (
      Echo encontrado %%~nxf
      Set _fbin=%%~nxf
    )
    REM echo Creando el tag automatico correspondiente al empaquetado
    cd ..\NSIS
    REM ..\ActiveTOL\bin\tol -v create_tag_build.tol
    REM call create_tag_build.bat
    REM Echo copiando last_development_tol_release_win.txt hacia www.tol-project.org
    REM ..\ActiveTOL\bin\tol -v -c"Text WriteFile(\"last_development_tol_release_win.txt\",Version);"
    REM pscp last_development_tol_release_win.txt toldevel@tolp.localbayes.es:.
    Echo copiando Tolbase hacia www.tol-project.org
    pscp Tolbase*.exe toldevel@tolp.localbayes.es:/var/www/packages/win32/.
    If "%_branchid%"=="REMtrunkREM" (
      Echo copiando tolbase-%_versionNumber%-setup.exe como tolbase-cvstrunk-setup.exe en www.tol-project.org 
      plink tdt@cvs.tol-project.org "cp /home/bayes/pub/bin/win/tolbase-%_versionNumber%-setup.exe /home/bayes/pub/bin/win/tolbase-cvstrunk-setup.exe"
    )
    REM Echo Moviendo el empaquetado a la carpeta de almacenamiento historico
    REM ..\ActiveTOL\bin\tol -v store_history.tol
    cd..
	GOTO END
    REM Echo Nueva version %_branchid% disponible en %_url_bin_%/!_fbin!>test_status\BODY.txt
    Echo.>>%_fbody%
    Echo El resultado de tol_tests esta disponible en el archivo adjunto.>>%_fbody%
    Echo.>>%_fbody%
  ) Else (
    Echo No se puede enviar Tolbase hacia www.tol-project.exe pues no existe en NSIS
    Set _msg_error=Utilice /pnsis para construirlo
    Goto ERROR
  )  
) Else (
  Goto END
)

GOTO END

:MAILTDT
If Not Defined _lastup (
  Echo.>>%_fbody%
  Echo Variable interna _lastup con la fecha de la ultima actualizacion no esta definida.>>%_fbody%
  Echo No se podra generar el reporte de cambios.>>%_fbody%
  Echo Verifique .cvsopts.bat>>%_fbody%
  Goto SENDMAIL
)
Echo Los cambios desde la fecha %_lastup% son los siguientes:>>%_fbody%
Rem Log de TOL
Set _mod=tol
Set _ret=RET_TOLLOG
Goto CVS_GENLOG
:RET_TOLLOG
Rem Log de TOLTCL
Set _mod=toltcl
Set _ret=RET_TTLOG
Goto CVS_GENLOG
:RET_TTLOG
Rem Log de VBTOL
Set _mod=vbtol
Set _ret=RET_VBLOG
Goto CVS_GENLOG
:RET_VBLOG
Rem Log de TOLBASE
Set _mod=tolbase
Set _ret=RET_TBLOG
Goto CVS_GENLOG
:RET_TBLOG
Rem Log de TOL_TESTS
Set _mod=tol_tests
Set _ret=RET_TTSTLOG
Goto CVS_GENLOG
:RET_TTSTLOG
Rem Actualizo la fecha de ultima actualizacion exitosa
Echo Set _lastup=%DATE:~6,4%-%DATE:~3,2%-%DATE:~0,2%>>.cvsopts.bat

:SENDMAIL
mailtdt %tdt_subject% %tdt_body% %tdt_attach%
:END
Rem Limpia variables d entorno
EndLocal
