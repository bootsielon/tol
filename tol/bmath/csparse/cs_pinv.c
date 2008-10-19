#include "cs.h"
/* Pinv = P', or P = Pinv' */
int *cs_pinv (int *P, int n)
{
    int k, *Pinv ;
    if (!P) return (NULL) ;			/* P = NULL denotes identity */
    Pinv = cs_malloc (n, sizeof (int)) ;	/* allocate resuult */
    if (!Pinv) return (NULL) ;			/* out of memory */
    for (k = 0 ; k < n ; k++) Pinv [P [k]] = k ;/* invert the permutation */    
    return (Pinv) ;				/* return result */
}
