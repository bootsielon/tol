#ifndef CMM_H
   #define CMM_H

   #define ATL_mmNOMULADD
   #define ATL_mmLAT 1
   #define ATL_mmMU  6
   #define ATL_mmNU  1
   #define ATL_mmKU  120
   #define MB 120
   #define NB 120
   #define KB 120
   #define NBNB 14400
   #define MBNB 14400
   #define MBKB 14400
   #define NBKB 14400
   #define NB2 240
   #define NBNB2 28800

   #define ATL_MulByNB(N_) ((N_) * 120)
   #define ATL_DivByNB(N_) ((N_) / 120)
   #define ATL_MulByNBNB(N_) ((N_) * 14400)
void ATL_cJIK120x120x120TN120x120x0_a1_b0(const int M, const int N, const int K, const TYPE alpha, const TYPE *A, const int lda, const TYPE *B, const int ldb, const TYPE beta, TYPE *C, const int ldc);
void ATL_cJIK120x120x120TN120x120x0_a1_b1(const int M, const int N, const int K, const TYPE alpha, const TYPE *A, const int lda, const TYPE *B, const int ldb, const TYPE beta, TYPE *C, const int ldc);
void ATL_cJIK120x120x120TN120x120x0_a1_bX(const int M, const int N, const int K, const TYPE alpha, const TYPE *A, const int lda, const TYPE *B, const int ldb, const TYPE beta, TYPE *C, const int ldc);

   #define NBmm_b1(m_, n_, k_, al_, A_, lda_, B_, ldb_, be_, C_, ldc_) \
{ \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_), lda_, (B_), ldb_, ATL_rnone, C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_b1(m_, n_, k_, al_, (A_), lda_, (B_)+NBNB, ldb_, ATL_rone, (C_)+1, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_)+NBNB, ldb_, ATL_rnone, C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_b1(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_), ldb_, ATL_rone, (C_)+1, ldc_); \
   }
   #define NBmm_b0(m_, n_, k_, al_, A_, lda_, B_, ldb_, be_, C_, ldc_) \
{ \
   ATL_cJIK120x120x120TN120x120x0_a1_b0(m_, n_, k_, al_, (A_), lda_, (B_), ldb_, ATL_rzero, C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_b0(m_, n_, k_, al_, (A_), lda_, (B_)+NBNB, ldb_, ATL_rzero, (C_)+1, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_)+NBNB, ldb_, ATL_rnone, C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_b1(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_), ldb_, ATL_rone, (C_)+1, ldc_); \
   }
   #define NBmm_bX(m_, n_, k_, al_, A_, lda_, B_, ldb_, be_, C_, ldc_) \
{ \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_), lda_, (B_), ldb_, -(be_), C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_), lda_, (B_)+NBNB, ldb_, be_, (C_)+1, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_bX(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_)+NBNB, ldb_, ATL_rnone, C_, ldc_); \
   ATL_cJIK120x120x120TN120x120x0_a1_b1(m_, n_, k_, al_, (A_)+NBNB, lda_, (B_), ldb_, ATL_rone, (C_)+1, ldc_); \
   }
   #define rNBmm_b1 ATL_sJIK120x120x120TN120x120x0_a1_b1
   #define rNBmm_b0 ATL_sJIK120x120x120TN120x120x0_a1_b0
   #define rNBmm_bX ATL_sJIK120x120x120TN120x120x0_a1_bX

#endif
