#include "cs.h"
/* y = A*x+y; return 1 if OK, 0 if error occurs */
int cs_gaxpy (cs *A, double *x, double *y)
{
    int p, j, n, *Ap, *Ai ;
    double *Ax ;
    if (!A || !x || !y) return (0) ;	    /* check inputs */
    n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    for (j = 0 ; j < n ; j++)
    {
	for (p = Ap [j] ; p < Ap [j+1] ; p++)
	{
	    y [Ai [p]] += Ax [p] * x [j] ;
	}
    }
    return (1) ;
}
