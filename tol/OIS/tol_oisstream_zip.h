/* tol_oisstream_zip.h: Data media management in ZIP files

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

#ifndef TOL_OISSTREAM_ZIP_H
#define TOL_OISSTREAM_ZIP_H 1

#include <tol/tol_oisstream.h>
#include <ZipArchive.h>
#include <ZipMemFile.h>

//------------------------------------------------------------------
   class BZipStreamHandler: public BStreamHandler
// ZipArchive based stream handler
//------------------------------------------------------------------
{
 public:
  CZipArchive zip_; //!< The Zip Archive object

  BZipStreamHandler() : zip_() {}
 ~BZipStreamHandler() { zip_.Close(); }
  bool Connect(const char* connection, 
               BOpenMode   openMode,
               bool errorWarning);
  void Disconnect();
  BMedia Media() { return(BSHM_zip); }
  BStream* Open(const char* title, const char* name, int index=-1);
  bool HasFile(const char* fileName) const;
  bool RemoveFiles(const BArray<BText>& files);
};

//------------------------------------------------------------------
   class BZipStream: public BStream
// ZipArchive based stream
//------------------------------------------------------------------
{
protected: 
  CZipMemFile        mf_;       //!< In memory cached file handler
  BZipStreamHandler& sh_;       //!< The Zip Archive object
  int                cmprsLvl_; //!< Compression level (0..9)
public:
  BZipStream(BZipStreamHandler& sh);
 ~BZipStream();
  bool Open(const char*  title, 
            const char*  name, 
            BStreamHandler::BOpenMode    openMode,
            int index=-1);
  void PutTime(time_t time);
  BINT64        Bytes      ();
  void          Flush      ();
  void          Close      ();
  bool          Eof        ();
  void          SetPos     (BINT64 offset);
  const BINT64& GetPos     ();
  int           GetChar    ();
  size_t        Write      (const void *buffer, size_t size, size_t count);
  size_t        Read       (      void *buffer, size_t size, size_t count);
  int           Print      (const char *fmt, ...);
  int           CmprsLvl   () { return(cmprsLvl_); }
  void          PutCmprsLvl(int lvl) { cmprsLvl_ = lvl; }
  static        BText CPUReport();
};


#endif //TOL_OISSTREAM_ZIP_H

