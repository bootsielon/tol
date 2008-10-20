/* matrix.h
 *
 * Copyright (C) 2005 Jorge Suit Perez Ronda
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/** @file matrix.h
 Full Matrix Storage.

 Full storage is the following obvious scheme: a matrix A is stored in a
 two-dimensional array a, with the matrix element aij stored in the array
 element a(i,j). If a matrix is triangular (upper or lower, as specified by
 the argument uplo), only the elements of the relevant triangle are stored;
 the remaining elements of the array need not be set. Routines that handle
 symmetric or Hermitian matrices allow for either the upper or lower triangle
 of the matrix to be stored in the corresponding elements of the array:

 uplo ='U' 	aij is stored in a(i,j) for i < j;
                other elements of a need not be set.
 uplo ='L' 	aij is stored in a(i,j) for j < i;
                other elements of a need not be set.
 */

#ifndef __GBLA_MATRIX_H__
#define __GBLA_MATRIX_H__

#include <gbla/vector.h>

__GBLA_BEGIN_DECLS

/** Full Matrix Storage.

 Full storage matrices are defined by a gbla_matrix_t structure which
 describes a generalized slice of a block. Like a vector it represents a set
 of elements in an area of memory, but uses two indices instead of one.
 */

typedef struct 
{
  /** Rows. Number of rows in the matrix. The range of valid row indices runs
   * from 0 to size1-1 */
  int size1;
  /** Columns. Number of columns in the matrix. The range of valid column
   * indices runs from 0 to size2-1 */
  int size2;
  /** Leading dimension. The physical column dimension, specifies the size of
   * a column of the matrix as laid out in memory.*/
  int lda;
  /** matrix memory. A pointer to the memory where the elements of the matrix
   * are stored. The pointer data gives the location of the first element of
   * the matrix in memory. */
  double * data;
  /** block memory. A pointer to the block owned by the matrix block. The
   * pointer block stores the location of the memory block in which the
   * elements of the matrix are located (if any).*/
  gbla_block_t block;
  /** owner flag. If the matrix is only a slice of a block owned by another
   * object then the owner field is zero and any underlying block will not be
   * freed.*/
  int flags;
} gbla_matrix_t;

/* Full Storage Initialization */

/** Matrix initialization.

 This function creates/initialize a matrix of size n1 rows by n2 columns,
 returning a pointer to a newly initialized matrix struct. A new block is
 allocated for the elements of the matrix, and stored in the block component
 of the matrix struct. The block is "owned" by the matrix, and will be
 deallocated when the matrix is deallocated.

 @param[in] n1 number of rows of the matrix.
 @param[in] n2 number of columns of the matrix.
 
 @param[in,out] m memory address to initialize. If m is NULL a new matrix is
                created from the heap, then initialized.

 @return the address of the initialized matrix.
 */
GBLA_API gbla_matrix_t * gbla_matrix_init (int n1, int n2, gbla_matrix_t *m);

/** Matrix initialization, cells initialized to zero.

 This function creates/initialize a matrix of size n1 rows by n2 columns,
 returning a pointer to a newly initialized matrix struct. A new block is
 allocated for the elements of the matrix, and stored in the block component
 of the matrix struct. The block is "owned" by the matrix, and will be
 deallocated when the matrix is deallocated. The elements of the matrix are
 initialized to zero.

 @param[in] n1 number of rows of the matrix.
 @param[in] n2 number of columns of the matrix.
 
 @param[in,out] m memory address to initialize. If m is NULL a new matrix is
                created from the heap, then initialized.
                
 @return the address of the initialized matrix.
 */
GBLA_API gbla_matrix_t * gbla_matrix_cinit (int n1, int n2, gbla_matrix_t *m);

/** Matrix release.
 
 This function frees a previously allocated matrix m. If the matrix was
 created using gbla_matrix_init/cinit then the block underlying the matrix
 will also be deallocated. If the matrix has being created from another object
 then the internal bock memory is still owned by that object and will not be
 deallocated.

 @param[in,out] m the matrix to release.
 */
GBLA_API void gbla_matrix_free (gbla_matrix_t * m);

/* Full Storage Views */

/** Submatrix View.
 
 This function return a matrix view of a submatrix of the matrix m. The
 upper-left element of the submatrix is the element (i,j) of the original
 matrix. The submatrix has n1 rows and n2 columns. The physical number of rows
 in memory given by lda is unchanged. The new matrix view is only a view of
 the block underlying the existing matrix, m. The block containing the
 elements of m is not owned by the new matrix view. Of course, the original
 matrix should not be freed while the view is still in use.

 @param[in] m matrix to take the view from.
 @param[in] i row of the upper-left  element of the submatrix.
 @param[in] j column of the upper-left  element of the submatrix.
 @param[in] n1 number of rows of the submatrix.
 @param[in] n2 number of columns of the submatrix.
 @return the matrix structure for the view.
 */
GBLA_API gbla_matrix_t 
gbla_matrix_submatrix (gbla_matrix_t* m, 
                       int i, int j, 
                       int n1, int n2);

/** Vector view from matrix row.

 This function return a vector view of the i-th row of the matrix m. The data
 pointer of the new vector is set to null if i is out of range.

 @param[in] m matrix to take the row view from.
 @param[in] i index of the row.
 @return the vector structure for the row view.
 */
GBLA_API gbla_vector_t 
gbla_matrix_row (gbla_matrix_t* m, int i);

/** Vector view from matrix column.

 This function return a vector view of the j-th column of the matrix m. The
 data pointer of the new vector is set to null if j is out of range.

 @param[in] m matrix to take the row view from.
 @param[in] j index of the column.
 @return the vector structure for the column view.
 */
GBLA_API gbla_vector_t 
gbla_matrix_column (gbla_matrix_t* m, int j);

/** Vector view from the matrix diagonal.

 This function returns a vector view of the diagonal of the matrix m. The
 matrix m is not required to be square. For a rectangular matrix the length of
 the diagonal is the same as the smaller dimension of the matrix.
 
 @param[in] m matrix to take the diagonal view from.
 @return the vector structure for the diagonal view.
 */
GBLA_API gbla_vector_t
gbla_matrix_diagonal (gbla_matrix_t* m);

/** Vector view from a matrix subdiagonal.

 This function returns a vector view of the k-th subdiagonal of the matrix
 m. The matrix m is not required to be square. The diagonal of the matrix
 corresponds to k = 0.

 @param[in] m matrix to take the diagonal view from.
 @param[in] k the index of the subdiagonal.
 @return the vector structure for the subdiagonal view.
 */
GBLA_API gbla_vector_t 
gbla_matrix_subdiagonal (gbla_matrix_t* m, int k);

/** Vector view from a matrix superdiagonal.

 This function returns a vector view of the k-th superdiagonal of the matrix
 m. The matrix m is not required to be square. The diagonal of the matrix
 corresponds to k = 0.

 @param[in] m matrix to take the diagonal view from.
 @param[in] k the index of the superdiagonal.
 @return the vector structure for the superdiagonal view.
 */
GBLA_API gbla_vector_t 
gbla_matrix_superdiagonal (gbla_matrix_t* m, int k);

/** Matrix from array.

 This function return a matrix view of the array base. The matrix has n1 rows
 and n2 columns. The physical number of columns in memory is also given by
 n2. Mathematically, the (i,j)-th element of the new matrix is given by,

 m(i,j) = base[i+j*n1]

 where the index i runs from 0 to n1-1 and the index j runs from 0 to n2-1.

 The new matrix is only a view of the array base. When the view goes out of
 scope the original array base will continue to exist. The original memory can
 only be deallocated by freeing the original array. Of course, the original
 array should not be deallocated while the view is still in use.

 @param[in] base physical memory of data
 @param[in] n1 number of rows of the matrix view.
 @param[in] n2 number of columns of the matrix view.
 @return the matrix structure for the view. 
 */
GBLA_API gbla_matrix_t
gbla_matrix_view_array (double * base,
                        int n1, 
                        int n2);

/** Matrix from array with lda.

 This functions return a matrix view of the array base with a physical number
 of rows lda which may differ from the corresponding dimension of the
 matrix. The matrix has n1 rows and n2 columns, and the physical number of
 rows in memory is given by lda. Mathematically, the (i,j)-th element of
 the new matrix is given by,

 m(i,j) = base[i + j*lda]

 where the index i runs from 0 to n1-1 and the index j runs from 0 to n2-1.

 The new matrix is only a view of the array base. When the view goes out of
 scope the original array base will continue to exist. The original memory can
 only be deallocated by freeing the original array. Of course, the original
 array should not be deallocated while the view is still in use.

 @param[in] base physical memory of data
 @param[in] n1 number of rows of the matrix view.
 @param[in] n2 number of columns of the matrix view.
 @param[in] lda physical number of rows in memory.
 @return the matrix structure for the view.
 */
GBLA_API gbla_matrix_t
gbla_matrix_view_array_with_lda (double * base, 
                                 int n1, 
                                 int n2,
                                 int lda);

/** Matrix view from vector.

 These functions return a matrix view of the vector v. The matrix has n1 rows
 and n2 columns. The vector must have unit stride. The physical number of
 rows in memory is also given by n1. Mathematically, the (i,j)-th element
 of the new matrix is given by,

 m(i,j) = v->data[i+j*n1]

 where the index i runs from 0 to n1-1 and the index j runs from 0 to n2-1.

 The new matrix is only a view of the vector v. When the view goes out of
 scope the original vector v will continue to exist. The original memory can
 only be deallocated by freeing the original vector. Of course, the original
 vector should not be deallocated while the view is still in use.

 @param[in] v  vector to take the view from.
 @param[in] n1 number of rows of the matrix view.
 @param[in] n2 number of columns of the matrix view.
 @return the matrix structure for the view. 
 */
GBLA_API gbla_matrix_t
gbla_matrix_view_vector (gbla_vector_t * v,
                         int n1, 
                         int n2);

/** Matrix view from vector with lda.

 These functions return a matrix view of the vector v with a physical number
 of rows lda which may differ from the corresponding matrix dimension. The
 vector must have unit stride. The matrix has n1 rows and n2 columns, and the
 physical number of rows in memory is given by lda. Mathematically, the
 (i,j)-th element of the new matrix is given by,

 m(i,j) = v->data[i + j*tda]

 where the index i runs from 0 to n1-1 and the index j runs from 0 to n2-1.

 The new matrix is only a view of the vector v. When the view goes out of
 scope the original vector v will continue to exist. The original memory can
 only be deallocated by freeing the original vector. Of course, the original
 vector should not be deallocated while the view is still in use.

 @param[in] v  vector to take the view from.
 @param[in] n1 number of rows of the matrix view.
 @param[in] n2 number of columns of the matrix view.
 @param[in] lda physical number of rows in memory.
 @return the matrix structure for the view. 
 */
GBLA_API 
gbla_matrix_t gbla_matrix_view_vector_with_lda (gbla_vector_t * v, int n1,
                                                int n2, int lda);

/* Full Storage Operations */

/** Get element value.

 This macro returns the (i,j)-th element of a matrix m. No dimension checking
 is done so if i or j lie outside the allowed range of 0 to n1-1 and 0 to n2-1
 then you will obtain unexepected results.

 @param[in] m the matrix.
 @param[in] i element row index.
 @param[in] j element row index.
 @return the element value m(i,j).
 */
#define gbla_matrix_get(m, i, j) ((m)->data[(i)+(j)*m->lda])

/** Set element value.

 This macro change the (i,j)-th element of a matrix m to the value x. No
 dimension checking is done so if i or j lie outside the allowed range of 0 to
 n1-1 and 0 to n2-1 then you will obtain unexepected results.

 @param[out] m the matrix.
 @param[in] i element row index.
 @param[in] j element row index.
 @param[in] x the new element value.
 */
#define gbla_matrix_set(m, i, j, x) ((m)->data[(i)+(j)*(m)->lda]=x)

/** Get element address.

 This macro returns the address of (i,j)-th element of a matrix m. No
 dimension checking is done so if i or j lie outside the allowed range of 0 to
 n1-1 and 0 to n2-1 then you will obtain unexepected results when using this
 address.

 @param[in] m the matrix.
 @param[in] i element row index.
 @param[in] j element row index.
 @return the address of the element m(i,j).
 */
#define gbla_matrix_ptr(m, i, j) ((m)->data+(i)+(j)*(m)->lda)

/** Set to zero.

 This function sets all the elements of the matrix m to zero.

 @param[in] m the matrix.
 */
GBLA_API void gbla_matrix_set_zero (gbla_matrix_t * m);

/** Matrix set identity.

 This function sets the elements of the matrix m to the corresponding elements
 of the identity matrix, \f$m(i,j) = \delta(i,j)\f$, i.e. a unit diagonal with
 all off-diagonal elements zero. This applies to both square and rectangular
 matrices.

 @param[in] m the matrix.
 */
GBLA_API void gbla_matrix_set_identity (gbla_matrix_t * m);

/** Matrix set all.
 
 This function sets all the elements of the matrix m to the value x.

 @param[in,out] m the matrix.
 @param[in] x the value to set into m.
 */
GBLA_API void gbla_matrix_set_all (gbla_matrix_t * m, double x);

/** Matrix random initialization

 This function sets all the elements of a matrix, given as argument, to a
 random value from the interval @f$[0,1]@f$. The random values are generated
 using the POSIX function rand().
 
 @param[in,out] m the matrix to initialize.
 */
GBLA_API void gbla_matrix_set_all_random (gbla_matrix_t * m);

/** Copying matrices.
 
 This function copies the elements of the matrix src into the matrix dest. The
 two matrices must have the same dimensions.

 @param[out] dest matrix where to copy elements.
 @param[in] src matrix where to read elements.
 @return GBLA_SUCCESS (no dimension check is done).
 */
GBLA_API int gbla_matrix_copy(gbla_matrix_t * dest, const gbla_matrix_t * src);

/** Swaping matrices.
 
 This function exchanges the elements of the matrices m1 and m2 by
 copying. The two matrices must have the same dimension.

 @param[in,out] m1
 @param[in,out] m2
 @return GBLA_SUCCESS (no dimension check is done).
 */
GBLA_API int gbla_matrix_swap(gbla_matrix_t * m1, gbla_matrix_t * m2);

/** Swapping rows.

 This function exchanges the i-th and j-th rows of the matrix m in-place.

 @param[in,out] m matrix where to swap rows.
 @param[in] i index of i-th row.
 @param[in] j index of j-th row.
 @return GBLA_SUCCESS (no dimension check is done).
 */
GBLA_API int gbla_matrix_swap_rows(gbla_matrix_t * m, int i, int j);

/** Swapping columns.

 This function exchanges the i-th and j-th column of the matrix m in-place.

 @param[in,out] m matrix where to swap columns.
 @param[in] i index of i-th column.
 @param[in] j index of j-th column.
 @return GBLA_SUCCESS (no dimension check is done).
 */
GBLA_API int gbla_matrix_swap_columns(gbla_matrix_t * m, int i, int j);

/** Swapping row with column.

 This function exchanges the i-th row and j-th column of the matrix m
 in-place. The matrix must be square for this operation to be possible.
 
 @param[in,out] m matrix where to swap.
 @param[in] i index of i-th row.
 @param[in] j index of j-th column.
 @return GBLA_SUCCESS (no dimension check is done).
*/
GBLA_API int gbla_matrix_swap_rowcol(gbla_matrix_t * m, int i, int j);

/** Matrix transpose in-place.

 This function replaces the matrix m by its transpose by copying the elements
 of the matrix in-place. The matrix must be square for this operation to be
 possible.

 @param[in,out] m matrix to transpose.
 @return GBLA_SUCCESS (no dimension check is done). 
 */
GBLA_API int gbla_matrix_transpose (gbla_matrix_t * m);

/** Matrix transpose.

 This function makes the matrix dest the transpose of the matrix src by
 copying the elements of src into dest. This function works for all matrices
 provided that the dimensions of the matrix dest match the transposed
 dimensions of the matrix src.

 @param[out] dest matrix where to copy transposed elements.
 @param[in] src matrix where to read elements.
 @return GBLA_SUCCESS (no dimension check is done). 
 */
GBLA_API 
int gbla_matrix_transpose_copy (gbla_matrix_t * dest, gbla_matrix_t * src);

/** Matrix maximun

 This function returns the maximum value in the matrix m.

 @param[in] m the matrix.
 @return the maximun element value in m.
 */
GBLA_API double gbla_matrix_max (gbla_matrix_t * m);

/** Matrix minimun

 This function returns the minimun value in the matrix m.

 @param[in] m the matrix.
 @return the minimun element value in m.
 */
GBLA_API double gbla_matrix_min (gbla_matrix_t * m);

/** Matrix min-max.

 This function returns the minimum and maximum values in the matrix m, storing
 them in min_out and max_out.
 
 @param[in] m the matrix.
 @param[out] min_out minimun element value in m.
 @param[out] max_out maximun element value in m.
 */
GBLA_API 
void gbla_matrix_minmax (gbla_matrix_t * m, double * min_out, double * max_out);

/** Matrix maximun index value.

 This function returns the indices of the maximum value in the matrix m,
 storing them in imax and jmax. When there are several equal maximum elements
 then the first element found is returned, searching in column-major order.
 
 @param[in] m the matrix.
 @param[out] imax row index with the maximun value.
 @param[out] jmax column index with the maximun value.
 */
GBLA_API void gbla_matrix_max_index (gbla_matrix_t * m, int * imax, int *jmax);

/** Matrix minimun index value.

 This function returns the indices of the minimun value in the matrix m,
 storing them in imin and jmin. When there are several equal minimun elements
 then the first element found is returned, searching in column-major order.
 
 @param[in] m the matrix.
 @param[out] imin row index with the minimun value.
 @param[out] jmin column index with the minimun value.
 */
GBLA_API void gbla_matrix_min_index (gbla_matrix_t * m, int * imin, int *jmin);

/** Matrix minimun & maximun index value.

 This function returns the indices of the minimum and maximum values in the
 matrix m, storing them in (imin,jmin) and (imax,jmax). When there are several
 equal minimum or maximum elements then the first elements found are returned,
 searching in column-major order.
 
 @param[in] m the matrix.
 @param[out] imin row index with the minimun value.
 @param[out] jmin column index with the minimun value.
 @param[out] imax row index with the maximun value.
 @param[out] jmax column index with the maximun value.
 */
GBLA_API void gbla_matrix_minmax_index (gbla_matrix_t * m,
					int * imin, int * jmin,
					int * imax, int * jmax);

/** Matrix test for null.

 This function returns 1 if all the elements of the matrix m are zero, and 0
 otherwise.
 
 @param[in] m
 @return
        - 1 if all the elements are null.
        - 0 otherwise.
 */
GBLA_API int gbla_matrix_isnull (gbla_matrix_t * m);

/** Matrix test for null with epsilon

 This function returns 1 if all the elements of the matrix @f$m@f$ are in a
 neighborhood of zero. The test is defined for @f$\epsilon>0@f$ as
 @f$|m_{ij}|<=\epsilon@f$ and 0 otherwise.

 @param[in] m the matrix to test.

 @param[in] eps is the value of @f$\epsilon>0@f$ which define de neighborhood
            @f$[-\epsilon,+\epsilon]@f$.
 
 @return
        - 1 if @f$|m_{ij}|<=\epsilon, \forall i,j@f$.
        - 0 otherwise.
 */
GBLA_API int gbla_matrix_isnull_eps (gbla_matrix_t * m, double eps);

/** Matrix add.

 This function adds the elements of matrix b to the elements of matrix a,
 a(i,j) = a(i,j) + b(i,j). The two matrices must have the same dimensions.

 @param[in,out] a
 @param[in] b
 @return GBLA_SUCCESS (no dimension check is done). 
*/
GBLA_API int gbla_matrix_add (gbla_matrix_t * a, gbla_matrix_t * b);

/** Matrix substraction.

  This function subtracts the elements of matrix b from the elements of matrix
  a, a(i,j) = a(i,j) - b(i,j). The two matrices must have the same
  dimensions.

 @param[in,out] a
 @param[in] b
 @return GBLA_SUCCESS (no dimension check is done). 
*/
GBLA_API int gbla_matrix_sub (gbla_matrix_t * a, gbla_matrix_t * b);

/** Matrix element multiply.

 This function multiplies the elements of matrix a by the elements of matrix
 b, a(i,j) = a(i,j) * b(i,j). The two matrices must have the same dimensions.
 
 @param[in,out] a
 @param[in] b
 @return GBLA_SUCCESS (no dimension check is done).
*/
GBLA_API int gbla_matrix_mul_elements (gbla_matrix_t * a, gbla_matrix_t * b);

/** Matrix element divide.

 This function divides the elements of matrix a by the elements of matrix b,
 a(i,j) = a(i,j) / b(i,j). The two matrices must have the same dimensions.
 
 @param[in,out] a
 @param[in] b
 @return GBLA_SUCCESS (no dimension check is done).
*/
GBLA_API int gbla_matrix_div_elements (gbla_matrix_t * a, gbla_matrix_t * b);

/** Matrix scale.

 This function multiplies the elements of matrix a by the constant factor x,
 a(i,j) = x * a(i,j).

 @param[in,out] a
 @param x scale factor.
 @return GBLA_SUCCESS.
 */
GBLA_API int gbla_matrix_scale (gbla_matrix_t * a, double x);

/** Matrix increment.

 This function adds the constant value x to the elements of the matrix a,
 a(i,j) = a(i,j) + x.
 
 @param[in,out] a
 @param x increment constant.
 @return GBLA_SUCCESS.
 */
GBLA_API int gbla_matrix_add_constant (gbla_matrix_t * a, double x);

/** Matrix increment diagonal.

 This function adds the constant value x to the diagonal elements of the
 matrix a, a(i,i) = a(i,i) + x.
 
 @param[in,out] a
 @param x increment constant.
 @return GBLA_SUCCESS.
 */
GBLA_API int gbla_matrix_add_diagonal (gbla_matrix_t * a, double x);

/** Matrix print

 This function print the content of a matrix using the @c C function printf().
 The format used to print each element if <tt>"%g  "</tt>. If you want to
 use another format use the function ::gbla_matrix_print_fmt.

 @param[in] A the matrix to print its content.
 */
GBLA_API void gbla_matrix_print(const gbla_matrix_t * A);

/** Matrix print with format.

 This function print the content of a matrix using the @c C function printf().
 The format used to print each element must be provided as argument.

 @param[in] A the matrix to print its content.
 
 @param[in] fmt a character string describing the format to be used when
            printing each element of the matrix. See the documentation of the
            standard @c C function @c printf() in order to find out the
            different valid format characters for double values.
 */
GBLA_API void gbla_matrix_print_fmt(const gbla_matrix_t * A, const char * fmt);

__GBLA_END_DECLS

#endif
