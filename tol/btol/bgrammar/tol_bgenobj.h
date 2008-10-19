/* tol_bgenobj.h: Template classes for generate grammatical classes
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

#ifndef TOL_BGENOBJ_H
#define TOL_BGENOBJ_H 1

#include <tol/tol_bgentmp.h>
#include <tol/tol_bsyntax.h>
#include <tol/tol_list.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
BBool InitGrammars ();


//--------------------------------------------------------------------
template <class Any>
class TOL_API BGraObject: public BSyntaxObject, public Any

/*! Base class to generate grammatic's objects of class Any.
 */
//--------------------------------------------------------------------
{
protected:
  static BGrammar* ownGrammar_;
  List*  tree_;
public:
  // Constructors & destructors: inline

  BGraObject()
  : BSyntaxObject(), Any(), tree_(NULL)
  {
    ownGrammar_->AddObject(this);
  }
  BGraObject(const BText& name)
  : BSyntaxObject(name), Any(), tree_(NULL)
  {
    ownGrammar_->AddObject(this);
  }

  BGraObject(const BText& name, const BText& desc)
  : BSyntaxObject(name,desc), Any(), tree_(NULL)
  {
    ownGrammar_->AddObject(this);
  }

 ~BGraObject()
  {
    ownGrammar_->DelObject(this);
    if(tree_) { tree_->Destroy(); }
  }


  // Access & manipulation: inline
    virtual bool IsBounded () const = 0;
    const List* GetOisTree    () const { return(tree_); }
    void PutOisTree (const List* tree) 
    { 
      if(!IsBounded()) 
      {
        if(tree) { tree_ = tree->duplicate(); }
        else     { tree_ = NULL; }   
      } 
    }
    
  BText Dump() const { return(Identify()); }
  BText Info() const { return(Identify()); }
  BInt  Mode() const { return(BOBJECTMODE); }

  BGrammar* Grammar() const 
      {
#       if defined(INIT_NULL_GRAMMAR)
	  return OwnGrammar();
#       else
	  return ownGrammar_;
#       endif
      }

  static BGrammar* OwnGrammar() 
      {
#       if defined(INIT_NULL_GRAMMAR)
	  if ( !ownGrammar_ )
	      InitGrammars();
#       endif
	  return ownGrammar_;
      }

  static BSyntaxObject* Casting      (BSyntaxObject*);
  static BSyntaxObject* FindConstant (const BText& name);
  static BDat Compare(const BSyntaxObject* obj1,const BSyntaxObject* obj2);
  static void  InitInstances();

  static BBool InitGrammar
  (
    int precedence, 
    const BText& nam, 
		const BText& des, 
		BCreator newRenamed, 
		BCreator newFunArg)
  {
	  if(!ownGrammar_)
	  {
	    ownGrammar_=new BGrammar
      (
        precedence,
        nam,
        des,
        false,
        Casting,
        FindConstant,
        newRenamed, 
        newFunArg,
        Compare
      );
      InitInstances();
	  }
	  return(BTRUE);
      }

  void ReCalc () 
      { flags_.calculated_ = BFALSE; CalcObject(); }

  virtual void CalcObject() 
      {};

  void DoCalcObject()
      {
	  flags_.calculating_ = BTRUE;
	  CalcObject();
	  flags_.calculating_ = BFALSE;
      };

  void ReadData()
      {
	  if(!flags_.calculating_)
	  {
	      if(!flags_.calculated_) { DoCalcObject(); }
	      flags_.calculated_ = BTRUE;
	  }
      }
};


#endif // TOL_BGENOBJ_H
