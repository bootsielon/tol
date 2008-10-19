#include "cs.h"
/* x=A\b where A can be rectangular; b overwritten with solution */
int cs_qrsol (cs *A, double *b, int order)
{
    double *x ;
    css *S ;
    csn *N ;
    cs *AT = NULL ;
    int k, m, n, ok ;
    if (!A || !b) return (0) ;		/* check inputs */
    n = A->n ;
    m = A->m ;
    if (m >= n)
    {
	S = cs_sqr (A, order, 1) ;	/* ordering and symbolic analysis */
	N = cs_qr (A, S) ;		/* numeric QR factorization */
	x = cs_calloc (m, sizeof (double)) ;
	ok = (S && N && x && S->m2 == m) ;
	if (ok)
	{
	    cs_ipvec (m, S->Pinv, b, x) ;   /* x(0:m-1) = P*b(0:m-1) */
	    for (k = 0 ; k < n ; k++)	    /* apply Householder refl. to x */
	    {
		cs_happly (N->L, k, N->B [k], x) ;
	    }
	    cs_usolve (N->U, x) ;	    /* x = R\x */
	    cs_ipvec (n, S->Q, x, b) ;	    /* b(0:n-1) = Q*x (permutation) */
	}
    }
    else
    {
	AT = cs_transpose (A, 1) ;	/* Ax=b is underdetermined */
	S = cs_sqr (AT, order, 1) ;	/* ordering and symbolic analysis */
	N = cs_qr (AT, S) ;		/* numeric QR factorization of A' */
	x = cs_calloc (n, sizeof (double)) ;
	ok = (AT && S && N && x && S->m2 == n) ;
	if (ok)
	{
	    cs_pvec (m, S->Q, b, x) ;	    /* x(0:m-1) = Q'*b (permutation) */
	    cs_utsolve (N->U, x) ;	    /* x = R'\x */
	    for (k = m-1 ; k >= 0 ; k--)    /* apply Householder refl. to x */
	    {
		cs_happly (N->L, k, N->B [k], x) ;
	    }
	    cs_pvec (n, S->Pinv, x, b) ;    /* b (0:n-1) = P'*x */
	}
    }
    cs_free (x) ;
    cs_sfree (S) ;
    cs_nfree (N) ;
    cs_spfree (AT) ;
    return (ok) ;
}
