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
#pragma pack(8)
#include "cholmod.h"
#pragma pack(4)
typedef hash_map_by_BIntPair<double>::sparse_ BHashPairs;

BEGIN_DECLS
#include <cblas.h>
#include <clapack.h>
END_DECLS

        
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

       
        
#endif // TOL_BVMAT_IMPL_H
