#include "cs.h"
/* 1-norm of a sparse matrix = max (sum (abs (A))), largest column sum */
double cs_norm (cs *A)
{
    int p, j, n, *Ap ;
    double *Ax, norm = 0, s = 0 ;
    if (!A || !A->x) return (-1) ;		/* check inputs */
    n = A->n ; Ap = A->p ; Ax = A->x ;
    for (j = 0 ; j < n ; j++)
    {
	for (s = 0, p = Ap [j] ; p < Ap [j+1] ; p++) s += fabs (Ax [p]) ;
	norm = CS_MAX (norm, s) ;
    }
    return (norm) ;
}
