#include "cs.h"
/* wrapper for malloc */
void *cs_malloc (int n, size_t size)
{
    return ((n < INT_MAX/(int) size) ? CS_MALLOC (CS_MAX(n,1) * size) : NULL) ;
}

/* wrapper for calloc */
void *cs_calloc (int n, size_t size)
{
    return ((n < INT_MAX/(int) size) ? CS_CALLOC (CS_MAX(n,1), size) : NULL) ;
}

/* wrapper for free */
void *cs_free (void *p)
{
    if (p) CS_FREE (p) ;    /* free p if it is not already NULL */
    return (NULL) ;	    /* return NULL to simplify the use of cs_free */
}

/* wrapper for realloc */
void *cs_realloc (void *p, int n, size_t size, int *ok)
{
    void *p2 ;
    *ok = (n < INT_MAX / (int) size) ;	   /* guard against int overflow */
    if (!(*ok)) return (p) ;		   /* do not change p if n too large */
    p2 = CS_REALLOC (p, CS_MAX (n,1) * size) ;	/* realloc the block */
    *ok = (p2 != NULL) ;
    return ((*ok) ? p2 : p) ;		   /* return original p if failure */
}
