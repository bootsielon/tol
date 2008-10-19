// Program to check some functions on matrices
// Miguel A. arranz
// Feb 15, 2005

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_fit.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_permutation.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include "llkarma.h"
#include <iostream>
#include <fstream>

using namespace std;

gsl_vector * readdat(ifstream & in, size_t n)
{
  double dat;

  if (in.eof()) {
    return gsl_vector_alloc(n);
  }
  in >> dat;
  in.ignore(2, '\n');
  gsl_vector * v = readdat(in,n+1);
  gsl_vector_set(v,n,dat);
  return v;
}

int main(int argc, char * argv[]){
  //Random generation settings
  const gsl_rng_type *T;
  gsl_rng *r;
  //
  int i, j;
  int N=350;
  double lV, determinante;
  gsl_vector *AR = gsl_vector_alloc(5);
  gsl_vector *MA = gsl_vector_alloc(2);
  gsl_matrix *D = gsl_matrix_alloc(5,5);
  gsl_matrix *D1 = gsl_matrix_alloc(5,5);
  //
  int p = AR->size;
  int q = MA->size;
  int m = p+q;
  //
  gsl_matrix *G = gsl_matrix_alloc(6,6);
  //
  gsl_matrix *Phi = gsl_matrix_alloc(m,m);
  gsl_matrix *Theta = gsl_matrix_alloc(m,m); 
  gsl_matrix *H1 = gsl_matrix_alloc(p,p);
  gsl_matrix *H2 = gsl_matrix_alloc(q,q);
  gsl_matrix *H = gsl_matrix_alloc(10,m);
  gsl_matrix *BH = gsl_matrix_alloc(10,m);
  //
  gsl_matrix *S=gsl_matrix_alloc(2,2);
  gsl_matrix *SS=gsl_matrix_alloc(2,3);
  //
  gsl_vector *pAR = gsl_vector_alloc(1);
  gsl_vector *pMA = gsl_vector_alloc(1);
  gsl_vector *pMA2 = gsl_vector_alloc(3);
  gsl_vector *pAR5=gsl_vector_alloc(6);
  gsl_vector *pMA53 = gsl_vector_calloc(54);
  //
  gsl_matrix *MSigmaUMA2 = gsl_matrix_alloc(2,2);
  gsl_matrix *MSigmaUAR5 = gsl_matrix_alloc(5,5);
  gsl_matrix *MSigmaUARMA52 = gsl_matrix_alloc(7,7);
  gsl_matrix *MSigmaUARMA52i = gsl_matrix_alloc(7,7);
  //
  gsl_rng_env_setup();
  T=gsl_rng_default;
  r=gsl_rng_alloc(T);
  //
  gsl_vector_set(pAR,0,1);
  gsl_vector_set(pMA,0,1);
  gsl_vector_set(pMA2,0,1);
  gsl_vector_set(pMA2,1,0.2);
  gsl_vector_set(pMA2,2,0.5);
  //
  gsl_vector_set(AR,0,0.5);
  gsl_vector_set(AR,1,0.4);
  gsl_vector_set(AR,2,0.3);
  gsl_vector_set(AR,3,0.2);
  gsl_vector_set(AR,4,0.1);
  //
  gsl_vector_set(MA,0,0.9);
  gsl_vector_set(MA,1,0.8);
  //
  gsl_vector_set(pMA53,0,1);
  gsl_vector_set(pMA53,1,0.5);
  gsl_vector_set(pMA53,52,0.5);
  gsl_vector_set(pMA53,53,-0.25);

  gsl_vector *ARp=gsl_vector_alloc(6);
  gsl_vector *MAp=gsl_vector_alloc(3);
  //

  gsl_vector *R;
  
  if (argc > 1) {
    ifstream in(argv[1]);
    R = readdat(in,0);
    cout << "Leido " << R->size << " datos\n";
  } else {
    R = gsl_vector_alloc(N);
    cout << "Creating Vector R Normal(0,1)" << endl;
    for(i=0; i<N; ++i){
      R->data[i*R->stride]=gsl_ran_ugaussian(r);
    }
  }
  lV=LLKR(R,pAR,pMA);
  cout << "Log-Likelihood " << lV << endl;
  cout << "Log-Likelihood with ARMA  " << endl;
  lV=LLKR(R,pAR,pMA2);
  cout << "Log-Likelihood " << lV << endl;
}
