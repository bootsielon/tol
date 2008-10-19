#include "cs.h"
/* x=A\b where A is symmetric positive definite; b overwritten with solution */
int cs_cholsol (cs *A, double *b, int order)
{
    double *x ;
    css *S ;
    csn *N ;
    int n, ok ;
    if (!A || !b) return (0) ;		/* check inputs */
    n = A->n ;
    S = cs_schol (A, order) ;		/* ordering and symbolic analysis */
    N = cs_chol (A, S) ;		/* numeric Cholesky factorization */
    x = cs_malloc (n, sizeof (double)) ;
    ok = (S && N && x) ;
    if (ok)
    {
	cs_ipvec (n, S->Pinv, b, x) ;	/* x = P*b */
	cs_lsolve (N->L, x) ;		/* x = L\x */
	cs_ltsolve (N->L, x) ;		/* x = L'\x */
	cs_pvec (n, S->Pinv, x, b) ;	/* b = P'*x */
    }
    cs_free (x) ;
    cs_sfree (S) ;
    cs_nfree (N) ;
    return (ok) ;
}
