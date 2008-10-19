#include "cs.h"
/* process edge (j,i) of the matrix */
static void cs_cedge (int j, int i, int k, int *first, int *prevnbr, int *c,
    int *prevleaf, int *set)
{
    int q, s, sparent ;
    if (first [j] > prevnbr [i])    /* j is a leaf of subtree of i */
    {
	c [j]++ ;
	if (prevleaf [i] != -1)
	{
	    /* q = least common ancestor of prevleaf[i] and j */
	    for (q = prevleaf [i] ; q != set [q] ; q = set [q]) ;
	    for (s = prevleaf [i] ; s != q ; s = sparent)
	    {
		sparent = set [s] ;	/* traverse up etree */
		set [s] = q ;		/* path compression */
	    }
	    c [q]-- ;
	}
	prevleaf [i] = j ;	    /* mark prevleaf [i] to be j */
    }
    prevnbr [i] = k ;		    /* flag i has having been seen at step k */
}

/* c = column counts of LL'=A or LL'=A'A, given parent and post ordering */
int *cs_counts (cs *A, int *parent, int *post, int ata)
{
    int i, j, k, p, n, m, ii, s, *ATp, *ATi, *prevnbr, *prevleaf, *set,
	*head = NULL, *next = NULL, *c, *w, *first ;
    cs *AT ;
    if (!A || !parent || !post) return (NULL) ;	    /* check inputs */
    m = A->m ; n = A->n ;
    s = 4*n + (ata ? (n+m+1) : 0) ;
    c = cs_malloc (n, sizeof (int)) ;		    /* allocate column counts */
    w = cs_malloc (s, sizeof (int)) ; first = w ;   /* allocate workspace */
    AT = cs_transpose (A, 0) ;
    if (!AT || !c || !w) return (cs_idone (c, AT, w, 1)) ;
    for (k = 0 ; k < s ; k++) w [k] = -1 ;	/* clear workspace w [0..s-1] */
    for (k = 0 ; k < n ; k++)			/* find first [j] */
    {
	j = post [k] ;
	c [j] = (first [j] == -1) ? 1 : 0 ;	/* c[j]=1 if j is a leaf */
	for ( ; (j != -1) && (first [j] == -1) ; j = parent [j]) first [j] = k ;
    }
    ATp = AT->p ; ATi = AT->i ;
    prevnbr = w + n ; prevleaf = w + 2*n ; set = w + 3*n ;
    if (ata)
    {
	head = w + 4*n ; next = w + 5*n + 1 ;
	for (k = 0 ; k < n ; k++) set [post [k]] = k ;    /* invert post */
	for (i = 0 ; i < m ; i++)
	{
	    k = n ;		    /* k = least postordered column in row i */
	    for (p = ATp [i] ; p < ATp [i+1] ; p++) k = CS_MIN (k, set[ATi[p]]);
	    next [i] = head [k] ;   /* place row i in link list k */
	    head [k] = i ;
	}
    }
    for (k = 0 ; k < n ; k++) set [k] = k ;	/* each node in its own set */
    for (k = 0 ; k < n ; k++)
    {
	j = post [k] ;		/* j is the kth node in postordered etree */
	if (parent [j] != -1) c [parent [j]]-- ;	/* j is not a root */
	prevnbr [j] = k ;	/* flag node j to exclude self edges (j,j) */
	if (ata)
	{
	    for (ii = head [k] ; ii != -1 ; ii = next [ii])
	    {
		for (p = ATp [ii] ; p < ATp [ii+1] ; p++)
		{
		    if (prevnbr [ATi [p]] >= k) continue ;
		    cs_cedge (j, ATi [p], k, first, prevnbr, c, prevleaf, set) ;
		}
	    }
	}
	else
	{
	    for (p = ATp [j] ; p < ATp [j+1] ; p++)
	    {
		if (ATi [p] <= j) continue ;	/* only use triu(A)=tril(A')' */
		cs_cedge (j, ATi [p], k, first, prevnbr, c, prevleaf, set) ;
	    }
	}
	if (parent [j] != -1) set [j] = parent [j] ;
    }
    for (j = 0 ; j < n ; j++)
    {
	if (parent [j] != -1) c [parent [j]] += c [j] ;
    }
    return (cs_idone (c, AT, w, 1)) ;	    /* success: free w and return c */
}
