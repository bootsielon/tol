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

TOL_API BDat& StrassenCutoff();


//Products of BMatrix and inherited matrix classes 
TOL_API int MatProd(const BMatrix<double>& A_, 
            const BMatrix<double>& B_,
                  BMatrix<double>& C);
TOL_API BMatrix<double> operator *(const BMatrix<double>& A, 
                           const BMatrix<double>& B);
TOL_API BSymMatrix<double> MtMSqr(const BMatrix<double>& A);
TOL_API BSymMatrix<double> MtMSqr(const BMatrix<double>& A);

TOL_API int MatProd(const BMatrix<BDat>& A_, 
            const BMatrix<BDat>& B_,
                  BMatrix<BDat>& C);
TOL_API BMatrix<BDat> operator *(const BMatrix<BDat>& A, 
                         const BMatrix<BDat>& B);
TOL_API int MtMSqr(const BMatrix<BDat>& A, BSymMatrix<BDat>& S);
TOL_API BSymMatrix<BDat> MtMSqr(const BMatrix<BDat>& A);


#endif	// TOL_BMFSTPRO_H
