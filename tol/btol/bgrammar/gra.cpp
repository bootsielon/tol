/* gra.cpp: Member functions of BGrammar and subclasses
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

#include <tol/tol_bgrammar.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bout.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bsymboltable.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BGrammar and subclasses).
//--------------------------------------------------------------------
BTraceInit("gra.cpp");
BGidHashByName BGrammar::gidByName_; 
BGraHashByName BGrammar::graByName_;
BGraHashByGid  BGrammar::graByGid_;
bool           BGrammar::stopFlag_      = false;
BText          BGrammar::editor_	      = "notepad ";
BList*         BGrammar::instances_     = NULL;
BGrammar*      BGrammar::last_          = NULL;
BDbiFinder     BGrammar::dbiFinder_     = NULL;
BInt           BGrammar::numGrammar_    = 0;
BSymbolTable*  BGrammar::symbolTable_       = NULL;
BInt           BGrammar::level_	        = 0;
#ifndef NDEBUG
BDat	         BGrammar::unparseNodes_  = 2;
#else
BDat           BGrammar::unparseNodes_  = 0;
#endif

short BStackManager::BDictionaryNode::charPos_ [256];
char  BStackManager::BDictionaryNode::allowed_ [numAllowedChar_];
int   BStackManager::currentEntries_ = 0;

BStackManager::BStackNode*      BStackManager::stack_ = NULL;
BStackManager::BDictionaryNode* BStackManager::root_  = NULL;


//------------------------------
#ifndef __USE_DYNSCOPE__
BGrammar::level_list_t BGrammar::stack_level;
#endif /* __USE_DYNSCOPE__ */
//------------------------------

BAnyGrammar*  BAnyGrammar::anyGrammar_ = NIL;

static int BG_called = 0;


//--------------------------------------------------------------------
static bool BGrammar_IsInitialized()
//--------------------------------------------------------------------
{
  static bool initialized_ = false;
  if(initialized_) { return(true); }
  initialized_ = true;
  return(false);
}


//--------------------------------------------------------------------
bool BGrammar::Initialize()
//--------------------------------------------------------------------
{
  if(BGrammar_IsInitialized()) { return(false); }
  SetEmptyKey(gidByName_, NULL);
  SetEmptyKey(graByName_, NULL);
  SetEmptyKey(graByGid_,  BGI_NotFound);
  gidByName_["Anything" ]=BGI_Anything;
  gidByName_["Real"     ]=BGI_Real;
  gidByName_["Complex"  ]=BGI_Complex;
  gidByName_["Matrix"   ]=BGI_Matrix;
  gidByName_["Date"     ]=BGI_Date;
  gidByName_["TimeSet"  ]=BGI_TimeSet;
  gidByName_["Serie"    ]=BGI_Serie;
  gidByName_["Set"      ]=BGI_Set;
  gidByName_["Polyn"    ]=BGI_Polyn;
  gidByName_["Ratio"    ]=BGI_Ratio;
  gidByName_["Text"     ]=BGI_Text;
  gidByName_["Code"     ]=BGI_Code;
  gidByName_["NameBlock"]=BGI_NameBlock;
  gidByName_["VMatrix"  ]=BGI_VMatrix;

  symbolTable_ = new BSymbolTable; 

  return(true);
}

//--------------------------------------------------------------------
BGrammar::BGrammar(int           precedence,
                   const BText&  name,
		               const BText&  description,
		               bool          autoContens,
		               BCasting      cast,
		               BFinderByName fC,
		               BCreator      newRenamed,
		               BCreator      newFunArg,
		               BCompare      compare)

/*! BGrammar constructor. Puts the name of the grammar and adds it to
 *  the list of instances if already doesn't exist another grammar with
 *  the same name.
 * \param name
 * \param description
 * \param cast
 * \param fC
 * \param nD1
 * \param compare
 */
//--------------------------------------------------------------------
  : BSyntaxObject(name, description),
    precedence_(precedence),
    autoContens_(autoContens),
    methods_(NIL),
    sortedMethods_(),
    castings_(),
    castingFunction_(cast),
    findConstant_(fC),
    newRenamed_(newRenamed),
    newFunArg_ (newFunArg),
    compare_(compare),
    defect_(NIL)
{
  Initialize() ;
  castings_.ReallocBuffer(BGI_LastId);
  castings_.ReallocBuffer(0);
  for(int i=0; i<BGI_LastId; i++)
  {
    hasCasting_[i]=false;
  }
  gid_ = BGI_NotFound;
  BGidHashByName::const_iterator found = gidByName_.find(name);
  if(found!=gidByName_.end())
  {
    gid_ = (BGrammarId)found->second;
  }
  else
  { 
    Error(Out()+"[FATAL]"+name+
          I2(" is not an allowed grammar. TOL cannot start.",
             " no es una gramática admisible. TOL no puede arrancar.")); 
    abort();
  }

  if (!BG_called) {
    BG_called = 1;
    BGrammar::instances_ = NULL;
  }
  PutSystem(true); // inherited from BSyntaxObject
  if(Exists(name)) {
    Error(Out() + I2("Duplicate grammar "+Name(),
		     "Gramatica "+ Name()+" duplicada"));
  } else {
    BGrammar::instances_ = LstAppend(BGrammar::instances_, (BAtom*)this);
    graByName_[Name()] = this;
    graByGid_ [gid_  ] = this;
  //Std(BText("\nBGrammar added with name '")+name+"' and GID "+(int)gid_);
  }
  numGrammar_++;
  
  if(numGrammar_>=MAXGRAMMAR) {
    Error(I2("Number of grammars exceding ",
	     "Numero de gramáticas mayor que") + BInt(MAXGRAMMAR));
  }
  //Std(BText("\nCreated grammar ") + name);
}


//--------------------------------------------------------------------
BGrammar::~BGrammar()

/*! BGrammar destructor.
 */
//--------------------------------------------------------------------
{
//Clean();
//Trace(BText("Destroying grammar ")+Name());
}

//--------------------------------------------------------------------
BInt BGrammar::Level()
//--------------------------------------------------------------------
{ 
  assert(level_>=0); 
  return(level_); 
}

//--------------------------------------------------------------------
void BGrammar::IncLevel() 
//--------------------------------------------------------------------
{
  assert(level_>=0); 
  level_++; 
//Std(BText("\nIncLevel -> ")+level_);
}

//--------------------------------------------------------------------
void BGrammar::DecLevel() 
//--------------------------------------------------------------------
{ 
  level_--; 
  assert(level_>=0); 
//Std(BText("\nDecLevel -> ")+level_);
}

//--------------------------------------------------------------------
BInt BGrammar::PutLevel(BInt level)
//--------------------------------------------------------------------
{
  assert(level_>=0);
  BInt old = level_;
  level_=level;
  assert(level_>=0);
//Std(BText("\nPutLevel -> ")+level_);
  return(old);
}


//--------------------------------------------------------------------
void BGrammar::Clean()
//--------------------------------------------------------------------
{
    BList* lstMet = methods_;
    while(lstMet)
    {
	((BMethod*)Car(lstMet))->DestroyFunction();
	lstMet = Cdr(lstMet);
    }
    methods_    = LstDestroyAllCons(methods_);
}


//--------------------------------------------------------------------
BBool BGrammar::End()
//--------------------------------------------------------------------
{
    return(BTRUE);
}


