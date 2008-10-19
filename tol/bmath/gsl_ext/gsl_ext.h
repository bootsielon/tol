/* @(#)gsl_ext.h
 */

#ifndef _GSL_EXT_H
#define _GSL_EXT_H 1

#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_vector.h>


__BEGIN_DECLS

/*
 * write a matrix in a FILE using a table format.
 */

void gsl_matrix_table(FILE * f, gsl_matrix * m, char sep);

/*
 * write a vector in a FILE using a table format as a row.
 */

void gsl_vector_table(FILE * f, gsl_vector * v, char sep);

/*
 * Random from a truncated normal.
 */

double gsl_rtnorm_combo(const gsl_rng * r, double m, double s,
                    double below, double above);

/*
 * Another sampler from a truncated normal.
 * What's the difference between this and the previous one?
 */

double gsl_rtnorm(const gsl_rng * r, double m, double s,
              double below, double above);

/*
 * Sampler from a truncated normal. This is the version for below truncation.
 */

double gsl_rtbnorm_combo (const gsl_rng * r, double m, double s,
                      double below, int iter);

/*
 * Sampler from a truncated normal. Truncated only for above.
 */

double gsl_rtanorm_combo (const gsl_rng * r, double m, double s,
                          double above, int iter);

/*
 * Random from a truncated normal for any posible pair of values
 * of below and above, including any feasible combination of 
 * finite or not finite values
 */

double gsl_rtabnorm_combo(const gsl_rng * r, double m, double s,
             double below, double above, int iter);

/*
 * Sampler from a constrained multivariate normal.
 * 
 */

/* The covariance matrix could be given in different forms:
 *
 *   A*COV*A' = I, COV = A^-1*(A^-1)'
 *
 * - full_chol: non-factored matrix: COV, cholesky is used to factor it.
 *
 * - full_svd: non-factored matrix: COV, SVD is used to factor it.
 *   
 * - constant diag : COV = s2*I, A^-1=I, A=I
 *
 * - non-constant diag: COV=(sii^2)=(sii)*(sii),
 *                          A^-1=(sii), A=(1/sii)
 *
 * - cholesky factored: COV=L*L', A^-1=L, A=L^-1
 *
 * - inversed cholesky factored: COV^-1=L*L', COV=(L')^-1*(L^-1),
 *                               A^-1=(L')^-1, A=L'
 *
 * - svd factored: COV=V*W*W*V', V'*V=I,V*V'=I,
 *                 COV=A^-1*A'^-1,A^-1=V*W, A=W^-1*V'
 *
 * - inversed svd factored: COV^-1=V*W*W*V', V'*V=I,V*V'=I,
 *                          COV=V*W^-1*W^-1*V'
 *                          COV=A^-1*A'^-1 , A^-1=V*W^-1, A=W*V'
 *
 *                 
 */
typedef enum {full_chol,
              full_svd,
              cov_scalar,
              cov_diag,
              fact_chol,
              fact_ichol,
              fact_svd,
              fact_isvd} cov_t;

/*  this structure contains all the information needed to generate a
 *  constrained multivariate normal chain.  A*COV*A' = I, COV must be positive
 *  semidefinite symmetric matrix, in this way COV = V*S^2*V' = C*C',
 *  A=inv(C). C = V*D, inv(C) = inv(S)*V', where inv(.) means pseudoinverse.
 */

typedef struct 
{
  double s;

  cov_t cov_type;
  /*
    COV will contain:

    fchol,fact_chol ==> LL'
    fsvd,fact_svd
    scalar,diag ==> NULL (don't care)
    fsvd,fact_svd ==> V factor of V*W2*V'
   */
  gsl_matrix *COV;
  /*
    W will contain:

    svd ==> W factor from COV=V*W*W*V'
    !svd ==> NULL (don't care)
   */
  gsl_vector *W;
  /*
   * pseudo-inverse of W: 1/wi if (wi>=e), 0 if (wi<e)
   */
  gsl_vector *pinvW;

  gsl_matrix * D;      /* D = B*A^-1
                          scalar==> B*(s)
                          diag==>B*(sii)
                          cholesky==>B*L
                          svd ==> B*V*W
                        */

  gsl_vector * b;
  gsl_vector * alpha;  /* alpha = A*mu
                          scalar==>(1/s)*mu
                          diag==>(1/sii)*mu
                          cholesky==>L^-1*mu
                          svd==>W^-1*V'*mu */
  
  int ** low_set;      /* indexes defining the lower set */
  int ** high_set;     /* indexes defining the upper set */
  gsl_vector * z;      /* current z sample */
  gsl_vector * bDz;    /* current value of b-D*z */
  gsl_vector * vec_tmp;    /* temporal vector used in BLAS operations,
                              normaly when SVD factorization is used */
} gsl_rcmnorm_workspace_t;

/*
 * 
 */

int gsl_rcmnorm_init(gsl_vector * mean,
                     cov_t cov_type,
                     double s2, gsl_matrix * COV,
                     gsl_vector * W,
                     gsl_matrix * B,
                     gsl_vector * b,
                     gsl_vector * x0,
                     gsl_rcmnorm_workspace_t * w);

/*
 *
 */ 

int gsl_rectangle_alloc(gsl_vector * low, gsl_vector * high,
                        gsl_matrix **B, gsl_vector **b, gsl_vector ** x0);

/*
 * 
 */

void gsl_rcmnorm_free(gsl_rcmnorm_workspace_t * w);
                                            
/*
 * 
 */

int gsl_rcmnorm_draw(const gsl_rng * r, gsl_rcmnorm_workspace_t * w, gsl_vector * x);

/*
 * 
 */

void gsl_rcmnorm_last(gsl_rcmnorm_workspace_t * w, gsl_vector * x);

typedef struct
{
  gsl_matrix * U;
  gsl_vector * S;
  gsl_matrix * V;
  gsl_vector * work;
  gsl_matrix * work_mult;
  double det; /* (pseudo) determinant */
  gsl_vector * S_1; /* S_1 = (pseudo) Inv(S) */
  unsigned char * flag_zero; /* flag indicating the "zero" elements in S */
  int own_U;
} gsl_svd_matrix_t;

#define gsl_svd_size1(svd)   (svd)->U->size1
#define gsl_svd_size2(svd)   (svd)->U->size2
#define gsl_svd_U(svd)       (svd)->U
#define gsl_svd_S(svd)       (svd)->S
#define gsl_svd_S_1(svd)     (svd)->S_1
#define gsl_svd_V(svd)       (svd)->V
#define gsl_svd_det(svd)     (svd)->det
#define gsl_svd_fzero(svd,i) (svd)->flag_zero[i]

/*
 * gsl_svd_init --
 *
 * Allocate a workspace to be used by function needing a SVD matrix.
 * svd->U!=NULL ==> svd->U==A, thus we will reuse A space when performing
 * SVD.
 * 
 */

int gsl_svd_init(gsl_matrix * A, int use_A, gsl_svd_matrix_t * svd);

/*
 * gsl_svd_set --
 *
 * this function is useful when we want to use a SVD and take advantage of a
 * previously alloced space with gsl_svd_init. The dimension of A must be the
 * same as the one used in the first call to gsl_svd_unit.
 */

int gsl_svd_set(gsl_matrix * A, gsl_svd_matrix_t * svd);

/*
 * gsl_svd_free --
 *
 * Release a SVD workspace.
 */

void gsl_svd_done(gsl_svd_matrix_t * svd);

/*
 * gsl_svd_pinverse --
 *
 * Compute pseudo-inverse from a SVD.
 * 
 */

int gsl_svd_pinverse(gsl_svd_matrix_t * svd, gsl_matrix ** pinv);

/*
 * Multivariate normal workspace --
 */

typedef struct 
{
  gsl_svd_matrix_t svd;
  double K; /* n * log(2Pi) + 0.5*log(det) */
} gsl_mvn_workspace_t;

/*
 * gsl_mvn_init --
 *
 * Allocate a workspace to be used in functions needing to compute normal
 * multivariate density.  svd->U!=NULL ==> svd->U==sigma, thus we will reuse
 * sigma space when performing SVD.. 
 */

int gsl_mvn_init(gsl_matrix *sigma, int use_sigma, gsl_mvn_workspace_t * mvn);

void gsl_mvn_done(gsl_mvn_workspace_t * mvn);
/*
 * Truncated multivariate normal workspace --
 */

typedef struct 
{
  gsl_mvn_workspace_t mvn;
  double cdf; /* integral(mvn;a,b) */
  double lcdf; /* log(cdf) */
} gsl_tmvn_workspace_t;

/*
 * gsl_tmvn_init --
 *
 * Allocate a workspace to be used in functions needing to compute truncated
 * normal multivariate density.  svd->U!=NULL ==> svd->U==sigma, thus we will
 * reuse sigma space when performing SVD.
 */

int gsl_tmvn_init(gsl_vector *mean, gsl_matrix *sigma, int use_sigma,
                  double *a, double *b,
                  gsl_rng *r, size_t iter,
                  gsl_tmvn_workspace_t *tmvn);

void gsl_tmvn_done(gsl_tmvn_workspace_t * tmvn);

/*
 * gsl_mvn_density --
 *
 * Evaluate the log-density of a multivariate normal. No checking is
 * done on asserting the simmetry of sigma.
 * 
 */

double gsl_log_mvn_density(gsl_vector *x, gsl_vector *mean,
                           gsl_mvn_workspace_t *mvn);

/*
 * gsl_tmvn_density --
 *
 * Evaluate the log-density of a truncated multivariate normal. No checking is
 * done on asserting the simmetry of sigma.
 * 
 */

double gsl_log_tmvn_density(gsl_vector *x, gsl_vector *mean,
                            gsl_tmvn_workspace_t *tmvn,
                            double *a, double *b);

/*
 * gsl_kronecker_prod --
 *
 *   K is the kronecker product of A * B.
 *   If K is NULL then it is alloced inside the function.
 */
           
void gsl_kronecker_prod(const gsl_matrix *A, const gsl_matrix *B,
			gsl_matrix ** K);
/* gsl_DiagVector */

/* creates a diagonal matrix D with the diagonal elements  */
/* equal to a given vector V */


void gsl_DiagVector(const gsl_vector *v, gsl_matrix *D);


/* gsl_vecop(A,V) */

/* This is the vech operator.  */
/* It stacks the rows of matrix A into a vector V */

void gsl_vecop(const gsl_matrix *A, gsl_vector *V);


/* gsl_MatSVDInverse */

/* Calculate the pseudo inverse of matrix using the SVD decomposition */
/* gsl_MatSVDinverse(A, Ainverse) */

void gsl_MatSVDInverse(const gsl_matrix *M, gsl_matrix *Mi);

/* gsl_MatLUInverse */

/* Calculate the pseudo inverse of matrix using the LU decomposition */
/* gsl_Matinverse(A, Ainverse) */

void gsl_MatInverse(const gsl_matrix *M, gsl_matrix *Mi);

/* gsl_Matdet */
/* Returns the determinant of Matrix using the LU decomposition */

double gsl_Matdet(const gsl_matrix *M);

/* gsl_MatSVDdet */
/* Returns the determinant of Matrix using the SVD decomposition */

double gsl_MatSVDdet(const gsl_matrix *M);

/* gsl_MatSVDdetInverse */
/* Simultaneously get determinant and  inverse of a matrix */

double gsl_MatSVDdetInverse(const gsl_matrix *M, gsl_matrix *Mi);



/* gsl_Solve_LU */
/* Solves the systems AX=B with the LU decomposition */

void gsl_Solve_LU(const gsl_matrix *A, const gsl_vector *b, gsl_vector *x);

/* gsl_rwish */
/* draws random variable variable from Wishart(nu,S) */
/* where nu is the degrees of freedom and  */
/* S is a symmetric, pos definite k*k scale matrix */

void gsl_rwish(const gsl_rng *r, const int nu, const gsl_matrix *Sigma, 
	      gsl_matrix *X);

/* gsl_riwish */
/* draws random variable variable from IWishart(nu,S) */
/* where nu is the degrees of freedom and  */
/* S is a symmetric, pos definite k*k inverse scale matrix */
/* coherent with MCMCpack, not so much with Gelman et al */

void gsl_riwish(const gsl_rng *r, const int nu, const gsl_matrix *Sigma, 
	      gsl_matrix *X);


__END_DECLS

#endif /* _GSL_EXT_H */
