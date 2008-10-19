#include "cs.h"
/* post order a forest */
int *cs_post (int n, int *parent)
{
    int j, k = 0, *post, *w, *head, *next, *stack ;
    if (!parent) return (NULL) ;			/* check inputs */
    post = cs_malloc (n, sizeof (int)) ;		/* allocate result */
    w = cs_malloc (3*n, sizeof (int)) ;			/* 3*n workspace */
    head = w ; next = w + n ; stack = w + 2*n ;
    if (!w || !post) return (cs_idone (post, NULL, w, 0)) ;
    for (j = 0 ; j < n ; j++) head [j] = -1 ;		/* empty link lists */
    for (j = n-1 ; j >= 0 ; j--)	    /* traverse nodes in reverse order*/
    {
	if (parent [j] == -1) continue ;    /* j is a root */
	next [j] = head [parent [j]] ;	    /* add j to list of its parent */
	head [parent [j]] = j ;
    }
    for (j = 0 ; j < n ; j++)
    {
	if (parent [j] != -1) continue ;    /* skip j if it is not a root */
	k = cs_tdfs (j, k, head, next, post, stack) ;
    }
    return (cs_idone (post, NULL, w, 1)) ;  /* success; free w, return post */
}
