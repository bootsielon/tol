#include "cs.h"
/* depth-first-search of the graph of a matrix, starting at node j */
int cs_dfs (int j, cs *L, int top, int *xi, int *w, int mark, int *Pinv, int *p1)
{
    int p, p2, done, jnew, head = 0, *Lp = L->p, *Li = L->i ;
    if (!L || !xi || !w) return (-1) ;
    Lp = L->p ; Li = L->i ;
    xi [0] = j ;		/* initialize the recursion stack */
    while (head >= 0)
    {
	j = xi [head] ;		/* get j from the top of the recursion stack */
	jnew = Pinv ? (Pinv [j]) : j ;
	if (w [j] != mark)
	{
	    w [j] = mark ;	    /* first time node j has been visited */
	    p1 [head] = (jnew < 0) ? 0 : (Lp [jnew]) ;
	}
	done = 1 ;		    /* node j done if no unvisited neighbors */
	p2 = (jnew < 0) ? -1 : (Lp [jnew+1]) ;
	for (p = p1 [head] ; p < p2 ; p++)  /* examine all neighbors of j */
	{
	    if (w [Li [p]] == mark) continue ;	/* skip visited node i=Li[p] */
	    p1 [head] = p ;	    /* pause depth-first search of node j */
	    xi [++head] = Li [p] ;  /* start dfs at node i */
	    done = 0 ;		    /* node j is not done */
	    break ;		    /* break, to start dfs (i) */
	}
	if (done)		/* depth-first search at node j is done */
	{
	    head-- ;		/* remove j from the recursion stack */
	    xi [--top] = j ;	/* and place in the output stack */
	}
    }
    return (top) ;
}
