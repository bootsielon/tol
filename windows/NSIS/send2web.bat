@echo Creating NSIS SETUP file
..\ActiveTOL\bin\tol -v Tolbase.nsi.generate.tol
makensis /V2 /X"SetCompressor /FINAL /SOLID lzma" ..\NSIS\Tolbase.nsi

@echo Updating web TOLBase installer
pscp Tolbase*.exe toldevel@tolp.localbayes.es:/var/www/packages/win32/.

call send2web.stdlib.bat

@echo Updating web release information
..\ActiveTOL\bin\tol -v -c"Text WriteFile(\"development_tol_release_win.txt\",Version);"
pscp development_tol_release_win.txt toldevel@tolp.localbayes.es:/var/www/packages/win32/.

pause
