#include "cs.h"
/* ordering and symbolic analysis for a Cholesky factorization */
css *cs_schol (cs *A, int order)
{
    int n, *c, *post, *P ;
    cs *C ;
    css *S ;
    if (!A) return (NULL) ;		    /* check inputs */
    n = A->n ;
    S = cs_calloc (1, sizeof (css)) ;	    /* allocate symbolic analysis */
    if (!S) return (NULL) ;		    /* out of memory */
    P = cs_amd (A, order) ;		    /* P = amd(A+A'), or natural */
    S->Pinv = cs_pinv (P, n) ;		    /* find inverse permutation */
    cs_free (P) ;
    if (order >= 0 && !S->Pinv) return (cs_sfree (S)) ;
    C = cs_symperm (A, S->Pinv, 0) ;	    /* C = spones(triu(A(P,P))) */
    S->parent = cs_etree (C, 0) ;	    /* find etree of C */
    post = cs_post (n, S->parent) ;	    /* postorder the etree */
    c = cs_counts (C, S->parent, post, 0) ; /* find column counts of chol(C) */
    cs_free (post) ;
    cs_spfree (C) ;
    S->cp = cs_malloc (n+1, sizeof (int)) ; /* find column pointers for L */
    S->unz = S->lnz = cs_cumsum (S->cp, c, n) ;
    cs_free (c) ;
    return ((S->lnz >= 0) ? S : cs_sfree (S)) ;
}
