#include "cs.h"
/* x=A\b where A is unsymmetric; b overwritten with solution */
int cs_lusol (cs *A, double *b, int order, double tol)
{
    double *x ;
    css *S ;
    csn *N ;
    int n, ok ;
    if (!A || !b) return (0) ;		/* check inputs */
    n = A->n ;
    S = cs_sqr (A, order, 0) ;		/* ordering and symbolic analysis */
    N = cs_lu (A, S, tol) ;		/* numeric LU factorization */
    x = cs_malloc (n, sizeof (double)) ;
    ok = (S && N && x) ;
    if (ok)
    {
	cs_ipvec (n, N->Pinv, b, x) ;	/* x = P*b */
	cs_lsolve (N->L, x) ;		/* x = L\x */
	cs_usolve (N->U, x) ;		/* x = U\x */
	cs_ipvec (n, S->Q, x, b) ;	/* b = Q*x */
    }
    cs_free (x) ;
    cs_sfree (S) ;
    cs_nfree (N) ;
    return (ok) ;
}
