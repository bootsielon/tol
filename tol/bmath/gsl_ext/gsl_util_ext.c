/* @(#) gsl_util_ext.c
 */

#include "gsl_ext.h"

void gsl_matrix_table(FILE * f, gsl_matrix * m, char sep)
{
  size_t i, j;

  for (i = 0; i < m->size1; i++) {
    for (j = 0; j < m->size2; j++) {
      fprintf(f, "%12.6g", gsl_matrix_get(m, i, j));
      if (j<m->size2-1)
        fprintf(f, "%c", sep);
    }
    fprintf(f, "\n");
  }
}

void gsl_vector_table(FILE * f, gsl_vector * v, char sep)
{
  size_t i;

  for (i = 0; i < v->size; i++) {
    fprintf(f, " %6.6g", gsl_vector_get(v, i));
    if (i<v->size)
      fprintf(f, "%c", sep);
  }
  fprintf(f, "\n");
}
