#include "cs.h"
/* compute vnz, Pinv, leftmost, m2 from A and parent */
static int *cs_vcount (cs *A, int *parent, int *m2, int *vnz)
{
    int i, k, p, pa, n = A->n, m = A->m, *Ap = A->p, *Ai = A->i ;
    int *Pinv = cs_malloc (2*m+n, sizeof (int)), *leftmost = Pinv + m + n ;
    int *w = cs_malloc (m+3*n, sizeof (int)) ;
    int *next = w, *head = w + m, *tail = w + m + n, *nque = w + m + 2*n ;
    if (!Pinv || !w) return (cs_idone (Pinv, NULL, w, 0)) ;
    for (k = 0 ; k < n ; k++) head [k] = -1 ;	/* queue k is empty */
    for (k = 0 ; k < n ; k++) tail [k] = -1 ;
    for (k = 0 ; k < n ; k++) nque [k] = 0 ;
    for (i = 0 ; i < m ; i++) leftmost [i] = -1 ;
    for (k = n-1 ; k >= 0 ; k--)
    {
	for (p = Ap [k] ; p < Ap [k+1] ; p++)
	{
	    leftmost [Ai [p]] = k ;	    /* leftmost[i] = min(find(A(i,:)))*/
	}
    }  
    for (i = m-1 ; i >= 0 ; i--)	    /* scan rows in reverse order */
    {
	Pinv [i] = -1 ;			    /* row i is not yet ordered */
	k = leftmost [i] ;
	if (k == -1) continue ;		    /* row i is empty */
	if (nque [k]++ == 0) tail [k] = i ; /* first row in queue k */
	next [i] = head [k] ;		    /* put i at head of queue k */
	head [k] = i ;
    } 
    (*vnz) = 0 ;
    (*m2) = m ;
    for (k = 0 ; k < n ; k++)		    /* find row permutation and nnz(V)*/
    {
	i = head [k] ;			    /* remove row i from queue k */
	(*vnz)++ ;			    /* count V(k,k) as nonzero */
	if (i < 0) i = (*m2)++ ;	    /* add a fictitious row */
	Pinv [i] = k ;			    /* associate row i with V(:,k) */
	if (--nque [k] <= 0) continue ;	    /* skip if V(k+1:m,k) is empty */
	(*vnz) += nque [k] ;		    /* nque [k] = nnz (V(k+1:m,k)) */
	if ((pa = parent [k]) != -1)	    /* move all rows to parent of k */
	{
	    if (nque [pa] == 0) tail [pa] = tail [k] ;
	    next [tail [k]] = head [pa] ;
	    head [pa] = next [i] ;
	    nque [pa] += nque [k] ;
	}
    }
    for (i = 0 ; i < m ; i++) if (Pinv [i] < 0) Pinv [i] = k++ ;
    return (cs_idone (Pinv, NULL, w, 1)) ;
}

/* symbolic analysis for QR or LU */
css *cs_sqr (cs *A, int order, int qr)
{
    int n, k, ok = 1, *post ;
    css *S ;
    if (!A) return (NULL) ;		    /* check inputs */
    n = A->n ;
    S = cs_calloc (1, sizeof (css)) ;	    /* allocate symbolic analysis */
    if (!S) return (NULL) ;		    /* out of memory */
    S->Q = cs_amd (A, order) ;		    /* fill-reducing ordering */
    if (order >= 0 && !S->Q) return (cs_sfree (S)) ;
    if (qr)				    /* QR symbolic analysis */
    {
	cs *C = (order >= 0) ? cs_permute (A, NULL, S->Q, 0) : A ; /* C=A(:,Q)*/
	S->parent = cs_etree (C, 1) ;	    /* etree of C'*C */
	post = cs_post (n, S->parent) ;
	S->cp = cs_counts (C, S->parent, post, 1) ;  /* col counts chol(C'*C) */
	cs_free (post) ;
	ok = C && S->parent && S->cp ;
	if (ok) S->Pinv = cs_vcount (C, S->parent, &(S->m2), &(S->lnz)) ;
	ok = ok && S->Pinv ;
	if (ok) for (S->unz = 0, k = 0 ; k < n ; k++) S->unz += S->cp [k] ;
	if (order >= 0) cs_spfree (C) ;
    }
    else
    {
	S->unz = 4*(A->p [n]) + n ;	    /* for LU factorization only, */
	S->lnz = S->unz ;		    /* guess nnz(L) and nnz(U) */
    }
    return (ok ? S : cs_sfree (S)) ;
}
