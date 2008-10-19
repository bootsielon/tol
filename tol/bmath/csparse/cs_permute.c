#include "cs.h"
/* C = A(P,Q) where P and Q are permutations of 0..m-1 and 0..n-1. */
cs *cs_permute (cs *A, int *Pinv, int *Q, int values)
{
    int p, j, k, nz = 0, m, n, *Ap, *Ai, *Cp, *Ci ;
    double *Cx, *Ax ;
    cs *C ;
    if (!A) return (NULL) ;		/* check inputs */
    m = A->m ; n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    C = cs_spalloc (m, n, Ap [n], values && Ax != NULL, 0) ;
    if (!C) return (cs_done (C, NULL, NULL, 0)) ;   /* out of memory */
    Cp = C->p ; Ci = C->i ; Cx = C->x ;
    for (k = 0 ; k < n ; k++)
    {
	Cp [k] = nz ;			/* column k of C is column Q[k] of A */
	j = Q ? (Q [k]) : k ;
	for (p = Ap [j] ; p < Ap [j+1] ; p++)
	{
	    if (Cx) Cx [nz] = Ax [p] ;	/* row i of A is row Pinv[i] of C */
	    Ci [nz++] = Pinv ? (Pinv [Ai [p]]) : Ai [p] ;
	}
    }
    Cp [n] = nz ;			/* finalize the last column of C */
    return (cs_done (C, NULL, NULL, 1)) ;
}
