
@echo Updating web TOLBase installer
pscp tolbase-v2.0.1-setup.exe toldevel@tolp.localbayes.es:/var/www/packages/win32/.

@echo Updating web release information
..\ActiveTOL\bin\tol -v -c"Text WriteFile(\"official_tol_release_win.txt\",Version);"
pscp official_tol_release_win.txt toldevel@tolp.localbayes.es:/var/www/packages/win32/.

pause
call send2web.stdlib.bat


