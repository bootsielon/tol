/* gsl_linalg_ext.c : Linnear algebra algorithm implemented in tol-project
                      following the gsl API.

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

#include <gsl/gsl_math.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_randist.h>
#include "gsl_ext.h"

/*
 * gsl_svd_build --
 *
 * Allocate a workspace to be used by function needing a SVD matrix.
 */

int gsl_svd_init(gsl_matrix *A, int use_A, gsl_svd_matrix_t *svd)
{
  if (use_A) {
    svd->U = A;
    svd->own_U = 0;
  } else {
    svd->U = gsl_matrix_alloc(A->size1,A->size2);
    svd->own_U =1;
  }
  svd->S = gsl_vector_alloc(A->size2);
  svd->S_1 = gsl_vector_alloc(A->size2);
  svd->V = gsl_matrix_alloc(A->size2,A->size2);
  svd->work = gsl_vector_alloc(A->size2);
  svd->flag_zero = (unsigned char*)malloc(sizeof(unsigned char)*A->size2);
  svd->work_mult = NULL;
  return gsl_svd_set(A,svd);  
}

/*
 * gsl_svd_set --
 *
 * this function is useful when we want to use a SVD and take advantage of a
 * previously alloced space with gsl_svd_init. The dimension of A must be the
 * same as the one used in the first call to gsl_svd_unit.
 */

int gsl_svd_set(gsl_matrix *A, gsl_svd_matrix_t *svd)
{
  size_t i;
  double v;
  
  if (svd->U!=A) {
    gsl_matrix_memcpy(svd->U, A);
  }
  gsl_linalg_SV_decomp(svd->U, svd->V, svd->S, svd->work);
  svd->det = 1.0;
  for (i = 0; i < A->size2; i++) {
    v = svd->S->data[i];
    if (fabs(v)<=1.0e-13) {
      /* consider it as zero!!! */
      svd->S_1->data[i] = 0.0;
      svd->flag_zero[i] = 1;
    } else {
      svd->det *= v;
      svd->S_1->data[i] = 1/v;
      svd->flag_zero[i] = 0;
    }
  }
  svd->work_mult = NULL;
  return GSL_SUCCESS;  
}

/*
 * gsl_svd_done --
 *
 * Release a SVD workspace.
 */

void gsl_svd_done(gsl_svd_matrix_t * svd)
{
  if (svd->own_U && svd->U) {
    gsl_matrix_free(svd->U);
    svd->U = NULL;
    svd->own_U = 0;
  }
  if (svd->S) {
    gsl_vector_free(svd->S);
    svd->S = NULL;
  }
  if (svd->V) {
    gsl_matrix_free(svd->V);
    svd->V = NULL;
  }
  if (svd->work)
    gsl_vector_free(svd->work);
  if (svd->work_mult)
    gsl_matrix_free(svd->work_mult);
  if (svd->flag_zero) {
    free(svd->flag_zero);
    svd->flag_zero = NULL;
  }
}

/*
 * gsl_svd_pinverse --
 *
 * Compute pseudo-inverse from a SVD.
 * 
 */

int gsl_svd_pinverse(gsl_svd_matrix_t * svd, gsl_matrix ** pinv)
{
  gsl_vector_view column_view;
  gsl_vector * column;
  size_t M = gsl_svd_size1(svd);
  size_t N = gsl_svd_size2(svd);
  size_t j;
  
  if (M!=N) 
    return GSL_ENOTSQR;
  if (*pinv == NULL) {
    *pinv = gsl_matrix_alloc(N,N);
  }
  if (svd->work_mult==NULL)
    svd->work_mult = gsl_matrix_alloc(N,N);
  
  gsl_matrix_memcpy(svd->work_mult,svd->V);
  /* compute first tmp = V*S^{-1} */
  for (j=0; j<N; j++) {
    column_view = gsl_matrix_column(svd->work_mult, j);
    column = &(column_view.vector);
    if (svd->flag_zero[j])
      gsl_vector_set_zero(column);
    else
      gsl_vector_scale(column, gsl_vector_get(svd->S_1,j));
  }
  /* now compute pinv = tmp * U' */
  /* do not init *pinv just because beta == 0.0 */
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,
                 1.0,svd->work_mult,svd->U,0.0,*pinv);
  
  return GSL_SUCCESS;
}

/*
 * gsl_kronecker_prod --
 *
 *   K is the kronecker product of A * B
 *   If K is NULL then it is alloced inside the function.
 */

void gsl_kronecker_prod(const gsl_matrix *A, const gsl_matrix *B,
			gsl_matrix ** K)
{
  //Matrix K is the Kronecker product of matrices A and B
  int i, j;
  int ra=A->size1;
  int rb=B->size1;
  int ca=A->size2;
  int cb=B->size2;
  double x;
  gsl_matrix_view sub_KB;
  
  if (!*K) {
    *K = gsl_matrix_alloc(ra*rb,ca*cb);
  }
  for(i=0; i<ra; ++i){
    for(j=0; j<ca; ++j){
      x=gsl_matrix_get(A,i,j);
      sub_KB = gsl_matrix_submatrix(*K,i*rb,j*cb,rb,cb);
      gsl_matrix_memcpy(&sub_KB.matrix,B);
      gsl_matrix_scale(&sub_KB.matrix,x);
    }
  }
}

void gsl_DiagVector(const gsl_vector *v, gsl_matrix *D){
  gsl_vector_view v_x = gsl_matrix_diagonal(D);
  gsl_vector_memcpy(&v_x.vector,v);
}




void gsl_vecop(const gsl_matrix *A, gsl_vector *V){
  int i, r=A->size1, c=A->size2;
  gsl_vector_view v;
  gsl_vector *w = gsl_vector_alloc(r);
  for(i=0; i<c; i++){
    v=gsl_vector_subvector(V,i*r,r);
    gsl_matrix_get_col(w,A,i);
    gsl_vector_memcpy(&v.vector,w);
  }
  gsl_vector_free(w);
}



double gsl_Matdet(const gsl_matrix *M){
  int signum, N=M->size1;
  double det;
  gsl_matrix *A = gsl_matrix_alloc(N,N);
  gsl_permutation *p = gsl_permutation_alloc(N);
  gsl_matrix_memcpy(A,M);
  gsl_linalg_LU_decomp(A,p,&signum);
  det=gsl_linalg_LU_det(A,signum);
  gsl_matrix_free(A);
  gsl_permutation_free(p);
  return det;
}

double gsl_MatSVDdet(const gsl_matrix *M){
  int i, N = M->size1;
  double det;
  gsl_matrix *U = gsl_matrix_alloc(N,N);
  gsl_matrix *V = gsl_matrix_alloc(N,N);
  gsl_vector *S = gsl_vector_alloc(N);
  gsl_vector *work = gsl_vector_alloc(N);
  gsl_matrix_memcpy(U,M);
  gsl_linalg_SV_decomp(U,V,S,work);
  det=1;
  for(i=0; i<N; ++i){
    det*=S->data[i*S->stride];
  }
  gsl_matrix_free(U);
  gsl_matrix_free(V);
  gsl_vector_free(S);
  gsl_vector_free(work);
  return det;
}

void gsl_MatInverse(const gsl_matrix *M, gsl_matrix *Mi){
  int signum, N=M->size1;
  gsl_permutation *p = gsl_permutation_alloc(N);
  gsl_matrix *A = gsl_matrix_alloc(N,N);
  gsl_matrix_memcpy(A,M);
  gsl_linalg_LU_decomp(A,p,&signum);
  gsl_linalg_LU_invert(A,p,Mi);
  gsl_permutation_free(p);
  gsl_matrix_free(A);
}

void gsl_MatSVDInverse(const gsl_matrix *M, gsl_matrix *Mi){
  int i, N = M->size1;
  gsl_matrix *U = gsl_matrix_alloc(N,N);
  gsl_matrix *Ut = gsl_matrix_alloc(N,N);
  gsl_matrix *V = gsl_matrix_alloc(N,N);
  gsl_matrix *D = gsl_matrix_alloc(N,N);
  gsl_vector *S = gsl_vector_alloc(N);
  gsl_vector *work = gsl_vector_alloc(N);

  gsl_matrix_memcpy(U,M);
  gsl_linalg_SV_decomp(U,V,S,work);

  for(i=0; i<N; ++i){
    S->data[i] = 1/(S->data[i]);
  }
  gsl_DiagVector(S,D);
  //
  gsl_vector_free(S);
  gsl_vector_free(work);
  //
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,1.0,D,U,0.0,Ut);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,V,Ut,0.0,Mi);
  //
  gsl_matrix_free(U);
  gsl_matrix_free(Ut);
  gsl_matrix_free(V);
  gsl_matrix_free(D);
}



double gsl_MatSVDdetInverse(const gsl_matrix *M, gsl_matrix *Mi){
  int i;
  int N = M->size1;
  double det;
  gsl_matrix *U = gsl_matrix_alloc(N,N);
  gsl_matrix *Ut = gsl_matrix_calloc(N,N);
  gsl_matrix *V = gsl_matrix_alloc(N,N);
  gsl_matrix *D = gsl_matrix_calloc(N,N);
  gsl_vector *s = gsl_vector_alloc(N);
  gsl_vector_view S;
  gsl_vector *work = gsl_vector_alloc(N);

  S = gsl_matrix_diagonal(D);
  gsl_matrix_memcpy(U,M);
  gsl_linalg_SV_decomp(U,V,&S.vector,work);
  gsl_vector_memcpy(s,&S.vector);
  det =1;
  for(i=0; i<N; ++i){
    det*= gsl_vector_get(&S.vector,i);
    gsl_vector_set(&S.vector, i, 1/gsl_vector_get(&S.vector,i));
  }
  //
  gsl_vector_free(s);
  gsl_vector_free(work);
  //
  gsl_matrix_set_zero(Mi);
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,  1.0,D,U, 0.0,Ut);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,V,Ut,0.0,Mi);
  //
  gsl_matrix_free(U);
  gsl_matrix_free(Ut);
  gsl_matrix_free(V);
  gsl_matrix_free(D);
  return(det);
}




void gsl_Solve_LU(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x){
  int s, n=b->size;
  int r = A->size1, c = A->size2;
  gsl_matrix *M = gsl_matrix_alloc(r,c);
  gsl_permutation *p=gsl_permutation_alloc(n);
  gsl_linalg_LU_decomp(M,p,&s);
  gsl_linalg_LU_solve(M,p,b,x);
  gsl_permutation_free(p);
  gsl_matrix_free(M);
}


void gsl_rwish(const gsl_rng *r, const int nu, const gsl_matrix *Sigma, 
	      gsl_matrix *X){
  //Slow version
  //Works with **
  int i, j;
  double a;
  int N = Sigma->size1;
  gsl_matrix *S = gsl_matrix_alloc(N,N);
  gsl_matrix *Z = gsl_matrix_alloc(N,N);
  gsl_vector *v = gsl_vector_alloc(N);
  gsl_vector *mu = gsl_vector_calloc(N);
  gsl_matrix *Y = gsl_matrix_alloc(N,1);
  // Esto es solo si X está definida con **no es necesaria
  gsl_matrix_set_all(X,0);
  //
/*   if (!*X) { */
/*     *X = gsl_matrix_calloc(N,N); */
/*   } */
  //Cholesky
   gsl_matrix_memcpy(S, Sigma);
   gsl_linalg_cholesky_decomp(S);
  for(i=0; i<nu; ++i){
    for(j=0; j<N; ++j){
      a=gsl_ran_ugaussian(r);
      gsl_vector_set(v,j,a);
    }
    gsl_blas_dtrmv(CblasLower,CblasNoTrans,CblasNonUnit,S,v);
    gsl_matrix_set_col(Y,0,v);
    gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,Y,Y,0,Z);
    //    gsl_matrix_add(*X,Z);
    gsl_matrix_add(X,Z);
  }
  //
  gsl_vector_free(v);
  gsl_vector_free(mu);
  gsl_matrix_free(Y);
  gsl_matrix_free(S);
  gsl_matrix_free(Z);

}


void gsl_riwish(const gsl_rng *r, const int nu, const gsl_matrix *Sigma, 
	      gsl_matrix *X){
  //Problems with ** (segmentation fault)
  int N = Sigma->size1;
  gsl_matrix *S = gsl_matrix_alloc(N,N);
  gsl_matrix *Y = gsl_matrix_alloc(N,N);
/*   if (!*X) { */
/*     *X = gsl_matrix_alloc(N,N); */
/*   } */
  gsl_MatInverse(Sigma,S);
  gsl_rwish(r,nu,S,Y);
/*   gsl_MatInverse(Y,&X); */
  gsl_MatInverse(Y,X);
  gsl_matrix_free(S);
  gsl_matrix_free(Y);

}
