/* linalg.cpp: Selected Linear Algebra Functions.
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
#include <tol/tol_bstat.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_btimer.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_multifit.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_linalg.h>

BTraceInit("linalg.cpp");

//#define TRZ_MinimumResidualsSolve

//--------------------------------------------------------------------
BDat MatTraze(const BMat& A)
//--------------------------------------------------------------------
{
    BDat tr = 0;
    for(BInt i = 0; (i<A.Rows()) && (i<A.Columns()); i++)
    {
	tr += A(i,i);
    }
    return(tr);
}


//--------------------------------------------------------------------
BMatrix<BDat>  gsl_linalgHHSolve(const BMatrix<BDat>& A_, 
				 const BMatrix<BDat>& b_)
{
    int i, j;
    BInt rows = A_.Rows(), cols = A_.Columns();
    /* PRECONDITIONS: ENSURING DIMENSIONS */
    if(!rows||!cols||!b_.Columns()||(rows<cols)||(rows!=b_.Rows())) 
    { 
      return(BMat(0,0)); 
    }

    gsl_matrix * A  = gsl_matrix_alloc (rows, cols);
    gsl_vector * b  = gsl_vector_alloc (rows);
    gsl_vector * x  = gsl_vector_alloc (cols);
    BMatrix<BDat> x_(cols, 1);
    
    for (i = 0; i < rows; i++)
	for (j = 0; j < cols; j++)
	    gsl_matrix_set (A, i, j, A_(i,j).Value());
    
    for (i = 0; i < rows; i++)
	gsl_vector_set (b, i, b_(i,0).Value());
    
    int res = gsl_linalg_HH_solve(A, b, x);
    
    for (j = 0; j < cols; j++)
	x_(j, 0) = gsl_vector_get (x, j);
    
    gsl_matrix_free (A); 
    gsl_vector_free (b); 
    gsl_vector_free (x);
    
    return(x_);
}

//--------------------------------------------------------------------
BMatrix<BDat> gsl_MinimumResidualsSolve(const BMatrix<BDat>& A_,
					const BMatrix<BDat>& b_)

/*! Solves Ax=b using gsl procedure from the A matrix 
 *  singular value decomposition
 */
//--------------------------------------------------------------------
{
  BInt rows = A_.Rows();
  BInt cols = A_.Columns();
  BMatrix<BDat> x(cols, 1);
  int i;
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!rows||!cols||!b_.Columns()||(rows<cols)||(rows!=b_.Rows())) 
  { 
    return(BMat(0,0)); 
  }
  if (rows==1) {
    if (cols==1) {
      // solucion trivial
      // what if A_(0,0) == 0 ?
      Warning("MRS: Solucion trivial");
      x(0,0) = b_(0,0) / A_(0,0);
    } else {
      Warning("MRS: a1*x1 + a2*x2 + ... + ac*xc = b");
      // la solucion mas estable de a1*x1 + a2*x2 + ... + ac*xc = b
      // buscar el mayor |ai| y hacer xj = 0 para j!=i y xi=b/ai
      double max_abs, max, v, v_abs;
      int idx_max = -1;
      BDat * bufferA = ((BMatrix<BDat>&)A_).GetData().GetBuffer();
      BDat * bufferx = ((BMatrix<BDat>&)x).GetData().GetBuffer();
      // busco el primer conocido
      for(i=0; i<cols; i++)
	if (bufferA[i].IsKnown()) {
	  idx_max = i;
	  max_abs = fabsl(max=bufferA[i].Value());
	}
      for (; i<cols; i++)
	if (bufferA[i].IsKnown()) {
	  v_abs = fabsl(v=bufferA[i].Value());
	  if (v_abs>max_abs) {
	    idx_max = i;
	    max_abs = v_abs;
	    max = v;
	  }
	}
      if (idx_max != -1) {
	for (i=0; i<cols; i++) {
	  if (i==idx_max) {
	    bufferx[i].PutValue(0.0);
	    bufferx[i].PutKnown();
	  }
	  else if (b_(0,0).IsKnown()) {
	    bufferx[i].PutValue(b_(0,0).Value()/max);
	    bufferx[i].PutKnown();
	  }
	}
      }
    }
  } else if (cols==1) {
    // y = A*cols, cols scalar
    //VBR: Este warning no tiene mucho sentido pues el número 
    //     de variables tiene todo el derecho del mundo ha ser 1
    //
    //Warning("MRS y = A*cols, cols scalar");
    //
    double sum_yiai = 0, sum_ai2 = 0, ai;
    for (int i = 0; i < rows; i++) {
      ai = A_(i,0).Value();
      sum_yiai += b_(i,0).Value()*ai;
      sum_ai2 += ai*ai;
    }
    x(0,0) = sum_yiai / sum_ai2;
  } else {
    gsl_matrix * A  = gsl_matrix_alloc (rows, cols);
    gsl_vector * b  = gsl_vector_alloc (rows);
    gsl_vector * w  = gsl_vector_alloc (rows);
    gsl_vector * cx = gsl_vector_alloc (cols);
    
    double  chisq;
    gsl_matrix  *cov = gsl_matrix_alloc (cols, cols);
    int j;
    
    for (i = 0; i < rows; i++)
      for (int j = 0; j < cols; j++)
	gsl_matrix_set (A, i, j, A_(i,j).Value());
    
    for (i = 0; i < rows; i++)
      gsl_vector_set (b, i, b_(i,0).Value());
    
    for (j = 0; j < rows; j++)
      gsl_vector_set (w, j, 1);
    
    gsl_multifit_linear_workspace * work = gsl_multifit_linear_alloc (rows, cols);
    gsl_multifit_wlinear (A, w, b, cx, cov, &chisq, work);
    gsl_multifit_linear_free (work);
    
    for (j = 0; j < cols; j++)
      x(j, 0) = gsl_vector_get (cx, j);
    
    // free gsl objects
    
    gsl_matrix_free (A); 
    gsl_vector_free (b); 
    gsl_vector_free (w); 
    gsl_vector_free (cx); 
    gsl_matrix_free (cov); 
  }

  return(x);
}

//--------------------------------------------------------------------
DMat MinimumResidualsSolve(const DMat& A_,
			   const DMat& b0,
			   const DMat& x0,
         double chop,
         int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  BInt k, rows=A_.Rows(), cols=A_.Columns();
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!rows||!cols||!b0.Columns()||!x0.Columns()||
     (rows<cols)||(rows!=b0.Rows())||(cols!=x0.Rows())) 
  { 
    return(DMat(0,0)); 
  }

  if(chop<0) { chop = Sqrt(DEpsilon()/cols); }
  if(maxIter<0) { maxIter = cols*20; }
#ifdef TRZ_MinimumResidualsSolve
  BText pr=BText("MinimumResidualsSolve(")+rows+"x"+cols+","+BDat(chop).Format("%lg")+")";
  BTimer tm(pr);
  pr = BText("\ncols  ")+pr;
#endif
  if((rows<cols)||(rows!=b0.Rows())) { return(x0); }
  BSpaMatrix<double> A, At;
  A  = A_;
#ifdef TRZ_MinimumResidualsSolve
  Std(BText("\ncols Not null rows ")+A.NotNullRows().Size());
#endif
  At = A_.T();
  DMat x  = x0;
  DMat r = b0 - A*x;
  DMat s = At*r;
  DMat p = s;
  DMat q, pa, rpa;
  double a, b, q2, advance, relAdv;
  double oldR, R=r.FrobeniusNorm();
  BDat xNorm=x.FrobeniusNorm(),
       oldxNorm, xNormAdvance,maxpa, oldMaxpa, oldS, S = MtMSqr(s)(0,0);

  for(k=0; k<maxIter;  k++)
  {
    q = A*p;
    q2 = MtMSqr(q)(0,0);
    a = S.Value()/q2;
    pa = p*a;
    oldMaxpa = maxpa;
    oldR = R;
    R = r.FrobeniusNorm();
    oldxNorm = xNorm;
    x += pa;
    xNorm = x.FrobeniusNorm();
    xNormAdvance = xNorm-oldxNorm;
    maxpa = 0;
    for(BInt i=0; i<cols; i++)
    {
      if(x(i,0)!=0)
      {
	double rpa = Abs(pa(i,0)/x(i,0));
	if(rpa>maxpa) { maxpa = rpa; }
      }
    }

    advance = R-oldR;
    relAdv = advance/oldR;
#ifdef TRZ_MinimumResidualsSolve
    Std(pr+" Iteration "+k+
	      "\tAbsMaxNorm : "+maxpa.Format("%lg")+
	      "\tResNorm : "+BDat(R).Format("%lg"));
#endif
    if(k)
    {
#ifdef TRZ_MinimumResidualsSolve
      Std(BText(" - ")+
	  BDat(oldR	   ).Format("%lg"   )+" = "+
	  BDat(advance	   ).Format("%lg"   )+" ("+
	  BDat(xNormAdvance).Format("%lg"   )+" ,"+
	  BDat(100*relAdv  ).Format("%8.4lf")+"%)");
#endif
      if(S.IsUnknown()               ) { break; }
      if(maxpa.IsUnknown()           ) { break; }
      if(advance             >   0   ) { break; }
      if(S                   <= chop ) { break; }
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
    r -= q*a;
    s  = At*r;
    oldS = S;
    S  = MtMSqr(s)(0,0);
    b  = S.Value()/oldS.Value();
    p*=b; p+=s;

//  Std(BText("\cols")+k+"\t"+S);
  }
/*
  Std(pr+" Iteration "+k+
	    "\tAbsMaxNorm : "+maxpa.Format("%lg")+
	    "\tResNorm : "+BDat(R).Format("%lg"));
  Std(BText(" - ")+oldR+" = "+
      BDat(   advance).Format("%lg"   )+" ("+
      BDat(100*relAdv).Format("%6.3lf")+"%)");
*/
  return(x);
}


//--------------------------------------------------------------------
BMat MinimumResidualsSolve(const BMat& A, const BMat& b, const BMat& x,
                           double chop,
                           int maxIter)

/*! Solves Ax=b using the Lanczos method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  DMat y = MinimumResidualsSolve
  (
    (const DMat&)A,
    (const DMat&)b,
    (      DMat&)x,
    chop, maxIter
  );
  return(*(BMat*)&y);
}

//--------------------------------------------------------------------
DMat MinimumResidualsSolve(const DMat& A, const DMat& b0,
                           double chop,
                           int maxIter)

/*! Solves Ax=b using the Lanczos method without a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. 
 */
//--------------------------------------------------------------------
{
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!A.Rows()||!A.Columns()||!b0.Columns()||
     (A.Rows()!=b0.Rows())) 
  { 
    return(DMat(0,0)); 
  }
  return(MinimumResidualsSolve(A, b0, A.T()*b0,chop,maxIter));
}

//--------------------------------------------------------------------
BMat MinimumResidualsSolve(const BMat& A, const BMat& b0,
                           double chop,
                           int maxIter)

/*! Solves Ax=b using the Lanczos method without a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. The matrix A must by
 *  symetric.
 */
//--------------------------------------------------------------------
{
  /* PRECONDITIONS: ENSURING DIMENSIONS */
  if(!A.Rows()||!A.Columns()||!b0.Columns()||
     (A.Rows()!=b0.Rows())) 
  { 
    return(BMat(0,0)); 
  }
  return(MinimumResidualsSolve(A, b0, A.T()*b0, chop, maxIter));
}


//--------------------------------------------------------------------
BMat Kernel(const BMat& A)

/*! Adds right columns to a orthonormal matrix until to obtain a
 *  orthonormal matrix of m columns
 */
//--------------------------------------------------------------------
{
  BInt i,j,k, r=A.Rows(), c=A.Columns();
  BTimer tm(BText("Kernel(")+r+","+c+")");
  int r0 = (r>=c)?1:(c-r+1);
  BMat K (0,0);
  BMat YK(0,0);
  BMat YA(r,1);  YA.SetAllValuesTo(0);
  BMat B0(r0,c); 
  BMat Y01(1,1); Y01.SetAllValuesTo(0);
  BMat Y0(r0,1); Y0.SetAllValuesTo(1);
  BDat relTol = BDat::RelTolerance();
  BDat tol    = BDat::Tolerance();
  BDat::RelTolerance() = DEpsilon();
  BDat::Tolerance   () = Sqrt(DEpsilon()/r);
  bool ok=true;
  for(k=0; ok && (k<c); k++)
  {
    for(i=0; i<r0; i++)
    { 
      for(j=0; j<c; j++)
      { 
        B0(i,j) = BUniformDist::Random01().Value()*Sqrt(DEpsilon()); 
      }
    }
    BMat B = B0 <<  K <<  A;
    BMat Y = Y0 << YK << YA;
  //Std(BText("\nKernel B =\n")+B.Name());
  //Std(BText("\nKernel Y =\n")+Y.Name());
    BMat X = gsl_MinimumResidualsSolve(B,Y);
    ok = X.Rows() && X.Columns();
  //Std(BText("\nKernel X =\n")+X.Name());
  //Std(BText("\nok=")+(int)ok);
    if(ok)
    {
      X /= Sqrt(MtMSqr(X)(0,0));
      BMat AX = A*X;
    //Std(BText("\nKernel AX=\n")+AX.Name());
      BDat norm = AX.FrobeniusNorm();
    //Std(BText("\n\nAX.FrobeniusNorm()=")+AX.FrobeniusNorm()+"\n");
      ok = norm<Sqrt(DEpsilon());
    //Std(BText("\nok=")+(int)ok);
      if(ok)
      {
        K  =  K <<  X.T();
        YK = YK << Y01;
      //Std(BText("\nKernel K =\n")+K .Name());
      //Std(BText("\nKernel YK=\n")+YK.Name());
      }
    }
  }
  BDat::RelTolerance() = relTol;
  BDat::Tolerance   () = tol;
  return(K.T());
}

//--------------------------------------------------------------------
void OrthonormalCompletion(BMat& U, BInt m)

/*! Adds right columns to a orthonormal matrix until to obtain a
 *  orthonormal matrix of m columns
 */
//--------------------------------------------------------------------
{
//BTimer tm("OrthonormalCompletion");
    BInt n = U.Rows   ();
    BInt c = U.Columns();
    if(n<=c) { return; }
    if(m> n) { m=n; }
    if(m<=c) { return; }
    BInt r=c+1;
    BInt i,j;
    BMat B(1,r);
    BMat y(r,1);
    for(i=0;i<r;i++) { B(0,i) = BUniformDist::Random01(); }
    y.SetAllValuesTo(0);
    y(0,0) = BUniformDist::Random01();
    BMat Urc = U.Sub(0,0,r,c);
    BMat A = B << Urc.T();
    BMat z(n-r,1); z.SetAllValuesTo(0);
    BMat v = MinimumResidualsSolve(A, y) ;
    
    for(j=0; j<c; j++)
    {
	BMat u = Urc.Sub(0,j,r,1);
	BDat uv = (u.T()*v)(0,0);
	v -= u*uv;
	BDat vv = MtMSqr(v)(0,0);
	v /= Sqrt(vv);
    }
    
    U = U | (v<<z);
    if(r<m)
    {
	OrthonormalCompletion(U,m);
    }
}


//--------------------------------------------------------------------
static BDat GetSVDSwartzInformationCriterium(const BMatrix<BDat>&     A,
					     const BMatrix<BDat>&     Y,
					     const BMatrix<BDat>&     U,
					     const BDiagMatrix<BDat>& D,
					     const BMatrix<BDat>&     V,
					           BInt		      r)
//--------------------------------------------------------------------
{
    BInt n = V.Rows();
    BInt m = U.Rows();
    BDiagMatrix<BDat> Dp;
    BMat P_, R, X;
    BDat S;
    Dp = D.Sub(0,0,r,r);
    Dp = Dp.P(Sqrt(DEpsilon()));
    P_ = V.Sub(0,0,n,r)*Dp;
    X  = P_*(U.Sub(0,0,m,r).T()*Y);
    R  = A*X-Y;
    S = Sqrt(MtMSqr(R)(0,0)/(m-r));
    BDat sw = S; //2*Log(S)+r*Log(m)/m;
//Std(BText("\nSVDSwartzInfCrit(")+r+") = "+sw);
    return(sw);
}


//--------------------------------------------------------------------
void EliminateLinearRelations(const BMatrix<BDat>&  kernel,
			      const BArray <BText>& name,
			            BArray <BBool>& varElim)
//--------------------------------------------------------------------
{
//BTimer tm("EliminateLinearRelations");
  BInt i, j, k, n = kernel.Rows(), d = kernel.Columns();

  BArray<BBool> colElim(d);
  varElim.ReallocBuffer(n);

  for(j=0; j<d; j++) { colElim(j) = BFALSE; }
  for(j=0; j<n; j++) { varElim(j) = BFALSE; }
/*
  if(d)
  {
    Warning(I2("\nThere are ", "\nHay ") + (d) +
	    I2(" linear combinations."," combinaciones lineales."));
  }
  else
  {
    Std(I2("\nThe system is regular.","\nEl sistema es regular."));
  }
*/
  BDat minVal;
  for(k=0;k<d;k++)
  {
    BDat maxVal;
    BInt iMax = -1;
    BInt jMax = -1;
    for(j=0; j<n; j++)
    {
      if(!varElim[j])
      {
	for(i=0; i<d; i++)
	{
	  if(!colElim[i])
	  {
  //	    Std("\n0 = ");
	    BDat abs = Abs(kernel(j,i));
  //	    Std(BText("\n  +")+V(j,i)+" * ("+XName(j)+")");
	    if(maxVal.IsUnknown() || (abs>maxVal))
	    {
	      maxVal = abs;
	      iMax = i;
	      jMax = j;
	    }
	  }
	}
      }
    }
//  Std(BText("\n  MaxValue(")+iMax+","+jMax+")="+maxVal);
    if((jMax>=0) && (iMax>=0))
    {
      varElim[jMax] = BTRUE;
      colElim[iMax] = BTRUE;
      Std(BText("\n  ")+(k+1)+")  "+
	  I2("Eliminated variable : ",
	     "Variable eliminada : ")+name(jMax)+
	  I2("\n  Posible related variables : ",
	     "\n  Variables posiblemente relacionadas : "));
      BText txt = "";
      for(j=0; j<n; j++)
      {
	BDat val = -(kernel(j,iMax))/maxVal;
	if((j!=jMax)&&(Abs(val)>Sqrt(BDat::Zero())))
	{
	  txt += BText("\n		   ");
	  if((val>0)&&(txt!="")) { txt += " + "; }
	  txt+=BText("")+val+" * ("+name(j)+")";
	}
      }
      if(txt=="") { txt = "0"; }
      Std(txt);
    }
  }
};


//--------------------------------------------------------------------
void WriteRowNullEquations(const BMatrix<BDat >& A,
			   const BArray <BText>& name)
//--------------------------------------------------------------------
{
//BTimer tm("EliminateLinearRelations");
    BInt i, j, r = A.Rows(), c = A.Columns();
    BText txt = "";
    for(i=0; i<r; i++)
    {
	txt += BText("Equ. ") + (i+1) +") \n";
	for(j=0; j<c; j++)
	{
	    if(Abs(A(i,j))>Sqrt(DEpsilon()))
	    {
		txt += BText("\n  ");
		if((A(i,j)>0)&&(txt!="")) { txt += " + "; }
		txt+=BText("")+A(i,j)+" * ("+name[j]+")";
	    }
	}
	txt += " = 0;\n";
    }
    Std(txt);  
};



//--------------------------------------------------------------------
BDat LinearLeastSquareSolve(const BMatrix<BDat>&	M,
			    const BMatrix<BDat>&	Y,
			    const BArray <BText>&	name,
			          BMatrix<BDat>&	X,
			          BMatrix<BDat>&	R,
			          BMatrix<BDat>&	P,
			          BSymMatrix<BDat>&	information,
			          BSymMatrix<BDat>&	covarianze,
			          BMatrix<BDat>&	kernel,
			          BArray <BBool>&	varElim,
			          BBool		        onlySolve)

/*! Given any rectangular matrix A and a column vector Y performs
 *  the least squares solution of
 *
 *			 Y = A*X + R
 *
 *	     and returns the euclidean norm of the residuals R
 *
 *	     If onlySolve is false then the function already returns
 *
 *	       i)  the pseudo-inverse of A into P
 *
 *			   A*P*A = A;
 *			   P*A*P = P;
 *
 *	      ii)  the information matrix of A and Y
 *
 *			     t(A)*A
 *			    --------
 *			     t(Y)*Y
 *
 *	     iii)  the covarianze matrix of parameters X that is the inverse
 *		   of the information matrix
 *
 *	      iv)  a base of the kernel of A
 *
 *			 K(A) = { k | A*k = 0 }
 */
//--------------------------------------------------------------------
{
  BTimer tm("LinearLeastSquareSolve");
  BDat chop = Sqrt(DEpsilon());

  /* PRECONDITIONS: ENSURING DIMENSIONS */
  BInt rows = M.Rows(), cols = M.Columns();
  if(!rows||!cols||!Y.Columns()||
     (rows<cols)||(cols!=Y.Rows())) 
  { 
    return(BDat::Unknown()); 
  }

  BDat norm, S2;
  BMatrix<BDat> U,V,A;
  BDiagMatrix<BDat> D;
/*
  BDiagMatrix<BDat> DA;
  DiagMaxAbsColumnsDecomp(M,A,DA);
  BDiagMatrix<BDat> DAp = DA.P(chop);
*/

  BDiagMatrix<BDat> DA(cols,1),DAp(cols,1);
  A = M;

  SingularValueDecomposition(A, U, D, V);
//TRZ(U); TRZ(D); TRZ(V);
//TRZ(D);  BMat UtAV = U.T()*A*V; TRZ(UtAV);
  if(!onlySolve)
  {
    BInt r = D.Rows(), r0=r;

    BDat sw1,sw2;

    sw2 = GetSVDSwartzInformationCriterium(A,Y,U,D,V,r);
    do
    {
      r--;
      sw1 = sw2;
      sw2 = GetSVDSwartzInformationCriterium(A,Y,U,D,V,r);
    }
    while((r>=2) && (sw2<=sw1));
    r++;
    r  = r0;

    U  = U.Sub(0,0,rows,r);
    D  = D.Sub(0,0,r,r);
    V  = V.Sub(0,0,cols,r);
    BDiagMatrix<BDat> Dp  = D.P(chop);

    //TRZ(V);
    //TRZ(D);
    BMat A_ = V*D;
    //TRZ(A_);
    A_ = DAp * A_;
    //TRZ(A_);
    BMat P_ = V*Dp;
    P_ = DA * P_;
    P = P_*U.T();
    X = P*Y;
    R = Y-M*X;
    S2 = MtMSqr(R)(0,0)/(rows-r);
    information = MtMSqr(A_.T())/S2;
    covarianze	= MtMSqr(P_.T())*S2;

//  TRZ(Y);   TRZ(P);  TRZ(X);	TRZ(R);	 TRZ(S2);
//  TRZ(information);
//  TRZ(covarianze);
//  BMat infcov	  = covarianze*information; TRZ(infcov);

    BMatrix<BDat> VK=V;
    OrthonormalCompletion(VK,cols);
    kernel = VK.Sub(0,r,cols,cols-r);
    kernel += 1000;
    kernel -= 1000;
    EliminateLinearRelations(kernel, name, varElim);
  }
  else
  {
    BDiagMatrix<BDat> Dp = D.P(chop);
    BMat P_ = DAp*V*Dp;
    X = P_*(U.T()*Y);
    R = A*X-Y;
    S2 = MtMSqr(R)(0,0)/(rows-cols);
  }
  return(Sqrt(S2));
};



//--------------------------------------------------------------------
BDat LinearLeastSquareSolve(const BMatrix<BDat>&	A,
			    const BMatrix<BDat>&	Y,
			          BMatrix<BDat>&	X,
			          BMatrix<BDat>&	R)
//--------------------------------------------------------------------
{
    BMatrix<BDat>	    P;
    BArray <BText>    name;
    BSymMatrix<BDat>  inf;
    BSymMatrix<BDat>  cov;
    BMatrix<BDat>	    ker;
    BArray <BBool>    varElim;
    return(LinearLeastSquareSolve(A,Y,name,X,R,P,inf,cov,ker,varElim,1));
}


#ifdef __USE_DEPRECATED_LINALG_METHOD__
//Old deprecated methods that has been abandoned
//are saved here for a few months

//--------------------------------------------------------------------
BBool MatDiagDom(const BMat& A)
//--------------------------------------------------------------------
{
    BInt i,j,n = A.Rows();
    BBool ok = BTRUE;
    for(i=0; ok && (i<n); i++)
    {
	BDat sum = 0;
	for(j=0; j<n; j++)
	{
	    if(i!=j) { sum += Abs(A(i,j)); }
	}
	ok = Abs(A(i,i))>sum;
    }
    return(ok);
}

//--------------------------------------------------------------------
BBool FirstComponent(BMatrix<BDat>& S,
		     BMat&	    b,
		     BDat&	    lambda,
		     BDat	    tolerance)
//--------------------------------------------------------------------
{
    BInt i, k, N = S.Rows();
    BDat sumSqr = 0;
    BDat dif=tolerance;
    BMat oldB;
    
    // Iniciamos b (de norma 1) con valores aleatorios
    b.Alloc(N,1);
    for(i=1; i<N; i++)
    {
	b(i,0) = BUniformDist(-1/sqrt((double)N),1/sqrt((double)N)).Random();
	sumSqr += b(i,0)*b(i,0);
    }
    b(0,0) = Sqrt(1-sumSqr);
/*
  Std("\n________________________________________________________________\n");
  TRZ(S);
  TRZ(S8);
  TRZ(b);
*/
    for(k=0; (dif>=BDat::Zero())&&(k<=Maximum(100,N)); k++)
    {
	oldB = b;
	b = S * b;
	lambda    = sqrt((double)N)*b.FrobeniusNorm();
	b	     /= lambda;
	dif = (oldB - b).MaxAbsNorm();
/*
    Std(BText("\nIteration number ") + k +
	      "\tlambda = "	     + lambda.Name() +
	     // "\tlambda = "    + (lambda^(1.0/8.0)).Name() +
	      "\ttolerance = "	     + tolerance.Name() +
	      "\tdif = "	     + dif.Name());
		  */
    }
    
    BMat test = b.T()*S*b;
    lambda    = test(0,0); 
    BBool ok  = (Abs(test(0,0)-lambda)<tolerance);
/*
  TRZ(test);
  Std("\n________________________________________________________________\n");
  Std(BText("\nIterations = ") + k +
	    "\tlambda = "      + lambda.Name() +
	    "\tdif = "	       + dif.Name());
  Std("\n________________________________________________________________\n");
*/

    return(ok);
}


//--------------------------------------------------------------------
BBool JordanIterative(BMatrix<BDat>	 S1,
		      BMatrix<BDat>&	 B,
		      BDiagMatrix<BDat>& lambda,
		      BDat		 tolerance)
//--------------------------------------------------------------------
{
    BMat b;
    BDat L;
    BInt i, j, r, c,  N = S1.Rows();
    B.Alloc(N);
    lambda.Alloc(N);
    BBool ok = BTRUE;
/*
  BMatrix<BDat> S2 = S1*S1;
  BMatrix<BDat> S4 = S2*S2;
  BMatrix<BDat> S8 = S4*S4;
*/
    BMatrix<BDat> S  = S1;
    for(j=0; j<N; j++)
    {
	if(j>0)
	{
	    for(r=0; r<N; r++)
	    {
		for(c=0; c<N; c++)
		{
		    S(r,c) -= L*b(r,0)*b(c,0);
		}
	    }
	}
	BBool nextOk = FirstComponent(S, b, L, tolerance);
	ok = ok && nextOk;
	lambda(j,j) = L; //^(1.0/8.0);
	for(i=0; i<N; i++)
	{
	    B(i,j) = b(i,0);
	}
    }
    return(ok);
}

//--------------------------------------------------------------------
BMat GaussSeidelSolve(const BMat& A, const BMat& b, const BMat& x0)

/*! Solves Ax=b using the Gauss-Seidel method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. The matrix A must by
 *  symetric.
 */
//--------------------------------------------------------------------
{
//BTimer tm("GaussSeidelSolve");
    BInt i,j,k,u,n=A.Rows();
    BMat x = x0;
    
    for(k=0; k<BDat::MaxIter();  k++)
    {
	BMat xOld = x;
	for(i=0; i<n; i++)
	{
	    for(u=0; u<b.Columns(); u++)
	    {
		x(i,u) = b(i,u);
		for(j=0; j<n; j++)
		{
		    if(i!=j) { x(i,u) -= A(i,j)*x(j,u); }
		}
		x(i,u) /= A(i,i);
	    }
	}
	BDat advance = (x-xOld).FrobeniusNorm();
//  Std(BText("\nGaussSeidelSolve Iteration ")+k+" : " + Sqrt(advance));
	if(advance<=BDat::Tolerance()) { break; }
    }
    
//Std(BText("\nGaussSeidelSolve Iterations (")+
//	    k+", " + Sqrt(rNorm)+", "+ Sqrt(advance)+")");
    
    return(x);
}

//--------------------------------------------------------------------
BMat ConjugateGradientSolve(const BMat& A, const BMat& b, const BMat& x0)

/*! Solves Ax=b using the Conjugate Gradient method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. The matrix A must by
 *  symetric.
 */
//--------------------------------------------------------------------
{
//BTimer tm("ConjugateGradientSolve");
    BInt k;
    BMat x = x0;
    BMat r = b - A*x;
    BDat c = MtMSqr(r)(0,0);
    BMat v = r;
    BDat advance;
    

//Std(BText("\nConjugateGradientSolve Iteration ")+k+" : " + Sqrt(norm0));
    for(k=0; (k<2*b.Rows());  k++)
    {
//  Std(BText("\nConjugateGradientSolve Iterations (")+
//		k+", " + c+", "+ Sqrt(advance)+")");

	if(c*c <= BDat::Zero()) { break; }
	
	advance = v.FrobeniusNorm();
	
	if(advance<=BDat::Tolerance()) { break; }
	
	BMat z    = A*v;
	BDat t    = c/(v.T()*z)(0,0);
	
	x += v*t;
	r -= z*t;
	BDat d = MtMSqr(r)(0,0);
	
	v *= (d/c);
	v += r;
	c = d;
//  TRZ(x);
	
    }
    
//Std(BText("\nConjugateGradientSolve Iterations (")+
//	    k+", " + Sqrt(rNorm)+", "+ Sqrt(advance)+")");
    
    return(x);
}

//--------------------------------------------------------------------
BMat BiconjugateGradientSolve(const BMat& A, const BMat& b, const BMat& x0)

/*! Solves Ax=b using the Conjugate Gradient method from a intial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. The matrix A must by
 *	    symetric.
 */
//--------------------------------------------------------------------
{
//BTimer tm("BiconjugateGradientSolve");
    BInt k;
    BMat At = A.T();
    BMat x = x0;
    BMat r = b - A*x;
    BMat r_= r;
    BMat p = r;
    BMat p_= r_;
    
    BDat R = MtMSqr(r_)(0,0);
    BDat advance;
    
//Std(BText("\nBiconjugateGradientSolve Iteration ")+k+" : " + Sqrt(R));
    for(k=0; (k<2*b.Rows());  k++)
    {
//  Std(BText("\nBiconjugateGradientSolve Iterations (")+
//	      k+", " + R+", "+ Sqrt(advance)+")");
	if(R*R <= BDat::Zero()) { break; }
	
	BMat Ap  = A*p;
	BMat Ap_ = At*p_;
	BDat P   = (p_.T()*Ap)(0,0);
	BDat a   = R / P;
	
	x += p*a;
	
	advance = Sqrt(Abs(P*a));
	
	if(advance<=BDat::Tolerance()) { break; }
	
	r  -= Ap *a;
	r_ -= Ap_*a;
	
	BDat S  = (r_.T()*r)(0,0);
	
	
	BDat b  = S/R;
	
	R = S;
	
	p  = r  + p *b;
	p_ = r_ + p_*b;
	
    }
    
//Std(BText("\nConjugateGradientSolve Iterations (")+
//	    k+", " + Sqrt(rNorm)+", "+ Sqrt(advance)+")");

    return(x);
}


#endif //__USE_DEPRECATED_LINALG_METHOD__