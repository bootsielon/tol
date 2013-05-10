/* vmat.cpp: BVMat Generic methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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
#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bvmat_impl.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <stdarg.h>

     
////////////////////////////////////////////////////////////////////////////////
// Static variables
////////////////////////////////////////////////////////////////////////////////
BText             *BVMat::engineTypeName_ [BVMat::engineTypeName_numOpt];
BText             *BVMat::cellTypeName_   [BVMat::cellTypeName_numOpt];
BText             *BVMat::storeTypeName_  [BVMat::storeTypeName_numOpt];
BText             *BVMat::codeName_       [BVMat::codeName_numOpt];
BText             *BVMat::cSysName_       [BVMat::cSysName_numOpt];
BText             *BVMat::cFacName_       [BVMat::cFacName_numOpt];
BVMat::StrDefCode  BVMat::defCode_        [BVMat::defCode_numOpt];
BVMat::StrConvert  BVMat::convert_        [BVMat::convert_numOpt];;
BVMat::StrProduct  BVMat::product_        [BVMat::product_numOpt];;
BVMat::StrCholSol  BVMat::cholSol_        [BVMat::cholSol_numOpt];;
BVMat::StrCholFac  BVMat::cholFac_        [BVMat::cholFac_numOpt];;
cholmod_common    *BVMat::common_             = NULL;
FILE              *BVMat::cholmod_print_file_ = NULL;
BDat               BVMat::autoConvert_        = true;
static cholmod_common BVMat_cholmod_common;

//#define TRACE_CHOLMOD_MEMORY

#ifdef TRACE_CHOLMOD_MEMORY
static int cholmod_num_register = 0;
static double cholmod_size_allocated = 0;
static double cholmod_num_allocated  = 0;
static double cholmod_size_deallocated = 0;
static double cholmod_num_deallocated  = 0;
static double cholmod_size_balance = 0;
static double cholmod_num_balance  = 0;

static FILE* fchlmd_ = NULL;

////////////////////////////////////////////////////////////////////////////////
void TraceCholmodInitialize()
////////////////////////////////////////////////////////////////////////////////
{
  fchlmd_ = fopen("fcholmod.txt","w");
  if(!fchlmd_)
  {
    Error("[TraceCholmodInitialize] cannot open file fcholmod.txt");
  }
  else
  {
    fprintf(fchlmd_,
      "NumRegister\tOperation\tType\tPointer\tSize\tObjects\tBytes\tCholmod_malloc_count\tCholmod_memory_usage\tCholmod_memory_inuse\n");
    cholmod_num_register = 0;
    cholmod_size_allocated = 0;
    cholmod_num_allocated  = 0;
    cholmod_size_deallocated = 0;
    cholmod_num_deallocated  = 0;
    cholmod_size_balance = 0;
    cholmod_num_balance  = 0;
  }
}


////////////////////////////////////////////////////////////////////////////////
void TraceCholmodAllocate(const BText& type, void* ptr, double size)
////////////////////////////////////////////////////////////////////////////////
{
  cholmod_num_register++;
  cholmod_size_allocated += size; 
  cholmod_num_allocated += 1.0; 
  cholmod_size_balance += size; 
  cholmod_num_balance += 1.0; 
  if(fchlmd_)
  {
    int ptr_ = (int)ptr;
    fprintf(fchlmd_,"%ld\tALLOCATE\t%s\t%ld\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\n",
      cholmod_num_register,type.String(),ptr_,size,
      cholmod_num_balance,cholmod_size_balance,
      (double)BVMat::Common()->malloc_count,
      (double)BVMat::Common()->memory_usage,
      (double)BVMat::Common()->memory_inuse);
    fflush(fchlmd_);
  }
  else
  {
    Error("[TraceCholmodAllocate] file fcholmod.txt is not open");
  }
}

////////////////////////////////////////////////////////////////////////////////
void TraceCholmodFree(const BText& type, void* ptr, double size)
////////////////////////////////////////////////////////////////////////////////
{
  cholmod_num_register++;
  cholmod_size_deallocated += size; 
  cholmod_num_deallocated += 1.0; 
  cholmod_size_balance -= size; 
  cholmod_num_balance -= 1.0; 
  if(fchlmd_)
  {
    int ptr_ = (int)ptr;
    fprintf(fchlmd_,"%ld\tFREE\t%s\t%ld\t%lg\t%lg\t%lg\t%lg\t%lg\t%lg\n",
      cholmod_num_register,type.String(),ptr_,size,
      cholmod_num_balance,cholmod_size_balance,
      (double)BVMat::Common()->malloc_count,
      (double)BVMat::Common()->memory_usage,
      (double)BVMat::Common()->memory_inuse);
    fflush(fchlmd_);
  }
  else
  {
    Error("[TraceCholmodAllocate] file fcholmod.txt is not open");
  }
}



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
)
{
  cholmod_R_dense* ptr = 
    cholmod_allocate_dense(nrow,ncol,d,xtype,Common);
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",ptr);
  return(ptr);
}

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
)
{
  cholmod_R_sparse* ptr =
    cholmod_allocate_sparse(nrow,ncol,nzmax,sorted,packed,stype,xtype,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
int CholmodSort
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_sparse *A,	/* matrix to sort */
    /* --------------- */
    cholmod_common *Common
)
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Sparse",A);
  int r = cholmod_sort(A,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",A);
  return(r);
}

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
)
{
  cholmod_R_triplet* ptr =
    cholmod_allocate_triplet(nrow,ncol,nzmax,stype,xtype,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Triplet",ptr);
  return(ptr);
}

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
) 
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Triplet",T);
  int r = cholmod_reallocate_triplet(nznew,T,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Triplet",T);
  return(r);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_factor *CholmodAllocate_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    size_t n,		/* L is n-by-n */
    /* --------------- */
    cholmod_common_struct *Common
)
{
  cholmod_R_factor* ptr =
    cholmod_allocate_factor(n,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Factor",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
int CholmodPackFactor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_factor *L,	/* factor to modify */
    /* --------------- */
    cholmod_common *Common
)
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Factor",L);
  int r = cholmod_pack_factor(L,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Factor",L);
  return(r);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_dense *CholmodCopy_dense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_dense *d,	/* dense to copy */
    /* --------------- */
    cholmod_common *Common
)
{
  cholmod_R_dense* ptr =
    cholmod_copy_dense(d,Common);
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodCopy_sparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_sparse *s,	/* sparse to copy */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_sparse* ptr =
    cholmod_copy_sparse(s,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

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
) 
{
  cholmod_R_sparse* ptr =
    cholmod_copy(A,stype,mode,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_triplet *CholmodCopy_triplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_triplet *t,	/* triplet to copy */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_triplet* ptr =
    cholmod_copy_triplet(t,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Triplet",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_factor *CholmodCopy_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_factor *L,	/* factor to copy */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_factor* ptr =
    cholmod_copy_factor(L,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Factor",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_dense *CholmodSparseToDense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_sparse *A,	/* matrix to copy */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_dense* ptr =
    cholmod_sparse_to_dense(A,Common);
  TRACE_CHOLMOD_ALLOCATE("Blas.R.Dense",ptr);
  return(ptr);
}
////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodDenseToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_dense *X,	/* matrix to copy */
    int values,		/* TRUE if values to be copied, FALSE otherwise */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_sparse* ptr =
    cholmod_dense_to_sparse(X,values,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_triplet *CholmodSparseToTriplet
(
    /* ---- input ---- */
    cholmod_sparse *A,	/* matrix to copy */
    /* --------------- */
    cholmod_common *Common
)
{
  cholmod_R_triplet* ptr =
    cholmod_sparse_to_triplet(A,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Triplet",ptr);
  return(ptr);
}


////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodTripletToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- input ---- */
    cholmod_triplet *T,	/* matrix to copy */
    int nzmax,		/* allocate at least this much space in output matrix */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_sparse* ptr =
    cholmod_triplet_to_sparse(T,nzmax,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
cholmod_sparse *CholmodFactorToSparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_factor *L,	/* factor to copy, converted to symbolic on output */
    /* --------------- */
    cholmod_common *Common
) 
{
  cholmod_R_sparse* ptr =
    cholmod_factor_to_sparse(L,Common);
  TRACE_CHOLMOD_ALLOCATE("Cholmod.R.Sparse",ptr);
  return(ptr);
}

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_dense
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_dense **d,	/* dense to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
)
{
  TRACE_CHOLMOD_FREE("Blas.R.Dense",*d);
  return(cholmod_free_dense(d,Common));
}
////////////////////////////////////////////////////////////////////////////////
int CholmodFree_sparse
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_sparse **s,	/* sparse to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
)
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Sparse",*s);
  return(cholmod_free_sparse(s,Common));
}

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_triplet
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_triplet **t,	/* triplet to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
)
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Triplet",*t);
  return(cholmod_free_triplet(t,Common));
}

////////////////////////////////////////////////////////////////////////////////
int CholmodFree_factor
////////////////////////////////////////////////////////////////////////////////
(
    /* ---- in/out --- */
    cholmod_R_factor **L,	/* factor to free, NULL on output */
    /* --------------- */
    cholmod_common_struct *Common
)
{
  TRACE_CHOLMOD_FREE("Cholmod.R.Factor",*L);
  return(cholmod_free_factor(L,Common));
}

////////////////////////////////////////////////////////////////////////////////
// BVMat members
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
BDat& BVMat::AutoConvert()
////////////////////////////////////////////////////////////////////////////////
{
  return(autoConvert_);
}

////////////////////////////////////////////////////////////////////////////////
int BVMat::InitializeClass()
////////////////////////////////////////////////////////////////////////////////
{
#ifdef TRACE_CHOLMOD_MEMORY
  TraceCholmodInitialize();
#endif
  bool done = false;
  if(!done)
  {
    done = true;
  //common_ = new cholmod_common;
    common_ = &BVMat_cholmod_common;
    cholmod_start(common_);
    cholmod_print_file_ = stdout;
    restore_cholmod_common();
  //int (*print_function) (const char *, ...)
    initializeIdentifiers();
  }
  return(done);
}  

////////////////////////////////////////////////////////////////////////////////
bool BVMat::force_natural_order(bool force)
////////////////////////////////////////////////////////////////////////////////
{
  static bool force_ = false;
  bool old = force_;
  force_ = force;
  if(force)
  {
    common_->nmethods            = 1 ;
    common_->method[0].ordering  = CHOLMOD_NATURAL ;
    common_->postorder           = false;
  }
  else
  {
    common_->nmethods           = 3 ;
    common_->method[0].ordering = CHOLMOD_GIVEN ;
    common_->method[1].ordering = CHOLMOD_AMD ;
    common_->method[2].ordering = CHOLMOD_METIS ;
    common_->postorder          = true;
  }
  return(old);
}

////////////////////////////////////////////////////////////////////////////////
void BVMat::restore_cholmod_common()
////////////////////////////////////////////////////////////////////////////////
{
#ifndef NDEBUG
  static bool initialized_ = false;
  if(!initialized_)
  {
    initialized_ = true;
  }
  else
  {
    if(common_->print!=5)
    {
      Warning("BVMat::common_->print has been modified");
    }
    if(common_->print_function!=cholmod_print_function)
    {
      Warning("BVMat::common_->print_function has been modified");
    }
    if(common_->error_handler!=cholmod_error_handler)
    {
      Warning("BVMat::common_->error_handler has been modified");
    }
    if(common_->try_catch)
    {
      Warning("BVMat::common_->try_catch has been modified");
    }
    if(common_->supernodal!=CHOLMOD_SUPERNODAL)
    {
      Warning("BVMat::common_->supernodal has been modified");
    }
    if(common_->final_ll!=1)
    {
      Warning("BVMat::common_->final_ll has been modified");
    }
    if(common_->final_pack!=1)
    {
      Warning("BVMat::common_->final_pack has been modified");
    }
  }
#endif
  common_->print          = 5;
  common_->print_function = cholmod_print_function;
  common_->error_handler  = cholmod_error_handler;
  common_->try_catch      = NULL;

  //Stop when non positive definite is found
  common_->quick_return_if_not_posdef = true;

  //Store allways as L*L', never as L*D*L'
  common_->final_ll = true ;

  //Needed to allow fast factor to sparse convertion
  common_->final_pack      = true ;
  common_->final_super     = false ;
  common_->final_monotonic = true;

  //We need specific final storement
  common_->final_asis = false ;

  //Natural order is not forced by default
  force_natural_order(false);

  //No growing allowed
  common_->grow2 = 0;

/* * /
//MatLab lchol options
  common_->supernodal                 = CHOLMOD_SIMPLICIAL;
  common_->final_asis                 = false ;
  common_->final_super                = false ;
  common_->final_ll                   = true ;
  common_->final_pack                 = true ;
  common_->final_monotonic            = true ;
  common_->final_resymbol             = false ;
  common_->quick_return_if_not_posdef = true;
  common_->nmethods                   = 1 ;
  common_->method[0].ordering         = CHOLMOD_NATURAL ;
  common_->postorder                  = false ;

/* */
}  


////////////////////////////////////////////////////////////////////////////////
  BVMat::BVMat()
//Constructor
////////////////////////////////////////////////////////////////////////////////
: code_(ESC_undefined)
{
  s_.undefined_=NULL;
}

////////////////////////////////////////////////////////////////////////////////
  BVMat::BVMat(const BVMat& v)
//Constructor
////////////////////////////////////////////////////////////////////////////////
: code_(ESC_undefined)
{
  s_.undefined_=NULL;
  Copy(v);
}

////////////////////////////////////////////////////////////////////////////////
  BVMat::BVMat(const BVMat& v, ECode c)
//Constructor
////////////////////////////////////////////////////////////////////////////////
: code_(ESC_undefined)
{
  s_.undefined_=NULL;
  Convert(v,c);
}
  
  
////////////////////////////////////////////////////////////////////////////////
  BVMat::~BVMat() 
//Destructor
////////////////////////////////////////////////////////////////////////////////
{ 
  Delete();  
}
  
////////////////////////////////////////////////////////////////////////////////
  const BVMat& BVMat::operator = (const BVMat& v)
//Copy operator
////////////////////////////////////////////////////////////////////////////////
{
  Copy(v);
  return(*this);
}

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Delete()
//Internal destructor  
////////////////////////////////////////////////////////////////////////////////
{
  if(s_.undefined_)
  {
    switch(code_)  {
    case(ESC_blasRdense   ):CholmodFree_dense  (&s_.blasRdense_,  common_);break;
    case(ESC_chlmRsparse  ):CholmodFree_sparse (&s_.chlmRsparse_, common_);break;
    case(ESC_chlmRfactor  ):CholmodFree_factor (&s_.chlmRfactor_, common_);break;
    case(ESC_chlmRtriplet ):CholmodFree_triplet(&s_.chlmRtriplet_,common_);break; 
    default: ; 
      Error(I2("FATAL ERROR in",
               "ERROR FATAL en")+" BVMat::Delete()"); 
      assert(1); }
    s_.undefined_ = NULL;
    code_ = ESC_undefined;
  }
};

////////////////////////////////////////////////////////////////////////////////
void BVMat::ShowCholmodStats()
////////////////////////////////////////////////////////////////////////////////
{
  Std("\n------------------------------------------------------------------------------");
  Std(BText("\nCholmod statistics"));
  Std("\n------------------------------------------------------------------------------\n");

  Std(BText("fl = ")+common_->fl+ " : LL' flop count from most recent analysis \n");
  Std(BText("lnz = ")+common_->lnz+ " : fundamental nz in L \n");
  Std(BText("anz = ")+common_->anz+ " : nonzeros in tril(A) if A is symmetric/lower,"
			                            " triu(A) if symmetric/upper, or tril(A*A') if "
			                            " unsymmetric, in last call to cholmod_analyze.\n");
  Std(BText("modfl = ")+common_->modfl+ " : flop count from most recent update/downdate/"
			                                " rowadd/rowdel (excluding flops to modify the "
			                                " solution to Lx=b, if computed)\n");
  Std(BText("malloc_count = ")+(double)common_->malloc_count+ " : # of objects malloc'ed minus the # free'd \n");
  Std(BText("memory_usage = ")+(double)common_->memory_usage+ " : peak memory usage in bytes \n");
  Std(BText("memory_inuse = ")+(double)common_->memory_inuse+ " : current memory usage in bytes \n");
  Std(BText("nrealloc_col = ")+common_->nrealloc_col+ " : # of column reallocations \n");
  Std(BText("nrealloc_factor = ")+common_->lnz+ " : # of factor reallocations due to col. reallocs  \n");
  Std(BText("ndbounds_hit = ")+common_->lnz+ " : # of times diagonal modified by dbound \n");
  Std(BText("rowfacfl = ")+common_->rowfacfl+ " : # of flops in last call to cholmod_rowfac \n");
  Std(BText("aatfl = ")+common_->lnz+ " : # of flops to compute A(:,f)*A(:,f)' \n");
}

////////////////////////////////////////////////////////////////////////////////
  int BVMat::Check()const
//Importing method
////////////////////////////////////////////////////////////////////////////////
{
  UStore& s = (UStore&)s_;
  int result = false;
  switch(code_)  {
  case(ESC_blasRdense  ):
    result=cholmod_check_dense  (s.blasRdense_,  common_);
    break;
  case(ESC_chlmRsparse ):
    result=cholmod_check_sparse (s.chlmRsparse_, common_);
    break;
  case(ESC_chlmRfactor ):
    result=cholmod_check_factor (s.chlmRfactor_, common_);
    break;
  case(ESC_chlmRtriplet):
    result=cholmod_check_triplet(s.chlmRtriplet_,common_);
    break; 
  default: ; 
    result = -1;
    Warning("Cannot Check undefined virtual matrix"); }
  return(result);
}


////////////////////////////////////////////////////////////////////////////////
  void BVMat::SetUnsymmetric()
////////////////////////////////////////////////////////////////////////////////
{
  if(code_ == ESC_blasRdense)
  {
    cholmod_R_sparse* old = s_.chlmRsparse_;
    s_.chlmRsparse_ = CholmodCopy(old, 0, 1, common_);
    CholmodFree_sparse(&old, common_);
  }
}

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Pack()
////////////////////////////////////////////////////////////////////////////////
{
  int nz,ncol,nrow,dense_size,sparse_size;
  BVMat aux;
  switch(code_) {
  case(ESC_blasRdense  ) : 
  {
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    dense_size = bytes_blasRdense(nrow*ncol);
    sparse_size = bytes_chlmRsparse(nz, ncol, true);
    if(sparse_size<dense_size)
    {
      aux.Convert(*this,ESC_chlmRsparse);
      *this = aux;
    }
    break;
  }
  case(ESC_chlmRsparse ) : 
  {
    cholmod_drop(0,s_.chlmRsparse_, common_);
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    dense_size = bytes_blasRdense(nrow*ncol);
    sparse_size = bytes_chlmRsparse(nz, ncol, true);
    if(sparse_size>dense_size)
    {
      aux.Convert(*this,ESC_blasRdense);
      *this = aux;
    }
    else
    {
      CholmodSort(s_.chlmRsparse_,   common_);
    }
    break;
  }
  case(ESC_chlmRtriplet) : 
  {
    Drop(0);
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    dense_size = bytes_blasRdense(nrow*ncol);
    sparse_size = bytes_chlmRsparse(nz, ncol, true);
    if(sparse_size<dense_size)
    {
      aux.Convert(*this,ESC_chlmRsparse);
    }
    else
    {
      aux.Convert(*this,ESC_blasRdense);
    }
    *this = aux;
    break; 
  }
  case(ESC_chlmRfactor ) : 
  {
    CholmodPackFactor(s_.chlmRfactor_,common_);
    break;
  }
  default: 
    Error(I2("FATAL ERROR in",
             "ERROR FATAL en")+" BVMat::Pack()"); 
    assert(1); }
};

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Pack(double sparsity)
//Importing method
////////////////////////////////////////////////////////////////////////////////
{
  if(sparsity==BDat::Nan()) { Pack(); return; }
  int nz,ncol,nrow;
  double sparseRatio;
  BVMat aux;
  switch(code_) {
  case(ESC_blasRdense  ) : 
  {
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    sparseRatio = 1.0 - double(nz)/double(nrow*ncol);
    if(sparseRatio>sparsity)
    {
      aux.Convert(*this,ESC_chlmRsparse);
      *this = aux;
    }
    break;
  }
  case(ESC_chlmRsparse ) : 
  {
    cholmod_drop(0,s_.chlmRsparse_, common_);
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    sparseRatio = 1.0 - double(nz)/double(nrow*ncol);
    if(sparseRatio<sparsity)
    {
      aux.Convert(*this,ESC_blasRdense);
      *this = aux;
    }
    else
    {
      CholmodSort(s_.chlmRsparse_,   common_);
    }
    break;
  }
  case(ESC_chlmRtriplet) : 
  {
    Drop(0);
    nz = NonNullCells(0);
    ncol=Columns();
    nrow=Rows();
    sparseRatio = 1.0 - double(nz)/double(nrow*ncol);
    if(sparseRatio>=sparsity)
    {
      aux.Convert(*this,ESC_chlmRsparse);
    }
    else
    {
      aux.Convert(*this,ESC_blasRdense);
    }
    *this = aux;
    break; 
  }
  case(ESC_chlmRfactor ) : 
  {
    CholmodPackFactor(s_.chlmRfactor_,common_);
    break;
  }
  default: 
    Error(I2("FATAL ERROR in",
             "ERROR FATAL en")+" BVMat::Pack()"); 
    assert(1); }
};

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Copy(const BVMat& v)
//Importing method
////////////////////////////////////////////////////////////////////////////////
{
//Std(BText("TRACE BVMat::Copy ")<<v.Info());
  if(&v==this) { return; }
  Delete();
  code_ = v.code_;
  if(!v.s_.undefined_) { return; }
  switch(code_) {
  case(ESC_blasRdense  ) : 
    s_.blasRdense_   = CholmodCopy_dense  (v.s_.blasRdense_,   common_); break;
  case(ESC_chlmRsparse ) : 
    s_.chlmRsparse_  = CholmodCopy_sparse (v.s_.chlmRsparse_,  common_); break;
  case(ESC_chlmRfactor ) : 
    s_.chlmRfactor_  = CholmodCopy_factor (v.s_.chlmRfactor_,  common_); break;
  case(ESC_chlmRtriplet) : 
    s_.chlmRtriplet_ = CholmodCopy_triplet(v.s_.chlmRtriplet_, common_); break; 
  default: 
    Error(I2("FATAL ERROR in",
             "ERROR FATAL en")+" BVMat::Copy(const BVMat& v)"); 
    assert(1); }
};

////////////////////////////////////////////////////////////////////////////////
  bool BVMat::CheckDefined(const char* fName) const
////////////////////////////////////////////////////////////////////////////////
{
  if((code_!=ESC_undefined)&&(!s_.undefined_))
  {
    err_wrong_defined(fName,*this);
    return(false);
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
  void BVMat::BlasRDense(int nrow, int ncol)
////////////////////////////////////////////////////////////////////////////////
{
  Delete();
  code_ = ESC_blasRdense;
  s_.blasRdense_ = 
    CholmodAllocate_dense(nrow, ncol, nrow, CHOLMOD_REAL, common_);
  CheckDefined("BlasRDense");
}  
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::BlasRDense(int nrow, int ncol, double value)
////////////////////////////////////////////////////////////////////////////////
{
  BlasRDense(nrow, ncol);
  if(!s_.blasRdense_) { return; }
  double* x = (double*)s_.blasRdense_->x;
  if(x)
  {
    int s = nrow*ncol;
    int k;
    for(k=0; k<s; k++, x++)
    {
     *x = value;
    }
  }
  CheckDefined("BlasRDense");
}  

////////////////////////////////////////////////////////////////////////////////
  void BVMat::BlasRDense(int nrow, int ncol, const double* ptr)
////////////////////////////////////////////////////////////////////////////////
{
  BlasRDense(nrow, ncol);
  if(!s_.blasRdense_) { return; }
  double* x = (double*)s_.blasRdense_->x;
  memcpy(x, ptr, nrow*ncol*sizeof(double));
  CheckDefined("BlasRDense");
}  

////////////////////////////////////////////////////////////////////////////////
  void BVMat::ChlmRSparse(int nrow, int ncol, int nzmax)
////////////////////////////////////////////////////////////////////////////////
{
  Delete();
  code_ = ESC_chlmRsparse;
  s_.chlmRsparse_ = 
    CholmodAllocate_sparse(nrow,ncol,nzmax,true,true,0,CHOLMOD_REAL,common_);
  CheckDefined("ChlmRSparse");
}  
  
////////////////////////////////////////////////////////////////////////////////
  void BVMat::ChlmRTriplet(int nrow, int ncol, int nzmax)
////////////////////////////////////////////////////////////////////////////////
{
  Delete();
  code_ = ESC_chlmRtriplet;
  s_.chlmRtriplet_ = 
    CholmodAllocate_triplet(nrow, ncol, nzmax, 0, CHOLMOD_REAL, common_);
  CheckDefined("ChlmRTriplet");
}  
  

////////////////////////////////////////////////////////////////////////////////
//Dimensions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
  int BVMat::Rows() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("Rows")) { return(0); }
  switch(code_) {
  case(ESC_blasRdense  ) : return(s_.blasRdense_  ->nrow);
  case(ESC_chlmRsparse ) : return(s_.chlmRsparse_ ->nrow);
  case(ESC_chlmRfactor ) : return(s_.chlmRfactor_ ->n   );
  case(ESC_chlmRtriplet) : return(s_.chlmRtriplet_->nrow);
  default : return(0); }
};
  
////////////////////////////////////////////////////////////////////////////////
  int BVMat::Columns() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("Columns")) { return(0); }
  switch(code_) {
  case(ESC_blasRdense  ) : return(s_.blasRdense_  ->ncol);
  case(ESC_chlmRsparse ) : return(s_.chlmRsparse_ ->ncol);
  case(ESC_chlmRfactor ) : return(s_.chlmRfactor_ ->n   );
  case(ESC_chlmRtriplet) : return(s_.chlmRtriplet_->ncol);
  default : return(0); }
};

//--------------------------------------------------------------------
static int intCmp_(const void* v1, const void* v2)

/*! Compare two integers.
 */
//--------------------------------------------------------------------
{
  int& i1 = *((int*)v1);
  int& i2 = *((int*)v2);
  return((i1<i2)?-1:((i1>i2)?1:0));
}

////////////////////////////////////////////////////////////////////////////////
  double* BVMat::bRd_checkCell(int i, int j, int& accessCode) const
////////////////////////////////////////////////////////////////////////////////
{
  static double nan_ = BDat::Nan();
  double* x;
  if((i<0)||(i>=s_.blasRdense_->nrow)||
     (j<0)||(j>=s_.blasRdense_->ncol))
  {
    accessCode=-1;
    nan_ = BDat::Nan();
    x=&nan_;
  }
  else
  {
    accessCode=0;
    x = &((double*)s_.blasRdense_->x)[j*s_.blasRdense_->nrow+i];
  }
  return(x);
}

////////////////////////////////////////////////////////////////////////////////
  double* BVMat::cRs_checkCell(int i, int j, int& accessCode) const
////////////////////////////////////////////////////////////////////////////////
{
  static double nan_  = BDat::Nan();
  static double zero_ = 0.0;
  double* x;
  int* found;
  int k,k0,k1;
  int* i0;
  if((i<0)||(i>=s_.chlmRsparse_->nrow)||
     (j<0)||(j>=s_.chlmRsparse_->ncol))
  {
    accessCode=-1;
    nan_ = BDat::Nan();
    x=&nan_;
  }
  else
  {
    cRs_ensure_packed(s_.chlmRsparse_);
    k0 = ((int*)s_.chlmRsparse_->p)[j];
    k1 = ((int*)s_.chlmRsparse_->p)[j+1];
    i0 = ((int*)s_.chlmRsparse_->i)+k0;
    if(k0<k1)
    {
      found = (int*) bsearch(&i, i0, k1-k0, sizeof(int), intCmp_);
    }
    else
    {
      found = NULL;
    }
    if(!found)
    {
      accessCode=1;
      zero_ = 0.0;
      x = &zero_;
    }
    else
    {
      accessCode=0;
      k = found-i0;
      x = &((double*)s_.chlmRsparse_->x)[k0+k];
    }
  }
  return(x);
};

////////////////////////////////////////////////////////////////////////////////
  double* BVMat::checkCell(int i, int j,
                           int& accessCode, bool readOnly,
                           const char* fName, bool showMsg) const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined(fName)) { return(NULL); }
  double* x=NULL;
  switch(code_) {
  case(ESC_blasRdense  ) : x = bRd_checkCell(i,j,accessCode); break;
  case(ESC_chlmRsparse ) : x = cRs_checkCell(i,j,accessCode); break;
  default : accessCode = -2; }
  if(showMsg)
  {
    if(accessCode == -2)
    {
      err_invalid_subtype(fName,*this); 
    }
    else if(accessCode == -1)
    {
      err_cannot_apply(fName,
                       I2("(non existant cell",
                          "(celda no existente")+" ["+i+","+j+"])",*this); 
    }
    else if((accessCode==1)&&!readOnly)
    {
      warn_convert2cRt(BText("PutVMatDat")+
                       I2(" of non stored cell",
                          " de celda no almacenada")+" ["+i+","+j+"]",*this); 
    }
  }
  return(x);
}
  
////////////////////////////////////////////////////////////////////////////////
  double BVMat::GetCell(int i, int j) const
////////////////////////////////////////////////////////////////////////////////
{
  static char* name = "GetCell";
  int accessCode;
  double* y = BVMat::checkCell(i, j, accessCode, true, name, true);
  if(y) { return(*y); }
  else  { return(BDat::Nan()); }
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::PutCell(int i, int j, const double& x)
////////////////////////////////////////////////////////////////////////////////
{
  static char* name = "PutCell";
  int accessCode;
  double* y = BVMat::checkCell(i, j, accessCode, false, name, true);
  if((accessCode==1)&&(x!=0.0))
  {
    BVMat tr(*this,ESC_chlmRtriplet);
    int n = tr.s_.chlmRtriplet_->nzmax+1;
    CholmodReallocateTriplet(n, tr.s_.chlmRtriplet_, common_);
    int*    i_=(int*)   tr.s_.chlmRtriplet_->i;
    int*    j_=(int*)   tr.s_.chlmRtriplet_->j;
    double* x_=(double*)tr.s_.chlmRtriplet_->x;
    i_[n-1]=i;
    j_[n-1]=j;
    x_[n-1]=x;
    tr.s_.chlmRtriplet_->nnz++;
    Convert(tr,ESC_chlmRsparse);
  }
  else if(accessCode==0)
  {
    *y = x;
  }
  return(accessCode);
}

////////////////////////////////////////////////////////////////////////////////
  int BVMat::PutCells(const BMatrix<double>& triplet)
////////////////////////////////////////////////////////////////////////////////
{
  static char* name = "PutCells";
  int r = triplet.Rows();
  int c = triplet.Columns();
  int rows = Rows();
  int cols = Columns();
  if(c!=3)
  {
    Error(BText(name)+" Second argument must be a triplet matrix (i,j,x)"); 
    return(0);
  }
  int i,j,k;
  if(code_==ESC_blasRdense)
  {
    const double* t = triplet.Data().Buffer();
    double* d = (double*)s_.blasRdense_->x;
    for(k=0; k<r; k++)
    { 
      i = (int)(*(t++))-1;
      j = (int)(*(t++))-1;
      if((i>=0)&&(i<rows)&&(j>=0)&&(j<cols))
      {
        d[j*rows+i] = *(t++);
      }
      else
      {
        Error(BText(name)+" Cannot access to cell ("+i+","+j+")"); 
        return(0);
      }
    }
  }
/*
  else if((code_==ESC_chlmRtriplet)||(code_==ESC_chlmRsparse))
  {
    int accessCode;
    BVMat* T = this;
    if(code_==ESC_chlmRsparse)
    {
      T = new BVMat(*this,ESC_chlmRtriplet);
    }
    int nnz = T->s_.chlmRtriplet_->nnz;
    int nnzr = nnz+r;
    CholmodReallocateTriplet(nnzr, T->s_.chlmRtriplet_, common_);
    int*    i_=(int*)   tr.s_.chlmRtriplet_->i;
    int*    j_=(int*)   tr.s_.chlmRtriplet_->j;
    double* x_=(double*)tr.s_.chlmRtriplet_->x;

    for(k=0; k<r; k++)
    { 
      i = (int)triplet(k,0)-1;
      j = (int)triplet(k,1)-1;
      if((i>=0)&&(i<rows)&&(j>=0)&&(j<cols))
      {
        int h = nnz+k;
        i_[h] = i;
        j_[h] = j;
        x_[h] = triplet(k,2);
        T->s_.chlmRtriplet_->nnz++;
      }
      else
      {
        Error(BText(name)+" Cannot access to cell ("+i+","+j+")"); 
        return(0);
      }
    }
    if(code_==ESC_chlmRsparse)
    {
      Convert(*T,ESC_chlmRsparse);
        
    }

  }
*/
  else 
  {
    Error(BText(name)+" is implemented only for dense matrices"); 
    return(0);
  }

  return(1);
}


////////////////////////////////////////////////////////////////////////////////
  int BVMat::bRd_bRd_PutBlock(int i0, int j0, const BVMat& x_, int& accessCode)
////////////////////////////////////////////////////////////////////////////////
{
  int rx=x_.Rows();
  int cx=x_.Columns();
  int r=Rows();
  int c=Columns();
  int ix, jx;
  double* y;
  double* x;
  for(jx=0; jx<cx; jx++)
  {
    y = ((double*)   s_.blasRdense_->x) + (r  * (jx+j0)) + i0;
    x = ((double*)x_.s_.blasRdense_->x) + (rx *  jx    );
    for(ix=0; ix<rx; ix++)
    {
      *y = *x;
      y++;
      x++;
    }
  }
  return(accessCode);
};

////////////////////////////////////////////////////////////////////////////////
  int BVMat::cRs_bRd_PutBlock(int i0, int j0, const BVMat& x_, int& accessCode)
////////////////////////////////////////////////////////////////////////////////
{
  int rx=x_.Rows();
  int cx=x_.Columns();
  int r=Rows();
  int c=Columns();
  double* y;
  double* x;
  int i, k, jx, k0y, k1y, ky, jy;
  int *i0y, *iy;
  cRs_ensure_packed(  s_.chlmRsparse_);
  int nfk = 0;
  BArray<int>    nfi(rx*cx);
  BArray<int>    nfj(rx*cx);
  BArray<double> nfx(rx*cx);
  for(jx=0; (jx<cx); jx++)
  {
    jy = j0+jx;
    x = ((double*)x_.s_.blasRdense_->x) + (rx*jx);
    k0y = ((int*)s_.chlmRsparse_->p)[jy];
    k1y = ((int*)s_.chlmRsparse_->p)[jy+1];
    i0y = ((int*)s_.chlmRsparse_->i)+k0y;
    if(k0y<k1y)
    {
      iy = (int*) bsearch(&i0,i0y, k1y-k0y, sizeof(int), intCmp_);
    }
    ky = k0y+(iy-i0y);
    y = ((double*)s_.chlmRsparse_->x)+ky;
    for(k=0; k<rx; k++, ky++, y++)
    {
      if(!iy || ( (i = *iy-i0) !=k))
      {
        nfi[nfk]=i0+k;
        nfj[nfk]=jy;
        nfx[nfk]=x[k];
        nfk++;
      }
      else
      {
        *y = x[k];
      }
      if(iy) 
      { 
        assert((ky<k1y)&&(*iy<i0+rx));
        iy++; 
      }
    }
  }
  if(nfk)
  {
#ifndef NDEBUG   
    warn_cannot_apply("PutVMatBlock",
      I2("(There are "+nfk+" non stored cells of sparse "
         "matrix that will be be modified)",
         "(Existen "+nfk+" celdas no almacenadas de "
         "la matriz sparse que serán modificadas)"),
      *this); 
#endif
    cholmod_R_sparse* unsym = CholmodCopy(s_.chlmRsparse_, 0, 1, common_);
    cholmod_R_triplet* tr_old = 
      CholmodSparseToTriplet(unsym, common_);
    CholmodFree_sparse(&unsym, common_); 
    cholmod_R_triplet* tr_new = 
      CholmodAllocate_triplet(r, c, tr_old->nzmax+nfk, 0, CHOLMOD_REAL, common_);
    tr_new->nnz = tr_new->nzmax;
    int*    i_old = (int*)   tr_old->i;
    int*    j_old = (int*)   tr_old->j;
    double* x_old = (double*)tr_old->x;
    int*    i_new = (int*)   tr_new->i;
    int*    j_new = (int*)   tr_new->j;
    double* x_new = (double*)tr_new->x;
    int c=0;
    for(k=0; k<tr_old->nzmax; k++)
    {
      i_new[c] = i_old[k];
      j_new[c] = j_old[k];
      x_new[c] = x_old[k];
      c++;
    }
    for(k=0; k<nfk; k++)
    {
      i_new[c] = nfi[k];
      j_new[c] = nfj[k];
      x_new[c] = nfx[k];
      c++;
    }
    CholmodFree_sparse (&s_.chlmRsparse_, common_);
    s_.chlmRsparse_ = CholmodTripletToSparse
    (tr_new, tr_new->nnz, common_);
    CholmodFree_triplet (&tr_old, common_);
    CholmodFree_triplet (&tr_new, common_);
  }
  return(accessCode);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::PutBlock(int i0, int j0, const BVMat& x)
////////////////////////////////////////////////////////////////////////////////
{
  static char* fName = "PutVMatBlock";
  int accessCode = false;
  int r=Rows();
  int c=Columns();
  int rx=x.Rows();
  int cx=x.Columns();
  if((i0<0)||(i0+rx>r)||(j0<0)||(j0+cx>c))
  {
    accessCode = -1;
    err_cannot_apply(fName,
      I2("(position ["+(i0+1)+","+(j0+1)+"] and block dimensions ["+rx+","+cx+"] are "
         "not compatible with matrix ["+r+","+c+"] to be modified)",
         "(la posición ["+(i0+1)+","+(j0+1)+"] y las dimensiones ["+rx+","+cx+"] del "
         "bloque no son compatibles con la matriz ["+r+","+c+"] a modificar)"),
      *this); 
    return(accessCode);
  }
  if((code_==ESC_blasRdense) && (x.code_==ESC_blasRdense))
  {
    bRd_bRd_PutBlock(i0,j0,x,accessCode);
  }
  else if((code_==ESC_chlmRsparse) && (x.code_==ESC_blasRdense))
  {
    cRs_bRd_PutBlock(i0,j0,x,accessCode);
  }
  else
  {
    err_invalid_subtypes(fName, *this, x);
    accessCode = -2;
  }
  return(accessCode);
}

////////////////////////////////////////////////////////////////////////////////
  int BVMat::StoredCells() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("StoredCells")) { return(0); }
  int nzmax = 0;
  switch(code_) {
  case(ESC_blasRdense  ) :
    nzmax = (int)(s_.blasRdense_->nrow * 
                  s_.blasRdense_->ncol); 
    break;
  case(ESC_chlmRsparse ) :
    nzmax = (int)(s_.chlmRsparse_->nzmax); 
    break;
  case(ESC_chlmRfactor ) :
    if(!s_.chlmRfactor_->is_super) { nzmax = (int)(s_.chlmRfactor_->nzmax); }
    else                           { nzmax = (int)(s_.chlmRfactor_->xsize); }
    break;
  case(ESC_chlmRtriplet) :
    nzmax = (int)(s_.chlmRtriplet_->nzmax); 
    break;
  default : ;}
  return(nzmax);
};

////////////////////////////////////////////////////////////////////////////////
  bool BVMat::StoredData(const double *&x, int &nzmax) const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("StoredCells")) { return(false); }
  nzmax = 0;
  x = NULL;
  bool ok = true;
  switch(code_) {
  case(ESC_blasRdense  ) :
    nzmax = (int)(s_.blasRdense_->nrow * 
                  s_.blasRdense_->ncol); 
    x = (const double*)s_.blasRdense_->x;
    break;
  case(ESC_chlmRsparse ) :
    nzmax = (int)(s_.chlmRsparse_->nzmax); 
    x = (const double*)s_.chlmRsparse_->x;
    break;
  case(ESC_chlmRfactor ) :
    if(!s_.chlmRfactor_->is_super) { nzmax = (int)(s_.chlmRfactor_->nzmax); }
    else                           { nzmax = (int)(s_.chlmRfactor_->xsize); }
    x = (const double*)s_.chlmRfactor_->x;
    break;
  case(ESC_chlmRtriplet) :
    nzmax = (int)(s_.chlmRtriplet_->nzmax); 
    x = (const double*)s_.chlmRtriplet_->x;
    break;
  default : ok = false; }
  return(ok);
};

////////////////////////////////////////////////////////////////////////////////
  bool BVMat::StoredData(double *&x, int &nzmax)
////////////////////////////////////////////////////////////////////////////////
{
  const double** y = (const double**)&x;
  return(StoredData(*y, nzmax));
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::Enlarge(int nrow, int ncol, int row0, int col0)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  static char* fName = "Enlarge";
  int result = 0;
  int *i, *j, k;
  switch(code_) {
  case(ESC_chlmRtriplet ) :
    s_.chlmRtriplet_->nrow = nrow;
    s_.chlmRtriplet_->ncol = ncol;
    i = (int*)s_.chlmRtriplet_->i;
    j = (int*)s_.chlmRtriplet_->j;
    for(k=0; k<s_.chlmRtriplet_->nzmax; k++)
    {
      i[k] += row0;
      j[k] += col0;
    }
    break;  
  default: 
    err_invalid_subtype(fName,*this); 
    result = -3; }
  return(result);
}


////////////////////////////////////////////////////////////////////////////////
  int BVMat::MergeRows(int nrow, BSet& items)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  static char* fName = "MergeRows";
  int result = 0;
  int *i, *j, *rIdx, r, k, K, n, N = items.Card();
  Delete();
  int nnz = 0;
  int nzmax = 0;
  const double* cx;
  double *x;
  BArray<BVMat*> a (N); 
  BArray<BVMat*> a_(N); 
  BArray< BArray<int> > rowIndex(N);
  int ncol=0;
  for(n=0; n<N; n++)
  {
    a[n] = a_[n] = NULL;
    if(items[n+1]->Grammar()!=GraSet())
    {
      err_cannot_create(fName, 
        BText("Item number ")+(n+1)+" is not a Set");
      result = -1;
      continue;
    }
    BSet& item = Set(items[n+1]);
    if(item[1]->Grammar()!=GraVMatrix())
    {
      err_cannot_create(fName, 
        BText("First element of item number ")+(n+1)+" is not a VMatrix");
      result = -2;
      continue;
    }
    a[n] = &VMat(item[1]);
    assert(a[n]->Check());
    if(item[2]->Grammar()!=GraMatrix())
    {
      a[n] = NULL;
      err_cannot_create(fName, 
        BText("Second element of item number ")+(n+1)+" is not a Matrix");
      result = -3;
      continue;
    }
    if(a[n]->code_ == ESC_undefined) 
    {
      err_cannot_create(fName, 
        BText("Item number ")+(n+1)+" has undefined subtype");
      result = -7;
      continue;
    }
    if(a[n]->code_ == ESC_chlmRtriplet) 
    {
      a_[n] = a[n];
    }
    else
    { 
      a_[n] = new BVMat;
      a_[n]->Convert(*a[n], ESC_chlmRtriplet);
    }
    if(!n) { ncol = a_[n]->s_.chlmRtriplet_->ncol; }
    else if (ncol != a_[n]->s_.chlmRtriplet_->ncol)
    {
      err_cannot_create(fName, 
        BText("Number of columns (")+ncol+") is not equal to previous one ("+
        int(a_[n]->s_.chlmRtriplet_->ncol)+") for item number "+(n+1));
      result = -5;
      continue;
    }

    BMatrix<double>& rowIdx = (BMatrix<double>&)Mat(item[2]);
    r = a_[n]->s_.chlmRtriplet_->nrow;
    if(rowIdx.Data().Size()!=r)
    {
      err_cannot_create(fName, 
        BText("Number of rows (")+rowIdx.Data().Size()+
         ") is not equal to card of row indexes ("+r+") at item number "+(n+1));
      result = -6;
      continue;
    }
    if(a_[n]->s_.chlmRtriplet_->nnz)
    {
      K = a_[n]->s_.chlmRtriplet_->nzmax;
      i = (int*)a_[n]->s_.chlmRtriplet_->i;
      rowIndex[n].AllocBuffer(K);
      rIdx = rowIndex[n].GetBuffer();
      cx = rowIdx.Data().Buffer();
      for(k=0; k<K; k++, i++, rIdx++) 
      { 
        assert((*i>=0)&&(*i<r));
        *rIdx = (int)cx[*i]-1; 
        if((*rIdx<0)||(*rIdx>=nrow))
        {
          err_cannot_create(fName, 
            BText("Invalid row index ")+((*rIdx)+1)+" out of range [1,"+nrow+"]");
          result = -7;
          break;
        }
      }
      nnz   += a_[n]->s_.chlmRtriplet_->nnz;
      nzmax += K;
    }
    else
    {
      delete a_[n];
      a[n] = a_[n] = NULL;
    }
  }
  if(result!=0)
  {
    for(n=0; n<N; n++)
    {
      if(a_[n] && (a_[n]!=a[n])) 
      { delete a_[n]; }
    }
  }
  else if(N)
  {
    assert(ncol);
    ChlmRTriplet(nrow, ncol, nzmax);
    s_.chlmRtriplet_->nnz = nnz;
    i = (int*)s_.chlmRtriplet_->i;
    j = (int*)s_.chlmRtriplet_->j;
    x = (double*)s_.chlmRtriplet_->x;
    for(n=0; n<N; n++)
    {
      if(a_[n])
      {
        K = a_[n]->s_.chlmRtriplet_->nzmax;
        memcpy(i, rowIndex[n].Buffer(),       K*sizeof(int));
        memcpy(j, a_[n]->s_.chlmRtriplet_->j, K*sizeof(int));
        memcpy(x, a_[n]->s_.chlmRtriplet_->x, K*sizeof(double));
        i += K;
        j += K;
        x += K;
        if(a_[n]!=a[n])
        { delete a_[n]; }
      }
    }
  }
  return(result);
}


////////////////////////////////////////////////////////////////////////////////
  int BVMat::NonNullCells(double chop) const
////////////////////////////////////////////////////////////////////////////////
{
  int i, nnz = 0, nzmax = 0;
  const double* x = NULL;
  if(!StoredData(x, nzmax)) { return(-1); }
  if(!x) { return(-1); }
  for(i=0; i<nzmax; i++)
  {
    nnz += fabs(x[i])>chop;
  }
  return(nnz);
};
  
////////////////////////////////////////////////////////////////////////////////
bool BVMat::IsDiagonal() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("IsDiagonal")) { return(false); }
  int i, j, k;
  int* r;
  int* c;
  int* p;
  double* x;
  switch(code_) {
  case(ESC_blasRdense  ) :
    x = (double*)s_.blasRdense_->x;
    for(j=k=0; j<s_.blasRdense_->ncol; j++)
    {
      for(i=0; i<s_.blasRdense_->nrow; i++, k++)
      {
        if((i!=j) && (x[k]!=0)) { return(false); }
      }
    }
    break;
  case(ESC_chlmRsparse ) :
    cRs_ensure_packed(s_.chlmRsparse_);
    r = (int*)s_.chlmRsparse_->i;
    p = (int*)s_.chlmRsparse_->p;
    x = (double*)s_.chlmRsparse_->x;
    for(j=0; j<s_.chlmRsparse_->ncol; j++)
    {
      for(k=p[j]; k<p[j+1]; k++)
      {
        i = r[k];
        if((i!=j)&&(x[k]!=0.0)) { return(false); }
      }
    }
    break;
  case(ESC_chlmRfactor ) :
    if(s_.chlmRfactor_->nzmax > s_.chlmRfactor_->n) { return(false); }
    break;
  case(ESC_chlmRtriplet) :
    r = (int*)s_.chlmRtriplet_->i;
    c = (int*)s_.chlmRtriplet_->j;
    x = (double*)s_.chlmRtriplet_->x;
    for(k=0; k<s_.chlmRtriplet_->nzmax; k++)
    {
      if((r[k]!=c[k])&&(x[k]!=0.0)) { return(false); }
    }
    break;
  default : err_invalid_subtype("IsDiagonal",*this); }
  return(true);
};  

////////////////////////////////////////////////////////////////////////////////
bool BVMat::IsSymmetric() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("IsSymmetric")) { return(false); }
  if((code_==ESC_chlmRsparse )&&(s_.chlmRsparse_ ->stype!=0)) { return(true); }
  if((code_==ESC_chlmRtriplet)&&(s_.chlmRtriplet_->stype!=0)) { return(true); }
  if(code_==ESC_chlmRfactor) { return(false); }
  if(Rows()!=Columns()) { return(false); }
  BVMat t = this->T();
  BVMat z = Drop(((*this)-t),DEpsilon());
  bool isSymm = z.NonNullCells()==0;
  return(isSymm);
};  


////////////////////////////////////////////////////////////////////////////////
void BVMat::CompactSymmetric(bool check)
////////////////////////////////////////////////////////////////////////////////
{
  bool isSymm = (check)?IsSymmetric():true;
  if(isSymm)
  {
    if((code_==ESC_chlmRsparse)&&(s_.chlmRsparse_ ->stype==0))
    {
      cholmod_sparse* sp = s_.chlmRsparse_;
      s_.chlmRsparse_ = CholmodCopy(sp,-1,1,common_);
      CholmodFree_sparse(&sp, common_);
    }
    if((code_==ESC_chlmRtriplet)&&(s_.chlmRtriplet_ ->stype==0))
    {
      s_.chlmRtriplet_->stype=0;
    }
  }
};  

////////////////////////////////////////////////////////////////////////////////
  BText BVMat::Info() const
////////////////////////////////////////////////////////////////////////////////
{
  if((code_!=ESC_undefined)&&(!s_.undefined_)) { return(""); }
  BText aux=BText("Virtual matrix ")+CodeName(code_);
  if(code_==ESC_undefined) { return(aux); }
  int r = Rows();
  int c = Columns();
  double cells = StoredCells();
  double nz = NonNullCells();
  double denseCells = (double)r*(double)c;
  double bytes = Bytes();
  double denseBytes = sizeof(BVMat)+sizeof(cholmod_dense)+ 
                     (denseCells)*sizeof(double);
  double nzPercent = round(10000.0*nz/denseCells)/100.0;
  double cellPercent = round(10000.0*cells/denseCells)/100.0;
  double bytesPercent = round(10000.0*bytes/denseBytes)/100.0;
  aux +=BText("\n")+ 
        " Non zero cells "+nz+"/("+r+"x"+c+")="+nzPercent+"%\n"+
        " Stored cells "+cells+"/("+r+"x"+c+")="+cellPercent+"%\n"+
        " Stored bytes "+bytes+"/"+denseBytes+"="+bytesPercent+"%\n"; 
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  BText BVMat::Dump() const
////////////////////////////////////////////////////////////////////////////////
{
  BText aux=Info();
  return(aux);
};
  

////////////////////////////////////////////////////////////////////////////////
  BVMat& BVMat::Unknown()
////////////////////////////////////////////////////////////////////////////////
{
  static BVMat unk;
  return(unk);
};    

////////////////////////////////////////////////////////////////////////////////
  BDat BVMat::Compare(const BVMat* A, const BVMat* B) 
////////////////////////////////////////////////////////////////////////////////
{
  static BDat zero(0.0);  
  return(zero);
}

////////////////////////////////////////////////////////////////////////////////
  int BVMat::ApplyFunRR(BFunRR f, bool f00, const char* fName,
                        const BVMat& A, BVMat& B)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  int i, nzmax=0;
  double* x=NULL;
  if((A.code_==ESC_blasRdense)||f00) 
  { 
    B.Copy(A); 
  }  
  else    
  { 
    warn_convert2bRd(fName,A); 
    B.Convert(A, ESC_blasRdense);
  }
  if(B.StoredData(x, nzmax)) 
  { 
    for(i=0; i<nzmax; i++) { x[i] = (*f)(x[i]); }
  }
  if(B.code_==ESC_chlmRsparse)
  {
    cRs_ensure_packed(B.s_.chlmRsparse_);
  }
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::ApplyFunRR(BFunRR f, bool f00, const char* fName) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  ApplyFunRR(f,f00,fName,*this,aux);
  return(aux);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::ApplyFunR2r(BFunR2R f, bool f00, bool f0x, const char* fName,
                         const BVMat& A, double b, BVMat& C)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A.CheckDefined(fName)) { return(-1); }
  int i, nzmax = 0;
  double* x = NULL;
  if((A.code_==ESC_blasRdense)||(f00&&(f0x||(b==0)))) 
  {
    C.Copy(A); 
  }
  else
  {
    warn_convert2bRd(fName,A);
    C.Convert(A, ESC_blasRdense);  
  }
  if(C.StoredData(x, nzmax))
  {
    for(i=0; i<nzmax; i++) { x[i] = f(x[i], b); }
  }
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::ApplyFunR2r(BFunR2R f, bool f00, bool f0x, const char* fName,
                          double b) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  ApplyFunR2r(f,f00,f0x,fName,*this,b,aux);
  return(aux);
};


////////////////////////////////////////////////////////////////////////////////
static void add_triplet
(
  int r, int c, int n,
  int i, int j, double x, 
  int*Ti,int*Tj,double*Tx,
  int&k
)
////////////////////////////////////////////////////////////////////////////////
{
  if(x!=0.0)
  {  
    assert(k<n);
    assert((0<=i)&&(i<r));
    assert((0<=j)&&(j<c));
    Ti[k] = i;
    Tj[k] = j;
    Tx[k] = x;
    k++;
  }
}


////////////////////////////////////////////////////////////////////////////////
  int BVMat::ApplyFunR2R(BFunR2R f, bool f00, bool f0x, const char* fName,
                         const BVMat& A_, const BVMat& B_, BVMat& C)
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  if(!A_.CheckDefined(fName)) { return(-1); }
  if(!B_.CheckDefined(fName)) { return(-1); }
  BVMat::ECode Acode = A_.code_;
  BVMat::ECode Bcode = B_.code_;
  int r  = A_.Rows();
  int c  = A_.Columns();
  int rB = B_.Rows();
  int cB = B_.Columns();
  if((r!=rB)||(c!=cB))
  {
    err_invalid_dimensions(fName,A_,B_);
    return(-1);
  }
  C.Delete();
  BVMat* A__ = (BVMat*)&A_, *B__ = (BVMat*)&B_;
  convertIfNeeded_all2bRd(A_,B_,A__,B__,fName, !f00); 
  BVMat &A = *A__,  &B = *B__;
  int result = 0;
  if(A.code_!=B.code_)
  {
    err_invalid_subtypes(fName,A,B);
    result = -2;
  }
  else
  {
    int i, j, k, n, Ak, Bk, Ak1, Bk1;
    cholmod_triplet* tr;
    int *Ti, *Tj, *Ai, *Ap, *Bi, *Bp;
    double *Tx, *Ax, *Bx, *Cx;
    int nnz = 0;
    double x;  

    switch(A.code_) {
    case(ESC_blasRdense  ) :
      C.BlasRDense(r,c);
      n = A.s_.blasRdense_->nzmax;
      Ax = (double*)A.s_.blasRdense_->x;
      Bx = (double*)B.s_.blasRdense_->x;
      Cx = (double*)C.s_.blasRdense_->x;
      for(k=0; k<n; k++)
      {
        x = f(Ax[k], Bx[k]);
        if(x!=0.0) { nnz++; }
        Cx[k] = x;
      };
      if((Acode==ESC_chlmRsparse) || (Bcode==ESC_chlmRsparse) && (nnz<0.50*r*c))
      {
        BVMat C_;
        BVMat::bRd2cRs(C_,C);
        C = C_;
      }
      break;
    case(ESC_chlmRsparse) :
      assert(f00);
      C.code_ = ESC_chlmRsparse;
      cRs_ensure_packed(A.s_.chlmRsparse_);
      cRs_ensure_packed(B.s_.chlmRsparse_);
      if(f0x)
      {
        n = Minimum(A.s_.chlmRsparse_->nzmax,B.s_.chlmRsparse_->nzmax);
      }
      else
      {
        n = A.s_.chlmRsparse_->nzmax + B.s_.chlmRsparse_->nzmax;
      }
      tr=CholmodAllocate_triplet(r,c,n,0,CHOLMOD_REAL,common_);
      Ti = (int*   )tr->i;
      Tj = (int*   )tr->j;
      Tx = (double*)tr->x;
      Ap = (int*   )A.s_.chlmRsparse_->p;
      Ai = (int*   )A.s_.chlmRsparse_->i;
      Ax = (double*)A.s_.chlmRsparse_->x;
      Bp = (int*   )B.s_.chlmRsparse_->p;
      Bi = (int*   )B.s_.chlmRsparse_->i;
      Bx = (double*)B.s_.chlmRsparse_->x;
      k = 0;
      for(j=0; j<c; j++)
      {
        Ak  = Ap[j];
        Ak1 = Ap[j+1];
        Bk  = Bp[j];
        Bk1 = Bp[j+1];
        for(; (Ak<Ak1)||(Bk<Bk1); )
        {
          if(Ak==Ak1)
          {
            if(!f0x) { add_triplet(r,c,n,i=Bi[Bk],j,x=f(0.0,Bx[Bk]),Ti,Tj,Tx,k); }
            Bk++;
          }
          else if(Bk==Bk1)
          {
            if(!f0x) { add_triplet(r,c,n,i=Ai[Ak],j,x=f(Ax[Ak],0.0),Ti,Tj,Tx,k); }
            Ak++;
          }
          else if(Ai[Ak]==Bi[Bk])
          {
            add_triplet(r,c,n,i=Ai[Ak],j,x=f(Ax[Ak],Bx[Bk]),Ti,Tj,Tx,k);
            Ak++; 
            Bk++;
          }
          else if(Ai[Ak]>Bi[Bk])
          {
            if(!f0x) { add_triplet(r,c,n,i=Bi[Bk],j,x=f(0.0,Bx[Bk]),Ti,Tj,Tx,k); }
            Bk++;
          }
          else //if(Ai[Ak]<Bi[Bk])
          {
            if(!f0x) { add_triplet(r,c,n,i=Ai[Ak],j,x=f(Ax[Ak],0.0),Ti,Tj,Tx,k); }
            Ak++;
          }
        }
      }  
      tr->nnz = k;
      C.s_.chlmRsparse_ = CholmodTripletToSparse(tr, tr->nnz, common_);
      CholmodFree_triplet(&tr, common_);
      break;
    default:
      err_invalid_subtype(fName,A); 
      result=-3; }
  };
  if(A__!=&A_) { delete A__; }
  if(B__!=&B_) { delete B__; }
  return(result);
};

////////////////////////////////////////////////////////////////////////////////
  BVMat BVMat::ApplyFunR2R(BFunR2R f, bool f00, bool f0x, const char* fName,
                           const BVMat& B) const
//Matrix algebra operator
////////////////////////////////////////////////////////////////////////////////
{
  BVMat aux;
  ApplyFunR2R(f,f00,f0x,fName,*this,B,aux);
  return(aux);
};

////////////////////////////////////////////////////////////////////////////////
  void BVMat::BinGroup(BBinFun f, BVMat** arr, int num, BVMat& C)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
       if(num==0) { return; }
  else if(num==1) { C = *arr[0]; }
  else if(num==2) { f(*arr[0],*arr[1],C); }
  else
  {
    f(*arr[0],*arr[1],C);         
    BVMat A;
    for(int i=2; i<num; i++)
    {
      A=C;
      (*f)(A,*arr[i],C);
    }
  }
}

