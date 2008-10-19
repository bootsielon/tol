#include "cs.h"
/* C = alpha*A + beta*B */
cs *cs_add (cs *A, cs *B, double alpha, double beta)
{
    int p, j, nz = 0, anz, *Cp, *Ci, *Bp, *Bi, m, n, bnz, *w, values ;
    double *x, *Bx, *Cx ;
    cs *C ;
    if (!A || !B) return (NULL) ;	/* check inputs */
    m = A->m ; anz = A->p [A->n] ;
    m = A->m ; n = B->n ; Bp = B->p ; Bi = B->i ; Bx = B->x ; bnz = Bp [n] ;
    w = cs_calloc (m, sizeof (int)) ;
    values = (A->x != NULL) && (Bx != NULL) ;
    x = values ? cs_malloc (m, sizeof (double)) : NULL ;
    C = cs_spalloc (m, n, anz + bnz, values, 0) ;
    if (!C || !w || (values && !x)) return (cs_done (C, w, x, 0)) ;
    Cp = C->p ; Ci = C->i ; Cx = C->x ;
    for (j = 0 ; j < n ; j++)
    {
	Cp [j] = nz ;			/* column j of C starts here */
	nz = cs_scatter (A, j, alpha, w, x, j+1, C, nz) ;  /* alpha*A(:,j)*/
	nz = cs_scatter (B, j, beta,  w, x, j+1, C, nz) ;  /* beta*B(:,j) */
	if (values) for (p = Cp [j] ; p < nz ; p++) Cx [p] = x [Ci [p]] ;
    }
    Cp [n] = nz ;			/* finalize the last column of C */
    cs_sprealloc (C, 0) ;		/* remove extra space from C */
    return (cs_done (C, w, x, 1)) ;	/* success; free workspace, return C */
}
