#include "cs.h"
/* xi [n..2n-1] < mark on input, <= mark on output.  xi is size 3n */
static
int cs_reach (cs *L, cs *B, int k, int *xi, int mark, int *Pinv, double *x)
{
    int p, n = L->n, *Bp = B->p, *Bi = B->i, top = n, *w = xi + n ;
    double *Bx = B->x ;
    for (p = Bp [k] ; p < Bp [k+1] ; p++)
    {
	x [Bi [p]] = Bx [p] ;	    /* x(i) = B(i,k) */
	if (w [Bi [p]] != mark)	    /* B(i,k) is nonzero, start a dfs */
	    top = cs_dfs (Bi [p], L, top, xi, w, mark, Pinv, xi+2*n) ;
    }
    return (top) ;
}

/* solve Lx=b(:,k), leaving pattern in xi[top..n-1], values scattered in x. */
static
int cs_splsolve (cs *L, cs *B, int k, int *xi, int mark, double *x, int *Pinv)
{
    int j, jnew, p, px, top, n = L->n, *Lp = L->p, *Li = L->i ;
    double *Lx = L->x ;
    top = cs_reach (L, B, k, xi, mark, Pinv, x) ; /* x=B(:,k), X=Reach(B(:,k))*/
    for (px = top ; px < n ; px++)		/* for each j in X */
    {
	j = xi [px] ;				/* x(j) is nonzero */
	jnew = Pinv [j] ;			/* j is column jnew of L */
	if (jnew < 0) continue ;		/* column jnew is empty */
	for (p = Lp [jnew]+1 ; p < Lp [jnew+1] ; p++)
	    x [Li [p]] -= Lx [p] * x [j] ;	/* x(i) -= L(i,j) * x(j) */
    }
    return (top) ;				/* return top of stack */
}

/* [L,U,Pinv]=lu(A, [Q lnz unz]). lnz and unz can be guess */
csn *cs_lu (cs *A, css *S, double tol)
{
    cs *L, *U ;
    csn *N ;
    double pivot, a, *Lx, *Ux, t, *x ;
    int *Lp, *Li, *Up, *Ui, *Pinv, *xi, *Q, n, ipiv, k, top, p, i, col, lnz,unz;
    if (!A || !S) return (NULL) ;		    /* check inputs */
    n = A->n ;
    Q = S->Q ; lnz = S->lnz ; unz = S->unz ;
    x = cs_malloc (n, sizeof (double)) ;
    xi = cs_calloc (3*n, sizeof (int)) ;
    N = cs_calloc (1, sizeof (csn)) ;
    if (!x || !xi || !N) return (cs_ndone (N, NULL, xi, x, 0)) ;
    N->L = L = cs_spalloc (n, n, lnz, 1, 0) ;	    /* initial L and U */
    N->U = U = cs_spalloc (n, n, unz, 1, 0) ;
    N->Pinv = Pinv = cs_malloc (n, sizeof (int)) ;
    if (!L || !U || !Pinv) return (cs_ndone (N, NULL, xi, x, 0)) ;
    Lp = L->p ; Up = U->p ;
    for (i = 0 ; i < n ; i++) x [i] = 0 ;	    /* clear workspace */
    for (i = 0 ; i < n ; i++) Pinv [i] = -1 ;	    /* no rows pivotal yet */
    lnz = 0 ;
    unz = 0 ;
    for (k = 0 ; k < n ; k++)	    /* compute L(:,k) and U(:,k) */
    {
	Lp [k] = lnz ;		    /* L(:,k) starts here */
	Up [k] = unz ;		    /* U(:,k) starts here */
	if ((lnz + n > L->nzmax && !cs_sprealloc (L, 2*L->nzmax + n)) ||
	    (unz + n > U->nzmax && !cs_sprealloc (U, 2*U->nzmax + n)))
	{
	    return (cs_ndone (N, NULL, xi, x, 0)) ;
	}
	Li = L->i ; Lx = L->x ; Ui = U->i ; Ux = U->x ;
	col = Q ? (Q [k]) : k ;
	top = cs_splsolve (L, A, col, xi, k+1, x, Pinv) ; /* x = L\A(:,col) */
	ipiv = -1 ;		    /* find pivot row */
	a = -1 ;
	for (p = top ; p < n ; p++)
	{
	    i = xi [p] ;	    /* x(i) is nonzero */
	    if (Pinv [i] < 0)	    /* row i is not pivotal */
	    {
		if ((t = fabs (x [i])) > a)
		{
		    a = t ;	    /* largest pivot candidate so far */
		    ipiv = i ;
		}
	    }
	    else		    /* x(i) is the entry U(Pinv[i],k) */
	    {
		Ui [unz] = Pinv [i] ;
		Ux [unz++] = x [i] ;
	    }
	}
	if (ipiv == -1 || a <= 0) return (cs_ndone (N, NULL, xi, x, 0)) ;
	if (Pinv [col] < 0 && fabs (x [col]) >= a*tol) ipiv = col ;
	pivot = x [ipiv] ;	    /* the chosen pivot */
	Ui [unz] = k ;		    /* last entry in U(:,k) is U(k,k) */
	Ux [unz++] = pivot ;
	Pinv [ipiv] = k ;	    /* ipiv is the kth pivot row */
	Li [lnz] = ipiv ;	    /* first entry in L(:,k) is L(k,k) = 1 */
	Lx [lnz++] = 1 ;
	for (p = top ; p < n ; p++) /* L(k+1:n,k) = x / pivot */
	{
	    i = xi [p] ;
	    if (Pinv [i] < 0)	    /* x(i) is an entry in L(:,k) */
	    {
		Li [lnz] = i ;	    /* save unpermuted row in L */
		Lx [lnz++] = x [i] / pivot ;	/* scale pivot column */
	    }
	    x [i] = 0 ;		    /* x [0..n-1] = 0 for next k */
	}
    } 
    Lp [n] = lnz ;		    /* finalize L and U */
    Up [n] = unz ;
    Li = L->i ;			    /* fix row indices of L for final Pinv */
    for (p = 0 ; p < lnz ; p++) Li [p] = Pinv [Li [p]] ;
    cs_sprealloc (L, 0) ;	    /* remove extra space from L and U */
    cs_sprealloc (U, 0) ;
    return (cs_ndone (N, NULL, xi, x, 1)) ;	/* success */
}
