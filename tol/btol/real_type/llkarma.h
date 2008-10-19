#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_
void pMatpol(const gsl_vector *v, int n, gsl_matrix *M);
void Matpol(const gsl_vector *v, int n, gsl_matrix *M);
void DiagVector(const gsl_vector *v, gsl_matrix *D);
void MatSVDInverse(const gsl_matrix *M, gsl_matrix *Mi);
void MatInverse(const gsl_matrix *M, gsl_matrix *Mi);
double Matdet(const gsl_matrix *M);
void H1_Matrix(const gsl_vector *v, gsl_matrix *H1);
void H2_Matrix(const gsl_vector *v, gsl_matrix *H2);
void Submatrix(const gsl_matrix *m, int r, int c, int nr, int nc, gsl_matrix *S);
void H_Matrix(const gsl_vector *AR, const gsl_vector *MA, int n, gsl_matrix *H);
void BigHMatrix(const gsl_vector *pAR, const gsl_vector *pMA, int n, gsl_matrix *H);
double LLKR(const gsl_vector *R, const gsl_vector *pAR, const gsl_vector *pMA, double &sigma2, const int k=0);
double MatSVDdet(const gsl_matrix *M);
void pARCov(const gsl_vector *AR, gsl_matrix *A, const double sigma = 1);
void Solve_LU(gsl_matrix *A, const gsl_vector *b, gsl_vector *x);
void SigmaU(const gsl_vector *pAR, const gsl_vector *pMA, gsl_matrix *SU);
void IthetaCol1(const gsl_vector *pMA, gsl_vector* X, int N);
void IthetaHMatrix(const gsl_vector *Itheta1, const gsl_vector *v, 
		   gsl_matrix *M);
void PIRMatrix2(gsl_vector *v, const gsl_vector *pAR, 
		const gsl_vector *pMA, 
		const gsl_vector *R, gsl_vector *PIR);
void PhiRMatrix(const gsl_vector *pAR,const gsl_vector *R, gsl_vector *PIR);
void MatSVDdetInverse(const gsl_matrix *M, gsl_matrix *Mi, double &det);
#endif
