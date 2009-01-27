/* vmat_iterative.cpp: BVMat iterative related methods
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

//--------------------------------------------------------------------
BVMat BVMat::MinimumResidualsSolve(const BVMat& A,
                                   const BVMat& b0,
                                   const BVMat& x0,
                                   double chop)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  BVMat x;
  int i;
  int k;
  int rows=A.Rows();
  int cols=A.Columns();
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!rows||!cols||!b0.Columns()||!x0.Columns()||
     (rows<cols)||(rows!=b0.Rows())||(cols!=x0.Rows())) 
  { 
    return(x); 
  }

  if(chop<0) { chop = sqrt(DEpsilon()/cols); }
  double tolerance = chop*chop;
  if((rows<cols)||(rows!=b0.Rows())) { return(x0); }
  x.Convert(x0,ESC_blasRdense);
  BVMat At = A.T();
  BVMat r = b0 - A*x;
  BVMat s = At*r;
  BVMat p = s;
  BVMat q;
  BVMat pa;
  BVMat rpa;
  double a;
  double b;
  double q2;
  double advance;
  double relAdv;
  double oldR;
  double R=r.FrobeniusNorm();
  BDat xNorm=x.FrobeniusNorm();
  BDat oldxNorm;
  BDat xNormAdvance;
  BDat maxpa;
  BDat oldMaxpa;
  BDat oldS;
  BDat S = s.SquaresdSum();
  BDat tol = BDat::Tolerance();

  for(k=0; k<20*cols;  k++)
  {
    if(k && !(k%cols))
    {
      Std(BText("\nBVMat::MinimumResidualsSolve ")+
        " iter="+k+
        " maxpa="+maxpa+
        " S="+S+
        " advance="+advance);
    }
    q = A*p;
    q2 = q.SquaresdSum();
    a = S.Value()/q2;
    pa = p*a;
    oldMaxpa = maxpa;
    oldR = R;
    R = r.FrobeniusNorm();
    oldxNorm = xNorm;
    x = x+pa;
    xNorm = x.FrobeniusNorm();
    xNormAdvance = xNorm-oldxNorm;

    maxpa = 0;
    for(i=0; i<cols; i++)
    {
      if(x.GetCell(i,0)!=0)
      {
        double rpa = fabs(pa.GetCell(i,0)/x.GetCell(i,0));
        if(rpa>maxpa) { maxpa = rpa; }
      }
    }

    advance = R-oldR;
    relAdv = advance/oldR;
    if(k)
    {
      if(S.IsUnknown()                    ) { break; }
      if(maxpa.IsUnknown()                ) { break; }
      if(advance             >   0        ) { break; }
      if(S                   <= tolerance ) { break; }
      if(1+maxpa-1           == 0         ) { break; }
/*
      if(maxpa               <= chop ) { break; }
      if(fabs(advance)       <= chop ) { break; }
      if(fabs(relAdv)        <= chop ) { break; }
      if(fabs(R)             <= chop ) { break; }
      if(oldMaxpa            <= tol  ) { break; }
      if(maxpa               <= tol  ) { break; }
      if(fabs(advance)       <= tol  ) { break; }
      if(xNormAdvance.Abs()  <= tol  ) { break; }
*/
    }
    r = r - q*a;
    s  = At*r;
    oldS = S;
    S  = s.SquaresdSum();
    b  = S.Value()/oldS.Value();
    p = p*b+s;
  }
  return(x);
}

//--------------------------------------------------------------------
BVMat BVMat::MinimumResidualsSolve(const BVMat& A,
                                   const BVMat& b0,
                                   double chop)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  return(BVMat::MinimumResidualsSolve(A,b0,A.T()*b0,chop));
}



