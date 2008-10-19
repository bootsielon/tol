/* arith.cpp: Generic arithmetical functions
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

#include <gsl/gsl_math.h>
#include <tol/tol_bout.h>
#include <tol/tol_barith.h>

BTraceInit("arith.cpp");

//--------------------------------------------------------------------
// constants
//--------------------------------------------------------------------
ldouble minL_  = 3.362103143112094E-4917L;
#ifdef _MSC_VER
static ldouble maxL_  = LDBL_MAX;
#else
static ldouble maxL_  = 1.1897314953572317649E+4932L;
#endif
ldouble rminL_ = Maximum(minL_, 2/maxL_);
static ldouble log2L_ = logl(2);
static ldouble infNegL_ = -LInf();
static ldouble infPosL_ =  LInf();

static ldouble minD_  = 2.2250738585072014E-308;
static ldouble maxD_  = 1.7976931348623158E+308;

static ldouble rminD_ = Maximum(minD_, 2/maxD_);
static ldouble log2D_ = log((long double) 2);
static ldouble infNegD_ = -DInf();
static ldouble infPosD_ =  DInf();

//--------------------------------------------------------------------
double DInf()
//--------------------------------------------------------------------
{
    return GSL_POSINF;
  /*
#ifdef _MSC_VER
  return DBL_MAX;
#else
  return(1.0/0.0);
  #endif*/
}


//--------------------------------------------------------------------
ldouble LInf()
//--------------------------------------------------------------------
{
    return GSL_POSINF;
  /*
#ifdef _MSC_VER
  return LDBL_MAX;
#else
  return(1.0/0.0);
#endif
  */
}


//--------------------------------------------------------------------
ldouble LEpsilon()
//--------------------------------------------------------------------
{
    static ldouble eps = 1;
    if(eps==1)
    {
	ldouble t = 2;
	ldouble k = 1;
	for(; (k<1000)&&(t>1); k++)
	{
	    eps /= 2;
	    t = eps + 1;
	};
	eps *= 2;
	k -= 2;
    }
    return(eps);
}


//--------------------------------------------------------------------
double DEpsilon()
//--------------------------------------------------------------------
{
    static double eps = 1;
    if(eps==1)
    {
	double t = 2;
	double k = 1;
	for(; (k<1000)&&(t>1); k++)
	{
	    eps /= 2;
	    t = eps + 1;
	};
	eps *= 2;
	k -= 2;
    }
    return(eps);
}


//--------------------------------------------------------------------
float FEpsilon()
//--------------------------------------------------------------------
{
    static float eps = 1;
    if(eps==1)
    {
	float t = 2;
	float k = 1;
	for(; (k<1000)&&(t>1); k++)
	{
	    eps /= 2;
	    t = eps + 1;
	};
	eps *= 2;
	k -= 2;
    }
    return(eps);
}


//--------------------------------------------------------------------
ldouble GetPowerScale(ldouble x, long int& n)

/*! Calculats y and e that
 *
 *		   x = y * 2^n
 *
 *		   Log(x,2) = Log(y,2) + n
 *
 *	    with
 *		   1/2 <= y < 1
 *
 *		   n is integer
 */
//--------------------------------------------------------------------
{
    if((x==0.0)||(x==infNegL_)||(x==infPosL_))
    {
	n = 0;
//  Std(BText("\nGetPowerScale")+"\tx="+x);
	return(x);
    }
    ldouble sign = Sign(x);
//Std(BText("\nGetPowerScale")+"\tx="+x);
    ldouble y = logl(Abs(x))/log2L_;
//Std(BText("\ty=")+y);
    n = (BInt) (((y<0)?1:0)+floorl(y));
//Std(BText("\tn=")+(int)n);
    ldouble z = sign*powl(2,y-n);
//Std(BText("\tz=")+z);
    return(z);
}

//--------------------------------------------------------------------
ldouble PowerScale(ldouble x, long int n)
//--------------------------------------------------------------------
{
    if(x==0) { return(0.0); }
    ldouble sign = Sign(x);
//Std(BText("\nPowerScale")+"\tx="+x+"\tn="+(int)n);
    ldouble y = logl(Abs(x))/logl(2);
//Std(BText("\ty=")+y);
    ldouble z = sign*powl(2,y-n);
//Std(BText("\tz=")+z);
    return(z);
}


//--------------------------------------------------------------------
BInt IsDivisible(BInt n, BInt m)

/*! Returns true in m divides n
 */
//--------------------------------------------------------------------
{
    return(!(n%m));
}

//--------------------------------------------------------------------
BInt IsPrime(BInt n)

/*! Returns true if n is a prime number
 */
//--------------------------------------------------------------------
{
    BReal s = sqrt((long double) n);
    for(BInt m=2; m<=s; m++)
    {
	if(IsDivisible(n,m)) { return(0); }
    }
    return(1);
}


//--------------------------------------------------------------------
BInt FirstPrimeAfter(BInt n)

/*! Returns the least prime number great than n
 */
//--------------------------------------------------------------------
{
    while(!IsPrime(++n)) { }
    return(n);
}


//--------------------------------------------------------------------
void FirstPrimes(BInt n, BArray<BInt>& p)

/*! Puts the first n prime numbers in p.
 */
//--------------------------------------------------------------------
{
    if(n<p.Size()) { return; }
    BInt N = p.Size();
    p.ReallocBuffer(n);
    if(N==0)
    {
	N=0;
	p[N++]=2;
	p[N++]=3;
    }
    BInt k = p[N-1];
    do
    {
	k+=2;
	BInt rk = BInt(Sqrt(k));
	BBool isPrime = BTRUE;
	for(BInt j=0; (p[j]<=rk) && (isPrime); j++)
	{
	    isPrime = k%p[j]!=0;
	}
	if(isPrime)
	{
	    p[N++] = k;
//	if(N%1000==0)
//	{  Std(BText("\n")+N+"\t"+k); }
	}
    }
    while(N<n);
}
