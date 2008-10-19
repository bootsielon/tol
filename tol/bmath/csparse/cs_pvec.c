#include "cs.h"
/* x = b(P), for dense vectors x and b; P=NULL denotes identity */
int cs_pvec (int n, int *P, double *b, double *x)
{
    int k ;
    if (!x || !b) return (0) ;				    /* check inputs */
    for (k = 0 ; k < n ; k++) x [k] = b [P ? P [k] : k] ;
    return (1) ;
}
