/* oisstream_zip.cpp: Data media management in ZIP files

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

#include <tol/tol_bcommon.h>

#ifdef __USE_ZIP_ARCHIVE__

#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btimer.h>
#include <tol/tol_oisstream_zip.h>
#include <zlib/zlib.h>

#if defined(UNIX)
#include <unistd.h>
#endif

BTraceInit("oistram_zip.cpp");
//#define __ZATIME_MONITOR__
#ifdef  __ZATIME_MONITOR__
static clock_t zaInitPointCPUClock_ = 0;   //!< Auxiliar CPU clock annotation
static clock_t zaTotalCPUClock_     = 0;
static double  zaTotalCPUSeconds_   = 0; //!< Auxiliar CPU time annotation
static void zaBeginTimer()
{
  zaInitPointCPUClock_ = clock();
}
static void zaEndTimer() 
{
  zaTotalCPUClock_   += clock() - zaInitPointCPUClock_;
  zaTotalCPUSeconds_  = BTimer::ClockToSecond(zaTotalCPUClock_);
}
BText BZipStream::CPUReport() 
{ 
  return(BText("\nZipArchive Stream Open total time = ")<<zaTotalCPUSeconds_); 
}

#else
#define zaBeginTimer()
#define zaEndTimer()
BText BZipStream::CPUReport() 
{ 
  return(""); 
}
#endif // __ZATIME_MONITOR__


class BOpenZipArchive: public CZipArchive
{
public:
  CZipCentralDir& CentralDir() { return(m_centralDir); }
};


//--------------------------------------------------------------------
  static void ShowCatchedException(CZipException& ex,
                                   const BText&   path,
                                   CZipArchive&   zip)
/*! Display the exception message with extended info about the ZIP
  archive */
//--------------------------------------------------------------------
{
  char errMsg[16384];
  sprintf(errMsg, "[ZipArchive]Error while processing archive %s: %s",
                  path.String(),  
                  (LPCTSTR)ex.GetErrorDescription());
  Error(errMsg);
  // close the archive safely releasing resources;
  // the archive will most probably be not usable after this call,
  // if it was modified
  zip.Close(CZipArchive::afAfterException);
}

//--------------------------------------------------------------------
  BZipStream::BZipStream(BZipStreamHandler& sh)
//--------------------------------------------------------------------
: BStream  (),
  sh_      (sh), 
  mf_      (),
  cmprsLvl_(Z_BEST_SPEED)
{
}

//--------------------------------------------------------------------
  BZipStream::~BZipStream()
//--------------------------------------------------------------------
{
  assert(!isOk_);
}


//--------------------------------------------------------------------
  bool BZipStream::Open(const char*  title, 
                        const char*  name, 
                        BStreamHandler::BOpenMode    openMode,
                        int index)
/*!
  If openMode is BSHOM_READ extracts a stream from a opened Zip 
  Archive, else does nothing because writing will be cached in mf_
  If index>=0 no searching will be done, else index will be searched
  by name */
//--------------------------------------------------------------------
{
  zaBeginTimer();
  BBool read  = openMode==BStreamHandler::BSHOM_READ;
  BBool write = openMode==BStreamHandler::BSHOM_WRITE;
  title_   = title;
  name_    = ReplaceSlash(name);
  offset_  = 0;
  entries_ = 0;
  isOk_ = true;
  BText action = I2("'INVALID ACTION'", "'ACCIÓN INVALIDA'");
  try
  {
    if(read)
    {
      action = I2("reading","lectura");
      if((index<0) || (index==ZIP_FILE_INDEX_NOT_FOUND))
      {
        index = sh_.zip_.FindFile(name_);
      }
      isOk_ = ((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND));
      if(isOk_)
      {
        sh_.zip_.ExtractFile(index, mf_);
        CZipFileHeader header;
        sh_.zip_.GetFileInfo(header,index);
        time_ = header.GetTime();
      }
    }
    else if(write)
    {
      action = I2("writing","escritura");
    }
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
  }
  if(!isOk_)
  {
    Warning(I2("Cannot open file ","No se puede abrir ")+name_+
            I2(" for "," para ")+action+
            I2(" from current ZIP ", " desde el ZIP en curso ")+
            sh_.Connection());
  }
  index_ = index;
  zaEndTimer();
  return(isOk_);
}

//--------------------------------------------------------------------
  void BZipStream::PutTime(time_t time)
//!VBR: DOESN'T WORK YET ALTHOUGH ISN'T NEEDED
//--------------------------------------------------------------------
{
  assert(isOk_);
  BStream::PutTime(time);
  Flush();
  BOpenZipArchive* zip = (BOpenZipArchive*)(&sh_.zip_);
  try
  {
    CZipFileHeader* header = zip->CentralDir()[index_];
    header->SetTime(time);
    sh_.zip_.FlushBuffers();
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
  }
}

//--------------------------------------------------------------------
  BINT64 BZipStream::Bytes()  
//! Return the size of current cached file
//--------------------------------------------------------------------
{ 
  assert(isOk_);
  BINT64 size;
  try
  {
    size = mf_.GetLength();
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    size = -1;
    isOk_ = false;
  }
  return(size);
}


//--------------------------------------------------------------------
  void BZipStream::Flush()
/*! Must be called just one time before closing in write mode to get 
    the total file size. */
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  try
  {
    if(index_<0)
    {
      sh_.zip_.AddNewFile(mf_, name_.String(), cmprsLvl_);
			//VBR: No s porqu en linux no se cumple esta condicin pero
			//luego todo parece funcionar bien.
      //assert(index_>=0);
    }
		sh_.zip_.FlushBuffers();
	}
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
  }
}

//--------------------------------------------------------------------
  void BZipStream::Close()
//! Close the file
//--------------------------------------------------------------------
{
  try
  {
    mf_.Close();
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
  }
  isOk_ = false;
}

//--------------------------------------------------------------------
  bool BZipStream::Eof()
//! Return true if file reeading is ended
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  size_t offset;
  size_t bytes;
  try
  {
    offset = mf_.GetPosition();
    bytes  = mf_.GetLength  ();
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    return(true);
  }
  assert(offset<=bytes);
  return(offset==bytes);
}


//--------------------------------------------------------------------
const BINT64& BZipStream::GetPos()
//! Returns the current position
//--------------------------------------------------------------------
{
  try
  {
    offset_ = mf_.GetPosition();
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    offset_ = -1;
  }
  return(offset_);
}

//--------------------------------------------------------------------
  void BZipStream::SetPos(BINT64 offset)
//! Goes to specified position
//--------------------------------------------------------------------
{
  offset_ = offset; 
  entries_++;
  try
  {
    if(mf_.Seek(offset_, CZipAbstractFile::begin) < 0) 
    {
      Error(BText("Cannot access position ")+offset_+" of ZIP file "+title_);
    }
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
  }
}

//--------------------------------------------------------------------
  int BZipStream::GetChar()
/*! Returns the next character from the stream and moves reading 
  cursor */
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  char ch;
  try
  {
    mf_.Read (&ch, 1);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    return(0);
  }
  return(ch);
}


//--------------------------------------------------------------------
  size_t BZipStream::Write(const void *buffer, size_t size, size_t count)
/*! Writes a specified number of items of <size> bytes each one and
  moves writing cursor*/ 
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  size_t c = size*count;
  try
  {
    mf_.Write(buffer,c);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    count = 0;
  }
  return(count);
}

//--------------------------------------------------------------------
  size_t BZipStream::Read (void *buffer, size_t size, size_t count)
/*! Reads a specified number of items of <size> bytes each one and
  moves reading cursor*/ 
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_READ));
  try
  {
    mf_.Read(buffer,size*count);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    count = 0;
  }
  return(count);
}

//--------------------------------------------------------------------
  int BZipStream::Print(const char *fmt, ...)
/*! The same behaviour that sprintf */ 
//--------------------------------------------------------------------
{
  assert(isOk_ && (sh_.OpenMode()==BStreamHandler::BSHOM_WRITE));
  static char mem[1024*16];
  int rc;
  va_list args;
  va_start(args, fmt);
  rc = vsprintf(mem, fmt, args);
  va_end(args);
  int len = strlen(mem);
  try
  {
    mf_.Write(mem, len);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, sh_.Connection()+"/"+name_, sh_.zip_);
    isOk_ = false;
    rc = 0;
  }
  return rc;
}

//--------------------------------------------------------------------
  bool BZipStreamHandler::Connect(const char* connection,
                                  BOpenMode   openMode,
                                  bool        errorWarning)
//! Opens the Zip Archive
//--------------------------------------------------------------------
{
  BBool read  = openMode==BStreamHandler::BSHOM_READ;
  BBool write = openMode==BStreamHandler::BSHOM_WRITE;
  connection_ = GetAbsolutePath(connection);
//Std(BText("\nOpening ZIP ")+connection_);
  openMode_ = openMode;
  BText action = I2("'INVALID ACTION'", "'ACCIN INVLIDA'");
  try
  {
    if(read)
    {
      action = I2("open","abrir");
      connected_ = CheckIsFile(connection_)!=0;
      if(connected_)
      {
        zip_.Open(connection_.String(), CZipArchive::zipOpenReadOnly, false);
      }
    }
    else if(write)
    {
      action = I2("create","crear");
      BText dir = GetFilePath(connection_);
      BSys::MkDir(dir,true);
      if(CheckIsFile(connection_)!=0)
      {
        zip_.Open(connection_.String(), CZipArchive::zipOpen);
      }
      else
      {
        zip_.Open(connection_.String(), CZipArchive::zipCreate);
      }
      zip_.FlushBuffers();
      connected_ = CheckIsFile(connection_)!=0;
    }
    if(!connected_)
    { 
      BText msg = I2("Cannot ","No se puede ")+action+" ZIP "+connection_;
      if(errorWarning) { Error(msg); }
      else             { Warning(msg); }
    }
    else
    {
      zip_.SetRootPath(GetFilePath(connection_).String());
      zip_.EnableFindFast();
    }  
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, connection_, zip_);
    connected_ = false;
  }

  return(connected_); 
}

//--------------------------------------------------------------------
  void BZipStreamHandler::Disconnect()
//! Closes this Zip Archive
//--------------------------------------------------------------------
{
//Std(BText("\nClosing ZIP ")+connection_);
  try
  {
    zip_.FlushBuffers();
    zip_.Close(CZipArchive::afNoException, openMode_==BSHOM_WRITE);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, connection_, zip_);
  }
  connected_ = false;
}

//--------------------------------------------------------------------
  BStream* BZipStreamHandler::Open(const char* title, 
                                   const char* name,
                                   int index)
//! Closes the specified file from this Zip Archive
//--------------------------------------------------------------------
{
  BZipStream* fs = NULL;
  try
  {
    fs = new BZipStream(*this);
    if(!fs->Open(title,name,openMode_,index))
    {
      delete fs;
      fs = NULL;
    }
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, connection_+"/"+name, zip_);
    fs = NULL;
  }
  return(fs);
}

//--------------------------------------------------------------------
  bool BZipStreamHandler::HasFile(const char* fileName) const
//! Returns true if 
//--------------------------------------------------------------------
{
  BZipStreamHandler* T = (BZipStreamHandler*)this;
  int index;
  try
  {
    index = T->zip_.FindFile(ReplaceSlash(fileName).String());
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, connection_+"/"+fileName, T->zip_);
    index = -1;
  }
  return((index>=0) && (index!=ZIP_FILE_INDEX_NOT_FOUND));
}

//--------------------------------------------------------------------
  bool BZipStreamHandler::RemoveFiles(const BArray<BText>& files)
//--------------------------------------------------------------------
{
  CZipStringArray files_;
  for(int n=0; n<files.Size(); n++)
  {
    CZipString str(files[n].String());
    files_.Add(str);
  }
  try
  {
    zip_.RemoveFiles(files_);
  }
  catch(CZipException ex)
  {
    ShowCatchedException(ex, connection_, zip_);
    return(false);
  }
  return(true);
}

#endif
