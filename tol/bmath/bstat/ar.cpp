/* ar.cpp: Generic statistical functions.
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

#include <tol/tol_bar.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bfibonac.h>

BTraceInit("ar.cpp");

//--------------------------------------------------------------------
void ApplyPolyn(const BPolyn<BDat>& pol,
		const BArray<BDat>& x,
		      BArray<BDat>& y)
//--------------------------------------------------------------------
{
  if(!pol.Size()) { return; }
  BInt minDeg = pol(0).Degree();
  BInt maxDeg = pol(pol.Size()-1).Degree();
  BInt dif = (maxDeg-minDeg);
//Std(BText("\nApplyPolyn realloc ") + (x.Size()-dif));
  y.ReallocBuffer(x.Size()-dif);
  BInt t = 0, k=dif;
  for(; t<y.Size(); t++, k++)
  {
    y(t) = 0;
    for(BInt i=0; i<pol.Size(); i++)
    {
      y(t) += pol(i).Coef()*x(k-pol(i).Degree());
    }
  }
}


//--------------------------------------------------------------------
static void CalcAutoRegSwartz(const BMatrix<BDat>& fi,
			      const BArray <BDat>& z,
			            BArray <BDat>& err,
			            BDat&	   s,
			            BDat&	   sw,
			            BInt	   n)
//--------------------------------------------------------------------
{
  BDat sum2=0, avr=0;
  BInt N = z.Size();
  BInt i,j;
//Std(BText("\nCalcAutoRegSwartz realloc ") + N);
  err.ReallocBuffer(N);

  for(i=0; i<n; i++)
  {
    err(i) = z(i);
    for(j=1; j<=n; j++) { err(i) -= fi(n,j)*z(i+j); }
    avr	 += err(i);
    sum2 += err(i)*err(i);
  }
  for(i=n; i<N; i++)
  {
    err(i) = z(i);
    for(j=1; j<=n; j++) { err(i) -= fi(n,j)*z(i-j); }
    avr	 += err(i);
    sum2 += err(i)*err(i);
  }
  avr /= N;
  s  = Sqrt(sum2/N - avr*avr);
  sw = 2*Log(s) + n*Log(N)/N;
}



//--------------------------------------------------------------------
class BAutoRegSwartz : public BRealSuc
//--------------------------------------------------------------------
{
public:
  const BMatrix <BDat>& FI_;
  const BArray	<BDat>& z_;
	BArray	<BDat>	err_;
	BDat		s_;
	BDat		sw_;
	BInt		n_;
	BPolyn<BDat>	pol_;

  BAutoRegSwartz(const BMatrix <BDat>& FI, const BArray <BDat>& z)
  : BRealSuc(), FI_(FI), z_(z)
  {
  }

  void Evaluate(BDat& y, const BInt& n)
  {
    CalcAutoRegSwartz(FI_, z_, err_, s_, sw_, n);
//  Std(BText("\nBAutoRegSwartz ")+n+"\t"+sw_+"\t"+s_);
    y=sw_;
  }

  void CalcMinimumBetween(BInt from, BInt until)
  {
//  Std(BText("\nCalcMinimumBetween(") + from + "," + until);
    n_ = BFibonacci::Minimum(this, sw_, from, until);
    if(n_>until) { n_=until; }
    if(n_<from)	 { n_=from; }
//  Std(BText("\nCalcMinimumBetween realloc ") + (n_+1));
    pol_.ReallocBuffer(n_+1);
    pol_(0)=BMonome<BDat>(1,0);
    for(BInt i=1; i<=n_; i++)
    {
      pol_(i)=BMonome<BDat>(-FI_(n_,i),i);
    }
  }
};


//--------------------------------------------------------------------
BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			           BArray <BDat>& e,
			           BDat&	  sigma,
			           BDat&	  swartz,
			           BInt	          minOrder,
			           BInt	          maxOrder)
    
/*! Identifyies the best order of the auto-regression model.
 */
//--------------------------------------------------------------------
{
  BInt		order=-1,N=z.Size();
  BDat		sw;
  BDat		s;
  BMatrix<BDat> FI;
  BArray<BDat>	err,aCorr;

  if(maxOrder<minOrder)
  {
    maxOrder = minOrder + BFibonacci::Inverse(N/4-minOrder);
  }

  AutoCor(z,aCorr,maxOrder);

  DurbinAutoReg(aCorr, FI);

  BAutoRegSwartz ars(FI, z);


  ars.CalcMinimumBetween(minOrder, N/4);

  swartz = ars.sw_;
  sigma	 = ars.s_;
  e	 = ars.err_;

  return(ars.pol_);
}


//--------------------------------------------------------------------
BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			           BArray <BDat>& err,
				   BDat&	  s,
				   BDat&	  sw,
				   BInt	          period,
				   BInt	          minOrder,
				   BInt	          maxOrder)

/*! Identifyies the best order of the auto-regression model.
 */
//--------------------------------------------------------------------
{
  BInt	       N = z.Size();
  if(maxOrder<=minOrder)
  {
    maxOrder = minOrder + BFibonacci::Inverse(N/4-minOrder);
  }
  if(period<2)
  {
    return(IdentifyAutoReg(z,err,s,sw,minOrder,maxOrder));
  }
  else
  {
    BInt i,j,k;
    BArray<BDat> sZ(N/period);
    for(i=j=0; i<sZ.Size(); i++)
    {
      sZ(i)=0;
      for(k=0; k<period; k++,j++) { sZ(i) += z(j); }
      sZ(i)/=period;
    }
    BPolyn<BDat> sPol = IdentifyAutoReg(sZ,1,minOrder/period,1+maxOrder/period);
    return(IdentifyAutoReg(z,err,s,sw,period*sPol.Degree(),maxOrder));
  }
}


//--------------------------------------------------------------------
BPolyn<BDat> IdentifyAutoReg(const BArray <BDat>& z,
			           BInt	          period,
			           BInt	          minOrder,
			           BInt	          maxOrder)
    
/*! Identifyies the best order of the auto-regression model.
 */
//--------------------------------------------------------------------
{
    BDat	       sw;
    BDat	       s;
    BArray<BDat> err;
    return(IdentifyAutoReg(z, err, s, sw, period, minOrder, maxOrder));
}


//--------------------------------------------------------------------
void IdentifyAutoRegIntegrated(const BArray <BDat>& z,
			             BArray<BDat>&  err,
			             BDat&	    s,
			             BDat&	    sw,
			             BArray<BDat>&  w,
			             BPolyn<BDat>&  dif,
			             BPolyn<BDat>&  ar,
			             BInt	    period)

/*! Identifyies the best order of the auto-regression model.
 */
//--------------------------------------------------------------------
{
  BInt min=0;

  BPolyn<BDat> one = BPolyn<BDat>::One();
  BPolyn<BDat> b   = BPolyn<BDat>::B();

  BPolyn<BDat> regDif = one-b;
  BPolyn<BDat> staDif = one-(b^period);

  BPolyn<BDat> difPol [5];
  BPolyn<BDat> arPol  [5];
  BDat	       sigma  [5];
  BDat	       swartz [5];

  difPol[0] = BPolyn<BDat>::One();
  difPol[1] = regDif;
  difPol[2] = regDif^2;
  if(period>=3)
  {
    difPol[3] = staDif;
    difPol[4] = regDif*staDif;
  }

  for(BInt k=0; (k<2) || ((period>=3)&&(k<5)); k++)
  {
    ApplyPolyn(difPol[k], z, w);
    arPol[k] = IdentifyAutoReg(w,err,sigma[k],swartz[k],period,-1,-1);
    if((k==0) || (s > sigma[k]))
    {
      sw = swartz[k];
      s = sigma[k];
      min = k;
    }
//  Std(BText("\n  ")+k+"\t"+sigma(k).Name()+"\t"+swartz(k).Name()+
//	      "\t"+ difPol(k).Name()+"\t"+arPol(k).Name());
  }

  ar  = arPol [min];
  dif = difPol[min];
}


//--------------------------------------------------------------------
void IdentifyAutoRegIntegrated(const BArray <BDat>& z,
			             BPolyn<BDat>&  dif,
			             BPolyn<BDat>&  ar,
			             BInt	    period)

/*! Identifyies the best order of the auto-regression model.
 */
//--------------------------------------------------------------------
{
    BArray<BDat>	err;
    BDat		s;
    BDat		sw;
    BArray<BDat>	w;
    IdentifyAutoRegIntegrated(z,err,s,sw,w,dif,ar,period);
}
