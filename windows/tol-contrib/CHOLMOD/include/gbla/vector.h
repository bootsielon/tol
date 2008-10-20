/* vector.h
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

#ifndef __GBLA_VECTOR_H__
#define __GBLA_VECTOR_H__

#include <gbla/block.h>

__GBLA_BEGIN_DECLS

/** \file vector.h
 Vector Type.
 */

/** Vectors.

 Vectors are defined by a gbla_vector_t structure which describes a slice of a
 block. Different vectors can be created which point to the same block. A
 vector slice is a set of equally-spaced elements of an area of memory.

 The gbla_vector_t structure contains five components, the size, the stride, a
 pointer to the memory where the elements are stored, data, a pointer to the
 block owned by the vector, block, if any, and an ownership flag, owner.
 */
typedef struct
{
  /** Vector size. The number of vector elements. The range of valid indices
   * runs from 0 to size-1 */
  int size;
  /** Step-size. Step-size from one element to the next in physical memory,
   * measured in units of the appropriate datatype. */
  int stride;
  /** First element address. Gives the location of the first element of the
   * vector in memory. */
  double *data;
  /** Block of memory. Stores the location of the memory block in which the
   * vector elements are located */
  gbla_block_t block;
  /** owner flag. If the vector owns this block then the owner flag is 1 and
   * the block will be deallocated when the vector is freed. If the vector
   * points to a block owned by another object then the owner flag is 0 and
   * any underlying block will not be deallocated.*/
  int flags;
} gbla_vector_t;

/** Vector structure initialization.

 This function initialize the members and resources in the vector structure v
 previously allocated either dynamically or statically. Individuals elements
 are not initialized. If you want to set every element to zero use
 gbla_vector_cinit instead. If block allocation fails then data member is set
 to NULL.
 
 @param[in] n number of elements to allocate.
 @param[in,out] v the vector to initialize. If v is NULL a new vector is
                created from the heap, then initialized.

 @return the address of the initialized vector.
 */
GBLA_API gbla_vector_t * gbla_vector_init (int n, gbla_vector_t *v);

/** Vector structure initialization.

 This function creates a vector of length n, returning a pointer to a newly
 initialized vector struct. A new block is allocated for the elements of the
 vector, and stored in the block component of the vector struct. The members
 of the vector are set to zero. The block is "owned" by the vector, and will
 be deallocated when the vector is deallocated. 

 @param[in] n number of elements to allocate.
 @param[in,out] v the vector to initialize. If v is NULL a new vector is
                created from the heap, then initialized.

 @return the address of the initialized vector.
 */
GBLA_API gbla_vector_t * gbla_vector_cinit (int n, gbla_vector_t *v);

/** Vector freeing.

 This functions release a vector and resources asigned to it. If the vector
 was created using ::gbla_vector_init or ::gbla_vector_cinit then the block
 underlying the vector will also be deallocated. If the vector has been
 created from another object then the memory is still owned by that object and
 will not be deallocated.
 
 @param[in,out] v the vector to release its resources.
 */
GBLA_API void gbla_vector_free(gbla_vector_t *v);

/** Vector view from array.

 This function returns a vector view of an array. The start of the new vector
 is given by base and has n elements. Mathematically, the i-th element of the
 new vector v is given by,

  v(i) = base[i]

  where the index i runs from 0 to n-1.

 The array containing the elements of v is not owned by the new vector
 view. When the view goes out of scope the original array will continue to
 exist. The original memory can only be deallocated by freeing the original
 pointer base. Of course, the original array should not be deallocated while
 the view is still in use.

 @param[in] v the base address to take the view from.
 @param[in] n the size of the vector.
 @return the structure definition of the vector view.
 */
GBLA_API gbla_vector_t gbla_vector_view_array (double *v, int n);

/** Vector view from array with stride.

 These functions return a vector view of an array base with an additional
 stride argument. The subvector is formed in the same way as for
 gsl_vector_view_array but the new vector has n elements with a step-size of
 stride from one element to the next in the original array. Mathematically,
 the i-th element of the new vector v is given by,

 v(i) = base[i*stride]

 where the index i runs from 0 to n-1. 

 The array containing the elements of v is not owned by the new vector
 view. When the view goes out of scope the original array will continue to
 exist. The original memory can only be deallocated by freeing the original
 pointer base. Of course, the original array should not be deallocated while
 the view is still in use.

 @param[in] base the base address to take the view from.
 @param[in] stride the step-size of the vector.
 @param[in] n the size of the vector.
 @return the structure definition of the vector view.
 */
GBLA_API gbla_vector_t
gbla_vector_view_array_with_stride (double *base, int stride, int n);

/** Subvector view.

 This function returns a vector view of a subvector of another vector v. The
 start of the new vector is offset by offset elements from the start of the
 original vector. The new vector has n elements. Mathematically, the i-th
 element of the new vector v' is given by,

 v'(i) = v->data[(offset + i)*v->stride]

 where the index i runs from 0 to n-1.

 No range checking is performed so if the combined parameters (offset,n)
 overrun the end of the original vector you can get unexpected results.

 The new vector is only a view of the block underlying the original vector,
 v. The block containing the elements of v is not owned by the new
 vector. When the view goes out of scope the original vector v and its block
 will continue to exist. The original memory can only be deallocated by
 freeing the original vector. Of course, the original vector should not be
 deallocated while the view is still in use.

 @param[in] v the vector to take the view from.
 @param[in] offset from where to start the new vector.
 @param[in] n the size of the new vector.
 @return the structure definition of the vector view.
 */
GBLA_API gbla_vector_t
gbla_vector_subvector (gbla_vector_t *v, int offset, int n);

/** Subvector view with stride.

 These functions return a vector view of a subvector of another vector v with
 an additional stride argument. The subvector is formed in the same way as for
 gsl_vector_subvector but the new vector has n elements with a step-size of
 stride from one element to the next in the original vector. Mathematically,
 the i-th element of the new vector v' is given by,

 v'(i) = v->data[(offset + i*stride)*v->stride]

 where the index i runs from 0 to n-1.

 Note that subvector views give direct access to the underlying elements of
 the original vector. For example, the following code will zero the even
 elements of the vector v of length n, while leaving the odd elements
 untouched,

 \code
 gbla_vector v_even = gbla_vector_subvector_with_stride(v, 0, 2, n/2);
 gbla_vector_set_zero(&v_even);
 \endcode

 The new vector is only a view of the block underlying the original vector,
 v. The block containing the elements of v is not owned by the new
 vector. When the view goes out of scope the original vector v and its block
 will continue to exist. The original memory can only be deallocated by
 freeing the original vector. Of course, the original vector should not be
 deallocated while the view is still in use.

 @param[in] v the vector to take the view from.
 @param[in] offset from where to start the new vector.
 @param[in] stride the step-size of the new vector with respect to the
 original vector.
 @param[in] n the size of the new vector.
 @return the structure definition of the vector view.
 */
GBLA_API gbla_vector_t
gbla_vector_subvector_with_stride (gbla_vector_t *v,
                                  int offset,
                                  int stride,
                                  int n);

/** Vector element value.
 
 This macro returns the i-th element of a vector v. No range checking is
 performed. If i lies outside the allowed range of 0 to n-1 then you can get
 unexpected results.

 @param[in] v the vector to access the element.
 @param[in] i the index of the element.
 @return return the value of the i-th element.
 */
#define gbla_vector_get(v,i) ((v)->data[(i) * ((v)->stride)])

/** Vector element modifying.

 This macro sets the i-th element of a vector v to a value x. No range
 checking is performed. If i lies outside the allowed range of 0 to n-1 then
 you can get unexpected results.

 @param[in,out] v the vector to access the element.
 @param[in] i the index of the element.
 @param[in] x the new element value.
 */
#define gbla_vector_set(v,i,x) ((v)->data[(i) * ((v)->stride)]=(x))

/** Vector element address.
 
 This macro returns the address of the i-th element of a vector v. No range
 checking is performed. If i lies outside the allowed range of 0 to n-1 then
 you can get unexpected results when you use the addres value returned.

 @param[in] v the vector to access the element.
 @param[in] i the index of the element.
 @return return the address of the i-th element.
 */
#define gbla_vector_ptr(v,i) (double*)((v)->data +((i) * ((v)->stride)))

/** Initialize vector elements to zero.
 
 This function sets all the elements of the vector v to zero.

 @param[in,out] v the vector to initialize.
 */
GBLA_API void gbla_vector_set_zero (gbla_vector_t* v);

/** Initialize vector elements to a value.
 
 This function sets all the elements of the vector v to the value x.

 @param[in,out] v the vector to initialize.
 @param[in] x the value to copy.
 */
GBLA_API void gbla_vector_set_all (gbla_vector_t* v, double x);

/** Vector random initialization.

 This function sets all the elements of a vector, given as argument, to a
 random value from the interval @f$[0,1]@f$. The random values are generated
 using the POSIX function rand().
 
 @param[in,out] v the vector to initialize.
*/
GBLA_API void gbla_vector_set_all_random (gbla_vector_t* v);

/** Build a basis vector.

 This function makes a basis vector by setting all the elements of the vector
 v to zero except for the i-th element which is set to one.

 @param[in,out] v the vector to set as a basis.
 @param[in] i index of the basis to set.
 */
GBLA_API int gbla_vector_set_basis (gbla_vector_t* v, int i);

/** Vector copy.

 This function copies the elements of the vector src into the vector dest. The
 two vectors must have the same length.

 @param[out] dest detination vector for copying.
 @param[in] src source vector when reading.
 @return GBLA_SUCCESS (no range checking is done).
 */
GBLA_API int gbla_vector_copy (gbla_vector_t* dest, gbla_vector_t* src);

/** Vector reverse elements.

 This function reverses the order of the elements of the vector v.
 
 @param[in,out] v the vector to reverse.
 @return GBLA_SUCCESS (no checking is done).
 */
GBLA_API int gbla_vector_reverse (gbla_vector_t* v);

/** Vector swap.

 This function exchanges the elements of the vectors v and w by copying. The
 two vectors must have the same length.

 @param[in,out] v a vector.
 @param[in,out] w a vector.
 @return GBLA_SUCCESS (no range checking is done).
 */
GBLA_API int gbla_vector_swap (gbla_vector_t* v, gbla_vector_t* w);

/** Vector swap elements.

 This function exchanges the i-th and j-th elements of the vector v in-place.

 @param[in,out] v the vector.
 @param[in] i index of the i-th element in v. 
 @param[in] j index of the j-th element in v. 
 @return GBLA_SUCCESS (no range checking is done).
 */
GBLA_API int gbla_vector_swap_elements (gbla_vector_t* v, int i, int j);

/** Vector maximun element value.

 This function returns the maximum value in the vector v.

 @param[in] v the vector to look in.
 @return the maximun value in v.
 */
GBLA_API double gbla_vector_max (gbla_vector_t* v);

/** Vector minimun element value.

 This function returns the minimun value in the vector v.

 @param[in] v the vector to look in.
 @return the minimun value in v.
 */
GBLA_API double gbla_vector_min (gbla_vector_t* v);

/** Vector minimun and maximun elements value.

 This function returns the minimum and maximum values in the vector v, storing
 them in min_out and max_out.

 @param[in] v the vector to look in.
 @param[out] min_out the address where to put the value of the minimun value.
 @param[out] max_out the address where to put the value of the maximun value.
 */
GBLA_API void gbla_vector_minmax (gbla_vector_t* v, double * min_out, double * max_out);

/** Vector index of maximun value.

 This function returns the index of the maximum value in the vector v. When
 there are several equal maximum elements then the lowest index is returned.

 @param[in] v the vector to look in.
 @return the index of the maximun value in v.
 */
GBLA_API int gbla_vector_max_index (gbla_vector_t* v);

/** Vector index of minimun value.

 This function returns the index of the minimun value in the vector v. When
 there are several equal minimun elements then the lowest index is returned.

 @param[in] v the vector to look in.
 @return the index of the minimun value in v.
 */
GBLA_API int gbla_vector_min_index (gbla_vector_t* v);

/** Vector indices of maximun and minimun values.

 This function returns the indices of the minimum and maximum values in the
 vector v, storing them in imin and imax. When there are several equal minimum
 or maximum elements then the lowest indices are returned.

 @param[in] v the vector to look in.
 @param[out] imin index of the minimun value found.
 @param[out] imax index of the maximun value found.
 */
GBLA_API void gbla_vector_minmax_index (gbla_vector_t* v,
                              int * imin, int * imax);

/** Vector addition.

 This function adds the elements of vector b to the elements of vector a,
 \f$a_i = a_i + b_i\f$. The two vectors must have the same length.

 @param[in,out] a a vector.
 @param[in] b a vector.
 @return GBLA_SUCCESS (no dimension checking is done)
 */
GBLA_API int gbla_vector_add (gbla_vector_t* a, gbla_vector_t* b);

/** Vector substraction.

 This function subtracts the elements of vector b from the elements of vector
 a, \f$a_i = a_i - b_i\f$. The two vectors must have the same length.

 @param[in,out] a a vector.
 @param[in] b a vector.
 @return GBLA_SUCCESS (no dimension checking is done)
 */
GBLA_API int gbla_vector_sub (gbla_vector_t* a, gbla_vector_t* b);

/** Vector multiply.

  This function multiplies the elements of vector a by the elements of vector
  b, \f$a_i = a_i * b_i\f$. The two vectors must have the same length.

 @param[in,out] a a vector.
 @param[in] b a vector.
 @return GBLA_SUCCESS (no dimension checking is done)
 */
GBLA_API int gbla_vector_mul (gbla_vector_t* a, gbla_vector_t* b);


/** Vector divide.

  This function divides the elements of vector a by the elements of vector b,
  \f$a_i = a_i / b_i\f$. The two vectors must have the same length.
  
 @param[in,out] a a vector.
 @param[in] b a vector.
 @return GBLA_SUCCESS (no dimension checking is done)
 */
GBLA_API int gbla_vector_div (gbla_vector_t* a, gbla_vector_t* b);


/** Vector scale.

 This function multiplies the elements of vector a by the constant factor x,
 \f$a_i = a_i * x\f$.
 
 @param[in,out] a the vector.
 @param[in] x the constant factor.
 @return GBLA_SUCCESS
 */
GBLA_API int gbla_vector_scale (gbla_vector_t* a, double x);

/** Vector increment.

 This function adds the constant value x to the elements of the vector a,
 \f$a_i = a_i + x\f$
 
 @param[in,out] a the vector.
 @param[in] x the constant increment.
 @return GBLA_SUCCESS
 */
GBLA_API int gbla_vector_add_constant (gbla_vector_t* a, double x);

/** Vector test for null

 This function returns 1 if all the elements of the vector v are zero, and 0
 otherwise.

 @param[in] v the vector to test.
 @return
        - 1 if all the elements of the vector v are zero.
        - 0 otherwise.
 */
GBLA_API int gbla_vector_isnull (gbla_vector_t* v);

/** Vector test for null with epsilon

 This function returns 1 if all the elements of the vector v are in a
 neighborhood of zero. The test is defined for @f$\epsilon>0@f$ as
 @f$|v_i|<=\epsilon@f$ and 0 otherwise.

 @param[in] v the vector to test.

 @param[in] eps is the value of @f$\epsilon>0@f$ which define de neighborhood
            @f$[-\epsilon,+\epsilon]@f$.
 
 @return
        - 1 if @f$|v_i|<=\epsilon, \forall i@f$.
        - 0 otherwise.
 */
GBLA_API int gbla_vector_isnull_eps (gbla_vector_t* v, double eps);

/** Vector print

 This function print the content of a vector using the @c C function
 printf(). The vector is printed as a row.  The format used to print each
 element if <tt>"%g "</tt>. If you want to use another format use the function
 ::gbla_vector_print_fmt.

 @param[in] X the vector to print its content.
 */
GBLA_API void gbla_vector_print(const gbla_vector_t * X);

/** Vector print with format.

 This function print the content of a vector using the @c C function printf().
 The format used to print each element must be provided as argument.

 @param[in] X the vector to print its content.
 
 @param[in] fmt a character string describing the format to be used when
            printing each element of the vector. See the documentation of the
            standard @c C function @c printf() in order to find out the
            different valid format characters for double values.
*/
GBLA_API void gbla_vector_print_fmt(const gbla_vector_t * X, const char * fmt);

__GBLA_END_DECLS

#endif
