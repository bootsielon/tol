/* blapi.h
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

/** \file blapi.h
  @b BLAS/LAPACK Low Level API.
  
  This is a dynamic API to @b BLAS/LAPACK functionality. It's main purpose is
  to provide an interface to a @b BLAS/LAPACK shared library for which we have
  no header file. This library is intended to operate with @b BLAS/LAPACK
  compiled with @b FORTRAN. The dynamic linking with the shared libraries is
  done throught @b libltdl package of @b libtool.
 */

/*
 * The following macros affect the way we define the prototype of the pointer
 * function providing the actual functionalities of @b BLAS/LAPACK.
 *
 * NEED_STDCALL -- __stdcall convention is needed.
 * 
 */

#ifndef __BLAPI_H__
#define __BLAPI_H__

#if defined(_MSC_VER)
# ifndef WIN32
#  define WIN32
# endif
# if defined(NEED_STDCALL)
#  define FORTRAN __stdcall
# else
#  define FORTRAN
# endif
# if defined(BLAPI_SHARED)
#  if defined(BLAPI_EXPORTS) 
#   define BL_API __declspec(dllexport)
#  else
#   define BL_API __declspec(dllimport)
#  endif
# else
#  define BL_API
# endif
#else
# define FORTRAN
# define BL_API
#endif

#if defined(__cplusplus)
extern "C" {
#endif

/** Single precision complex struct.
 
 Structure to be used when passing @c COMPLEX arguments to @b FORTRAN routines.
 */
typedef struct {
  /** real part of the complex number */
  float    real;
  /** imaginary part of the complex number */
  float    imag;
} blapi_complex8_t;

/** Double precision complex struct.
 
 Structure to be used when passing @c COMPLEX*8 arguments to @b FORTRAN
 routines.
 */
typedef struct {
  /** real part of the complex number */
  double    real;
  /** imaginary part of the complex number */
  double    imag;
} blapi_complex16_t;

/* Table of pointers type to @b BLAS/LAPACK routines.
 */
typedef struct {
  void *blas_handle;
  void *lapack_handle;
  
#include <gbla/blas_members.h>
#ifdef __USE_PRISM_STRASSEN__
void FORTRAN (*ptr_dgefmm)(char *transa,char *transb,int *m,int *n,int *k,double *
alpha,double *a,int *lda,double *b,int *ldb,double *beta,double *c,int *ldc);
#endif
#include <gbla/lapack_members.h>
  
} blapi_table_t;

/** Table of pointers to @b BLAS/LAPACK routines.

 This variable contain a table of pointers to each @b BLAS/LAPACK
 routine. That will enable us to make a low level call to the @b FORTRAN
 routine directly. For each function a macro is provided to make easy the
 access to the members of ::blapi_table. For instance if you want to call the
 function @c dgemm you should call the macro @c call_dgemm in a way similar to
 this:

 @verbatim
 char transA, transB;
 int M, N, K;
 double *A, *B, *C;
 int lda, ldb, ldc;
 ...
 call_dgemm(&transA, &transB, &M, &N, &K, &alpha, A, &lda, B, &ldb, &beta, C, &ldc);
 @endverbatim

 A high level API to access BLAS routines is provided in blas.h 
 */
extern BL_API blapi_table_t blapi_table;

#include <gbla/blas_defines.h>

#if !defined(IN_DOXYGEN)
#ifdef __USE_PRISM_STRASSEN__
#define call_dgefmm  (*(blapi_table.ptr_dgefmm))
#endif
#endif

#include <gbla/lapack_defines.h>

/** @b BLAS API initialization.
  
 This function initialize the internal table of pointers to @b BLAS
 functions. It takes as one of its arguments the name of the shared object
 library containing the actual @b BLAS implementation. The name should be
 provided without extension. The library determine the appropiate extension
 depending on the platform. The rest of the arguments describe the name
 decoration used by the @b FORTRAN compiler that generate the shared object.
  
 \param[in] shared_blas name of the @b BLAS shared lib to load
 \param[in] capital indicate if the names should be requested in upper case.
 \param[in] preffix is a string that should be prepended to the name of the
 routine before linking the function pointer to the shared object code.
 \param[in] suffix is a string that should be appened to the name of the
 routine before linking the function pointer to the shared object code.
 \return
         - 0 in case of success.
         - -1 if the @b BLAS shared lib could not be found or opened.  
 */
BL_API int blapi_blas_init(const char * shared_blas, const char * preffix, int capital, const char * suffix);

/** @b LAPACK API initialization.
  
 This function initialize the internal table of pointers to @b LAPACK
 functions. It takes as one of its arguments the name of the shared object
 library containing the actual @b LAPACK implementation. The name should be
 provided without extension. The library determine the appropiate extension
 depending on the platform. The rest of the arguments describe the name
 decoration used by the @b FORTRAN compiler that generate the shared object.
  
 \param[in] shared_lapack name of the @b LAPACK shared lib to load
 \param[in] capital indicate if the names should be requested in upper case.
 \param[in] preffix is a string that should be prepended to the name of the
 routine before linking the function pointer to the shared object code.
 \param[in] suffix is a string that should be appened to the name of the
 routine before linking the function pointer to the shared object code.
 \return
         - 0 in case of success.
         - -1 if the @b LAPACK shared lib could not be found or opened.  
 */
BL_API int blapi_lapack_init(const char * shared_lapack, const char * preffix, int capital, const char * suffix);

/** Close the @b BLAS/LAPACK connection.

 This function close the connection to the handlers of the @b BLAS/LAPACK
 shared libraries.
 
 \return
         - 0 in case of success.
         - !0 if the @b BLAS/LAPACK handlers could not be closed.  
 */
BL_API int blapi_exit();

/** @b BLAS/LAPACK API initialization

 This function initialize the function's pointers for both @b BLAS and @b
 LAPACK API. This function make use of the values of the globals variables:
 <tt>blapi_blas_lib, blapi_blas_prefix, blapi_blas_capital, blapi_blas_suffix,
 blapi_lapack_lib, blapi_lapack_prefix, blapi_lapack_capital</tt> and
 @c blapi_lapack_suffix to specify the name decoration for the @b FORTRAN
 routines.  All those variables has default values but can be initialized from
 environment variable by calling ::blapi_blapi_env_setup
 
 \return
         - 0 in case of success.
         - -1 if @b BLAS could not be initialized.
         - -2 if @b LAPACK could not be initialized.
 */
BL_API int blapi_init();

/** BLAS library name. \see blapi_env_setup.
 */
extern BL_API char * blapi_blas_lib;

/** BLAS prefix decoration. \see blapi_env_setup.
 */
extern BL_API char * blapi_blas_prefix;

/** BLAS upper case decoration. \see blapi_env_setup.
 */
extern BL_API int blapi_blas_capital;

/** BLAS suffix decoration. \see blapi_env_setup.
 */
extern BL_API char * blapi_blas_suffix;

/** LAPCK library name. \see blapi_env_setup.
 */
extern BL_API char * blapi_lapack_lib;

/** LAPACK prefix decoration. \see blapi_env_setup.
 */
extern BL_API char * blapi_lapack_prefix;

/** @b LAPACK upper case decoration. \see blapi_env_setup.
 */
extern BL_API int blapi_lapack_capital;

/** LAPACK suffix decoration. \see blapi_env_setup.
 */
extern BL_API char * blapi_lapack_suffix;

/** BLAPI environment setup.

 This function initialized a set of global variables from the
 environment. This variables can be used as the arguments value to the
 functions ::blapi_blas_init and ::blapi_lapack_init.
 
 The library can take the configuration of the @b BLAS/LAPACK linking
 description by the use of environment variables. In this way we can change
 from one @b BLAS/LAPACK implementation to another without recompiling the
 application, just changing the values of a set of environment variables we
 can change the @b BLAS/LAPACK implementation to which the application link.

 In the following when we say "match to" we means "takes its value from the
 environment variable". The environment variables related to this global
 variables are:
 
 - @c blapi_blas_lib match to @c BLAPI_BLAS_LIB (defaults to @c "libblas32"in @c
   WIN32, @c "libblas" in other case).
 
 - @c blapi_blas_prefix match to @c BLAPI_BLAS_PREFIX (defaults to "").
 
 - @c blapi_blas_capital match to @c BLAPI_BLAS_CAPITAL (defaults to 1 in @c
   WIN32, 0 in other case).
 
 - @c blapi_blas_suffix match to @c BLAPI_BLAS_SUFFIX (defaults to @c "_").
  
 - @c blapi_lapack_lib match to @c BLAPI_LAPACK_LIB (defaults to @c
   "liblapack32" in @c WIN32, @c "liblapack" in other case).
 
 - @c blapi_lapack_prefix match to @c BLAPI_LAPACK_PREFIX (defaults to @c
   BLAPI_BLAS_PREFIX).
 
 - @c blapi_lapack_capital match to @c BLAPI_LAPACK_CAPITAL (defaults to @c
   BLAPI_BLAS_CAPITAL).
 
 - @c blapi_lapack_suffix match to @c BLAPI_LAPACK_SUFFIX (defaults to @c
   BLAPI_BLAS_SUFFIX).
 */
BL_API void blapi_env_setup();

#if defined(__cplusplus)
}
#endif

#endif
