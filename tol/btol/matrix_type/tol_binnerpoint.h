/* tol_innerpoint.cpp: Finds an inner point of a non empty polytope.

            GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bmatimp.h>


//-----------------------------------------------------------------------------
//Purpose: It is used to obtain a vector X of R^n, such as B*X < c 
//
//Arguments: 
// B ---> matrix mxn 
// c ---> independent terms vector mx1
//
//Method:
//It finds the generators of the polyhedron S of all the feasible solutions
//of the system B*x <= c, where S comes expressed from the form: S = V + W + Q
//Any solution can be obtained combining linearly the generators
//the coefficients of V can be any real value, them of W any value not negative
//and them of Q any not negative value and that its sum is equal to 1
//
//Return:
// X - InnerPoint, A point especially that is interior to the polyhedron 
//     Is the returned value
//These other matrices are passed by reference in the first interface and
//are hiden in second one:
// V - Vectorial Space. 
// W - Cone. 
// Q - Polytopo. 
//
//-----------------------------------------------------------------------------
BMat TOL_API InnerPoint(const BMat & B, const BMat & c, BMat & V, BMat & W, BMat & Q);
BMat TOL_API InnerPoint(const BMat & B, const BMat & c);

