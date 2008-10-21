@echo Creating automatic tag
..\ActiveTOL\bin\tol -v create_tag_build.tol
call create_tag_build.bat
@echo Updating web release information
..\ActiveTOL\bin\tol -v -c"Text WriteFile(\"last_development_tol_release_win.txt\",Version);"
pscp last_development_tol_release_win.txt tdt@cvs.tol-project.org:/home/bayes/pub/bin/win/last_development_tol_release_win.txt
@echo Updating web TOLBase installer
pscp tolbase-1.1.7-setup.exe tdt@cvs.tol-project.org:/home/bayes/pub/bin/win/tolbase-cvstrunk-setup.exe
@echo Moviendo el empaquetado a la carpeta de almacenamiento historico
..\ActiveTOL\bin\tol -v store_history.tol
pause
