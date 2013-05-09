/* tol_bcode.h: Classes and functions for real numbers grammar
                of GNU/TOL language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCODE_H
#define TOL_BCODE_H 1

#include <tol/tol_boper.h>
#include <tol/tol_brealfun.h>
class BSpecialFunction;

//--------------------------------------------------------------------
class TOL_API BCode

/*! Class to integrate operators like objects of a special grammar
 *  called Code. This feature allows to pass operators as arguments
 *  of other operators.
 */
//--------------------------------------------------------------------
{
 private:
    BStandardOperator* operator_;
    const BSpecialFunction*  specialFunction_;
    static BCode unknown_;
    
 public:
    // Constructors and destructors: inline
  BCode() 
  : operator_(NIL), 
    specialFunction_(NIL) 
  {}
  BCode(const BCode& code)  
  : operator_(NIL),
    specialFunction_(NIL)
  { 
    PutOperator(code.Operator()); 
  }
 ~BCode() { DelOperator(); }


  BText		 Name	     () const;
  BText		 Dump	     () const;
  BText		 Info	     () const;
  BGrammar*	 Grammar     () const;

  BStandardOperator* Operator        () const;
  const BSpecialFunction*  SpecialFunction () const;

  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  void PutSpecialFunction(const BSpecialFunction*  spf);
  void PutOperator (BStandardOperator* opr);
  void DelOperator ();

  BSyntaxObject* Evaluate  (const List* args)
  {
	 if(Operator()) { return(Operator()->Evaluate(args)); }
	 else		{ return(NIL); }
  }

  BSyntaxObject* Evaluator  (BList* args) const
  {
	 if(Operator()) { return(Operator()->Evaluator(args)); }
	 else		{ return(NIL); }
  }

  const BCode&	operator =  (const BCode& code);
  const BCode&	Replicate   (const BCode& code);

  static BCode&	 Unknown() { return(unknown_); }
  static BBool Compare(BCode* cod1, BCode* cod2) { return(0); }
};


//--------------------------------------------------------------------
class BRRCode : public BRRFunction

/*! Real to Real functions.
 */
//--------------------------------------------------------------------
{
  private :
    const BCode& eval_;
    BBool  isGood_;
  public:
    BRRCode(const BCode& code);
    void  Evaluate(BDat& y, const BDat& x);
    BBool IsGood() { return(isGood_); }
};

//--------------------------------------------------------------------
class BRnRmCode : public BRnRmFunction

/*! Vector to Vector functions.
 */
//--------------------------------------------------------------------
{
  private :
    const BCode& eval_;
    const BCode* jacobian_;
    BBool  isGood_;
  public:
    BRnRmCode(BInt n, BInt m, const BCode& eval, const BCode* jacobian=NIL);
    void Jacobian (const BArray<BDat>& x,
		   const BArray<BDat>& fx,
			 BMatrix<BDat>& G);
    void  Evaluate(BArray<BDat>& y, const BArray<BDat>& x);
    BBool IsGood() { return(isGood_); }
};


//--------------------------------------------------------------------
class BRnRCode : public BRnRFunction

/*! Vector to Vector functions.
 */
//--------------------------------------------------------------------
{
  private :
    const BCode& eval_;
    BBool  isGood_;
    BGrammar* vectorType_;
  public:
    BRnRCode(BInt n, const BCode& eval);
    void  Evaluate(BDat& y, const BArray<BDat>& x);
    BBool IsGood() { return(isGood_); }
};

#endif // TOL_BCODE_H
