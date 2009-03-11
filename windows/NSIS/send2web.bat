rem @echo Creating automatic tag
rem ..\ActiveTOL\bin\tol -v create_tag_build.tol
rem call create_tag_build.bat

@echo Creating NSIS SETUP file
..\ActiveTOL\bin\tol -v Tolbase.nsi.generate.tol
makensis /V2 /X"SetCompressor /FINAL /SOLID lzma" ..\NSIS\Tolbase.nsi

@echo Updating web release information
..\ActiveTOL\bin\tol -v -c"Text WriteFile(\"last_development_tol_release_win.txt\",Version);"
 pscp last_development_tol_release_win.txt toldevel@tolp.localbayes.es:/var/www/packages/win32/.

@echo Updating web TOLBase installer
 pscp Tolbase*.exe toldevel@tolp.localbayes.es:/var/www/packages/win32/.
pause
