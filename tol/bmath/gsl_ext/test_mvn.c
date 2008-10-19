/*

gcc -g2 -Wall -I/usr/local/include  test_mvn.c gsl_util_ext.c gsl_linalg_ext.c tmvnd.c -L/usr/local/gsl/lib -lgsl -lgslcblas

*/

#include <gsl/gsl_math.h>
#include <assert.h>
#include <time.h>
#include "gsl_ext.h"

gsl_rng *r;

void test_dim2()
{
  double sigma[] = {0.478515478675,	0.190837240583,
                    0.190837240583,	0.836015981203};
  double mu[] = { 0.0,   0.0 };
  double xl[] = { 0.0,   0.0 };
  double xu[] = { 10.0,  10.0 };
  double x[] =  { 0.0,   0.0 };
  gsl_matrix_view view_sigma;
  gsl_vector_view view_mu, view_x;
  gsl_tmvn_workspace_t tmvn_ws;

  printf("testing : test_dim2()\n");
  view_sigma = gsl_matrix_view_array(sigma, 2,2);
  view_mu = gsl_vector_view_array(mu, 2);
  view_x = gsl_vector_view_array(x, 2);

  gsl_tmvn_init(&(view_mu.vector), &(view_sigma.matrix), 0, xl, xu, r, 10000, &tmvn_ws);
  printf("constant for truncated = %lf\n", tmvn_ws.cdf);
  gsl_tmvn_done(&tmvn_ws);
}


void test_dim5()
{
  const int N = 5;
  int i,j;
  FILE * sigmatxt;
  double v;
  double mu[] = { 0.0, 0.0, 0.0, 0.0, 0.0 };
  gsl_vector_view view_mu;
  gsl_matrix * sigma = gsl_matrix_alloc(N,N);
  gsl_matrix * tmp = gsl_matrix_alloc(N,N);
  gsl_mvn_workspace_t mvn_ws;
  gsl_tmvn_workspace_t tmvn_ws;
  double xl[]= { 0, 0, 0, 0, 0};
  double xu[]= { 3, 3, 3, 3, 3 };
  double x[] = { 0, 0, 0, 0, 0};
  double x_out[] = { 0, 0, 0, 0, -1};
  double x_in[] = { 0.5, 0.5, 0.5, 0.5, 0.5};
  
  
  gsl_vector_view view_x;
  clock_t t0;
  clock_t timing;
  
  double density;
  
  view_mu = gsl_vector_view_array(mu, N);
  sigmatxt = fopen("sigma.txt", "r");
  assert(sigmatxt);
  for (i=0; i < N; i++) {
    for (j=0; j < N; j++) {
      fscanf(sigmatxt, "%lf", &v);
      gsl_matrix_set(sigma, i, j, v);
    }
  }
  fclose(sigmatxt);
  
  gsl_mvn_init(sigma, 0, &mvn_ws);

  printf("det(sigma)=%.7lf\n", mvn_ws.svd.det);
  printf("mvn_ws.K=%.7lf\n", mvn_ws.K);

#ifdef DUMP_MATRIX
  printf("--------- sigma ---------\n");
  gsl_matrix_table(stdout, sigma, ' ');

  printf("--------- U ---------\n");
  gsl_matrix_table(stdout, mvn_ws.svd.U, ' ');
  printf("--------- V ---------\n");
  gsl_matrix_table(stdout, mvn_ws.svd.V, ' ');
  gsl_matrix_memcpy(tmp, mvn_ws.svd.U);
  gsl_matrix_sub(tmp, mvn_ws.svd.V);
  printf("--------- U-V ---------\n");
  gsl_matrix_table(stdout, tmp, ' ');
#endif
  
  view_x = gsl_vector_view_array(x_in, N);
  t0 = clock();
  density = gsl_log_mvn_density(&(view_x.vector),
                                &(view_mu.vector), &mvn_ws);
  printf("log-density  = %.7lf, timing = %ld\n", density, clock()-t0);
  t0 = clock();
  gsl_mvn_done(&mvn_ws);
  gsl_tmvn_init(&(view_mu.vector), sigma, 1, xl, xu, r, 10000, &tmvn_ws);
  printf("constant for truncated = %lf\n", tmvn_ws.cdf);
  density = gsl_log_tmvn_density(&(view_x.vector), &(view_mu.vector), &tmvn_ws, xl, xu);
  timing = clock()-t0;
  printf("log-density (truncated, x) = %.7lf, timing = %ld (%lf)\n",
         density, timing, ((double)timing)/CLOCKS_PER_SEC);
  t0 = clock();
  view_x = gsl_vector_view_array(x_out, N);
  density = gsl_log_tmvn_density(&(view_x.vector), &(view_mu.vector), &tmvn_ws, xl, xu);
  timing = clock()-t0;
  printf("log-density (truncated, x_out) = %.7lf, timing = %ld (%lf)\n",
         density, timing, ((double)timing)/CLOCKS_PER_SEC);
  t0 = clock();
  view_x = gsl_vector_view_array(x_in, N);
  density = gsl_log_tmvn_density(&(view_x.vector), &(view_mu.vector), &tmvn_ws, xl, xu);
  timing = clock()-t0;
  printf("log-density (truncated, x_in) = %.7lf, timing = %ld (%lf)\n",
         density, timing, ((double)timing)/CLOCKS_PER_SEC);
  
      
  gsl_tmvn_done(&tmvn_ws);
  gsl_matrix_free(sigma);
  gsl_matrix_free(tmp);
}

int main()
{
  const gsl_rng_type *T;

  gsl_rng_env_setup ();

  T = gsl_rng_default;
  r = gsl_rng_alloc (T);

  //test_dim2();
  test_dim5();
  return 0;
}
