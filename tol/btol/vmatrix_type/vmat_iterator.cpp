/* vmat_iterator.cpp: BVMat Iterator accessing methods
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


////////////////////////////////////////////////////////////////////////////////
// BVMatColMajIter members
////////////////////////////////////////////////////////////////////////////////

double BVMatColMajIter::zero_ = 0; //To be referenced in null cells

////////////////////////////////////////////////////////////////////////////////
BVMatColMajIter::BVMatColMajIter(const BVMat& M, const char* fName)
////////////////////////////////////////////////////////////////////////////////
: M_(M), 
  fName_(fName),
  r_(M.Rows()),
  c_(M.Columns()),
  i_(0),
  j_(0),
  cell_(NULL)
{
  switch(M_.code_) {
  case(BVMat::ESC_blasRdense  ) : 
    cell_ = ((double*)M_.s_.blasRdense_->x);
    break;
  case(BVMat::ESC_chlmRsparse ) : 
    BVMat::cRs_ensure_packed(M_.s_.chlmRsparse_);
    cell_ = ((double*)M_.s_.chlmRsparse_->x);
    break;
  default : cell_=NULL; }
  if(!cell_)
  {
    BVMat::err_cannot_apply(fName_, 
      "Column major iteratator not allowed.", M_); 
  }
}

////////////////////////////////////////////////////////////////////////////////
double BVMatColMajIter::GetCell()
////////////////////////////////////////////////////////////////////////////////
{
  if(!cell_) 
  { 
    BVMat::err_cannot_apply(fName_, 
      " (Column major iteratator invalid access in method GetCell).", M_); 
    return(BDat::Nan()); 
  }
  else
  {
    return(*cell_);
  }
}

////////////////////////////////////////////////////////////////////////////////
bool BVMatColMajIter::cRs_SetCell()
////////////////////////////////////////////////////////////////////////////////
{
  if(k0_>=k1_) 
  {
    cell_ = &zero_; 
    return(true);
  }
  if((k_<k0_)||(k_>k1_))
  {
    BVMat::err_cannot_apply(fName_, 
      " (Column major iteratator invalid access in method cRs_SetCell).", M_); 
    return(false);
  }
  if(i_nz_>i_) 
  { 
    cell_ = &zero_; 
  }
  else
  { 
    cell_ = &((double*)M_.s_.chlmRsparse_->x)[k_]; 
    k_++;
    if(k_<k1_)
    {
      i_nz_ = ((int*)M_.s_.chlmRsparse_->i)[k_];
    }
    else
    {
      i_nz_ = r_;
    }
  }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
bool BVMatColMajIter::GoToCol(int j)
////////////////////////////////////////////////////////////////////////////////
{
  j_=j;
  i_=0;
  if((j_<0) || (j_>=c_)) 
  { 
    BVMat::err_cannot_apply(fName_, 
      " (Column major iterator invalid access in method GoToCol.", M_); 
    return(false);
  }
  switch(M_.code_) {
  case(BVMat::ESC_blasRdense  ) : 
    cell_ = ((double*)M_.s_.blasRdense_->x)+j_*r_;
    break;
  case(BVMat::ESC_chlmRsparse ) : 
    k0_   = ((int*)M_.s_.chlmRsparse_->p)[j];
    k1_   = ((int*)M_.s_.chlmRsparse_->p)[j+1];
    k_    = k0_;
    i_nz_ = ((int*)M_.s_.chlmRsparse_->i)[k_];
    cRs_SetCell();
    break;
  default : return(false); }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
bool BVMatColMajIter::NextCol()
////////////////////////////////////////////////////////////////////////////////
{
  return(GoToCol(j_+1));
}

////////////////////////////////////////////////////////////////////////////////
bool BVMatColMajIter::NextRow()
////////////////////////////////////////////////////////////////////////////////
{
  if((i_<0)||(i_>=r_)) 
  { 
    BVMat::err_cannot_apply(fName_, 
      " (Column major iterator invalid access in method NextRow)", M_); 
    return(false);
  }
  i_++;
  switch(M_.code_) {
  case(BVMat::ESC_blasRdense  ) : 
    cell_++;
    break;
  case(BVMat::ESC_chlmRsparse ) : 
    cRs_SetCell();
    break;
  default : return(false); }
  return(true);
}

////////////////////////////////////////////////////////////////////////////////
bool BVMatColMajIter::GetCol(int j, BArray<double>& x)
////////////////////////////////////////////////////////////////////////////////
{
  if(!GoToCol(j)) { return(false); }
  int i;
  x.AllocBuffer(r_);
  double* x_ = x.GetBuffer();
  for(i=0; i<r_; i++)
  {
    if(i) { NextRow(); }
    *x_ = *cell_;
    x_++;
  }
  return(true);
}



