@echo Compressing StdLib
"c:\Archivos de programa\7-Zip\7z.exe" a -r -tzip StdLib.zip ..\ActiveTol\bin\StdLib
@echo Updating web StdLib
pscp StdLib.zip toldevel@tolp.localbayes.es:/var/www/packages/win32/official/.
