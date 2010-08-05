/* tol_gslmat.h: Interface between gsl_matrix and BMatrix<BDat>
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

/*
  OJO: este archivo es interno y solo se incluye directamente desde
  los cpp de tol, no forma parte del SDK.
 */

#include <tol/tol_bmatrix.h>
#include <gsl/gsl_matrix.h>

/*
 * bmat_to_gsl --
 *
 *   convert a TOL matrix to a GSL matrix. If gsl_mat is NULL then it is
 *   alloced inside this function.
 *
 */

inline
void bmat_to_gsl(BMatrix<BDat> & tol_mat, gsl_matrix * & gsl_mat )
{
  int M = tol_mat.Rows();
  int N = tol_mat.Columns();
  int r,c;
  BDat * mat_buffer = tol_mat.GetData().GetBuffer();

  if (!gsl_mat) {
    gsl_mat = gsl_matrix_alloc(M,N);
  }
  int iptr = 0;
  for (r=0; r<M; r++) {
    for (c=0; c<N; c++,++iptr) {
      gsl_matrix_set(gsl_mat, r, c, mat_buffer[iptr].Value());
    }
  }
}

/*
 * bmat_to_gsl --
 *
 *   convert a TOL matrix to a GSL matrix. If gsl_mat is NULL then it is
 *   alloced inside this function.
 *
 */

inline
void bmat_to_gsl(BMatrix<BDat> & tol_mat, gsl_matrix_view &gsl_view)
{
  int M = tol_mat.Rows();
  int N = tol_mat.Columns();
  BDat * mat_buffer = tol_mat.GetData().GetBuffer();

  gsl_view=gsl_matrix_view_array(&(mat_buffer[0].GetValue()), M, N);
}

/*
 * this convert the first column, first row or diag of tol_mat to a
 * gsl_vector
 */

typedef enum {
  bycol, byrow, bydiag
} bmat_order_t;
inline
void bmat_to_gsl(BMatrix<BDat> & tol_mat, gsl_vector * & gsl_vec,
                 bmat_order_t order=bycol)
{
  gsl_vector_view gsl_v;
  gsl_matrix_view gsl_m;
  
  int M = tol_mat.Rows();
  int N = tol_mat.Columns();
  BDat * mat_buffer = tol_mat.GetData().GetBuffer();

  gsl_m = gsl_matrix_view_array((double*)mat_buffer, M, N);
  switch (order) {
  case bycol:
    gsl_v = gsl_matrix_column(&gsl_m.matrix, 0);
    break;
  case byrow:
    gsl_v = gsl_matrix_row(&gsl_m.matrix, 0);
    break;
  case bydiag:
    gsl_v = gsl_matrix_diagonal(&gsl_m.matrix);
    break;
  }
  if (!gsl_vec) {
    gsl_vec = gsl_vector_alloc(gsl_v.vector.size);
  }
  gsl_vector_memcpy(gsl_vec, &gsl_v.vector);
}

/*
 * gsl_to_bmat --
 *
 *   convert a GSL matrix to a TOL matrix.
 *
 */

inline
void gsl_to_bmat(gsl_matrix * gsl_mat, BMatrix<BDat> & tol_mat)
{
  int M = gsl_mat->size1;
  int N = gsl_mat->size2;
  int r,c;

  tol_mat.Alloc(M,N);
  BDat * mat_buffer = tol_mat.GetData().GetBuffer();
  int iptr = 0;
  for (r=0; r<M; r++) {
    for (c=0; c<N; c++, ++iptr) {
      mat_buffer[iptr].PutValue(gsl_matrix_get(gsl_mat, r,c));
      mat_buffer[iptr].PutKnown(1);
    }
  }
}

/*
 * this convert a gsl_vector to a column matrix.
 */

inline
void gsl_to_bmat(gsl_vector * gsl_vec, BMatrix<BDat> & tol_mat)
{
  int M = gsl_vec->size;
  int r;

  tol_mat.Alloc(M,1);
  BDat * mat_buffer = tol_mat.GetData().GetBuffer();
  int iptr = 0;
  for (r=0; r<M; r++, iptr++) {
    mat_buffer[iptr].PutValue(gsl_vector_get(gsl_vec, r));
    mat_buffer[iptr].PutKnown(1);
  }
}
