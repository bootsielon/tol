/* StoreZipArchive.cpp: ZipArchive API

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

//--------------------------------------------------------------------
StoreZipArchive::StoreZipArchive()
//--------------------------------------------------------------------
: Store() 
{
}

//--------------------------------------------------------------------
StoreZipArchive::~StoreZipArchive() 
//--------------------------------------------------------------------
{
}

//--------------------------------------------------------------------
//Abre el archivo para lectura o escritura
bool StoreZipArchive::Open(BText path, char openMode)
//--------------------------------------------------------------------
{
  _path = path; 
  _openMode = openMode; 
  bool ok = true;
  try
  {
    if(openMode=='w')
    {
      remove(_path); 
      _zip.Open(_path, CZipArchive::zipCreate);
    }
    else if(openMode=='r')
    {
      _zip.Open(_path, CZipArchive::zipOpenReadOnly, false);
    }
  }
  catch(CZipException ex)
  {
    _openMode = ' ';
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while %s archive %s: %s",
      _path.String(),  
      (openMode=='r')?"reading":"writing",
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}

//--------------------------------------------------------------------
//Cierra el archivo
bool StoreZipArchive::Close()
//--------------------------------------------------------------------
{
  if(_openMode == ' ') { return(true); }
  bool ok = true;
  try
  {
    _zip.Close();
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while %s archive %s: %s",
      _path.String(),  
      "closing",
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
  }
  if(!ok) { Error(_errMsg); }
  else { _openMode = ' '; }
  return(ok);
}


//--------------------------------------------------------------------
//Añade un fichero al archivo
bool StoreZipArchive::FileAdd(BText originalFilePath)
//--------------------------------------------------------------------
{
  bool ok = true;
  try
  {
    _zip.AddNewFile(originalFilePath);
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s file %s : %s",
      _path.String(),  
      originalFilePath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
}


//--------------------------------------------------------------------
//Extrae un fichero del archivo
bool StoreZipArchive::FileExtract(BText originalFilePath, BText destinationDirPath)
//--------------------------------------------------------------------
{
  bool ok = true;
  try
  {
    int index = _zip.FindFile(originalFilePath);
    if((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND))
    {
      _zip.ExtractFile(index, destinationDirPath);
    }
    else
    {
      ok = false;  
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while extracting from archive file %s to %s%s: %s",
      _path.String(),  
      originalFilePath.String(),
      destinationDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
}


//--------------------------------------------------------------------
//Determina si existe o no un fichero dentro del archivo
bool StoreZipArchive::FileExist(BText originalFilePath)
//--------------------------------------------------------------------
{
  int index = -1;
  bool ok = false;
  try
  {
    index = _zip.FindFile(originalFilePath);
    ok = ((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND));
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while finding in archive %s file %s: %s",
      _path.String(),  
      originalFilePath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
}


//--------------------------------------------------------------------
//Añade un directorio completo recursivamente
bool StoreZipArchive::DirAdd(BText originalDirPath)
//--------------------------------------------------------------------
{
  bool ok = true;
  try
  {
    // to include empty directories, use the following filter
    ZipArchiveLib::CNameFileFilter filter(
      _T("*"), 
      false, 
      ZipArchiveLib::CNameFileFilter::toAll);
    // This will include empty directories
    _zip.AddNewFiles(originalDirPath, filter);
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s directory %s : %s",
      _path.String(),  
      originalDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
}


//--------------------------------------------------------------------
//Extrae un directorio completo recursivamente
bool StoreZipArchive::DirExtract(BText originalDirPath, BText destinationDirPath)
//--------------------------------------------------------------------
{
  bool ok = true;
  try
  {
    CZipIndexesArray indexes;
    // find files using wildcards
    //zip.FindMatches(_T("file?.*"), indexes);    
    _zip.FindMatches(_T(originalDirPath+"/*"), indexes);    
    _tprintf(_T("Matches found at positions:\r\n"));
    ok = indexes.GetCount()>0;
    for (ZIP_ARRAY_SIZE_TYPE i = 0; i < indexes.GetCount(); i++)
    {
       _zip.ExtractFile(i, destinationDirPath);
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while extracting from archive %s file %s to %s: %s",
      _path.String(),  
      originalDirPath.String(),
      destinationDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
};


//--------------------------------------------------------------------
//Determina si existe o no un directorio dentro del archivo
bool StoreZipArchive::DirExist(BText originalDirPath)
//--------------------------------------------------------------------
{
  bool ok = true;
  try
  {
    CZipIndexesArray indexes;
    // find files using wildcards
    //zip.FindMatches(_T("file?.*"), indexes);    
    _zip.FindMatches(_T(originalDirPath+"/*"), indexes);    
    _tprintf(_T("Matches found at positions:\r\n"));
    ok = indexes.GetCount()>0;
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while finding directory %s in archive %s: %s",
      _path.String(),  
      originalDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
  }
  return(ok);
};
