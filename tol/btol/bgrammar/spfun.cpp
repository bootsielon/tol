/* spfun.cpp: Special functions of BGrammar of GNU/TOL language.
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

#include <tol/tol_bspfun.h>
#include <tol/tol_bout.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("spfun.cpp");

BSpFunHash BSpecialFunction::instances_;
BSpFunHash BSpecialFunction::leftInstances_;


//--------------------------------------------------------------------
// member functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BSpecialFunction::BSpecialFunction(const BText&       name,
				   const BText&       arguments,
				   const BText&       description,
				   BSpecialEvaluator  function)
//--------------------------------------------------------------------
    : name_	       (name),
      arguments_   (arguments),
      description_ (description),
      function_    (function)
{}


//--------------------------------------------------------------------
BSpecialFunction::~BSpecialFunction()
//--------------------------------------------------------------------
{
//  Std(BText("\n")+"\tDestroying(&"+TxtFmt(BInt(this),"%X")+","+name_+")");
}



//--------------------------------------------------------------------
BSyntaxObject* BSpecialFunction::Evaluate(      BGrammar* gra,
 					  const List*     tre,
                                                BBool     left) const
//--------------------------------------------------------------------
{
    return((*function_)(gra,tre,left));
}


//--------------------------------------------------------------------
BInt BSpFunOrderCriterium(const BAny vSpFun1, const BAny vSpFun2)
    
/*! Comparision function to sort by name in arrays of BToken*
 */
//--------------------------------------------------------------------
{
    BSpecialFunction* sym1 = *((BSpecialFunction**)vSpFun1);
    BSpecialFunction* sym2 = *((BSpecialFunction**)vSpFun2);
    BInt cmp = StrCmp(sym1->Name(), sym2->Name());
    return(cmp);
}

//--------------------------------------------------------------------
const BSpecialFunction* BSpecialFunction::Get(const BText& name)
//--------------------------------------------------------------------
{
  BSpFunHash::const_iterator found = instances_.find(name);
  if(found==instances_.end())
  {
    return(NULL);
  }
  else
  {
    return((BSpecialFunction*)found->second);
  }
}

//--------------------------------------------------------------------
const BSpecialFunction* BSpecialFunction::LeftGet(const BText& name)
//--------------------------------------------------------------------
{
  BSpFunHash::const_iterator found = leftInstances_.find(name);
  if(found==leftInstances_.end())
  {
    return(NULL);
  }
  else
  {
    return((BSpecialFunction*)found->second);
  }
}
