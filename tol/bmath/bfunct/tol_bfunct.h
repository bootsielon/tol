/* tol_bfunct.h: Definition of the class BFunction.
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

#ifndef TOL_BFUNC_H
#define TOL_BFUNC_H 1

#include <tol/tol_bdat.h>
#include <tol/tol_barray.h>

//--------------------------------------------------------------------
template <class Domain, class Range>
class BFunction

/*! Representation of functions.
 */
//--------------------------------------------------------------------
{
public:
  typedef void	(*BEval1Function)(Range& r, const Domain& d);
  typedef Range (*BEval2Function)(	    const Domain& d);

private:
  BArray<BDat> parameters_;
  BEval1Function eval1_;
  BEval2Function eval2_;

public:
  // Constructors & destructor:
  BFunction()
  : eval1_(NIL),
    eval2_(NIL)
  {}

  BFunction(const BArray<BDat>& param)
  : eval1_(NIL),
    eval2_(NIL)
  { parameters_ = param; }

  virtual ~BFunction() {}

  BArray<BDat>& Parameters() { return(parameters_); }
  BText Name() const {return("Function");}

  // Access & manipulation:
  void PutEval(BEval1Function eval) { eval1_ = eval; }
  void PutEval(BEval2Function eval) { eval2_ = eval; }

  virtual void Evaluate(Range& r, const Domain& d)
  {
	 if(eval1_) {	  (*eval1_)(r,d); }
    else if(eval2_) { r = (*eval2_)(  d); }
  }
  Range operator[](const Domain& d) { Range r; Evaluate(r, d); return(r); }
};

#endif // TOL_BFUNCT_H
