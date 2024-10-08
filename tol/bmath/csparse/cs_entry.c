#include "cs.h"
/* add an entry to a triplet matrix; return 1 if ok, 0 otherwise */
int cs_entry (cs *T, int i, int j, double x)
{
    if (!T || (T->nz >= T->nzmax && !cs_sprealloc (T, 2*(T->nzmax)))) return(0);
    if (T->x) T->x [T->nz] = x ;
    T->i [T->nz] = i ;
    T->p [T->nz++] = j ;
    T->m = CS_MAX (T->m, i+1) ;
    T->n = CS_MAX (T->n, j+1) ;
    return (1) ;
}
