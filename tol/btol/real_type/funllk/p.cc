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

using namespace std;

int main(){
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
  gsl_matrix *Phi = gsl_matrix_alloc(10,10);
  gsl_matrix *Theta = gsl_matrix_alloc(10,10); 
  gsl_matrix *PI = gsl_matrix_alloc(10,10);
  gsl_matrix *ITheta = gsl_matrix_alloc(10,10); 
  //
  gsl_matrix *H1 = gsl_matrix_alloc(p,p);
  gsl_matrix *H2 = gsl_matrix_alloc(q,q);
  gsl_matrix *H = gsl_matrix_alloc(10,m);
  gsl_matrix *BH = gsl_matrix_alloc(10,m);
  gsl_matrix *L = gsl_matrix_alloc(10,m);
  //
  gsl_matrix *S=gsl_matrix_alloc(2,2);
  gsl_matrix *SS=gsl_matrix_alloc(2,3);
  //
  gsl_vector *R = gsl_vector_alloc(N);
  gsl_vector *pAR = gsl_vector_alloc(1);
  gsl_vector *pMA = gsl_vector_alloc(1);
  gsl_vector *pMA2 = gsl_vector_alloc(3);
  gsl_vector *pAR5=gsl_vector_alloc(6);
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
  gsl_vector *ARp=gsl_vector_alloc(6);
  gsl_vector *MAp=gsl_vector_alloc(3);
  //
  Phi= Matpol(AR,10);
  Theta = Matpol(MA,10);
  cout << " Phi Matrix " << endl;
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(Phi,i,j) << " "  ;
    }
    cout << endl;
  }
  //
  cout << "Phi Matrix with pMatpol " << endl;
  gsl_vector_set(ARp,0,1);
  gsl_vector_view v_AR = gsl_vector_subvector(ARp,1,5);
  gsl_vector_memcpy(&v_AR.vector,AR);
  cout << "Ar polynomial " << endl;
  for(i=0; i<6; ++i){
    cout << gsl_vector_get(ARp,i) << " " ;
  }
  cout << endl;
  cout << " Phi Matrix " << endl;
  Phi = pMatpol(ARp,10);
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(Phi,i,j) << " "  ;
    }
    cout << endl;
  }
  //

  cout << " Theta Matrix " << endl;
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(Theta,i,j) << " "  ;
    }
    cout << endl;
  }
  cout << "Theta Matrix with pMatpol " << endl;
  gsl_vector_set(MAp,0,1);
  gsl_vector_view v_MA = gsl_vector_subvector(MAp,1,2);
  gsl_vector_memcpy(&v_MA.vector,MA);
  cout << "MA polynomial " << endl;
  for(i=0; i<3; ++i){
    cout << gsl_vector_get(MAp,i) << " " ;
  }
  cout << endl;
  cout << " Theta Matrix " << endl;
  Theta = pMatpol(MAp,10);
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(Theta,i,j) << " "  ;
    }
    cout << endl;
  }
  cout << "Inverse of Theta, Itheta it should be Lower Triangular " << endl;
  ITheta=MatSVDInverse(Theta);
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(ITheta,i,j) << " "  ;
    }
    cout << endl;
  }

  cout <<"H Matrix " << endl;
  BH=BigHMatrix(ARp,MAp,10);
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(BH,i,j) << " "  ;
    }
    cout << endl;
  }

  cout << "L Matrix, L=Itheta*H , triangular Itheta and diag=1" << endl;
  gsl_matrix_memcpy(L,BH);
  gsl_blas_dtrmm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1,ITheta,L);
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(L,i,j) << " "  ;
    }
    cout << endl;
  }

  cout << "L Matrix, L=Itheta*H , triangular Itheta and diag=1, inverting" << endl;
  gsl_matrix_memcpy(L,BH);
  gsl_blas_dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1,Theta,L);
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(L,i,j) << " "  ;
    }
    cout << endl;
  }

  cout << "L Matrix, L=Itheta*H , assume nothing" << endl;
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,ITheta,BH,0,L);
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(L,i,j) << " "  ;
    }
    cout << endl;
  }
  cout << "PI = Inv(Theta)*Phi " << endl;
  gsl_matrix_memcpy(PI,Phi);
  gsl_blas_dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1,Theta,PI);
  for(i=0; i<10; ++i){
    for(j=0; j<10 ; ++j){
      cout << gsl_matrix_get(PI,i,j) << " "  ;
    }
    cout << endl;
  }


}
