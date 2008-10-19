/* tol_bsetgra.h: Classes and functions for set grammar
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

#ifndef TOL_BSETGRA_H
#define TOL_BSETGRA_H 1

#include <tol/tol_bdate.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bgencon.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserSet       BGraContensBase  < BSet >
#define BContensSet    BGraContens      < BSet >
#define BSystemSet     BGraConstant     < BSet >
#define BSetTemporary  BTmpContens      < BSet >

template class TOL_API BGraContensBase < BSet >;
template class TOL_API BGraContens     < BSet >;
template class TOL_API BTmpObject      < BUserSet >;
template class TOL_API BTmpContens     < BSet >;

/*
  Give warning C4661: no suitable definition provided for explicit template instantiation request

template class BGraContens  <BSet>;
template class BGraConstant <BSet>;
template class BTmpContens  <BSet>;
*/


// Tcl_Eval hook

typedef int (*Tcl_EvalPtr)(char * script, const char ** result);
typedef int (*Tcl_EvalExPtr)(const BSet&, const char ** result);

TOL_API Tcl_EvalPtr InstallTclEval(Tcl_EvalPtr);
TOL_API Tcl_EvalExPtr InstallTclEvalEx(Tcl_EvalExPtr);

#ifdef UNIX
  typedef BInt (*BTerminator)();
#else
  extern "C"
  {
#ifdef _MSC_VER
    typedef  BInt (__stdcall * BTerminator)();
#else
    typedef BInt _stdcall (*BTerminator)();
#endif
  }
#endif


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Set);
class BTsrPrimary;

//--------------------------------------------------------------------
/*! This class provides contens storement and Fixed Name and
 *  Description (FND) for BSet objects created from a robbed list, 
 *  and specifying the eventual structure and subtype. 
 *  This is usefull to returned objects by built-in functions saving 
 *  a lot of memory.
 */
  template <char* Name_, char* Description_>
  class TOL_API BGraContensFNDSet: 
    public BGraContensFND<BSet, Name_, Description_>
//--------------------------------------------------------------------
{
public:
  BGraContensFNDSet(BList* elem, BStruct* str, BSet::BSubType sub) 
  : BGraContensFND<BSet, Name_, Description_>() 
  {  
      this->Contens().RobStruct(elem, str, sub);
  }

 ~BGraContensFNDSet() {};
};

//This macro creates a BGraContensFND object using constant strings
//declared with FNDParam1 or FNDParam (see tol_bgencon.h)
#define PrefixNewSetFND(PREFIX_, NAME, ELEM, STR, SUB) \
  new BGraContensFNDSet< _##PREFIX_##NAME##_FND_Name, _##PREFIX_##NAME##_FND_Desc>(ELEM, STR, SUB)

//--------------------------------------------------------------------
class BSetTuple: public BContensSet
//--------------------------------------------------------------------
{
public:
  BSetTuple():BContensSet()	    {}
  BSetTuple(const BText& name):BContensSet(name,BSet::Unknown())	    {}
  BSetTuple(const BText& name, const BSet& contens):BContensSet(name, contens) {}
  BSet& Contens()     { return(contens_); }
  void	CalcContens() {}
  void ReCalc() {}
  RedeclareClassNewDelete(BSetTuple);
};


//--------------------------------------------------------------------
inline BUserSet* NewSet(const BText& name, const BText& uName,
		                 BList* elem, BStruct* str, BSet::BSubType sub)
/*! This function provides contens storement and NON-Fixed Name and
 *  Description (FND) for BSet objects created from a robbed list, 
 *  and specifying the eventual structure and subtype. 
 *  This is usefull to returned objects by built-in functions 
 */
//--------------------------------------------------------------------
{
  BSetTuple* result = new BSetTuple(name, BSet::Unknown());
  result->PutDescription(uName);
  result->Contens().RobStruct(elem, str, sub);
  return(result);
}

//--------------------------------------------------------------------
inline BUserSet* NewSet(BList* elem, BStruct* str, BSet::BSubType sub)
/*! This function provides contens storement and NON-Fixed Name and
 *  Description (FND) for BSet objects created from a robbed list, 
 *  and specifying the eventual structure and subtype. 
 *  This is usefull to returned objects by built-in functions 
 */
//--------------------------------------------------------------------
{
  BSetTuple* result = new BSetTuple();
  result->Contens().RobStruct(elem, str, sub);
  return(result);
}

//--------------------------------------------------------------------
class BSetOfType: public BSetTemporary

/*! Construct a set by enumeration of its elements
 */
//--------------------------------------------------------------------
{
public:
  BSetOfType(BList* arg) : BSetTemporary(arg) {}
 ~BSetOfType() {}
  void CalcContens();
  RedeclareClassNewDelete(BSetOfType);
};
DeclareEvaluator(BSetOfType);


class BSetFromFile;
class BSourcePath;

template class TOL_API BArray<const BSourcePath*>;


//--------------------------------------------------------------------
class TOL_API BSourcePath: public BSyntaxObject
//--------------------------------------------------------------------
{
private:
  static BArray<const BSourcePath*> statusTable_;
  static const BSourcePath* current_;
  const BSetFromFile* set_;

  void PutSet(const BSetFromFile* set) { set_ = set; }

public:
  BSourcePath(const BSetFromFile* set, const BText& path);
 ~BSourcePath() {}

	BInt Mode() const;
	BGrammar* Grammar () const { return(GraAnything()); }
  const BSetFromFile* Set() const { return(set_); }

  void SetDestroyed();
  static int GetItemNumber() { return(statusTable_.Size()); } 
  static const BSourcePath* GetItem(int n) { return(statusTable_[n]); } 
  static void SetCurrent(const BSourcePath*  cp);
  static void SetCurrent(const BSetFromFile* set);
  static const BSourcePath*	Current();
  static const BSourcePath*	Find(const BText& path);
  static bool IsAlive(const BSourcePath* sp);
  DeclareClassNewDelete(BSourcePath);
};

//--------------------------------------------------------------------
class TOL_API BSetFromFile: public BSetTemporary

/*! Construct a set from a file
 */
//--------------------------------------------------------------------
{
protected:
  static BTerminator  terminator_;
  static BList*	      compiled_;
  static BInt	        nSetFromFile_;
  BText  tolPath_;
#if defined(_MSC_VER)
#pragma warning( disable : 4251)
#endif

  ifstream file_;

#if defined(_MSC_VER)
#pragma warning( default : 4251)
#endif

  BBool	   busy_;

  const BSourcePath* oldSourcePath_;
  const BSourcePath* curSourcePath_;
  BList* makeGlobal_;

public:

  BSetFromFile(BList* arg);
  BSetFromFile(const BText& path);
 ~BSetFromFile();

#ifndef FOR_BTOLLNK
  BBool    IsFile      () const { return(BTRUE); }
#endif

  BInt BinWrite();
  BInt BinRead ();

  static BInt NSetFromFile() { return(nSetFromFile_); }
  const BText& TolPath() const { return(tolPath_); }
  BBool	    Open       ();
  void	    Close      ();
  BBool	    IsOpen     () { return((BBool)file_.good()); }
  ifstream& File       () { return(file_); }
  BBool	    Busy       () { return(busy_); }
  void AddMakeGlobal(BSyntaxObject* obj) const;
  static BBool Terminate()
  { return((terminator_)?(BBool)((*terminator_)()):BFALSE); }

  static const BSetFromFile*	FindCompiled (BInt n);
  static const BSetFromFile*	FindCompiled (const BText& path);
  static BSyntaxObject* FindObject   (const BText& tolPath);
  static void		TableInfo    (const BText& tolPath,
				      const BText& fileName);
  static void		PutTerminator(BTerminator t) { terminator_ = t; }
  static BList * Compiled() { return compiled_; }
  RedeclareClassNewDelete(BSetFromFile);
};
DeclareEvaluator(BSetFromFile);



//--------------------------------------------------------------------
class BSetIncludeBDT: public BSetFromFile

/*! Construct a set from a file
 */
//--------------------------------------------------------------------
{
private:
  static BDat fillValue_;

  BArray<BTsrPrimary*> serie_;
  BBool		       enableWarning_;
  BText		       header_;
  BDate		       first_;
  BDate		       last_;
  BChar		       sep_;
  BBool		       hasHeader_;
public:
  static BDat& FillValue() { return(fillValue_); }

  BSetIncludeBDT(BList* arg)
  : BSetFromFile(arg),
    enableWarning_(BFALSE),
    first_(),
    last_(),
    sep_(';'),
    hasHeader_(BTRUE)
  {
  }

 ~BSetIncludeBDT() { }
  void CalcContens();
  void ReadData();
  RedeclareClassNewDelete(BSetIncludeBDT);
};
DeclareEvaluator(BSetIncludeBDT);

//--------------------------------------------------------------------
class BSetWriteSet: public BSetTemporary
{
public:
    BSetWriteSet(BList* arg=NIL)
	: BSetTemporary(arg) 
	{ 
	    CalcContens(); flags_.calculated_= BTRUE; 
	}
    void CalcContens();
  RedeclareClassNewDelete(BSetWriteSet);
};
DeclareEvaluator(BSetWriteSet);

//--------------------------------------------------------------------
// extern functions
//--------------------------------------------------------------------
TOL_API BText FindCompiled(const BText&   fileName);
TOL_API BUserSet* IncludeFile(const BText&   fileName);
BUserSet* NewSet(const BText& name,
		 const BText& uName,
		 BList* elem,
		 BStruct* str,
		 BSet::BSubType sub);



#endif // BSETGRAMMAR


