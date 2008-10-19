#include "cs.h"
/* sparse QR factorization [V,beta,p,R] = qr (A) */
csn *cs_qr (cs *A, css *S)
{
    double *Rx, *Vx, *Ax, *Beta, *x ;
    int i, k, p, m, n, vnz, p1, top, m2, len, col, rnz, *s, *leftmost, *Ap,
	*Ai, *parent, *Rp, *Ri, *Vp, *Vi, *w, *Pinv, *Q ;
    cs *R, *V ;
    csn *N ;
    if (!A || !S || !S->parent || !S->Pinv) return (NULL) ; /* check inputs */
    m = A->m ; n = A->n ; Ap = A->p ; Ai = A->i ; Ax = A->x ;
    Q = S->Q ; parent = S->parent ; Pinv = S->Pinv ; m2 = S->m2 ;
    vnz = S->lnz ; rnz = S->unz ;
    leftmost = Pinv + m + n ;
    w = cs_malloc (m2+n, sizeof (int)) ;
    x = cs_malloc (m2, sizeof (double)) ;
    N = cs_calloc (1, sizeof (csn)) ;
    if (!w || !x || !N) return (cs_ndone (N, NULL, w, x, 0)) ;
    s = w + m2 ;				/* size n */
    for (k = 0 ; k < m2 ; k++) x [k] = 0 ;	/* clear workspace x */
    N->L = V = cs_spalloc (m2, n, vnz, 1, 0) ;	/* allocate V */
    N->U = R = cs_spalloc (m2, n, rnz, 1, 0) ;	/* allocate R, m2-by-n */
    N->B = Beta = cs_malloc (n, sizeof (double)) ;
    if (!R || !V || !Beta) return (cs_ndone (N, NULL, w, x, 0)) ;
    Rp = R->p ; Ri = R->i ; Rx = R->x ;
    Vp = V->p ; Vi = V->i ; Vx = V->x ;
    for (i = 0 ; i < m2 ; i++) w [i] = -1 ;	/* clear w, to mark nodes */
    rnz = 0 ; vnz = 0 ;
    for (k = 0 ; k < n ; k++)		    /* compute V and R */
    {
	Rp [k] = rnz ;			    /* R(:,k) starts here */
	Vp [k] = p1 = vnz ;		    /* V(:,k) starts here */
	w [k] = k ;			    /* add V(k,k) to pattern of V */
	Vi [vnz++] = k ;
	top = n ;
	col = Q ? Q [k] : k ;
	for (p = Ap [col] ; p < Ap [col+1] ; p++)   /* find R(:,k) pattern */
	{
	    i = leftmost [Ai [p]] ;	    /* i = min(find(A(i,Q))) */
	    for (len = 0 ; w [i] != k ; i = parent [i])	/* traverse up to k */
	    {
		s [len++] = i ;
		w [i] = k ;
	    }
	    while (len > 0) s [--top] = s [--len] ; /* push path on stack */
	    i = Pinv [Ai [p]] ;		    /* i = permuted row of A(:,col) */
	    x [i] = Ax [p] ;		    /* x (i) = A(.,col) */
	    if (i > k && w [i] < k)	    /* pattern of V(:,k) = x (k+1:m) */
	    {
		Vi [vnz++] = i ;	    /* add i to pattern of V(:,k) */
		w [i] = k ;
	    }
	}
	for (p = top ; p < n ; p++) /* for each i in pattern of R(:,k) */
	{
	    i = s [p] ;			    /* R(i,k) is nonzero */
	    cs_happly (V, i, Beta [i], x) ; /* apply (V(i),Beta(i)) to x */
	    Ri [rnz] = i ;		    /* R(i,k) = x(i) */
	    Rx [rnz++] = x [i] ;
	    x [i] = 0 ;
	    if (parent [i] == k) vnz = cs_scatter (V, i, 0, w, NULL, k, V, vnz);
	}
	for (p = p1 ; p < vnz ; p++)	    /* gather V(:,k) = x */
	{
	    Vx [p] = x [Vi [p]] ;
	    x [Vi [p]] = 0 ;
	}
	Ri [rnz] = k ;			   /* R(k,k) = norm (x) */
	Rx [rnz++] = cs_house (Vx+p1, Beta+k, vnz-p1) ;	/* [v,beta]=house(x) */
    }
    Rp [n] = rnz ;			    /* finalize R */
    Vp [n] = vnz ;			    /* finalize V */
    return (cs_ndone (N, NULL, w, x, 1)) ;  /* success */
}
