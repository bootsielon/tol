/* stat.cpp: Generic statistical functions.
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

#include <tol/tol_bstat.h>
#include <tol/tol_matrix.h>
#include <tol/tol_bratio.h>

#include <gsl/gsl_math.h>

BTraceInit("stat.cpp");

//--------------------------------------------------------------------
typedef BDat (*BSimpleStatisticFunction) (const BArray<BDat>& vec);

//--------------------------------------------------------------------
BDat SubSampleAverage(BSimpleStatisticFunction stat,
		      const BArray<BDat>& vec)

/*! Returns the value of the statistic giving by stat filtered of 
 *  outliters greater than 3 sigmas.
 * \param stat: statistic to be calculated.
 * \param vec: data vector
 * \sa Average()
 * \sa StandardDeviation()
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat nu    = Average		 (vec);
    BDat sigma = StandardDeviation (vec);
    BDat nor;
    BInt haycambio = 1;
//  BInt cont=0;
    
    BArray<BDat> x(vec);
    //for(BInt n=1; haycambio && sigma != 0; n++)
    for(BInt n=1; haycambio && sigma; n++)
    {
	haycambio = 0;
	for(BInt k = 0; k<x.Size(); k++)
	{
	    if(vec(k).IsKnown())
	    {
		nor = (x(k)-nu)/sigma;
		if(Abs(nor)>3.0)
		{
		    x(k) = BDat::Unknown();
		    haycambio = 1;
		}
	    }
	}
	nu = Average(x);
	sigma = StandardDeviation(x);
    }
    return((*stat)(x));
}
//--------------------------------------------------------------------
BDat Sum(const BArray<BDat>& vec)

/*! Returns the Sum statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat result = 0;
    BInt num = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    result += vec(k);
	    num ++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    return(result);
}


//--------------------------------------------------------------------
BDat Product(const BArray<BDat>& vec)

/*! Returns the Product statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat result = 1;
    BInt num = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if((vec(k)==0) || (result==0)) { return(0); }
	if(vec(k).IsKnown())
	{
	    result *= vec(k);
	    num ++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    return(result);
}


//--------------------------------------------------------------------
BDat Average(const BArray<BDat>& vec)

/*! Returns the Average statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    BDat result = 0;
    BInt num    = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    result += vec(k);
	    num++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    else       { result /= num; }
    return(result);
}


//--------------------------------------------------------------------
BDat HarmonicAverage(const BArray<BDat>& vec)

/*! Returns the Harmonic Average statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat result = 0;
    BInt num    = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	BDat x = 1/vec(k);
	if(x.IsKnown())
	{
	    result += x;
	    num++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    else       { result /= num; }
    return(result);
}


//--------------------------------------------------------------------
BDat GeometricAverage(const BArray<BDat>& vec)

/*! Returns the Geometric Average statistic of a vector of real
 *  numbers
 */
//--------------------------------------------------------------------
{
    BDat result = 0;
    BInt num    = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	BDat x = Log(vec(k));
	if(x==0) { return(0); }
	if(x.IsKnown())
	{
	    result += x;
	    num++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    else       { result /= num; }
    return(Exp(result));
}


//--------------------------------------------------------------------
BDat Moment(const BArray<BDat>& vec, BInt m)

/*! Returns the m-moment statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    if(m<1) { m=1; }
    BDat result = 0;
    BInt num    = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    BDat v  = vec(k);
	    BDat vm = v^(BDat(m));
	    result += vm;
	    num++;
	}
    }
    if(num==0) { result = BDat::Unknown(); }
    else       { result /= num; }
    return(result);
}


//--------------------------------------------------------------------
BDat CenterMoment(const BArray<BDat>& vec, BInt m)

/*! Returns the center m-moment statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BArray<BDat> centered = vec;
    BDat avr = Average(vec);
    for(BInt k = 0; k<vec.Size(); k++)
    {
	centered(k) -= avr;
    }
    return(Moment(centered,m));
}


//--------------------------------------------------------------------
BDat Varianze(const BArray<BDat>& vec)

/*! Returns the Varianze statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    
    return CenterMoment(vec,2);
    
  /*
  BDat avr  = Average(vec);
  BDat avr2 = avr*avr;
  BDat m2   = Moment(vec,2);
  return(m2-avr2);*/
}


//--------------------------------------------------------------------
BDat StandardDeviation(const BArray<BDat>& vec)

/*! Returns the Standard Deviation statistic of a vector of real
 *  numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    return(Sqrt(Varianze(vec)));
}


//--------------------------------------------------------------------
BDat AsymmetryCoefficient(const BArray<BDat>& vec)

/*! Returns the Asymmetry Coefficient statistic of a vector of real
 *  numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    return(CenterMoment(vec,3)/(Varianze(vec)^(3/2)));
}


//--------------------------------------------------------------------
BDat Kurtosis(const BArray<BDat>& vec)

/*! Returns the Kurtosis statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    return((CenterMoment(vec,4)/(Varianze(vec)^2))-3);
}


//--------------------------------------------------------------------
BDat Maximum(const BArray<BDat>& vec)

/*! Returns the Maximum statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat result = BDat::NegInf();
  bool allUnknown = true;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    if(result<vec(k))
	    {
		result = vec(k);
      if(allUnknown) { allUnknown=true; }
	    }
	}
    }
    if(allUnknown) { result = BDat::Nan(); }
    return(result);
}


//--------------------------------------------------------------------
BDat Minimum(const BArray<BDat>& vec)

/*! Returns the Minimum statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    BDat result = BDat::PosInf();
  bool allUnknown = true;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    if(result>vec(k)) { result = vec(k); }
      if(allUnknown) { allUnknown=true; }
	}
    }
    if(allUnknown) { result = BDat::Nan(); }
    return(result);
}


//--------------------------------------------------------------------
BDat Quantile(const BArray<BDat>& vec, BDat q)

/*! Returns the q-Quantile statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    if(q>1)  { q = 1; }
    if(q<0)  { q = 0; }
    BDat p = 1-q;
    BArray<BDat> sorted(vec.Size());
    int i, j;
    for(i=j=0; i<vec.Size(); i++)
    {
      if(vec(i).IsKnown())
      {
        sorted(j++) = vec(i);
      }
    }
    sorted.ReallocBuffer(j);
    sorted.Sort(DatCmp);
    BDat Q = (1.0/q).Value();
    BDat n = BDat(sorted.Size()-1)/BDat(Q);
//Std(BText("\nMedian q=")+q+" Q="+Q+" n="+n);
    BDat result;
    if(n.IsInteger())
    {
      result =sorted(int(n.Value()));
    }
    else
    {
	BInt nq = (BInt)Ceil (n).Value();
	BInt np = (BInt)Floor(n).Value();
	BDat sq = sorted(nq);
	BDat sp = sorted(np);
//  Std(BText("nq=")+nq+" np="+np+" sq="+sq+" sp="+sp);
	result = (q*sq)+(p*sp);
    }
    return(result);
}

//--------------------------------------------------------------------
void Quantile(const BArray<BDat>& vec, 
              const BArray<BDat>& r, 
                    BArray<BDat>& Qu)

/*! Returns a set of q-Quantile statistics of a vector of real numbers
 */
//--------------------------------------------------------------------
{
  Qu.AllocBuffer(0);
  if(!vec.Size()) { return; }
  BArray<BDat> sorted(vec.Size());
  int i, j;
  for(i=j=0; i<vec.Size(); i++)
  {
    if(vec(i).IsKnown())
    {
      sorted(j++) = vec(i);
    }
  }
  sorted.ReallocBuffer(j);
  sorted.Sort(DatCmp);
  Qu.AllocBuffer(r.Size());
  int k;
  for(k=0; k<r.Size(); k++)
  {
    BDat q = r[k];
    if(q>1)  { q = 1; }
    if(q<0)  { q = 0; }
    BDat p = 1-q;
    BDat Q = (1.0/q).Value();
    BDat n = BDat(sorted.Size()-1)/BDat(Q);
    BDat result;
    if(n.IsInteger())
    {
      Qu[k] = sorted(int(n.Value()));
    }
    else
    {
      BInt nq = (BInt)Ceil (n).Value();
      BInt np = (BInt)Floor(n).Value();
      BDat sq = sorted(nq);
      BDat sp = sorted(np);
      Qu[k] = (q*sq)+(p*sp);
    }
  }
}


//--------------------------------------------------------------------
BDat Median(const BArray<BDat>& vec)

/*! Returns the Median statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(BDat::Unknown()); }
    return(Quantile(vec,0.5));
}


//--------------------------------------------------------------------
BDat Covarianze(const BArray<BDat>& v1, const BArray<BDat>& v2)

/*! Returns the covarianze between v1 and v2 vectors
 */
//--------------------------------------------------------------------
{
    if(!v1.Size()||!v2.Size()) { return(BDat::Unknown()); }
    BInt k;
    BDat result;
    BInt num = 0;
    if(v1.Size()==v2.Size())
    {
	BDat v1Avr = Average(v1);
	BDat v2Avr = Average(v2);
	result = 0;
	for(k = 0; k<v1.Size(); k++)
	{
	    if(v1(k).IsKnown() && v2(k).IsKnown())
	    {
		num++;
		result += (v1(k)-v1Avr)*(v2(k)-v2Avr);
	    }
	}
    }
    return(result/BDat(num));
}


//--------------------------------------------------------------------
BDat Correlation(const BArray<BDat>& v1, const BArray<BDat>& v2)

/*! Returns the correlation between v1 and v2 vectors
 */
//--------------------------------------------------------------------
{
  if(!v1.Size()||!v2.Size()) { return(BDat::Unknown()); }
  BInt j, n=v1.Size();
  BArray<BDat> w1 = v1;
  BArray<BDat> w2 = v2;
  BDat avr1 = Average(v1);
  BDat avr2 = Average(v2);
  for(j=0; j<n; j++)
  {
  	w1(j) -= avr1;
  }
  for(j=0; j<n; j++)
  {
	  w2(j) -= avr2;
  }
  BDat w11=0, w22=0, w12=0;
  for(j=0; j<n; j++)
  {
	  w11 += w1(j)*w1(j);
	  w12 += w1(j)*w2(j);
	  w22 += w2(j)*w2(j);
  }
  BDat cor = w12/Sqrt(w11*w22);
  return(cor);
}


//--------------------------------------------------------------------
void Covarianze(const BMatrix<BDat>& d, BSymMatrix<BDat>& cov)

/*! Returns the correlation between rows of data
 */
//--------------------------------------------------------------------
{
    BMatrix<BDat> data = d;
    BInt n = data.Rows();
    BInt N = data.Columns();
    BInt M = 0;
    BInt i,j,k;
    BArray<BDat> avr;
    avr.ReallocBuffer(n);
    cov.Alloc(n);
    
    for(k=0; k<N; k++)
    {
	BDat c = 0;
	for(j=0; c.IsKnown() && (j<n); j++)
	{
	    c = data(j,k);
	}
	if(!c.IsKnown())
	{
	    for(j=0; j<n; j++)
	    {
		data(j,k).PutKnown(BUNKNOWN);
	    }
	}
	else
	{
	    M++;
	}
    }
    
    for(i=0; i<n; i++)
    {
	avr(i) = 0;
	for(j=0; j<N; j++) { avr(i) += data(i,j); }
	avr(i)/=BDat(N);
    }
    
    for(i=0; i<n; i++)
    {
	for(j=0; j<=i; j++)
	{
	    BDat c = 0;
	    for(k=0; k<N; k++)
	    {
		c += (data(i,k)-avr(i))*(data(j,k)-avr(j));
	    }
	    cov(i,j)=c/BDat(M);
	}
    }
}


//--------------------------------------------------------------------
void Correlation(const BMatrix<BDat>& data,
		 BSymMatrix<BDat>& cor)
    
/*! Returns the correlation between rows of data
 */
//--------------------------------------------------------------------
{
    BSymMatrix<BDat> cov;
    Covarianze(data, cov);
    CorrelationF(cov,cor);
}

//--------------------------------------------------------------------
void CorrelationF(const BSymMatrix<BDat>& cov,
		  BSymMatrix<BDat>& cor)

/*! Returns the correlation between rows of data
 */
//--------------------------------------------------------------------
{
    BInt n = cov.Rows();
    BInt i,j;
    cor = cov;
    BArray<BDat> sq; sq.ReallocBuffer(n);
    for(i=0; i<n; i++)
    {
	sq(i) = Sqrt(cor.Get(i,i));
    }
    for(i=0; i<n; i++)
    {
	for(j=0; j<i; j++)
	{
	    BDat cij = cor.Get(i,j);
	    BDat sij = sq(i)*sq(j);
	    BDat xij = cij / sij;
	    cor(i,j)=xij;
	}
	cor(i,i)=1;
    }
}


//--------------------------------------------------------------------
void PartialCorrelation(const BMatrix<BDat>&    data,
			BSymMatrix<BDat>& pcor)

/*! Returns the partial correlation between rows of data
 */
//--------------------------------------------------------------------
{
  BSymMatrix<BDat> cor;
  Correlation(data, cor);
  PartialCorrelationF(cor,pcor);
}

//--------------------------------------------------------------------
void PartialCorrelationF(const BSymMatrix<BDat>& cor,
			 BSymMatrix<BDat>& pcor)

/*! Returns the partial correlation between rows of data
    Using matrix inversion
    http://en.wikipedia.org/wiki/Partial_correlation
 */
//--------------------------------------------------------------------
{
  BInt  i,j,r=cor.Rows();
  BSymMatrix<BDat> aux;
  BBool ok = Inverse(cor,aux);
  pcor.Alloc(r);
  for(i=0; i<r; i++)
  {
    for(j=0; j<r; j++)
    {
      pcor(i,j) = aux(i,j) / Sqrt(aux(i,i)*aux(j,j));
    }
    pcor(i,i) = 1;
  }
  pcor *= -1;

/*
  Old method Using recursive formula (http://en.wikipedia.org/wiki/Partial_correlation)

    BInt n = cor.Rows();
    BInt i,j,k,m;
    pcor.Alloc(n);
    for(i=0; i<n; i++)
    {
	pcor(i,i)=1;
	for(j=0; j<i; j++)
	{
	    BArray<BInt> cond; cond.ReallocBuffer(n-2);
	    for(k=m=0; k<n; k++) { if((k!=i)&&(k!=j)) { cond(m++)=k; } }
	    BDat r = RecursivePartialCorrelation(i,j,cond,cor);
	    pcor(i,j)=r;
	    pcor(j,i)=r;
	}
    }
*/
}

/*
//--------------------------------------------------------------------
static BDat RecursivePartialCorrelation(      BInt x1, BInt x2,
					const BArray    <BInt>& cond,
					const BSymMatrix<BDat>& cor)
    
/*! Returns the partial correlation between rows of data
 * /
//--------------------------------------------------------------------
{
    BInt n = cond.Size();
    if(n==0) { return(cor.Get(x1,x2)); }
    
    BArray<BInt> cond1 = cond;
    cond1.ReallocBuffer(n-1);
    BInt xn = cond(n-1);
    BDat r12 = RecursivePartialCorrelation(x1,x2,cond1,cor);
    BDat r1n = RecursivePartialCorrelation(x1,xn,cond1,cor);
    BDat r2n = RecursivePartialCorrelation(x2,xn,cond1,cor);
    return((r12-r1n*r2n)/Sqrt((1-(r1n^2))*(1-(r2n^2))));
}
*/


//--------------------------------------------------------------------
BDat ResidualVar(const BDat var, const BInt j,
		 const BSymMatrix<BDat>& pcor)

/*! Returns the residual varianze
 */
//--------------------------------------------------------------------
{
    BInt n = pcor.Rows();
    BInt i;
    BDat rvar = var;
    for(i=0; i<n; i++)
    {
	if(i!=j) { rvar *= (1-(pcor.Get(j,i)^2)); }
  }
    return(rvar);
}


//--------------------------------------------------------------------
void RegressionCoef(const BSymMatrix<BDat>& cov,
		    const BSymMatrix<BDat>& pcor,
			           BMatrix<BDat>&    regr)

/*! Returns the regression coefficient between rows of data
 */
//--------------------------------------------------------------------
{
    BInt n = pcor.Rows();
    regr.Alloc(n,n);
    BInt i,j;
    BArray<BDat> rvar; rvar.ReallocBuffer(n);
    for(i=0; i<n; i++)
    {
	rvar(i) = ResidualVar(cov.Get(i,i), i, pcor);
//  Std(BText("rvar(")+i+")="+rvar(i));
    }
    for(i=0; i<n; i++)
    {
	regr(i,i) = 1;
	for(j=0; j<i; j++)
	{
	    BDat cij	 = rvar(i)/rvar(j);
	    regr(i,j) = pcor(i,j)*cij;
	    regr(j,i) = pcor(i,j)/cij;
	}
    }
}


//--------------------------------------------------------------------
BDat R2Coefficient(const BArray<BDat>& v1, const BArray<BDat>& v2)

/*! Returns the Square R Coefficient between v1 and v2 vectors
 */
//--------------------------------------------------------------------
{
  BDat result;
  if(v1.Size()==v2.Size())
  {
	  result = (Correlation(v1,v2)^2.0);//*(BDat(v1.Size())/BDat(v1.Size()-1));
  }
  return(result);
}


//--------------------------------------------------------------------
BDat PearsonCoefficient(const BArray<BDat>& v1, 
		       const BArray<BDat>& v2)

/*! Returns the Pearson-Coefficient between v1 and v2 vectors
 */
//--------------------------------------------------------------------
{
    return(Sqrt(R2Coefficient(v1,v2)));
}


//--------------------------------------------------------------------
// vectorial statistics
//--------------------------------------------------------------------


//--------------------------------------------------------------------
void AutoCov(const BArray<BDat>& vec, 
	     BArray<BDat>& cov, 
	     BInt order,
	     BInt demean)

/*! Puts in cov the order-autocovarianze vector of vec
 */
//--------------------------------------------------------------------
{
  BInt i,t;
  cov.ReallocBuffer(order);
  if(vec.Size()>1)
  {
    if(demean==1)
    {
	    BDat avr = Average(vec);
      for(i = 0; i<order; i++)
	    {
	      cov(i)=0;
	      for(t=i; t<vec.Size(); t++)
	      {
		      if(vec(t).IsKnown()&&vec(t-i).IsKnown()) 
		      {
		        cov(i)+= (vec(t)-avr)*(vec(t-i)-avr); 
		      }
	      }
	      cov(i)/=BDat(vec.Size());
	    }
    }
    else
    {
	    for(i = 0; i<order; i++)
	    {
	      cov(i)=0;
	      for(t=i; t<vec.Size(); t++)
	      {
		      if(vec(t).IsKnown()&&vec(t-i).IsKnown()) 
		      {
		        cov(i)+= vec(t)*vec(t-i); 
		      }
	      }
	      cov(i)/=BDat(vec.Size());
	    }
	  }
  }
}


//--------------------------------------------------------------------
void AutoCor(const BArray<BDat>& vec,BArray<BDat>& cor,BInt order,BInt demean)

/*! Puts in cov the order-autocorrelation vector of vec
 */
//--------------------------------------------------------------------
{
    BArray<BDat> cov;
    BArray<BDat> v;
    BInt n, m, i;
    
//    for(n=0; (n<vec.Size()-1) && ((vec(n)==0) || vec(n).IsUnknown()); n++) {}
//    for(m=vec.Size()-1; (m>0) && ((vec(m)==0) || vec(m).IsUnknown()); m--) {}
    for(n=0; (n<vec.Size()-1) && (vec(n).IsUnknown()); n++) {}
    for(m=vec.Size()-1; (m>0) && (vec(m).IsUnknown()); m--) {}

    v.ReallocBuffer(m-n+1);
    
    for(i=0; (i+n<=m); i++) { v(i) = vec(i+n); }
    AutoCov(v,cov,order+1,demean);
    
    cor.ReallocBuffer(order);
    for(i = 0; i<order; i++) { cor(i)=cov(i+1)/cov(0); }
}

/*
//--------------------------------------------------------------------
void DurbinAutoReg(const BArray<BDat>&  r, BMatrix<BDat>& FI)

// PURPOSE:
//
//--------------------------------------------------------------------
{
  BInt order = r.Size();
  BInt p,j;

  FI.Alloc(order+1, order+1);

  FI(0,0)=1;
  for(p = 1; p<=r.Size(); p++)
  {
    FI(0,p)=0;
    FI(p,0)=1;
  }
  for(p = 0; p<r.Size(); p++)
  {
    BReal up   = r(p).Value();
    BReal down = 1;
    for(j = 1; j<=p; j++)
    {
      up   -= (FI(p,j)*r(p-j)).Value();
      down -= (FI(p,j)*r(  j)).Value();
    }
    FI(p+1,p+1)	 = up/down;

    for(j = 1; j<=p; j++)
    {
      FI(j,p+1)=0;
      FI(p+1,j)=FI(p,j)-FI(p+1,p+1)*FI(p,p-j+1);
    }
  }
  TRZ(FI);
}
*/

//--------------------------------------------------------------------
void DurbinAutoReg(const BArray<BDat>&  r,  BMatrix<BDat>& FI)

/*! Calculates the partial autocorrelations matrix with the
 *  Durbin algorithm
 */
//--------------------------------------------------------------------
{
    BInt order = r.Size();
    BInt p,j;
    
    FI.Alloc(order, order);
    
    FI(0,0)=r(0);
    for(p = 2; p<=order; p++)
    {
	BDat up   = r(p-1);
	BDat down = 1;
	for(j = 1; j<p; j++)
	{
	    up   -= FI(p-2,j-1) * r(p-j-1);
	    down -= FI(p-2,j-1) * r(	j-1);
	}
	FI(p-1,p-1)	 = up/down;
	
	for(j = 1; j<p; j++)
	{
	    FI(j-1,p-1) = 0;
	    FI(p-1,j-1) = FI(p-2,j-1) - FI(p-1,p-1) * FI(p-2,p-j-1);
	}
    }
//TRZ(FI);
}



//--------------------------------------------------------------------
void PartAutoCor(const BArray<BDat>& r, BArray<BDat>& fi)

/*! Puts in fi the order-partial-autocorrelation vector of
 *  order-autocorrelation vector r
 */
//--------------------------------------------------------------------
{
    BMatrix<BDat> FI;
    DurbinAutoReg(r, FI);
    fi.ReallocBuffer(r.Size());
    for(BInt p = 0; p<r.Size(); p++)
    {
	fi(p)=FI(p,p);
    }
}


//--------------------------------------------------------------------
void InverseAutoCor(const BArray<BDat>& cor, BArray<BDat>& p)

/*! Puts in p the order-inverse-autocorrelation vector of
 *  order-autocorrelation vector cor
 */
//--------------------------------------------------------------------
{
    BInt i, N = cor.Size();
    BPolyn<BDat> corPol;
    corPol.ReallocBuffer(1+N);
    corPol(0).PutDegree(0);
    corPol(0).PutCoef(1);
    for(i=1; i<=N; i++)
    {
	corPol(i).PutDegree(i);
	corPol(i).PutCoef(cor(i-1));
    }
    corPol.Aggregate();
    BRational<BDat> rat = BPolyn<BDat>::One() / corPol;
    BPolyn<BDat> invCorPol = rat % N;
    p.ReallocBuffer(N);
    for(i=0; i<N; i++) { p(i) = invCorPol.Coef(i+1); }
}


//--------------------------------------------------------------------
void InvPartAutoCor(const BArray<BDat>& cor, BArray<BDat>& p)

/*! Puts in p the order-inverse-partial-autocorrelation vector of
 *  order-autocorrelation vector cor
 */
//--------------------------------------------------------------------
{
    BArray<BDat> q;
    InverseAutoCor(cor,q);
    PartAutoCor(q,p);
}


//--------------------------------------------------------------------
BDat BoxPierceLjung(const BArray<BDat>& vec, BInt m, BInt p, BInt q)

/*! Returns the Box-Pierce-Ljung statistic of a vector with m
 *  autocorrelations, p AR-parameters and q MA-parameters
 */
//--------------------------------------------------------------------
{
    BInt k, N = vec.Size();
    BDat Q = 0;
    BArray<BDat> cor;
    AutoCor(vec,cor,m);
    for(k = 0; k<m; k++) { Q += ((cor(k)^2)/BDat(N-k-1)); }
    Q *= BDat(N*(N+2));
    return(Q);
}


//--------------------------------------------------------------------
BDat BoxCoxTrans(      
        BDat          exponent,
        BDat          addition,
  const BArray<BDat>& vec,
        BInt          N,
        BInt          length)
    
/*! Evaluates the Box-Cox transformation (exponent and addition)
 *  of a vector by homokedastic analysis.
 *  \param exponet: power in the Box-Cox transformation
 *  \param addition: number needed add to the vector to be positive
 *  \param vec: data vector
 *  \param period: interval length
 *  \param N: total number of intervals
 *  \param length: period
 *  \return  standard deviation of the differents intervals.
 */
//--------------------------------------------------------------------
{
//BArray<BDat> tz(length);
//Std(BText("\nBoxCoxTrans(")+exponent+","+addition+")\n");
  BInt tVec = vec.Size();
  BArray<BDat> tz(tVec);
  BArray<BDat> nu(N), sigma(N);
  BInt i=0;
  BBool ok=BTRUE;
  BDat bk,z;
  BDat k;
  BInt n;
  for(n=0; (n<vec.Size()); n++)
  {
    tz(n) = vec(n) + addition;   
    if(exponent==0) { tz(n).Log(); }
    else      { tz(n) = tz(n)^exponent; }
  }
  for(n=0; ok && (n<N); n++)
  {
    nu(n) = 0;
    sigma(n) = 0;
    for(BInt t=0; ok && (t<length); t++, i++)
    {
      z = tz(i);
      nu(n) += z;
      sigma(n) += z*z;
      ok = ok && z.IsKnown() && nu(n).IsKnown() && sigma(n).IsKnown();
      k = i/length;
    }
    if(ok)
    {
      nu(n) /= BDat(length);
      sigma(n) /= BDat(length);
      sigma(n) -= nu(n)*nu(n);
      sigma(n).Sqrt();
      ok = nu(n).IsKnown() && sigma(n).IsKnown();
    }
  }
  if(ok)
  {
    bk = Abs(Covarianze(nu,sigma))/Varianze(nu);
  }
//Std(BText("\nBoxCoxTrans(")+exponent+","+addition+") -> "+bk+"\n");
  return(bk);
}


//--------------------------------------------------------------------
void BoxCoxTrans(
  BDat& exponent,
  BDat& addition,
  const BArray<BDat>& vec,
  BInt period,
  const BArray<BDat>& values)
    
/*! Returns the best Box-Cox exponential transformation of a vector
 *  by homokedastic analysis.
 */
//--------------------------------------------------------------------
{
  BText fin;
  if(values.Size()==0) 
  { 
    BoxCoxTrans(exponent,addition,vec,period); 
    return; 
  }
  BInt length = period;
  if(length<=1) { length=10; }
    
  BDat mi  = Minimum(vec);
  addition = fabs(GSL_MIN(0, mi.Value()));
  if (mi.Value() <= 0) 
  {
    BDat lg  = Log(mi.Zero());
    addition += (mi.Zero() * 1000);  
  }
    
  BInt N = vec.Size()/length;
  if(N>3)
  {
    BDat b;
    BInt index = -1;
    for(BInt k=0; k<values.Size(); k++)
    {
      BDat bk = BoxCoxTrans(values(k),addition,vec,N,length);
      if(bk.IsKnown() && (!b.IsKnown()) || (bk<b))
      {
        b = bk;
        index = k;
      }
    }
    if (index != -1)
    { 
      exponent = values(index);
    } 
    else 
    {
      Warning(I2("Negative values after calculation of additive cofficient in ", 
                 "Elementos negativos despues de calcular el coeficiente aditivo en ")+
              "BoxCoxTrans");
    }
  }
  else
  {
    Warning(I2("Cannot apply BoxCoxTrans to a time series with length ",
               "No se puede aplicar BoxCoxTrans a una serie temporal de longitud "));
    exponent = 1;
    addition = 0;
  }
}


//--------------------------------------------------------------------
void BoxCoxTrans(
  BDat& exponent,
  BDat& addition,
  const BArray<BDat>& vec,
  BInt period)
    
/*! Returns the best Box-Cox exponential transformation of a vector
 *  by homokedastic analysis.
 */
//--------------------------------------------------------------------
{
  static BDat vv[9] =
  { -1,-1.0/2.0,-1.0/3.0,-1.0/4.0,0,1.0/4.0,1.0/3.0,1.0/2.0,1 };
  static BArray<BDat> values(9,vv);
  BoxCoxTrans(exponent,addition,vec,period,values);
}



//--------------------------------------------------------------------
void Frequency(const BArray<BDat>& vec, BMatrix<BDat>& M ,
	       BInt parts, BDat min, BDat max)

/*! Puts in M the frequency analysis of vec with parts rows and
 *  two columns. In the first column puts the upper cote of the
 *  n-th interval. In the second column puts the number of values in
 *  vec that are in the n-th interval.
 */
//--------------------------------------------------------------------
{
  BInt i,j;
  if(!vec.Size()) { return; }
  BArray<BDat> sorted(vec.Size());
  for(i=j=0; i<vec.Size(); i++)
  {
    if(vec(i).IsKnown())
    {
      sorted(j++) = vec(i);
    }
  }
  sorted.ReallocBuffer(j);
  sorted.Sort(DatCmp);
  if(!min.IsKnown()) { min = sorted(0); }
  if(!max.IsKnown()) { max = sorted(sorted.Size()-1); }
  BDat	tol = (max-min) / BDat(parts);
  M.Alloc(parts, 2);
  j = 0;
  for(i=1; i<=parts; i++)
  {
    M(i-1,0) = min + i*tol;
    BInt k = j;
    while(j<sorted.Size() && (M(i-1,0)>=sorted(j))) { j++; }
    M(i-1,1) = j-k;
  }
}
