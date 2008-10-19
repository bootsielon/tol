/* tol_barith.h: Simple arithmetic functions.
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

#ifndef TOL_BARITH_H
#define TOL_BARITH_H 1

#include <tol/tol_butil.h>
#include <tol/tol_bdat.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define ldouble long double

extern ldouble minL_;
extern ldouble rminL_;

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
TOL_API ldouble LInf();
TOL_API double  DInf();
TOL_API ldouble LEpsilon();
TOL_API double  DEpsilon();
TOL_API float   FEpsilon();
TOL_API ldouble GetPowerScale(ldouble x, long int& n);
TOL_API ldouble PowerScale   (ldouble x, long int  n);

TOL_API BInt IsDivisible(BInt n, BInt m);
TOL_API BInt IsPrime(BInt n);
TOL_API BInt FirstPrimeAfter(BInt n);
TOL_API void FirstPrimes(BInt n, BArray<BInt>& p);

inline ldouble Abs	(BInt a)  { return(fabsl(a)); }
inline ldouble Sqrt	(BInt a)  { return(sqrtl(a)); }

inline double Sign    (double a) { return((a>0)?1:((a<0)?-1:0)); }
inline double Sqrt    (double a) { return(sqrt(a)); }
inline double Pow     (double a, double b) { return(pow(a,b)); }
inline double Abs     (double a) { return(fabs(a)); }
inline double IsKnown (double a) { return(true); }

inline ldouble Sign    (ldouble a) { return((a>0)?1:((a<0)?-1:0)); }
inline ldouble Sqrt    (ldouble a) { return(sqrtl(a)); }
inline ldouble Pow     (ldouble a, ldouble b) { return(powl(a,b)); }
inline ldouble Abs     (ldouble a) { return(fabsl(a)); }
inline ldouble IsKnown (ldouble a) { return(true); }

#endif	//BARITH
