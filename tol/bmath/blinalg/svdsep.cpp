/* svdsep.cpp: Singular Value Decomposition and Symmetric Eigen Problem.
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

#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include <tol/tol_matrix.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_brealfun.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bprdist.h>

BTraceInit("svdsep.cpp");

//--------------------------------------------------------------------
static double LBDRecalcMatrixNorm(const BArray<double>& a,
				   const BArray<double>& b,
				         int		  j)
//--------------------------------------------------------------------
{
    double anorm;
    if(j==1) { anorm = a(1); }
    else if(j==2) {
	anorm = sqrt(a(1)*a(1)+b(2)*b(2)+a(1)*b(1)); 
    } else {
	anorm = sqrt(a(j-1)*a(j-1)+b(j)*b(j)+a(j-1)*b(j-1)+a(j)*b(j)); 
    }
    double ab = a(j)+b(j);
    if(anorm<ab) { anorm=ab; }
    return(anorm);
};


//--------------------------------------------------------------------
void LanczosBidiagonalization(const DMat&    Aa,
			                        const DMat&    p0,
			                              DMat&    U,
			                              DMat&    B,
			                              DMat&    V,
			                              double& anorm)
//--------------------------------------------------------------------
{
  BInt m = Aa.Rows   ();
  BInt n = Aa.Columns();
//BTimer tm(BText("LanczosBidiagonalization(")+m+"x"+n+")");
  BInt k = n;
  BInt i,j,h;
  if(!m || !n || (m<n)) { return; }


/*
//TRZ(Aa);
  BArray<BInt> PIV(n), PIVi(n);
  ColumnMaxAbsNormPivots(A,PIV);
  PivotByColumns(A, PIV);
//TRZ(A);
  InversePivots(PIV,PIVi,n);
*/

  double af	  = Aa.FrobeniusNorm();
  double norm2 = sqrt((double)n*m)*af;
  double eps1	= (m+n)*LEpsilon();
  double delta = Sqrt(LEpsilon()/k);
  double eta	  = 10*pow((double)LEpsilon(),0.75);
  BInt	  ret	  = 5;
/*
  Std(BText("\nEps   = ")+epsL_);
  Std(BText("\nEps1  = ")+eps1);
  Std(BText("\nDelta = ")+delta);
  Std(BText("\nEta   = ")+eta);
*/

  if(m!=p0.Rows	  ()) { return; }
  if(1!=p0.Columns()) { return; }

  BSpaMatrix<double> A=Aa, At=Aa.T();

  U.Alloc(m,  k+1); U.SetAllValuesTo(0);
  B.Alloc(k+1,k);   B.SetAllValuesTo(0);
  V.Alloc(k,  k);   V.SetAllValuesTo(0);
  B.SetAllValuesTo(0);

  DMat p(m,1), r(n,1);
  DMat* u  = new DMat[k+2];
  DMat* v  = new DMat[k+1];
  DMat* ut = new DMat[k+2];
  DMat* vt = new DMat[k+1];
  BSymMatrix<double> mu(k+1), nu(k+2);
//mu.SetAllValuesTo(0);
//nu.SetAllValuesTo(0);
  for(i=1; i<=k;   i++) { mu(i,i)=1; mu(i,0)=0; }
  for(i=1; i<=k+1; i++) { nu(i,i)=1; nu(i,0)=1; }

  BArray<double> b(k+2), a(k+1);
//Std("\n  0.0");
  a(0) = 0;
  b(1) = Sqrt(MtMSqr(p0)(0,0));
  u[1] = p0/b(1);
  ut[1] = u[1].T();
  v[0].Alloc(n,1);
  v[0].SetAllValuesTo(0);
  vt[1] = v[0].T();
//Std("\n  0.1");
  for(i=0; i<m; i++) { U(i,0) = u[1](i,0); }
//Std("\n  0.2");

  BInt iter = 0;
  BInt calc = 0;
  register double aux, sgn, muji, nuj1i, factor=1;
//Std("\n  0.3");
  BArray<BBool> Lmu(k+1), Lnu(k+2);
  for(j=1; j<=k;   j++) { Lmu(j) = BFALSE; }
//Std("\n  0.4");
  for(j=1; j<=k+1; j++) { Lnu(j) = BFALSE; }
//Std("\n  0.5");


  BBool fullreort = BFALSE;
  BInt k_=k;
  for(j=1; j<=n; j++)
  {
    if(fullreort)
    {
//    Warning("\nLANCZOS FULL REORTHOGONALIZATION");
      r	   = At*u[j] - v[j-1]*b(j);
      for(i=1; i<=j-1; i++)
      {
	r -= v[i]*(vt[i]*r)(0,0);
	calc++; iter++;
      }

      a(j) = Sqrt(MtMSqr(r)(0,0));
      {
//	v[j]   = r/a(j);

	long int ea;
	double fa = GetPowerScale(Abs(a(j)),ea);
	v[j].Alloc(n,1);
	for(i=0; i<n; i++)
	{
	  double sign = Sign(r(i,0))/Sign(a(j));
	  long int ev;
	  double fv = GetPowerScale(Abs(r(i,0)),ev);
	  v[j](i,0) = sign * (fv/fa) * Pow(2.0,ev-ea);
	}
	vt[j] = v[j].T();

      }

      p = A*v[j] - u[j]*a(j);
      for(i=1; i<=j; i++)
      {
	p -= u[i]*(ut[i]*p)(0,0);
	calc++; iter++;
      }

      b(j+1) = Sqrt(MtMSqr(p)(0,0));
      {
//	u[j+1] = p/b(j+1);

	long int eb;
	double fb = GetPowerScale(Abs(b(j+1)),eb);
	u[j+1].Alloc(m,1);
	for(i=0; i<m; i++)
	{
	  double sign = Sign(p(i,0))/Sign(b(j+1));
	  long int eu;
	  double fu = GetPowerScale(Abs(p(i,0)),eu);
	  u[j+1](i,0) = sign * (fu/fb) * Pow(2.0,eu-eb);
	}
	ut[j+1] = u[j+1].T();

      }
    }
    else
    {
      r	   =  At*u[j]- v[j-1]*b(j);
      a(j) = Sqrt(MtMSqr(r)(0,0));
      v[j]   = r*(1.0/a(j));
      vt[j]   = v[j].T();
      anorm = LBDRecalcMatrixNorm(a,b,j);
//    TRZ(u[j]);TRZ(v[j-1]);TRZ(BDat(b(j)));TRZ(r);TRZ(BDat(anorm));

      BBool neverLmu = BTRUE;
      for(i=1; i<=j-1; i++)
      {
	aux = b(i+1)*nu(j,i+1)+a(i)*nu(j,i)-b(j)*mu(j-1,i);
	sgn = (aux>=0)?1:-1;
	mu(j,i) = factor*(aux+sgn*eps1*norm2)/a(j);
  //	Std(BText("\nmu(")+j+","+i+")="+mu(j,i)+" : "+
  //		  (v[i].T()*v[j])(0,0));
	Lmu(i) = (factor*fabsl(mu(j,i))>=delta);
	neverLmu = neverLmu || Lmu(i);
      }

      for(i=ret; i<=j-1-ret; i++)
      {
	for(h=-ret;!Lmu(i)&&(h>=ret);h++)
	{
	  Lmu(i) = Lmu(i) || (fabsl(mu(j,i+h))>=eta);
	}
      }

      factor=1;
      double aux;
      for(i=1; i<=j-1; i++)
      {
	iter++;
	if(Lmu(i) || (neverLmu && Lnu(i)))
	{
	  muji = (vt[j]*v[i])(0,0);
	  aux = fabs(muji)/fabsl(mu(j,i));
	  if(factor<aux){ factor=aux; }
  //	  Std(BText("\nmu\t")+j+"\t"+i+"\t"+muji+"\t"+mu(j,i)+"\t"+factor);
	  r -= v[i]*(a(j)*muji);
	  mu(j,	 i) = LEpsilon()/2;
	  a(j) = Sqrt(MtMSqr(r)(0,0));
	  v[j]	 = r*(1.0/a(j));
	  anorm = LBDRecalcMatrixNorm(a,b,j);
	  calc++;
	}
      }
      a(j)    = Sqrt(MtMSqr(r)(0,0));
      v[j]    = r*(1.0/a(j));
      vt[j]   = v[j].T();
      p	      = A*v[j]	- u[j]*a(j);
      b(j+1)  = Sqrt(MtMSqr(p)(0,0));
      u[j+1]  = p*(1.0/b(j+1));
      ut[j+1] = u[j+1].T();

      anorm = LBDRecalcMatrixNorm(a,b,j);
      BBool neverLnu = BTRUE;
      for(i=1; i<=j; i++)
      {
	aux = a(i)*mu(j,i)+b(i)*mu(j,i-1)-a(j)*nu(j,i);
	sgn = (aux>=0)?1:-1;
	nu(j+1,i) = factor*(aux+sgn*eps1*norm2)/b(j+1);
  //	Std(BText("\nnu(")+(j+1)+","+i+")="+nu(j+1,i)+" : "+
  //		  (u[i].T()*u[j+1])(0,0));
	Lnu(i) = (factor*fabsl(nu(j+1,i))>=delta);
	neverLnu = neverLnu || Lnu(i);
      }

      for(i=ret; i<=j-ret; i++)
      {
	for(h=-ret;!Lmu(i)&&(h>=ret);h++)
	{
	  Lnu(i) = Lnu(i) || (fabsl(nu(j+1,i+h))>=eta);
	}
      }

      factor = 1;
      for(i=1; i<=j; i++)
      {
	iter++;
	if(Lmu(i) || (neverLnu && Lnu(i)))
	{
	  nuj1i = (ut[j+1]*u[i])(0,0);
	  aux = fabsl(nuj1i)/fabsl(nu(j+1,i));
	  if(factor<aux){ factor=aux; }
  //	  Std(BText("\nnu\t")+(j+1)+"\t"+i+"\t"+nuj1i+"\t"+nu(j+1,i)+"\t"+factor);
	  p -= u[i]*(b(j+1)*nuj1i);
	  nu(j+1,i) = LEpsilon()/2;
	  b(j+1) = Sqrt(MtMSqr(p)(0,0));
	  u[j+1] = p*(1.0/b(j+1));
	  anorm = LBDRecalcMatrixNorm(a,b,j);
	  calc++;
	}
      }
      b(j+1) = Sqrt(MtMSqr(p)(0,0));
      u[j+1] = p*(1.0/b(j+1));
      ut[j+1] = u[j+1].T();

      fullreort =
	(norm2 <= delta) ||
	(eps1*norm2>=a(j)*delta) ||
	(eps1*norm2>=b(j+1)*delta);
    }
    anorm = LBDRecalcMatrixNorm(a,b,j);
/*
    BText aj;  aj .Copy(a(j)  ,"%.18Lg");
    BText bj1; bj1.Copy(b(j+1),"%.18Lg");
      Std(BText("\n\nj=")+j+
		"\ta="+aj+
		"\tb="+bj1+
		"\t|A|="+anorm+
		"\tfactor="+1+"\n");
*/
    B(j-1, j-1) = a(j);
    B(j,   j-1) = b(j+1);
    for(i=0; i<m; i++) { U(i,j	) = u[j+1](i,0); }
    for(i=0; i<n; i++) { V(i,j-1) = v[j	 ](i,0); }

/*
    if( (Abs(a(j)) <= rminL_) || (Abs(b(j+1)) <= rminL_) )
    {
      k_=j;
      break;
    }
*/
/*
    if((j>n)&& (Abs(a(j)) <= Sqrt(epsL_)) || (Abs(b(j+1)) <= Sqrt(epsL_)) )
    {
      k_=j;
      break;
    }
*/
  }


  delete [] u;
  delete [] v;
  delete [] ut;
  delete [] vt;

  U = U.Sub(0,0,m,    k_+1);
  B = B.Sub(0,0,k_+1, k_);
  V = V.Sub(0,0,n,    k_);
/*
  Std(BText("\nLBD : Rows    = ")+m);
  Std(BText("\nLBD : Columns = ")+n);
  Std(BText("\nLBD : Range   = ")+k_);
  Std(BText("\nLBD : Iter    = ")+iter);
  Std(BText("\nLBD : Calc    = ")+calc);
  Std(BText("\nLBD : Ratio   = ")+(BDat(calc)/BDat(iter))+"\n");
*/
//PivotByColumns(V, PIVi);

}


//--------------------------------------------------------------------
void LanczosBidiagonalization(const BMat&   A,
			      const BMat&   p0,
			            BMat&   U,
			            BMat&   B,
			            BMat&   V,
			            BDat&   anorm)
//--------------------------------------------------------------------
{
  LanczosBidiagonalization
  (
    (const DMat&)A,
    (      DMat&)p0,
    (      DMat&)U,
    (      DMat&)B,
    (      DMat&)V,
    (    double&)anorm
  );
}


//--------------------------------------------------------------------
void LanczosBidiagonalization(const DMat&    A_,
			            DMat&    U_,
			            DMat&    B_,
			            DMat&    V_,
			            double& anorm_)
//--------------------------------------------------------------------
{
    BInt i,j;
    BInt m = A_.Rows   ();
    BInt n = A_.Columns();
    DMat p(m,1);
    BDiagMatrix<double> D;
    D.PutColumnsNorm(A_);
    
    for(i=0;i<m;i++)
    {
	//p(i,0) = BUniformDist::Random01().Value();
	p(i,0) = 0;
	for(j=0;j<n;j++)
	{
	    p(i,0) += A_(i,j)/(D(j,j)?D(j,j):1);
	}
    }
    LanczosBidiagonalization(A_, p, U_, B_, V_, anorm_);
    BInt r = B_.Columns();
    U_ = U_.Sub(0,0,m,r+1);
    B_ = B_.Sub(0,0,r+1,r);
}


//--------------------------------------------------------------------
void LanczosBidiagonalization(const BMat&   A_,
			            BMat&   U_,
			            BMat&   B_,
			            BMat&   V_,
			            BDat&   anorm_)
//--------------------------------------------------------------------
{
    BInt i,j;
    BInt m = A_.Rows   ();
    BInt n = A_.Columns();
    BMat p(m,1);
    BDiagMatrix<BDat> D;
    D.PutColumnsNorm(A_);
    
    for(i=0;i<m;i++)
    {
	p(i,0) = 0;
	for(j=0;j<n;j++)
	{
	    p(i,0) += A_(i,j)/(D(j,j).Value()?D(j,j):BDat(1));
	}
    }
    LanczosBidiagonalization(A_, p, U_, B_, V_, anorm_);
    BInt r = B_.Columns();
    U_ = U_.Sub(0,0,m,r);
    B_ = B_.Sub(0,0,r+1,r);
}


//--------------------------------------------------------------------
BInt TridiagSEPNegCount(const BSymMatrix<double>& A, double z)
//--------------------------------------------------------------------
{
  if(!A.Rows()||!A.Columns()) { return(0); }
  register double d = A(0,0)-z;
  register BInt i, n= A.Rows(), nc=(d<0);
  register long int nb, nd;
  register double b2, fb, fd = GetPowerScale(d , nd);
  for(i=1; i<n; i++)
  {
    b2 = A(i,i-1)*A(i,i-1);
    fb = GetPowerScale(b2,  nb);
    d  = A(i,i)-z- (fb/fd)*powl(2.0, nb-nd);
    fd = GetPowerScale(d, nd);
    nc += (d<0);

/*
    if(Abs(b2)>epsL_)
    {
      d = A(i,i)-z-b2/d;
      nc += (d<0);
    }
    else
    {
      d = A(i,i)-z;
      nc += (d<0);
    }
*/

  }
//Std(BText("\nTridiagSEPNegCount(A,")+z+")="+nc);
  return(nc);
}


//--------------------------------------------------------------------
BInt TridiagSEPNegCount(const BSymMatrix<BDat>& A, BDat z)
//--------------------------------------------------------------------
{
  double d = (A(0,0)-z).Value();
  BInt i, n= A.Rows(), nc=(d<0);
  long int nb, nd;
  double b, fb, fd = GetPowerScale(d,nd);
  for(i=1; i<n; i++)
  {
    b  = A(i,i-1).Value();
    fb = GetPowerScale(b, nb);
    d  = (A(i,i)-z).Value() - (fb*fb/fd)*powl(2.0,2*nb-nd);
    fd = GetPowerScale(d, nd);
    nc += (d<0);
/*
    if(Abs(b2)>BDat::Zero())
    {
      d = A(i,i)-z-b2/d;
      nc += (d<0);
    }
    else
    {
      d = A(i,i)-z;
      nc += (d<0);
    }
*/
  }
//Std(BText("\nTridiagSEPNegCount(A,")+z+")="+nc);
  return(nc);
}

static BInt  TSEPBCount = 0;

//--------------------------------------------------------------------
static void TridiagSEPBisection(const BSymMatrix<double>& A,
				      double a, BInt na, double b, BInt nb,
				      BArray<double>& d,
				      double tolerance)
//--------------------------------------------------------------------
{
    if(b<a)    { return; }
    if(na==nb) { return; }
    BInt i;
    double c = (a+b)/2;
    double ba = b-a;
    double bc = b-c;
    double ca = c-a;
//Std(BText("\nTridiagSEPBisection(")+TSEPBCount+", "+
//	    a+" <= "+c+" <= "+b+", "+ba+", "+ca+", "+bc+", ("+na+", "+nb+") )");
    if((b<=tolerance)||(ba==bc)||(!bc)||(!ca))
    {
	for(i=na; i<nb; i++) { d.Add(c); }
    }
    else
    {
	BInt nc = TridiagSEPNegCount(A,c);
	TridiagSEPBisection(A,a,na,c,nc,d,tolerance);
	TridiagSEPBisection(A,c,nc,b,nb,d,tolerance);
    }
}

//--------------------------------------------------------------------
BInt TridiagSEPBisection(const BSymMatrix<double>& A,
			       double a, double b,
			       BArray<double>& d,
			       double tolerance)
//--------------------------------------------------------------------
{
    TSEPBCount = 0;
    BInt na = TridiagSEPNegCount(A,a);
    BInt nb = TridiagSEPNegCount(A,b);
    d.AllocBuffer(A.Rows());
    d.AllocBuffer(0);
    TridiagSEPBisection(A,a,na,b,nb,d,tolerance);
    return(d.Size());
}


//--------------------------------------------------------------------
BInt TridiagSEPBisection(const BSymMatrix<BDat>& A,
			       BDat a, BDat b,
			       BArray<BDat>& d,
			       BDat tolerance)
//--------------------------------------------------------------------
{
  BInt r = TridiagSEPBisection
  (
    (const BSymMatrix<double>&)A,
    a.Value(),
    b.Value(),
    (BArray<double>&)d,
    tolerance.Value()
  );
  return(r);
}


//--------------------------------------------------------------------
void SymmetricEigenvalueProblem(const BSymMatrix<BDat>&   A,
				      BMat&		  U,
				      BDiagMatrix<BDat>&  D)
    
/*! Given a symmetric matrix A this routine computes its
 *  eigenvalues and eigenvectors
 *
 *				  A = U * D * U'
 *
 *				  U' * U = I
 */
//--------------------------------------------------------------------
{
};

//--------------------------------------------------------------------
void SymTridiagGivensProduct(const BMatrix <double>& V,
			     BInt i, double c, double s)
//--------------------------------------------------------------------
{
    BInt j, n = V.Rows(), m = V.Columns();
    double a, b;
    for(j=0; j<m; j++)
    {
	a = V(i,  j);
	b = V(i+1,j);
	V(i,  j) = c*a+s*b;
	V(i+1,j) =-s*a+c*b;
    }
}

//--------------------------------------------------------------------
void SymTridiagGivensSymProduct(BArray<double>& a,
				BArray<double>&b, double &p,
				BInt i, double c, double s)
//--------------------------------------------------------------------
{
    BInt n = a.Size();
    BArray<double> a_=a;
    BArray<double> b_=b;
    
    if(i>0)
    {
	b(i-1) = c*b_(i-1)-s*p;
    }
    a(i) = -s*(-s*a_(i+1)+c*b_(i  ))
	+c*( c*a_(i	)-s*b_(i  ));
    b(i) =  c*( s*a_(i	)+c*b_(i  ))
	   -s*( c*a_(i+1)+s*b_(i  ));
    a(i+1) =  s*( s*a_(i	)+c*b_(i  ))
	           +c*( c*a_(i+1)+s*b_(i  ));
    if(i+2<n)
    {
	p      = -s*b_(i+1);
	b(i+1) =  c*b_(i+1);
    }
}


//--------------------------------------------------------------------
void SymTridiagDeflation(const BArray  <double>& a,
			 const BArray  <double>& b,
			       double            Dj,
			       BArray  <double>& a_,
			       BArray  <double>& b_,
			       BArray  <double>& c,
			       BArray  <double>& s,
			       BMatrix <double>& V,
			       BInt	          j)
//--------------------------------------------------------------------
{
  BInt i, n = a.Size();
//BTimer tm(BText("SymTridiagDeflation(")+n+"x"+m);
//Std(BText("\nSymTridiagDeflation D")+Dj);
//TRZ(T);

  if(!n) { return; }

  if(n==1)
  {
    V(0,0) = 1;
    return;
  }

  double r = Abs(a(n-1));
  for(i=0;i<n-1;i++)

  {

    if(r>Abs(a(i))) { r = Abs(a(i)); }

    if(r>Abs(b(i))) { r = Abs(b(i)); }

  }

  long int e = 0;
  if( (r<rminL_/LEpsilon()) || (r>LEpsilon()/rminL_) )
  {
    //scale T and eigenvalues by a power of 2
    r = GetPowerScale(r,e);
    for(i=0; i<n;   i++) { a(i) = PowerScale(a(i), e); }
    for(i=0; i<n-1; i++) { b(i) = PowerScale(b(i), e); }
  }

  //Replace zero subdiagonals by the minimum real value
  for(i=0; i<n-1; i++) { if(b(i)==0) { b(i) = minL_; } }

  //Replacement for very small pivots in LDU and UDL factorizations
  double dmin = Maximum(rminL_,(rminL_*r)*r);

  BArray<double> d_(n);
  Dj = PowerScale(Dj, e);

    //Std(BText("\nlambda(")+j+") = "+D(j)+"*2^"+(int)e);
  for(i=0; i<n; i++) { d_(i) = a(i) - Dj; }

  //LDU factorization of T-lambda*I
  BArray<double> d1(d_), u1(n-1);
  if(Abs(d1(0))<dmin) { d1(0) = dmin; }
  for(i=1; i<n;	 i++)
  {
    long int nb, nd;
    double fb = GetPowerScale(b (i-1),nb);
    double fd = GetPowerScale(d1(i-1),nd);
    u1(i-1) = (fb   /fd)*powl(2.0,    nb-nd);
    d1(i)  -= (fb*fb/fd)*powl(2.0,2.0*nb-nd);
//    u1(i-1) = b(i-1)/d1(i-1);
//    d1(i)  -= b (i-1)*u1(i-1);
    if(Abs(d1(i))<dmin) { d1(i) = dmin; }
  }

  //UDL factorization of T-lambda*I
  BArray<double> d2(d_), l2(n-1);
  if(Abs(d2(n-1))<dmin) { d2(n-1) = dmin; }
  for(i=n-2; i>=0;  i--)
  {

    long int nb, nd;
    double fb = GetPowerScale(b (i  ),nb);
    double fd = GetPowerScale(d2(i+1),nd);
    l2(i)  = (fb   /fd)*powl(2.0,    nb-nd);
    d2(i) -= (fb*fb/fd)*powl(2.0,2.0*nb-nd);
//    l2(i) = b(i)/d2(i+1);
//    d2(i)  -= b(i)*l2(i);
    if(Abs(d2(i))<dmin) { d2(i) = dmin; }
  }
  // Optimal k and gamma
  double gamma = Abs(d1(0)+d2(0)-d_(0));
  int k = 0;
  for(i=0; i<n; i++)
  {
    double g = Abs(d1(i)+d2(i)-d_(i));
    if(Abs(g)<gamma)
    {
      gamma = Abs(g);
      k = i;
    }
/*
    Std(BText("\n\td1(")+i+")="+d1(i)+"\td2("+i+")="+d2(i)+
	      "\td("+i+")-lambda="+d(i)+
	      "\tg = "+g+"\tmin = "+gamma+"\tk = "+k);
*/
  }
//Std(BText("\nmin = ")+gamma+"\tk = "+k);

  BArray<double> g(n-1);
  a_=a;
  b_=b;
  c.AllocBuffer(n-1);
  s.AllocBuffer(n-1);
  double  p, f, sf, cf = 1;
  long int se, ce = 0,	ce1;

  for(i=0; i<n-1; i++)
  {
    if(i<k)
    {
      if(u1(i)!=0)
      {
	sf = GetPowerScale(u1(i),se);
      }
      else
      {
	sf = GetPowerScale(d1(i+1),se);
	 f = GetPowerScale(b(i), e);
	sf/=f;
	se-=e;
      }
    }
    else
    {
      if(l2(i)!=0)
      {
	sf = 1.0/GetPowerScale(l2(i),se);
	se = -se;
      }
      else
      {
	sf = GetPowerScale(d2(i+1),se);
	 f = GetPowerScale(b (i	 ), e);
	sf/=f;
	se-=e;
      }
    }
    e = Maximum(ce,se);
    ce -= e;
    se -= e;
    r  = Sqrt(cf*cf*powl(2,2*ce)+sf*sf*powl(2,2*se));
    cf = GetPowerScale(cf/r,ce1);
    ce += ce1;
    c(i) =  cf*powl(2,ce);
    s(i) = -(sf/r)*powl(2,se);
/*
    s(i) = -(i<=k)?u1(i):1.0/l2(i);
    c(i) = (i)?c(i-1):1;
    double sc = Sqrt(s(i)*s(i)+c(i)*c(i));
    c(i)/=sc;
    s(i)/=sc;
*/
    double sc = Sqrt(s(i)*s(i)+c(i)*c(i));
//  Std(BText("\n\nc(")+i+")="+c(i)+"\ts("+i+")="+s(i)+"\tsc="+sc);

    SymTridiagGivensSymProduct(a_,b_,p,i,c(i),s(i));
//  TRZ(t);
  }
//TRZ(t);

  V(k,j) = 1;
  for(i=k-1; i>=0; i--)
  {
    V(i,j) = -u1(i) * V(i+1,j);
  }
  for(i=k+1; i<n; i++)
  {
    V(i,j) = -l2(i-1) * V(i-1,j);
  }
//TRZ(V);
}


//--------------------------------------------------------------------
void SymTridiagIsolatedAutoVectors(const BArray  <double>& a,
				   const BArray  <double>& b,
				   const BArray  <double>& D,
				         BMatrix <double>& V)
//--------------------------------------------------------------------
{
    BInt j, n = a.Size(), m = D.Size();
//BTimer tm(BText("SymTridiagIsolatedAutoVectors(")+n+"x"+m);
//TRZ(T);
    
    if(!m || !n || (m>n)) { return; }
    
    V.Alloc(n,m);
    BArray<double> a_, b_, c, s;
    for(j=0; j<m; j++)
    {
	SymTridiagDeflation(a, b, D(j), a_, b_, c, s, V, j);
    }
}


//--------------------------------------------------------------------
void SymTridiagClusterAutoVectors(const BArray  <double>& a,
				  const BArray  <double>& b,
				  const BArray  <double>& D,
				        BMatrix <double>& V)
//--------------------------------------------------------------------
{
  BInt i, j, n = a.Size(), m = D.Size();
//BTimer tm(BText("SymTridiagClusterAutoVectors(")+n+"x"+m);
//TRZ(T);

  if(!m || !n || (m>n)) { return; }

  V.Alloc(n,m);
  BArray<double> a_, b_, c, s;
  BMatrix<double> V0(n,1);
  SymTridiagDeflation(a, b, D(0), a_, b_, c, s, V0, 0);
  if(m==1)
  {
    V = V0;
  }
  else
  {
    BArray     <double> D1(m-1), a1(n-1), b1(n-1);
//  DMat V1(n-1,m-1);
/*
    Std(BText("\n\n========================================\n\nDeflacted : ").
	Concat(D(0),"%.18Lf ").
	Concat(a_(n-1),"%.18Lf "));
*/
    for(i=0; i<m-1; i++) { D1(i) = D(i+1); }
    for(i=0; i<n-1; i++) { a1(i) = a_(i);   }
    for(i=0; i<n-2; i++) { b1(i) = b_(i);   }
    SymTridiagClusterAutoVectors(a1,b1,D1,V);
    DMat V1 = V;
//  TRZ(V1);
//  DMat V1tV1	 = V1.T()*   V1; TRZ(V1tV1  );
    V.Alloc(n,m);
    V.SetAllValuesTo(0);
    for(i=0; i<n-1; i++)
    {
      for(j=0; j<m-1; j++)
      {
	V(i,j) = V1(i,j);
      }
    }
    V(n-1,m-1)=1;
//  TRZ(V);
//  DMat VtV  = V.T()*	V; TRZ(VtV );
    for(i=n-2; i>=0; i--) { SymTridiagGivensProduct(V,i,c(i),s(i)); }
//  TRZ(V);
//  VtV	 = V.T()*  V; TRZ(VtV );
  }
}


//--------------------------------------------------------------------
BBool IsIsolated(const BArray<double> d, BInt i, double max)
//--------------------------------------------------------------------
{
    BInt n = d.Size();
//if((i==0)||(i==n-1)) { return(BFALSE); }
    double r = d(0)-d(n-1);
    double p = 100;
    double b = d(i);
    double a = (i==0)?2*b:d(i-1);
    double e = p*max/n;
    BBool leftIsolated  = a-b > e;
    double c = (i==n-1)?-b:d(i+1);
    BBool rightIsolated = b-c > e;
/*
  Std(BText("\n(")+a+","+b+","+c+")\t r="+r+"\t max="+max+
      "\t e="+e+
      "\t (a-b)="+(a-b)+
      "\t (b-c)="+(b-c));
*/
    return(leftIsolated && rightIsolated);
}

#define SVD_CHECK_DIM
  
//--------------------------------------------------------------------
void gsl_SingularValueDecomposition(const BMatrix<BDat>&     A_,
				          BMatrix<BDat>&     U_,
				          BDiagMatrix<BDat>& D_,
				          BMatrix<BDat>&     V_,
				          BText              method)
    
/*! Computes the singular value decomposition of A
 *
 *			    A = U * D * V'
 */
//--------------------------------------------------------------------
{
  BInt r = A_.Rows();
  BInt c = A_.Columns();
  BInt i, j;
  
  if (!r           || !c              || 
      U_.Rows()!=r || U_.Columns()!=c || 
      D_.Rows()!=c || D_.Columns()!=c ||
      V_.Rows()!=c || V_.Columns()!=c) {
#ifdef SVD_CHECK_DIM
    Error("Invalid dimensions calling SVD ...");
#endif
    return;
  }

  if (r==1 || c==1) 
  {
    double ai, norm = 0;
    BDat * buffer = ((BMatrix<BDat>&)A_).GetData().GetBuffer(), * bufferU;
    int size = ((BMatrix<BDat>&)A_).GetData().Size();

    // calculo la norma de la matriz(vector) A_ (fila o columna)
    for (i = 0; i<size; i++) {
      if (buffer[i].IsKnown()) {
	ai = buffer[i].Value();
	norm += ai*ai;
      }
    }
    norm = Sqrt(norm);
    // ahora construyo la matriz (vector) ortogonal U
    // tiene el mismo tamaño que A_
    bufferU = U_.GetData().GetBuffer();
    for (i = 0; i<size; i++) {
      if (buffer[i].IsKnown()) {
	bufferU[i].PutValue(buffer[i].Value()/norm);
      }
    }
    // contruyo la matriz diagonal D_
    // contante igual a 1/norm
    buffer = D_.GetData().GetBuffer();
    size = D_.GetData().Size();
    for (i = 0; i<size; i++) 
    {
      buffer[i].PutValue(norm);
    }
    // construyo V_ = Identidad.
    buffer = V_.GetData().GetBuffer();
    size = V_.GetData().Size();
    for (i = 0; i<size; i++) {
      buffer[i].PutValue(1.0);
    }
  } else {
    gsl_matrix * A     = gsl_matrix_alloc (r, c);
    gsl_matrix * V     = gsl_matrix_alloc (c, c);
    gsl_matrix * X     = gsl_matrix_alloc (c, c);
    gsl_vector * S     = gsl_vector_alloc (c);
    gsl_vector * work  = gsl_vector_alloc (c);
    
    for (i = 0; i < r; i++)
    {
      for (j = 0; j < c; j++)
      { 
	       gsl_matrix_set (A, i, j, A_(i,j).Value());
      }
    }
    
    if (method=="Golub_Reinsch")	
      int gsl = gsl_linalg_SV_decomp (A, V, S, work);
    
    if (method=="Golub_Reinsch_Mod")	
      int gsl = gsl_linalg_SV_decomp_mod (A, X, V, S, work);
    
    if (method=="Jacobi")	
      int gsl = gsl_linalg_SV_decomp_jacobi (A, V, S);
    for (i = 0; i < r; i++)
    {
      for (j = 0; j < c; j++)
      {
	       U_(i, j) = gsl_matrix_get (A, i, j);
      }
    }
    for (i = 0; i < c; i++)
    {
      D_(i, i) = gsl_vector_get (S, i);
    }
    for (i = 0; i < c; i++)
    {
      for (j = 0; j < c; j++)
      { 
	       V_(i, j) = gsl_matrix_get (V, i, j);
      }
    }
    //VBR: In order to return in a canonical way we multiplies the j-th column
    //     of U and V (u[j], v[j]) by the sign of the first non null value of 
    //     u[j][1], ... , u[j][r], v[j][1], ... , v[j][c]
    BArray<BDat> sign(c);
    for (j = 0; j < c; j++)
    {
      sign(j)=0;
      for(i=0; !sign(j).Value() && (i < r); i++)
      {
        sign(j) = Sign(U_(i, j));
      }
      for(i=0; !sign(j).Value() && (i < c); i++)
      {
        sign(j) = Sign(V_(i, j));
      }
      if(!sign(j).Value()) { sign(j)=1; }
    }
    for (i = 0; i < r; i++)
    {
      for (j = 0; j < c; j++)
      {
	       U_(i, j) *= sign(j);
      }
    }
    for (i = 0; i < c; i++)
    {
      for (j = 0; j < c; j++)
      { 
	       V_(i, j) *= sign(j);
      }
    }
    
    gsl_matrix_free (A); 
    gsl_matrix_free (V); 
    gsl_matrix_free (X); 
    gsl_vector_free (S); 
    gsl_vector_free (work); 
  }
  
}

//--------------------------------------------------------------------
void SingularValueDecomposition(const DMat&			A,
				      DMat&			U,
				      BDiagMatrix<double>&	D,
				      DMat&			V)

/*! Given a matrix A[1..m][1..n], this routine computes its singular
 *  value decomposition,
 *
 *			    A = U * D * V'
 */
//--------------------------------------------------------------------
{
//BTimer tm("SingularValueDecomposition");
  double eps1 = Sqrt(LEpsilon());

  BInt i,j,k,n = A.Columns(), m = A.Rows();
  DMat Ub, B, Vb;
  double norm;
  LanczosBidiagonalization(A,Ub,B,Vb,norm);
//TRZ(B); TRZ(Ub); TRZ(Vb);
  BInt rB = B.Columns();
//Std(BText("\nB(")+B.Rows()+","+B.Columns()+")");

  double TDisc = 0;
  // Building the Golub-Kahan matrix of B
  //
  //		  | a1			   |
  //		  | b1	a2		   |
  //	  B =	  |	b2  .		   |
  //		  |	    .  a(n-1)	   |
  //		  |	       b(n-1)  an  |
  //
  //
  //		  |  0	a1		 |
  //		  | a1	0  b1		 |
  //		  |    b1  0  a2	 |
  //	  T =	  |	  a2  0	 b2	 |
  //		  |	     b2	   .	 |
  //		  |		.    an	 |
  //		  |		  an  0	 |
  //
  //
  //		 |0  B|
  //	  T = P' |B' 0| P
  //
  //	  Con P = [ e(1),e(n+1),e(2),e(n+1), ... , e(n),e(2n) ]
  //

  BSymMatrix<double> T(2*rB+2); T.SetAllValuesTo(0);
  BInt rU=0, rV=0;
  for(k=0; k<rB; k++)
  {
//  Std(BText("\nT(")+ (2*k+1)	 +","+(2*k)+")=");
//  Std(BText("\tB(")+ k   +","+k+")="+B(k  ,k));
    T(2*k+1, 2*k) = B(k,k);
    if(k>0)
    {
      TDisc = Maximum(TDisc,Abs(B(k,k))+Abs(B(k,k-1)));
    }
    TDisc = Maximum(TDisc,Abs(B(k,k))+Abs(B(k+1,k)));
//  Std(BText("\nT(")+ 2*(k+1)	 +","+(2*k+1)+")=");
//  Std(BText("\tB(")+(k+1)+","+k+")="+B(k+1,k));
//  Std(BText("\tTDisc=")+TDisc);
    T(2*(k+1), 2*k+1) = B(k+1,k);

    if(Abs(B(k,k))>eps1) { rV++; }
    else { break; }
    if(Abs(B(k+1,k))>eps1) { rU++; }
    else { break; }

//  Std(BText("\trU=")+rU);
//  Std(BText("\trV=")+rV);
  }
//Std(BText("\nrB = ")+rB);
//Std(BText("\nrU = ")+rU);
//Std(BText("\nrV = ")+rV);
  if(!(rB*rU*rV)) { return; }

  Ub = Ub.Sub(0,0,m ,  rU+1);
  B  = B .Sub(0,0,rU+1,rV);
  Vb = Vb.Sub(0,0,n,   rV );
  BInt t = 1+rU+rV;
  T = T.Sub(0,0,t,t);
  BArray<double> a(t), b(t-1);
  for(j=0; j<t;	  j++) { a(j) = 0;	  }
  for(j=0; j<t-1; j++) { b(j) = T(j+1,j); }
//Std(BText("\nTDisc = ")+TDisc);
/*
  BArray<double> dz0;
  TridiagSEPBisection(T,rminD_,TDisc,dz0,0);
  BInt s0 = dz0.Size();
  for(j=0; j<s0; j++)
  {
    Std(BText("\ndz0(")+(j+1)+")="+dz0(s0-j-1));
  }
*/
  BArray<double> dz;
  {
//  BTimer tm2("Symmetric Tridiagonal Eigenvalue Bisection");
    TridiagSEPBisection(T,Sqrt(DEpsilon()),TDisc,dz,0);
  }
  BInt s = dz.Size();
  D.Alloc(s);
  for(j=0; j<s; j++) { D(j,j) = dz(s-j-1); }
  for(j=0; j<s; j++) { dz(j)  = D(j,j);	   }
 /*{
//TRZ(B);
  DMat UbtUb  = Ub.T()*Ub;   TRZ(UbtUb );
  DMat VbtVb  = Vb.T()*Vb;   TRZ(VbtVb );
  DMat UbtAVb = Ub.T()*A*Vb; TRZ(UbtAVb);
//DMat UbBVbt_A = Ub*B*Vb.T()-A; TRZ(UbBVbt_A);
  TRZ(T);
  TRZ(D);
  }*/
  BArray<BBool> isolated(s);
  for(j=0; j<s; j++)
  {
    isolated(j) = IsIsolated(dz,j,TDisc);
  }
  BArray< BArray<double> >block(s);
  BMatrix<double> Wb, W;
  block(0).ReallocBuffer(s);
  for(k=i=j=0; j<s; j++)
  {
//  Std((BText("\nd(")+(j+1)+")=").Concat(dz(j),"%.18Lg")+"\tisolated : "+isolated(j));
    if(isolated(j)!=isolated(i))
    {
//    Std(BText("\nblock(")+k+") va de "+i+" a "+(j-1)+" -> "+(j-i));
      block(k).ReallocBuffer(j-i);
      k++;
      block(k).ReallocBuffer(s-j);
      i=j;
    }
    block(k)(j-i) = dz(j);
  }
//Std(BText("\nblock(")+k+") va de "+i+" a "+(j-1)+" -> "+(j-i));
  block(k).ReallocBuffer(j-i);
  block.ReallocBuffer(k+1);
  {
//  BTimer tm3("Symmetric Tridiagonal Eigenvectos Deflation");
    for(i=k=0; k<block.Size(); k++)
    {
      if(isolated(i))
      {
	SymTridiagIsolatedAutoVectors(a,b,block(k),Wb);
      }
      else
      {
	DMat Wc;
	SymTridiagClusterAutoVectors(a,b,block(k),Wc);
  //	TRZ(Wc);
	Wb.Alloc(t,Wc.Columns());
	for(j=0; j<Wc.Columns(); j++)
	{
	  for(i=0; i<t; i++)
	  {
	    Wb(i,j) = Wc(i,Wc.Columns()-j-1);
	  }
	}
  //	TRZ(Wb);
      }
      i+=block(k).Size();
      W = W | Wb;
    }
  }
  DMat WB;
  BDiagMatrix<double> WD;
  DiagNormColumnsDecomp(W,WB,WD);
  W = WB;

  /*{
  DMat WtW    = W.T()*W;	    TRZ(WtW);
//DMat WWt    = W*(W.T());	    TRZ(WWt);
  DMat WtTW   = W.T()*T*W;	    TRZ(WtTW);
//DMat WDWt_T = (W*WtTW*(W.T()))-T; TRZ(WDWt_T);
  }*/

  BInt desp = (t%2)+t/2;
  BArray<BInt> perm(t);
  for(k=0;k<t/2;k++)
  {
    perm(2*k  ) = k;
    perm(2*k+1) = k+desp;
  }
  if(t%2) { perm(t-1) = t/2; }
//for(k=0; k<t; k++) { Std(BText("\nperm(")+k+") = "+perm(k)); }

  W = W.PermutateRows (perm);

  DMat Uw  = W .Sub(0,	  0, rU+1, s)*Sqrt(2.0);
  DMat Vw  = W .Sub(rU+1, 0, rV,   s)*Sqrt(2.0);

  U  = Ub * Uw;
  V  = Vb * Vw;
  /*{
  DMat WtW    = W.T()*W;	    TRZ(WtW);
  DMat UwtUw = Uw.T()*Uw; TRZ(Uw); TRZ(UwtUw);
  DMat VwtVw = Vw.T()*Vw; TRZ(Vw); TRZ(VwtVw);
  DMat UtU  = U.T()*U;	 TRZ(UtU);
  DMat VtV  = V.T()*V;	 TRZ(VtV);
  DMat UtTV = U.T()*A*V; TRZ(UtTV);
  DMat UDVt_A = U*D*V.T()-A; TRZ(UDVt_A);
  }*/
};


//--------------------------------------------------------------------
void SingularValueDecomposition(const BMat&		 A,
				      BMat&		 U,
				      BDiagMatrix<BDat>& D,
				      BMat&		 V)

/*! Given a matrix A[1..m][1..n], this routine computes its singular
 *  value decomposition,
 *    A = U * D * V'
 */
//--------------------------------------------------------------------
{
  SingularValueDecomposition
  (
    (const DMat               &)A,
    (      DMat               &)U,
    (      BDiagMatrix<double>&)D,
    (      DMat               &)V
  );
}
