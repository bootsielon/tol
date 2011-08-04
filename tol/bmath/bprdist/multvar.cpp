/* multvar.cpp: Multivariant analysis.
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

#include <tol/tol_bprdist_internal.h>

#include <tol/tol_matrix.h>
#include <tol/tol_bmultvar.h>
#include <tol/tol_bfibonac.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include "gsl_ext.h"
#include <gsl/gsl_randist.h>
#include <gsl/gsl_math.h>
#include <tol/tol_lapack.h> 

BTraceInit("multvar.cpp");

//#define USE_BTruncatedNormalDist

//--------------------------------------------------------------------
// Static variable for BMultivarDist.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
BMultivarDist::BMultivarDist(      BProbDist*	      dist,
			     const BMatrix    <BDat>  nu,
			     const BSymMatrix <BDat>& cov)

/*! BMultivarDist Constructor
 */
//--------------------------------------------------------------------
    : N_(cov.Rows()), cov_(cov), nu_(nu)
{
    component_.ReallocBuffer(N_);
    for(BInt n=0; n<N_; n++) { component_[n]=dist; }
    Build();
}


//--------------------------------------------------------------------
BMultivarDist::BMultivarDist(const BArray     <BProbDist*> component,
			     const BMatrix    <BDat>       nu,
			     const BSymMatrix <BDat>&      cov)

/*! BMultivarDist Constructor
 */
//--------------------------------------------------------------------
    : N_(cov_.Rows()), component_(component), cov_(cov), nu_(nu)
{
    Build();
}


//--------------------------------------------------------------------
void BMultivarDist::Build()

/*! BMultivarDist Constructor
 */
//--------------------------------------------------------------------
{
    if((component_.Size()!=N_)||(nu_.Rows()!=N_))
    {
	Error(I2("Bad dimensions for building a multivariate distribution.",
		 "Dimensiones erróneas para construir una distribución "
		 "multivariante."));
	
	return;
    }
    avr_.Alloc(N_,1);
    for(BInt n=0; n<N_; n++)
    {
	if(component_[n])
	{
	    avr_(n,0) = component_[n]->Average() + nu_(n,0);
	}
	else
	{
	    Error(I2("Null comoponent in a multivariate distribution.",
		     "Componente nula en distribución multivariante."));
	    N_ = 0;
	    component_.ReallocBuffer(0);
	    cov_.Alloc(0);
	    avr_.Alloc(0,0);
	    return;
	}
    }
  TolLapack::dpotrf(CblasLower, cov_, choleski_);
}


//--------------------------------------------------------------------
BMatrix<BDat> BMultivarDist::Random() const

/*! Generates a random column vector with Normal(nu_, cov_)
 *  distribution.
 */
//--------------------------------------------------------------------
{
  BMatrix<BDat> r (N_, 1);
  for(BInt i=0; i<N_; i++) { r(i,0) = component_[i]->Random(); }
  BMatrix<BDat> cr;
  TolBlas::dtrmm(CblasLeft, CblasLower, CblasNoTrans, CblasNonUnit,1.0,
                 choleski_, r, cr);
  return(cr+nu_);
}


//--------------------------------------------------------------------
void RandPermutation(BMatrix<BDat>& M, BInt numPer, BInt numElem)

/*! Generates numPer random permutations of numElem elements
 */
//--------------------------------------------------------------------
{
    BInt i, j;
    
    BArray<BIndexed<BDat> > r(numElem);
    
    M.Alloc(numPer,numElem);
    
    for(i=0; i<numPer; i++)
    {
	for(j=0; j<numElem; j++)
	{
	    r[j].pos_ = j+1;
	    r[j].dat_ = BUniformDist::Random01();
	}
	BIndexed<BDat>::Sort(r);
	for(j=0; j<numElem; j++)
	{
	    M(i,j) = r[j].pos_;
	}
    }
}



//--------------------------------------------------------------------
class BMaxProb : public BRRFunction
//--------------------------------------------------------------------
{
private:
    BInt m_;
    BInt n_;
    BDat cmn_;
public:
    BMaxProb(BInt m, BInt n) : BRRFunction(), m_(m), n_(n)
	{
	    if(m<n) {
		Error(BText("En creación de BMaxProb(")+m_+","+n_+")"); 
	    }
	    cmn_ = LogFactorial(m_) - LogFactorial(m_-n_);
	}

    void Evaluate(BDat& y, const BDat& x)
	{
	    y = LogGamma(x+1)-LogGamma(x-n_+1) - cmn_;
//  Std(BText("\nBMaxProb(")+x+")="+y);
	};

    BDat Solve(const BDat& y)
	{
	    BDat m  = m_;
	    BDat n  = n_;
	    BDat x0 = n/2 + (m - n/2) * Exp(y/n);
//  BDat x  = Floor(NewtonSolve(x0,y,0.1));
//  if(!x.IsKnown() || (x<n_) || (x>m_))
	    BDat x  = Floor(BFibonacci::Solve(this,y,n_,m_,0.1));
	    BDat y1 = (*this)[x];
	    if(y1<y) { x += 1; }
	    return(x);
	};
};


//--------------------------------------------------------------------
void RandCombination(BMatrix<BDat>& M, BInt numCom, BInt m, BInt n)

/*! Generates numPer random combinations of n from m elements
 */
//--------------------------------------------------------------------
{
    BInt i, j, k=0;
    M.Alloc(numCom,n);
    for(i=0; i<numCom; i++)
    {
//  Std(BText("\nRandCombination  (")+m+","+n+")");
	BInt max = m;
	for(j=0; j<n; j++)
	{
	    if(max==n-j)
	    {
		M(i,n-j-1) = max;
	    }
	    else
	    {
		BDat nj = n-j;
		BMaxProb f(max, n-j);
		BDat p = BUniformDist::Random01();
		BDat y = Log(p);
		BDat x = f.Solve(y);
		BDat yy = f[x];
		M(i,n-j-1) = x;
	//Std(BText("\nj=")+(j)+" n-j="+(n-j)+" max="+max+" p="+p+" y="+y+
	//" x="+x+" yy="+yy);
	    }
	    max = BInt(M(i,n-j-1).Value()-1);
	}
    }
}

//--------------------------------------------------------------------
void RandMultinomial(int N, const BArray<double>& p, 
                     BArray<unsigned int>& n)
// Use GSL function gsl_ran_multinomial
// http://www.network-theory.co.uk/docs/gslref/TheMultinomialDistribution.html
//--------------------------------------------------------------------
{
  size_t K = p.Size();
  n.ReallocBuffer(K);
  gsl_ran_multinomial (getGslRng(), K, N, p.Buffer(), n.GetBuffer());
}


//--------------------------------------------------------------------
bool RandTruncatedMultNormal(         BMatrix<BDat>& tn, 
                                const BMatrix<BDat>& A,
                                const BMatrix<BDat>& B,
                                const BMatrix<BDat>& nu,
                                      BDat           s2,
                                      BInt           num)

/*! Generates num instances of a truncated multi-normal with bounds 
 *  in the n-rectangle given by cartesian product of intervals (Ai,Bi)
 *  and with average nu and covariance matrix the identity
 */
//--------------------------------------------------------------------
{
  BInt i, k, n=nu.Rows();
  if((1!=nu  .Columns())||(1> n             )||
     (1!=A   .Columns())||(1!=B   .Columns())||
     (n!=A   .Rows   ())||(n!=B   .Rows   ())  )
  {
    Error(I2("Invalid dimensions for RandTruncatedIdMultiNormal",
             "Dimensiones no válidas para RandTruncatedIdMultiNormal")+
          " A("+A.Rows()+"x"+A.Columns()+")"+
          " B("+B.Rows()+"x"+B.Columns()+")"+
          " nu("+nu.Rows()+"x"+nu.Columns()+")");
    tn.Alloc(0,0);
    return(false);
  }
  BDat s = Sqrt(s2);
  BMat a=(A-nu)/s, b=(B-nu)/s, x, y;
  tn.Alloc(n,num);
  y.Alloc(n,1);
  for(k=0; k<num; k++)
  {
    for(i=0; i<n; i++)
    {
# ifdef USE_BTruncatedNormalDist
      BTruncatedNormalDist tni(a(i,0), b(i,0), 0, 1);
      y(i,0) = tni.Random();
# else
      y(i,0) = gsl_rtabnorm_combo(getGslRng(), 0.0, 1.0,
                                  a(i,0).Value(), b(i,0).Value(), 10);
# endif
    }
    x = (y*s)+nu;
    for(i=0; i<n; i++)
    {
      tn(i,k) = x(i,0);
    }
  }
  return(true);
}

//--------------------------------------------------------------------
BDat LogDensTruncatedMultNormal(const BMatrix<BDat>& y,
                                const BMatrix<BDat>& A,
                                const BMatrix<BDat>& B,
                                const BMatrix<BDat>& nu,
                                      BDat           s2)

/*! Logarithm of density of a truncated multi-normal with bounds 
 *  in the n-rectangle given by cartesian product of intervals (Ai,Bi)
 *  and with average nu and covariance matrix cov*s2 which choleski 
 *  low triangular factor LCov (cov = LCov*LCov')
 */
//--------------------------------------------------------------------
{
  BInt i, n=nu.Rows();
  if((1< nu  .Columns())||(1> n             )||
     (1!=A   .Columns())||(1!=B   .Columns())||
     (n!=A   .Rows   ())||(n!=B   .Rows   ())  )
  {
    Error(I2("Invalid dimensions for RandTruncatedMultiNormal",
             "Dimensiones no válidas para RandTruncatedMultiNormal")+
          " A("+A.Rows()+"x"+A.Columns()+")"+
          " B("+B.Rows()+"x"+B.Columns()+")"+
          " nu("+nu.Rows()+"x"+nu.Columns()+")");
    return(false);
  }
  BDat logDens;
  BDat s = Sqrt(s2);
  BMat a=(A-nu)/s, b=(B-nu)/s, x;
  logDens = 0; 
  for(i=0; i<n; i++)
  {
    BTruncatedNormalDist tni(a(i,0), b(i,0), 0, 1);
    logDens+=tni.Dens(y(i,0));
  }
  return(logDens);
}


//--------------------------------------------------------------------
bool RandTruncatedMultNormal(      BMatrix<BDat>& tn, 
                             const BMatrix<BDat>& A,
                             const BMatrix<BDat>& B,
                             const BMatrix<BDat>& nu,
                             const BLowTrMatrix<BDat>& LCov,
                                   BDat           s2,
                                   BInt           num)

/*! Generates num instances of a truncated multi-normal with bounds 
 *  in the n-rectangle given by cartesian product of intervals (Ai,Bi)
 *  and with average nu and covariance matrix cov*s2 which choleski 
 *  low triangular factor LCov (cov = LCov*LCov')
 */
//--------------------------------------------------------------------
{
  BInt i, j, k, n=nu.Rows();
  if((1< nu  .Columns())||(1> n             )||
     (1!=A   .Columns())||(1!=B   .Columns())||
     (n!=A   .Rows   ())||(n!=B   .Rows   ())||
     (n!=LCov.Rows   ())||(n!=LCov.Columns())  )
  {
    Error(I2("Invalid dimensions for RandTruncatedMultiNormal",
             "Dimensiones no válidas para RandTruncatedMultiNormal")+
          " A("+A.Rows()+"x"+A.Columns()+")"+
          " B("+B.Rows()+"x"+B.Columns()+")"+
          " nu("+nu.Rows()+"x"+nu.Columns()+")"+
          " LCov("+LCov.Rows()+"x"+LCov.Columns()+")");
    tn.Alloc(0,0);
    return(false);
  }
  tn.Alloc(n,num);
  BDat s = Sqrt(s2);
  BMat a=(A-nu)/s, b=(B-nu)/s, y(n,1), x;
  for(k=0; k<num; k++)
  {
    for(i=0; i<n; i++)
    {
      BDat aux = 0;  
      for(j=0; j<i; j++)
      {
        aux+=LCov.Get(i,j)*y(j,0);   
      }
      BDat Li = LCov.Get(i,i);
    //Std(BText("\nL[")+i+"]="+Li);
      if(Abs(Li.Value())<=Sqrt(1.E-15))
      {
        y(i,0) = 0;
       /*
        Error(I2("Non positive defined covariance matrix, so it cannot be used "
                 "in RandTruncatedMultiNormal",
                 "La matriz de covarianzas no es definida positiva y no se "
                 "puede usar en RandTruncatedMultiNormal")+
              ". Triangular Choleski of covariance is LCov = \n"+LCov.Name());
        tn.Alloc(0,0);
        return(false); */
      }
      else
      {
        BDat ai, bi;
        if(Li>0)
        {
          ai = (a(i,0)-aux)/Li;
          bi = (b(i,0)-aux)/Li;
        }
        else
        {
          ai = (b(i,0)-aux)/Li;
          bi = (a(i,0)-aux)/Li;
        }
        BTruncatedNormalDist tni(ai, bi, 0, 1);
        y(i,0) = tni.Random();
      }
    }
    for(i; i<n; i++)
    {
      y(i,0) = 0;
    }
    x = LCov*(y*s)+nu;
    for(i=0; i<n; i++)
    {
      tn(i,k) = x(i,0);
    }
  }
  return(true);
}

//--------------------------------------------------------------------
BDat LogDensTruncatedMultNormal(const BMatrix<BDat>& x,
                                const BMatrix<BDat>& A,
                                const BMatrix<BDat>& B,
                                const BMatrix<BDat>& nu,
                                const BLowTrMatrix<BDat>& LCov,
                                const BUpTrMatrix <BDat>& LiCov,
                                      BDat                s2)

/*! Logarithm of density of a truncated multi-normal with bounds 
 *  in the n-rectangle given by cartesian product of intervals (Ai,Bi)
 *  and with average nu and covariance matrix cov*s2 which choleski 
 *  low triangular factor LCov (cov = LCov*LCov')
 */
//--------------------------------------------------------------------
{
  BInt i, j, n=nu.Rows();
  if((1< nu  .Columns())||(1> n             )||
     (1!=A   .Columns())||(1!=B   .Columns())||
     (n!=A   .Rows   ())||(n!=B   .Rows   ())||
     (n!=LCov.Rows   ())||(n!=LCov.Columns())  )
  {
    Error(I2("Invalid dimensions for RandTruncatedMultiNormal",
             "Dimensiones no válidas para RandTruncatedMultiNormal")+
          " A("+A.Rows()+"x"+A.Columns()+")"+
          " B("+B.Rows()+"x"+B.Columns()+")"+
          " nu("+nu.Rows()+"x"+nu.Columns()+")"+
          " LCov("+LCov.Rows()+"x"+LCov.Columns()+")");
    return(false);
  }
  BDat logDens;
  BDat s = Sqrt(s2);
  BMat a=(A-nu)/s, b=(B-nu)/s, y=LiCov*(x-nu)/s;
  logDens = 0; 
  for(i=0; i<n; i++)
  {
    BDat aux = 0;  
    for(j=0; j<i; j++)
    {
      aux+=LCov.Get(i,j)*y(j,0);   
    }
    BDat Li = LCov.Get(i,i);
  //Std(BText("\nL[")+i+"]="+Li);
    if(Abs(Li.Value())<=Sqrt(1.E-15))
    {
      logDens += Log(BNormalDist::Dens01(y(i,0)));
    }
    else
    {
      BDat ai, bi;
      if(Li>0)
      {
        ai = (a(i,0)-aux)/Li;
        bi = (b(i,0)-aux)/Li;
      }
      else
      {
        ai = (b(i,0)-aux)/Li;
        bi = (a(i,0)-aux)/Li;
      }
      BTruncatedNormalDist tni(ai, bi, 0, 1);
      logDens+=tni.Dens(y(i,0));
    }
  }
  return(logDens);
}
