/* tmvnd.c : Evaluate the log-density of a truncated multivariate normal.

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

#include <assert.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>

#include "gsl_ext.h"

/*
 * gsl_mvn_density --
 *
 * Evaluate the log-density of a multivariate normal. No checking is
 * done on asserting the simmetry of sigma.
 * 
 */

int gsl_mvn_init(gsl_matrix *sigma, int use_sigma,
                 gsl_mvn_workspace_t * mvn)
{
  if (sigma->size1 != sigma->size2)
    return GSL_ENOTSQR; /* not square matrix */
  gsl_svd_init(sigma, use_sigma, &(mvn->svd));
  /* n * log(2Pi) + log(det) */
  mvn->K = sigma->size1 * (M_LN2 + M_LNPI) + log(mvn->svd.det);
  return GSL_SUCCESS;
}

void gsl_mvn_done(gsl_mvn_workspace_t * mvn)
{
  gsl_svd_done(&(mvn->svd));
}

double gsl_log_mvn_density(gsl_vector *x, gsl_vector *mean,
                           gsl_mvn_workspace_t *mvn)
{
#ifndef NDEBUG
  int status;
#endif
  size_t i;  
  gsl_vector *x_m = gsl_vector_alloc(x->size); /* x-m */
  gsl_vector *y = gsl_vector_alloc(x->size);  /* U'*(x-m) */
  double yt_s_y = 0.0; /* y'* S^{-1} * y */
  gsl_matrix *V = mvn->svd.V;
  gsl_vector *S_1 = mvn->svd.S_1;
  unsigned char *flag_zero = mvn->svd.flag_zero;
  
  gsl_vector_memcpy(x_m, x);
  gsl_vector_sub(x_m, mean);
  gsl_vector_set_zero(y); /* I think this not needed as beta = 0.0 */

  /* compute y = V'*x_m */
#ifndef NDEBUG
  status = gsl_blas_dgemv(CblasTrans, 1.0, V, x_m, 0.0, y);
  assert(status==GSL_SUCCESS);
#else
  gsl_blas_dgemv(CblasTrans, 1.0, V, x_m, 0.0, y);
#endif
  /* compute y'* S^{-1} * y */
  for (i=0; i < x->size; i++) {
    if (!flag_zero[i])
      yt_s_y += S_1->data[i]*y->data[i]*y->data[i];
  }
  gsl_vector_free(y);
  gsl_vector_free(x_m);
  return -0.5 * (mvn->K +  yt_s_y);
}

typedef struct
{
  gsl_vector * mean;
  gsl_mvn_workspace_t * mvn;
} monte_mvn_t;

static
double gsl_monte_mvn(double * x, size_t dim, void * p)
{
  gsl_vector_view x_view = gsl_vector_view_array(x, dim);
  monte_mvn_t *mvn_data = (monte_mvn_t *)p;
  return exp(gsl_log_mvn_density(&(x_view.vector),
                                 mvn_data->mean, mvn_data->mvn));
}

int gsl_tmvn_init(gsl_vector *mean, gsl_matrix *sigma, int use_sigma,
                  double *a, double *b,
                  gsl_rng *r, size_t iter,
                  gsl_tmvn_workspace_t *tmvn)
{
  gsl_monte_vegas_state *monte_s = gsl_monte_vegas_alloc(mean->size);
  monte_mvn_t mvn_data = { mean, &(tmvn->mvn) };
  gsl_monte_function MVN = { &gsl_monte_mvn, mean->size, &mvn_data };
  double err;
  
  gsl_mvn_init(sigma, use_sigma, &(tmvn->mvn));
  
  gsl_monte_vegas_integrate(&MVN, a, b, mean->size, iter, r, monte_s,
                            &(tmvn->cdf), &err);
  do
  {
    gsl_monte_vegas_integrate(&MVN, a, b, mean->size, iter/5, r, monte_s,
                              &(tmvn->cdf), &err);
    printf ("result = % .6f sigma = % .6f "
            "chisq/dof = %.1f\n", tmvn->cdf, err, monte_s->chisq);
  }
  while (fabs (monte_s->chisq - 1.0) > 0.5);

  printf("monte_vegas err =%lf\n", err);
  tmvn->lcdf = log(tmvn->cdf);
  gsl_monte_vegas_free(monte_s);
  return GSL_SUCCESS;
}

void gsl_tmvn_done(gsl_tmvn_workspace_t * tmvn)
{
  gsl_mvn_done(&(tmvn->mvn));
}

/*
 * gsl_tmvn_density --
 *
 * Evaluate the log-density of a truncated multivariate normal.
 * 
 */

double gsl_log_tmvn_density(gsl_vector * x, gsl_vector *mean,
                            gsl_tmvn_workspace_t *tmvn,
                            double *a, double *b)
{
  size_t i;
  double v;
  
  /* verify if x is in region */
  for (i = 0; i < x->size; i++) {
    v = x->data[i*x->stride];
    if (gsl_fcmp(v, a[i], 1.0E-7)==-1 || gsl_fcmp(v, b[i], 1.0E-7)==1)
      return 0;
  }
  return gsl_log_mvn_density(x, mean, &(tmvn->mvn)) - tmvn->lcdf;
}
