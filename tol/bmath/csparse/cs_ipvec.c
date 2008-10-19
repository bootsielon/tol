#include "cs.h"
/* x(P) = b, for dense vectors x and b; P=NULL denotes identity */
int cs_ipvec (int n, int *P, double *b, double *x)
{
    int k ;
    if (!x || !b) return (0) ;				    /* check inputs */
    for (k = 0 ; k < n ; k++) x [P ? P [k] : k] = b [k] ;
    return (1) ;
}
