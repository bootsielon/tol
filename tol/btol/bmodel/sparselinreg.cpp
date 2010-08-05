/* sparselinreg.cpp: Sparse linear regression
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

#include <stdarg.h>

#include <gsl/gsl_randist.h>
#include <tol/tol_bprdist_internal.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btxtgra.h>

#include <tol/tol_btimer.h>

#include "cholmod.h"

BTraceInit("sparselinreg.cpp");

/*
 *  Compute the covarianze cov = P*i(R)*i(R')*P'
 *
 *  code taken fron GSL source code: gsl_multifit_covar.
 */

int compute_covar(int n, double * R, int * perm, double * COV)
{
  int i, j, k;
  const int n_1 = n+1;
  
  /* Form the inverse of R in the full upper triangle of R */
  
  for (k = 0 ; k < n ; k++) {
    double rkk = R[n_1*k];

    /* gsl_matrix_set(r, k, k, 1.0/rkk); */
    R[n_1*k] = 1.0/rkk;

    for (j = 0; j < k ; j++) {
      double t = R[j*n+k] / rkk;
      /* gsl_matrix_set (r, j, k, 0.0); */
      R[j*n+k] = 0;
      for (i = 0; i <= j; i++) {
/*         double rik = gsl_matrix_get (r, i, k); */
        double rik = R[i*n+k];
/*         double rij = gsl_matrix_get (r, i, j); */
        double rij = R[i*n+j];
/*         gsl_matrix_set (r, i, k, rik - t * rij); */
        R[i*n+k] = rik - t * rij;
      }
    }
  }
  
  /* Form the full upper triangle of the inverse of R^T R in the full
     upper triangle of R */
  
  for (k = 0; k < n ; k++) {
    for (j = 0; j < k; j++) {
/*       double rjk = gsl_matrix_get (r, j, k); */
      double rjk = R[j*n+k];
      
      for (i = 0; i <= j ; i++) {
/*         double rij = gsl_matrix_get (r, i, j); */
        double rij = R[i*n+j];
/*         double rik = gsl_matrix_get (r, i, k); */
        double rik = R[i*n+k];
        
/*         gsl_matrix_set (r, i, j, rij + rjk * rik); */
        R[i*n+j] = rij + rjk * rik;
        
      }
    }
    
    {
/*       double t = gsl_matrix_get (r, k, k); */
      double t = R[n_1*k];
      
      for (i = 0; i <= k; i++) {
/*         double rik = gsl_matrix_get (r, i, k); */
        double rik = R[i*n+k];
        
/*         gsl_matrix_set (r, i, k, t * rik); */
        R[i*n+k] = t * rik;
      }
    }
  }
  
  /* Form the full lower triangle of the covariance matrix in the
     strict lower triangle of R and in COV */
  
  for (j = 0 ; j < n ; j++) {
/*     int pj = gsl_permutation_get (perm, j); */
    int pj = perm[j];
    
    for (i = 0; i <= j; i++) {
/*       int pi = gsl_permutation_get (perm, i); */
      int pi = perm[i];
      
      double rij;
      
/*       rij = gsl_matrix_get (r, i, j); */
      rij = R[i*n+j];
      
      if (pi > pj) {
/*         gsl_matrix_set (r, pi, pj, rij);  */
        R[pi*n+pj] = rij;
      } 
      else if (pi < pj) {
/*         gsl_matrix_set (r, pj, pi, rij); */
        R[pj*n+pi] = rij;
      }
    }
    
    { 
/*       double rjj = gsl_matrix_get (r, j, j); */
      double rjj = R[n_1*j];
/*       gsl_matrix_set (covar, pj, pj, rjj); */
      COV[n_1*pj] = rjj;
    }
  }
  
  
  /* symmetrize the covariance matrix */
  
  for (j = 0 ; j < n ; j++) {
    for (i = 0; i < j ; i++) {
/*       double rji = gsl_matrix_get (r, j, i); */
      double rji = R[j*n+i];
      
/*       gsl_matrix_set (covar, j, i, rji); */
      COV[j*n+i] = rji;
/*       gsl_matrix_set (covar, i, j, rji); */
      COV[i*n+j] = rji;
    }
  }
  return 1;
}

#define SPARSE_NZ_INIT   256
#define SPARSE_NZ_GROW   1.5

int sparse_entry(int i, int j, double Tij, cholmod_triplet *T, cholmod_common *cm)
{
  int *Ti, *Tj;
  double *Tx;
  
  if (T->nnz >= T->nzmax &&
      !cholmod_reallocate_triplet(size_t(T->nzmax*SPARSE_NZ_GROW),
                                  T, cm)) {
    return 0;
  }
  Ti = (int*)(T->i);
  Tj = (int*)(T->j);
  Tx = (double*)(T->x);
  Tx[T->nnz] = Tij;
  Ti[T->nnz] = i ;
  Tj[T->nnz++] = j ;
  if (i+1 > (int)T->nrow)
    T->nrow = i+1;
  if (j+1 > (int)T->ncol)
    T->ncol = j+1;
  return 1;
}

int sparse_assembly(int m0, int n0, BSet &sparse, int &m, int &n,
                    cholmod_triplet *T, cholmod_common *cm);

int sparse_assembly_block(int m0, int n0, BSyntaxObject *B, int &m, int &n,
                          cholmod_triplet *T, cholmod_common *cm)
{
  if (B->Grammar() == GraReal()) {
    BDat * dat = &(((BUserDat*)B)->Contens());
    
    if (!sparse_entry(m0, n0, dat->Value(), T, cm)) {
      /* could not assambly Real at (m0,n0) */
      return 0;
    }
    m = n = 1;
  } else if (B->Grammar() == GraMatrix()) {
    BMatrix<BDat> * mat = &(((BUserMat*)B)->Contens());
    m = mat->Rows();
    n = mat->Columns();
    for (int r = 0; r < m; r++) {
      for (int c = 0; c < n; c++) {
        if (!sparse_entry(m0+r, n0+c, (*mat)(r,c).Value(), T, cm)) {
          /* could not assambly Real at (m0,n0) */
          return 0;
        }
      }
    }
  } else if (B->Grammar() == GraSet()) {
    return sparse_assembly(m0, n0, ((BUserSet*)B)->Contens(), m, n, T, cm);
  } else {
    /* invalid grammar for block */
    return 0;    
  }
  return 1;
}

int sparse_assembly_mdiag(int m0, int n0, BSet &sparse, int &m, int &n,
                          cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  int _m, _n, md, nd, tmp;

  m = md = m0;
  n = nd = n0;
  for (int i = 2; i <= size; i++) {
    if (!sparse_assembly_block(md, nd, sparse[i], _m, _n, T, cm)) {
      /* could not assembly block */
      return 0;
    }
    /* update size of matrix */
    if ((tmp=md+_m) > m)
      m = tmp;
    if ((tmp=nd+_n) > n)
      n = tmp;
    /* update diagonal coordinate */
    tmp = _m<_n?_m:_n;
    md += tmp;
    nd += tmp;
  }
  m -= m0;
  n -= n0;
  return 1;
}

int sparse_assembly_diag(int m0, int n0, BSet &sparse, int &m, int &n,
                         cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  int _m, _n, md, nd;

  m = n = 0;
  md = m0;
  nd = n0;
  for (int i = 2; i <= size; i++) {
    if (!sparse_assembly_block(md, nd, sparse[i], _m, _n, T, cm)) {
      /* could not assembly block */
      return 0;
    }
    /* update size of matrix */
    m += _m;
    n += _n;
    /* update diagonal coordinate */
    md += _m;
    nd += _n;
  }
  return 1;
}

int sparse_assembly_sdiag(int m0, int n0, BSet &sparse, int &m, int &n,
                          cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  BUserSet * tmp_set;
  BUserDat * crd;
  BSet * S_i;
  int di, _m, _n;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    tmp_set = USet(sparse[i]);
    if (!tmp_set) {
      /* block element must be a Set */
      return 0;
    }
    S_i = &(tmp_set->Contens());
    if (S_i->Card() != 2) {
      /* an element in a sparse vector must be a Set of 2 elements */
      return 0;
    }
    crd = UDat((*S_i)[1]);
    if (!crd) {
      /* diagonal coordinate must be a number */
      return 0;
    }
    di = int(crd->Contens().Value())-1; /* because TOL matrices are 1-based */
    if (di<0 || !sparse_assembly_block(m0+di, n0+di, (*S_i)[2], _m, _n, T, cm)) {
      /* could not assembly block */
      return 0;
    }
    /* update size of matrix */
    _m += di;
    if (_m > m)
      m = _m;
    _n += di;
    if (_n > n)
      n = _n;
  }
  return 1;
}

int sparse_assembly_row(int m0, int n0, BSet &sparse, int &m, int &n,
                        cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  int _m, _n;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    if (!sparse_assembly_block(m0, n0+n, sparse[i], _m, _n,  T, cm)) {
      /* could not assembly block */
      return 0;
    }
    /* update size of matrix */
    if (_m > m)
      m = _m;
    n += _n;
  }
  return 1;
}

int sparse_assembly_srow(int m0, int n0, BSet &sparse, int &m, int &n,
                         cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  BUserSet * tmp_set;
  BUserDat * crd;
  BSet * S_i;
  int _m, _n, ni;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    tmp_set = USet(sparse[i]);
    if (!tmp_set) {
      /* block element must be a Set */
      Error("a block element in a 'srow' definition must be a Set of two elements: [[col_i, B_i]]");
      return 0;
    }
    S_i = &(tmp_set->Contens());
    if (S_i->Card() != 2) {
      /* an element in a sparse vector must be a Set of 2 elements */
      Error("a block element in a 'srow' definition must be a Set of two elements: [[col_i, B_i]]");
      return 0;
    }
    crd = UDat((*S_i)[1]);
    if (!crd) {
      /* column coordinate must be a number */
      Error("the column coordinate of a block in a 'srow' definition must be a possitive number");
      return 0;
    }
    ni = int(crd->Contens().Value())-1; /* because TOL matrices are 1-based */
    if (ni>=0) {
      if (!sparse_assembly_block(m0, n0+ni, (*S_i)[2], _m, _n, T, cm)) {
        /* could not assembly block */
        char buffer[256];
        snprintf(buffer, 255,
                 "could not assembly block of coordinate %d for srow", ni);
        Error(buffer);
        
        return 0;
      }
    } else {
      Error("the column coordinate of a block in a 'srow' definition must be a possitive number");
      return 0;
   }
    
    /* update size of matrix */
    if (_m > m)
      m = _m;
    _n += ni;
    if (_n > n) 
      n = _n;
  }
  return 1;
}

int sparse_assembly_col(int m0, int n0, BSet &sparse, int &m, int &n,
                        cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  int _m, _n;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    if (!sparse_assembly_block(m0+m, n0, sparse[i], _m, _n, T, cm)) {
      /* could not assembly block */
      return 0;
    }
    /* update size of matrix */
    m += _m;
    if (_n > n)
      n = _n;
  }
  return 1;
}

int sparse_assembly_scol(int m0, int n0, BSet &sparse, int &m, int &n,
                     cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  BUserSet * tmp_set;
  BUserDat * crd;
  BSet * S_i;
  int _m, _n, mi;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    tmp_set = USet(sparse[i]);
    if (!tmp_set) {
      /* block element must be a Set */
      Error("a block element in a 'scol' definition must be a Set of two elements: [[row_i, B_i]]");
      return 0;
    }
    S_i = &(tmp_set->Contens());
    if (S_i->Card() != 2) {
      /* an element in a sparse vector must be a Set of 2 elements */
      Error("a block element in a 'scol' definition must be a Set of two elements: [[row_i, B_i]]");
      return 0;
    }
    crd = UDat((*S_i)[1]);
    if (!crd) {
      /* row coordinate must be a number */
      Error("the row coordinate of a block in a 'scol' definition must be a possitive number");
      return 0;
    }
    mi = int(crd->Contens().Value())-1; /* because TOL matrices are 1-based */
    if (mi>=0) {
      if (!sparse_assembly_block(m0+mi, n0, (*S_i)[2], _m, _n, T, cm)) {
        /* could not assembly block */
        char buffer[256];
        snprintf(buffer, 255,
                 "could not assembly block of coordinate %d for scol", mi);
        Error(buffer);
        return 0;
      }
    } else {
      Error("the row coordinate of a block in a 'scol' definition must be a possitive number");
      return 0;
   }
    
    /* update size of matrix */
    _m += mi;
    if (_m > m) 
      m = _m;
    if (_n > n)
      n = _n;
  }
  return 1;
}

int sparse_assembly_triplet(int m0, int n0, BSet &sparse, int &m, int &n,
                            cholmod_triplet *T, cholmod_common *cm)
{
  int size = sparse.Card();
  BUserSet * tmp_set;
  BUserDat *crd;
  BSet * S_i;
  int _m, _n, mi, ni;

  m = n = 0;
  for (int i = 2; i <= size; i++) {
    tmp_set = USet(sparse[i]);
    if (!tmp_set) {
      /* block element must be a Set */
      Error("a block element in a 'triplet' definition must be a Set of three elements: [[row_i, col_i, B_i]]");
      return 0;
    }
    S_i = &(tmp_set->Contens());
    if (S_i->Card() != 3) {
      /* an element in a sparse triplet must be a Set of 3 elements */
       Error("a block element in a 'triplet' definition must be a Set of three elements: [[row_i, col_i, B_i]]");
       return 0;
    }
    crd = UDat((*S_i)[1]);
    if (!crd) {
      /* triplet coordinate must be a number */
      Error("the row coordinate of a block in a 'triplet' definition must be a possitive number");
      return 0;
    }
    mi = int(crd->Contens().Value())-1; /* because TOL matrices are 1-based */
    crd = UDat((*S_i)[2]);
    if (!crd) {
      /* triplet coordinate must be a number */
       Error("the column coordinate of a block in a 'triplet' definition must be a possitive number");
     return 0;
    }
    ni = int(crd->Contens().Value())-1; /* because TOL matrices are 1-based */
    if (mi>=0 || ni>=0) {
      if (!sparse_assembly_block(m0+mi, n0+ni, (*S_i)[3], _m, _n, T, cm)) {
        /* could not assembly block */
        char buffer[256];
        snprintf(buffer, 255,
                 "could not assembly block of coordinate (%d,%d) for triplet",
                 mi,ni);
        Error(buffer);
        return 0;
      }
    } else {
      Error("the row and column coordinates of a block in a 'triplet' definition must be possitive numbers");
    }
    
   /* update size of matrix */
    _m += mi;
    if (_m > m)
      m = _m;
    _n += ni;
    if (_n > n)
      n = _n;
  }
  return 1;
}


int sparse_assembly_identity(int m0, int n0, BSet &sparse, int &m, int &n,
                          cholmod_triplet *T, cholmod_common *cm)
{
  int card = sparse.Card(), size;
  BSyntaxObject *soSize;
  BDat * datSize;
  
  if (card != 2) {
    Error("Invalid identity matrix definition: must be a set with 2 elements "
          "'[[\"ident\", size]]");
    return 0;
  }
  soSize = sparse[2];
  if (soSize->Grammar() != GraReal()) {
    Error("Invalid identity matrix definition: size must be a Real");
    return 0;    
  }
  datSize = &(((BUserDat*)soSize)->Contens());
  if (!datSize->IsKnown() || ((size=int(datSize->Value()))<0)) {
    Error("Invalid identity matrix definition: size must be positive");
    return 0;    
  }
  
  m = n = size;
  for (int i = 0; i < size; i++) {
    if (!sparse_entry(m0+i, n0+i, 1.0, T, cm)) {
      /* could not assambly Real at (m0,n0) */
      return 0;
    }
  }
  return 1;  
}

int sparse_assembly(int m0, int n0, BSet &sparse, int &m, int &n,
                    cholmod_triplet *T, cholmod_common *cm)
{
  if (sparse.Card() < 2) {
    /* at least {type, S1} */
    return 0;
  }
  BUserText *ut_type = UText(sparse[1]);
  if (!ut_type) {
    /* type element must be Text */
    return 0;
  }
  char * type = ut_type->Contens().Buffer();
  if (!strcasecmp(type, "DIAG"))
    return sparse_assembly_diag(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "SDIAG"))
    return sparse_assembly_sdiag(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "ROW"))
    return sparse_assembly_row(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "SROW"))
    return sparse_assembly_srow(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "COL"))
    return sparse_assembly_col(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "SCOL"))
    return sparse_assembly_scol(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "triplet"))
    return sparse_assembly_triplet(m0, n0, sparse, m, n, T, cm);
  else if (!strcasecmp(type, "ident")) {
    return sparse_assembly_identity(m0, n0, sparse, m, n, T, cm);
  } else {
    /* unknown sparse matrix type */
    return 0;    
  }
}

/*
 * Build a sparse matrix fron a Set sparse definition
 */

cholmod_sparse * sparse_create(BSet & sparse, cholmod_common *cm)
{
  cholmod_triplet *T;
  cholmod_sparse *S = NULL;
  
  int m, n;
  
  T = cholmod_allocate_triplet (0, 0, SPARSE_NZ_INIT, 0, CHOLMOD_REAL,
                                cm) ;
  if (T) {
    if (sparse_assembly(0, 0, sparse, m, n, T, cm)) {
      S = cholmod_triplet_to_sparse(T, 0, cm);
    } else {
      S = NULL;
    }
    cholmod_free_triplet(&T, cm);
  }
  return S;
}

/*
 * compute y = X*b + E
 *
 * if C is NULL : E ~ N[0,s2*I]
 *
 * if C is not NULL: E ~ N[0,s2*W]
 *
 *   W = H*H^t, W^-1 = H^{-t}*H^-1 = C^t*C, C = H^-1
 *
 * (X^t*X)^-1 = Vb = F*F^t
 *
 * override 'y' with the solution
 *
 * return the solution b or NULL in case of failure.
 */

cholmod_dense * sparse_regression(cholmod_sparse *X,
                                  cholmod_dense  *y,
                                  cholmod_sparse *C,
                                  double *sse,
				  double *mse,
				  double *sst,
				  double *R2,
				  double *R2_adj,
				  double *residuals,
                                  cholmod_sparse **F,
                                  cholmod_common *cm)
{
  
  double zero[2] = {0,0}; /* basic scalars */
  double one [2] = {1,0};
  double minusone [2] = {-1,0};
  cholmod_sparse *A, *I, *M;
  cholmod_factor *L;
  cholmod_dense *b, *z, *z1;
  int n, k, i;

  n = X->nrow;
  k = X->ncol;

  if (C) {
    /* weight X as C*X = A */
    A = cholmod_ssmult(C, X,
                       0 /* stype */, 1 /* values? */, 0 /* sorted? */, cm);
    z = cholmod_allocate_dense(n, y->ncol, n, CHOLMOD_REAL, cm);
   /* weight y as C*y = z */
    cholmod_sdmult(C, 0, one, zero, y, z, cm);	    /* z = C*y */

    /* now the regression is z = A*b */
    
    z1 = cholmod_allocate_dense(k, y->ncol, k, CHOLMOD_REAL, cm);
    /* z1 = A' * z */
    cholmod_sdmult(A, 1 /* A'*/, one, zero, z, z1, cm);

    /* transpose A so that A'A will be factored */
    cholmod_sparse* tmp = A;
    A = cholmod_transpose (tmp, 1, cm);
    cholmod_free_sparse(&tmp, cm);
  } else {
    z = y; /* to compute residual */
    z1 = cholmod_allocate_dense(k, y->ncol, k, CHOLMOD_REAL, cm);
    /* z1 = X' * y */
    cholmod_sdmult(X, 1 /* X'*/, one, zero, y, z1, cm);
    /* transpose X so that X'X will be factored */
    A = cholmod_transpose (X, 1, cm);
  }
  
  /* factorize P*A'*A*P' = L*L' */
#define _MEASURE_CHOLMOD_FACTORIZE_

#ifdef _MEASURE_CHOLMOD_FACTORIZE_
  BTimer *tm = new BTimer("midiendo cholmod_factorize");
#endif
  L = cholmod_analyze (A, cm);
  cholmod_factorize(A, L, cm);
  
#ifdef _MEASURE_CHOLMOD_FACTORIZE_
  delete tm;
#endif
  
  if (cm->status) {
    char buffer[256];

    snprintf(buffer, 256, "cholmod_factorize failed at column %d",
             L->minor+1);
    Error(buffer);
    
    /* release z */
    if (z != y)
      cholmod_free_dense(&z, cm);
    
    cholmod_free_dense(&z1, cm);
    /* release A */
    cholmod_free_sparse (&A, cm);
    return NULL;
  }

#if defined(_PRINT_FACTOR_)
  cholmod_print_factor(L, "L", cm);
#endif
  
  /* Solve A*b = z1 */
  b = cholmod_solve(CHOLMOD_A, L, z1, cm);
  /* release z1 */
  cholmod_free_dense(&z1, cm);

  /* compute SST for z */
  double *zx = (double*)(z->x), zi;
  double z_mean = 0, _sst;
  for (i = 0; i < (int)z->nrow; i++) {
    z_mean += zx[i];
  }
  z_mean /= z->nrow;
  _sst = 0.0;
  for (i = 0; i < (int)z->nrow; i++) {
    zi = zx[i] - z_mean;
    _sst += zi*zi;
  }
  *sst = _sst; 
  /* compute MSE, r = z - A*b, s2 = (r'*r)/(n-k) */
  /* remember that at this point A is in fact A' */
  /* r = z - A * b */
  cholmod_sdmult (A, 1 /* (A')'*/,
                  minusone, one, b, z, cm) ; /* z -= X*b */
  /* release A as I don't need it from now on */
  cholmod_free_sparse (&A, cm);

  zx = (double*)(z->x);
  double _sse = 0.0;
  if (residuals) {
    for (i = 0; i < (int)z->nrow; i++) {
      residuals[i] = zi = zx[i];
      _sse += zi*zi;
    }
  } else {
    for (i = 0; i < (int)z->nrow; i++) {
      zi = zx[i];
      _sse += zi*zi;
    }
  }
  
  *sse = _sse;
  *R2  = 1.0 - _sse/_sst;
  if (n > k) {
    *mse    = _sse/(n-k);
    *R2_adj = 1 - ((n-1)*(1-*R2)/(n-k-1));
  } else {
    *mse    = _sse;
    *R2_adj = *R2;
  }

  /* release z */
  if (z != y)
    cholmod_free_dense(&z, cm);
  
  /* compute cholesky factor of Vb:

     X' * X = P' * L * L' * P

     (X' * X)^-1 = P' * (L')^-1 * L^-1 * P
     
     F = P' * (L')^-1

     L'*M = I

     F = P'M
  */
  cholmod_change_factor(CHOLMOD_REAL,
                        1, /* convert to LL' */
                        L->is_super, /* keep in supper or simplicial */
                        0, /* do not pack */
                        0, /* do not put in order */
                        L, cm);
  
#if defined(_PRINT_FACTOR_)
  cholmod_print_factor(L, "L", cm);
#endif

  I = cholmod_speye(k, k, CHOLMOD_REAL, cm);
  M = cholmod_spsolve(CHOLMOD_Lt, L, I, cm);  /* L' * M = I */
  cholmod_free_sparse(&I, cm);
  *F = cholmod_spsolve(CHOLMOD_Pt, L, M, cm); /* F = P' * M */
  cholmod_free_sparse(&M, cm);
  cholmod_free_factor(&L, cm);
  return b;
}

static
BList * BuildList(int n, ...)
{
  BList *lst = NIL;
  BList *aux = NIL;
  BSyntaxObject *item;
  va_list ap;

  if (n<=0)
    return NULL;
  va_start(ap, n);
  for (int i = 0; i < n; i++) {
    item = va_arg(ap, BSyntaxObject *);
    LstFastAppend(lst, aux, item);
  }
  va_end(ap);
 return lst;
}

static
BList * BuildList(int n, BSyntaxObject **soVector)
{
  BList *lst = NIL;
  BList *aux = NIL;

  if (n<=0)
    return NULL;
  for (int i = 0; i < n; i++) {
    LstFastAppend(lst, aux, soVector[i]);
  }
  return lst;
}

static
BSyntaxObject *BuildMatrix(int m, int n, double *data,
                           const char * name, const char * desc)
{
  BUserMat *umat;
  BMat *mat;

  umat = new BContensMat("",BMat(), desc);
  umat->PutName(name);
  mat = &(umat->Contens());
  mat->Alloc(m,n);
  for (int r = 0; r < m; r++) {
    for (int c = 0; c < n; c++, ++data) {
      (*mat)(r,c) = *data;
    }
  }
  return umat;
}

/*
 * Run a simulation from p(beta,s2|y) using a gibbs sampler:
 *
 *     s2|y ~ Inv-X^2[n-k, mse]
 *     beta ~ N[mean,s2*Vb], Vb = F*F'
 *
 * the return value is a matrix of size (k+1)-by-size, where the first row
 * correspond to the variance sample, the rest of rows are the sampling for
 * the beta parameters.
 *
 * The algorithm initially discard 'burning' samples.
 */

static
BSyntaxObject * SparseSamplePosterior(int n, int burning, int size,
                                      cholmod_dense *mean, cholmod_sparse *F, double mse,
                                      cholmod_common * cm)
{
  BUserMat *sample;
  BMat *mat_sample;
  int i, j, k, deg;
  double s2, s[2] = {1,0};
  cholmod_dense *beta, *iid;
  double *beta_x, *mean_x, *iid_x;
  double one [2] = {1,0};
  double ns2;
  
  k = F->nrow;
  deg = n-k;
  ns2 = deg*mse;
  
  beta = cholmod_allocate_dense(k, 1, k, CHOLMOD_REAL, cm);
  iid = cholmod_allocate_dense(k, 1, k, CHOLMOD_REAL, cm);
  mean_x = (double*)(mean->x);
  beta_x = (double*)(beta->x);
  iid_x = (double*)(iid->x);
  
  /* burning period */
  for (i = 0; i < burning; i++) {
    /* sample s2 */
    s2 = ns2/gsl_ran_chisq(getGslRng(), deg);
    s[0] = sqrt(s2); 

    /* sample IID N[0,1] */
    for (j = 0; j < k; j++) {
      iid_x[j] = gsl_ran_ugaussian(getGslRng());
    }
    /* sample beta correlated multinormal: beta = mean + s*F*iid */
    cholmod_copy_dense2(mean, beta, cm);           /* beta = mean */
    cholmod_sdmult(F, 0, s, one, iid, beta, cm); /* beta += s*F*iid */    
  }

  sample = new BContensMat("",BMatrix<BDat>(),"sample");
  mat_sample = &(sample->Contens());
  mat_sample->Alloc(size, k+1);
  
  /* sampling period */
  for (i = 0; i < size; i++) {
    /* sample s2 */
    s2 = ns2/gsl_ran_chisq(getGslRng(), deg);
    (*mat_sample)(i,0) = s2;
    s[0] = sqrt(s2); 

    /* sample IID N[0,1] */
    for (j = 0; j < k; j++) {
      iid_x[j] = gsl_ran_ugaussian(getGslRng());
    }

    /* sample beta correlated multinormal: beta = mean + s*F*iid */
    cholmod_copy_dense2(mean, beta, cm);           /* beta = mean */
    cholmod_sdmult(F, 0, s, one, iid, beta, cm); /* beta += ss*F*iid */
    for (j = 0; j < k; j++) {
      (*mat_sample)(i,j+1) = beta_x[j];
    }
  }
  cholmod_free_dense(&beta, cm);
  cholmod_free_dense(&iid, cm);
  
  return sample;
}


class ReturnArg 
{
public:
  virtual ~ReturnArg() {}
  static
  void Init(int _n, cholmod_dense *_beta,
	    double _mse, double _sse, double _sst, double _R2, double _R2_adj,
	    double *_residuals,
	    cholmod_sparse *_F,
            cholmod_common * _cm)
    {
      ReturnArg::n = _n;
      ReturnArg::beta = _beta;
      ReturnArg::mse = _mse;
      ReturnArg::sse = _sse;
      ReturnArg::sst = _sst;
      ReturnArg::R2 = _R2;
      ReturnArg::R2_adj = _R2_adj;
      ReturnArg::residuals = _residuals;
      ReturnArg:: F = _F;
      ReturnArg::cm = _cm;
    }
  virtual BSyntaxObject *Create(const char **next) = 0;
protected:
  static int n;
  static cholmod_dense *beta;
  static double mse;
  static double sse;
  static double sst;
  static double R2;
  static double R2_adj;
  static double* residuals;
  static cholmod_sparse *F; /* Vb = F*F' */
  static cholmod_common *cm;
};


int ReturnArg::n = 0;
cholmod_dense *ReturnArg::beta = NULL;
double ReturnArg::mse = 0.0;
double ReturnArg::sse = 0.0;
double ReturnArg::sst = 0.0;
double ReturnArg::R2 = 0.0;
double ReturnArg::R2_adj = 0.0;
double* ReturnArg::residuals = NULL;
cholmod_sparse *ReturnArg::F = NULL; /* Vb = F*F' */
cholmod_common *ReturnArg::cm = NULL;

class ReturnBeta : public ReturnArg
{
public:
  virtual ~ReturnBeta() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnBeta::Create(const char **)
{
  return BuildMatrix(ReturnArg::beta->nrow,
                     ReturnArg::beta->ncol,
                     (double*)(ReturnArg::beta->x),
                     "beta", "beta estimated");
}

class ReturnMSE : public ReturnArg
{
public:
  virtual ~ReturnMSE() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnMSE::Create(const char **)
{
  BUserDat *udat;
  
  udat = new BContensDat("", ReturnArg::mse, "mean square error");
  udat->PutName("MSE");
  return udat;
}

class ReturnSSE : public ReturnArg
{
public:
  virtual ~ReturnSSE() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnSSE::Create(const char **)
{
  BUserDat *udat;
  
  udat = new BContensDat("", ReturnArg::sse, "sum square error");
  udat->PutName("SSE");
  return udat;
}

class ReturnSST : public ReturnArg
{
public:
  virtual ~ReturnSST() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnSST::Create(const char **)
{
  BUserDat *udat;
  
  udat = new BContensDat("", ReturnArg::sst, "sum square total");
  udat->PutName("SST");
  return udat;
}

class ReturnR2 : public ReturnArg
{
public:
  virtual ~ReturnR2() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnR2::Create(const char **)
{
  BUserDat *udat;
  
  udat = new BContensDat("", ReturnArg::R2,
		      "R-squared, coefficient of multiple determination");
  udat->PutName("R2");
  return udat;
}

class ReturnR2_adj : public ReturnArg
{
public:
  virtual ~ReturnR2_adj() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnR2_adj::Create(const char **)
{
  BUserDat *udat;
  
  udat = new BContensDat("", ReturnArg::R2_adj, "R2 adjusted");
  udat->PutName("R2_adj");
  return udat;
}

class ReturnResiduals : public ReturnArg
{
public:
  virtual ~ReturnResiduals() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnResiduals::Create(const char **)
{
  return BuildMatrix(ReturnArg::n, 1, ReturnArg::residuals,
		     "Residuals", "Residals = Y - Y_hat");
}

class ReturnVb : public ReturnArg
{
public:
  virtual ~ReturnVb() {}
  virtual BSyntaxObject *Create(const char **);
};

BSyntaxObject *ReturnVb::Create(const char **)
{
  /* I thing that it is better to return a Set */
  cholmod_dense *denseF;
  BSyntaxObject *so;

  denseF = cholmod_sparse_to_dense(ReturnArg::F, ReturnArg::cm);
  
  so = BuildMatrix(denseF->nrow, denseF->ncol, (double*)(denseF->x),
                   "F", "Vb = F*F'");
  cholmod_free_dense(&denseF, ReturnArg::cm);
  return so;
}

class ReturnSample : public ReturnArg
{
public:
  virtual ~ReturnSample() {}
  virtual BSyntaxObject *Create(const char **next);
};

BSyntaxObject *ReturnSample::Create(const char **next)
{
  int burning, size, n;
  const char *tmp;
  
  if ((n=sscanf(*next, "%d,%d", &burning, &size))!=2) {
    printf("%d argument were read instead of 2\n", n);
    Error("Could not extract burning and size parameters for Gibbs sampler requested");
    return NULL;
  }
  /* set next pointing to next ':' or EOS */
  tmp = strchr(*next, ':');
  if (tmp) {
    *next = tmp;
  } else {
    *next += strlen(*next);
  }
  return SparseSamplePosterior(ReturnArg::n, burning, size,
                               ReturnArg::beta,
                               ReturnArg::F,
                               ReturnArg::mse,
                               ReturnArg::cm);
}

static
void my_cholmod_error(int status, char *file, int line, char *message);

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetSparseLinReg);
DefExtOpr(1, BSetSparseLinReg, "SparseLinearRegression", 3, 4,
          "Text Matrix Set Set",
          "(Text result, Matrix y, Set SparseX [, Set SparseCW])",
          "Estimates the linear regression model Y = X*b + E, E ~ N[0,s2*W]\n"
          "SparseX is the sparse specification of X,\n"
          "SparseCW is the sparse specification of a factor C, where W^-1=C'*C, C defaults to I\n"
          "The result argument is a colon separated string indicating the requested result from the regression. The posible values are:\n"
          "BETA : the estimated parameters\n"
          "SSE  : sum square error\n"
          "MSE  : mean square error\n"
          "SST  : sum square total\n"
          "R2   : R-squared, coefficient of multiple determination\n"
          "R2A  : R2 adjusted\n"
          "RES  : vector of residuals\n"
          "VAR  : varianze of parameters. In this case what is returned is a sparse factor F such as Vb = F*F'\n"
          "SAMPLE=B,N : return a sample of size N from p(beta,s2|y,X) using B as burning size\n"
          "ALL  : return all the values except SAMPLE that needs arguments."
          "if result is empty or invalid a default value is used: \"Beta:MSE:VAR\"",
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetSparseLinReg::CalcContens()
//--------------------------------------------------------------------
{
  cholmod_sparse *X, *F, *W;
  cholmod_common cm;
  cholmod_dense *y, *beta;
  BText * arg_ret;
  BMat *arg_y;
  BSet *arg_X;
  int i, n, k;
  double mse, sse, sst, R2, R2_adj;
  double *residuals;

  arg_ret = &(((BUserText*)Arg(1))->Contens());  
  arg_y = &(((BUserMat*)Arg(2))->Contens());
  arg_X = &(((BUserSet*)Arg(3))->Contens());
  
  if (arg_y->Columns() != 1) {
    Error("Matrix argument 'y' to SparseLinearRegression must be a column vector");
    return;
  }
  cholmod_start(&cm);
  cm.error_handler = &my_cholmod_error;
  X = sparse_create(*arg_X, &cm);
  if (!X) {
    Error("Could not create sparse matrix X in SparseLinearRegression");
    cholmod_finish (&cm) ;
    return;
  }
  n = X->nrow;
  k = X->ncol;
  if (n != arg_y->Rows()) {
    printf("%dx%d %dx1\n", n, k, arg_y->Rows());
    
    Error("Matrix y and X must match in row numbers in SparseLinearRegression");
    return;
  }
  y = cholmod_allocate_dense(n, 1, n, CHOLMOD_REAL, &cm);
  double * yx = (double*)(y->x);
  for (i = 0; i < n; i++) {
    yx[i] =(*arg_y)(i,0).Value();
  }
  if (NumArgs()>3) {
    W = sparse_create(((BUserSet*)Arg(4))->Contens(), &cm);
    if (!W) {
      Error("Could not create sparse matrix W in SparseLinearRegression");
      cholmod_free_sparse(&X, &cm) ;
      cholmod_free_dense(&y, &cm) ;
      cholmod_finish (&cm) ;
      return;
    }
    if (W->nrow!=n || W->ncol!=n) {
      Error("Weight matrix must be square and same number of rows as X");
      cholmod_free_sparse(&X, &cm) ;
      cholmod_free_sparse(&W, &cm) ;
      cholmod_free_dense(&y, &cm) ;
      cholmod_finish (&cm) ;
      return;
    }
  } else {
    W = NULL;
  }

  F = NULL;
  residuals = new double[n];
  beta = sparse_regression(X, y, W,
			   &sse, &mse, &sst, &R2, &R2_adj, residuals,
			   &F, &cm);
  cholmod_free_sparse(&W, &cm);
  cholmod_free_dense(&y, &cm);
  
  if (!beta) {
    /* regression failed */
    if (F)
      cholmod_free_sparse(&F, &cm);
    /* return X'X in order to check outside */
    /* compute Xt */
    cholmod_sparse *Xt = cholmod_transpose(X, 1, &cm);
    cholmod_free_sparse(&X, &cm);
    
    /* compute Xt*X */
    cholmod_sparse *XtX = cholmod_aat(Xt, NULL, 0, 1, &cm);
    cholmod_free_sparse(&Xt, &cm);

    cholmod_dense *dense_XtX = cholmod_sparse_to_dense(XtX, &cm);
    cholmod_free_sparse(&XtX, &cm);

    BSyntaxObject *so = BuildMatrix(dense_XtX->nrow, dense_XtX->ncol,
                                    (double*)(dense_XtX->x), "XtX", "XtX");
    cholmod_free_dense(&dense_XtX, &cm);

    contens_.RobElement(Cons(so,NIL));
    cholmod_finish(&cm);
    delete []residuals;
    return;
  }
  cholmod_free_sparse(&X, &cm);

  ReturnBeta      retBeta;
  ReturnVb        retVb;
  ReturnMSE       retMSE;
  ReturnSSE       retSSE;
  ReturnSST       retSST;
  ReturnR2        retR2;
  ReturnR2_adj    retR2A;
  ReturnResiduals retRES;
  ReturnSample    retSample;
  ReturnArg::Init(n, beta, mse, sse, sst, R2, R2_adj, residuals, F, &cm);

  struct table_ret_t {
    const char * name;
    int used;
    int need_arg;
    ReturnArg *returner;
  };
  
  table_ret_t table_returner[] = 
    {
      {
        "BETA", 0, 0, &retBeta
      },
      {
        "VAR", 0, 0, &retVb
      },
      {
        "MSE", 0, 0, &retMSE
      },
      {
        "SSE", 0, 0, &retSSE
      },
      {
        "SST", 0, 0, &retSST
      },
      {
        "R2", 0, 0, &retR2
      },
      {
        "R2A", 0, 0, &retR2A
      },
      {
        "RES", 0, 0, &retRES
      },
      {
        "SAMPLE", 0, 1, &retSample
      }      
    };
#define NMAX_RETURN sizeof(table_returner)/sizeof(table_returner[0])

  int n_ret = 0;
  BSyntaxObject *result[NMAX_RETURN], *so;
  
  const char *str, *str_end;
  char buffer[256];
  
  str = arg_ret->Length()?arg_ret->String():"BETA:MSE:VAR";
  int is_all;
  while (1) {
    str_end = strpbrk(str,":=");
    if (!str_end)
      str_end = str+strlen(str);
    is_all = !strncasecmp("ALL", str, str_end-str);
    for (i = 0; i < NMAX_RETURN; i++) {
      if (is_all) {
	if (!table_returner[i].need_arg) {
	  if(*str_end && *str_end!=':') {
	    snprintf(buffer, 255, "Return option %s does not need arguments",
		     table_returner[i].name);
	    Warning(buffer);
	  } else {
	    so = table_returner[i].returner->Create(&str_end);
	    if (so) {
	      result[n_ret++] = so;
	      table_returner[i].used = 1;
	    } else {
	      snprintf(buffer, 255, "Could not create requested result '%s'",
		       table_returner[i].name);
	      Error(buffer);
	    }
	  }
	}
      } else {
	if (!strncasecmp(table_returner[i].name, str, str_end-str)) {
	  if (table_returner[i].used) {
	    snprintf(buffer, 255, "Return option %s already added",
		     table_returner[i].name);
	    Warning(buffer);
	    /* advance to next option */
	    if (*str_end=='=') {
	      str_end = strchr(str_end, ':');
	      if (!str_end)
		str_end = str+strlen(str);
	    }
	    break;
	  } else {
	    if (table_returner[i].need_arg) {
	      if (*str_end=='=') {
		++str_end;
	      } else {
		snprintf(buffer, 255, "Return option %s needs arguments",
			 table_returner[i].name);
		Warning(buffer);
		break;
	      }
	    } else if(*str_end && *str_end!=':') {
	      snprintf(buffer, 255, "Return option %s does not need arguments",
		       table_returner[i].name);
	      Warning(buffer);
	      break;
	    }
	    so = table_returner[i].returner->Create(&str_end);
	    if (so) {
	      result[n_ret++] = so;
	      table_returner[i].used = 1;
	    } else {
	      snprintf(buffer, 255, "Could not create requested result '%s'",
		       table_returner[i].name);
	      Error(buffer);
	    }
	  }
	  break;  
	}
      }
    }
    if (!is_all && i == NMAX_RETURN) {
      snprintf(buffer, 255, "Unrecongnized option '%s'", str);
      Warning(buffer);
    }
    if (*str_end)
      str = str_end+1;
    else
      break;
  }
  if(n_ret) {
    contens_.RobElement(BuildList(n_ret, result));
    
  } else {
    Warning("No result Set was created in SparseLinearRegression");
  }
  delete []residuals;
  cholmod_free_dense(&beta, &cm);
  cholmod_free_sparse(&F, &cm);
  cholmod_finish (&cm) ;
}

static
void my_cholmod_error(int status, char *file, int line, char *message)
{
  char buffer[1024];

  snprintf(buffer, 1024, "error at line %s:%d: %s", file, line, message);
  Error(buffer);
}
