/* cod.cpp: Statistic Grammar's functions
            GNU/TOL Language

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

//static const char* bayesVersionId =

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcode.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// codeic variables
//--------------------------------------------------------------------
BTraceInit("cod.cpp");
BCode  BCode::unknown_;


//--------------------------------------------------------------------
  BStandardOperator* BCode::Operator() const 
//--------------------------------------------------------------------
{ 
  assert(!operator_ || !specialFunction_); 
  return(operator_); 
}

//--------------------------------------------------------------------
  const BSpecialFunction*  BCode::SpecialFunction() const 
//--------------------------------------------------------------------
{ 
  assert(!operator_ || !specialFunction_); 
  return(specialFunction_); 
}

//--------------------------------------------------------------------
void BCode::DelOperator()

/*! BCode destructor
 */
//--------------------------------------------------------------------
{
  if(operator_) 
  { 
    operator_->DecNRefs(); 
    if(!operator_->NRefs())
    {
      DESTROY(operator_);
    }
  }
}


//--------------------------------------------------------------------
void BCode::PutSpecialFunction(const BSpecialFunction* spf)

/*! Change the specialFunction_ member
 */
//--------------------------------------------------------------------
{
    DelOperator();
    operator_=NULL;
    specialFunction_ = spf;
}

//--------------------------------------------------------------------
void BCode::PutOperator(BStandardOperator* opr)

/*! Change the operator_ member
 */
//--------------------------------------------------------------------
{
    DelOperator();
    operator_=opr;
    if(operator_) 
    { 
      operator_->IncNRefs(); 
    }
}


//--------------------------------------------------------------------
BText BCode::Name() const

/*! Returns the grammar of operator_ member.
 */
//--------------------------------------------------------------------
{
    BText txt;
    if(Operator()) { txt = Operator()->Name(); }
    return(txt);
}


//--------------------------------------------------------------------
BText BCode::Dump() const

/*! Returns the grammar of operator_ member.
 */
//--------------------------------------------------------------------
{
    BText txt;
    if(Operator())
    {
	if(Operator()->Arguments().Last()=='}') {
	    txt = BText("(") + Operator()->Grammar()->Name() + ") " +
		Operator()->Arguments();
	} else {
	    txt = Operator()->Grammar()->Name() +" "+
		Operator()->Name() + " " +
		Operator()->Arguments();
	}
    }
    return(txt);
}

//--------------------------------------------------------------------
BText BCode::Info() const
//--------------------------------------------------------------------
{
    return(Dump());
}

//--------------------------------------------------------------------
BGrammar* BCode::Grammar() const

/*! Returns the grammar of operator_ member.
 */
//--------------------------------------------------------------------
{
    BGrammar* gra = NIL;
    if(Operator()) { gra = Operator()->Grammar(); }
    return(gra);
}


//--------------------------------------------------------------------
BRRCode::BRRCode(const BCode& code)

/*! Evaluates the eval_ field for x value and puts the result in y.
 */
//--------------------------------------------------------------------
    : BRRFunction(), eval_(code)
{
    isGood_ =
	eval_.Operator() &&
	(eval_.Grammar() == GraReal()) &&
	(eval_.Operator()->MinArg() == 1) &&
	(eval_.Operator()->MaxArg() == 1) &&
	(eval_.Operator()->GrammarForArg(1) == GraReal());
//Std(BText("\nBRRCode::IsGood = ")+isGood_);
}

//--------------------------------------------------------------------
void BRRCode::Evaluate(BDat& y, const BDat& x)
    
/*! Evaluates the eval_ field for x value and puts the result in y.
 */
//--------------------------------------------------------------------
{
  BDat r;
  if(isGood_)
  {
	  BUserDat* uX  = new BContensDat(x);
    BList*    lst = NCons(uX);
    BUserDat* uY  = UDat(eval_.Evaluator(lst));
	  if(uY) 
    { 
       r = Dat(uY); 
  	   DESTROY(uY);
    }
  }
  y = r;
//Std(BText("\nBRRCode::Evaluate(")+x+")="+y);
}

//--------------------------------------------------------------------
BRnRmCode::BRnRmCode(      BInt   n,
			   BInt   m,
		     const BCode& eval,
		     const BCode* jacobian)
    
/*! Evaluates the eval_ field for x value and puts the result in y.
 */
//--------------------------------------------------------------------
    : BRnRmFunction(n,m), eval_(eval), jacobian_(jacobian)
{
    isGood_ =
	eval_.Operator() &&
	(eval_.Grammar() == GraMatrix()) &&
	(eval_.Operator()->MinArg() == 1) &&
	(eval_.Operator()->MaxArg() == 1) &&
	(eval_.Operator()->GrammarForArg(1) == GraMatrix());
//   Std(BText("\nBRnRmCode::IsGood = ")+isGood_);
}

 
//--------------------------------------------------------------------
void BRnRmCode::Jacobian (const BArray<BDat>& x,
			  const BArray<BDat>& y,
			        BMatrix<BDat>& J)
//--------------------------------------------------------------------
{
  if((x.Size()!=n_)||(y.Size()!=m_))
  {
   	Error(I2("Invalid dimension for jacobian function ",
		           "Dimensiones no válidas para función jacbiano ")+
	            "R^"+n_+" -> "+"R^"+m_);
	   return;
  }
  if(!jacobian_)
  {
	   BRnRmFunction::Jacobian(x,y,J);
  }
  else
  {
	   BMatrix<BDat> Y(1,y.Size(),y.Buffer()), X(1,x.Size(),x.Buffer());
	   BUserMat* uX  = new BContensMat(X);
	   BUserMat* uY  = new BContensMat(Y);
	   BList*    lst = Cons(uX,NCons(uY));
	   BUserMat* uJ  = UMat(jacobian_->Evaluator(lst));
	   if(uJ)
	   {
	     J = Mat(uJ);
	     DESTROY(uJ);
	   }
  }
}

//--------------------------------------------------------------------
void BRnRmCode::Evaluate(BArray<BDat>& y, const BArray<BDat>& x)

/*! Evaluates the eval_ field for x value and puts the result in y.
 */
//--------------------------------------------------------------------
{
  if(x.Size()!=n_)
  {
    Error(I2("Invalid dimension for function ",
	     "Dimensiones no válidas para función ")+"R^"+n_+" -> "+"R^"+m_);
    return;
  }
  if(isGood_)
  {
	   BMatrix<BDat> Y, X(1,x.Size(),x.Buffer());
   	 BUserMat*      uX  = new BContensMat(X);
	   BList*         lst = NCons(uX);
	   BSyntaxObject* bs  = eval_.Evaluator(lst);
   	 BUserMat*      uY  = UMat(bs);
   	if(uY)
	   {
	     Y = Mat(uY);
	     y = Y.Data();
	     DESTROY(uY);
	   }
    else
    {
	     DESTROY(lst);
    }
  }
}




//--------------------------------------------------------------------
BRnRCode::BRnRCode(BInt   n,  const BCode& eval)
//--------------------------------------------------------------------
    : BRnRFunction(n), eval_(eval)
{
    isGood_ = 1;
    /*
      eval_.Operator() &&
      (eval_.Grammar() == GraMatrix()) &&
      (eval_.Operator()->MinArg() == 1) &&
      (eval_.Operator()->MaxArg() == 1) &&
      (eval_.Operator()->GrammarForArg(1) == GraMatrix());
    */
//   Std(BText("\nBRnRmCode::IsGood = ")+isGood_);
}


//--------------------------------------------------------------------
void BRnRCode::Evaluate(BDat& y, const BArray<BDat>& x)

/*! Evaluates the eval_ field for x value and puts the result in y.
 */
//--------------------------------------------------------------------
{
  if(x.Size()!=n_)
  {
    Error(I2("Invalid dimension for function ",
    "Dimensiones no válidas para función ")+"R^"+n_+" -> "+"R >> "+x.Size());
    return;
  }
  if(isGood_)
  {
	   BMatrix<BDat>  X(x.Size(), 1,x.Buffer());
	   BUserMat*      uX  = new BContensMat(X);
	   BList*         lst = NCons(uX);
	   BSyntaxObject* bs  = eval_.Evaluator(lst);
	   BUserDat*      uY  = UDat(bs);
	   if(uY)
	   {
	     y = Dat(uY);
	     DESTROY(uY);
	   }
    else
    {
	     DESTROY(lst);
    }
  }
}
