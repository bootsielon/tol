/* tol_bfibonac.h: BFibonacci (fib) Class and functions header.
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

#ifndef TOL_BFIBONAC_H
#define TOL_BFIBONAC_H 1

#include <tol/tol_brealfun.h>

//--------------------------------------------------------------------
class BFibonacci

/*! Fibonacci numbers generator and manipulator.
 */
//--------------------------------------------------------------------
{
  private :
	 static const BDat a_;	 // Constants to generate random access
	 static const BDat b_;	 // to Fibonacci numbers and inverse of
   static const BDat c_;	  // Fibonacci numbers.
	 static const BDat d_;


  public :


	 static	 void Generate(BArray<BINT64>&);
	 static	 void Generate(BInt, BArray<BINT64>&);
	 static	 void Generate(BArray<BINT64>&, BDat);

	 static BInt Minimum( BRealSuc* suc,
			BDat& minimumValue,
			BInt ini, BInt fin);

	 static BDat Minimum( BRRFunction* funcion,
			BDat& minimumValue,
			BDat ini, BDat fin,
			BDat tolerance=1);

	 static BDat Solve( BRRFunction* funcion,
		      BDat value,
		      BDat ini, BDat fin,
		      BDat tolerance=1);

	 static BInt Number(BInt n);
	 static BInt Inverse(BDat f);

};

#endif // TOL_BFIBONAC_H
