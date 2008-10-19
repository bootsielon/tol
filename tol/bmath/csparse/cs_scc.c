#include "cs.h"
/* find the strongly connected components of a square matrix */
csd *cs_scc (cs *A)
{
    int n, i, k, b = 0, *w, top, *x1, *p1, *P, *R ;
    cs *AT ;
    csd *D ;
    if (!A) return (NULL) ;
    n = A->n ;
    D = cs_dalloc (n, 0) ;
    AT = cs_transpose (A, 0) ;		    /* AT = A' */ 
    w = cs_calloc (3*n, sizeof (int)) ;	    /* allocate workspace */
    x1 = w + n ; p1 = w + 2*n ;
    if (!D || !AT || !w) return (cs_ddone (D, AT, w, 0)) ;
    P = D->P ; R = D->R ;
    top = n ;
    for (i = 0 ; i < n ; i++)	/* first dfs(A) to find finish times (x1) */
    {
	if (w [i] == 0) top = cs_dfs (i, A, top, x1, w, 1, NULL, p1) ;
    }
    top = n ;
    b = n ;
    for (k = 0 ; k < n ; k++)	/* dfs(A') to find strongly connnected comp. */
    {
	i = x1 [k] ;		/* get i in reverse order of finish times */
	if (w [i] == 2) continue ;  /* skip node i if already ordered */
	R [b--] = top ;		/* node i is the start of a component in P */
	top = cs_dfs (i, AT, top, P, w, 2, NULL, p1) ;
    }
    R [b] = 0 ;			/* first block starts at zero; shift R up */
    for (k = b ; k <= n ; k++) R [k-b] = R [k] ;
    D->nb = R [n+1] = b = n-b ;	/* b = # of strongly connected components */
    return (cs_ddone (D, AT, w, 1)) ;
}
