/* fftw.cpp: Selected Linear Algebra Functions.
               GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_hash_map.h>
#include <tol/tol_matrix.h>
#include <tol/tol_bvmat.h>
#include <tol/tol_fftw.h>


//--------------------------------------------------------------------
struct fftw_tol_wisdom 
//--------------------------------------------------------------------
{
  fftw_complex* in;
  fftw_complex* out;
  fftw_plan     plan;
};

typedef hash_map_by_int<fftw_tol_wisdom*>::sparse_ fftw_tol_wisdom_hash;

fftw_tol_wisdom_hash fftw_tol_wisdom_hash_;

//--------------------------------------------------------------------
fftw_tol_wisdom* fftw_tol_wisdom_find(int n, int sign)
//--------------------------------------------------------------------
{
  int sn = sign*n;
  fftw_tol_wisdom_hash::const_iterator found = fftw_tol_wisdom_hash_.find(sn);
  if(found!=fftw_tol_wisdom_hash_.end())
  {
    return(found->second);
  }
  else
  {
    fftw_tol_wisdom* wsd = new(fftw_tol_wisdom);
    fftw_tol_wisdom_hash_[sn] = wsd;
    wsd->in   = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
    wsd->out  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * n);
    wsd->plan = fftw_plan_dft_1d(n, wsd->in, wsd->out, sign, FFTW_ESTIMATE);
    return(wsd);
  }
}


//--------------------------------------------------------------------
void tol_fftw_dft_1d(
  const BMatrix<double>& in, 
  BMatrix<double>& out, 
  int sign
)
//--------------------------------------------------------------------
{
  int n = in.Data().Size() / 2;
  out.Alloc(n,2);
  fftw_tol_wisdom* wsd = fftw_tol_wisdom_find(n,sign);
  memcpy(wsd->in, in.Data().Buffer(), n*sizeof(fftw_complex));
  fftw_execute(wsd->plan);
  memcpy(out.GetData().GetBuffer(), wsd->out, n*sizeof(fftw_complex));
}

void tol_fftw_dft_1d(
  const BMatrix<BDat>& in, 
  BMatrix<BDat>& out, 
  int sign
)
{
  tol_fftw_dft_1d((const BMatrix<double>&)in, 
  (BMatrix<double>&)out, 
  sign);
}
  
