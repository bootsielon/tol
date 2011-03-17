/* tol_PackArchive.h: packs file and directories intos just one 
   archive that can be compressed or not

                   GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef TOL_PackArchive_H
#define TOL_PackArchive_H 1

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_btext.h>
#include <tol/tol_bout.h>
#include <tol/tol_barray.h>
#include <tol/tol_hash_map.h>

class Store;

typedef hash_map_by_int<Store*>::dense_ HashStoreByHandle;

//------------------------------------------------------------------
   class Store
// Abstract archive store
//------------------------------------------------------------------
{
 public:

  static char _errMsg[16384];
  static HashStoreByHandle _instances;
  static bool _isInitialized;
  static int _uniqueId;
  int _handle;

  Store();
  virtual ~Store();

  static bool Initialize();
  static Store* FindInstance(int handle);
  static bool AddInstance(Store* store);
  static bool RemoveInstance(Store* store);

  //Identificador del motor de compresión 
  BText _engine;
  //Extensión por defecto para los archivos
  BText _ext;
  //Ubicación física del archivo en el disco local
  BText _path;
  //Indica si el archivo está correctamente abierto
  //para lectura o escritura o si no se pudo abrir
  // ' ' : no se pudo abrir
  // 'r' : se pudo abrir para lectura
  // 'w' : se pudo abrir para escritura
  char _openMode;

  //Abre el archivo para lectura o escritura
  virtual bool Open(BText path, char openMode);
  //Cierra el archivo
  virtual bool Close();
  //Añade un fichero al archivo
  virtual bool FileAdd(BText originalFilePath, BText pathInZip);
  //Extrae un fichero del archivo
  virtual bool FileExtract(BText originalFilePath, BText destinationDirPath) = 0;
  //Determina si existe o no un fichero dentro del archivo
  virtual bool FileExist(BText originalFilePath) = 0;
  //Añade un directorio completo recursivamente
  virtual bool DirAdd(BText originalDirPath) = 0;
  //Extrae un directorio completo recursivamente
  virtual bool DirExtract(BText originalDirPath, BText destinationDirPath) = 0;
  //Determina si existe o no un directorio dentro del archivo
  virtual bool DirExist(BText originalDirPath) = 0;

  //Creates an instance of corresponding ingerited class for specified engine
  //ad opens it
  static int Create(BText engine, BText path, char openMode);

};

#endif // TOL_PackArchive_H

