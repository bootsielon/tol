/* bstruct.h: Class and functions header. TOL Language.

   Copyright (C) 2003-2007, Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BSTRUCT_H
#define TOL_BSTRUCT_H

#include <tol/tol_barray.h>
#include <tol/tol_bobject.h>
#include <tol/tol_bsyntax.h>

class BList;

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class TOL_API BNewStruct;
class TOL_API BStruct;
class TOL_API BField;
//template class TOL_API BArray<BField>;

//--------------------------------------------------------------------
class TOL_API BField : public BObject
//--------------------------------------------------------------------
{
 protected:
    BGrammar* grammar_;
    BStruct*  struct_;
    
 public:
    // Constructors and destructors: bgrammar\syn.cpp
    BField();
    BField(const BText& name, BGrammar* gra, BStruct* str=NIL);
    BField(const BText& name, const BText& gra, const BText& str="");
   ~BField();
    
    // Access & Manipulation: inline
    BGrammar* Grammar() const { return(grammar_); }
    BStruct* Struct() const { return(struct_); }
    BText GetType() const;
    void PutType(const BText& type);
    void PutGrammar(BGrammar* gra) { grammar_ = gra; }
    void PutStruct (BStruct*  str) { struct_  = str; }
    BText Dump() const;
    DeclareClassNewDelete(BField);
};
//--------------------------------------------------------------------

//--------------------------------------------------------------------
struct TOL_API BStructStatus
//--------------------------------------------------------------------
{
  BStruct* str_;
  BText    name_;
  bool     alive_;
  BStructStatus(BStruct* str=NULL, bool alive=false);
};


template class TOL_API BArray<BField>;
template class TOL_API BArray<BStructStatus>;

//--------------------------------------------------------------------
class TOL_API BStruct: public BSyntaxObject
//--------------------------------------------------------------------
{
public:

protected:
  static BList*	instances_;
  static BArray<BStructStatus> *statusTable_;

  BArray<BField> * field_;
  BNewStruct*	function_;

public:
  // Constructors and destructors: bgrammar\syn.cpp
  BStruct(const BText& name, bool addToSymbolTable=true);
 ~BStruct();

  // Virtual functions inheritage
	BInt	    Mode	     () const { return(BSTRUCTMODE); }
	BGrammar* Grammar    () const;
	BText	    Dump	     () const;

  // Access & Manipulation: inline
  BInt	    FindField  (const BText& name) const;
  BField&   operator[] (BInt n)		   const { return((*field_)[n]); }
  BInt	    Size       ()		   const { return((*field_).Size()); }
  BText	    FieldName  (BInt n)		   const { return((*field_)[n].Name()); }

  BNewStruct* Function() { return(function_); }
  void	PutFunction (BNewStruct* fun);
  void	PutDesc	    ();
  void	PutField    (const BArray<BField>& field);
  void	PutField    (const BText& def);
  BSyntaxObject* Create(BList* lst, const BText& desc="");
  static BStruct* Alive(BStruct* str, bool showError);

  friend TOL_API BStruct* NewStruct(const BText& name, const BText& def);
  friend TOL_API BStruct* NewStructSymbol(const BText& name, const BText& def);
  friend TOL_API BStruct* FindStruct(const BText& name);
  DeclareClassNewDelete(BStruct);
};


//--------------------------------------------------------------------

/* ARM-style name-injection of friend declaration not ANSI/C++
   standard. */
TOL_API BStruct* NewStruct(const BText& name, const BText& def);
TOL_API BStruct* NewStructSymbol(const BText& name, const BText& def);
TOL_API BStruct* FindStruct(const BText& name);

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BInt FieldCmp(const void*, const void*);


#endif // TOL_BSTRUCT_H
