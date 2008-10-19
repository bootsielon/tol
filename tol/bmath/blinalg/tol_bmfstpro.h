/* bmfstpro.h: Matrix fast products.
               GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or MatProd FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef TOL_BMFSTPRO_H
#define TOL_BMFSTPRO_H 1

#include <tol/tol_bmatimp.h>

BDat& StrassenCutoff();


//Products of BMatrix and inherited matrix classes 
int MatProd(const BMatrix<double>& A_, 
            const BMatrix<double>& B_,
                  BMatrix<double>& C);
BMatrix<double> operator *(const BMatrix<double>& A, 
                           const BMatrix<double>& B);
BSymMatrix<double> MtMSqr(const BMatrix<double>& A);
BSymMatrix<double> MtMSqr(const BMatrix<double>& A);

int MatProd(const BMatrix<BDat>& A_, 
            const BMatrix<BDat>& B_,
                  BMatrix<BDat>& C);
BMatrix<BDat> operator *(const BMatrix<BDat>& A, 
                         const BMatrix<BDat>& B);
int MtMSqr(const BMatrix<BDat>& A, BSymMatrix<BDat>& S);
BSymMatrix<BDat> MtMSqr(const BMatrix<BDat>& A);


#endif	// TOL_BMFSTPRO_H
