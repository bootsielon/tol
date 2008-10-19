/* tol_matrix_previous.h: Includes, macroes and forward definitions for  
                        matrix classes.
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

#ifndef TOL_MATRIX_PREVIOUS_H
#define TOL_MATRIX_PREVIOUS_H 1

#include <tol/tol_bindex.h>
#include <tol/tol_barith.h>

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#ifdef ENSURE_DIM
#  define BMAT_ENSURE_DIM(i,j) this->EnsureDimensions(i,j);
#else
#  define BMAT_ENSURE_DIM(i,j)
#endif

#define BMatA BMatrix	     <Any>
#define BSpMA BSpaMatrix   <Any>
#define BSyMA BSymMatrix   <Any>
#define BLTMA BLowTrMatrix <Any>
#define BUTMA BUpTrMatrix  <Any>
#define BDiMA BDiagMatrix  <Any>


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
enum BMatrixStoreType
{
  BMST_undefined_ = 0,
  BMST_dense_     = 1,
  BMST_sparse_    = 2,
  BMST_symmetric_ = 3,
  BMST_lowTriang_ = 4,
  BMST_uppTriang_ = 5,
  BMST_diagonal_  = 6
};


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
template <class Any> class BMatrix;
template <class Any> class BSpaMatrix;
template <class Any> class BSymMatrix;
template <class Any> class BLowTrMatrix;
template <class Any> class BUpTrMatrix;
template <class Any> class BDiagMatrix;



//--------------------------------------------------------------------
// template instances
//--------------------------------------------------------------------
template class TOL_API BArray < BInt >;

#endif // TOL_MATRIX_PREVIOUS_H
