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
#include "tol_bdir.h"

//--------------------------------------------------------------------
StoreZipArchive::StoreZipArchive()
//--------------------------------------------------------------------
: Store(), _exceptions(0) 
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
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  _path = path; 
  _openMode = openMode; 
  bool ok = true;
  try
  {
    if(openMode=='w')
    {
      remove(_path); 
      ok = _zip.Open(_path.String(), CZipArchive::zipCreate);
    }
    else if(openMode=='r')
    {
     ok =  _zip.Open(_path.String(), CZipArchive::zipOpenReadOnly, false);
    }
    if(!ok)
    {
      sprintf(_errMsg,"[ZipArchive] The archive %s was already opened "
      "before or an invalid open mode was specified.",_path.String());
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
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}

//--------------------------------------------------------------------
//Cierra el archivo
bool StoreZipArchive::Close()
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
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
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  else { _openMode = ' '; }
  return(ok);
}


//--------------------------------------------------------------------
//Añade un fichero al archivo
bool StoreZipArchive::FileAdd(BText originalFilePath, BText pathInZip)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  bool ok = true;
  try
  {
    if(pathInZip=="")
    {
      BText stdFilePath = GetStandardAbsolutePath(originalFilePath);
      BText stdZipPath = GetStandardAbsolutePath(_path);
      pathInZip = Replace(stdFilePath,stdZipPath,"");
    }
    ok = _zip.AddNewFile(originalFilePath.String(),pathInZip.String());
    if(!ok)
    {
      sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s file %s",
        _path.String(),  
        originalFilePath.String());
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s file %s : %s",
      _path.String(),  
      originalFilePath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}


//--------------------------------------------------------------------
//Extrae un fichero del archivo
bool StoreZipArchive::FileExtract(BText originalFilePath, BText destinationDirPath_)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  bool ok = true;
  BText destinationDirPath = GetStandardAbsolutePath(destinationDirPath_+"/");
  try
  {
    int index = _zip.FindFile(originalFilePath.String());
    if((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND))
    {
      ok = _zip.ExtractFile(index, destinationDirPath.String());
      if(!ok)
      {
        sprintf(_errMsg, "[ZipArchive] Error while extracting from archive %s file %s to %s",
          _path.String(),  
          originalFilePath.String(),
          destinationDirPath.String());
      }
    }
    else
    {
      ok = false;
      sprintf(_errMsg, "[ZipArchive] Error while extracting from archive %s file %s to %s: File not found in ZIP",
        _path.String(),  
        originalFilePath.String(),
        destinationDirPath.String());
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while extracting from archive %s file %s to %s: %s",
      _path.String(),  
      originalFilePath.String(),
      destinationDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}


//--------------------------------------------------------------------
//Determina si existe o no un fichero dentro del archivo
bool StoreZipArchive::FileExist(BText originalFilePath)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  int index = -1;
  bool ok = false;
  try
  {
    index = _zip.FindFile(originalFilePath.String());
    ok = ((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND));
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while searching in archive %s file %s: %s",
      _path.String(),  
      originalFilePath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}


//--------------------------------------------------------------------
//Añade un directorio completo recursivamente
bool StoreZipArchive::DirAdd(BText originalDirPath)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  bool ok = true;
  try
  {
    // to include empty directories, use the following filter
    ZipArchiveLib::CNameFileFilter filter(
      _T("*"), 
      false, 
      ZipArchiveLib::CNameFileFilter::toAll);
    // This will include empty directories
    ok = _zip.AddNewFiles(originalDirPath.String(), filter);
    if(!ok)
    {
      sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s directory %s : %s",
        _path.String(),  
        originalDirPath.String());
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while adding to archive %s directory %s : %s",
      _path.String(),  
      originalDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
}


//--------------------------------------------------------------------
//Extrae un directorio completo recursivamente
bool StoreZipArchive::DirExtract(BText originalDirPath, BText destinationDirPath)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  bool ok = true;
  try
  {
    CZipIndexesArray indexes;
    // find files using wildcards
    //zip.FindMatches(_T("file?.*"), indexes);    
    BText match = originalDirPath;
    _zip.FindMatches(match.String(), indexes);    
    ok = indexes.GetCount()>0;
    if(!ok)
    {
      sprintf(_errMsg, "[ZipArchive] Cannot find files to extract from archive "
        "%s matching pattern '%s'",
        _path.String(),  
        match.String());
    }
    for (ZIP_ARRAY_SIZE_TYPE i = 0; i < indexes.GetCount(); i++)
    {
       _zip.ExtractFile(i, destinationDirPath);
    }
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while extracting from archive %s file "
      "%s to %s: %s",
      _path.String(),  
      originalDirPath.String(),
      destinationDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
};


//--------------------------------------------------------------------
//Determina si existe o no un directorio dentro del archivo
bool StoreZipArchive::DirExist(BText originalDirPath)
//--------------------------------------------------------------------
{
  if(_exceptions) { return(false); }
  _errMsg[0]='\0';
  bool ok = true;
  try
  {
    CZipIndexesArray indexes;
    // find files using wildcards
    //zip.FindMatches(_T("file?.*"), indexes);    
    BText match = originalDirPath+"/*";
    _zip.FindMatches(match.String(), indexes);    
    ok = indexes.GetCount()>0;
  }
  catch(CZipException ex)
  {
    ok = false;
    sprintf(_errMsg, "[ZipArchive] Error while finding directory %s in archive %s: %s",
      _path.String(),  
      originalDirPath.String(),
      (LPCTSTR)ex.GetErrorDescription());
    _zip.Close(CZipArchive::afAfterException);
    _exceptions ++;
  }
  if(!ok) { Error(_errMsg); }
  return(ok);
};
