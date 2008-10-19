#include "cs.h"
/* load a triplet matrix from a file */
cs *cs_load (FILE *f)
{
    int i, j ;
    double x ;
    cs *T ;
    if (!f) return (NULL) ;
    T = cs_spalloc (0, 0, 1, 1, 1) ;
    while (fscanf (f, "%d %d %lg\n", &i, &j, &x) == 3)
    {
	if (!cs_entry (T, i, j, x)) return (cs_spfree (T)) ;
    }
    return (T) ;
}
