/* divcong.cpp: Singular Value Decomposition and Symmetric Eigen Problem.
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
#include <tol/tol_bsvdsep.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_brealfun.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_btimer.h>

BTraceInit("divconq.cpp");

//--------------------------------------------------------------------
DMat SymTridiagByMatrixProduct(const BSymMatrix<double>& A, 
			       const DMat& B)
//--------------------------------------------------------------------
{
    BInt i,j,k,n=A.Rows(),m=B.Columns();
    if(B.Rows()!=n) { return(DMat(0,0)); }
    DMat y(n,m);
    double s;
//Std("\nSymTridiagByMatrixProduct 0.0");
    for(j=0;j<m;j++)
    {
	for(i=0;i<n;i++)
	{
	    s = 0;
	    for(k=Maximum(0,i-1);k<=Minimum(n-1,i+1);k++)
	    {
		s+=A(i,k)*B(k,j);
	    }
	    y(i,j) = s;
	}
    }
    return(y);
}


//--------------------------------------------------------------------
DMat TridiagMinimumResidualsSolve(const BSymMatrix<double>& A, 
				  const DMat& b0, 
				  const DMat& x0)

/*! Solves Ax=b using the Conjugate Gradient method from a initial
 *  aproximation in a given maximum of iterations and obtaining an
 *  standard error less than a given tolerance. The matrix A must by
 *  symetric and tridiagonal.
 */
//--------------------------------------------------------------------
{
//BTimer tm("TridiagMinimumResidualsSolve");
    BInt k, m=A.Rows(), n=A.Columns();
    if((m<n)||(m!=b0.Rows())) { return(x0); }
    DMat x  = x0;
    DMat r = b0 - A*x;
    DMat s = A*r;
    DMat p = s;
    DMat q;
    double a, b, maxpa=1;
    
    double S = MtMSqr(s)(0,0);
    
//Std(BText("\nMinimumResidualsSolve Iteration ")+k+" : " + Sqrt(R));
    for(k=0; (k<2*n)&&(maxpa>1.0E-18);  k++)
    {
	q = SymTridiagByMatrixProduct(A,p);
	double q2 = MtMSqr(q)(0,0);
	a = S/q2;
	DMat pa = p*a;
	maxpa = pa.MaxAbsNorm();
	x += pa;
	r -= q*a;
	s  = SymTridiagByMatrixProduct(A,r);
	double oldS = S;
	S  = MtMSqr(s)(0,0);
	b  = S/oldS;
	p*=b; p+=s;
	
//  Std(BText("\n")+k+"\t"+S);
    }
    
//Std(BText("\nMinimumResidualsSolve Iterations (")+
//	    k+", " + Sqrt(rNorm)+", "+ Sqrt(advance)+")");
    
    return(x);
}

//--------------------------------------------------------------------
class BTSEFunction : public BFunction<double, double>

/*! Evaluates the secular equation
 *
 *		      f(x) = 1 + r * Sum(u(i)^2/(d(i)-x))
 *
 *	     and its derivative
 */
//--------------------------------------------------------------------
{
public:
  BInt n_;
  BArray<double>& u_;
  BArray<double>  u2_;
  BArray<double>& d_;
  BArray<double>  A1_;
  BArray<double>  A2_;
  BArray<double>  D1_;
  BArray<double>  D2_;
  double s_;
  double r_;
  double f_;
  double Df_;
  BBool calcDer_;

public:
  BTSEFunction(BArray<double>& u, BArray<double>& d, double r, double s)
  :BFunction<double, double>(),
   n_(u.Size()), u_(u), d_(d), s_(s), r_(r), calcDer_(BFALSE)
  {
    u2_.ReallocBuffer(n_);
    register int i;
    for(i = 0; i<n_; i++)
    {
      u2_(i) = u_(i)*u_(i);
    }
  };
 ~BTSEFunction() {};
  void Evaluate(double& f, const double& x)
  {
    register int i;
    register double g,h;
    f_	= Df_ = 0;
    if(calcDer_)
    {
      for(i = 0; i<n_; i++)
      {
	h    = d_(i)-x;
	g    = u2_(i)/h;
	f_  += g;
	Df_ += g/h;
//	Std(BText("\nBTSE[")+i+"]\tx="+x+"\th="+h+"\tg="+g+"\tf="+f_+"\tDf="+Df_);
      }
      Df_ *= r_;
    }
    else
    {
      for(i = 0; i<n_; i++)
      {
	h    = d_(i)-x;
	g    = u2_(i)/h;
	f_  += g;
//	Std(BText("\nBTSE[")+i+"]\tx="+x+"\th="+h+"\tg="+g+"\tf="+f_);
      }
    }
    f_ *= r_;
    f_ += s_;
    f = f_;
//  Std(BText("\nBTSEFunction(")+x+" = "+f);
  };
  double fad(double x, BInt j)
  {
    double f=0;
    for(int i = 0; i<n_; i++)
    {
      if((i!=j)&&(i!=j+1)) { f += u2_(i)/(d_(i)-x); }
    }
    return(f);
  };
  void fadLast(double x0, double& f, double& Df)
  {
    register double h, g, x = x0;
    f=Df=0;
    for(int i = 0; i<n_-1; i++)
    {
      h	  = (d_(i)-x);
      g	  = u2_(i)/h;
      f	 += g;
      Df += g/h;
    }
  };
  double AproxSolveLast(double x0) //j==n-1
  {
    double f0, Df0;
    fadLast(x0,f0,Df0);
    double a	 = -Df0;
    double b	 = u2_(n_-1);
    double C	 = s_/r_ + f0;
    double disc2=C*C-4*a*b;
    double disc =(disc2<0)?0:Sqrt(disc2);
    double x1	 = x0 + (C+disc)/(2*a);
    double x2	 = x0 + (C-disc)/(2*a);
    if(x1>x0) { return(x1); }
    else      { return(x2); }
  }
  double AproxSolveNoLast(double x0, BInt j) //j<n+1
  {
    double C  = s_/r_+fad(x0,j);
    double v2 = u2_(j);
    double a  = d_(j);
    double w2 = u2_(j+1);
    double b  = d_(j+1);
    double B  = (a + b)*C + v2 + w2;
    double D2 = B*B - 4*C*(a*w2 + b*v2 + a*b*C);
    double D  = (D2<0)?0:Sqrt(D2);
    double x1 = (B+D)/(2*C);
    double x2 = (B-D)/(2*C);
    if((x1>a)&&(x1<b)) { return(x1); }
    else	       { return(x2); }
  };
  double AproxSolve(double x0, BInt j)
  {
    if(j==n_-1) { return(AproxSolveLast	 (x0  )); }
    else	{ return(AproxSolveNoLast(x0,j)); }
  }
  double AproxSolve(BInt j)
  {
    if(j==n_-1) {  return(AproxSolveLast(d_(n_-1)));
    }
    else
    {
      double x1 = AproxSolveNoLast(d_(j  ), j);
      double x2 = AproxSolveNoLast(d_(j+1), j);
      return((x1+x2)/2);
    }
  }
};

struct BIndexedLDouble
{
  double dat_;
  BInt pos_;
};



//--------------------------------------------------------------------
static void SolveTridiagSecularEquation(      BArray<double>& a_,
					      BArray<double>& f_,
					const BArray<double>& u_,
					const BArray<double>& d_,
					const double	       r_)

/*! Puts in a the zeroes of the secular equation
 *
 *		  f(x) = 1 + r * Sum(u(i)^2/(d(i)-x))
 */
//--------------------------------------------------------------------
{
  register int i,j,k,n=u_.Size();
  register double x,f,Df,dx;
  a_.ReallocBuffer(n);
  f_.ReallocBuffer(n);
//BTimer tm(BText("SolveTridiagSecularEquation ")+n);

  BArray< BIndexed<double> > index(n);
  double v = Abs(u_(0));
  for(i=0; i<n; i++)
  {
    index(i).dat_ = d_(i)*Sign(r_);
    index(i).pos_ = i;
  }
  BArray<double> u(n), d(n);
  BIndexed<double>::Sort(index);
//Std(BText("\nr_ = ")+r_);
  BArray<BInt>	order(n);
  for(j=0; j<n; j++)
  {
    i = index(j).pos_;
    order(i) = j;
    u(j) = u_(i);
    d(j) = index(j).dat_;
//  Std(BText("\nd[")+j+"]=\t"+d(j)+"\tu["+j+"]=\t"+u(j));
  }
  BTSEFunction F(u,d,1,1.0/Abs(r_));
  for(i=0; i<n; i++)
  {
    j = index(i).pos_;
    if(Abs(d(i))<Sqrt(BDat::Zero()))
    {
      a_(j) = 0;
    }
    else if((i<n-1)&&(Abs(d(i)-d(i+1))<Sqrt(BDat::Zero())))
    {
      Std(BText("\nRepeated \td(")+i+")="+d(i)+" \t~\t d("+(i+1)+")="+d(i+1)+
			  " \tu(" +i+")="+u(i)+" \t~\t u("+(i+1)+")="+u(i+1));
      a_(j) = d(i)*Sign(r_);
      f_(j) = 0;
    }
    else
    {
      double x0 = F.AproxSolve(i);
//    Std(BText("\n[")+i+" of "+n+"]\tu = "+u(i)+"\td = "+d(i)+"\tx0 = "+x0);
      x	 = x0;
      double x1=x-100, x2 = x-200, x3 = x-300, x4 = x-400;
      f = Df = dx = 1;
      F.calcDer_ = BTRUE;
      for(k=0; (k<5) ||
	       ((k<20+n)&&
		(x4!=x) && (x3!=x) && (x2!=x) &&(x1!=x) &&
		(f!=0)&&(Df!=0)); k++)
      {
	F.Evaluate(f,x);
	Df = F.Df_;
	dx = -f/Df;
//	Std(BText("\nNewton iteration ")+i+"\t"+k+"\t"+x+"\t"+f+"\t"+Df+"\t"+dx);
	x4 = x3; x3 = x2; x2 = x1; x1 = x;
	x += dx;
	if( (x<=d(i)) || ( (i<n-1) && (x>=d(i+1)) ) )
	{
	  x0 = F.AproxSolve(x0, i);
	  x = x0;
//	  x1=x-100, x2 = x-200, x3 = x-300, x4 = x-400;
//	  Std(BText("\nNewton reset \t")+x);
	}
      }
      f	 = F[x];
      Df = F.Df_;
//    Std(BText("\nNewton iteration ")+i+"\t"+k+"\t"+x+"\t"+f+"\t"+Df+"\t"+dx);
      a_(j) = x*Sign(r_);
      f_(j) = f;
    }
  }
}

//--------------------------------------------------------------------
void TridiagSEPDivideAndConquer(const BArray<double>&  a,
				const BArray<double>&  b,
				      DMat&		U,
				      BArray<double>&  d)
    
/*! Given a tridiagonal symmetric matrix A this routine computes its
 *  eigenvalues and eigenvectors
 *
 *	       A(i,i)	= a(i) para todo i=1..n
 *	       A(i,i-1) = b(i) para todo i=2..n
 *	       A(i+1,i) = b(i) para todo i=1..n-1
 *             A(i,j)	= 0    en otros casos
 *
 *	       A = U * D * U'
 *
 *             U' * U = I
 *
 *     The diagonal matrix of singular values D is output as a vector
 *     d[1..n].
 *
 *     The algorithm used is the Divide and Conquer Method.
 */
//--------------------------------------------------------------------
{
  BInt i, j, k, n = a.Size();
  if(n==0) { return; }
  U.Alloc(n,n);
  d.ReallocBuffer(n);
  if(n==1)
  {
    U(0,0) = 1;
    d(0) = a(0);
    return;
  }
  BInt n1 = n/2;
  BInt n2 = n-n1;
  DMat U1, U2;
  BArray<double> a1(n1), a2(n2), b1(n1-1), b2(n2-1), d1, d2, u(n), x, f;
  for(i=0; i<n1-1; i++)	    { a1(i) = a(i); b1(i) = b(i); }
  a1(i) = a(i); i++;
  for(j=0; i<n-1;  j++,i++) { a2(j) = a(i); b2(j) = b(i); }
  a2(j) = a(i);
  double r = b(n1-1);
  a1(n1-1) -= r;
  a2(0)	   -= r;
  TridiagSEPDivideAndConquer(a1,b1,U1,d1);
  TridiagSEPDivideAndConquer(a2,b2,U2,d2);
  DMat u_(n,1);
  U.SetAllValuesTo(0);
  for(i=0; i<n1; i++)
  {
    u_(i,0) = u(i) = U1(n1-1,i); d(i) = d1(i);
    for(k=0; k<n1; k++) { U(i,k) = U1(i,k); }
  }
  for(j=0; i<n;	 j++,i++)
  {
    u_(i,0) = u(i) = U2(0,j); d(i) = d2(j);
    for(k=n1; k<n; k++) { U(i,k) = U2(j,k-n1); }
  }

  DMat zero(n,1); zero.SetAllValuesTo(0);
  BSymMatrix<double> T(n);
  T.SetAllValuesTo(0);
  for(i=0;i<n;i++)
  {
    T(i,i) = a(i);
    if(i<n-1) { T(i+1,i)=b(i); }
  }
  BDiagMatrix<double> I(n,1);

  if(Abs(r)>BDat::Zero())
  {
    SolveTridiagSecularEquation(x,f,u,d,r);
    DMat V(n,n);
    BArray<DMat> v(n);
    DMat w(n,1);
    for(j=0; j<n; j++)
    {
      v(j).Alloc(n,1);
      double s = 0;
      for(i=0; i<n; i++)
      {
	double dif = d(i)-x(j);
	if(Abs(dif)>Sqrt(LEpsilon()))
	{
	  w(i,0) = v(j)(i,0) = u(i)/(d(i)-x(j));
	}
	else
	{
	  w(i,0) = v(j)(i,0) = 0;
	}
      }
      double vv = MtMSqr(v(j))(0,0);
      v(j) /= Sqrt(vv);
      w /= Sqrt(vv);
      DMat Uw = U*w;
      BSymMatrix<double> Ij,Tj;
      Ij = I*x(j);
      Tj = T-Ij;
      DMat TjUw = Tj*Uw;
      double TjUw2 = Sqrt(MtMSqr(TjUw)(0,0));
      if(TjUw2>BDat::Zero())
      {
	Std(BText("\nf(x(")+j+")="+x(j)+")="+f(j)+"\tTjw2="+TjUw2);
	Uw = TridiagMinimumResidualsSolve(Tj,zero,Uw);
	double ww = MtMSqr(Uw)(0,0);
	Uw /= Sqrt(ww);
	DMat TjUw_ = Tj*Uw;
	double TjUw2_ = Sqrt(MtMSqr(TjUw_)(0,0));
	Std(BText(" -> ")+TjUw2_);
	w = U.T()*Uw;
      }
      for(i=0; i<n; i++)
      {
	V(i,j) = w(i,0);
      }
    }
    U = U * V;
    d = x;
/*
    DMat U_(n,n);
    BInt jj;
    for(j=jj=0;j<n;j++)
    {
      if(Abs(x(j))>BDat::Zero().Value())
      {
	d(jj) = x(j);
	for(i=0;i<n;i++)
	{
	  U_(i,jj)=U(i,j);
	}
	jj++;
      }
    }
    U = U_.Sub(0,0,n,jj);
    d.ReallocateBuffer(jj);
*/
  }
};


//--------------------------------------------------------------------
void TridiagSEPDivideAndConquer(const BSymMatrix  <double>& T,
				      BMatrix     <double>& U,
				      BDiagMatrix <double>& D)

/*! Given a tridiagonal symmetric matrix T this routine computes its
 *  eigenvalues and eigenvectors
 *
 *			       T(i,i)	= a(i) para todo i=1..n
 *			       T(i,i-1) = b(i) para todo i=2..n
 *			       T(i+1,i) = b(i) para todo i=1..n-1
 *			       T(i,j)	= 0    en otros casos
 *
 *			       T = U * D * U'
 *
 *			       U' * U = I
 *
 *	     The algorithm used is the Divide and Conquer Method.
 */
//--------------------------------------------------------------------
{
  BInt i, j, n = T.Rows();
//BTimer tm(BText("TridiagSymmetricEigenvalueProblem ")+n);
//TRZ(T);
  BArray<double>  a_(n), b_(n-1), d_(n);
  for(i=0; i<n;	  i++) { a_(i) = T(i,  i); }
  for(i=0; i<n-1; i++) { b_(i) = T(i+1,i); }
  DMat U_;
  TridiagSEPDivideAndConquer(a_,b_,U_,d_);
//TRZ(U_);
//DMat U_tU_ = U_.T()*U_; TRZ(U_tU_);

  D.Alloc(n);
  U.Alloc(n,n);

  BArray< BIndexed<double> >index(n);
  for(i=0; i<n; i++)
  {
    index(i).dat_ = d_(i);
    index(i).pos_ = i;
  }
  BIndexed<double>::Sort(index);
  for(i=0; i<n; i++)
  {
    D(i,i) = d_(index(i).pos_);
    for(j=0; j<n; j++)
    {
      U(j,i) = U_(j,index(i).pos_);
    }
  }
};

//--------------------------------------------------------------------
void TridiagSEPDivideAndConquer(const BSymMatrix  <BDat>& T,
				      BMatrix     <BDat>& U,
				      BDiagMatrix <BDat>& D)

/*! Given a tridiagonal symmetric matrix T this routine computes its
 *  eigenvalues and eigenvectors
 *
 *
 *			       T(i,i)	= a(i) para todo i=1..n
 *			       T(i,i-1) = b(i) para todo i=2..n
 *			       T(i+1,i) = b(i) para todo i=1..n-1
 *			       T(i,j)	= 0    en otros casos
 *
 *
 *			       T = U * D * U'
 *
 *			       U' * U = I
 *
 *
 *	     The algorithm used is the Divide and Conquer Method.
 */
//--------------------------------------------------------------------
{
  TridiagSEPDivideAndConquer((const BSymMatrix <double>&)T,
                             (      BMatrix    <double>&)U,
                             (      BDiagMatrix<double>&)D);
}

//--------------------------------------------------------------------
void SEPInverseMethod(const BSymMatrix<double>& A,
		            double	         d0,
		            double&		 d,
		            BMatrix<double>&    x)
//--------------------------------------------------------------------
{
    BInt k, n = A.Rows();
    double dd = 0, dif;
    x.Alloc(n,1);
    for(k=0;k<n;k++) { x(k,0) = 1; }
    DMat y = x;
    BSymMatrix <double> A0 = A;
    for(k=0;k<n;k++) { A0(k,k) -= d0; }
    d = d0;
    k=0;
    DMat xOld, xDif;
    do
    {
	y = TridiagMinimumResidualsSolve(A0,x,y);
	xOld = x;
	x = y/Sqrt(MtMSqr(y)(0,0));
	xDif = x-xOld;
	dif = Sqrt(MtMSqr(xDif)(0,0));
	double old = d;
	d = (x.T()*A*x)(0,0);
	dd = Abs(d-old);
	Std(BText("\nSEPInverseMethod(")+d0+","+k+") -> "+d);
	k++;
    }
    while((k<n) && ((k<2) || (dif > BDat::Zero())));
    Std(BText("\nSEPInverseMethod(")+d+"\nx=\n"+x.Name());
}


//--------------------------------------------------------------------
void SEPInverseMethod(const BSymMatrix<BDat>& A,
		            BDat	      d0,
		            BDat&	      d,
		            BMatrix<BDat>&    x)
//--------------------------------------------------------------------
{
  SEPInverseMethod
  (
    (const BSymMatrix<double>&) A,
   *(double*)(&d0),
    (double&) d,
    (      BMatrix<double>   &) x
  );
}
