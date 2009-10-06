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
   USA. */

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bvmat_impl.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_blanguag.h>


//--------------------------------------------------------------------
BVMat BVMat::MinimumResidualsSolve(const BVMat& A,
                                   const BVMat& b0,
                                   const BVMat& x0,
                                   double chop,
                                   int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  BVMat x, r;
  int i, k;
  r.Convert(b0,ESC_blasRdense);
  if(!x0.Rows())
  {
    x = (r.T()*A).T();
  }
  else
  {
    x.Convert(x0,ESC_blasRdense);
  }
  int rows=A.Rows();
  int cols=A.Columns();
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!rows||!cols||!r.Columns()||!x.Columns()||
     (rows<cols)||(rows!=r.Rows())||(cols!=x.Rows())) 
  { 
    return(x); 
  }

  if(chop<0) { chop = sqrt(DEpsilon()/cols); }
  double tolerance = chop*chop;
  if((rows<cols)||(rows!=r.Rows())) { return(x); }
//BVMat At = A.T();
  r = r - A*x;
//BVMat s = At*r;
  BVMat s = (r.T()*A).T();
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

  for(k=0; k<maxIter;  k++)
  {
/*
    if(k && !(k%cols))
    {
      Std(BText("\nBVMat::MinimumResidualsSolve ")+
        " iter="+k+
        " maxpa="+maxpa+
        " S="+S+
        " advance="+advance);
    }
*/
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
      if(advance             >=  0        ) { break; }
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
  //s  = At*r;
    s = (r.T()*A).T();
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
                                   double chop,
                                   int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  BVMat x0;
  return(BVMat::MinimumResidualsSolve(A,b0,x0,chop,maxIter));
}


//--------------------------------------------------------------------
   static bool CheckOprProd(const BStandardOperator* opr, 
                            const BText& funName)
// Checks if opr have expected API VMatrix(VMatrix x, Real traspose)
//--------------------------------------------------------------------
{
	bool ok = opr &&
	          (opr->Grammar() == GraVMatrix()) &&
	          (opr->MinArg() == 2) &&
	          (opr->MaxArg() == 2) &&
	          (opr->GrammarForArg(1) == GraVMatrix())&&
	          (opr->GrammarForArg(2) == GraReal());
  if(!ok)
  {
    Error(I2("Invalid declaration","Declaración no válida")+": \n  "+
          opr->Grammar()->Name()+" "+opr->Identify()+opr->Arguments()+"\n"+
          I2("for linear operator used in","para el operador lineal")+" "+funName+"\n"+
          I2("Expected declaration was","La declaración esperada era")+":\n  "+
          "VMatrix"+" "+opr->Identify()+"(VMatrix x, Real traspose)");
  }
  return(ok);
}

//--------------------------------------------------------------------
   static bool OprProd(const BStandardOperator* opr, 
                       const BVMat& x, 
                       bool traspose,
                       BVMat& y)
// Applys linear operator to x and returns y = opr(x,traspose) 
//--------------------------------------------------------------------
{
  bool ok=false;
  BContensVMat* uX  = new BContensVMat(x);
  BContensDat*  uT  = new BContensDat(traspose);
  BList*        lst = Cons(uX,NCons(uT));
  BUserVMat*    uY  = (BUserVMat*)(opr->Evaluator(lst));
  if(uY) 
  { 
    ok = true;
    y = VMat(uY); 
    DESTROY(uY);
  }
  else
  {
    ok=false;
    y.Delete();
  }
  return(ok);
};

//--------------------------------------------------------------------
BVMat BVMat::MinimumResidualsSolve(BStandardOperator* A,
                                   const BVMat& b0,
                                   const BVMat& x0,
                                   double chop,
                                   int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  static BText funName = "MinimumResidualsSolve";
  int i, k;
  BVMat x, r;
  if(!CheckOprProd(A,funName)) { return(x); }
  r.Convert(b0,ESC_blasRdense);
  if(!x0.Rows())
  {
    OprProd(A,r,true,x);
  }
  else
  {
    x .Convert(x0,ESC_blasRdense);
  }
  BVMat q;
  OprProd(A,x,false, q);
//BVMat At = A.T();
  r = r - q;
//BVMat s = At*r;
  BVMat s;
  OprProd(A,r,true,s);
  BVMat p = s;

  int rows = q.Rows();
  int cols = p.Rows();
  //PRECONDITIONS: ENSURING DIMENSIONS 
  if(!rows||!cols||!r.Columns()||!x.Columns()||
     (rows<cols)||(rows!=r.Rows())||(cols!=x.Rows())) 
  { 
    return(x); 
  }
  
  if(chop<0) { chop = sqrt(DEpsilon()/cols); }
  double tolerance = chop*chop;
  if((rows<cols)||(rows!=r.Rows())) { return(x); }

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

  for(k=0; k<maxIter;  k++)
  {
    if(k && !(k%cols))
    {
      Std(BText("\nBVMat::MinimumResidualsSolve ")+
        " iter="+k+
        " maxpa="+maxpa+
        " S="+S+
        " advance="+advance);
    }
    OprProd(A,p,false,q);
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
      if(S.IsUnknown()    ) 
      { break; }
      if(maxpa.IsUnknown()) 
      { break; }
      if(S <= tolerance   ) 
      { break; }
/* */
      if(1+maxpa-1 == 0   ) 
      { break; }
      if(advance >   0    ) 
      { break; }
/* */
    }
    r = r - q*a;
  //s  = At*r;
    OprProd(A,r,true,s);
    oldS = S;
    S  = s.SquaresdSum();
    b  = S.Value()/oldS.Value();
    p = p*b+s;
  }
  return(x);
}

//--------------------------------------------------------------------
BVMat BVMat::MinimumResidualsSolve(BStandardOperator* A,
                                   const BVMat& b0,
                                   double chop,
                                   int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  BVMat x0;
  return(BVMat::MinimumResidualsSolve(A,b0,x0,chop,maxIter));
}
/* */