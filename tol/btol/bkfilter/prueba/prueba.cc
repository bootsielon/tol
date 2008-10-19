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
#include <gsl/gsl_math.h>
//#include "rtnorm.h"
#include "LLKKFSS.h"
// #include "linalgebra.h"
#include "gsl_ext.h"
#include <iostream>
#include <time.h>
#include <fstream>

using namespace std;

int main(){
  int i, j, m=4, k=4, T=156, n=15;
  double a;
  clock_t t0,t1;
  ifstream phif("Fhi.bmt");
//   ifstream eif("E.bmt");
  ifstream hif("H.bmt");
//   ifstream dif("D.bmt");
  ifstream zif("Z.bmt");
  ifstream uif("U.bmt");
//   ifstream p0if("P0.bmt");
  ifstream gamif("Gamma.dat");
  gsl_matrix *Phi=gsl_matrix_alloc(m,m);
  gsl_matrix *Gamma=gsl_matrix_alloc(m,n);
  gsl_matrix *E=gsl_matrix_alloc(m,k);
  gsl_matrix *H=gsl_matrix_alloc(k,m);
  gsl_matrix *D=gsl_matrix_calloc(k,n);
  gsl_matrix *C=gsl_matrix_alloc(k,k);
  gsl_matrix *R=gsl_matrix_alloc(k,k);
  gsl_matrix *Q=gsl_matrix_alloc(k,k);
  gsl_matrix *S=gsl_matrix_calloc(k,k);
  gsl_matrix *P0=gsl_matrix_alloc(m,m);
  gsl_matrix *Zt=gsl_matrix_alloc(T,k);
  gsl_matrix *Z=gsl_matrix_alloc(k,T);
  gsl_matrix *Ut=gsl_matrix_alloc(T,n);
  gsl_matrix *U=gsl_matrix_alloc(n,T);
  gsl_vector *x0=gsl_vector_calloc(m);
  gsl_vector *U0 = gsl_vector_calloc(n);

  gsl_matrix *EQ=gsl_matrix_alloc(m,k);
  gsl_matrix *EQE=gsl_matrix_alloc(m,m);

  for(i=0; i<m; ++i){
    for(j=0; j<m; ++j){
      phif >> a;
      gsl_matrix_set(Phi,i,j,a);      
    }
  }


  for(i=0; i<m; ++i){
    for(j=0; j<n; ++j){
      gamif >> a;
      gsl_matrix_set(Gamma,i,j,a);      
    }
  }

  for(i=0; i<k; ++i){
    for(j=0; j<m; ++j){
      hif >> a;
      gsl_matrix_set(H,i,j,a);      
    }
  }

  gsl_matrix_set_identity(P0);
  gsl_matrix_scale(P0,1.9607843137);
  gsl_matrix_set_identity(E);
  gsl_matrix_set_identity(C);
  gsl_matrix_set_identity(R);
  gsl_matrix_scale(R,4);
  gsl_matrix_set_identity(Q);
  //  gsl_matrix_set_identity(S);
  for(i=0; i<T; ++i){
    for(j=0; j<n; ++j){
      uif >> a;
      gsl_matrix_set(Ut,i,j,a);      
    }
  }
  gsl_matrix_transpose_memcpy(U,Ut);
//   cout << "Dimensions of Ut " << U->size1 << " " << U->size2 << endl;
  for(i=0; i<T; ++i){
    for(j=0; j<k; ++j){
      zif >> a;
      gsl_matrix_set(Zt,i,j,a);      
    }
  }
  gsl_matrix_transpose_memcpy(Z,Zt);

//     for(int i=0; i<m; ++i){
//       for(int j=0; j<n; ++j){
//         cout << gsl_matrix_get(Gamma,i,j) << " " ;      
//       }
//       cout << endl;
//     }

  std::cout << " Primer paso " << std::endl;

  t0 = clock();
//   a = LLKKFSSi(H,D,C, Phi, Gamma, E, Q, R, S,Z, U,x0, P0);
//   cout << "LLK " << a << endl;	       
  a = LLKKFSS(H,D,C, Phi, Gamma, E, Q, R, S,Z, U,x0, P0, 1);
//   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,E ,Q,0,EQ);
//   gsl_blas_dgemm(CblasNoTrans,CblasTrans  ,1,EQ,E,0,EQE);
//  gsl_msmt(E,Q,EQE);
//  a = LLKKFSSp(D, E1,E3,E4,F1, 200, F3,F4,Q,Z,x0,P0);
  t1 = clock();
  cout << "LLK " << a << endl;	       
  cout << "elapsed time  "<< t1-t0 << endl;
	       

}





