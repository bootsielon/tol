@echo Creating NSIS SETUP file
..\ActiveTOL\bin\tolsh -v Tolbase.nsi.generate.tol
makensis /V2 /X"SetCompressor /FINAL /SOLID lzma" ..\NSIS\Tolbase.nsi
