/* tol_filehandle.h:  Input-Output built-in TOL functions.
            GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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
#include <win_tolinc.h>
#endif


#include <tol/tol_btext.h>
#include <tol/tol_hash_map.h>

/////////////////////////////////////////////////////////////////////////////
class BFileDesc
/////////////////////////////////////////////////////////////////////////////
{
public:
  typedef hash_map_by_int <BFileDesc*>::dense_ BFileHashByHandle;
  static BFileDesc noFileDesc_;

  FILE* file_;
  BText path_;
  BFileDesc(FILE* file, BText path)
  : file_(file),
    path_(path)
  { }

  static BFileHashByHandle& FileHandle();
  static BFileDesc* FindFileHandle(int handle, bool verbose, const BText& msg, 
                             BFileHashByHandle::const_iterator& found);

  static BFileDesc* FindFileHandle(int handle, bool verbose, const BText& msg);
  static FILE* CheckFileHandle(int handle, bool verbose, const BText& msg, 
                             BFileHashByHandle::const_iterator& found);

  static FILE* CheckFileHandle(int handle, bool verbose, const BText& msg);
  static void SysChkErrNo(bool clean, bool verbose, const BText& msg);

};



