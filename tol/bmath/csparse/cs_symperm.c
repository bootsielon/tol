#include "cs.h"
/* C = A(p,p) where A and C are symmetric the upper part stored, Pinv not P */
cs *cs_symperm (cs *A, int *Pinv, int values)
{
    int i, j, p, q, i2, j2, n, *Ap, *Ai, *Cp, *Ci, *w ;
    double *Cx, *Ax ;
    cs *C ;
    if (!A) return (NULL) ;
    n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    C = cs_spalloc (n, n, Ap [n], values && (Ax != NULL), 0) ;
    w = cs_calloc (n, sizeof (int)) ;
    if (!C || !w) return (cs_done (C, w, NULL, 0)) ;	/* out of memory */
    Cp = C->p ; Ci = C->i ; Cx = C->x ;
    for (j = 0 ; j < n ; j++)		/* count entries in each column of C */
    {
	j2 = Pinv ? Pinv [j] : j ;	/* column j of A is column j2 of C */
	for (p = Ap [j] ; p < Ap [j+1] ; p++)
	{
	    i = Ai [p] ;
	    if (i > j) continue ;	/* skip lower triangular part of A */
	    i2 = Pinv ? Pinv [i] : i ;	/* row i of A is row i2 of C */
	    w [CS_MAX (i2, j2)]++ ;	/* column count of C */
	}
    }
    cs_cumsum (Cp, w, n) ;		/* compute column pointers of C */
    for (j = 0 ; j < n ; j++)
    {
	j2 = Pinv ? Pinv [j] : j ;	/* column j of A is column j2 of C */
	for (p = Ap [j] ; p < Ap [j+1] ; p++)
	{
	    i = Ai [p] ;
	    if (i > j) continue ;	/* skip lower triangular part of A*/
	    i2 = Pinv ? Pinv [i] : i ;	/* row i of A is row i2 of C */
	    Ci [q = w [CS_MAX (i2, j2)]++] = CS_MIN (i2, j2) ;
	    if (Cx) Cx [q] = Ax [p] ;
	}
    }
    return (cs_done (C, w, NULL, 1)) ;	/* success; free workspace, return C */
}
