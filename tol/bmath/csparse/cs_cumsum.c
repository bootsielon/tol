#include "cs.h"
/* p [0..n] = cumulative sum of c [0..n-1], and then copy p [0..n-1] into c */
int cs_cumsum (int *p, int *c, int n)
{
    int i, nz = 0 ;
    if (!p || !c) return (-1) ;	    /* check inputs */
    for (i = 0 ; i < n ; i++)
    {
	p [i] = nz ;
	nz += c [i] ;
	c [i] = p [i] ;
    }
    p [n] = nz ;
    return (nz) ;		    /* return sum (c [0..n-1]) */
}
