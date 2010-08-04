/* tol_LoadDynLib.h: A protocol for defining C++ built-in TOL entities over 
dynamic link libraries (.so, .dll) and an automatic cross-platform loading 
system which will be called from TOL.

This file implements the macroes needed to create C++ built-in libraries.


                   GNU/TOL Language.

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

#ifndef TOL_LoadDynLib_H
#define TOL_LoadDynLib_H 1

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_boper.h>
#include <tol/tol_bcodgra.h>

TOL_API BUserCode* NewMethod(
  BNameBlock& nb,
  const BText& file,
  const BText& name,
  BGrammar* gra,
  const BText& grammars,
  BEvaluator evaluator,
  BInt min,
  BInt max,
  const BText& args,
  const BText& desc,
  BOperClassify* cl       );

TOL_API BGraContensP<BNameBlock>* NewUserNameBlock();

#define DefMethod(ORD,CLASS,NAME,MINARG,MAXARG,LISTGRA,LISTARGS,DES,CL)  \
static BUserCode * new_local_method_##CLASS = NewMethod(			  \
    LOCAL_NAMEBLOCK, \
    __FILE__, \
    NAME,								   \
    CLASS::OwnGrammar(),						   \
    LISTGRA,								   \
    CLASS##Evaluator,							   \
    MINARG,								   \
    MAXARG,								   \
    LISTARGS,								   \
    DES,							   \
    CL									   \
  ); 

#define DefMember(CLASS,PARAMCLASS,NAME,DESCRIPTION,EXPRESSION)              \
  static void * add_local_member_##NAME () {                                 \
  BSyntaxOject* opr = NewExternalOperator  	                                 \
    ExtOprConstructor(CLASS,"",MINARG,MAXARG,LISTGRA,LISTARGS,DESC,CL);      \
  opr->PutCppFile(__FILE__);                                                 \
  opr->PutName(NAME);                                                        \
  LOCAL_NAMEBLOCK.AddElement(opr,True)                                       \
  return opr;                                                                \
}                                                                            \
static BExternalOperator* local_method_##NAME = add_local_method_##NAME(); 

#endif // TOL_LoadDynLib_H

