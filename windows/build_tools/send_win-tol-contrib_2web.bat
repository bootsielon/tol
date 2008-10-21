@echo Updating web win-tol-contrib-trunk.exe
pscp win-tol-contrib-trunk.exe tdt@cvs.tol-project.org:/home/bayes/pub/bin/win/contrib/win-tol-contrib-trunk.exe
plink tdt@cvs.tol-project.org "cp /home/bayes/pub/bin/win/contrib/win-tol-contrib-trunk.exe /home/bayes/pub/bin/win/contrib/win-tol-contrib-1.1.7.exe"
pause
