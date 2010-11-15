@echo Creating NSIS SETUP file
..\ActiveTOL\bin\tol -v Tolbase.nsi.generate.tol
makensis /V2 /X"SetCompressor /FINAL /SOLID lzma" ..\NSIS\Tolbase.nsi
