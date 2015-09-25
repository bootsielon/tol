/* tol_bvmat_impl.h: BVMat Header for implementation files.
 
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef TOL_BVMAT_IMPL_H
#define TOL_BVMAT_IMPL_H 1

#include <tol/tol_bvmat.h>
#include <tol/tol_hash_map.h>
#include <tol/tol_bmatrix.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bset.h>

#if defined(_MSC_VER)
#pragma pack(push, 8)
#endif
#include "cholmod.h"
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

typedef hash_map_by_BIntPair<double>::sparse_ BHashDoubleByIntPair;
typedef hash_map_by_BIntPair<bool>::sparse_ BHashBoolByIntPair;
        
#define DefineMonary(FUN,fun,f00,NAME)                                         \
int BVMat::FUN(const BVMat& A, BVMat& B)                                       \
{ return(ApplyFunRR(fun,  f00,  NAME, A,B)); }                                 \
BVMat BVMat::FUN() const                                                       \
{ return(ApplyFunRR(fun,  f00,   NAME)); }                                     \


#define DefineBinaryR(FUN,fun,f00,f0x,NAME)                                    \
int BVMat::FUN(const BVMat& A, double b, BVMat& C)                             \
{ return(ApplyFunR2r(fun,  f00,  f0x, NAME, A,b,C)); }                         \
BVMat BVMat::FUN(double b) const                                               \
{ return(ApplyFunR2r(fun,  f00,  f0x, NAME, b)); }                             \
 
#define DefineBinary(FUN,fun,f00,f0x,NAME)                                     \
int BVMat::FUN(const BVMat& A, const BVMat& B, BVMat& C)                       \
{ return(ApplyFunR2R(fun,  f00,  f0x, NAME, A,B,C)); }                         \
BVMat BVMat::FUN(const BVMat& B) const                                         \
{ return(ApplyFunR2R(fun,  f00, f0x,  NAME, B)); }                             \


//#define TRACE_CHOLMOD_MEMORY

#ifdef TRACE_CHOLMOD_MEMORY

void TraceCholmodAllocate(const BText& type, void* ptr, double size);
void TraceCholmodFree    (const BText& type, void* ptr, double size);

#define TRACE_CHOLMOD_ALLOCATE(TYPE,PTR) \
{ \
  size_t size = BVMat::bytes(PTR); \
  TraceCholmodAllocate(TYPE,PTR,size); \
}

#define TRACE_CHOLMOD_FREE(TYPE,PTR) \
{ \
  size_t size = BVMat::bytes(PTR); \
  TraceCholmodFree(TYPE,PTR,size); \
}

#else

#define TRACE_CHOLMOD_ALLOCATE(TYPE,PTR)
#define TRACE_CHOLMOD_FREE(TYPE,PTR)

#endif


////////////////////////////////////////////////////////////////////////////////
cholmod_R_dense *CholmodAllocate_dense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t nrow,	/* # of rows of matrix */
    size_t ncol,	/* # of columns of matrix */
    size_t d,		/* leading dimension */
    int xtype,		/* CHOLMOD_REAL, _COMPLEX, or _ZOMPLEX */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
cholmod_R_sparse *CholmodAllocate_sparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t nrow,	/* # of rows of A */
    size_t ncol,	/* # of columns of A */
    size_t nzmax,	/* max # of nonzeros of A */
    int sorted,		/* TRUE if columns of A sorted, FALSE otherwise */
    int packed,		/* TRUE if A will be packed, FALSE otherwise */
    int stype,		/* stype of A */
    int xtype,		/* CHOLMOD_PATTERN, _REAL, _COMPLEX, or _ZOMPLEX */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
int CholmodSort
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_sparse *A,	/* matrix to sort */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_R_triplet *CholmodAllocate_triplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t nrow,	/* # of rows of T */
    size_t ncol,	/* # of columns of T */
    size_t nzmax,	/* max # of nonzeros of T */
    int stype,		/* stype of T */
    int xtype,		/* CHOLMOD_PATTERN, _REAL, _COMPLEX, or _ZOMPLEX */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
int CholmodReallocateTriplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t nznew,	/* new # of entries in T */
    /* ---- in/out --- */
    cholmod_triplet *T,	/* triplet matrix to modify */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_factor *CholmodAllocate_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t n,		/* L is n-by-n */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
int CholmodPackFactor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_factor *L,	/* factor to modify */
    /* --------------- */
    cholmod_common *Common
) ;


////////////////////////////////////////////////////////////////////////////////
cholmod_dense *CholmodCopy_dense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_dense *d,	/* dense to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodCopy_sparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_sparse *L,	/* sparse to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodCopy 
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_sparse *A,	/* matrix to copy */
    int stype,		/* requested stype of C */
    int mode,		/* >0: numerical, 0: pattern, <0: pattern (no diag) */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_triplet *CholmodCopy_triplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_triplet *L,	/* triplet to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_factor *CholmodCopy_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_factor *L,	/* factor to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_dense *CholmodSparseToDense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_sparse *A,	/* matrix to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodDenseToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_dense *X,	/* matrix to copy */
    int values,		/* TRUE if values to be copied, FALSE otherwise */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_triplet *CholmodSparseToTriplet
(
    /* ---- input ---- */
    cholmod_sparse *A,	/* matrix to copy */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodTripletToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_triplet *T,	/* matrix to copy */
    int nzmax,		/* allocate at least this much space in output matrix */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodFactorToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_factor *L,	/* factor to copy, converted to symbolic on output */
    /* --------------- */
    cholmod_common *Common
) ;

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_dense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_dense **d,	/* dense to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
);


////////////////////////////////////////////////////////////////////////////////
int CholmodFree_sparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_sparse **s,	/* sparse to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_triplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_triplet **t,	/* triplet to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
);

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_factor **L,	/* factor to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
);
   
#endif // TOL_BVMAT_IMPL_H
