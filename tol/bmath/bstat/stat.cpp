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

#include <gsl/gsl_math.h>
#include <gsl/gsl_linalg.h>

#include <tol/tol_bstat.h>
#include <tol/tol_matrix.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_gsl.h>
#include <tol/tol_gslmat.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_bratio.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_barma.h>

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
BDat Known(const BArray<BDat>& vec)

/*! Returns the Sum statistic of a vector of real numbers
 */
//--------------------------------------------------------------------
{
    if(!vec.Size()) { return(0); }
    BDat result = 0;
    for(BInt k = 0; k<vec.Size(); k++)
    {
	if(vec(k).IsKnown())
	{
	    result += 1;
	}
    }
    return(result);
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
    return(CenterMoment(vec,3)/(Varianze(vec)^(1.5)));
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
      if(allUnknown) { allUnknown=false; }
      if(result<vec(k)) {  result = vec(k); }
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
      if(allUnknown) { allUnknown=false; }
      if(result>vec(k)) { result = vec(k); }
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

/*! Returns the covarianze between rows of data
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
//Std(BText("TRACE [Covariance] d=\n")+d.Name());
//Std(BText("TRACE [Covariance] cov=\n")+cov.Name());
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
    BDat sqi = sq(i);
    for(j=0; j<i; j++)
    {
      BDat sqj = sq(j);
      if((sqi!=0)&&(sqj!=0))
      {
        BDat cij = cor.Get(i,j);
        BDat sij = sqi*sq(j);
        BDat xij = cij / sij;
        cor(i,j)=xij;
      }
      else
      { 
        cor(i,j)=0;
      }
    }
    if(sqi!=0) { cor(i,i)=1; }
    else       { cor(i,i)=0; }
  }
//Std(BText("TRACE [CorrelationF] cov=\n")+cov.Name());
//Std(BText("TRACE [CorrelationF] cor=\n")+cor.Name());
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
  BMatrix<BDat> A = cor; 
  BMatrix<BDat> U(r, r), V(r, r), Vt;
  BDiagMatrix<BDat> D(r, 0), Dp;
//gsl_SingularValueDecomposition(A,U,D,V, "Golub_Reinsch");
//gsl_SingularValueDecomposition(A,U,D,V, "Jacobi");
  SingularValueDecomposition(A,U,D,V);
  Vt = V.T();
  Dp = D.P(DEpsilon());
  BMatrix<BDat> Ap = V * Dp * Vt;
  pcor.Alloc(r);
  if((Ap.Rows()==r) && (Ap.Columns()==r))
  {
    for(i=0; i<r; i++)
    {
      for(j=0; j<r; j++)
      {
        pcor(i,j) = Ap(i,j) / Sqrt(Ap(i,i)*Ap(j,j));
      }
      pcor(i,i) = 1;
    }
  }
  pcor *= -1;

//Std(BText("TRACE [PartialCorrelationF] A=\n")+A.Name());
//Std(BText("TRACE [PartialCorrelationF] U=\n")+U.Name());
//Std(BText("TRACE [PartialCorrelationF] D=\n")+D.Name());
//Std(BText("TRACE [PartialCorrelationF] Dp=\n")+Dp.Name());
//Std(BText("TRACE [PartialCorrelationF] Ap=\n")+Ap.Name());
//Std(BText("TRACE [PartialCorrelationF] pcor=\n")+pcor.Name());

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
    
/ *! Returns the partial correlation between rows of data
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
  BInt i,t,m=vec.Size();
  cov.ReallocBuffer(order);
  const BDat* vec_ = vec.Buffer();
  const BDat* vec_t;
  const BDat* vec_ti;
  BDat* cov_ = cov.GetBuffer();
  if(vec.Size()>1)
  {
    if(demean==1)
    {
	    BDat avr = Average(vec);
      for(i = 0; i<order; i++, cov_++)
	    {
	      *cov_=0;
        vec_t = vec_+i;
        vec_ti = vec_;
	      for(t=i; t<vec.Size(); t++, vec_t++, vec_ti++)
	      {
		      if(vec_t->IsKnown()&&vec_ti->IsKnown()) 
		      {
		        *cov_+= (*vec_t-avr)*(*vec_ti-avr); 
		      }
	      }
	      *cov_/=m;
	    }
    }
    else
    {
	    for(i = 0; i<order; i++, cov_++)
	    {
	      *cov_=0;
        vec_t = vec_+i;
        vec_ti = vec_;
	      for(t=i; t<vec.Size(); t++, vec_t++, vec_ti++)
	      {
		      if(vec_t->IsKnown()&&vec_ti->IsKnown()) 
		      {
		        *cov_+= *vec_t * *vec_ti; 
		      }
	      }
	      *cov_/=m;
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
BDat BoxPierceACF(const BArray<BDat>& acf, BInt m, BInt s)

/*! Returns the Box-Pierce statistic for first m autocorrelations
 *  of a time series of length s 
 */
//--------------------------------------------------------------------
{
  BDat Q = 0;
  BInt k;
  if(m>acf.Size()) { m=acf.Size(); }
  for(k=1; k<=m; k++) 
  { 
    Q += (acf(k-1)^2); 
  }
  Q *= BDat(s);
  return(Q);
}

//--------------------------------------------------------------------
double BoxPierceCenterMoment1(int m, int n)
//--------------------------------------------------------------------
{
  int h = max(0,m-n/2);
  double n2 = n*n;
  double n3 = n*n2;
  double EQBP = (m*(2*n3-(m+3)*n2+(m+1)*(2*m+1))-4*n*h*(2*m-n+1-h)) /
                (2*n*(n2-1));
  return(EQBP);
}

//--------------------------------------------------------------------
double BoxPierceCenterMoment2(int m, int n)
//--------------------------------------------------------------------
{
  double m2 = m*m;
  double m3 = m*m2;
  double m4 = m*m3;
  double m5 = m*m4;
  double n2 = n*n;
  double n3 = n*n2;
  double n4 = n*n3;
  double n5 = n*n4;
  double n6 = n*n5;
  int zm = m%2;
  int zn = n%2;
  int q2 = max(0,2*m-n+1)/2;
  int q3 = max(0,3*m-n+1)/2;
  int q4 = max(0,4*m-n+2)/2;
  double EQBP2 = (1/((n2-1)*(n+3)*(n+5))) * 
  (
    (m/(12*n2))*(
      24*n*(-2-2*n-n2+3*n3-2*n4+n5)
      +(35+114*n2+60*n3-261*n4+84*n5+12*n6)*m
      -6*(-35-80*n-70*n2-14*n3+17*n4+2*n5)*m2
      +(455+720*n+282*n2+24*n3+3*n4)*m3
      -12*(-35-24*n+n2)*m4
      +140*m5
    )
    +(4*n*(n-m)*(n-m-1)+6*m-2*n+3)*zm
    +4*n*q4*(q4+1)*(4*q4-12*m+3*n-4)
    +4*q3*(q3+1)*(2*q3*(q3+1)-(3*m-n+1)*(3*m-n+2)-1)
    +(q2/(6*n))*(
      -(2*m-n+2)*(
        (24-84*n-150*n2+59*n3+16*n4)
        -2*m*(-63-54*n+63*n2+10*n3)
        -6*m2*(-39-18*n+n2)
        +156*m3
      )
      +zn*(
        48-27*n-111*n2+43*n3+16*n4
        -2*m*(-87-111*n+63*n2+10*n3)
        -6*m2*(-47-18*n+n2)
        +156*m3
      )
    )
  ); 
  return(EQBP2);
}

//--------------------------------------------------------------------
BDat BoxPierceModACF(const BArray<BDat>& acf, BInt m, BInt s)

/*! Returns the modified Box-Pierce statistic for first m autocorrelations
 *  of a time series of length s 
 */
//--------------------------------------------------------------------
{
  BDat QBP = BoxPierceACF(acf, m, s);
  BDat EQBP  = BoxPierceCenterMoment1(m, s);
  BDat EQBP2 = BoxPierceCenterMoment2(m, s);
  BDat VQBP  = EQBP2-(EQBP^2);
  BDat QBPM = m+Sqrt(2*m/VQBP)*(QBP-EQBP);
  return(QBPM);
}

//--------------------------------------------------------------------
BDat BoxPierceLjungACF(const BArray<BDat>& acf, BInt m, BInt s)

/*! Returns the Box-Pierce-Ljung statistic for first m autocorrelations
 *  of a time series of length s 
 */
//--------------------------------------------------------------------
{
  BDat Q = 0;
  BInt k;
  if(m>acf.Size()) { m=acf.Size(); }
  for(k=1; k<=m; k++) 
  { 
    Q += ((acf(k-1)^2)/BDat(s-k)); 
  }
  Q *= BDat(s*(s+2));
  return(Q);
}


//--------------------------------------------------------------------
BDat BoxPierceLjung(const BArray<BDat>& vec, BInt m)

/*! Returns the Box-Pierce-Ljung statistic for first m autocorrelations
 *  of a time series vec.
 */
//--------------------------------------------------------------------
{
  BInt s = vec.Size();
  BArray<BDat> acf;
  AutoCor(vec,acf,m);
  return(BoxPierceLjungACF(acf,m,s));
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
    // unused
    // BDat lg  = Log(mi.Zero());
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
      // suggest parentheses around ‘&&’ within ‘||’ [-Wparentheses]
      if(bk.IsKnown() && (!b.IsKnown() || (bk<b)))
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

//--------------------------------------------------------------------
static BDat AbsNormal(BDat x)
//--------------------------------------------------------------------
{
  return(2*BNormalDist::Dist01(Abs(x)) - BDat(1));
}

//--------------------------------------------------------------------
bool Diagnostic_ARIMA_ResAcf_NormalUnivariate(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& acf,
  int dataLength,
  int lag)
//--------------------------------------------------------------------
{
  if(acf.Size()<lag) { return(false); }
  statValue = acf[lag];
  refuseProb = AbsNormal(statValue*Sqrt(dataLength)); 
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_ARIMA_ResAcf_BoxPierceLjung(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& acf,
  int dataLength,
  int p,
  int q)
//--------------------------------------------------------------------
{
  if(acf.Size()<=p+q) { return(false); }
  int m = acf.Size();
  statValue = BoxPierceLjungACF(acf, m, dataLength);
  BChiSquareDist Chi(m-p-q);
  refuseProb = Chi.Dist(statValue); 
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_ARIMA_ResAcf_BoxPierceMod(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& acf,
  int dataLength,
  int p,
  int q)
//--------------------------------------------------------------------
{
  if(acf.Size()<=p+q) { return(false); }
  int m = acf.Size();
  statValue = BoxPierceModACF(acf, m, dataLength);
  BChiSquareDist Chi(m-p-q);
  refuseProb = Chi.Dist(statValue); 
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_NormalReg_Res_Pearson(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& res)
//--------------------------------------------------------------------
{
  int N_ = res.Size();
  if(N_<=100) { return(false); }
  BInt         i,j;
//Std(BText("\nTRACE PearsonNormalityTest "));
  BNormalDist U(0,1);
  //Cálculo de la desviación típica en base a cuantiles para filtrar outliers
  BDat sQ = 0;
  {
    int nQ = (N_>30)?3:1;
    BArray<BDat> q(nQ), p(nQ), Q(nQ);
    if(nQ==1) 
    {
      q[0]=1.0;
    }
    else
    {
      q[0]=0.9; 
      q[1]=1.0; 
      q[2]=1.1; 
    }
    for(i=0; i<nQ; i++) { p[i] = U.Dist(q[0]); }
    BArray<BDat> A(N_);
    for(i=0; i<N_; i++) { A[i] = Abs(res(i)); }
    Quantile(A, p, Q);
    for(i=0; i<nQ; i++) 
    { 
      sQ += Q[i]/(Abs(q[i])*Sqrt(2.0));
    }
    sQ /= nQ;
  }

  //Filtrado de outliers
  BArray<BDat> Y(N_);
  BDat kSig = -U.Inverse(1.0/N_,0);
  for(i=j=0; i<N_; i++)
  {
    BDat x = res(i)/sQ;
    if(Abs(x) <= kSig) { Y(j++) = x; }
  }
  int N = j;
  Y.ReallocBuffer(N);

  //Pearson goodness of fit test statistic
  BInt nQ = (N>=900)?(int)sqrt((double)N):(N>=400)?30:(N>=100)?6:4;
//Std(BText("TRACE BModel::PearsonNormalityTest nQ=")+nQ+" sQ="+sQ+"\n");
  BChiSquareDist Chi(nQ-3);
  BArray<BDat> q(nQ+1);
  BDat chi=0; 
  BDat step = (((double)N_-2.0)/double(N_*nQ));
  BDat p = 1.0/N_;
  for(j=0; j<=nQ; j++, p+=step)
  {
    q[j] = U.Inverse(p,0);
  }
  p = 1.0/nQ;
  for(j=1; j<=nQ; j++)
  {
    BDat q0 = q[j-1];
    BDat q1 = q[j];
    BDat Q = 0;
    for(i=0; i<N; i++)
    {
      Q += And(q0 <= Y[i], Y[i] < q1);
    }
    Q/=N;
    BDat dif = Q-p;
    BDat dif2 = dif*dif;
    BDat dif2p = dif2/p;
  //Std(BText("TRACE BModel::PearsonNormalityTest   Q=")+Q+" p="+p+" dif="+dif+" dif2="+dif2+" dif2p="+dif2p+"\n");
    chi += dif2p;
  }
  statValue = chi;
  refuseProb = Chi.Dist(chi); 
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_NormalReg_ParamSignif(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& mean,
  const BArray<BDat>& stdErr,
  int freeDeg)
//--------------------------------------------------------------------
{
  int numParam = mean.Size();
  if(!numParam) { return(false); }

  BTStudentDist T(freeDeg);
  BInt i;
  statValue=BDat::PosInf();
  for(i=0; i<numParam; i++)
  {
    BDat t = Abs(mean[i]/stdErr[i]);
    if(!t.IsKnown())
    {
      statValue = t;
      refuseProb = 1;
      break;
    }
    else if(statValue > t)
    {
      statValue = t;
    }
  }
  refuseProb = 2*(1-T.Dist(statValue)); 
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_NormalReg_ParamMulticolinearity(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BDat>& mean,
  const BArray<BDat>& covEigenValues,
  const BMatrix<BDat>& covEigenVectors,
  int dataLength,
  BDat standardError)
//--------------------------------------------------------------------
{
  int numParam = mean.Size();
  if(!numParam) { return(false); }
  if(covEigenValues.Size()!=numParam ||
     covEigenVectors.Rows()!=numParam ||
     covEigenVectors.Columns()!=numParam ||
     standardError.IsUnknown())
  {
    statValue = BDat::Unknown(); 
    refuseProb = 1; 
    return(true);
  }
  int i,j;
  BTStudentDist T(dataLength-numParam);
  statValue = 0;
  for(j=0; j<numParam; j++)
  {
    BDat c = 0;
    for(i=0;i<numParam;i++) { c +=mean[i]*covEigenVectors(i,j); }
    BDat d = Sqrt(covEigenValues(i));
    BDat s = standardError/d;
    BDat t = c/s;
    BDat p = 2*(1-T.Dist(Abs(t)));
    if(p>statValue) 
    { 
      statValue = t; 
      refuseProb = p; 
    }
  }
  return(true);
}

//--------------------------------------------------------------------
bool Diagnostic_ARIMA_ParamUnitRoots(
  BDat& statValue,
  BDat& refuseProb,
  const BArray<BARIMAFactor>& factors,
  const BMatrix<BDat>& z,
  const BMatrix<BDat>& w0,
  const BMatrix<BDat>& a0)
//--------------------------------------------------------------------
{
  BARIMA arima;
  arima.PutFactors(factors);
  arima.PutOutputData(z);

  statValue = 0;
  refuseProb = 1; 
  return(true);
}



