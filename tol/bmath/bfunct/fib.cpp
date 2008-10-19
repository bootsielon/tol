/* fib.cpp: BFibonacci: All functions
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

#include <tol/tol_bcommon.h>
#include <tol/tol_bfibonac.h>
#include <tol/tol_bout.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class).
//--------------------------------------------------------------------
BTraceInit("fib.cpp");

const BDat BFibonacci::a_= (BDat(1.0)+1.0/Sqrt(5.0))/2.0;
const BDat BFibonacci::b_= (BDat(1.0)+Sqrt(5.0))/2.0;
const BDat BFibonacci::c_= Log(a_);
const BDat BFibonacci::d_= 1.0/Log(b_);


//--------------------------------------------------------------------
void BFibonacci::Generate(BArray<BINT64> & arr)

/*! Generates the fibonacci number for each item in arr.
 */
//--------------------------------------------------------------------
{
    arr[0]=1;
    arr[1]=1;
    for(BInt n=2; n<arr.Size(); n++)
    { 
      arr[n]=arr[n-1]+arr[n-2]; 
    //Std(BText("\nfib[")+n+"]="+arr[n]);
    }
}


//--------------------------------------------------------------------
void BFibonacci::Generate(BInt n, BArray<BINT64> & arr)

/*! Puts the size of arr to n and generates the n first fibonacci
 *  numbers for each item in arr.
 */
//--------------------------------------------------------------------
{
  arr.AllocBuffer(n+1);
  Generate(arr);
}


//--------------------------------------------------------------------
void BFibonacci::Generate(BArray<BINT64> & arr, BDat f)

/*! Generates the fibonacci numbers not great than f for each item
 *  in arr.
 */
//--------------------------------------------------------------------
{
  Generate(Inverse(f), arr);
}


//--------------------------------------------------------------------
BInt BFibonacci::Number(BInt n)

/*! Returns the n-th fibonacci number
 */
//--------------------------------------------------------------------
{
    register BDat x;
    register BInt f;
    x = a_ * (b_^n);
    f=(BInt)Round(x).Value();
    return(f);
}


//--------------------------------------------------------------------
BInt BFibonacci::Inverse(BDat f)

/*! Returns the smaller integer n which fibonacci number is great or
 *  equal than f.
 */
//--------------------------------------------------------------------
{
    register BDat x;
    register BInt n;
    x=( Log(f) - c_ )*d_;
    n=(BInt)Floor(x).Value();
    if(Number(n)==f) { return(n); }
    else	           { return(n+1); }
}


//--------------------------------------------------------------------
BDat BFibonacci::Minimum( BRRFunction* function,
			  BDat& minimumValue,
			  BDat ini, BDat fin ,
			  BDat tol)
    
/*! Returns the minimum of fibonacci of function in the
 *  interval [ini, fin] with tolerance tol.
 *
 * ALGORITHM: The algorithm of Fibonacci.
 *	      (See any book in non linear programation.)
 *
 *	      1) Generate the Fibonacci numbers non great than (fin-ini)/tol
 *	      2) Obtain the
 *
 */
//--------------------------------------------------------------------
{
  BDat incerInf, incerSup, incer;
  BDat inf, sup;
  BDat infValue=0, supValue=0;
  BArray<BINT64> fib;
  if(tol<DEpsilon()) { tol = DEpsilon(); }
  BDat minimum=0;
  Generate(fib, (fin-ini)/tol);
  BInt N = fib.Size()-1;
  incerInf = ini;
  incerSup = fin;
  incer = incerSup-incerInf;
  inf=incerInf + incer * double(fib[N-2]) / double(fib[N]);
  sup=incerInf + incer * double(fib[N-1]) / double(fib[N]);
  infValue=(*function)[inf];
  supValue=(*function)[sup];

  for(BInt n=1; n<N; n++)
  {
    if (infValue>supValue)
    {
      incerInf=inf;
      incer = incerSup-incerInf;
      inf=sup;
      infValue=supValue;
      sup=incerSup - incer * double(fib[N-n-1]) / double(fib[N-n+1]);
      supValue=(*function)[sup];
      minimum=inf;
      minimumValue = infValue;
    }
    else
    {
      incerSup=sup;
      incer = incerSup-incerInf;
      sup=inf;
      supValue=infValue;
      inf=incerInf + incer * double(fib[N-n-1]) / double(fib[N-n+1]);
      infValue=(*function)[inf];
      minimum=sup;
      minimumValue = supValue;
    }
  }
  return(minimum);
}



//--------------------------------------------------------------------
BInt BFibonacci::Minimum( BRealSuc* function,
			  BDat& minimumValue,
			  BInt ini, BInt fin)
    
/*! Returns the minimum of fibonacci of function in the
 *  interval [ini, fin] with tolerance tol.
 *
 * ALGORITHM: The algorithm of Fibonacci.
 *	      (See any book in non linear programation.)
 *
 *	      1) Generate the Fibonacci numbers non great than (fin-ini)/tol
 *	      2) Obtain the
 *
 */
//--------------------------------------------------------------------
{
  BInt incerInf, incerSup, incer;
  int inf, sup;
  BDat infValue=0, supValue=0;
  BArray<BINT64> fib;

  BInt minimum;

  Generate(fib, fin-ini);
  BInt N = fib.Size()-1;
  incerInf = ini;
  incerSup = fin;
  incer = incerSup-incerInf;
  inf=incerInf + fib[N-2];
  sup=incerSup - fib[N-2];
  infValue=(*function)[inf];
  supValue=(*function)[sup];

  for(BInt n=1; n<N; n++)
  {
    if (infValue>supValue)
    {
      incerInf=inf;
      incer = incerSup-incerInf;
      inf=sup;
      infValue=supValue;
      sup=incerSup - fib[N-n-1];
      supValue=(*function)[sup];
      minimum=inf;
      minimumValue = infValue;
    }
    else
    {
      incerSup=sup;
      incer = incerSup-incerInf;
      sup=inf;
      supValue=infValue;
      inf=incerInf + fib[N-n-1];
      infValue=(*function)[inf];
      minimum=sup;
      minimumValue = supValue;
    }
  }
  return(minimum);
}



//--------------------------------------------------------------------
BDat BFibonacci::Solve( BRRFunction* function,
			BDat value,
			BDat ini, BDat fin ,
			BDat tol)
    
/*! Returns the minimum of fibonacci of function in the
 *  interval [ini, fin] with tolerance tol.
 *
 * ALGORITHM: The algorithm of Fibonacci.
 *	      (See any book in non linear programation.)
 *
 *	      1) Generate the Fibonacci numbers non great than (fin-ini)/tol
 *	      2) Obtain the
 *
 */
//--------------------------------------------------------------------
{
  BDat incerInf, incerSup, incer;
  BDat inf, sup;
  BDat infValue=0, supValue=0, fInf, fSup;
  BArray<BINT64> fib;
  if(ini>fin)
  {
    Error(I2("Bounds error for Fibonacci method",
	     "Error en los límites para el método de Fibonacci."));
    return(BDat::Unknown());
  }
  BDat minimum=0;
  Generate(fib, (fin-ini)/(tol/2));
  BInt n, N = fib.Size()-1;
//Std(BText("Fibonacci(")+N+")");
  incerInf = ini;
  incerSup = fin;
  incer = incerSup-incerInf;
//Std(BText("\nFibonacci search between	  ")+"\t"+ini+"\tand\t"+fin);

  inf=incerInf + incer * double(fib[N-2]) / double(fib[N]);
  sup=incerSup - incer * double(fib[N-2]) / double(fib[N]);
  fInf = (*function)[inf];
  fSup = (*function)[sup];
  infValue=Abs(fInf-value);
  supValue=Abs(fSup-value);
//Std(BText("\nFibonacci iteration ")+1+
//    "\tx in ["+inf+","+sup+"] -> \ty in ["+fInf+","+fSup+"] ");

  for(n=2; n<fib.Size(); n++)
  {
    if (infValue>supValue)
    {
      incerInf=inf;
      incer = incerSup-incerInf;
      inf=sup;
      infValue=supValue;
      sup=incerSup - incer * double(fib[N-n-1]) / double(fib[N-n+1]);
      fSup = (*function)[sup];
      supValue=Abs(fSup-value);
      minimum=inf;
    }
    else
    {
      incerSup=sup;
      incer = incerSup-incerInf;
      sup=inf;
      supValue=infValue;
      inf=incerInf + incer * double(fib[N-n-1]) / double(fib[N-n+1]);
      fInf = (*function)[inf];
      infValue=Abs(fInf-value);
      minimum=sup;
    }
//  Std(BText("\nFibonacci iteration ")+n+
//	"\tx in ["+inf+","+sup+"] -> \ty in ["+fInf+","+fSup+"] ");
  }
  return(minimum);
}
