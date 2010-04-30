/* oisstream.cpp: Generic data media management

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

#include <tol/tol_bsys.h>
#include <tol/tol_bdir.h>

#include <tol/tol_oisstream.h>
#include <tol/tol_oisstream_dir.h>
#ifdef __USE_ZIP_ARCHIVE__
#include <tol/tol_oisstream_zip.h>
#endif

BTraceInit("oisstream.cpp");

//--------------------------------------------------------------------
  BStream::BStream()
//--------------------------------------------------------------------
: index_  (-1),
  offset_ (0),
  entries_(0),
  title_  (),
  name_   (),
  time_   (),
  isOk_   (false) 
{
}

//--------------------------------------------------------------------
  BStream::~BStream()
//--------------------------------------------------------------------
{
  Close();
}


//--------------------------------------------------------------------
  BStreamHandler*
  BStreamHandler::GetConnect(const char* connection, 
                             BStreamHandler::BOpenMode openMode,
                             bool errorWarning)
/*!
  Depending on the string connection determines the type of stream
  handler to use to open and return it.
  If any error happens returns NULL
*/
//--------------------------------------------------------------------
{
  BMedia media = BSHM_dir;
  BText c = connection;
  BText b = c.SubString(0,4);
  BText e = c.SubString(c.Length()-4,c.Length());
  b.ToLower();
  e.ToLower();
//if(b=="db:/") { media = BSHM_db; }
  if(e==".zip") { media = BSHM_zip; }
  if(e==".oza") { media = BSHM_zip; }
//Std(BText("TRACE BStreamHandler::GetConnect 4\n"));
  BStreamHandler* fh = NULL;
  switch(media)
  {
    case BSHM_dir: fh = new BDirStreamHandler;  break;
# ifdef __USE_ZIP_ARCHIVE__
    case BSHM_zip: fh = new BZipStreamHandler;  break;
# else
    case BSHM_zip: Error("ZipArchive is disabled in this TOL distribution");  break;
# endif
  //case BSHM_db:  fh = new BDbStreamHandler;   break;
    default: assert(1); break;
  };
//Std(BText("TRACE BStreamHandler::GetConnect 5\n"));
  if(fh && !fh->Connect(connection, openMode, errorWarning))
  {
  //Std(BText("TRACE BStreamHandler::GetConnect 6\n"));
    delete fh;
  //Std(BText("TRACE BStreamHandler::GetConnect 7\n"));
    fh = NULL;
  }
//Std(BText("TRACE BStreamHandler::GetConnect 8\n"));
  return(fh);
}

