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
//#include "linalgebra.h"
#include "gsl_ext.h"
#include "LLKKFSS.h"
#include <iostream>
using std::cout;
using std::endl;



void SSselfstart(gsl_vector *X0, gsl_matrix *P, 
		 gsl_matrix *Phi, gsl_matrix *Gamma, 
		 gsl_matrix *EQE,
		 gsl_vector *U0){
  int r = Gamma->size2;
  int m = Phi->size2;
  int n = U0->size;

//   std::cout << "r " << r << std::endl;
//   std::cout << "m " << m << std::endl;
//   std::cout << "n " << n << std::endl;
//   std::cout << "Size of Gamma " << Gamma->size1 << " " << Gamma->size2 << std::endl;
//   std::cout << "Size of U0 " << U0->size <<  std::endl;
//   std::cout << "Size of X0 " << X0->size <<  std::endl;

//   int k = E->size2;
  // This is for X
  gsl_vector *U = gsl_vector_alloc(m);
  gsl_matrix *IP = gsl_matrix_alloc(m,m);
  gsl_matrix *IPI = gsl_matrix_alloc(m,m);
  // This is for P
  gsl_matrix  *MP  = gsl_matrix_alloc(m,m);
  gsl_matrix  *MPM = gsl_matrix_alloc(m,m);
  // X
   gsl_matrix_set_identity(IP);
   gsl_matrix_sub(IP,Phi);
   gsl_MatSVDInverse(IP,IPI);
   gsl_blas_dgemv(CblasNoTrans,1,Gamma,U0,0,U);
   gsl_blas_dgemv(CblasNoTrans,1,IPI,U,0,X0);
  // P
   gsl_matrix_set_identity(P);
   for(int i=0; i< 200; ++i){
     gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Phi,P,0,MP); 
     gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,MP,Phi,0,MPM);
     gsl_matrix_add(MPM,EQE);
     gsl_matrix_memcpy(P,MPM);
   }

  gsl_vector_free(U);
  gsl_matrix_free(IP);
  gsl_matrix_free(IPI);
  gsl_matrix_free(MP);
  gsl_matrix_free(MPM);
}



double LLKKFSS(gsl_matrix *H, gsl_matrix *D, gsl_matrix *C,
	       gsl_matrix *Phi, gsl_matrix *Gamma, gsl_matrix *E,
	       gsl_matrix *Q, gsl_matrix *R, gsl_matrix *S,
	       gsl_matrix *Z, gsl_matrix *U,
	       gsl_vector *X0, gsl_matrix *P0, int stm){
  //
  // Function to evaluate the log-loikelihood of a state-space system
  //

  // Notice that R and Q are spd matrices
  // S is symmetric but not spd
  // B must be spd
  // Matrix C is not really necessary, since it is to be I

  int i;
  int m = Z->size1, T = Z->size2;
  int r = U->size1;
  int k = Phi->size2;
  int c = C->size2;
  int e = E->size2;

  double logV, s, db, cb, suma;
//   cb=m*log(2*M_PI);
  cb=m*T*(M_LN2+M_LNPI);

  //Initialize some matrices
  //   gsl_matrix  *P = gsl_matrix_alloc(k,k);  
  gsl_vector  *X = gsl_vector_alloc(k);     
  gsl_matrix  *K = gsl_matrix_alloc(k,m); 
  gsl_matrix  *B = gsl_matrix_alloc(m,m);  
  gsl_matrix  *Binv= gsl_matrix_alloc(m,m); 


  //  gsl_matrix_set_zero(K); 

  // ESC', CRC', EQE' are constant matrices
  gsl_matrix  *CR  = gsl_matrix_alloc(m,c);
  gsl_matrix  *CRC = gsl_matrix_alloc(m,m);
  gsl_matrix  *ES  = gsl_matrix_alloc(k,c);
  gsl_matrix  *ESC = gsl_matrix_alloc(k,m);
  gsl_matrix  *EQ  = gsl_matrix_alloc(k,e);
  gsl_matrix  *EQE = gsl_matrix_alloc(k,k);
  //
  gsl_blas_dsymm(CblasRight,CblasUpper,1,R,C,0,CR);
  gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,CR,C,1,CRC);
  //
  gsl_blas_dsymm(CblasRight,CblasUpper,1,S ,E,0,ES);
  gsl_blas_dgemm(CblasNoTrans,CblasTrans  ,1,ES,C,0,ESC);
  //
  gsl_blas_dsymm(CblasRight,CblasUpper,1,Q ,E,0,EQ);
  gsl_blas_dgemm(CblasNoTrans,CblasTrans  ,1,EQ,E,0,EQE);
  //
  gsl_matrix_free(CR);
  gsl_matrix_free(ES);
  gsl_matrix_free(EQ);
  //
  //
  //Starting points
  //
  if(stm==1){
    gsl_vector *U0 = gsl_vector_calloc(r);
    SSselfstart(X0, P0, Phi, Gamma, EQE, U0);
    gsl_vector_free(U0);
  }
  //
  //
  // Create temporary matrices
  gsl_vector  *Zt     = gsl_vector_alloc(m);
  gsl_vector  *Rt     = gsl_vector_alloc(m);
  gsl_vector  *Ut     = gsl_vector_alloc(r);
  gsl_vector  *MXt    = gsl_vector_alloc(k);
  gsl_vector  *BinvRt = gsl_vector_alloc(m);
  gsl_matrix  *PH  = gsl_matrix_alloc(k,m);
  //   gsl_matrix *HPH  = gsl_matrix_alloc(m,m);
  gsl_matrix  *MPH = gsl_matrix_alloc(k,m);
  gsl_matrix  *MP  = gsl_matrix_alloc(k,k);
  gsl_matrix  *MPM = gsl_matrix_alloc(k,k);
  gsl_matrix  *KB  = gsl_matrix_alloc(k,m);
  gsl_svd_matrix_t svd;
  gsl_svd_init(B,0,&svd);
  //
  // Start Kalman Filter
  //
  logV=0;
  suma=0;
  //
  for (i=0; i<T; ++i){
    // Recall that SetSerMat returns a matrix whose rows are 
    // the data of the series of a set.
    gsl_matrix_get_col(Zt,Z,i);
    gsl_matrix_get_col(Ut,U,i);
    gsl_vector_memcpy(Rt,Zt);
    //    
    // Bt = HPH'+CRC'  
    //
    gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,P0,H,0,PH);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,H,PH,0, B);
    gsl_matrix_add(B,CRC);
//     db = gsl_MatSVDdet(B);
//     gsl_MatSVDInverse(B,Binv); 
//     db=gsl_MatSVDdetInverse(B,Binv);
    gsl_svd_set(B,&svd);
    gsl_svd_pinverse(&svd, &Binv);
    db=svd.det;
    //
    //     
    // Kt =(PhiPH + ESC).Binv 
    //   
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Phi,PH,0,MPH);
    gsl_matrix_add(MPH,ESC);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,MPH,Binv,0,K);
    //
    //
    // Pt =(PhiPPhi' + EQE'-KBK') 
    //
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,Phi,P0,0,MP); 
    gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,MP,Phi,0,MPM);
    gsl_matrix_add(MPM,EQE);
    gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,K,B,0,KB); 
    //    gsl_blas_dgemm(CblasNoTrans,CblasTrans,-1,KB,K,0,P);
    gsl_blas_dgemm(CblasNoTrans,CblasTrans,-1,KB,K,1,MPM);
    //     gsl_matrix_add(P,MPM);
    //     gsl_matrix_memcpy(P0,P);
    gsl_matrix_memcpy(P0,MPM);
    //
    //Rt = Zt - H_*Xt - D_*Ut;
    //
    gsl_blas_dgemv(CblasNoTrans,-1,H,X0,1,Rt);   
    gsl_blas_dgemv(CblasNoTrans,-1,D,Ut,1,Rt); 
    //
    //
    //Xt+1 := Phi*Xt + Gamma*Ut + K*Rt
    //
    gsl_blas_dgemv(CblasNoTrans,1,Phi ,X0,0,X);
    gsl_blas_dgemv(CblasNoTrans,1,Gamma ,Ut,1,X); 
    gsl_blas_dgemv(CblasNoTrans,1,K,Rt,1,X); 
    gsl_vector_memcpy(X0,X);
    //
    //     gsl_vector_set_zero(BinvRt);
    gsl_blas_dgemv(CblasNoTrans,1,Binv,Rt,0,BinvRt);   
    gsl_blas_ddot(Rt,BinvRt,&s);
    //s+=(log(db));
    //     s+=(m*log(2*M_PI));
    //     s*=-0.5;
    logV+=(s+log(db));

  }
  gsl_svd_done(&svd);
//   std::cout << "suma " << suma << std::endl;
  suma=logV;
  logV+=cb;
  logV*=(-0.5);
//    for(int i=0; i<m; ++i){
//      for(int j=0; j<r; ++j){
//        cout << gsl_matrix_get(Gamma,i,j) << " " ;      
//      }
//      cout << endl;
//    }
//   for(int i=0; i<m; ++i){
//     cout << gsl_vector_get(X,i) << endl;
//   }


  //
  gsl_vector_free(Zt);    
  gsl_vector_free(Rt);    
  gsl_vector_free(Ut);    
  gsl_vector_free(MXt);   
  gsl_vector_free(BinvRt);
  gsl_matrix_free(PH);
  //   gsl_matrix_free(HPH);
  gsl_matrix_free(MPH);
  gsl_matrix_free(MP);
  gsl_matrix_free(MPM);
  gsl_matrix_free(KB);
  //
  gsl_matrix_free(CRC);
  gsl_matrix_free(EQE);
  //
  //   gsl_matrix_free(P);
  gsl_matrix_free(K);
  gsl_matrix_free(B);
  gsl_matrix_free(Binv);
  gsl_vector_free(X);
  return logV;


}


