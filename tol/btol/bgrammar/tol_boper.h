/* tol_boper.h: Classes and functions related with BOperator
                of GNU/TOL language.

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

/*! \file tol_boper.h
//  \brief Defines BOperClassify, BOperator, BEqualOperator,
    \brief BUserFunctionCreator, BStructCreator, BStandardOperator,
    \brief BInternalOperator, BExternalOperator, BNewStruct and
    \brief BUserFunction classes.
*/
/*! \class BOperClassify
//  \brief Class to classify the operators.
//  \brief Funcionality has been implemented in opr.cpp and oprimp.cpp
*/ 
/*! \class BOperator
//  \brief Class to derive operators of any grammar.
//  \brief Funcionality has been implemented in opr.cpp and oprimp.cpp
*/ 

#ifndef TOL_BOPER_H
#define TOL_BOPER_H 1

#include <tol/tol_delay_init.h>
#include <tol/tol_bexc.h>
#include <tol/tol_blist.h>
#include <tol/tol_list.h>
#include <tol/tol_bobject.h>
#include <tol/tol_bsyntax.h>

// From C++ STL
#include <vector>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
typedef BSyntaxObject* (*BEvaluator)(BList* args);
typedef BSyntaxObject* (*BEvaluate) (const List* args);

template class TOL_API BArray<BAtom*>;

class BUserFunCode;
class BStruct;
class BClass;

//--------------------------------------------------------------------
class  TOL_API BOperClassify: public BObject

/*! Classifies the operators
 */
//--------------------------------------------------------------------
{
protected:
  //! Classify of operators
  BList* operators_;	   
  BText	 description_;
  //! Classes
  static BList* instances_; 
  static BArray<BAtom*> sortedTheme_;
public:
  static BOperClassify* Various_;
  static BOperClassify* General_;
  static BOperClassify* System_;
  static BOperClassify* FileIncluding_;
  static BOperClassify* Text_;
  static BOperClassify* RealArythmetic_;
  static BOperClassify* RealArythmetic_GSL_;
  static BOperClassify* ComplexArythmetic_;
  static BOperClassify* NumericalAnalysis_;
  static BOperClassify* Logic_;
  static BOperClassify* SetAlgebra_;
  static BOperClassify* Statistic_;
  static BOperClassify* Probability_;
  static BOperClassify* Sthocastic_;
  static BOperClassify* Conversion_;
  static BOperClassify* MatrixAlgebra_;
  static BOperClassify* RetardPolynomial_;
  static BOperClassify* TimeAlgebra_;
  static BOperClassify* CalendarVariables_;
  static BOperClassify* TimeSeriesAlgebra_;
  static BOperClassify* TimeSeriesLogic_;
  static BOperClassify* BayesDataBase_;
  static BOperClassify* Extension_;
  static BOperClassify* DataMining_;

public:
  // Constructors and destructors: opr.cpp
  //! BOperClassify constructor
  BOperClassify(const BText& name, const BText& desc);

  ~BOperClassify() {}

  BInt Mode() const { return(BNOMODE); }
  const BText Description() const;
  //! Adds a operator to the classify
  void Add(BSyntaxObject* opr);
  //! Removes a operator from the classify
  void Del(BSyntaxObject* opr);

  BList* Operators();

  static BOperClassify* Find(const BText& className);
  static BList*	Instances();
  static void		InitInstances();
};

template class TOL_API BArray<BText>;
template class TOL_API BTextCell;

//--------------------------------------------------------------------
class TOL_API BOperator: public BSyntaxObject

/*! Basic class to derive operators of any grammar.
 *  An operator is of type G if its result type is of type G.
 */
//--------------------------------------------------------------------
{
private:
  BUserFunCode* uCode_;
protected:
  //! Grammar of the result of evaluation
  BGrammar*	grammar_;   
  //! Classify of operators
  BOperClassify* theme_; 
  //! Arguments
  BTextCell	argTable_;
  //profiler store 
  void* profiler_;

  BText cppFile_;

  void AddSystemOperator();

public:

  // Constructors and destructors: opr.cpp
  BOperator(const BText& name, BGrammar* gra, const BText& desc = "",
	    BOperClassify* cl = NIL);
 ~BOperator();

  BInt Mode() const { return BBUILTINFUNMODE; };
  void* GetProfiler()          { return(profiler_); }
  void  PutProfiler(void* prf) { profiler_=prf; }

  const BText& CppFile() const;
  void PutCppFile(const BText& cppFile);

  //! Gets operator's grammar
  BGrammar*	 Grammar	  () const { return(grammar_); }
  //! Gets operator's classify
  BOperClassify*	 Theme	  () const { return(theme_); }
  //! Returns a text with all important information about the operator
  BText		 Dump	   () const;
  //! Returns the name of the operator's classify
  BText		 ThemeName () const;
  void		 PutTheme(BOperClassify* cl);

  // Implementation : oprimp.cpp
  //! Method to evaluate the operator
  virtual BSyntaxObject* Evaluate  (const List* arg)	    = 0;

  virtual BText		   Arguments	   ()       const { return("(...)"); }
  virtual BGrammar*	 GrammarForArg (BInt n) const { return(grammar_); }
  virtual BInt       NumOptForArg  (BInt n) const { return(1); }
  virtual BGrammar*	 GrammarForArg (BInt n, BInt k) const { return(grammar_); }
  virtual BInt		   MinArg	       ()       const { return(1); }
  virtual BInt		   MaxArg	       ()       const { return(1); }
  virtual BInt		   NumArg	       ()       const { return(1); }

  BUserFunCode* GetCode();
  void PutCode(BUserFunCode* uc);

  const BTextCell& ArgTable	  () const { return(argTable_);}

  friend BBool Compare(BOperator** ope1, BOperator** ope2);

};


//--------------------------------------------------------------------
class TOL_API BEqualOperator: public BOperator

/*! Basic class to manage equal operator =.
 */
//--------------------------------------------------------------------
{
public:
  static BText creatingName_;
  static BText currentFatherName_;
  static bool  isCreatingNameBlock_;
  static BText currentFullName_;
  static const BClass* creatingClass_;

  // Constructors and destructors: opr.cpp
  //! BEqualOperator constructor
  BEqualOperator(BGrammar* gra);
  //! BEqualOperator destructor
  ~BEqualOperator() {}

  // Implementation : oprimp.cpp
  friend BGrammar* GetLeft(BGrammar*, const BList*, BText&);
  //! Evaluate BEqualOperator operator. Creates a new object.
  BSyntaxObject* Evaluate (const List*) ;

  static const BText&  CreatingName () { return(creatingName_); }
  static const BText&  CurrentFatherName () { return(currentFatherName_); }
  static const bool IsCreatingNameBlock () { return(isCreatingNameBlock_); }
  static const BText&  CurrentFullName () { return(currentFullName_); }
  static const BClass* CreatingClass() { return(creatingClass_); }
  static void CleanCreating() { creatingName_=""; creatingClass_=NULL; }

};


//--------------------------------------------------------------------
class TOL_API BUserFunctionCreator: public BOperator
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: opr.cpp
  BUserFunctionCreator(BGrammar* gra);
  ~BUserFunctionCreator() {}

  // Implementation : oprimp.cpp
  //! Evaluate BUserFunctionCreator operator. Creates a new function
  BSyntaxObject* Evaluate (const List*);
};


//--------------------------------------------------------------------
class TOL_API BStructCreator: public BOperator
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: opr.cpp
  BStructCreator(const BText& name, BGrammar* gra);
  ~BStructCreator() {}

  // Implementation : oprimp.cpp
  //! Evaluate BStructCreator operator. Creates a new struct
  BSyntaxObject* Evaluate (const List*);
};

template class TOL_API BArray<BStruct*>;

//--------------------------------------------------------------------
class TOL_API BStandardOperator: public BOperator

/*! This operators are defined by C++ functions and classes.
 *  For every operator of a grammar G exists one and only one 
 *  temporary class of type G that implements the methods of this
 *  type for the operator's return result.
 *  Thus we can identify an operator class with its corresponding
 *  operational class C. To rebound this feature, we will declare it
 *  as a nested class of C, that's also subclas of BOperator.
 *  This class will must to have an only instance, that could be a
 *  static member of C.
*/
//--------------------------------------------------------------------
{
protected:
  BInt	     minArg_;	  //!< Minimum of arguments to be operated
  BInt	     maxArg_;	  //!< Maximum of arguments to be operated
  BInt	     numArg_;	  //!< Actual number of arguments to be operated
  BText	     arguments_;
  BEvaluator evaluator_;
  BArray<BStruct*>  structs_;	//!< Structures.
  BArray<BClass*>   classes_;	//!< Classes.
public:
  static bool           evaluatingFunctionArgument_;
  // Constructors and destructors: opr.cpp
  BStandardOperator(const BText& name, BGrammar* gra,
		    BEvaluator evaluator, BInt minArg, BInt maxArg,
		    const BText& args = "",
		    const BText& desc = "",
		    BOperClassify* cl = NIL);
  ~BStandardOperator() {}

  // Access & Manipulation: inline
  BInt	MinArg	     () const { return(minArg_); }
  BInt	MaxArg	     () const { return(maxArg_); }
  BInt	NumArg	     () const { return(numArg_); }
  BText Arguments    () const { return(arguments_); }
  BText Dump	       () const;
  // Implementation : oprimp.cpp
  BSyntaxObject* EvaluateArgument(BGrammar*gra, const List* branch);
  virtual BSyntaxObject* Evaluator    (BList* arg) const
  { return((*evaluator_)(arg)); }
  //! Evaluate BStandardOperator operator. Evaluates argument-trees
  BSyntaxObject* Evaluate (const List*);
};


//--------------------------------------------------------------------
class TOL_API BInternalOperator: public BStandardOperator

/*! Basic class to derive internal operators of any grammar.
 *  The return result and the arguments of an external operator in an
 *  grammar G must be of type G.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors:
  BInternalOperator(const BText& nam, BGrammar* gra,
		    BEvaluator evaluator, BInt min, BInt max,
		    const BText& args = "",
		    const BText& desc = "",
		    BOperClassify* cl = NIL);
  ~BInternalOperator() {}

  // Virtual functions redefinition: inline
  BGrammar* GrammarForArg(BInt n) const
  { return(((maxArg_>0) && (n>maxArg_)) ? NIL : grammar_); }
  DeclareClassNewDelete(BInternalOperator);
};

template class TOL_API BArray<BGrammar*>;

//--------------------------------------------------------------------
class TOL_API BExternalOperator: public BStandardOperator

/*! Basic class  to derive external operators of any grammar.
 *  The return result of an external operator in an grammar G
 *  must be of type G, but his arguments can be of any type.
 */
//--------------------------------------------------------------------
{
protected:
  //! Grammars to evaluate the corresponding arguments.
  BArray< BArray<BGrammar*> >    grammars_; 
public:
  // Constructors and destructors:
  BExternalOperator(const BText& name,
		    BGrammar* gra,
		    BList* grammars,
		    BEvaluator evaluator = NIL,
		    BInt min = 0,
		    BInt max = 0,
		    const BText& args = "",
		    const BText& desc = "",
		    BOperClassify* cl = NIL);
  BExternalOperator(const BText& name,
		    BGrammar* gra,
		    const BText& grammars,
		    BEvaluator evaluator = NIL,
		    BInt min = 0,
		    BInt max = 0,
		    const BText& args = "",
		    const BText& desc = "",
		    BOperClassify* cl = NIL);
  ~BExternalOperator() {}

  // Virtual functions redefinition: inline
  BGrammar* GrammarForArg(BInt n) const;
  BInt       NumOptForArg  (BInt n) const;
  BGrammar*	 GrammarForArg (BInt n, BInt k) const;

  DeclareClassNewDelete(BExternalOperator);
};


//--------------------------------------------------------------------
class TOL_API BNewStruct: public BExternalOperator
//--------------------------------------------------------------------
{
private:
  BStruct* struct_;

public:
  BNewStruct(BStruct& str);
  ~BNewStruct() {}

  static BText StructGrammars(BStruct& str);
  static BText StructArguments(BStruct& str);
  BSyntaxObject* Evaluator(BList* argTrees) const;
  BSyntaxObject* Evaluate (const List* arg);
  RedeclareClassNewDelete(BNewStruct);
};


template class TOL_API BArray<BSyntaxObject*>;


//--------------------------------------------------------------------
class BUserFunction: public BExternalOperator
//--------------------------------------------------------------------
{
 protected:
    List* declaration_;
    List* definition_;
    BText declare_;
    BText define_;

    BText inFile_; //!< File where this BUserFunctions has been defined
    static std::vector<BUserFunction*> activeFunctions_; //!< currently active
    
    BArray<BSyntaxObject*> lastCalling_;
    BArray<BText> names_;

 public:
    const BClass* staticOwner_;

    // Constructors and destructors: opr.cpp
    BUserFunction(const BText& name, BGrammar* gra);
    BUserFunction(const BText& name, BGrammar* gra,
		  const BText& dec, const BText& def);
    ~BUserFunction() { Clean(); }
    
    static BUserFunction* ActiveFunction();
    static void AddActiveFunction(BUserFunction* uFunction);
    static void RemoveActiveFunction();
	  static void ShowCallStack();
	  static BText GetCallStack();

    BInt Mode() const { return BUSERFUNMODE; };

    BText  FullName() const;
    BText  Dump    () const;
    
    const List*  Declaration() const { return(declaration_); }
    const List*  Definition () const { return(definition_ ); }

    const BText& Declare()    const { return(declare_); }
    const BText& Define()     const { return(define_); }

          BBool  SetExpression(const BText&, const BText&);
          BBool  SetExpression(const List*, const List*);
    
	  void   SetInFile(const BText&);
    const BText& InFile() const { return inFile_; }

    BSyntaxObject* Evaluate(const List* argTrees);
    //! Evaluates 
    BSyntaxObject* Evaluator(BList*)     const;
    BSyntaxObject* FastEvaluator(BList*) const;
    BBool          Compile();
    void           Clean();
    BSyntaxObject* LastCalling(BInt n) { return(lastCalling_[n]); }
    const BText&    ArgName(BInt n)    { return(names_[n]); }

    BUserFunction* Replicate() const;
    RedeclareClassNewDelete(BUserFunction);
};


//--------------------------------------------------------------------
// macros for BInternalOperator
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//! Macro that declares a generic evaluator function for internal classes
//--------------------------------------------------------------------
#define DeclareEvaluator(CLASS)			                   \
  inline BSyntaxObject* CLASS##Evaluator(BList* args) {            \
    try {                                                          \
        return((BSyntaxObject*)(new CLASS(args)));                 \
    } catch(BException &exc) {                                     \
	Std(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));     \
	return NIL;                                                \
    } catch(...) {                                                 \
	return NIL;                                                \
    }                                                              \
}

//--------------------------------------------------------------------
//! Macro that declares an specific evaluator function for TimeSet
//--------------------------------------------------------------------
#define DeclareTimeSetEvaluator(CLASS)		                           \
  inline BSyntaxObject* CLASS##Evaluator(BList* args) {                    \
    try {                                                                  \
        return((BSyntaxObject*)(new CLASS(args)));                         \
    } catch(TimeSetException& exc) {                                       \
	Warning(I2(exc.enReason_.c_str(), exc.esReason_.c_str()));         \
        Std(I2("Object created as W (empty TimeSet)",                      \
	       "Objeto creado como W (el TimeSet vacío)"));                \
	return BGrammar::FindByGid(BGI_TimeSet)->FindVariable("W");                                               \
    } catch(...) {                                                         \
        Warning(I2("Wrong arguments. Object created as W (empty TimeSet)", \
                   "Argumentos erróneos. Objeto creado como W (conjunto"   \
                   " temporal vacío)"));                                   \
                   return BGrammar::FindByGid(BGI_TimeSet)->FindVariable("W");                                               \
    }                                                                      \
}


#define DeclareNoLazzyEvaluator(CLASS)	                                   \
  inline BSyntaxObject* CLASS##Evaluator(BList* args)			   \
  {									   \
    CLASS* result = new CLASS(args);					   \
    result->Do();							   \
    return((BSyntaxObject*)result);					   \
  }


//--------------------------------------------------------------------
//! Macro that defines arguments for BInternalOperator constructor.
//--------------------------------------------------------------------
#define IntOprConstructor(CLASS, NAME , MINARG, MAXARG, LISTARGS, DESC, CL)\
  (									   \
    NAME,								   \
    CLASS::OwnGrammar(),						   \
    CLASS##Evaluator,							   \
    MINARG,								   \
    MAXARG,								   \
    LISTARGS,								   \
    DESC,								   \
    CL									   \
  )


//--------------------------------------------------------------------
//! Macro that calls BExternalOperator constructor.
//--------------------------------------------------------------------
#define IntOpr(CLASS, NAME , MINARG, MAXARG, LISTARGS, DESC, CL)	   \
  BInternalOperator							   \
  IntOprConstructor(CLASS, NAME , MINARG, MAXARG, LISTARGS, DESC, CL)



//--------------------------------------------------------------------
//! Macro that declares an static BExternalOperator variable
//--------------------------------------------------------------------
#if !defined(USE_DELAY_INIT)

#define DefIntOpr(ORD, CLASS, NAME , MINARG, MAXARG, LISTARGS, DESC, CL)   \
  static BInternalOperator* CLASS##Int##ORD = new BInternalOperator	   \
  IntOprConstructor(CLASS, NAME, MINARG, MAXARG, LISTARGS, DESC, CL);

#else

#define DefIntOpr(ORD, CLASS, NAME , MINARG, MAXARG, LISTARGS, DESC, CL)   \
  static void * clone##CLASS##Int##ORD () {                                \
     BInternalOperator* opr = new BInternalOperator	                       \
	   IntOprConstructor(CLASS,NAME,MINARG,MAXARG,LISTARGS,DESC,CL);         \
     opr->PutCppFile(__FILE__);                                            \
     return opr;                                                           \
  }                                                                        \
  static BInternalOperator* CLASS##Int##ORD  =                             \
	 (BInternalOperator*)(__delay_init((void**)(&CLASS##Int##ORD),     \
                                           &clone##CLASS##Int##ORD));

#endif

//--------------------------------------------------------------------
// macros for BExternalOperator
//--------------------------------------------------------------------

//--------------------------------------------------------------------
//! Macro that defines arguments for BExternalOperator constructor.
//--------------------------------------------------------------------
#define ExtOprConstructor(CLASS, NAME , MINARG, MAXARG,			   \
			  LISTGRAM, LISTARGS, DESCRIPTION, CL)		   \
  (									   \
    NAME,								   \
    CLASS::OwnGrammar(),						   \
    LISTGRAM,								   \
    CLASS##Evaluator,							   \
    MINARG,								   \
    MAXARG,								   \
    LISTARGS,								   \
    DESCRIPTION,							   \
    CL									   \
  )


//--------------------------------------------------------------------
//! Macro that calls BExternalOperator constructor.
//--------------------------------------------------------------------
#define ExtOpr(CLASS, NAME , MINARG, MAXARG, LISTGRAM, LISTARGS, DES, CL)  \
  BExternalOperator							   \
  ExtOprConstructor(CLASS, NAME , MINARG, MAXARG, LISTGRAM,		   \
		    LISTARGS, DES, CL)


//--------------------------------------------------------------------
//! Macro that declares an static BExternalOperator variable
//--------------------------------------------------------------------

#if !defined(USE_DELAY_INIT)
#  define DefExtOpr(ORD,CLASS,NAME,MINARG,MAXARG,LISTGRA,LISTARGS,DES,CL)  \
    static BExternalOperator* CLASS##Ext##ORD  = new BExternalOperator	   \
     ExtOprConstructor(CLASS,NAME,MINARG,MAXARG,LISTGRA,LISTARGS,DES,CL);
#else
#  define DefExtOpr(ORD,CLASS,NAME,MINARG,MAXARG,LISTGRA,LISTARGS,DES,CL)  \
    static void * clone##CLASS##Ext##ORD () {                              \
      BExternalOperator* opr = new BExternalOperator	                     \
      ExtOprConstructor(CLASS,NAME,MINARG,MAXARG,LISTGRA,LISTARGS,DES,CL); \
      opr->PutCppFile(__FILE__);                                           \
      return opr;                                                          \
    }                                                                      \
    static BExternalOperator* CLASS##Ext##ORD  =                           \
	 (BExternalOperator*)(__delay_init((void**)(&CLASS##Ext##ORD),     \
                                           &clone##CLASS##Ext##ORD));
#endif

//--------------------------------------------------------------------
//! Macro that declares an evaluator function for an external class
//--------------------------------------------------------------------
#define DeclareGrammarEvaluator(GRAMMAR, BEXTERNALCLASS)		   \
  inline BSyntaxObject* GRAMMAR##BEXTERNALCLASS##Evaluator(BList* args)	   \
  { return((BSyntaxObject*)(new CLASS(Gra(#GRAMMAR), args))); }


//--------------------------------------------------------------------
//! Macro that declares an external class and its evaluator function
//--------------------------------------------------------------------
#define DeclareExternalClass(BEXTERNALCLASS)				   \
class BEXTERNALCLASS: public BExternalOperatorInstance		           \
{									   \
public:									   \
  BEXTERNALCLASS(BGrammar* gra, BList* arg)				   \
  : BExternalOperatorInstance(gra, arg)					   \
 ~BEXTERNALCLASS(){DESTROY(argList_);}					   \
  void	 Calc();							   \
};									   \
DeclareGrammarEvaluator(GRAMMAR, BEXTERNALCLASS);

#endif // TOL_BOPER_H
