/* oisstream_file.cpp: Data media management in standard plain directory

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

#include <stdio.h>
#include <stdarg.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btimer.h>

#include <tol/tol_oisstream_dir.h>
#include <tol/tol_StoreZipArchive.h>

BTraceInit("oisstram_dir.cpp");

//--------------------------------------------------------------------
  BDirStream::BDirStream(BDirStreamHandler& sh)
//--------------------------------------------------------------------
: BStream(),
  sh_    (sh),
  file_  (NULL),
  bytes_ (-1)
{
}

//--------------------------------------------------------------------
  BDirStream::~BDirStream()
//--------------------------------------------------------------------
{
  assert(!file_);
}

//--------------------------------------------------------------------
  bool BDirStream::Open(const char* title, 
                        const char* name, 
                        BStreamHandler::BOpenMode   openMode,
                        int index)
//--------------------------------------------------------------------
{
  BBool read  = openMode==BStreamHandler::BSHOM_READ;
  BBool write = openMode==BStreamHandler::BSHOM_WRITE;
  title_   = title;
  name_    = name;
  offset_  = 0;
  entries_ = 0;
  isOk_    = false;
  BText action = I2("'INVALID ACTION'", "'ACCIÓN INVÁLIDA'");
  if(read)
  {
    action = I2("open","abrir");
    file_  = fopen(name, "rb");
    time_  = GetFileTime(name);
  }
  else if(write)
  {
    BText dir = GetFilePath(name);
    BSys::MkDir(dir,true);
    action = I2("create","crear");
    file_  = fopen(name, "wb");
  }
  isOk_ = file_!=NULL;
# ifndef NDEBUG
  if(!isOk_)
  {
    Warning(I2("Cannot open file ","No se puede abrir el fichero ")+name+
            I2(" for "," para ")+action+
            I2(" from current directory ", " desde el directorio en curso ")+
            BDir::GetCurrent());
  }
#endif
  return(isOk_);
}

//--------------------------------------------------------------------
  void BDirStream::PutTime(time_t time)
//--------------------------------------------------------------------
{
  assert(isOk_);
  BStream::PutTime(time);
  SetFileTime(name_, time);
}

//--------------------------------------------------------------------
  BINT64 BDirStream::Bytes()  
//--------------------------------------------------------------------
{ 
  assert(isOk_);
  if(sh_.OpenMode()==BStreamHandler::BSHOM_READ)
  {
    if(bytes_<0)
    {
     bytes_ = GetFileSize(name_);
    }
    return(bytes_);
  }
  else if(!file_) 
  {
    bytes_ = GetFileSize(name_);
  }
  return(bytes_);
}

//--------------------------------------------------------------------
  void  BDirStream::Flush() 
//--------------------------------------------------------------------
{ 
  assert(file_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  fflush(file_); 
  Close();
}

//--------------------------------------------------------------------
  void BDirStream::Close()
//--------------------------------------------------------------------
{
  if(file_)
  {
    fclose(file_);
    file_ = NULL;
  }
}

//--------------------------------------------------------------------
  bool BDirStream::Eof()
//--------------------------------------------------------------------
{
  assert(file_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  return(feof(file_)!=0);
}


//--------------------------------------------------------------------
  int BDirStream::GetChar()
//--------------------------------------------------------------------
{
  assert(file_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  return(fgetc(file_));
}


//--------------------------------------------------------------------
const BINT64& BDirStream::GetPos()
//--------------------------------------------------------------------
{
#if defined(UNIX)
  offset_ = ftell(file_);
  if(offset_==-1) {
    Error(strerror(errno));
  }
#else
  if(fgetpos(file_, &offset_)) {
    Error(strerror(errno));
  }
#endif
  entries_++; 
  return(offset_);
}

//--------------------------------------------------------------------
  void BDirStream::SetPos(BINT64 offset)
//--------------------------------------------------------------------
{
  offset_ = offset; 
#if defined(UNIX)
  if(fseek(file_, offset_, SEEK_SET) == -1) 
  {
    Error(strerror(errno));
  }
#else
  if(fsetpos(file_, &offset)) 
  {
    Error(strerror(errno));
  }
#endif
  entries_++;
}

//--------------------------------------------------------------------
  size_t BDirStream::Write(const void *buffer, size_t size, size_t count)
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  return(fwrite(buffer,size,count,file_));
}

//--------------------------------------------------------------------
  size_t BDirStream::Read (      void *buffer, size_t size, size_t count)
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  return(fread(buffer,size,count,file_));
}

//--------------------------------------------------------------------
  int BDirStream::Print(const char *fmt, ...)
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  int rc;
  va_list args;
  va_start(args, fmt);
  rc = vfprintf(file_, fmt, args);
  va_end(args);
  return rc;
}

//--------------------------------------------------------------------
  bool BDirStreamHandler::Connect(const char* connection,
                                  BOpenMode   openMode,
                                  bool        errorWarning)
//--------------------------------------------------------------------
{
  BBool read  = openMode==BSHOM_READ;
  BBool write = openMode==BSHOM_WRITE;
  connection_ = GetStandardAbsolutePath(connection);
  connection_.Replace('\\','/');
  connected_ = BDir::CheckIsDir(connection_)!=0;
  openMode_ = openMode;
  BText action = I2("'INVALID ACTION'", "'ACCIÓN INVÁLIDA'");
  if(read)
  {
    action = I2("open","abrir");
  }
  else if(write)
  {
    action = I2("create","crear");
    if(!connected_)
    {
      BSys::MkDir(connection_,true);
      connected_ = BDir::CheckIsDir(connection_)!=0;
    }
  }
# ifndef NDEBUG
  if(!connected_)
  { 
    BText msg = I2("Cannot ","No se puede ")+action+" DIR "+connection_;
    if(errorWarning) { Error(msg); }
    else             { Warning(msg); }
  }
# endif
  return(connected_); 
}

//--------------------------------------------------------------------
  BStream* 
  BDirStreamHandler::Open(const char* title, 
                          const char* name,
                          int index)
//--------------------------------------------------------------------
{
  BDirStream* fs = new BDirStream(*this);
  if(!fs->Open(title,connection_+"/"+name,openMode_,index))
  {
    delete fs;
    fs = NULL;
  }
  return(fs);
}


//--------------------------------------------------------------------
  bool BDirStreamHandler::HasFile(const char* fileName) const
//--------------------------------------------------------------------
{
  return(CheckIsFile(connection_+"/"+fileName)!=0);
}
//--------------------------------------------------------------------
  bool BDirStreamHandler::RemoveFiles(const BArray<BText>& files)
//--------------------------------------------------------------------
{
  for(int n=0; n<files.Size(); n++)
  {
    BSys::Remove(files[n]);
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BZDirStreamHandler::Connect(const char* connection,
                                  BOpenMode   openMode,
                                  bool        errorWarning)
//--------------------------------------------------------------------
{
  BBool read  = openMode==BSHOM_READ;
  BBool write = openMode==BSHOM_WRITE;
  connection_ = GetStandardAbsolutePath(connection);
  #ifdef UNIX
     connection_.Replace('\\','/');
  #else
     connection_.Replace('/','\\');
  #endif
  openMode_ = openMode;
  if(read && !CheckIsFile(connection_))
  {
    connected_ = false;
  }
  else
  {
    BText dir = GetFilePath(connection_)+GetFilePrefix(connection_);
    if(read)
    {
      Std(BText("[BZDirStreamHandler] Extracting from ")+connection_);
      StoreZipArchive sza;
      sza.Open(connection_, 'r');
      sza.DirExtract("*",dir);
      sza.Close();
    }
    connected_ = dsh_.Connect(dir,openMode,errorWarning);
  }
  return(connected_); 
}

//--------------------------------------------------------------------
 void BZDirStreamHandler::Disconnect()
//--------------------------------------------------------------------
{
  if(connected_ && (openMode_==BSHOM_WRITE))
  {
    Std(BText("[BZDirStreamHandler] Compressing to ")+connection_);
    StoreZipArchive sza;
    sza.Open(connection_, 'w');
    BText dir = dsh_.Connection();
    #ifdef UNIX
       dir.Replace('\\','/');
    #else
       dir.Replace('/','\\');
    #endif
    sza.DirAdd(dir);
    sza.Close();
  }
  BSys::RmDir(dsh_.Connection());
  connected_ = false;
}

//--------------------------------------------------------------------
  BStream* 
  BZDirStreamHandler::Open(const char* title, 
                          const char* name,
                          int index)
//--------------------------------------------------------------------
{
  return(dsh_.Open(title,name,index));
}

//--------------------------------------------------------------------
  bool BZDirStreamHandler::HasFile(const char* fileName) const
//--------------------------------------------------------------------
{
  return(dsh_.HasFile(fileName));
}

//--------------------------------------------------------------------
  bool BZDirStreamHandler::RemoveFiles(const BArray<BText>& files)
//--------------------------------------------------------------------
{
  return(dsh_.RemoveFiles(files));
}


