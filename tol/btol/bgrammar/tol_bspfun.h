/* tol_bspfun.h: Description of special funcions of GNU/TOL language.

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

#ifndef TOL_BSPFUN_H
#define TOL_BSPFUN_H 1

#include <tol/tol_bsyntax.h>

//these warnings disabling macros don't work from included headers
#ifdef _MSC_VER
//base class has not dll interface
#pragma warning(disable : 4275)
//needs to have dll-interface to be used by clients of class
#pragma warning(disable : 4251)
#endif
class BList;
class BSpecialFunction;

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------

typedef BSyntaxObject* (*BSpecialEvaluator)
                       (BGrammar* gra, const List* tre, BBool left);


typedef hash_map_by_name<BSpecialFunction*>::dense_ BSpFunHash;

//--------------------------------------------------------------------
class TOL_API BSpecialFunction
//--------------------------------------------------------------------
{
 private:
  static BSpFunHash instances_;
  static BSpFunHash leftInstances_;
	BText		     name_;
	BText		     arguments_;
	BText		     description_;
	BSpecialEvaluator         function_;
    
 public:

  BSpecialFunction(const BText&	    name	= "",
      const BText&	    arguments	= "",
      const BText&	    description = "",
      BSpecialEvaluator     function	= NIL);

  ~BSpecialFunction();

  static bool Initialize();
  static BText LstIdentify(
    const BList* lst,
    const BText& openSign,
    const BText& closeSign,
    const BText& separatorSign);
  static BInt NumBranches(const List* tre);
  static BBool TestNumArg(
    const BText& name, 
    BInt min, BInt n, BInt max);
  static void GetArg(BSet& set, 
                     const List* tre, 
                     BGrammar* graParam[], 
                     int numParam);
  static BSyntaxObject* TestResult(const BText&        name,
                                         BSyntaxObject* result,
                                   const List*        tre,
                                         BSyntaxObject* defect,
                                         BBool        warn);
  static BSyntaxObject* TestResult(const BText&         name,
                                         BSyntaxObject* result,
                                   const List*          tre,
                                         BSyntaxObject* defect,
                                         BBool          warn,
                                   const BText&         msg);
  static void ArgumentFail(
    const BText & function, const BText & arg, const List* tre);

  static void AddInstance(const BText&		  name,
       const BText&		  arguments,
       const BText&		  description,
	     BSpecialEvaluator      function);

  static void AddLeftInstance(const BText&	         name,
	   const BText&	         arguments,
	   const BText&	         description,
	         BSpecialEvaluator function);
  static const BSpFunHash& Instances();
  void PutName(const BText& name) { name_ = name; }

  const BText& Name	  () const 
  { return(name_	); }

  const BText& Arguments  () const 
  { return(arguments_	); }

  const BText& Description() const 
  { return(description_); }

  BSyntaxObject* Evaluate(      BGrammar* gra,
       const List*     tre,
             BBool     left=BFALSE) const;

  static const BSpecialFunction* Get	     (const BText& name);
  static const BSpecialFunction* LeftGet  (const BText& name);

};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

BGrammar* GetLeft(BGrammar* grammar,
		  BList* left,
		  BText& name,
		  BList** rest);

BText LstIdentify(const BList* lst,
		  const BText& openSign="(",
		  const BText& closeSign=")",
		  const BText& separatorSign=",");

#endif // TOL_BSPFUN_H
