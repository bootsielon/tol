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
  Phi= Matpol(AR,m);
  Theta = Matpol(MA,m);
  cout << " Phi Matrix " << endl;
  for(i=0; i<m; ++i){
    for(j=0; j<m ; ++j){
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
  Phi = pMatpol(ARp,m);
  for(i=0; i<m; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(Phi,i,j) << " "  ;
    }
    cout << endl;
  }
  //

  cout << " Theta Matrix " << endl;
  for(i=0; i<m; ++i){
    for(j=0; j<m ; ++j){
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
  Theta = pMatpol(MAp,m);
  for(i=0; i<m; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(Theta,i,j) << " "  ;
    }
    cout << endl;
  }


  H1=H1_Matrix(AR);
  cout << " H1 " << endl;
  for(i=0; i<p; ++i){
    for(j=0; j<p ; ++j){
      cout << gsl_matrix_get(H1,i,j) << " "  ;
    }
    cout << endl;
  }

  S=Submatrix(H1,0,3,2,2);
  cout << "Submatrix upper_right 2x2 " << endl;
  for(i=0; i<2; ++i){
    for(j=0; j<2 ; ++j){
      cout << gsl_matrix_get(S,i,j) << " "  ;
    }
    cout << endl;
  }
  cout << " Again but with views " << endl;

  gsl_matrix_const_view v_S = gsl_matrix_const_submatrix(H1,0,2,2,3);
  gsl_matrix_memcpy(SS,&v_S.matrix);
  for(i=0; i<2; ++i){
    for(j=0; j<3 ; ++j){
      cout << gsl_matrix_get(SS,i,j) << " "  ;
    }
    cout << endl;
  }




  H2=H2_Matrix(MA);
  cout << " H2 " << endl;
//   cout << "Nrows " << H2->size1 << endl;
//   cout << "Ncols " << H2->size2 << endl;
//   cout << "MA order " << MA->size << " q " << q << endl;

  for(i=0; i<q; ++i){
    for(j=0; j<q ; ++j){
      cout << gsl_matrix_get(H2,i,j) << " "  ;
    }
    cout << endl;
  }
  determinante = MatSVDdet(H2);
  cout << "Determinant of H2 " << determinante <<  endl;

  H=H_Matrix(AR,MA,10);
  cout << "Big H Matrix " << endl;
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(H,i,j) << " "  ;
    }
    cout << endl;
  }

  cout <<"Alternative H Matrix " << endl;
  BH=BigHMatrix(ARp,MAp,10);
  for(i=0; i<10; ++i){
    for(j=0; j<m ; ++j){
      cout << gsl_matrix_get(BH,i,j) << " "  ;
    }
    cout << endl;
  }

  cout << "Creating Vector R Normal(0,1)" << endl;
  for(i=0; i<N; ++i){
    R->data[i*R->stride]=gsl_ran_ugaussian(r);
  }
  lV=LLKR(R,pAR,pMA);
  cout << "Log-Likelihood " << lV << endl;
  cout << "Sigma U of ARMA(0,2) "<< endl;
  MSigmaUMA2 = SigmaU(pAR,pMA2);
  for(i=0; i<2; ++i){
    for(j=0; j<2; ++j){
      cout << gsl_matrix_get(MSigmaUMA2,i,j) << " " ;
    }
    cout << endl;
  }
  cout << "Log-Likelihood with ARMA  " << endl;
  lV=LLKR(R,pAR,pMA2);
  cout << "Log-Likelihood " << lV << endl;

  G=pARCov(AR,1.0);
  cout << "G Matrix of AR polynomial " << endl;
  for(i=0; i<6; ++i){
    for(j=0; j<6 ; ++j){
      cout << gsl_matrix_get(G,i,j) << " "  ;
    }
    cout << endl;
  }
//   D=DiagVector(AR);
//   for(i=0; i<5; ++i){
//     for(j=0; j<5 ; ++j){
//       cout << gsl_matrix_get(D,i,j) << " "  ;
//     }
//     cout << endl;
//   }
//   D1=MatSVDInverse(D);
//   for(i=0; i<5; ++i){
//     for(j=0; j<5 ; ++j){
//       cout << gsl_matrix_get(D1,i,j) << " "  ;
//     }
//     cout << endl;
//   }

  cout << "Sigma U of MA(2) process " << endl;
  MSigmaUMA2 = SigmaU(pAR,pMA2);
  for(i=0; i<2; ++i){
    for(j=0; j<2; ++j){
      cout << gsl_matrix_get(MSigmaUMA2,i,j) << " " ;
    }
    cout << endl;
  }

  cout << "Sigma U of AR(5) process " << endl;
  gsl_vector_set(pAR5,0,1);
  for(i=1; i<6; ++i){
    gsl_vector_set(pAR5,i,AR->data[i-1]);
  }

  MSigmaUAR5 = SigmaU(pAR5,pMA);
  for(i=0; i<5; ++i){
    for(j=0; j<5; ++j){
      cout << gsl_matrix_get(MSigmaUAR5,i,j) << " " ;
    }
    cout << endl;
  }

  cout << "Sigma U of ARMA(5,2) process " << endl; 
  MSigmaUARMA52 = SigmaU(pAR5,pMA2);
  for(i=0; i<7; ++i){
    for(j=0; j<7; ++j){
      cout << gsl_matrix_get(MSigmaUARMA52,i,j) << " " ;
    }
    cout << endl;
  }

  cout << "SVD inverse of U" << endl;
  MSigmaUARMA52i =MatSVDInverse(MSigmaUARMA52);

  for(i=0; i<7; ++i){
    for(j=0; j<7; ++j){
      cout << gsl_matrix_get(MSigmaUARMA52i,i,j) << " " ;
    }
    cout << endl;
  }
  determinante = MatSVDdet(MSigmaUARMA52);
  cout << "Determinant of U Matrix " << determinante << endl;
  determinante = Matdet(MSigmaUARMA52);
  cout << "Determinant of U Matrix " << determinante << endl;


//   cout << "Sigma U of ARMA(5,2) process " << endl; 
//   for(i=0; i<7; ++i){
//     for(j=0; j<7; ++j){
//       cout << gsl_matrix_get(MSigmaUARMA52,i,j) << " " ;
//     }
//     cout << endl;
//   }

}
