/* tol_dbd.h: Class and functions header.
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

#ifndef TOL_BDB_H
#define TOL_BDB_H 1

#include <tol/tol_bqfile.h>
#include <tol/tol_bset.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BDataBase;
class BStruct;
class BGrammar;

//--------------------------------------------------------------------
class BDataBase: public BSyntaxObject
//--------------------------------------------------------------------
{
protected:
  BQFile*  file_;
  BStruct* struct_;

public:
  // Constructors and destructors: bgrammar\syn.cpp
  BDataBase(const BText&   name,
	    const BText&   path,
		  BBool	   fixed,
		  BChar	   sep,
	    const BText&   def,
		  BStruct* str);
 ~BDataBase();

  // Virtual functions inheritage
  BInt	    Mode	() const { return(BDATABASEMODE); }
  BGrammar* Grammar	() const;
  BStruct*  Struct	() const { return(struct_); }

  BSyntaxObject*  Field (BInt reg, BInt field, BGrammar* gra);
  BSyntaxObject*  Field (BInt reg, BInt	 field);
  void		  Reg	(BInt reg, BSet& set);

  // Access & Manipulation: inline
  BQFile* File() { return(file_); }

  friend BDataBase* FindDataBase (const BText& name);
};

/* ARM-style name-injection of friend declaration not ANSI/C++
   standard.*/
BDataBase* FindDataBase (const BText& name);

#endif // TOL_BDB_H
