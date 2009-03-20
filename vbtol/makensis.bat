@Echo off
If Not Exist VBTol.nsi (
  Echo El empaquetador de VBTol no existe.
)
If Exist VBTol*.exe (
  Del /Q VBTol*.exe
)
Echo Creando instalador de VBTol ...
makensis.exe /V2 /X"SetCompressor /FINAL /SOLID lzma" VBTol.nsi
If Exist VBTol*.exe (
  Echo El instalador de VBTol ha sido creado.
) Else (
  Echo No se ha encontrado el resultado de ejecutar makensis.
)
