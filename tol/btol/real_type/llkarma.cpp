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
//#include "funciones.h"
#include "llkarma.h"
#include <iostream>
//using namespace std;

void gsl_matrix_table(FILE * f, gsl_matrix * m)
{
  size_t i, j;

  for (i = 0; i < m->size1; i++) {
    for (j = 0; j < m->size2; j++)
      fprintf(f, " %6.2g", gsl_matrix_get(m, i, j));
    fprintf(f, "\n");
  }
}

// void pMatpol(const gsl_vector *v, int n, gsl_matrix *M){
//   //Equivalent to PolMat in tol
//   // The poynomial is a vector and the first
//   // term (1) is included 
//   // The polynomial is Phi_0-Phi_1B-Phi_2B^2-...-Phi_pB^p
//   // or 1-Theta_1B-Theta_2B^2-...-Theta_qB^q
//   int i, j;
//   gsl_matrix_set_identity(M);
//   int K = v->size;
//  //  for(i=0; i<K; ++i){
//  //    std::cout << gsl_vector_get(v,i) << std::endl;
//  //  }
//   if(K>1){
//     for(j=0; j<K-1; ++j){
//       for(i=j+1; i<n; ++i){
// 	gsl_matrix_set(M,i,i-j-1,-v->data[j+1]);
//       }
//     }
//   }

// }


 void pMatpol(const gsl_vector *v, int n, gsl_matrix *M){
   //Equivalent to PolMat in tol
   // The poynomial is a vector and the first
   // term (1) is included 
   // The polynomial is Phi_0-Phi_1B-Phi_2B^2-...-Phi_pB^p
   // or 1-Theta_1B-Theta_2B^2-...-Theta_qB^q
   int i;
    gsl_matrix_set_identity(M);
   int K = v->size;
 // Version views
     double s;
     gsl_vector_view w;
     if(K>1){
       for(i=1; i<K; ++i){
 	w=gsl_matrix_subdiagonal(M,i);
 	s=gsl_vector_get(v,i);
 	gsl_vector_set_all(&w.vector,-s);
       }
     }

 }

void Matpol(const gsl_vector *v, int n, gsl_matrix *M){
  //Equivalent to PolMat in tol
  // The poynomial is a vector and the first
  // term (1) is omitted 
  // The polynomial is -Phi_1B-Phi_2B^2-...-Phi_pB^p
  // or -Theta_1B-Theta_2B^2-...-Theta_qB^q
  int i, j;
  gsl_matrix_set_identity(M);
  int K = v->size;
  for(j=0; j<K; ++j){
    for(i=j+1; i<n; ++i){
      gsl_matrix_set(M,i,i-j-1,-v->data[j]);
    }
  }
}

void DiagVector(const gsl_vector *v, gsl_matrix *D){
  //  int n=v->size;
  // Versión 'a capella'
  //   for(i=0; i<n; ++i){
  //     gsl_matrix_set(D,i,i,v->data[i]);
  //   }
  //Version 'views'
  gsl_vector_view v_x = gsl_matrix_diagonal(D);
  gsl_vector_memcpy(&v_x.vector,v);
}

//  void H1_Matrix(const gsl_vector *v, gsl_matrix *H1){
// // Reverse before calling
//    int r, c;
//    int p=v->size;
//    for(r=0; r<p; ++r){
//      for(c=p-1; c>=r; --c){
//        gsl_matrix_set(H1,r,c,v->data[c-r]); 
//      }
//    }
//  }

 void H1_Matrix(const gsl_vector *v, gsl_matrix *H1){
   // This is with no need to reverse
   int r;
   int p=v->size;
   double c;
   gsl_vector_view z;
   for(r=0; r<p; ++r){
     z=gsl_matrix_superdiagonal(H1,r);
     c=gsl_vector_get(v,p-r-1);
     gsl_vector_set_all(&z.vector,c);
   }
 }

// void H2_Matrix(const gsl_vector *v, gsl_matrix *H2){
// Reverse before calling
//   int r, c;
//   int q=v->size;
//   for(r=0; r<q; ++r){
//     for(c=q-1; c>=r; --c){
//       gsl_matrix_set(H2,r,c,-v->data[c-r]); 
//     }
//   }
// }


void H2_Matrix(const gsl_vector *v, gsl_matrix *H2){
  // No reverse
   int r;
   int p=v->size;
   double c;
   gsl_vector_view z;
   for(r=0; r<p; ++r){
     z=gsl_matrix_superdiagonal(H2,r);
     c=-gsl_vector_get(v,p-r-1);
     gsl_vector_set_all(&z.vector,c);
   }

}


void BigHMatrix(const gsl_vector *pAR, const gsl_vector *pMA, int n, 
		gsl_matrix *H){
  // Constructs the H matrix from the vectors
  // of coefficients (including the 0 order coefficients)
  // Notice that the case p=q=0 is non-valid, since matrix H is not 
  // necessary
  int nAR=pAR->size;
  int nMA=pMA->size;
  int p=nAR-1;
  int q=nMA-1;
  //  int m=p+q;
  //This is the result matrix
  //  gsl_matrix *H=gsl_matrix_calloc(n,m);
  if((p!=0)&&(q==0)){
    gsl_matrix *H1=gsl_matrix_alloc(p,p);
    gsl_vector *AR=gsl_vector_alloc(p);
    gsl_vector_const_view vAR = gsl_vector_const_subvector(pAR,1,p);
    gsl_vector_memcpy(AR,&vAR.vector);
    // Jaime reverse
    //    gsl_vector_reverse(AR);
    H1_Matrix(AR,H1);
    gsl_matrix_view vHA = gsl_matrix_submatrix(H,0,0,p,p);
    gsl_matrix_memcpy(&vHA.matrix,H1);
    gsl_vector_free(AR);
    gsl_matrix_free(H1);
  }
  else if((p==0)&&(q!=0)){
    gsl_matrix *H2=gsl_matrix_alloc(q,q);
    gsl_vector *MA=gsl_vector_alloc(q);
    gsl_vector_const_view vMA = gsl_vector_const_subvector(pMA,1,q);
    gsl_vector_memcpy(MA,&vMA.vector);
    //    gsl_vector_reverse(MA);
    H2_Matrix(MA,H2);
    gsl_matrix_view vHB = gsl_matrix_submatrix(H,0,0,q,q);
    gsl_matrix_memcpy(&vHB.matrix,H2);
    gsl_vector_free(MA);
    gsl_matrix_free(H2);
  }
  else{
    gsl_matrix *H1=gsl_matrix_alloc(p,p);
    gsl_vector *AR=gsl_vector_alloc(p);
    gsl_vector_const_view vAR = gsl_vector_const_subvector(pAR,1,p);
    gsl_vector_memcpy(AR,&vAR.vector);
    gsl_matrix *H2=gsl_matrix_alloc(q,q);
    gsl_vector *MA=gsl_vector_alloc(q);
    gsl_vector_const_view vMA = gsl_vector_const_subvector(pMA,1,q);
    gsl_vector_memcpy(MA,&vMA.vector);
//     gsl_vector_reverse(AR);
//     gsl_vector_reverse(MA);
    H1_Matrix(AR,H1);
    H2_Matrix(MA,H2);
    gsl_matrix_view vHC = gsl_matrix_submatrix(H,0,0,p,p);
    gsl_matrix_memcpy(&vHC.matrix,H1);
    gsl_matrix_view vHD = gsl_matrix_submatrix(H,0,p,q,q);
    gsl_matrix_memcpy(&vHD.matrix,H2);
    gsl_vector_free(AR);
    gsl_vector_free(MA);
    gsl_matrix_free(H1);
    gsl_matrix_free(H2);
  }
}

void H_Matrix(const gsl_vector *AR, const gsl_vector *MA, int n, gsl_matrix *H){
  // Constructs the H matrix from the vectors
  // of coefficients (excluding the 0 order coefficients)
  int r,c;
  int p=AR->size;
  int q=MA->size;
  //  int m=p+q;
  gsl_matrix *H1=gsl_matrix_alloc(p,p);
  gsl_matrix *H2=gsl_matrix_alloc(q,q);
  H1_Matrix(AR,H1);
  H2_Matrix(MA,H2);
  for(r=0; r<p; ++r){
    for(c=0; c<p; ++c){
      gsl_matrix_set(H,r,c,H1->data[r*H1->tda+c]);
    }
  }
  for(r=0; r<q; ++r){
    for(c=0; c<q; ++c){
      gsl_matrix_set(H,r,p+c,H2->data[r*H2->tda+c]);
    }
  }
  gsl_matrix_free(H1);
  gsl_matrix_free(H2);

}

double Matdet(const gsl_matrix *M){
  int signum, N;
  double det;
  N=M->size1;
  gsl_matrix *A = gsl_matrix_alloc(N,N);
  gsl_permutation *p = gsl_permutation_alloc(N);

  gsl_matrix_memcpy(A,M);
  gsl_linalg_LU_decomp(A,p,&signum);
  det=gsl_linalg_LU_det(A,signum);
  gsl_matrix_free(A);
  gsl_permutation_free(p);
  return det;

}

double MatSVDdet(const gsl_matrix *M){
  int i;
  double det;
  int N = M->size1;
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
  //  det*=pow((-1),(double)N);
  gsl_matrix_free(U);
  gsl_matrix_free(V);
  gsl_vector_free(S);
  gsl_vector_free(work);
  return det;
}

void MatInverse(const gsl_matrix *M, gsl_matrix *Mi){
  int signum, N;
  N=M->size1;
  gsl_permutation *p = gsl_permutation_alloc(N);
  gsl_matrix *A = gsl_matrix_alloc(N,N);
  gsl_matrix_memcpy(A,M);
  gsl_linalg_LU_decomp(A,p,&signum);
  gsl_linalg_LU_invert(A,p,Mi);
  gsl_permutation_free(p);
  gsl_matrix_free(A);

}

void MatSVDInverse(const gsl_matrix *M, gsl_matrix *Mi){
  int i;
  int N = M->size1;
  gsl_matrix *U = gsl_matrix_alloc(N,N);
  gsl_matrix *Ut = gsl_matrix_calloc(N,N);
  gsl_matrix *V = gsl_matrix_alloc(N,N);
  gsl_matrix *D = gsl_matrix_calloc(N,N);
  //gsl_vector *S = gsl_vector_alloc(N);
  gsl_vector_view S;
  gsl_vector *work = gsl_vector_alloc(N);

  S = gsl_matrix_diagonal(D);
  gsl_matrix_memcpy(U,M);
  gsl_linalg_SV_decomp(U,V,&S.vector,work);

  for(i=0; i<N; ++i){
    gsl_vector_set(&S.vector, i, 1/gsl_vector_get(&S.vector,i));
    //S->data[i] = 1/(S->data[i]);
  }
  
  //DiagVector(S,D);
  //
  //gsl_vector_free(S);
  gsl_vector_free(work);
  //
  gsl_matrix_set_zero(Mi);
  /*
    cout << "D :\n";
    gsl_matrix_table(stdout, D);
    cout << "U :\n";
    gsl_matrix_table(stdout, U);
    cout << "Ut :\n";
    gsl_matrix_table(stdout, Ut);
  */
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,  1.0,D,U, 0.0,Ut);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,V,Ut,0.0,Mi);
  //
  gsl_matrix_free(U);
  gsl_matrix_free(Ut);
  gsl_matrix_free(V);
  gsl_matrix_free(D);
}



void MatSVDdetInverse(const gsl_matrix *M, gsl_matrix *Mi, double &det){
  int i;
  int N = M->size1;
  gsl_matrix *U = gsl_matrix_alloc(N,N);
  gsl_matrix *Ut = gsl_matrix_calloc(N,N);
  gsl_matrix *V = gsl_matrix_alloc(N,N);
  gsl_matrix *D = gsl_matrix_calloc(N,N);
  //gsl_vector *S = gsl_vector_alloc(N);
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
    //S->data[i] = 1/(S->data[i]);
  }
  
  //DiagVector(S,D);
  //
  gsl_vector_free(s);
  gsl_vector_free(work);
  //
  gsl_matrix_set_zero(Mi);
  /*
    cout << "D :\n";
    gsl_matrix_table(stdout, D);
    cout << "U :\n";
    gsl_matrix_table(stdout, U);
    cout << "Ut :\n";
    gsl_matrix_table(stdout, Ut);
  */
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,  1.0,D,U, 0.0,Ut);
  gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1.0,V,Ut,0.0,Mi);
  //
  gsl_matrix_free(U);
  gsl_matrix_free(Ut);
  gsl_matrix_free(V);
  gsl_matrix_free(D);
}

void Submatrix(const gsl_matrix *m, int r, int c, int nr, int nc, 
	       gsl_matrix *S){
  //  int i, j;
  // Esta es la versión 'a capella' y funciona bien
  //   for(i=0; i<nr; ++i){
  //     for(j=0; j<nc; ++j){
  //       gsl_matrix_set(A,i,j,m->data[(r+i)*m->tda +(c+j)]);
  //     }
  //   }
  // view
  gsl_matrix_const_view X = gsl_matrix_const_submatrix(m,r,c,nr,nc);
  gsl_matrix_memcpy(S,&X.matrix);

}

void IthetaCol1(const gsl_vector *pMA, gsl_vector* X, int N){
  int i, q = pMA->size-1;
  double c;
  gsl_vector *MA = gsl_vector_alloc(q);
  gsl_vector_const_view v_MA = gsl_vector_const_subvector(pMA,1,q);
  gsl_vector_view v, z;
  gsl_vector_memcpy(MA,&v_MA.vector);
  gsl_vector_set(X,0,1);
  if(q==1){
    c=gsl_vector_get(pMA,1);
    for(i=1; i<N; ++i){
      gsl_vector_set(X,i,pow(c,i));
    }
  }


   
  // This is to get Theta^{-1} (only the first column)
  else{
//     gsl_vector_scale(MA,-1);

    gsl_vector_reverse(MA);
   

    for(i=1; i<=q-1; ++i){
      v = gsl_vector_subvector(MA,q-i,i);
      z = gsl_vector_subvector(X,0,i);
      gsl_blas_ddot(&v.vector,&z.vector,&c);
      gsl_vector_set(X,i,c);
    }
  
  
    for(i=q; i<N; ++i){
      z = gsl_vector_subvector(X,i-q,q);
      gsl_blas_ddot(MA,&z.vector,&c);
      gsl_vector_set(X,i,c);
    }
  }
  gsl_vector_free(MA);

}

void IthetaHMatrix(const gsl_vector *Itheta1, const gsl_vector *v, 
		   gsl_matrix *M){
  // It forms the matrix InveTheta * H with vector v giving the
  // AR or MA polynomial (check signs first)
  int i, N = Itheta1->size, p=v->size;
  double c;
  gsl_vector *w = gsl_vector_alloc(p);
  gsl_vector *row = gsl_vector_alloc(p);
  gsl_vector *v1 = gsl_vector_alloc(p);
  // gsl_vector *v2 = gsl_vector_alloc(p-1);
  //  gsl_vector_const_view v_MA = gsl_vector_const_subvector(v,1,p);
  gsl_vector_view  z1, z2;
  //gsl_vector_memcpy(w,&v_MA.vector);
  if(p==1){
    c=gsl_vector_get(v,0);
    gsl_vector *V = gsl_vector_alloc(N);
    gsl_vector_memcpy(V,Itheta1);
    gsl_vector_scale(V,c);
    gsl_matrix_set_col(M,0,V);
    gsl_vector_free(V);
  }
  else{
  gsl_vector_memcpy(w,v);
  gsl_vector_reverse(w),
  c = gsl_vector_get(Itheta1,0);
  gsl_vector_memcpy(row,w);
  gsl_vector_scale(row,c);
  gsl_matrix_set_row(M,0,row);
  gsl_vector_set(v1,0,0);
  for(i=1; i<N; ++i){
    z1 = gsl_vector_subvector(row,0,(p-1));
    z2 = gsl_vector_subvector(v1,1,(p-1));
    gsl_vector_memcpy(&z2.vector,&z1.vector);
    c = gsl_vector_get(Itheta1,i);
    gsl_vector_memcpy(row,w);
    gsl_vector_scale(row,c);
    gsl_vector_add(row,v1);
    gsl_matrix_set_row(M,i,row);
   
  }

  }

  gsl_vector_free(w);
  gsl_vector_free(row);
  gsl_vector_free(v1);
  //  gsl_vector_free(v2);
}

void PIRMatrix2(gsl_vector *v, const gsl_vector *pAR, 
		const gsl_vector *pMA, const gsl_vector *R, 
		gsl_vector *PIR){
		
  int i, N = v->size;
  int p = pAR->size-1, q = pMA->size-1;
  double c;
  if((q==0)&&(p!=0)){
    PhiRMatrix(pAR,R,PIR);      
    }
  else if((q!=0) && (p==0)){
    gsl_vector_view x;
    gsl_vector_reverse(v);
    for(i=0; i<N; ++i){
      x=gsl_vector_subvector(v,N-i-1,i+1);
      gsl_vector_const_view vz=gsl_vector_const_subvector(R,0,i+1);
      gsl_blas_ddot(&x.vector,&vz.vector,&c);
      gsl_vector_set(PIR,i,c);
    }

    }
  else if((q!=0) && (p!=0)){
    gsl_vector *PhiR = gsl_vector_alloc(N);
    gsl_vector *RR = gsl_vector_alloc(N);
    gsl_vector_memcpy(RR,R);
    PhiRMatrix(pAR,RR,PhiR);
    //
    gsl_vector_view x;
    gsl_vector_reverse(v);
    for(i=0; i<N; ++i){
      x=gsl_vector_subvector(v,N-i-1,i+1);
      gsl_vector_const_view vz=gsl_vector_const_subvector(PhiR,0,i+1);
      gsl_blas_ddot(&x.vector,&vz.vector,&c);
      gsl_vector_set(PIR,i,c);
    }



    //


    gsl_vector_free(RR);
    gsl_vector_free(PhiR);
  }


}

void PhiRMatrix(const gsl_vector *pAR, const gsl_vector *R, gsl_vector *PhiR){
  int N = R->size;
  int p=pAR->size-1;
  int i;
  double c, a;

  gsl_vector *AR = gsl_vector_alloc(p);
  gsl_vector *nAR = gsl_vector_alloc(p+1);
  gsl_vector_view v;
  //  gsl_vector_const_view r;

  gsl_vector_const_view v_AR = gsl_vector_const_subvector(pAR,1,p);
  gsl_vector_memcpy(AR,&v_AR.vector);
  gsl_vector_scale(AR,-1);
  gsl_vector_set(nAR,0,1);
  v = gsl_vector_subvector(nAR,1,p);
  gsl_vector_memcpy(&v.vector,AR);
  gsl_vector_reverse(nAR);
  //
  a=gsl_vector_get(R,0);
  gsl_vector_set(PhiR,0,a);
  for(i=1; i<=p ; ++i){
    v=gsl_vector_subvector(nAR,p-i,i+1);
    gsl_vector_const_view r=gsl_vector_const_subvector(R,0,i+1);
    gsl_blas_ddot(&v.vector,&r.vector,&c);
    gsl_vector_set(PhiR,i,c);
  }
  for(i=p+1; i<N; ++i){
    gsl_vector_const_view r=gsl_vector_const_subvector(R,i-p,p+1);
    gsl_blas_ddot(nAR,&r.vector,&c);
    gsl_vector_set(PhiR,i,c);

  }



  gsl_vector_free(AR);
  gsl_vector_free(nAR);



}


double LLKR(const gsl_vector *R, const gsl_vector *pAR, const gsl_vector *pMA, double &sigma2, const int k){
  // Returns loglikelihood of Noise R where R follows an ARMA model. The
  // full polynomials of the ARMA model are given by pAR and pMA
  // Mean is zero and variance is estimated from sample if 
  // sigma2=0 or taken as parameter if sigma2>0
  int p=pAR->size -1;
  int q=pMA->size -1;
  int m=p+q;
  int N=R->size;
  double logV;


  if(m==0){
    //     sigma2=gsl_stats_variance(R->data,1,N);
    if(sigma2==0){
      gsl_blas_ddot(R, R, &sigma2);
      sigma2/=(N-k-1);
    }
    gsl_blas_ddot(R, R, &logV);
    logV/=(-2.0*sigma2);
    logV-=0.5*N*log(2.0*M_PI*sigma2);
  }
  else{
    gsl_matrix *SU=gsl_matrix_alloc(m,m);
    gsl_matrix *ISU=gsl_matrix_alloc(m,m);
    gsl_matrix *K=gsl_matrix_alloc(m,m);
    gsl_matrix *IK=gsl_matrix_alloc(m,m);
    gsl_matrix *L=gsl_matrix_calloc(N,m);
    gsl_matrix *Lt=gsl_matrix_alloc(m,N);
    gsl_matrix *BH=gsl_matrix_alloc(N,m);
    gsl_matrix *Phi=gsl_matrix_calloc(N,N);
    gsl_matrix *PI=gsl_matrix_alloc(N,N);
    gsl_matrix *Theta=gsl_matrix_calloc(N,N);
    gsl_matrix *ITheta=gsl_matrix_alloc(N,N);
    gsl_matrix *Eye=gsl_matrix_alloc(N,N);
    gsl_matrix *IKLT = gsl_matrix_alloc(m,N);
    gsl_vector *VLU=gsl_vector_alloc(N);
    gsl_vector *VSU=gsl_vector_alloc(m);
    gsl_vector *PIR=gsl_vector_alloc(N);
    gsl_vector *U=gsl_vector_alloc(m);
    gsl_vector *A = gsl_vector_alloc(N);
    double ldsu, ldk, aa, s2;
    SigmaU(pAR,pMA,SU);
    //           ldsu=MatSVDdet(SU);
    //           ldsu=log(ldsu);
    // //     //SU is probably a sparse matrix, but we have used SVD to invert it
    //           MatSVDInverse(SU,ISU);
    //Determinant and Inverse of SU in one step
    MatSVDdetInverse(SU,ISU,ldsu);
    ldsu=log(ldsu);
    // OLD CODE
    
    //       pMatpol(pAR,N,Phi);
    //       pMatpol(pMA,N,Theta);
    //       gsl_matrix_set_zero(BH);
    //       BigHMatrix(pAR,pMA,N,BH);
    //       //Theta is a lower triangular matrix with the diagonal equal to 1
    //       //We construct L = inv(Theta)*BH with dtrsm
    //       //Idem PI=inv(Theta)*Phi;    
    //       gsl_matrix_memcpy(L,BH);
    //       gsl_blas_dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1.0,Theta,L);
    
    // NEU code starts here
    if(N<20){
      pMatpol(pAR,N,Phi);
      pMatpol(pMA,N,Theta);
      gsl_matrix_set_zero(BH);
      BigHMatrix(pAR,pMA,N,BH);
      //Theta is a lower triangular matrix with the diagonal equal to 1
      //We construct L = inv(Theta)*BH with dtrsm
      //Idem PI=inv(Theta)*Phi;   
      gsl_matrix_memcpy(L,BH);
      gsl_blas_dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1.0,Theta,L);
      gsl_matrix_memcpy(PI,Phi);
      gsl_blas_dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasUnit,1.0,Theta,PI);
      //Is PI triangular? then gsl_blas_dtrmv. I think it is
      gsl_vector_memcpy(PIR,R);
      gsl_blas_dtrmv(CblasLower,CblasNoTrans,CblasUnit,PI,PIR);

    }
    // Check new function PIRMatrix with N>200
    else{ // Big N
      gsl_vector *Itheta1v = gsl_vector_alloc(N);
      if(q>0){ //MA
 	gsl_vector *MA = gsl_vector_alloc(q);
 	gsl_vector_const_view vMA = gsl_vector_const_subvector(pMA,1,q);
 	gsl_vector_memcpy(MA,&vMA.vector);
	gsl_vector_scale(MA,-1);
 	IthetaCol1(pMA, Itheta1v, N);
 	// Calculate L matrix by multiplying the H submatrices
 	// we should make cases p==0, p>0
 	if(p==0){ //MA(q)
 	  IthetaHMatrix(Itheta1v, MA, L);
 	}
 	else{ //ARMA(p,q)
 	  gsl_vector *AR = gsl_vector_alloc(p);
 	  gsl_vector_const_view vAR = gsl_vector_const_subvector(pAR,1,p);
 	  gsl_vector_memcpy(AR,&vAR.vector);
 	  gsl_matrix_view L1 = gsl_matrix_submatrix(L,0,0,N,p);
 	  gsl_matrix_view L2 = gsl_matrix_submatrix(L,0,p,N,q);
 	  IthetaHMatrix(Itheta1v, MA, &L2.matrix);
 	  IthetaHMatrix(Itheta1v, AR, &L1.matrix);	  
 	  gsl_vector_free(AR);
 	}
 	gsl_vector_free(MA);
      }
      else{ // AR(p)
 	gsl_vector *AR = gsl_vector_alloc(p);
 	gsl_vector_const_view vAR = gsl_vector_const_subvector(pAR,1,p);
 	gsl_matrix_view L1 = gsl_matrix_submatrix(L,0,0,p,p);
 	H1_Matrix(AR,&L1.matrix);	
 	gsl_vector_free(AR);
      }
      // Originally it was
      // PIRMatrix(pAR,pMA,R,PIR);
      // now we know Itheta1v
      PIRMatrix2(Itheta1v, pAR, pMA, R,PIR); 
      gsl_vector_free(Itheta1v);
    }
    // NEU code ends here
    //K=inv(SU)+L'L
    gsl_matrix_memcpy(K,ISU);
    // Check that L is Lower Triangular TODO (dtrmm)
    gsl_blas_dgemm(CblasTrans,CblasNoTrans,1,L,L,1,K);
    //     ldk=MatSVDdet(K);
    //     ldk=log(ldk);
    //     MatSVDInverse(K,IK);
    MatSVDdetInverse(K,IK,ldk);
    ldk=log(ldk);
    //Vector U=matrix(inv(K)*t(L))*vector(PI*R)
    gsl_matrix_transpose_memcpy(Lt,L);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,IK,Lt,0,IKLT);
    // No assumptions about IKLT
    gsl_blas_dgemv(CblasNoTrans,1,IKLT,PIR,0,U);
    // A=PI*R-L*U
    gsl_blas_dgemv(CblasNoTrans,1,L,U,0,VLU);
    gsl_vector_memcpy(A,PIR);
    gsl_vector_sub(A,VLU);
    // Matrix S2 = t(A)*A+t(U)*ISU*U, in fact it is double (1x1) 
    gsl_blas_ddot(A,A,&aa);
    // ISU is symmetric for sure -> dsymv
    gsl_blas_dsymv(CblasUpper,1,ISU,U,0,VSU);
    gsl_blas_ddot(U,VSU,&s2);
    s2+=aa;
    if(sigma2==0){
      //k is the number of linear terms
      sigma2=s2/(N-k-1);
    }
    // LogLikelihod
    logV=N*log(2.0*M_PI*sigma2)+ldsu+ldk+(s2/sigma2);
    logV*=(-0.5);
    //    gsl_vector_free(AR);
    //    gsl_vector_free(MA);
    gsl_vector_free(U);
    gsl_vector_free(VLU);
    gsl_vector_free(VSU);
    gsl_vector_free(PIR);
    gsl_vector_free(A);
    gsl_matrix_free(IKLT);
    gsl_matrix_free(SU);
    gsl_matrix_free(ISU);
    gsl_matrix_free(BH);
    gsl_matrix_free(L);
    gsl_matrix_free(Lt);
    gsl_matrix_free(K);
    gsl_matrix_free(IK);
    gsl_matrix_free(Phi);
    gsl_matrix_free(PI);
    gsl_matrix_free(Theta);
    gsl_matrix_free(ITheta);
    gsl_matrix_free(Eye);
  }
  return logV;
}

void pARCov(const gsl_vector *AR, gsl_matrix *A, const double sigma){
  //Returns the first p+1 (0:p)autocovariances
  // of an AR(p) process solving
  // Yule-Walker equations
  //Requires AR coefficients following BJ excluding term 0
  // Starting point for ARMAAcov
  int i,j;
  int p=AR->size;
  int n=p+1;
  gsl_matrix *A1=gsl_matrix_calloc(n,n);
  gsl_matrix *A2=gsl_matrix_calloc(n,n);
  gsl_vector *b=gsl_vector_calloc(n);
  gsl_vector *x = gsl_vector_alloc(n);
  gsl_vector *y=gsl_vector_alloc(n);
  //
  // Start forming the A1 matrix
  gsl_vector_set(y,0,-1);
  //Versión con views
  gsl_vector_view vy = gsl_vector_subvector(y,1,n-1);
  gsl_vector_memcpy(&vy.vector,AR);
  for(i=0; i<n; ++i){
    for(j=0; j<n-i; ++j){
      gsl_matrix_set(A1,i,j,y->data[(i+j)]);
    }
  }
  //
  gsl_vector_set(b,0,sigma*sigma);
  //Construct A2 matrix
  for(i=1; i<n; ++i){
    for(j=1; j<=i; ++j){
      gsl_matrix_set(A2,i,j,y->data[(i-j)]);
    }
  }
  // A=-(A1+A2)
  gsl_matrix_add(A1,A2);
  gsl_matrix_memcpy(A,A1);
  gsl_matrix_scale(A,-1);
  //
  Solve_LU(A,b,x);
  gsl_matrix_set_all(A,0);
  // Contruct A matrix properly similar to Matpol
  DiagVector(x,A);
  for(i=0; i<n; ++i){
    for(j=i; j<n; ++j){
      gsl_matrix_set(A,j,i,x->data[j-i]);
    }
  }
  //
  gsl_matrix_transpose_memcpy(A2,A);
  gsl_matrix_add(A,A2);
  for(i=0; i<n; ++i){
    gsl_matrix_set(A,i,i,x->data[0]);
  }
  gsl_matrix_free(A1);
  gsl_matrix_free(A2);
  gsl_vector_free(b);
  gsl_vector_free(x);
  gsl_vector_free(y);
}

void SigmaU(const gsl_vector *pAR, const gsl_vector *pMA, gsl_matrix *SU){
  // This functions calculates the covariance matrix of the initial
  //   conditions of a transfer function model, following Section 7.2 of 
  // 'Estimacion Maximo-Verosimil de modelos de funcion de transferencia 
  // con interrrupciones'
  // The parameters are the coefficients of the Ar and MA polynomials
  // following the BJ convention including the coefficient of order 0
  int p=pAR->size - 1;
  int q=pMA->size - 1;
  int n=p+q;
  if(p!=0&&q==0){
    int m=p+1;
    gsl_matrix *M = gsl_matrix_alloc(m,m);
    //     gsl_matrix *Ezz = gsl_matrix_alloc(p,p);
    gsl_vector *AR = gsl_vector_alloc(p);
    //Versión views
    gsl_vector_const_view v_AR = gsl_vector_const_subvector(pAR,1,p);
    gsl_vector_memcpy(AR,&v_AR.vector);
    pARCov(AR,M);
    //     Submatrix(M,0,0,p,p,Ezz);
    Submatrix(M,0,0,p,p,SU);
    //     gsl_matrix_memcpy(SU,Ezz);
    gsl_matrix_free(M);
    //     gsl_matrix_free(Ezz);
  }
  else
    if(p==0&&q!=0){
      //       gsl_matrix *Eaa = gsl_matrix_alloc(q,q);
      //       gsl_matrix_set_identity(Eaa);
      //       gsl_matrix_memcpy(SU,Eaa);
      //       gsl_matrix_free(Eaa);
      gsl_matrix_set_identity(SU);
    }
    else
      if(p!=0&&q!=0){
	int m=p+1;
	gsl_matrix *M = gsl_matrix_alloc(m,m);
	gsl_matrix *Ezz = gsl_matrix_alloc(p,p);
	gsl_vector *AR = gsl_vector_alloc(p);
	gsl_vector *MA = gsl_vector_alloc(q);
	// Versión con views
	gsl_vector_const_view v_AR = gsl_vector_const_subvector(pAR,1,p);
	gsl_vector_memcpy(AR,&v_AR.vector);
	gsl_vector_const_view v_MA = gsl_vector_const_subvector(pMA,1,q);
	gsl_vector_memcpy(MA,&v_MA.vector);
	//
	pARCov(AR,M);
	Submatrix(M,0,0,p,p,Ezz);
	gsl_matrix *Eaa = gsl_matrix_alloc(q,q);
	gsl_matrix_set_identity(Eaa);
	gsl_matrix *FI = gsl_matrix_alloc(n,n);
	gsl_matrix *Theta = gsl_matrix_alloc(n,n);
	gsl_matrix *InvTheta = gsl_matrix_alloc(n,n);
	gsl_matrix *Psi = gsl_matrix_alloc(n,n);
	gsl_matrix *DPsi = gsl_matrix_alloc(n,n);
	gsl_matrix *Psit = gsl_matrix_alloc(n,n);
	gsl_matrix *Eza = gsl_matrix_alloc(p,q);
	gsl_matrix *Ezat=gsl_matrix_alloc(q,p);
	gsl_vector *x=gsl_vector_alloc(n);
	//
	Matpol(AR,n,FI);
	Matpol(MA,n,Theta);
	MatSVDInverse(Theta,InvTheta);
	gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,FI,InvTheta,0,Psi);
	gsl_matrix_free(FI);
	gsl_matrix_free(Theta);
	gsl_matrix_free(InvTheta);
	//Jaime, 
	// 	gsl_matrix_transpose_memcpy(Psit,Psi);
	// 	for(i=0; i<n; ++i){
	// 	  gsl_vector_set(x,i,Psi->data[(i*Psi->tda)+i]);
	// 	}

	// 	DiagVector(x, DPsi);
	//Eaz = Phi+Phit-I
	gsl_matrix_set_identity(DPsi);
	gsl_matrix_add(Psi,Psit);
	gsl_matrix_sub(Psi,DPsi);
	Submatrix(Psi,0,0,p,q,Eza);
	gsl_matrix_transpose_memcpy(Ezat,Eza);
	//
	gsl_matrix_view v_U1=gsl_matrix_submatrix(SU,0,0,p,p);
	gsl_matrix_view v_U2=gsl_matrix_submatrix(SU,0,p,p,q);
	gsl_matrix_view v_U3=gsl_matrix_submatrix(SU,p,0,q,p);
	gsl_matrix_view v_U4=gsl_matrix_submatrix(SU,p,p,q,q);
	//
	gsl_matrix_memcpy(&v_U1.matrix,Ezz);
	gsl_matrix_memcpy(&v_U2.matrix,Eza);
	gsl_matrix_memcpy(&v_U3.matrix,Ezat);
	gsl_matrix_memcpy(&v_U4.matrix,Eaa);
	//
	gsl_matrix_free(M);
	gsl_matrix_free(Ezz);
	gsl_matrix_free(Eaa);
	gsl_matrix_free(Eza);
	gsl_matrix_free(Ezat);
	gsl_matrix_free(Psi);
	gsl_matrix_free(Psit);
	gsl_matrix_free(DPsi);
	gsl_vector_free(x);
      }
}



void Solve_LU(gsl_matrix *A, const gsl_vector *b, gsl_vector *x){
  int n=b->size;
  gsl_permutation *p=gsl_permutation_alloc(n);
  int s;
  gsl_linalg_LU_decomp(A,p,&s);
  gsl_linalg_LU_solve(A,p,b,x);
  gsl_permutation_free(p);
}
