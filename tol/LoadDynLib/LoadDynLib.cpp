/* LoadDynLib.cpp: A protocol for defining C++ built-in TOL entities over 
dynamic link libraries (.so, .dll) and an automatic cross-platform loading 
system which will be called from TOL.

This file implements the global built-in function used to load libraries.

            GNU/TOL Language.

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

#include "tol_LoadDynLib.h"
#include <tol/tol_bnameblock.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdir.h>

#include <ltdl.h>

typedef void* (*get_nb_t)(void);

static int  lt_dlinit_ =  lt_dlinit();

//--------------------------------------------------------------------
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
  BOperClassify* cl)
//--------------------------------------------------------------------
{
  BExternalOperator* opr = new BExternalOperator(
    "",gra,grammars,evaluator,min,max,args,desc,cl);
  opr->PutCppFile(file),   
  opr->PutName(name);
  BCode code;
  BUserCode* uCode = new BContensCode("", code, desc);
  uCode->Contens().PutOperator(opr);
  uCode->PutName(name);
  uCode->PutDescription(desc);
  nb.AddElement(uCode,true);
  return(uCode);
}

//--------------------------------------------------------------------
BGraContensP<BNameBlock>* NewUserNameBlock()
//--------------------------------------------------------------------
{
  return(new BGraContensP<BNameBlock>("", new BNameBlock));
}

//--------------------------------------------------------------------
class BLoadDynLib: public BExternalOperator
{
public:
  BLoadDynLib() : BExternalOperator
  (
    "LoadDynLib",GraNameBlock(),"Text Text", NIL,1,2,
    "(Text libraryPath [, Text libraryName])",
    I2("Returns a NameBlock that contains methods and members written "
    "in C + + in a precompiled library for dynamic linking. Once "
    "loaded Nameblock is nothing particular and can be used like "
    "any other object created with language TOL. See more details on ",
    "Devuelve un NameBlock que contiene métodos y miembros escritos "
    "en C++ en una biblioteca previamente compilada para su enlace "
    "dinámico. Una vez cargado el Nameblock no tiene nada de "
    "particular y puede ser usado como cualquier otro objeto creado "
    "con el lenguaje TOL. Vea más detalles en ")+"\n"
    "https://www.tol-project.org/wiki/LoadDynLib",
    BOperClassify::System_
  )
  { IncNRefs(); }
  BSyntaxObject* Evaluator(BList* argTrees) const;
};

//--------------------------------------------------------------------
BSyntaxObject* BLoadDynLib::Evaluator(BList* arg) const
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("LoadDynLib")) { return(NULL); }
  BText& libraryPath = Text(Car(arg));
  BText libraryName = GetFilePrefix(libraryPath);  
  if(Cdr(arg))
  {
    libraryName = Text(Car(Cdr(arg)));
  }
//Std(BText("\nTRACE BLoadDynLib::Evaluator 1 libraryPath=")+libraryPath);
//Std(BText("\nTRACE BLoadDynLib::Evaluator 2 libraryName=")+libraryName);
  BUserNameBlock* unb = NULL;

  lt_dlhandle handleLib;

  // abro la DynLib
  handleLib = lt_dlopen( libraryPath );
  if ( !handleLib ) 
  {
  //Std(BText("\nTRACE BLoadDynLib::Evaluator 3 lt_dlerror='")+lt_dlerror()+"'");
    BText reason( lt_dlerror( ) );
    Error(BText("[LoadDynLib] lt_dlerror:'")+reason+"'");
  } 
  else 
  {
    // si OK, pido el nameblock
    BText functionName( "GetDynLibNameBlock" );
    functionName += libraryName;
  //Std(BText("\nTRACE BLoadDynLib::Evaluator 4 functionName=")+functionName);
    get_nb_t getNameBlock =
      reinterpret_cast<get_nb_t>( lt_dlsym( handleLib, functionName ) );
    if ( !getNameBlock ) 
    {
    //Std(BText("\nTRACE BLoadDynLib::Evaluator 5 lt_dlerror='")+lt_dlerror()+"'");
      BText reason( lt_dlerror( ) );
      Error(BText("[LoadDynLib] lt_dlerror:'")+reason+"'");
    } 
    else 
    {
    //Std(BText("\nTRACE BLoadDynLib::Evaluator 6"));
      unb = reinterpret_cast<BUserNameBlock*>((*getNameBlock)());
      if ( !unb ) 
      {
      //Std(BText("\nTRACE BLoadDynLib::Evaluator 7"));
        Error(BText("[LoadDynLib]")+
              "No se pudo convertir a NameBlock: dynamic_cast<BUserNameBlock*>" );
      }
    }
  }
//Std(BText("\nTRACE BLoadDynLib::Evaluator 8"));
  //Al final hay que destruir la lista de argumentos antes de salir
  DESTROY(arg);
  return(unb);
}

#if !defined(USE_DELAY_INIT)
  static BLoadDynLib loadDynLib_;
#else
void * cloneBLoadDynLib_() { return new BLoadDynLib; }
static BLoadDynLib * loadDynLib_ = 
  (BLoadDynLib*)(__delay_init((void**)(&loadDynLib_), &cloneBLoadDynLib_));
#endif