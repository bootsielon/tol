/* tol_fftw.h: API for Fast Fourier Transformation in the West.
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

#ifndef TOL_FFTW_H
#define TOL_FFTW_H 1

#include <fftw3.h>

  
void tol_fftw_dft_1d(
  const BMatrix<double>& in, 
  BMatrix<double>& out, 
  int sign
);

void tol_fftw_dft_1d(
  const BMatrix<BDat>& in, 
  BMatrix<BDat>& out, 
  int sign
);

#endif	// TOL_FFTW_H
