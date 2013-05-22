/* nrealfun.cpp: Definition of class BRRFunction.
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
#include <tol/tol_brealfun.h>
#include <tol/tol_bfibonac.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bstat.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_bpolyn.h>
#include <tol/tol_bcomplex.h>
#include <tol/tol_bprdist.h>

#include <gsl/gsl_multifit.h>
#include <gsl/gsl_multimin.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_multifit_nlin.h>

BTraceInit("nrealfun.cpp");

static BDat marquardtFactor_       = 3;
static BDat mHouseHolder_	   = 1;
static BDat mGivens_		   = 2;
static BDat mCholeski_		   = 3;
static BDat mMRS_		   = 4;
static BDat mSVD_		   = 5;
static BDat marquardtLinearMethod_ = mHouseHolder_;
static const BChar* LinMetTable[] =
{
    "Householder",
    "Givens",
    "Choleski",
    "Minimum Residuals Solve (MRS)",
    "Singular Value Decomposition (SVD)"
};


BDat& MarquardtFactor       () { return(marquardtFactor_); }
BDat& MarquardtLinearMethod () { return(marquardtLinearMethod_); }

//--------------------------------------------------------------------
BDat& CGMaxIter()
    
/*! Internal access function to CGMaxIter TOL variable 
 */
//--------------------------------------------------------------------
{
  static BDat _CGMaxIter_=3;
  return(_CGMaxIter_);
}

BArray <BDat> arrayBDatDeclaration_;
// BMatrix<BDat> matrixBDatDeclaration_; ASC

//--------------------------------------------------------------------
class BProyectionFunction : public BRRFunction

/*! Evaluates the real to real function
 *
 *    f(x) = F(P + x H)
 *
 * where F is a real vector to real function and P is the initial point
 * and H is the direction vector.
 */
//--------------------------------------------------------------------
{
 public:
  BRnRFunction& F_;
  BArray<BDat>  P_;
  BArray<BDat>  H_;
  BProyectionFunction(BRnRFunction& F,
                      const BArray<BDat>& P,
                      const BArray<BDat>& H)
  : F_(F), P_(P), H_(H) {}
 ~BProyectionFunction() {};
  void Evaluate(BDat& f, const BDat& x)
  {
    BArray<BDat> Q = P_;
    for(int i=0; i<Q.Size(); i++) { Q(i) += x * H_(i); }
    f = F_[Q];
  }
};

//--------------------------------------------------------------------
class BPartialFunction : public BRRFunction

/*! Evaluates the real to real function
 *
 *    f(x) = F(P + x*E[k])
 *
 * where F is a real vector to real function and P is the initial point
 * and E[k] is the direction vector of k-th coordinate.
 */
//--------------------------------------------------------------------
{
 public:
  BRnRFunction& F_;
  BArray<BDat>  P_;
  int k_;
  BPartialFunction(BRnRFunction& F, const BArray<BDat>& P, int k)
  : F_(F), P_(P), k_(k) {}
 ~BPartialFunction() {};
  void Evaluate(BDat& f, const BDat& x)
  {
    BArray<BDat> Q = P_;
    Q(k_) += x; 
    f = F_[Q];
  }
};

//--------------------------------------------------------------------
class BPartialAbsDifFunction : public BRRFunction

/*! Evaluates the real to real function
 *
 *    f(x) = Abs(Abs(F(P + x*E[k])-F(P))-fDist)
 *
 * where F is a real vector to real function and P is the initial point
 * and E[k] is the direction vector of k-th coordinate.
 */
//--------------------------------------------------------------------
{
 public:
  BRnRFunction& F_;
  BArray<BDat>  P_;
  BDat fDist_;
  BDat fP_;
  int k_; 
  BPartialAbsDifFunction(BRnRFunction& F, const BArray<BDat>& P, BDat fDist, int k)
  : F_(F), P_(P), fDist_(fDist), k_(k) 
  {
    fP_ = F_[P_];
  }
 ~BPartialAbsDifFunction() {};
  void Evaluate(BDat& f, const BDat& x)
  {
    BArray<BDat> Q = P_;
    Q(k_) += x; 
    BDat f1 = Abs(Abs(F_[Q]-fP_)-fDist_);
    Q(k_) -= 2*x; 
    BDat f2 = Abs(Abs(F_[Q]-fP_)-fDist_);
    if(f1.IsUnknown()) { f=f2; }
    else if(f2.IsUnknown()) { f=f1; }
    else if(f1<=f2) { f=f1; }
    else { f=f2; }
  }
};


//--------------------------------------------------------------------
// BRnRFunction functions.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
bool BRnRFunction::AutoScale (
  const BArray<BDat>& x, 
  const BArray<BDat>& xMin, 
  const BArray<BDat>& xMax, 
  BDat fDist, 
  BDat tolerance,
  bool verbose, 
  BArray<BDat>& S)

/*! Returns the numerical Gradient vector of this function in a point
 */
//--------------------------------------------------------------------
{
  S.ReallocBuffer(n_);
  int k;
  fDist = Abs(fDist);
  for(k=0; k<n_; k++)
  {
    if(xMin[k].IsFinite() && xMin[k]==xMax[k]) { continue; }
    if(verbose)
    {
      Std(BText("  [BRnRFunction::AutoScale] Adjusting scale of variable ")+
          (k+1)+" of "+n_+" in ["+xMin[k]+","+xMax[k]+"] \n");
    }
    BPartialAbsDifFunction Fk(*this, x, fDist, k);
    BDat fd;
    BDat hMin = 0;
    BDat hMax = Maximum(Abs(x[k]-xMin[k]),Abs(x[k]-xMax[k]));
    if(hMax.IsFinite())
    {
      S[k] = BFibonacci::Minimum(&Fk,fd,hMin,hMax,tolerance);
    }
    else
    {
      S[k] = Abs(Fk.NewtonSolve(x[k], 0));
      if(!(S[k].IsFinite()))
      {
        BPartialFunction fk(*this, x, k);
        BDat x0 = x[k];
        BDat f0 = fk[x0];
        BDat ha = tolerance;
        BDat hb = tolerance;
        BDat a,b,fa,fb,adf=0;
        int iter = 0;
        do
        {
          iter++;
          a = x0-ha; 
          b = x0+hb; 
          fa = fk[a];
          fb = fk[b];
          if(!(fa.IsFinite())) { a = x0; fa=f0; ha=0; }
          if(!(fb.IsFinite())) { b = x0; fb=f0; hb=0; }
          adf = Abs(fb-fa);
          if(adf<fDist)
          {
            ha *= 2.0;
            hb *= 2.0;
          }
          else
          {
            ha *= 0.75;
            hb *= 0.75;
          }
        }
        while((Abs(adf-fDist)>tolerance) && (ha>0 || hb>0) && (iter<=100));
        if(iter<=100) { S[k] = ha+hb; }
        else { S[k] = 1; }
      }
    }
  }
  if(!(S[k].IsFinite())) { S[k]=1; }
  return(true);
/*
  BDat f, f0, df, a,b,c;
  BBool ok; 
  int i,j;
  (*this).Evaluate(f0, x); ok = f0.IsFinite();
  if(!ok) { return(false); }
  fDist = Abs(fDist);
  for(i=0; i<n_; i++)
  {
    BDat x0 = x[i];
    BDat h = Distance();
    x[i] = x0+h;
    (*this).Evaluate(f, x); ok = f.IsFinite();
    if(!ok) 
    { 
      h*=-1;
      x[i] = x0+h;
      (*this).Evaluate(f, x); ok = f.IsFinite();
    }
    df = Abs(f-f0);
    a=x0;
    b=x[i];
    for(j=0;j<maxIter;j++)
    {
      if(!ok) { break; }
      if(Abs(df-fDist)<=tolerance) { break; }
      else if(df>fDist)            { c; }
      else                         { s*=2; }
      x[i] = x0+h;
      (*this).Evaluate(f, x); ok = f.IsFinite();
      df = Abs(f-f0);
    }
    S[i]=Abs(h);
    x[i] = x0;
  }
*/
}

//--------------------------------------------------------------------
void BRnRFunction::Gradient (const BArray<BDat>& x, const BArray<BDat>& scale, BArray<BDat>& G)

/*! Returns the numerical Gradient vector of this function in a point
 */
//--------------------------------------------------------------------
{
  G.ReallocBuffer(n_);
  BDat f0, f1, f_1, f2, f_2; 
  BBool ok0,ok1,ok_1,ok2,ok_2;
  BDat h1 = Distance();
  int i;
  bool scaled = scale.Size()==x.Size();
  for(i=0; i<n_; i++)
  {
    BDat h = h1*((scaled)?scale[i]:1);
    x[i]+=h;
    (*this).Evaluate(f1, x); ok1 = f1.IsFinite();
    x[i]+=h;
    (*this).Evaluate(f2, x); ok2 = f2.IsFinite();
    x[i]-=3*h;
    (*this).Evaluate(f_1, x); ok_1 = f_1.IsFinite();
    x[i]-=h;
    (*this).Evaluate(f_2, x); ok_2 = f_2.IsFinite();
    x[i]+=2*h;
    if(ok_2&&ok_1&&ok1&&ok2)
    {
      G[i]=(f_2-8*f_1+8*f1-f2)/(h*12);
    }
    else
    {
      (*this).Evaluate(f0, x); ok0 = f0.IsFinite();
      if(ok_2&&ok_1&&ok0&&ok1)
      {
        G[i]=(-2*f_2+9*f_1-18*f0+11*f1)/(h*6);
      }
      else if(ok_1&&ok0&&ok1&&ok2)
      {
        G[i]=(-2*f_1-3*f0+6*f1-f2)/(h*6);
      }
      else if(ok_2&&ok_1&&ok0)
      {
        G[i]=(f_2-4*f_1+3*f0)/(h*2);
      }
      else if(ok0&&ok1&&ok2)
      {
        G[i]=(-3*f0+4*f1-f2)/(h*2);
      }
      else if(ok0&&ok1)
      {
        G[i]=(f1-f0)/h;
      }
      else if(ok0&&ok_1)
      {
        G[i]=(f0-f_1)/h;
      }
      else if(ok1&&ok2)
      {
        G[i]=(f2-f1)/h;
      }
      else if(ok_1&&ok_2)
      {
        G[i]=(f_1-f_2)/h;
      }
      else
      {
        G[i]=BDat::Unknown();
      }
    }
  }
}


//--------------------------------------------------------------------
void BRnRFunction::Gradient2 (const BArray<BDat>& x, const BArray<BDat>& scale, BArray<BDat>& G2)

/*! Returns the numerical second Gradient vector of this function in a point
 */
//--------------------------------------------------------------------
{
  G2.ReallocBuffer(n_);
  BDat f0, f1, f_1, f2, f_2; 
  BBool ok0,ok1,ok_1,ok2,ok_2;
  BDat h1 = Distance();
  int i;
  bool scaled = scale.Size()==x.Size();
  for(i=0; i<n_; i++)
  {
    BDat h = h1*((scaled)?scale[i]:1);
    BDat h2 = h*h;
    BDat h2_12 = 12*h2;
    BDat x0 = x[i];
    (*this).Evaluate(f0, x); ok0 = f0.IsFinite();
    x[i]=x0+h;
    (*this).Evaluate(f1, x); ok1 = f1.IsFinite();
    x[i]=x0+2*h;
    (*this).Evaluate(f2, x); ok2 = f2.IsFinite();
    x[i]=x0-h;
    (*this).Evaluate(f_1, x); ok_1 = f_1.IsFinite();
    x[i]=x0-2*h;
    (*this).Evaluate(f_2, x); ok_2 = f_2.IsFinite();
    x[i]=x0;
    if(ok_2&&ok_1&&ok0&&ok1&&ok2)
    {
      G2[i]=(-f_2+16*f_1-30*f0+16*f1-f2)/h2_12;
    }
    else if(ok_1&&ok0&&ok1)
    {
      G2[i]=(f_1-2*f0+f1)/h2;
    }
    else if(ok0&&ok1&&ok2)
    {
      G2[i]=(f0-2*f1+f2)/h2;
    }
    else if(ok_2&&ok_1&&ok0)
    {
      G2[i]=(f_2-2*f_1+f0)/h2;
    }
    else
    {
      G2[i]=BDat::Unknown();
    }
  }
}

//--------------------------------------------------------------------
void BRnRFunction::Gradient (const BArray<BDat>& x, BArray<BDat>& G)
//--------------------------------------------------------------------
{
  BArray<BDat> scale;
  Gradient(x,scale,G);
}

//--------------------------------------------------------------------
void BRnRFunction::Gradient2 (const BArray<BDat>& x, BArray<BDat>& G2)
//--------------------------------------------------------------------
{
  BArray<BDat> scale;
  Gradient2(x,scale,G2);
}

//--------------------------------------------------------------------
void BRnRFunction::Hessian(const BArray<BDat>& x, BMatrix<BDat>& H)

/*! Returns the numerical Hessian vector of this function in a point
 */
//--------------------------------------------------------------------
{
  H.Alloc(n_,n_);
  double h = Distance().Value();
  double h2_144 = 144*h*h;
  int i,j,ih,jh;
  BDat f0, f1, f_1, f2, f_2, fijh; 
  const double c[4][4] = {
    {+ 1, - 8, + 8, - 1},
    {- 8, +64, -64, + 8},
    {+ 8, -64, +64, - 8},
    {- 1, + 8, - 8, + 1} };
  const double dh[4] = 
    {+ 2, + 1, - 1, - 2};
  for(i=0; i<n_; i++)
  {
    for(j=0; j<=i; j++)
    {
      BDat dij=0;
      for(jh=0; jh<4; jh++)
      {  
        x[j]+=dh[jh]*h;
        for(ih=0; ih<4; ih++)
        {
          x[i]+=dh[ih]*h;
          (*this).Evaluate(fijh, x);
          dij += c[jh][ih]*fijh;
          x[i]-=dh[ih]*h;
        }
        x[j]-=dh[jh]*h;
      }
      H(i,j)=H(j,i)=dij/h2_144;
    }
  }

}


//--------------------------------------------------------------------
BDat BRnRFunction::Dimension ()
//
//
//--------------------------------------------------------------------
{ return(n_); }



//--------------------------------------------------------------------
BDat BInitialValueProblem::Dy(BDat x, BDat y)
//--------------------------------------------------------------------
{
    BArray<BDat> xy;
    xy.ReallocBuffer(2);
    xy[1]=x;
    xy[2]=y;
    BDat dy;
    Evaluate(dy,xy);
    return(dy);
}


//--------------------------------------------------------------------
BDat BInitialValueProblem::Primitive(BDat x)
//--------------------------------------------------------------------
{
    BInt i,size=4;
    BDat y;
    if(IsUpdated())
    {
	for(i=0; (i<net_.Size()) && (x>=net_[i].x_); i++) { }
	if(x==net_[i].x_) { y = net_[i].y_; }
	else
	{
	    i-=(size/2);
	    if(i<0) { i = 0; }
	    if(i+size>=net_.Size()) { size = net_.Size()-i-1; }
	    y = Solve(x,i,size);
	}
    }
    return(y);
}

//--------------------------------------------------------------------
BDat BInitialValueProblem::Solve(BDat x, BInt from, BInt size)
//--------------------------------------------------------------------
{
    BInt i,j;
    if(!size) { return(BDat::Unknown()); }
    BMatrix<BDat> Z(size,size);
    BMatrix<BDat> Y(size,1);
    BMatrix<BDat> C(size,1);
    for(i=0; i<size; i++)
    {
	Y(i,0) = net_[from+i].y_;
	Z(i,0) = 1;
	for(j=1; j<size; j++)
	{
	    Z(i,j) = Z(i,j-1)*net_[from+i].x_;
	}
    }
    TRZ(Z);
    TRZ(Y);
    Z.Solve(Y,C);
    TRZ(C);
    BDat y = 0;
    for(j=size-1; j>=1; j--)
    {
	y += C(j,0);
	y *= x;
    }
    y += C(0,0);
    return(y);
}


//--------------------------------------------------------------------
void BInitialValueProblem::RungeKuttaFehlberg(BDat x0, BDat y0, 
					      BDat x1,
					      BDat tolerance, 
					      BDat hmin, BDat hmax)
//--------------------------------------------------------------------
{
  BDat t = x0;
  BDat w = y0;
  BDat h = hmax;
  BInt n = net_.Size();
  while(t<x1)
  {
    BDat K1 = h*Dy(t,w);
    BDat K2 = h*Dy(t+h/4,w+K1/4);
    BDat K3 = h*Dy(t+3*h/8,w+3*K1/32+9*K2/32);
    BDat K4 = h*Dy(t+12*h/13,w+1932*K1/2197-7200*K2/2197+7296*K3/2197);
    BDat K5 = h*Dy(t+h,w+439*K1/216-8*K2+3680*K3/513-845*K4/4104);
    BDat K6 = h*Dy(t+h/2,w-8*K1/27+2*K2-3544*K3/2565+1859*K4/4104-11*K5/40);
    BDat R  = Abs(K1/360-128*K3/4275-2197*K4/75240+K5/50+2*K6/55)/h;
    if(R.IsKnown())
    {
      if(R<=tolerance)
      {
	if(n>=net_.Size()) { net_.ReallocBuffer(2*n); }
	net_.ReallocBuffer(n+1);
	net_[n].x_ = t;
	net_[n].y_ = w;
	t+=h;
	w+=25*K1/216+1408*K3/2565+2197*K4/4104-K5/5;
	n++;
      }
      BDat d  = 0.84*Sqrt(Sqrt(tolerance/R));
	   if(d<=0.1) { h *= 0.1;  }
      else if(d>=4.0) { h *= 4.0;  }
      else	      { h *= d;	   }
      if(h>hmax)      { h  = hmax; }
      if(h<hmin)
      {
	Warning(I2("Runge-Kutta-Fehlberg method has failed.",
		   "Fallo en el método de Runge-Kutta-Fehlberg."));
	break;
      }
      Std(BText("\nn=")+n+"\tt="+t.Name()+"\th="+h.Name()+
			  "\tw="+w.Name()+"\tR="+R.Name());
    }
    else
    {
      Warning(I2("Runge-Kutta-Fehlberg method failed. "
		 "The diferential equation has failed.",
		 "Fallo en el método de Runge-Kutta-Fehlberg."
		 "La ecuación diferencial ha fallado."));
      break;
    }
  }
}


//--------------------------------------------------------------------
// BRnRmFunction functions
//--------------------------------------------------------------------
void BRnRmFunction::SetDimensions(BInt n, BInt m)
{
  // HERE WE SHOULD VERIFY the arguments n > 1, m > 1
    n_ = n;
    m_ = m;
    gsl_X.AllocBuffer(n_);
    gsl_Fx.AllocBuffer(m_);
    gsl_FStep.AllocBuffer(m_);
}

//--------------------------------------------------------------------
void BRnRmFunction::Jacobian (const BArray<BDat>&  x,
			      const BArray<BDat>&  fx,
			            BMatrix<BDat>& J)

/*! Returns the numerical Jacobian vector of this function in a point
 *  calculated as
 *
 *		(f(x+h)-f(x))/h
 */
//--------------------------------------------------------------------
{
  //BTimer tmJacobian("Numerical jacobian matrix");
    BArray<BDat> fxh(m_);
    J.Alloc(m_,n_);
    for(BInt i=0; i<n_; i++)
    {
 	x(i)+=Distance();
	Evaluate(fxh,x);
	x(i)-=Distance();
	for(BInt j=0; j<m_; j++)
	{
	    J(j,i)=(fxh(j)-fx(j))/Distance();
	}
    }
}


//--------------------------------------------------------------------
BDat BRnRmFunction::LeastSqrGaussNewton(BArray<BDat>& x,
					BArray<BDat>& r,
					BMatrix<BDat>& J)

/*! Minimizes by Gauss-Newton algorithm (Swartz, 7.4.1, 320)
 *
 *	     Trasp(F(x))*F(x) = Sum(Fi(x)^2)
 */				 
//--------------------------------------------------------------------
{
  BInt i;
  BArray<BDat> R, T, epsilon(n_),y;
  BInt	iter = 0;
  BBool ok = BTRUE;
  BDat	norm, oldNorm, advance=BDat::Tolerance();
  J.Alloc(m_,n_);
  do
  {
    iter++;
    Evaluate(y,x);
    oldNorm = norm;
    norm    = FrobeniusNorm(y);
    Std(BText("\nEstandard error = ")+norm.Name());
    if(oldNorm.IsKnown())
    {
      advance = oldNorm-norm;
      Std(BText("\nAdvance = ")+advance.Name());
    }
    Std("\n================================================================");
    Std(BText("\nGauss-Newton Iteration = ")+iter);
    if(advance>=BDat::Tolerance())
    {
      Jacobian(x, y, J);
      if(ok=HouseholderTransformation(T,J))
      {
		  LeastSqrHouseholder(T,J,y,epsilon,R);
	for(i=0; i<n_; i++) { x[i]+=epsilon[i]; }
      }
    }
  }
  while(ok && (iter<BDat::MaxIter()) && (advance>=BDat::Tolerance()));
  if(iter==BDat::MaxIter())
  {
    Warning(BText("GaussNewton ")+
	    I2("was interrupted at maximum iteration ",
	       "fue interrumpido en la iteración maxima ")+
	    (BInt)(BDat::MaxIter().Value()));
  }
  if(ok)
  {
    for(i=0; i<m_; i++) { r[i]=-R[i]; }
  }
  else
  {
    Error(BText("GaussNewton ")+
	  I2("failed at iteration ", "fallo en la iteracion ")+
	  iter);
  }
  return(norm);
}



//--------------------------------------------------------------------
BDat NonSquareLargestEigenValue(const BMatrix<BDat>&	J_,
                                      BMatrix<BDat>& v,
                                      BMatrix<BDat>& u)
    
/*! 
 */				 
//--------------------------------------------------------------------
{
  BInt i, n = J_.Columns(), m = J_.Rows();
  if(!n ||!m) { return(BDat::Unknown()); }
//TRZ(J);
  BDat norm = J_.FrobeniusNorm()*Sqrt(n*m);
  BMatrix<BDat> w, r(1,n), J = J_/norm, Jt=J.T();
  for(i=0; i<n; i++) { r(0,i) = BUniformDist::Random01(); }
  BDat lambda=1, d=1, c = (r*v)(0,0);
  i = 0;
  for(i=1; i<=m+n; i++)
  {
    u = J*v;  
    w = Jt*u; 
    d = (r*v)(0,0);
    c = (r*w)(0,0);
    lambda = c/d;
    BDat err = FrobeniusNorm((w-v*lambda).Data());
  //Std(BText("\nNonSquareLargestEigenValue iteration(")<<i<<") = "<<norm*Sqrt(lambda)<<
  //    " err = "<<err /*<<
  //    "\nv = " << v.T().Name()<<
  //    "\nu = " << u.T().Name() */);
    if(err<DEpsilon()) { break; }
    d = Sqrt(MtMSqr(w)(0,0));
    v = w/d;
  }
  v /= Sqrt(MtMSqr(v)(0,0));
  u /= Sqrt(MtMSqr(u)(0,0));
  return(Sqrt(lambda)*norm);
}

//--------------------------------------------------------------------
  void FindRoot3(const BPolyn<BDat> p, BDat& z1, BComplex& z2, BComplex& z3)
/*! 
 */				 
//--------------------------------------------------------------------
{
  long double a     = p.Coef(3).Value();
  if(a==0) {  return; }
  long double b     = p.Coef(2).Value()/a;
  long double c     = p.Coef(1).Value()/a;
  long double d     = p.Coef(0).Value()/a;
  a = 1;
  long double aux1  = 3.0*a*c-b*b;
  long double aux2  = -2.0*b*b*b+9.0*a*b*c-27.0*a*a*d;
  long double aux3a = 4.0*Pow(aux1,(long double)3)+Pow(aux2,(long double)2);
  BComplex    aux3  = Sqrt(BComplex::RC(aux3a));
  BComplex    aux4a = aux2+aux3;
  BComplex    aux4  = aux4a^(1.0/3.0);
  long double k1    = Pow(2.0,1.0/3.0);
  BComplex    k2      (k1/2.0, Sqrt(3.0)*k1/2.0);
  BComplex    k3      (k1/2.0,-Sqrt(3.0)*k1/2.0);
  BComplex    x1    = (-b - aux1*k1/aux4 + aux4/k1)/(3.0*a);
  BComplex    x2    = (-b + aux1*k2/aux4 - aux4/k2)/(3.0*a);
  BComplex    x3    = (-b + aux1*k3/aux4 - aux4/k3)/(3.0*a);
  BDat        minY  = Minimum( Minimum(Abs(x1.Y()),Abs(x2.Y())),Abs(x3.Y()));
       if(Abs(x1.Y())==minY) {  z1 = x1.X(); z2 = x2; z3 = x3; }
  else if(Abs(x2.Y())==minY) {  z1 = x2.X(); z2 = x1; z3 = x3; }
  else                       {  z1 = x3.X(); z2 = x2; z3 = x1; }
}

//--------------------------------------------------------------------
   BDat BRnRmFunction::OneWaySearch(      BMatrix <BDat>& x1,
                                          BMatrix <BDat>& y1,
                                    const BMatrix <BDat>& x,
                                    const BMatrix <BDat>& y,
                                          BInt iter)
/*! 
 */				 
//--------------------------------------------------------------------
{
  BInt i, k;
  BDat u;
  BInt m_ = y.Rows();
  BInt n_ = x.Rows();
//BTimer tm(BText("One Way Search(")+m_+","+n_+")");
  BMatrix<BDat> dx, x2, y2(m_,1), xu, yu(m_,1);
  BPolyn<BDat> p, sp2=BPolyn<BDat>::Zero(), dsp2;

  dx = x1-x;

  BDat norm   = FrobeniusNormU(y.Data());
  BDat norm1  = FrobeniusNormU(y1.Data());
  BDat normdx = FrobeniusNormU(dx.Data());
//TRZ(dx); TRZ(normdx);  TRZ(x);   TRZ(x1); TRZ(norm); TRZ(norm1); 
  if(!normdx) { return(norm); }

//Std(I2("\n   One Way Search",
//       "\n   Búsqueda Unidireccional"));
  BDat h = 1;
  BDat advance = 1;
  BInt interIter = 0;
  BInt hDiv = 2;
  while((norm.IsUnknown() || (norm.Value()==GSL_POSINF) || (norm1>norm))&&
//while((norm.IsUnknown() || (norm.Value()==GSL_POSINF) )&&
        (advance>BDat::Tolerance())&&
        (h>=Sqrt(DEpsilon())))
  {
    h /= hDiv;
    hDiv *= 2*hDiv;
    x1 = x + dx*h;
    norm1 = FrobeniusNormU(x1.Data());
    if(norm1.IsKnown())
    {
      Evaluate(y1.GetData(), x1.Data());
      norm1 = FrobeniusNormU(y1.Data());
      advance = norm1-norm;
    }
  }
  if(norm1.IsUnknown() || (norm1.Value()==GSL_POSINF)) 
  { 
  //Std(I2(" has failed."," ha fallado."));
    norm1 = norm; x1 = x; y1 = y;
    return(norm1); 
  }
  x2 = x + dx*(h*0.5);
  BDat norm2 = FrobeniusNormU(x2.Data());
  if(norm2.IsKnown())
  {
    Evaluate(y2.GetData(), x2.Data());
    norm2 = FrobeniusNormU(y2.Data());
  }
//TRZ(x2);   TRZ(norm2); 
  if(norm2.IsUnknown() || (norm2.Value()==GSL_POSINF))
  {
    if(norm<norm1) 
    { 
    //Std(I2(" has failed."," ha fallado."));
      norm1 = norm; x1 = x; y1 = y; 
    }
  //Std(I2(" norm = "," norma = ")<<norm1);
    return(norm1);
  }
  else
  {
    p.AllocBuffer(3);
    for(k=0; k<p.Size(); k++) { p(k).PutDegree(k); }
    for(i=0; i<m_; i++)
    {
      p(0).PutCoef(   y(i,0)                    );
      p(1).PutCoef(-3*y(i,0) - 1*y1(i,0) + 4*y2(i,0));
      p(2).PutCoef( 2*y(i,0) + 2*y1(i,0) - 4*y2(i,0));
      sp2 += p^2;
    }
    sp2 /= m_;
    dsp2 = sp2.Derivate();
    BDat normu;
  //TRZ(dsp2);
    if(dsp2.Degree()==3)
    {
      BArray<BComplex> z(3);
      FindRoot3(dsp2, u, z[1], z[2]);
	  z[0] = BComplex::RC(u);
      BDat fu;
      for(k=0; k<z.Size(); k++)
      {
        BDat u2 = z[k].X();
        if(z[k].Y()<100*DEpsilon())
        {
          BDat f2 = sp2.Eval(u2);
          if(fu.IsUnknown() || (f2<fu)) { u = u2; fu = f2; }
        }
      }
      if(fu.IsUnknown()) { fu = sp2.Eval(u); }

      xu = x+dx*(h*u);
      normu = FrobeniusNormU(xu.Data());
      if(normu.IsKnown())
      {
        Evaluate(yu.GetData(), xu.Data());
      //TRZ(u); TRZ(h); TRZ(x); TRZ(yu); 
        normu = FrobeniusNormU(yu.Data());
      //TRZ(xu); TRZ(normu); 
      }
    }
    if(normu.IsUnknown() || (normu.Value()==GSL_POSINF)) { normu = norm1; xu=x1; yu=y1; u=1; }
  //TRZ(sp2);TRZ(dsp2);
  //TRZ(norm); TRZ(norm1); TRZ(norm2); TRZ(normu);

    if(norm1>norm ) { x1 = x;  y1 = y;  norm1 = norm ; }
    if(norm1>norm2) { x1 = x2; y1 = y2; norm1 = norm2; }
    if(norm1>normu) { x1 = xu; y1 = yu; norm1 = normu; }
  }
//Std(I2(" norm = "," norma = ")<<norm1);
  return(norm1);
}


//--------------------------------------------------------------------
BBool LinearStep(      BMatrix<BDat>& dX,
                 const BMatrix<BDat>& J,
                 const BMatrix<BDat>& dY)
//--------------------------------------------------------------------
{
  BInt r = J.Rows(), c = J.Columns();
  dX = gsl_MinimumResidualsSolve(J,-dY);
  BMatrix<BDat> test = J.T()*(J*dX+dY);
  BDat testErr = FrobeniusNormU(test.Data());
  BBool ok = testErr < Sqrt(DEpsilon());
/*
  if(testErr>Sqrt(DEpsilon()))
  {
    
    BMatrix<BDat> U(r, c), V(c, c), B;
    BDat anorm;
    LanczosBidiagonalization(J, U, B, V, anorm);
    dX = V*gsl_MinimumResidualsSolve(B,-(dY.T()*U).T());
    test = J.T()*(J*dX+dY);
    testErr = FrobeniusNormU(test.Data());
    ok = testErr < Sqrt(DEpsilon());
  }
/* */
  if(!ok)
  {
    Warning(I2("Lost accurate solving linear step of Marquardt.",
               "Pérdida de precisión en el paso lineal de Marquardt.")<<
            "("<< testErr <<")");
  }
  return(ok);
}




//--------------------------------------------------------------------
BDat GaussNewtonStep(      BRnRmFunction& fun,
                     const BMatrix<BDat>& J,
                           BMatrix<BDat>& x,
                           BMatrix<BDat>& y,
                     const BMatrix<BDat>& dY)
//--------------------------------------------------------------------
{
  Std(BText("\n  Gauss Newton Step"));
  BMatrix<BDat> dX;
  BDat norm;
  LinearStep(dX,J,dY);
  x+=dX;
  norm = FrobeniusNormU(x.Data());
  if(norm.IsKnown())
  {
    fun.Evaluate(y.GetData(), x.Data());
  }
  norm = FrobeniusNormU(y.Data());
  return(norm);
}


//--------------------------------------------------------------------
BDat OrtegaRheinboldtStep(      BRnRmFunction& fun,
		                        const BMatrix<BDat>& J,
			                             BMatrix<BDat>& x,
			                             BMatrix<BDat>& y,
		                        const BMatrix<BDat>& dY1)
//--------------------------------------------------------------------
{
  Std(BText("\n  Gauss Newton Step"));
  BInt m = y.Rows();
  BInt r = J.Rows(), c = J.Columns();
  BDat norm = FrobeniusNormU(y.Data());

  BMatrix<BDat> dX1, x1, y1(m,1); BDat norm1;
  
  if(!LinearStep(dX1,J,dY1))
  {
    return(norm);
  }
  else
  {
    x1 = x+dX1;
    fun.Evaluate(y1.GetData(), x1.Data());
    norm1 = FrobeniusNormU(y1.Data());
    norm = norm1;
  //TRZ(dY1.T()); TRZ(x.T()); TRZ(dX1.T()); TRZ(x1.T()); TRZ(y1.T()); TRZ(norm1); 
    Std(BText("\n  Ortega Rheinboldt Step"));
    BMatrix<BDat> dX2, dY2, x2, y2(m,1); BDat norm2;
         if(r==m  ) { dY2 = y+dY1; }
    else if(r==m+c) { dY2 = (y1<<x)+dY1; }
    LinearStep(dX2,J,dY2);
    x2 = x+dX2;
    fun.Evaluate(y2.GetData(), x2.Data());
    norm2 = FrobeniusNormU(y2.Data());
    if(norm2<norm) { norm = norm2; }
         if(norm1==norm) { x = x1; y = y1; }
    else if(norm2==norm) { x = x2; y = y2; }
    return(norm);
  }
}

//--------------------------------------------------------------------
BDat NewtonStep(      BRnRmFunction& fun,
                const BMatrix<BDat>& J,
                      BMatrix<BDat>& x,
                      BMatrix<BDat>& y,
                const BMatrix<BDat>& dY)
//--------------------------------------------------------------------
{
  return(GaussNewtonStep     (fun,J,x,y,dY));
//return(OrtegaRheinboldtStep(fun,J,x,y,dY));
}


//--------------------------------------------------------------------
BDat ComputeLambda(      BRnRmFunction& fun,
		                 const BMatrix<BDat>& J,
                         BMatrix<BDat>& x,
                         BMatrix<BDat>& y,
		                 const BMatrix<BDat>& dY,
			                      BDat	        lambda)
//--------------------------------------------------------------------
{
//BTimer tm("Linear optimization");
  Std(BText("\n Tikhonov regularization lambda=")<<lambda.Value());
  BInt n = J.Columns();
  BDiagMatrix<BDat> L; 
  BMatrix<BDat> Jn;
  DiagNormColumnsDecomp (J, Jn, L);
  L *= lambda;
  BMatrix<BDat> J_ = J << L; 
  BMatrix<BDat> zero(n,1); zero.SetAllValuesTo(0);
  BMatrix<BDat> dY_ = dY << zero;
  return(NewtonStep(fun,J_,x,y,dY_));
}


//--------------------------------------------------------------------
BDat LeastSquareLambda(      BRnRmFunction& fun,
		                     const BMatrix<BDat>& J,
                             BMatrix<BDat>& x,
                             BMatrix<BDat>& y,
		                           BMatrix<BDat>& dY,
		                           BDat&	         lambda,
		                           BInt	          iter,
		                           BInt&	         interIter)
//--------------------------------------------------------------------
{
  BDat v  = marquardtFactor_;
  BDat norm1, advance, norm = FrobeniusNormU(y.Data()), oldNorm1;
  BDat lambdaE, normE, norm2;
    
  BMatrix<BDat> x1, y1;
  clock_t tm_0, tm_1;
  BReal tm_dif;
    
  BBool ok = BFALSE;
    
  tm_0 = BTimer::Clocks();
  for(interIter; !ok && (interIter<=BDat::MaxIter()); interIter++)
	 {
	   BDat oldNorm1 = norm1;
    x1=x;
    y1=y;
	   norm1 = ComputeLambda(fun, J, x1, y1, dY, lambda);
	   advance = norm1-norm;
    ok = norm1.IsKnown() && (norm1.Value()!=GSL_POSINF) && (advance<0);
	   BDat relAdv = 100*advance/norm;
	   tm_1 = BTimer::Clocks();
	   tm_dif = BTimer::ClockToSecond(tm_1-tm_0);
	   tm_0 = tm_1;
	   if (!(interIter%1)) 
    {
		    Std(I2("\n    Subiteration(","\n    Subiteración(")<<
		           TxtFmt(iter,"%2ld")<<", "<< TxtFmt(interIter,"%2ld") << ")"<<
		           " Lambda : " << lambda <<
		           I2("  norm : ","  norma : ") << norm1 <<
		           " - " << norm<<" = "<< advance<<
		           " ("<<relAdv.Format("%6.3lf")<<"%) : "<<
		           I2("\tTime : "," Tiempo : ") << 
		           tm_dif<<I2(" seconds"," segundos"));
	   }
	   if(Abs(relAdv )	< BDat::RelTolerance()) { break; }
	   if(Abs(advance) < BDat::Tolerance   ()) { break; }  
    if(1.0/lambda<Sqrt(DEpsilon()))         { break; }
	   lambda *= v;
  }
  if(ok)
  {
    x = x1;
    y = y1;
    norm = norm1;
  }
  return(norm);
}


//--------------------------------------------------------------------
BDat GaussNewtonStepWithSVD(      BRnRmFunction&     fun,
                            const BMatrix<BDat>&     U,
                            const BDiagMatrix<BDat>& D,
                            const BMatrix<BDat>&     V,
                            const BDiagMatrix<BDat>& Djp,
                                  BMatrix<BDat>&     x,
                                  BMatrix<BDat>&     y,
                            const BMatrix<BDat>&     dY)
//--------------------------------------------------------------------
{
//Std(BText("\n  Gauss Newton Step"));
  BDiagMatrix<BDat> Dp = D.P(Sqrt(DEpsilon()));
  BMatrix<BDat> dX = -(Djp*V)*(Dp*(dY.T()*U).T());
  x+=dX;
  BDat norm = FrobeniusNormU(x.Data());
  if(norm.IsKnown())
  {
    fun.Evaluate(y.GetData(), x.Data());
  }
  norm = FrobeniusNormU(y.Data());
//Std(I2(" norm = ", " norma ")<<normL);
  return(norm);
}


//--------------------------------------------------------------------
BDat OrtegaRheinboldtStepWithSVD(      BRnRmFunction&     fun,
                                 const BMatrix<BDat>&     U,
                                 const BDiagMatrix<BDat>& D,
                                 const BMatrix<BDat>&     V,
                                 const BDiagMatrix<BDat>& Djp,
  			                                  BMatrix<BDat>&     x,
		  	                                  BMatrix<BDat>&     y,
		                               const BMatrix<BDat>&     dY1)
//--------------------------------------------------------------------
{
  Std(BText("\n  Gauss Newton Step"));
  BInt m = y.Rows();
  BInt r = U.Rows(), c = V.Columns();
  BDat norm = FrobeniusNormU(y.Data());
  BDiagMatrix<BDat> Dp = D.P(Sqrt(DEpsilon()));
  BMatrix<BDat> dX1, x1, y1(m,1); BDat norm1;
  dX1 = -(Djp*V)*(Dp*(dY1.T()*U).T());
  x1 = x+dX1;
  fun.Evaluate(y1.GetData(), x1.Data());
  norm1 = FrobeniusNorm(y1.Data());
  Std(I2(" norm = ", " norma ")<<norm1.Value());
  if(norm1<norm)
  {
    norm = norm1;
  //TRZ(dY1.T()); TRZ(x.T()); TRZ(dX1.T()); TRZ(x1.T()); TRZ(y1.T()); TRZ(norm1); 
    Std(BText("\n  Ortega Rheinboldt Step"));
    BMatrix<BDat> dX2, dY2, x2, y2(m,1); BDat norm2;
         if(r==m  ) { dY2 = y+dY1; }
    else if(r==m+c) { dY2 = (y1<<x)+dY1; }
    dX2 = -(Djp*V)*(Dp*(dY2.T()*U).T());
    x2 = x+dX2;
    fun.Evaluate(y2.GetData(), x2.Data());
    norm2 = FrobeniusNormU(y2.Data());
    if(norm2<norm) { norm = norm2; }
         if(norm1==norm) { x = x1; y = y1; }
    else if(norm2==norm) { x = x2; y = y2; }
    Std(I2(" norm = ", " norma ")<<norm.Value());
  }
  return(norm);
}


//--------------------------------------------------------------------
BDat ComputeLambdaWithSVD(      BRnRmFunction&     fun,
                          const BMatrix<BDat>&     UtdY,
                          const BDiagMatrix<BDat>& D,
                          const BMatrix<BDat>&     V,
                          const BDiagMatrix<BDat>& Djp,
                                BMatrix<BDat>&     x,
                                BMatrix<BDat>&     y,
			                             BDat	              lambda)
//--------------------------------------------------------------------
{
//BTimer tm("ComputeLambdaWithSVD");
  BDat lambda_ = (lambda^2.0);
  BDiagMatrix<BDat> DLi = D*(((D^2.0)+lambda_)^(-1.0));
  BMat dX = -V*(DLi*UtdY);
  dX = Djp*dX;
  x += dX;
	 fun.Evaluate(y.GetData(),x.Data());
  BDat norm = FrobeniusNormU(y.Data());
  if(norm.IsUnknown()) 
  { return(norm = BDat::PosInf()); }
  return(norm);
};


//--------------------------------------------------------------------
BDat LeastSquareLambdaWithSVD(      BRnRmFunction&     fun,
                              const BMatrix<BDat>&     U,
                              const BDiagMatrix<BDat>& D,
                              const BMatrix<BDat>&     V,
                              const BDiagMatrix<BDat>& Djp,
                                    BMatrix<BDat>&     x,
                                    BMatrix<BDat>&     y,
		                                  BMatrix<BDat>&     dY,
                                    BDat&              lambda)
//--------------------------------------------------------------------
{
  BDat v  = marquardtFactor_;
  BDat advance, norm = FrobeniusNormU(y.Data());
    
  BMatrix<BDat> xL=x, yL=y, xLv=x, yLv=y, UtdY = (dY.T()*U).T();
    
  BInt interIter=1;
  Std(BText("\n Marquardt regularization (")<< (interIter++) <<") with lambda="<<(lambda^2));
  BDat normL  = ComputeLambdaWithSVD(fun,UtdY,D,V,Djp,xL,yL,  lambda);
  Std(I2(" norm = ", " norma ")<<normL.Value());
  Std(BText("\n Marquardt regularization (")<< (interIter++) <<") with lambda="<<((lambda/v)^2));
  BDat normLv = ComputeLambdaWithSVD(fun,UtdY,D,V,Djp,xLv,yLv,lambda/v);
  Std(I2(" norm = ", " norma ")<<normLv.Value());
  if(normLv<=norm) 
  {
    lambda /= v;
    if(normL<=normLv) 
    {
      x       = xL;
      y       = yL;
      norm    = normL;
    }
    else
    {
      x       = xLv;
      y       = yLv;
      norm    = normLv;
    }
  } 
  else
  {
    if(normL<=norm) 
    {
      x    = xL;
      y    = yL;
      norm = normL;
    } 
    else
    {
      lambda *= v;
      BDat lambda_ = lambda;

      for(; interIter<=BDat::MaxIter(); interIter++)
	     {
        normLv = normL;
        Std(BText("\n Marquardt regularization (")<<interIter<<") with lambda="<<lambda_);
        normL = ComputeLambdaWithSVD(fun,UtdY,D,V,Djp,xL=x,yL=y,lambda);
        Std(I2(" norm = ", " norma ")<<normL.Value());
        
        if(normL.IsUnknown() || (normL.Value()==GSL_POSINF))                              
        { 
          xL=x;
          yL=y;
          break; 
        } 
        if(normL<=norm)                              { break; } 
        if(1.0/lambda<Sqrt(DEpsilon()))              { break; }
    	   if(Abs(normLv-normL) < BDat::Tolerance   ()) { break; }  
    	   if((interIter>=5) && (normL>normLv))         { break; }  
        lambda_ *= v;
      }
      x    = xL;
      y    = yL;
      norm = normL;
    }
  }
  return(norm);
}


//--------------------------------------------------------------------
BDat BRnRmFunction::LeastSqrMarquardt(BArray<BDat>&	   x0,
				                                  BArray<BDat>&	   y0,
				                                  BMatrix<BDat>&	  J)
    
/*! Minimizes by Marquardt algorithm (Swartz, 7.4.2, 324)
 *
 *	     Trasp(F(x))*F(x) = Sum(Fi(x)^2)
 */				 
//--------------------------------------------------------------------
{
  BText linMet = LinMetTable[(BInt)marquardtLinearMethod_.Value()-1];
    
  BTimer timer(I2(" Marquardt's Least Square Method",
		                " Método de minimización cuadrática de Marquardt"));
  Std(I2("\n  Absolute Tolerance    = ",
	        "\n  Tolerancia Absoluta   = ")+BDat::Tolerance().Format("%lg"));
  Std(I2("\n  Relative Tolerance    = ",
	        "\n  Tolerancia relativa   = ")+BDat::RelTolerance().Format("%lg"));
  Std(I2("\n  Maximum of iterations = ",
	        "\n  Máximo de iteraciones = ")+BDat::MaxIter().Format("%.0lf"));
  Std(I2("\n  Numeric differential pass size = ",
	        "\n  Tamaño de paso para diferenciales numéricas = ")+
	 Distance().Format("%lg"));
  
  clock_t tm_0   = BTimer::Clocks();

//La función Evaluate puede producir efectos colaterales sobre las variables 
//x0, y0 si estas apuntan a miembros de clases heredadas como es el caso del
//estimador ARIMA. Por eso se debe recoger su valor eb otras variables para
//poder usarlas en el algoritmo.
  BDat norm = FrobeniusNormU(x0);
  BMatrix<BDat> U_, V_;

  if(norm.IsKnown())
  {
    Evaluate(y0, x0);
    norm = FrobeniusNormU(y0);
  }
  n_ = x0.Size();
  m_ = y0.Size();
  BMatrix<BDat> x(n_,1,x0.Buffer()), y(m_,1,y0.Buffer());
  Std(I2("\n  Dimensions  = ",
	        "\n  Dimensiones = ")+n_+"x"+m_);
  clock_t tm_1   = BTimer::Clocks();
  BReal tm_dif = BTimer::ClockToSecond(tm_1-tm_0);
  tm_0 = tm_1;
  Std(I2("\nIteration(","\nIteración(")+" 0)"+
      I2("  norm : ","	norma : ") + norm +
      I2("\tTime : "," Tiempo : ") + tm_dif+I2(" seconds"," segundos"));

  BDat maxAbsJtY, norm1, advance=-1.0, oldAdvance;
  BBool ok     = BTRUE;

//El valor de lambda recomendado por Marquardt con J'J normalizada es 0.01
//para pasar a resolver (J'J-lambda*I)*dx = -J'dy
//Como aquí se resuelve (J' | lambda*I)dx = -dy se toma la raíz cuadrada  
  BDat lambda = 0.1;

  J.Alloc(m_,n_);
  BInt iter, j, k, usedSVDIter = 0; 

  for(iter=1; ok && (iter<=BDat::MaxIter()); iter++, usedSVDIter++)
  {
    BMat x_=x, y_=y; 
    norm1 = BDat::Unknown();
    if(norm.IsUnknown()||(x.Rows()!=n_)||(y.Rows()!=m_)) 
    {
      Error(I2("Objective function evaluation has failed.",
               "La evaluación de la función objetivo ha fallado."));
      break; 
    }
  //BDiagMatrix<BDat> Dn;
  //BMatrix<BDat> Jn;
    {
    //BTimer tj("Jacobian calculation");
      Jacobian(x.Data(), y.Data(), J);
    //J.NormColumns(Jn,Dn);
    }
    if(J.Rows()!=m_) 
    { 
      Error(I2("Fail in Jacobian calculation","Fallo en el cálculo del Jacobiano")+
            "Rows(J)="+J.Rows()+"!="+m_);
      break; 
    } 
    if(J.Columns()!=n_) 
    { 
      Error(I2("Fail in Jacobian calculation","Fallo en el cálculo del Jacobiano")+
            "Columns(J)="+J.Columns()+"!="+n_);
      break; 
    } 
    if(FrobeniusNormU(J.Data()).IsUnknown()) 
    { 
      Error(I2("Fail in Jacobian calculation","Fallo en el cálculo del Jacobiano")+
            "FrobeniusNorm(J) is unknown");
      break; 
    } 
    BMatrix<BDat> JtY = (y.T()*J).T();
    maxAbsJtY = JtY.MaxAbsNorm();
    if(maxAbsJtY<=BDat::Tolerance()) { break; } 

    BDiagMatrix<BDat> Dj, Djp;
    BMat Jn;
    DiagNormColumnsDecomp (J, Jn, Dj);
    Djp = Dj.P(Sqrt(DEpsilon()));
    if((Jn.Rows()!=m_)||(Jn.Columns()!=n_)||FrobeniusNormU(Jn.Data()).IsUnknown()) 
    { 
      usedSVDIter = 10000000*(int)BDat::MaxIter().Value();
      Jn  = J; 
      Dj  = Djp = BDiagMatrix<BDat>(n_,1.0);
    //Warning(I2("Fail in Jacobian normalization.",
    //           "Fallo en la normalización del Jacobiano."));
    } 

/*
gsl: C:\home\jsperez\tol\gsl-1.3\gsl-1.3\matrix\init_source.c
:29: ERROR: matrix dimension n1 must be positive integer
Default GSL error handler invoked.
*/

    //TRZ(J); //Jt=J.T(); //BMat JtJ = Jt*J; TRZ(JtJ);
    BMatrix<BDat>dY = y;
    BInt interIter = 1;                                  
	//BReal tm_ini   = BTimer::Clocks();
    BMatrix<BDat> x1=x, y1=y;
/* * /
    norm1 = NewtonStep(*this, J, x1, y1, dY);
    advance = norm1-norm;
  //Std(BText("\nGaussNewton norm=")<<norm1<<" advance="<<advance);
/* * / 
    {
      norm1 = LeastSquareLambda(*this, J, x1.GetData(), y1.GetData(), dY, lambda, iter, interIter);
      advance = norm1-norm;
    //Std(BText("\nMarquardt norm=")<<norm1<<" advance="<<advance);
    }
/* */
    BText method = BText("Golub_Reinsch_Mod");
    BMatrix<BDat> dX, W;
    BInt r = J.Rows(), c = J.Columns();
    BDiagMatrix<BDat> D, Dp;
    if(usedSVDIter>=0)
    {
      usedSVDIter = 0;
      U_.Alloc(0,0);
    }
    if(!U_.Rows()) 
    { 
    //BTimer tsvd("Singular Value Decomposition of Jacobian 0");
      D.Alloc(c); U_.Alloc(r,c); V_.Alloc(c,c);
      gsl_SingularValueDecomposition(Jn,U_,D,V_, method);
    }
    else
    {
    //BTimer tsvd("Singular Value Decomposition of Jacobian 1");
      BMat J_ = U_.T()*Jn*V_;
      BInt r_ = J_.Rows(), c_ = J_.Columns();
      BMatrix<BDat> U2(r_,c_), V2(c_,c_);
      D.Alloc(c_);
      gsl_SingularValueDecomposition(J_,U2,D,V2, method);
      if(FrobeniusNormU(D.Data()).IsUnknown())
      {
        D.Alloc(c); U_.Alloc(r,c); V_.Alloc(c,c);
        gsl_SingularValueDecomposition(Jn,U_,D,V_, method);
      }
      else
      {
        U_ = U_*U2; 
        V_ = V_*V2; 
      }
    }
/*
    Std(BText("\nJacobian related norms :")<<
              " F(J)  ="<<FrobeniusNormU(J  .Data()) <<
              " F(Dj) ="<<FrobeniusNormU(Dj .Data()) << 
              " F(Djp)="<<FrobeniusNormU(Djp.Data()) << 
              " F(Jn) ="<<FrobeniusNormU(Jn .Data()) << 
              " F(U_) ="<<FrobeniusNormU(U_ .Data()) << 
              " F(D)  ="<<FrobeniusNormU(D  .Data()) << 
              " F(V_) ="<<FrobeniusNormU(V_ .Data())); 
*/
  //norm1 = GaussNewtonStepWithSVD     (*this, U_, D, V_, Djp, x1=x, y1=y, dY); advance = norm1-norm;
  //norm1 = OrtegaRheinboldtStepWithSVD(*this, U_, D, V_, Djp, x1=x, y1=y, dY); advance = norm1-norm;
  //if(norm1.IsUnknown() || (norm1.Value()==GSL_POSINF) || (advance>=-BDat::Tolerance()))
    {
      norm1 = LeastSquareLambdaWithSVD(*this, U_, D, V_, Djp, x1=x, y1=y, dY, lambda);
      advance = norm1-norm;
    }
/* */
    norm1 = OneWaySearch(x1,y1,x,y,iter);
    advance = norm1-norm;
/* */
    if(norm1.IsUnknown() || (norm1.Value()==GSL_POSINF) || (advance>=-BDat::Tolerance()))
    {
      x1 = x;
      y1 = y;
    //Std(BText("\n Conjugate Gradient method"));
      BDat absDUtYSum = 0, absDUtYPartialSum = 0, norm0 = norm;
      BMatrix<BDat> Uty = (dY.T()*U_).T();
      BMatrix<BDat> DUty = D*Uty;
      BArray<BDat> absDUtY = DUty.Data(); 
      BArray<BInt> direct;
      BArray<BInt> inverse;

      for(j=0; j<DUty.Rows(); j++) 
      { 
        absDUtY(j)  = Abs(DUty(j,0)); 
        absDUtYSum += absDUtY(j); 
      }
      BIndexed<BDat>::Order(absDUtY,direct,inverse);

    //TRZ(JtY.T());
      int cgm = (int)CGMaxIter().Value();
      BInt maxCGIter = Minimum(cgm,n_);
      for(j=1; j<=maxCGIter; j++)
      {
        k = direct(n_-j);
        absDUtYPartialSum += absDUtY(k);
        BDat prop = absDUtYPartialSum/absDUtYSum;
        W = -V_.SubCol(k)*DUty(k,0);
        x1 += Djp*W; 
        norm1 = FrobeniusNormU(x1.Data());
        if(norm1.IsKnown())
        {
          Evaluate(y1.GetData(), x1.Data());
          norm1 = OneWaySearch(x1,y1,x,y,iter);
        }
        advance = norm1-norm;
        if(norm1.IsKnown() && (norm1.Value()!=GSL_POSINF) && (advance<0))
        { 
          x = x1; 
          y = y1; 
        //if((j>1) && (advance<-BDat::Tolerance())) { break; }
        }
        else
        {
          x1 = x; 
          y1 = y; 
        }
      //if((j>1) && (prop >= .99)) { break; }
        Std(BText("\n  Conjugate direction [")<<j<<"]="<<(100*prop).Format("%05.1lf%%")<<" norm="<<norm1);
      }
    }
/* */
    BDat maxDifx = (x1-x_).MaxAbsNorm();
    BDat relAdv = 100*advance/norm;
   
    tm_1 = BTimer::Clocks();
    tm_dif = BTimer::ClockToSecond(tm_1-tm_0);
    tm_0 = tm_1;
    Std(I2("\nIteration(","\nIteración(")+TxtFmt(iter,"%2ld")+")"+
        I2("  norm : ","	norma : ") + norm1 +
           " - " + norm+" = "+ advance+
	          " ("+relAdv.Format("%6.3lf")+"%) "+
           "maxDifx="+maxDifx+" "+
           "Max|Jta|="+maxAbsJtY+" "+
		   I2("\tTime : "," Tiempo : ") + tm_dif+I2(" seconds"," segundos"));

    ok = norm1.IsKnown() && (norm1.Value()!=GSL_POSINF) && (advance<0)&&
        ( 
         //(maxDifx >= BDat::Tolerance()) ||
          (  
            (Abs(advance)>=BDat::Tolerance   ()) &&
            (Abs(relAdv )>=BDat::RelTolerance())
          ) 
        );
    if(norm1.IsKnown()&&(advance<0))
    { 
    //Std("\nOK"); 
      x = x1; 
      y = y1; 
      norm = norm1;
    }
  }
//Std("\nNOT OK");TRZ(BMat(1,n_,x.Buffer()));
  Evaluate(y.GetData(),x.Data()); 
  x0 = x.Data(); 
  y0 = y.Data();

  if(iter==BDat::MaxIter())
  {
    Warning(BText("Marquardt ")+
	           I2("was interrupted at iteration (",
	              "fue interrumpido en la iteración (")+ iter+")");
  }
  if(!norm.IsKnown())
  {
    Error(BText("Marquardt ")+
 	        I2("failed at iteration (", "falló en la iteración (")+ iter+")");
  }

  return(norm);
}




//--------------------------------------------------------------------
BDat BRnRmFunction::ConjugateGradient(BArray<BDat>&	   x,
				      BArray<BDat>&	   y,
				      BMatrix<BDat>&	   J_)

/*! Minimizes by Conjugate Gradient algorithm
 *
 *	     Trasp(F(x))*F(x) = Sum(Fi(x)^2)
 */				 
//--------------------------------------------------------------------
{
  BInt i;
  BTimer timer(" Conjugate Gradient's Least Square Method");

  Std(I2("\nNumber of variables = ","\nNúmero de variables = ") + n_);
  Std(I2("\nNumber of data	= ","\nNúmero de datos	   = ") + m_);
  if(!m_||!n_) { return(BDat::Unknown()); }
  J_.Alloc(m_,n_);
  Evaluate(y, x);
  Jacobian(x, y, J_);
  DMat& J = (DMat&)J_;
  DMat Jt = J.T();
  DMat X(n_,1); for(i=0; i<n_; i++) { X(i,0) = x(i).Value(); }
  DMat Y(m_,1); for(i=0; i<m_; i++) { Y(i,0) = y(i).Value(); }
  double norm = Sqrt(MtMSqr(Y)(0,0)/(m_-n_));

  TRZ(X);

  DMat G=-(Jt*Y)*2, H=G;
  BSymMatrix<double> A;
  A = MtMSqr(J)*2;
  double g2 = MtMSqr(G)(0,0);
//long int gn;
//double gx = GetPowerScale(g2, gn);
  DMat AH = A*H;
  double h2 = (H.T()*AH)(0,0);
//long int hn;
//double hx = GetPowerScale(h2, hn);
//double lambda = (gx/hx) + powl(2,gn-hn);
  double lambda = g2/h2;
  X += H*lambda;
    Std(BText("\n  CG-Iteration(")+"0)"+
	      "\t lambda = " + BDat(lambda)+
	      "\t h2 = " + BDat(h2)+
	      "\t g2 = " + BDat(g2)+"; ");
  TRZ(X);TRZ(G);TRZ(H);
  for(i=0; (i<2*n_)&&(g2>LEpsilon()); i++)
  {
//  G /= Sqrt(g2);
//  H /= Sqrt(h2);
    DMat G_ = G - AH*lambda;
    double g2_ = ((G_-G).T()*G_)(0,0);
//  long int gn_;
//  double gx_	  = GetPowerScale(g2_, gn_);
//  double gamma = (gx_/gx) + powl(2,gn_-gn);
    double gamma = g2_/g2;
    G = G_;
    H = G + H*gamma;
    AH = A*H;
    h2 = (H.T()*AH)(0,0);
//  hx = GetPowerScale(h2, hn);
//  lambda = (gx/hx) + powl(2,gn-hn);
    lambda = g2/h2;
    X += H*lambda;
    TRZ(X);TRZ(G);TRZ(H);
    g2 = MtMSqr(G)(0,0);
//  gx = GetPowerScale(g2, gn);
    Std(BText("\n  CG-Iteration(")+i+")"+
	      "\t lambda = " + BDat(lambda)+
	      "\t gamma = " + BDat(gamma)+
	      "\t h2 = " + BDat(h2)+
	      "\t g2 = " + BDat(g2)+
	      "\t g2_= " + BDat(g2_)+"; ");
  }
  BArray<BDat> y_(m_), x_(n_); for(i=0; i<n_; i++) { x_(i) = X(i,0); }

  Evaluate(y_, x_);
  DMat Y_(m_,1); for(i=0; i<m_; i++) { Y_(i,0) = y_(i).Value(); }
  double norm_ = Sqrt(MtMSqr(Y_)(0,0)/(m_-n_));
  double advance = norm_ - norm;
  Std(BText("\nIteration(")+1+")"+
	    "\tNorm(" + BDat(norm_)+
	    "\t- " + BDat(norm)+"\t= "+ BDat(advance)+")");
  y = y_;
  x = x_;

  return(norm_);
}


int gsl_EvalFunction(const gsl_vector * x, void * instance, gsl_vector * f )
{
  BRnRmFunction * this_ = (BRnRmFunction*)instance;
  int i;

  BArray<BDat> &X = this_->GetX();
  BArray<BDat> &Fx = this_->GetFx();
  
  for(i=0; i<this_->GetDimN(); i++) X(i)= gsl_vector_get(x, i);
  this_->Evaluate(Fx, X);
  for(i=0; i<this_->GetDimM(); i++) gsl_vector_set(f, i, Fx(i).Value());
  return GSL_SUCCESS;
  
}

int gsl_EvalJacFunction(const gsl_vector * x, void * instance, gsl_matrix * J )
{
  BRnRmFunction * this_ = (BRnRmFunction*)instance;
  
  int i, j;
  
  BArray<BDat> &X = this_->GetX();
  BArray<BDat> &Fx = this_->GetFx();
  BArray<BDat> &FStep = this_->GetFStep();
  
  for(i=0; i<this_->GetDimN(); i++) X(i)= gsl_vector_get(x, i);
  this_->Evaluate(Fx, X);
  
  for(i=0; i<this_->GetDimN(); i++)
    {
      X(i) += Distance();
      this_->Evaluate(FStep, X);
      X(i)-=Distance();
      for(j=0; j<this_->GetDimM(); j++)
	{
	  BDat d = (FStep(j)-Fx(j))/Distance();
	  gsl_matrix_set(J, j, i, d.Value());
	}
    }
  return GSL_SUCCESS;
}

int gsl_EvalBoth(const gsl_vector * x, void * instance, gsl_vector * f, gsl_matrix * J )
{
  BRnRmFunction * this_ = (BRnRmFunction*)instance;
  BArray<BDat> &X = this_->GetX();
  BArray<BDat> &Fx = this_->GetFx();
  BArray<BDat> &FStep = this_->GetFStep();
  
  int i, j;
  double d, dist = Distance().Value();
  
  for(i=0; i<this_->GetDimN(); i++)
    X(i)= gsl_vector_get(x, i);
  this_->Evaluate(Fx, X);
  for(j=0; j<this_->GetDimM(); j++)
    gsl_vector_set(f, j, Fx(j).Value());
  
  for(i=0; i<this_->GetDimN(); i++)
    {
      X(i) += Distance();
      this_->Evaluate(FStep, X);
      X(i)-=Distance();
      for(j=0; j<this_->GetDimM(); j++)
	{
	  d = (FStep(j)-Fx(j)).Value()/dist;
	  gsl_matrix_set(J, j, i, d);
	}
    }
  return GSL_SUCCESS;
}

BDat BRnRmFunction::gsl_Marquardt(BArray<BDat>& X, BArray<BDat>& r, BMatrix<BDat>& J)
{
  Std(BText("\n Comienza Marquardt \n"));
  int j;
  double *x_;
  x_ = new double[ n_ ];
  
  //for(j=0; j<p_; j++){ Std(BText(" \n Terminos de X:")+X(j).Value());}
  
  for(j=0; j<n_; j++){ x_[j] = X(j).Value();}
  gsl_vector_view x = gsl_vector_view_array (x_, n_);
  
  const gsl_multifit_fdfsolver_type *T;
  gsl_multifit_fdfsolver *s;
  
  int status;
  size_t i, iter = 0;
  
  const size_t n = m_;
  const size_t p = n_;
  
  gsl_matrix *covar = gsl_matrix_alloc (p, p);
  
  gsl_multifit_function_fdf f;
  
  f.f      = &gsl_EvalFunction;     //&expb_f;
  f.df     = &gsl_EvalJacFunction;  //&expb_df;
  f.fdf    = &gsl_EvalBoth;         //&expb_fdf;
  f.n      = n;
  f.p      = p;
  f.params = this;                  //&d;
  
  T = gsl_multifit_fdfsolver_lmsder;
  s = gsl_multifit_fdfsolver_alloc (T, n, p);
  gsl_multifit_fdfsolver_set (s, &f, &x.vector);
  do
    {
      iter++;
      status = gsl_multifit_fdfsolver_iterate (s);
      if (status)
        break;
      status = gsl_multifit_test_delta (s->dx, s->x, 1e-4, 1e-4);
    }
  while (status == GSL_CONTINUE && iter < 500);
  
  gsl_multifit_covar (s->J, 0.0, covar);
  
  for(i=0; i<n_; i++) { X(i) = gsl_vector_get(s->x, i);}
  (*this).Evaluate(r, X);
  free(x_) ;
  gsl_multifit_fdfsolver_free (s);
  
  return (FrobeniusNorm(r));
  
  //  return 1;
}

