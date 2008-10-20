#ifndef SMM_H
   #define SMM_H

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
   #define NBmm ATL_sJIK120x120x120TN120x120x0_a1_b1
   #define NBmm_b1 ATL_sJIK120x120x120TN120x120x0_a1_b1
   #define NBmm_b0 ATL_sJIK120x120x120TN120x120x0_a1_b0
   #define NBmm_bX ATL_sJIK120x120x120TN120x120x0_a1_bX

#endif
