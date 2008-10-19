/* tol_bmatimp.h: Matrix implementations.
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

#ifndef TOL_BMATIMP_H
#define TOL_BMATIMP_H 1

#include <tol/tol_bmatrix.h>

#define BMat BMatrix<BDat>
#define DMat BMatrix<double>

#ifndef _MSC_VER
double Sign     (double a);
double Sqrt     (double a);
double Pow      (double a, double b);
double Abs      (double a);
double IsKnown  (double a);
#endif

void BArrDouble2BDat(const BArray<double>& A, BArray<BDat>& B);
void BArrBDat2Double(const BArray<BDat>& A, BArray<double>& B);
void BMatDouble2BDat(const DMat& A, BMat& B);
void BMatBDat2Double(const BMat& A, DMat& B);


void MatrixInitialize();

void InversePivots(const BArray<BInt>& pivot,
		   BArray<BInt>& pivotInv,
		   BInt max);

void InversePivots(const BArray<BInt>& pivot,
		   BArray<BInt>& pivotInv);


BBool Choleski(const BSymMatrix<double>& a,
	             BLowTrMatrix<double>& l);

BBool Inverse(const BSymMatrix<double>& s, BSymMatrix<double>& inv);

void Inverse(const BLowTrMatrix<double>& m, BLowTrMatrix<double>& inv);


BBool Choleski(const BSymMatrix<BDat>& a,
	             BLowTrMatrix<BDat>& l);

BBool Inverse(const BSymMatrix<BDat>& s, BSymMatrix<BDat>& inv);

void Inverse(const BLowTrMatrix<BDat>& m, BLowTrMatrix<BDat>& inv);



        
#endif	// TOL_BMATIMP_H
