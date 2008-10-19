/* tol_oisstream.h: Generic data media management

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


#ifndef TOL_OISSTREAM_H
#define TOL_OISSTREAM_H 1

#include <tol/tol_barray.h>
class BStream;

//------------------------------------------------------------------
   class BStreamHandler
/*! A stream handler is a way to connect with a set of streams 
    supporting information of any type. For example all files in a
    same directory, all files archived in the same ZIP archive or all
    blob fields in the same register of a table in a database.
    This API has been designed for OIS but it's possible to use it 
    in other features.
*/
//------------------------------------------------------------------
{
 public:
  //------------------------------------------------------------------
    enum BOpenMode
  //------------------------------------------------------------------
  {
    BSHOM_UNKNOWN = 0,
    BSHOM_READ    = 1,
    BSHOM_WRITE   = 2
  };
  //------------------------------------------------------------------
     enum BMedia
  //------------------------------------------------------------------
  {
    BSHM_none = 0,
    BSHM_dir  = 1,
    BSHM_zip  = 2,
    BSHM_db   = 3
  };

  BStreamHandler() 
  : connection_(),
    connected_ (false),
    openMode_  ()
  {}
  virtual ~BStreamHandler() { Disconnect(); }
  static BStreamHandler* GetConnect(const char* connection, 
                                    BOpenMode   openMode, 
                                    bool        errorWarning); 
  bool         Connected () { return(connected_); }
  BOpenMode    OpenMode  () { return(openMode_); }
  const BText& Connection() { return(connection_); }
  virtual bool Connect(const char*        connection, 
                       BStreamHandler::BOpenMode openMode,
                       bool               errorWarning) = 0;
  virtual BMedia Media() const { return(BSHM_none); }
  virtual BStream* Open(const char* title, const char* name, int index=-1) = 0;
  virtual void Disconnect() { connected_ = false; }
  virtual bool HasFile(const char* fileName) const = 0;
  virtual bool RemoveFiles(const BArray<BText>& files) = 0;
 protected:
  BText     connection_; //!<A path, a query, ...
  bool      connected_;  //!<true if stream handler is ok
  BOpenMode openMode_;   //!<Current access type: read or write
};

//------------------------------------------------------------------
   class BStream
// Stream class encapsulates all methods to access data from 
// different media sources as files, ZIP's, data bases, etc; that
// has been previously open in a BStreamHandler
//------------------------------------------------------------------
{
public: 

  BStream();
  virtual ~BStream();

  const BText&  Title      () const { return(title_); }
  const BText&  Name       () const { return(name_); }
  BINT64        Offset     () const { return(offset_); }
  int           Entries    () const { return(entries_); }
  time_t        Time       () const { return(time_); } 
  bool          IsOk       () const { return(isOk_); }
  void PutEntries (int entries) { entries_=entries; }
  void IncEntries ()            { entries_++; }

  virtual void          PutTime    (time_t time) { time_ = time; };
  virtual BINT64        Bytes      () = 0;
  virtual void          Flush      () = 0;
  virtual void          Close      ()  {};
  virtual bool          Eof        () = 0;
  virtual void          SetPos     (BINT64 offset) = 0;
  virtual const BINT64& GetPos     () = 0;
  virtual int           GetChar    () = 0;
  virtual bool          Open       (const char* title, 
                                    const char* name, 
                                    BStreamHandler::BOpenMode openMode, 
                                    int index=-1) = 0;
  virtual size_t        Write      (const void *buffer, size_t size, size_t count) = 0;
  virtual size_t        Read       (      void *buffer, size_t size, size_t count) = 0;
  virtual int           Print      (const char *fmt, ...) = 0;
  virtual int           CmprsLvl   () { return(0); }
  virtual void          PutCmprsLvl(int lvl) { }

protected: 
  int    index_;    //!<Number of the item inside the BStreamHandler
  BINT64 offset_;   //!<Current position of cursor
  int    entries_;  //!<Current number of written items
  BText  title_;    //!<Informative description
  BText  name_;     //!<Name to access it (path, field, ...)
  time_t time_;     //!<Last modified time
  bool   isOk_;     //!<true when it's all right
};




#endif //TOL_OISSTREAM_H
