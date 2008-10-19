/* tol_bvmatgra.h: Virtual Matrix Grammar's
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   BVMat later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT BVMat WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef TOL_BVMATGRA_H
#define TOL_BVMATGRA_H 1

#include <tol/tol_bgencon.h>
#include <tol/tol_bvmat.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserVMat      BGraContensBase < BVMat >
#define BContensVMat   BGraContens     < BVMat >
#define BSystemVMat    BGraConstant    < BVMat >
#define BVMatTemporary BTmpContens     < BVMat >


//CastingsDeclaration(BVMat);

inline BUserVMat* UVMat(BSyntaxObject* arg)			   
{								   
  BUserVMat* uVMat = NIL;					   
  if(arg && (arg->Grammar()==BUserVMat::OwnGrammar()))		   
  { uVMat = BCast<BUserVMat,BSyntaxObject>::c(arg); }				   
  return(uVMat);						   
}								   
								   
inline BUserVMat* UVMat(BCore* arg)				   
{								   
  return(UVMat(BCast<BSyntaxObject,BCore>::c(arg)));				   
}								   
								   
inline BUserVMat* UVMat(const BText& expression)		   
{								   
  return(UVMat(BUserVMat::OwnGrammar()->EvaluateExpr(expression)));  
}								   
								   
TOL_API BVMat& BVMatUnKnown();				   
inline BVMat& VMat (BSyntaxObject* arg)				   
{								   
  if(arg && (arg->Grammar()==BUserVMat::OwnGrammar()))		   
  {								   
    BUserVMat*	 uVMat = UVMat(arg);				   
    return(uVMat->Contens());					   
  }								   
  else								   
  {								   
      return(BVMatUnKnown());				   
  }								   
}								   
								   
inline BVMat& VMat (BCore* arg)					   
{								   
  return(VMat(UVMat(arg)));					   
}								   
								   
inline BVMat& VMat (const BText& expr)				   
{								   
  return(VMat(UVMat(expr)));					   
}

//--------------------------------------------------------------------
// extern functions
//--------------------------------------------------------------------

#endif // TOL_BVMATGRA_H
