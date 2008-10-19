/*

gcc -g2 -Wall -I/usr/local/include  test_svd.c gsl_util_ext.c gsl_linalg_ext.c  -L/usr/local/gsl/lib -lgsl -lgslcblas

*/

#include <gsl/gsl_math.h>
#include <gsl/gsl_blas.h>
#include "gsl_ext.h"

void test_svd_inverse(gsl_matrix * A, gsl_svd_matrix_t *svd)
{
  gsl_matrix *inv = NULL;
  int i,j;
  double v;
  double epsilon;

  epsilon = 1.0E-10;
  gsl_svd_pinverse(svd, &inv);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,
                 1.0,A,inv,0.0,svd->work_mult);
  for (i = 0; i < A->size1; i++) {
    for (j = 0; j < A->size2; j++) {
      v = gsl_matrix_get(svd->work_mult,i,j);
      if (i==j) {
       if (gsl_fcmp(v,1.0,epsilon)) {
         printf("error at cell (%d,%d), should be 1.0 and it is %g\n", i, j, v);
       }
      } else if (fabs(v)>=epsilon) {
        printf("error at cell (%d,%d), should be 0.0 and it is %g\n", i, j, v);
      }
    }
  }
  
  gsl_matrix_free(inv);
}

int main()
{
#define N 5
  
  double A[] = 
    {
      3.7779406,  0.8044385, -3.518737,  0.7422526, -3.463709,
      0.8044385,  1.8806651, -3.127749,  1.0173059, -1.652422,
      -3.5187373, -3.1277487,  8.993765, -1.8112513,  6.750959,
      0.7422526,  1.0173059, -1.811251,  1.9202310, -1.249161,
      -3.4637086, -1.6524219,  6.750959, -1.2491613,  5.803521
    };
  double B[] = 
    {
      1.00170360556, 0.742779999457, 0.953483214672, 1.43127185484, 1.49911186571,
      0.742779999457, 2.05415754908, 1.18013525325, 1.80234925951, 1.80095207864,
      0.953483214672, 1.18013525325, 1.32233633193, 1.89820953352, 1.74395787662,
      1.43127185484, 1.80234925951, 1.89820953352, 3.11866424748, 2.95442737472,
      1.49911186571, 1.80095207864, 1.74395787662, 2.95442737472, 2.95335792961
    };
  
  gsl_matrix_view view = gsl_matrix_view_array(A,5,5);
  gsl_svd_matrix_t svd;
  
  gsl_svd_init(&(view.matrix),0,&svd);
  test_svd_inverse(&(view.matrix), &svd);
  printf("det(A)=%g\n", svd.det);
  
  view = gsl_matrix_view_array(B,5,5);
  
  gsl_svd_set(&(view.matrix),&svd);
  test_svd_inverse(&(view.matrix), &svd);
  printf("det(B)=%g\n", svd.det);
  
  gsl_svd_done(&svd);
  return 0;
}
