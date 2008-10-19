#include "cs.h"
/* compute nonzero pattern of L(k,:) */
static
int cs_ereach (cs *A, int j, int k, int *parent, int *s, int *w, double *x,
    int top)
{
    int i, p, len, *Ap = A->p, *Ai = A->i ;
    double *Ax = A->x ;
    for (p = Ap [j] ; p < Ap [j+1] ; p++)	/* get pattern of L(k,:) */
    {
	i = Ai [p] ;		    /* A(i,k) is nonzero */
	if (i > k) continue ;	    /* only use upper triangular part of A */
	x [i] = Ax [p] ;	    /* x(i) = A(i,k) */
	for (len = 0 ; w [i] != k ; i = parent [i]) /* traverse up etree */
	{
	    s [len++] = i ;	    /* L(k,i) is nonzero */
	    w [i] = k ;		    /* mark i as visited */
	}
	while (len > 0) s [--top] = s [--len] ; /* push path onto stack */
    }
    return (top) ;		    /* s [top..n-1] contains pattern of L(k,:)*/
}

/* L = chol (A, [Pinv parent cp]), Pinv is optional */
csn *cs_chol (cs *A, css *S)
{
    double d, lki, *Lx, *x ;
    int top, i, p, k, n, *Li, *Lp, *cp, *Pinv, *w, *s, *c, *parent ;
    cs *L, *C, *E ;
    csn *N ;
    if (!A || !S || !S->cp || !S->parent) return (NULL) ;   /* check inputs */
    n = A->n ;
    N = cs_calloc (1, sizeof (csn)) ;
    w = cs_malloc (3*n, sizeof (int)) ; s = w + n, c = w + 2*n ;
    x = cs_malloc (n, sizeof (double)) ;
    cp = S->cp ; Pinv = S->Pinv ; parent = S->parent ;
    C = Pinv ? cs_symperm (A, Pinv, 1) : A ;
    E = Pinv ? C : NULL ;
    if (!N || !w || !x || !C) return (cs_ndone (N, E, w, x, 0)) ;
    N->L = L = cs_spalloc (n, n, cp [n], 1, 0) ;
    if (!L) return (cs_ndone (N, E, w, x, 0)) ;
    Lp = L->p ; Li = L->i ; Lx = L->x ;
    for (k = 0 ; k < n ; k++)
    {
	Lp [k] = c [k] = cp [k] ;   /* column k of L starts here */
	x [k] = 0 ;		    /* x (0:k) is now zero */
	w [k] = k ;		    /* mark node k as visited */
	top = cs_ereach (C, k, k, parent, s, w, x, n) ;   /* find row k of L*/
	d = x [k] ;		    /* d = C(k,k) */
	x [k] = 0 ;		    /* clear workspace for k+1st iteration */
	for ( ; top < n ; top++)    /* solve L(0:k-1,0:k-1) * x = C(:,k) */
	{
	    i = s [top] ;	    /* s [top..n-1] is pattern of L(k,:) */
	    lki = x [i] / Lx [Lp [i]] ; /* L(k,i) = x (i) / L(i,i) */
	    x [i] = 0 ;		    /* clear workspace for k+1st iteration */
	    for (p = Lp [i] + 1 ; p < c [i] ; p++)
	    {
		x [Li [p]] -= Lx [p] * lki ;
	    }
	    d -= lki*lki ;	    /* d = d - L(k,i)*L(k,i) */
	    p = c [i]++ ;
	    Li [p] = k ;	    /* store L(k,i) in column i */
	    Lx [p] = lki ;
	}
	if (d <= 0) return (cs_ndone (N, E, w, x, 0)) ; /* not pos def */
	p = c [k]++ ;
	Li [p] = k ;		    /* store L(k,k) = sqrt (d) in column k */
	Lx [p] = sqrt (d) ;
    }
    Lp [n] = cp [n] ;		    /* finalize L */
    return (cs_ndone (N, E, w, x, 1)) ; /* success: free E,w,x; return N */
}
