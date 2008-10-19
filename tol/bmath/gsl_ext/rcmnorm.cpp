/* @(#)rcmnorm.c
 */

#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_randist.h>
#include "gsl_ext.h"

#include <assert.h>


//#define TRACE_ALL_DUMP

#if !defined(NDEBUG) && defined(TRACE_ALL_DUMP)
#define DUMP_MATRIX(msg,M)                      \
  do {                                          \
    printf("%s", msg);                          \
    if (M->size1<10 && M->size2<10)             \
      gsl_matrix_table(stdout,M,',');           \
  } while(0);

#define DUMP_VECTOR(msg,V)                      \
  do {                                          \
    printf("%s", msg);                          \
    if (V->size<10)                             \
      gsl_vector_table(stdout,V,',');           \
  } while(0);

#define GSL_CHECK_CONSTRAINTS
#define TRACE_INTERVALS 1
#else
#define DUMP_MATRIX(msg,M)
#define DUMP_VECTOR(msg,V)
#endif
#ifdef GSL_CHECK_CONSTRAINTS
static gsl_matrix * check_B  = NULL;
static gsl_vector * check_bx = NULL;

static
int gsl_check_constraints(gsl_vector * x, gsl_rcmnorm_workspace_t * work)
{
  const double eps = 1.0e-7;
  size_t i;
  
  /* verify that Bx <= b */
  gsl_blas_dgemv(CblasNoTrans, 1.0, check_B, x, 0.0, check_bx);
  for (i = 0; i < check_bx->size; i++) {
    if (gsl_vector_get(check_bx, i) > gsl_vector_get(work->b, i)+eps) {
      printf("i = %d\n", i);
      //gsl_vector_table(stdout, x, ',');
      GSL_ERROR("Invalid sample: constraints system does not hold", GSL_EDOM);
    }
  }
  return GSL_SUCCESS;
}

#endif

/*
 * Compute:
 *
 * D = B * A^-1
 * 
 */
inline
void rcmn_build_D(gsl_rcmnorm_workspace_t *work, gsl_matrix *B)
{
  work->D = gsl_matrix_alloc(B->size1,B->size2);
  if (work->cov_type==cov_scalar) {
    gsl_matrix_memcpy(work->D, B);
    return;
  }
  gsl_vector_view rowD;
  size_t i;
  
  switch (work->cov_type) {
  case cov_diag:
    /* A^-1=(sii)=W*/
    assert(work->W && work->W);
    
    gsl_matrix_memcpy(work->D, B);
    for (i = 0; i < B->size1; i++) {
      rowD = gsl_matrix_row(work->D,i);
      gsl_vector_mul(&(rowD.vector), work->W);
    }
    break;
  case fact_chol:
    /* A^-1=L*/
    assert(work->COV && work->COV->size1==work->D->size2);
    
    gsl_matrix_memcpy(work->D, B);
    gsl_blas_dtrmm(CblasRight, CblasLower, CblasNoTrans, CblasNonUnit,
                   1.0, work->COV, work->D);    
    break;
  case fact_ichol:
    /* A^-1=(L^-1)'*/
    gsl_matrix_memcpy(work->D, B);
    gsl_blas_dtrsm(CblasRight, CblasLower, CblasTrans, CblasNonUnit,
                   1.0, work->COV, work->D);    
    break;    
  case fact_svd:
    /* A^-1=V*W */
    gsl_matrix_set_zero(work->D);
    
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,       /* D := B*V */
                   1.0, B, work->COV, 0.0, work->D);

    /* D := D*W */
    for (i = 0; i < B->size1; i++) {
      rowD = gsl_matrix_row(work->D,i);
      gsl_vector_mul(&(rowD.vector), work->W);
    }
    break;
  case fact_isvd:
    /* A^-1=V*W^-1 */
    gsl_matrix_set_zero(work->D);
    
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,       /* D := B*V */
                   1.0, B, work->COV, 0.0, work->D); 
    /* D := D*W^-1 */
    for (i = 0; i < work->D->size1; i++) {
      rowD = gsl_matrix_row(work->D,i);
      gsl_vector_mul(&(rowD.vector), work->pinvW);
    }
    break;
    
  default:
    assert(0);
  }
  DUMP_MATRIX("\nD = \n",work->D);
}

/*
 * Compute z:=A*x
 *
 * alpha = A*mean
 *
 * z0 = A*x0
 * 
 */
inline void rcmn_apply_A(gsl_rcmnorm_workspace_t *work,
                         gsl_vector *z, const gsl_vector *x)
{
  if (work->cov_type==cov_scalar) {
    gsl_vector_memcpy(z, x);  /* z := x */
    return;
  }
  switch (work->cov_type) {
  case cov_diag:
    /* A=(1/sii)=W^-1 */
    assert(work->pinvW && work->pinvW->size==z->size);
    gsl_vector_memcpy(z, x);
    gsl_vector_mul(z,work->pinvW);
    break;
  case fact_chol:
    /* A=L^-1*/
    gsl_vector_memcpy(z, x);
    gsl_blas_dtrsv(CblasLower, CblasNoTrans, CblasNonUnit,
                   work->COV, z);
    break;
  case fact_ichol:
    /* A=L'*/
    gsl_vector_memcpy(z, x);
    gsl_blas_dtrmv(CblasLower, CblasTrans, CblasNonUnit,
                   work->COV, z); /* z := L'*z */
    break;
  case fact_svd:
    /* A=W^-1*V'*/

    gsl_vector_set_zero(z); /* may be no needed becase beta=0.0 ¿? */
    gsl_blas_dgemv(CblasTrans,
                   1.0, work->COV, x, 0.0, z); /* z = V'*x */
    gsl_vector_mul(z, work->pinvW); /* z := W^-1*z */
    break;
  case fact_isvd:
    /* A=W*V'*/

    gsl_vector_set_zero(z); /* may be no needed becase beta=0.0 ¿? */
    gsl_blas_dgemv(CblasTrans,
                   1.0, work->COV, x, 0.0, z); /* z = V'*x */
    gsl_vector_mul(z, work->W); /* z := W*z */
    break;
  default:
    assert(0);
  }  
}

/*
 * Compute: x = A^-1*z
 */
inline void rcmn_apply_invA(gsl_rcmnorm_workspace_t *work,
                            gsl_vector *x, const gsl_vector *z)
{
  if (work->cov_type==cov_scalar) {
    gsl_vector_memcpy(x, z);  /* x = z */
    return;
  }
  switch (work->cov_type) {
  case cov_diag:
    /* A^-1=(sii)=W */
    assert(work->W && work->W->size==z->size);
    gsl_vector_memcpy(x, z);  /* x = z */    
    gsl_vector_mul(x,work->W);
    break;
  case fact_chol:
    /* A^-1=L */
    gsl_vector_memcpy(x, z);  /* x = z */
    gsl_blas_dtrmv(CblasLower, CblasNoTrans, CblasNonUnit, work->COV, x);
    break;
  case fact_ichol:
    /* A^-1=(L')^-1 */
    gsl_vector_memcpy(x, z);  /* x = z */
    gsl_blas_dtrsv(CblasLower, CblasTrans, CblasNonUnit, work->COV, x);
    break;
  case fact_svd:
    /* A^-1=V*W */
 
    gsl_vector_memcpy(work->vec_tmp, z);  /* tmp = z */
    gsl_vector_mul(work->vec_tmp, work->W); /* tmp := W*tmp */
    gsl_vector_set_zero(x); /* may be not needed because beta=0.0 ? */
    gsl_blas_dgemv(CblasNoTrans,
                   1.0, work->COV, work->vec_tmp, 0.0, x); /* x = V*tmp */
    break;
  case fact_isvd:
    /* A^-1=V*W^-1 */
 
    gsl_vector_memcpy(work->vec_tmp, z);  /* tmp = z */
    gsl_vector_mul(work->vec_tmp, work->pinvW); /* tmp := W^-1*tmp */
    gsl_vector_set_zero(x); /* may be not needed because beta=0.0 ? */
    gsl_blas_dgemv(CblasNoTrans,
                   1.0, work->COV, work->vec_tmp, 0.0, x); /* x = V*tmp */
    break;
  default:
    assert(0);
  }
}

int gsl_rcmnorm_init(gsl_vector * mean,
                     cov_t cov_type,
                     double s2, gsl_matrix * COV,
                     gsl_vector * W,
                     gsl_matrix * B,
                     gsl_vector * b,
                     gsl_vector * x0,
                     gsl_rcmnorm_workspace_t *work)
{
  size_t i, j;
  gsl_vector_view dj;
  double dij;
  int * indexes_L = NULL;
  int * indexes_H = NULL;
  size_t n_low, n_high;
  int ret;
  gsl_matrix *V;
  
  work->COV = NULL;
  work->W = NULL;
  work->pinvW = NULL;
  work->D = NULL;
  work->b = NULL;
  work->alpha = NULL;
  work->low_set = NULL;
  work->high_set = NULL;
  work->z = NULL;
  work->bDz = NULL;
  work->vec_tmp = NULL;

  if (mean->size != B->size2)
    return GSL_EBADLEN;
  if (cov_type==fact_ichol || cov_type==fact_isvd)
    work->s = 1/sqrt(s2);
  else
    work->s = sqrt(s2);
  work->cov_type = cov_type;
  if (cov_type==cov_diag) {
    if (W->size != mean->size)
      return GSL_EBADLEN;
    work->W = gsl_vector_alloc(W->size);
    gsl_vector_memcpy(work->W, W);
    work->pinvW = gsl_vector_alloc(W->size);
    for (i = 0; i < W->size; i++) {
      gsl_vector_set(work->pinvW, i,
                     1/gsl_vector_get(work->W, i));
    }
  } else if (cov_type==full_chol||
             cov_type==fact_chol||
             cov_type==fact_ichol) {
    if (mean->size != COV->size1 || COV->size1 != COV->size2)
      return GSL_ENOTSQR;
    if (cov_type==full_chol) {
      work->COV = gsl_matrix_alloc(COV->size1,COV->size1);
      gsl_matrix_memcpy(work->COV, COV);
      if ((ret=gsl_linalg_cholesky_decomp(work->COV)) != GSL_SUCCESS) {
        gsl_rcmnorm_free(work);
        return ret;
      }
      work->cov_type = fact_chol;
#ifndef NDEBUG
      //printf("\nchol = \n");
      //gsl_matrix_table(stdout,work->COV,',');
#endif
    } else {
      work->COV = gsl_matrix_alloc(COV->size1,COV->size1);
      /* instead a memcpy, try to copy only the lower triangular part */
      gsl_matrix_memcpy(work->COV, COV);
    }
  } else if (cov_type==full_svd||
             cov_type==fact_svd||
             cov_type==fact_isvd) {
    if (cov_type==full_svd) {
      if (mean->size != COV->size1 || COV->size1 != COV->size2) {
        gsl_rcmnorm_free(work);
        return GSL_ENOTSQR;
      }
      work->COV = gsl_matrix_alloc(COV->size1,COV->size1);
      gsl_matrix_memcpy(work->COV, COV);
      V = gsl_matrix_alloc(COV->size1,COV->size1);
      work->W = gsl_vector_alloc(mean->size);
      work->vec_tmp = gsl_vector_alloc(mean->size);
      if ((ret=gsl_linalg_SV_decomp(work->COV,
                                    V, work->W,
                                    work->vec_tmp))!=GSL_SUCCESS) {
        gsl_matrix_free(V);
        gsl_rcmnorm_free(work);
        return ret;
      }
      gsl_matrix_free(V);
      /* take sqrt of W */
      for (i = 0; i < mean->size; i++) {
        double svi = gsl_vector_get(work->W, i);
        gsl_vector_set(work->W, i, sqrt(svi));
      }
      work->cov_type = fact_svd;
    } else {
      work->vec_tmp = gsl_vector_alloc(mean->size);
      work->COV = gsl_matrix_alloc(COV->size1,COV->size1);
      gsl_matrix_memcpy(work->COV, COV);
      work->W = gsl_vector_alloc(mean->size);
      gsl_vector_memcpy(work->W, W);
    }
    /* do pseudo-inverse */
    work->pinvW = gsl_vector_alloc(mean->size);
    double sv0=gsl_vector_get(work->W,0);
    gsl_vector_set(work->pinvW, 0, 1.0/sv0);
    for (i = 1; i < mean->size; i++) {
      double svi = gsl_vector_get(work->W, i);
      if (svi >= sv0*GSL_SQRT_DBL_EPSILON) {
        gsl_vector_set(work->pinvW, i, 1.0/svi);
      } else {
        gsl_vector_set(work->pinvW, i, 0.0);
        break;
      }
    }
    for (;i < mean->size; i++) {
      gsl_vector_set(work->pinvW, i, 0.0);
    }
  }
  
  DUMP_MATRIX("B = ", B);
  
  DUMP_VECTOR("b = ", b);
  
  /* D = B*inv(A) */
  rcmn_build_D(work, B);
  
  work->b = gsl_vector_alloc(b->size);
  gsl_vector_memcpy(work->b, b);

#ifdef GSL_CHECK_CONSTRAINTS
  if (check_bx)
    gsl_vector_free(check_bx);
  check_bx = gsl_vector_alloc(b->size);
  gsl_vector_set_zero(check_bx);
  if (check_B)
    gsl_matrix_free(check_B);
  check_B = gsl_matrix_alloc(B->size1,B->size2);
  gsl_matrix_memcpy(check_B, B);
#endif
  
  /* solve alpha <- A*mean */
  work->alpha = gsl_vector_alloc(mean->size);
  rcmn_apply_A(work, work->alpha, mean);
  
  /* compute the Low and High index set for each column of D */
  work->low_set = (int**)malloc(work->D->size2*sizeof(int*));
  work->high_set = (int**)malloc(work->D->size2*sizeof(int*));
  indexes_L = (int*)malloc(work->D->size1*sizeof(int));
  indexes_H = (int*)malloc(work->D->size1*sizeof(int));
  for (j = 0; j < work->D->size2; j++) {
    dj = gsl_matrix_column(work->D, j);
    n_low = 0; n_high = 0;
    for (i = 0; i < work->D->size1; i++) {
      dij = gsl_vector_get(&dj.vector, i);
      int dij_cmp  = gsl_fcmp(dij+1.0,1.0,DBL_EPSILON);
      if ( dij_cmp < 0) {
        /* add this index to Low set */
        indexes_L[n_low] = i;
        ++n_low;
      } else if (dij_cmp > 0) {
        /* add this index to High set */
        indexes_H[n_high] = i;
        ++n_high;
      }
    }
    if (n_low) {
      work->low_set[j] = (int*)malloc((n_low+1)*sizeof(int));
      memcpy(work->low_set[j], indexes_L, n_low*sizeof(int));
      work->low_set[j][n_low] = -1;
    } else {
      work->low_set[j] = NULL;
    }
    if (n_high) {
      work->high_set[j] = (int*)malloc((n_high+1)*sizeof(int));
      memcpy(work->high_set[j], indexes_H, n_high*sizeof(int));
      work->high_set[j][n_high] = -1;
    } else {
      work->high_set[j] = NULL;
    }    
  }
  free(indexes_L);
  free(indexes_H);
  work->z = gsl_vector_alloc(mean->size);

  gsl_vector *diff = gsl_vector_alloc(b->size);
  if (x0) {
    DUMP_VECTOR("x0 = ", x0);
    /* compute z0 = A*x0 */
    rcmn_apply_A(work, work->z, x0);
    DUMP_VECTOR("z0 = A*x0\n", work->z);

    /* Check if x0 hold the constraints */
    gsl_blas_dgemv(CblasNoTrans, 1.0, B, x0, 0.0, diff);  /* diff = B*x0 */
    gsl_vector_sub(diff, b);  /* diff := diff - b  */
    for (i = 0; i < diff->size; i++) {
      double item = gsl_vector_get(diff, i);
      if ( item > 0) {
        printf("B*x0 <= b does not hold (%g) at constraint %d\n ", item, i);
      }
    }
  } else 
    gsl_vector_set_zero(work->z);

#ifndef NDEBUG
  /* Check if z0 hold the transformed constraints D*z0 <= b */
  gsl_vector_memcpy(diff, b);
  /* diff = -D*z0 + b */
  gsl_blas_dgemv(CblasNoTrans, -1.0, work->D, work->z, 1.0, diff);
  for (i = 0; i < diff->size; i++) {
    double item = gsl_vector_get(diff, i);
    if ( item < 0) {
      printf("D*z0 <= b does not hold (%g) at constraint %d\n", item, i);
    }
  }
#endif

  gsl_vector_free(diff);
  
  work->bDz = gsl_vector_alloc(b->size);
  return GSL_SUCCESS;
}

int gsl_rectangle_alloc(gsl_vector * low, gsl_vector * high,
                        gsl_matrix **B, gsl_vector **b, gsl_vector ** x0)
{
  gsl_vector_view v_view;
  
  *B = gsl_matrix_alloc(2*low->size, low->size);
  *b = gsl_vector_alloc(2*low->size);
  gsl_matrix_set_zero(*B);
  v_view = gsl_matrix_diagonal(*B);
  gsl_vector_set_all(&v_view.vector, -1.0);
  v_view = gsl_matrix_subdiagonal(*B, low->size);
  gsl_vector_set_all(&v_view.vector, 1.0);
  v_view = gsl_vector_subvector(*b, 0, low->size);
  gsl_vector_memcpy(&v_view.vector, low);
  gsl_vector_scale(&v_view.vector, -1.0);
  v_view = gsl_vector_subvector(*b, low->size, low->size);
  gsl_vector_memcpy(&v_view.vector, high);
  *x0 = gsl_vector_alloc(low->size);
  gsl_vector_memcpy(*x0, low);
  gsl_vector_add(*x0, high);
  gsl_vector_scale(*x0, 0.5);
  return GSL_SUCCESS;
}

void gsl_rcmnorm_free(gsl_rcmnorm_workspace_t * work)
{
  size_t j;

  if (work->COV) {
    gsl_matrix_free(work->COV);
    work->COV = NULL;
  }
  if (work->W) {
    gsl_vector_free(work->W);
    work->W = NULL;
    assert(work->pinvW);
    gsl_vector_free(work->pinvW);
    work->pinvW = NULL;
  }

  if (work->D) {
    gsl_matrix_free(work->D);
    work->D = NULL;
  }
  
  if (work->b) {
    gsl_vector_free(work->b);
    work->b = NULL;
  }

  if (work->alpha) {
    for (j = 0; j < work->alpha->size; j++) {
      if (work->low_set[j])
        free(work->low_set[j]);
      if (work->high_set[j])
        free(work->high_set[j]);
    }
    gsl_vector_free(work->alpha);
    work->alpha = NULL;
    free(work->low_set);
    work->low_set = NULL;
    free(work->high_set);
    work->high_set = NULL;
  }

  if (work->z) {
    gsl_vector_free(work->z);
    work->z = NULL;
  }

  if (work->bDz) {
    gsl_vector_free(work->bDz);
    work->bDz = NULL;
  }

  if (work->vec_tmp) {
    gsl_vector_free(work->vec_tmp);
    work->vec_tmp = NULL;
  }
}

static
int gsl_find_low(gsl_rcmnorm_workspace_t * work,
                 gsl_vector * dj, int j, double * low)
{
  register int i;
  register int * idx;
  register double _low;
  
  idx = work->low_set[j];
  if (idx) {
    i = *idx;
    *low = gsl_vector_get(work->bDz, i)/gsl_vector_get(dj,i) + gsl_vector_get(work->z,j);      
    ++idx;
    while (*idx!=-1) {
      i = *idx;
      _low = gsl_vector_get(work->bDz, i)/gsl_vector_get(dj,i) + gsl_vector_get(work->z,j);
      if (_low > *low)
        *low = _low;
      ++idx;
    }
    return 1;
  } else {
    /* unbounded */
    return 0;
  }
}

static
int gsl_find_high(gsl_rcmnorm_workspace_t * work,
                  gsl_vector * dj, int j, double * high)
{
  register int i;
  register int * idx;
  register double _high;
  
  idx = work->high_set[j];
  if (idx) {
    i = *idx;
    *high = gsl_vector_get(work->bDz, i)/gsl_vector_get(dj,i) + gsl_vector_get(work->z,j);   
    ++idx;
    while (*idx!=-1) {
      i = *idx;
      _high = gsl_vector_get(work->bDz, i)/gsl_vector_get(dj,i) + gsl_vector_get(work->z,j);
      if (_high < *high)
        *high = _high;
      ++idx;
    }
    return 1;
  } else {
    /* unbounded */
    return 0;
  }  
}

int gsl_rcmnorm_draw(const gsl_rng           *r,
                     gsl_rcmnorm_workspace_t *work,
                     gsl_vector              *x)
{
  const int iter = 10;
  double lim_inf = -100, lim_sup = 100;
  gsl_vector_view dj;
  int a, b;
  size_t j;
  double z, m;
  
  /* update bDz in order to fix numerical errors accumulated
   * we can try not to refresh in order to verify performance
   */

  /* b - Dz = -D*z + b */
  gsl_vector_memcpy(work->bDz, work->b);
  gsl_blas_dgemv(CblasNoTrans, -1.0, work->D, work->z, 1.0, work->bDz);

  DUMP_VECTOR("b-Dz = ", work->bDz);

#define NINT 3 
  for (j = 0; j < work->z->size; j++) {
    dj = gsl_matrix_column(work->D, j);
    b = gsl_find_low(work, &dj.vector, j, &lim_inf);
    a = gsl_find_high(work, &dj.vector, j, &lim_sup);
    m = gsl_vector_get(work->alpha,j);
    if (a)
      if (b) {
#ifdef TRACE_INTERVALS
        printf("sampling from (%g,%g)\n", lim_inf, lim_sup);
#endif
        /* be careful with this test!!!!!*/
        if (lim_inf>lim_sup) {
#ifdef TRACE_INTERVALS
          printf("caso jodido %d\n", j);
#endif
          z = gsl_vector_get(work->z, j);
          //z = m;
        }
        else
          z = gsl_rtnorm_combo(r, m, work->s, lim_inf, lim_sup);
      } else {
#ifdef TRACE_INTERVALS
        printf("sampling from (-inf,%g)\n", lim_sup);
#endif      
        z = gsl_rtanorm_combo(r, m, work->s, lim_sup, iter);
      }
    else
      if (b) {
#ifdef TRACE_INTERVALS
        printf("sampling from (%g,+inf)\n", lim_inf);
#endif        
        z = gsl_rtbnorm_combo(r, m, work->s, lim_inf, iter);
      } else {
#ifdef TRACE_INTERVALS
        printf("sampling from (-inf,+inf)\n");
#endif        
        z = m + gsl_ran_gaussian(r,work->s);
      }
    /* update b-Dz with this new sample, z'j is the new value
     *     
     *     bDz = b - D*z = b - D_*z_ - dj*zj
     *     zj <- z'j
     *     bDz' =  b - D_*z_ - dj*z'j + dj*zj - dj*zj
     *          =  bDz + dj*(zj-z'j)
     */
    gsl_blas_daxpy(gsl_vector_get(work->z,j)-z, &dj.vector, work->bDz);
    /* now replace the new sample */
#ifdef TRACE_INTERVALS
    printf("z = %g\n", z);
#endif
    gsl_vector_set(work->z, j, z);
  }
  if (x) {
    /* compute x from work->z : x = inv(A)*z*/
    rcmn_apply_invA(work, x, work->z);
#ifdef GSL_CHECK_CONSTRAINTS
    gsl_check_constraints(x, work);
#endif
  }
  return GSL_SUCCESS;
}

void gsl_rcmnorm_last(gsl_rcmnorm_workspace_t *work, gsl_vector *x)
{
  if (x) {
    /* compute x from work->z : x = A^-1*z*/
    rcmn_apply_invA(work, x, work->z);
  }
}
