#ifndef _KALMAN_H_ 
#define _KALMAN_H_ 

void SSselfstart(gsl_vector *X0, gsl_matrix *P, 
		 gsl_matrix *Phi, gsl_matrix *Gamma, 
		 gsl_matrix *EQE, gsl_vector *U0);


double LLKKFSS(gsl_matrix *H, gsl_matrix *D, gsl_matrix *C,
	       gsl_matrix *Phi, gsl_matrix *Gamma, gsl_matrix *E,
	       gsl_matrix *Q, gsl_matrix *R, gsl_matrix *S,
	       gsl_matrix *Z, gsl_matrix *U,
	       gsl_vector *X0, gsl_matrix *P0, int stm);

#endif
