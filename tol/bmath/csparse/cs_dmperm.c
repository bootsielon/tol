#include "cs.h"
/* breadth-first search for coarse decomposition (C0,C1,R1 or R0,R3,C3) */
static int cs_bfs (cs *A, int n, int *wi, int *wj, int *queue, int *imatch,
    int *jmatch, int mark)
{
    int *Ap, *Ai, head = 0, tail = 0, j, i, p, j2 ;
    cs *C ;
    for (j = 0 ; j < n ; j++)		/* place all unmatched nodes in queue */
    {
	if (imatch [j] >= 0) continue ;	/* skip j if matched */
	wj [j] = 0 ;			/* j in set C0 (R0 if transpose) */
	queue [tail++] = j ;		/* place unmatched col j in queue */
    }
    if (tail == 0) return (1) ;		/* quick return if no unmatched nodes */
    C = (mark == 1) ? A : cs_transpose (A, 0) ;	/* bfs of A' to find R0,R3,C3 */
    if (!C) return (0) ;
    Ap = C->p ; Ai = C->i ;
    while (head < tail)			/* while queue is not empty */
    {
	j = queue [head++] ;		/* get the head of the queue */
	for (p = Ap [j] ; p < Ap [j+1] ; p++)
	{
	    i = Ai [p] ;
	    if (wi [i] >= 0) continue ;	/* skip if i is marked */
	    wi [i] = mark ;		/* i in set R1 (C3 if transpose) */
	    j2 = jmatch [i] ;		/* traverse alternating path to j2 */
	    if (wj [j2] >= 0) continue ;/* skip j2 if it is marked */
	    wj [j2] = mark ;		/* j2 in set C1 (R3 if transpose) */
	    queue [tail++] = j2 ;	/* add j2 to queue */
	}
    }
    if (mark != 1) cs_spfree (C) ;	/* free A' if it was created */
    return (1) ;
}

/* collect matched rows and columns into P and Q */
static void cs_matched (int m, int *wi, int *jmatch, int *P, int *Q, int *cc,
	int *rr, int set, int mark)
{
    int kc = cc [set], i ;
    int kr = rr [set-1] ;
    for (i = 0 ; i < m ; i++)
    {
	if (wi [i] != mark) continue ;	    /* skip if i is not in R set */
	P [kr++] = i ;
	Q [kc++] = jmatch [i] ;
    }
    cc [set+1] = kc ;
    rr [set] = kr ;
}

/* collect unmatched rows into the permutation vector P */
static void cs_unmatched (int m, int *wi, int *P, int *rr, int set)
{
    int i, kr = rr [set] ;
    for (i = 0 ; i < m ; i++) if (wi [i] == 0) P [kr++] = i ;
    rr [set+1] = kr ;
}

/* return 1 if row i is in R2 */
static int cs_rprune (int i, int j, double aij, void *other)
{
    int *rr = (int *) other ;
    return (i >= rr [1] && i < rr [2]) ;
}

/* Given A, find coarse dmperm */
csd *cs_dmperm (cs *A)
{
    int m, n, i, j, k, p, cnz, nc, *jmatch, *imatch, *wi, *wj, *Pinv, *Cp, *Ci,
	*Ps, *Rs, nb1, nb2, *P, *Q, *cc, *rr, *R, *S, ok ;
    cs *C ;
    csd *D, *scc ;
    if (!A) return (NULL) ;			/* check inputs */
    m = A->m ; n = A->n ;
    D = cs_dalloc (m, n) ;			/* allocate result */
    if (!D) return (NULL) ;
    P = D->P ; Q = D->Q ; R = D->R ; S = D->S ; cc = D->cc ; rr = D->rr ;
    jmatch = cs_maxtrans (A) ;			/* max transversal */
    imatch = jmatch + m ;			/* imatch = inverse of jmatch */
    if (!jmatch) return (cs_ddone (D, NULL, jmatch, 0)) ;
    wi = R ; wj = S ;				/* use R and S as workspace */
    for (j = 0 ; j < n ; j++) wj [j] = -1 ;	/* unmark all cols for bfs */
    for (i = 0 ; i < m ; i++) wi [i] = -1 ;	/* unmark all rows for bfs */
    cs_bfs (A, n, wi, wj, Q, imatch, jmatch, 1) ;	/* find C0, C1, R1 */
    ok = cs_bfs (A, m, wj, wi, P, jmatch, imatch, 3) ;	/* find R0, R3, C3 */
    if (!ok) return (cs_ddone (D, NULL, jmatch, 0)) ;
    cs_unmatched (n, wj, Q, cc, 0) ;			/* unmatched set C0 */
    cs_matched (m, wi, jmatch, P, Q, cc, rr, 1, 1) ;	/* set R1 and C1 */
    cs_matched (m, wi, jmatch, P, Q, cc, rr, 2, -1) ;	/* set R2 and C2 */
    cs_matched (m, wi, jmatch, P, Q, cc, rr, 3, 3) ;	/* set R3 and C3 */
    cs_unmatched (m, wi, P, rr, 3) ;			/* unmatched set R0 */
    cs_free (jmatch) ;
    Pinv = cs_pinv (P, m) ;	    /* Pinv=P' */
    if (!Pinv) return (cs_ddone (D, NULL, NULL, 0)) ;
    C = cs_permute (A, Pinv, Q, 0) ;/* C=A(P,Q) (it will hold A(R2,C2)) */
    cs_free (Pinv) ;
    if (!C) return (cs_ddone (D, NULL, NULL, 0)) ;
    Cp = C->p ; Ci = C->i ;
    nc = cc [3] - cc [2] ;	    /* delete cols C0, C1, and C3 from C */
    if (cc [2] > 0) for (j = cc [2] ; j <= cc [3] ; j++) Cp [j-cc[2]] = Cp [j] ;
    C->n = nc ;
    if (rr [2] - rr [1] < m)	    /* delete rows R0, R1, and R3 from C */
    {
	cs_fkeep (C, cs_rprune, rr) ;
	cnz = Cp [nc] ;
	if (rr [1] > 0) for (p = 0 ; p < cnz ; p++) Ci [p] -= rr [1] ;
    }
    C->m = nc ;
    scc = cs_scc (C) ;		    /* find strongly-connected components of C*/
    if (!scc) return (cs_ddone (D, C, NULL, 0)) ;
    Ps = scc->P ;		    /* C(Ps,Ps) is the permuted matrix */
    Rs = scc->R ;		    /* kth block is Rs[k]..Rs[k+1]-1 */
    nb1 = scc->nb  ;		    /* # of blocks of A(*/
    for (k = 0 ; k < nc ; k++) wj [k] = Q [Ps [k] + cc [2]] ;	/* combine */
    for (k = 0 ; k < nc ; k++) Q [k + cc [2]] = wj [k] ;
    for (k = 0 ; k < nc ; k++) wi [k] = P [Ps [k] + rr [1]] ;
    for (k = 0 ; k < nc ; k++) P [k + rr [1]] = wi [k] ;
    nb2 = 0 ;			    /* create the fine block partitions */
    R [0] = 0 ;
    S [0] = 0 ;
    if (cc [2] > 0)
    {
	R [1] = rr [1] ;	    /* leading coarse block A (R1, [C0 C1]) */
	S [1] = cc [2] ;
	nb2++ ;
    }
    for (k = 0 ; k < nb1 ; k++)	    /* coarse block A (R2,C2) */
    {
	R [nb2] = Rs [k] + rr [1] ; /* A (R2,C2) splits into nb1 fine blocks */
	S [nb2] = Rs [k] + cc [2] ;
	nb2++ ;
    }
    if (rr [2] < m)
    {
	R [nb2] = rr [2] ;	    /* trailing coarse block A ([R3 R0], C3) */
	S [nb2] = cc [3] ;
	nb2++ ;
    }
    R [nb2] = m ;
    S [nb2] = n ;
    D->nb = nb2 ;
    cs_dfree (scc) ;
    return (cs_ddone (D, C, NULL, 1)) ;
}
