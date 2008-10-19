/* linreg.cpp: Generic statistical functions.
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

#include <tol/tol_matrix.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_blinreg.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bltrimat.h>
#include <tol/tol_lapack.h> 

BTraceInit("linreg.cpp");

//--------------------------------------------------------------------
bool CheckCholeski(const BMatrix<BDat>& S, 
                         BMatrix<BDat>& L)
//--------------------------------------------------------------------
{
  int i;
  int n = S.Rows();
  if(!n) { return(false); }
  bool ok = (TolLapack::dpotrf(CblasLower, S, L)==0) &&
             (L.Rows()==n)&&(L.Columns()==n);
  if(ok)
  {
    BMatrix<BDat> r(n,1);
    BNormalDist u(0,1);
    for(i=0; i<r.Rows(); i++) { r(i,0) = u.Random().Value(); }
    BDat rtSr   = (r.T()*(S*r))(0,0);    
    BMatrix<BDat> Ltr; 
    TolBlas::dtrmm(CblasLeft, CblasLower, CblasTrans, CblasNonUnit, 1.0, L, r, Ltr);
    BDat rtLLtr = MtMSqr(Ltr)(0,0);  
    BDat err = Abs(rtLLtr-rtSr)/Abs(rtLLtr);
    ok = err<1.E-10;
  }
  return(ok);
};

//--------------------------------------------------------------------
BDat  LinearRegression(const BArray <BDat>& y,
		       const BMatrix <BDat>& X,
		       BArray <BDat>& A,
		       BArray <BDat>& e,
		       BMatrix<BDat>& Li,
		       BMatrix<BDat>& L,
		       BMatrix<BDat>& cov,
		       BMatrix<BDat>& cor,
		       BDat & R2,
		       BDat & F,
		       BInt & v1,
		       BInt & v2,
		       BDat & alfa,
		       BDat & resSqSum,
		       BDat & totSqSum)

/*! Estimates the linear regression model
 *
 *  	y = X * A + e;
 *
 *      dim(A) = m   : number of variables
 *      dim(y) = n   : number of data
 *      dim(X) = n,m
 *      dim(e) = n
 *
 *                     -1
 *    E(A)   = (X' * X)    * X' * y
 *
 *  	Var(e) ~ s^2 = (1/(n-m)) * (y - X * A)' * (y - X * A)
 *
 *    Matrix Li is a square root decomposition of information matrix
 *        -1
 *     cov   = s^2 * (X' * X) = Li * Li'
 *
 *    Matrix L is a square root decomposition of covarianze
 *
 *     cov   = L' * L
 *
 *    If Choleski decomposition fails then SVD is used to build Li 
 *    and L. In this case they aren't triangular matrices.
 *
 *  	freedom degrees : v1 = m; v2 = n-m-1;
 *  	F = (R2*v2) /((1-R2)*v1) ;
 *  	F(v1,v2)(F) = alfa;
 *
 *  	residual sum of squares : resSqSum = Sum(e[i]^2,i=1..n);
 *  	total sum of squares	: totSqSum = Var(y), i=1..n);
 */
//--------------------------------------------------------------------
{
//BArray<BDat> T;
//BMatrix<BDat> C = X;	C*=(-1);
    BDat s;
    BInt m = X.Columns();
    BInt n = X.Rows();
    BInt i, j;
    
    cov.Alloc(m);
    cor.Alloc(m);
    Li.Alloc(m);
    e.ReallocBuffer(n);
    A.ReallocBuffer(m);
    BMatrix<BDat> XtY, B, Yprev, R, Y(n,1,y.Buffer());
    
//if(!HouseholderTransformation(T,C)) { return; }
//LeastSqrHouseholder(T,C,y,A,e);
    BMatrix<BDat> XtX = MtMSqr(X);
  //Std(BText("\nXtX=")<<XtX.Name());
    bool ok = CheckCholeski(XtX,Li);
  //Std(BText("\nLi=")<<Li.Name());

    if(ok) 
    { 
      BMatrix<BDat> I = BDiagMatrix<BDat>(m,1);
      TolBlas::dtrsm(CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit, 1.0, Li, I, L);
    //Std(BText("\nL")<<L.Name());
    //Std(BText("\nL*Li")<<(L*Li).Name());
      XtY = X.T() * Y;
    //Std(BText("\nXtYi")<<XtY.Name());
      TolLapack::dpotri(CblasLower,Li,cov);
    //Std(BText("\nLtL")<<cov.Name());
      B = cov*XtY;
    //Std(BText("\nB")<<B.Name());
    }
    else
    {
      BMatrix<BDat>U(n,m),V(m,m);
		  BDiagMatrix<BDat>D, Dp;
      D.Alloc(m);
      gsl_SingularValueDecomposition(X,U,D,V,"Golub_Reinsch");
      Dp = D.P(Sqrt(DEpsilon()));
      Li = V*D;
      L  = Dp*V.T();
    //Std(BText("\nL*Li")<<(L*Li).Name());
      cov= MtMSqr(L);
      B  = ((Y.T()*U)*L).T();
    }
/*
    Std(BText("Li = \n")+Li.Name());
    Std(BText("L = \n")+L.Name());
    Std(BText("test Li*Li'-XtX = \n")+(Li*Li.T()-XtX).Name());
    Std(BText("test L*Li = \n")+(L*Li).Name());
    Std(BText("test L'*L-cov = \n")+(L.T()*L-cov).Name());
    Std(BText("test cov*XtX = \n")+(cov*XtX).Name());
*/
    Yprev = X*B;
    R = Y-Yprev;
    
    for(i=0; i<m; i++) {	A(i) = B(i,0); }
    s = 0;
    
    for(j=0; j<n; j++)
    {
	e[j] = R(j,0);
	s += e(j) ^ 2;
    }
    s /= n;
    s.Sqrt();
    
/*//Householder
  for(i=0; i<m; i++)
  {
    for(j=0; j<i; j++) { Li.Put(i,j,C[j][i]*s); }
    Li.Put(i,i,T[i]*s);
  }
*/
  Li  = Li/s;
  L   = L *s;
  cov = cov*(s*s);
    
    for(i=0; i<m; i++)
    {
	cor(i, i)=1;
	for(j=0; j<i; j++)
	{
	    cor(i, j)=cov(i,j)/Sqrt(cov(i,i)*cov(j,j));
	}
    }
    
    resSqSum = Moment(e,2)*n;
    totSqSum = Moment(y,2)*n;
    R2 = R2Coefficient(Y.Data(),Yprev.Data());
    v1 = m;
    v2 = n-m-1;
    F  = R2*BDat(v2) / ((1-R2)*BDat(v1));
    if((v1>0)&&(v2>0)) { alfa = BFSnedecorDist(v1,v2).Dist(F); }
    else	             { alfa.PutKnown(BUNKNOWN); }
    return(s);
}
