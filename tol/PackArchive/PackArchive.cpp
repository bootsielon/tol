/* PackArchive.cpp: packs file and directories intos just one 
   archive that can be compressed or not

            GNU/TOL Language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif


#include "tol_PackArchive.h"
#include "tol_StoreZipArchive.h"

#include <tol/tol_btext.h>
#include <tol/tol_bout.h>
#include <tol/tol_barray.h>
#include <tol/tol_hash_map.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>



//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("PackArchive.cpp");


char Store::_errMsg[16384];
HashStoreByHandle Store::_instances;
bool Store::_isInitialized = false;
int Store::_uniqueId = 1;

bool _Store_initialize_ = Store::Initialize(); 


//--------------------------------------------------------------------
Store::Store() 
//--------------------------------------------------------------------
: _engine(""),
  _ext(""),
  _path(""),
  _openMode(' ')
{
  _handle = _uniqueId++;
  AddInstance(this);
}

//--------------------------------------------------------------------
Store::~Store()
//--------------------------------------------------------------------
{
  if(_openMode!=' ') { Close(); }
  RemoveInstance(this);
}  

//--------------------------------------------------------------------
bool Store::Initialize() 
//--------------------------------------------------------------------
{ 
  if(_isInitialized) { return(false); }
  SetEmptyKey  (_instances, NULL);
  SetDeletedKey(_instances, -1);
  return(true);
}

//--------------------------------------------------------------------
Store* Store::FindInstance(int handle)
//--------------------------------------------------------------------
{
  HashStoreByHandle::const_iterator found;
  found = _instances.find(handle);
  if(found!=_instances.end())
  {
    return(found->second);
  }
  else
  {
    return(NULL);
  }
}

//--------------------------------------------------------------------
bool Store::AddInstance(Store* store)
//--------------------------------------------------------------------
{
  HashStoreByHandle::const_iterator found;
  found = _instances.find(store->_handle);
  if(found==_instances.end())
  {
    _instances[store->_handle] = store;
    return(true);
  }
  else
  {
    return(false);
  }
}

//--------------------------------------------------------------------
bool Store::RemoveInstance(Store* store)
//--------------------------------------------------------------------
{
  HashStoreByHandle::const_iterator found;
  found = _instances.find(store->_handle);
  if(found!=_instances.end())
  {
    _instances.erase(store->_handle);
    return(true);
  }
  else
  {
    return(false);
  }
}

//--------------------------------------------------------------------
//VBR: Deberían ser métodos virtuales puros pero fala el linker de
//Visual Studio 2008 de forma misteriosa
//--------------------------------------------------------------------

bool Store::Open(BText path, char openMode) { return(false); };
bool Store::Close() { return(false); };
bool Store::FileAdd(BText originalFilePath, BText pathInZip) { return(false); };
bool Store::FileExtract(BText originalFilePath, BText destinationDirPath) { return(false); };
bool Store::FileExist(BText originalFilePath) { return(false); };
bool Store::DirAdd(BText originalDirPath)  { return(false); };
bool Store::DirExtract(BText originalDirPath, BText destinationDirPath)  { return(false); };
bool Store::DirExist(BText originalDirPath)  { return(false); };

//--------------------------------------------------------------------
//Creates an instance of corresponding ingerited class for specified engine
//ad opens it
  int Store::Create(BText engine, BText path, char openMode)
//--------------------------------------------------------------------
{
  Store* store = NULL;
  if(engine=="ZipArchive")
  {
    store = new StoreZipArchive(); 
  }
  if(!store) { return(0); }
  store->Open(path, openMode);
  return(store->_handle);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveOpen);
  DefExtOpr(1, BDatPackArchiveOpen, "PackArchive.Open", 
  3, 3, "Text Text Text",
  "(Text engine, Text path, Text openMode)",
  I2("Open a packed archive for reading if openMode=='r' or writing "
     "if openMode=='w'",
     "Abre el archivo empaquetado para lectura si openMode=='r' o "
     "escritura si openMode=='w'.\n"),
  BOperClassify::Conversion_);
  void BDatPackArchiveOpen::CalcContens()
//--------------------------------------------------------------------
{
  const BText& engine = Text(Arg(1));
  const BText& path = Text(Arg(2));
  const BText& openMode = Text(Arg(3));

  contens_ = Store::Create(engine, path, openMode[0]);
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveClose);
  DefExtOpr(1, BDatPackArchiveClose, "PackArchive.Close", 
  1, 1, "Real",
  "(Real packArchiveId)",
  I2("Closes a packed archive that was open with ",
     "Cierra archivo empaquetado que fue abierto con ")+
     "PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveClose::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    store->Close();
    delete store;
    contens_ = true;
  }
  else
  {
    contens_ = false;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveFileAdd);
  DefExtOpr(1, BDatPackArchiveFileAdd, "PackArchive.FileAdd",
  3, 3, "Real Text Text",
  "(Real packArchiveId, Text originalFilePath, Text pathInZip)",
  I2("Adds a file to a packed archive that was open with ",
     "Añade un fichero a un archivo empaquetado que fue abierto con ")+
     "PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveFileAdd::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->FileAdd(Text(Arg(2)),Text(Arg(3)));
  }
  else
  {
    contens_ = true;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveFileExtract);
  DefExtOpr(1, BDatPackArchiveFileExtract, "PackArchive.FileExtract", 
  3, 3, "Real Text Text",
  "(Real packArchiveId, Text originalFilePath, Text destinationDirPath)",
  I2("Extracts a file from a packed archive that was open with ",
     "Extrae un fichero de un archivo empaquetado que fue abierto con ")+
     "PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveFileExtract::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->FileExtract(Text(Arg(2)), Text(Arg(3)));
  }
  else
  {
    contens_ = true;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveFileExist);
  DefExtOpr(1, BDatPackArchiveFileExist, "PackArchive.FileExist",
  2, 2, "Real Text",
  "(Real packArchiveId, Text originalFilePath)",
  I2("Checks if there is a file into a packed archive that was open with ",
     "Comprueba si existe un fichero dentro de un archivo empaquetado "
     "que fue abierto con ")+"PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveFileExist::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->FileExist(Text(Arg(2)));
  }
  else
  {
    contens_ = true;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveDirAdd);
  DefExtOpr(1, BDatPackArchiveDirAdd, "PackArchive.DirAdd",
  2, 2, "Real Text",
  "(Real packArchiveId, Text originalDirPath)",
  I2("Adds recursively a directory to a packed archive that was open "
     "with ",
     "Añade recursivamente un directorio a un archivo empaquetado que "
     "fue abierto con ")+
     "PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveDirAdd::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->DirAdd(Text(Arg(2)));
  }
  else
  {
    contens_ = true;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveDirExtract);
  DefExtOpr(1, BDatPackArchiveDirExtract, "PackArchive.DirExtract", 
  3, 3, "Real Text Text",
  "(Real packArchiveId, Text originalFilePath, Text destinationDirPath)",
  I2("Extracts recursively a directory from a packed archive that was open with ",
     "Extrae recursivamente un directorio de un archivo empaquetado que fue abierto con ")+
     "PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveDirExtract::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->DirExtract(Text(Arg(2)), Text(Arg(3)));
  }
  else
  {
    contens_ = true;
  }
}  

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPackArchiveDirExist);
  DefExtOpr(1, BDatPackArchiveDirExist, "PackArchive.DirExist",
  2, 2, "Real Text",
  "(Real packArchiveId, Text originalFilePath)",
  I2("Checks if there is a directory into a packed archive that was open with ",
     "Comprueba si existe un directorio dentro de un archivo empaquetado "
     "que fue abierto con ")+"PackArchive.Open",
  BOperClassify::Conversion_);
  void BDatPackArchiveDirExist::CalcContens()
//--------------------------------------------------------------------
{
  int handle = (int)Real(Arg(1));

  Store* store = Store::FindInstance(handle);
  if(store)
  {
    contens_ = store->DirExist(Text(Arg(2)));
  }
  else
  {
    contens_ = true;
  }
}  

/*
  //Abre el archivo para lectura o escritura
  Real PackArchive.Open(NameBlock archive, BText path, BText readOrWrite);
  //Cierra el archivo
  Real PackArchive.Close(NameBlock archive);
  //Añade un fichero al archivo
  Real PackArchive.File.Add(NameBlock archive, Set originalFilePath);
  //Extrae un fichero del archivo
  Real PackArchive.File.Extract(NameBlock archive, BText originalFilePath, BText destinationDirPath);
  //Determina si existe o no un fichero dentro del archivo
  Real PackArchive.File.Exist(NameBlock archive, BText originalFilePath)
  //Añade un directorio completo recursivamente
  Real PackArchive.Dir.Add(NameBlock archive, BText originalDirPath);
  //Extrae un directorio completo recursivamente
  Real PackArchive.Dir.Extract(NameBlock archive, BText originalDirPath, BText destinationDirPath);
  //Determina si existe o no un directorio dentro del archivo
  Real PackArchive.Dir.Exist(NameBlock archive, BText originalFilePath)
*/

