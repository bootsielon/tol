/* logit.cpp: logistic model.
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
#include <tol/tol_blogit.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bltrimat.h>
#include <tol/tol_btimer.h>

BTraceInit("logit.cpp");


//--------------------------------------------------------------------
BBool Logit(const BMatrix    <BDat>& y,
	    const BMatrix    <BDat>& X,
		  BMatrix    <BDat>& B,
		  BMatrix    <BDat>& e,
		  BMatrix    <BDat>& p,
		  BMatrix    <BDat>& G,
		  BSymMatrix <BDat>& H,
		  BMatrix    <BDat>& lnL,
		  BDat&	             lnLikelyhood,
		  BDat&	             likelyhood)

/*! Estimates the boolean model
 *
 *		y = F(X * B) + e;
 *
 *		  dim(B) = n   : number of variables
 *		  dim(y) = N   : number of data
 *		  dim(X) = N,n
 *		  dim(e) = N
 *
 *  where y is 0 or 1 and F is the logistic distribution.
 */
//--------------------------------------------------------------------
{
  BInt i, j, k, iter, n=X.Columns(), N=X.Rows();
  BTimer tm(BText("Logit model (")+N+"x"+n+")");

  BMatrix<BDat> dif, f(N,1), H_(n,n), Xb(N,1);
  e.Alloc(N,1);
  p.Alloc(N,1);
  lnL.Alloc(N,1);
  B.Alloc(n,1);
  G.Alloc(n,1);
  H.Alloc(n);
  BDat norm, advance, maxAbsDif, oldLlh;
  B.SetAllValuesTo(0);
  clock_t tm_1, tm_0 = BTimer::Clocks();
  BReal tm_dif;
  BDat Lp, L1p;

  for(iter=0; iter<BDat::MaxIter(); iter++)
  {
    H_.SetAllValuesTo(0);
    G.SetAllValuesTo(0);
    if(iter==0)
    {
      Xb.SetAllValuesTo(0);
    }
    else
    {
    //Std(BText("\nTRACE Logit B=\n")+B.Name());
    //Std(BText("\nTRACE Logit X=\n")+X.Name());
      Xb = X*B;
    //Std(BText("\nTRACE Logit Xb=\n")+Xb.Name());
    }
    oldLlh = lnLikelyhood;
    lnLikelyhood=0;
    for(i=0; i<N; i++)
    {
      if(Xb(i,0)>=0)
      {
        p(i,0) = 1.0/(Exp(-Xb(i,0))+1.0);
      }
      else
      {
        p(i,0) = Exp(Xb(i,0))/(Exp(Xb(i,0))+1.0);
      }
      f(i,0) = p(i,0)*(1.0-p(i,0));
      e(i,0) = y(i,0)-p(i,0);
      for(j=0; j<n; j++)
      {
	      G(j,0) += X(i,j)*e(i,0);
	      for(k=0; k<n; k++)
        {
	        H_(j,k) -= f(i,0)*X(i,j)*X(i,k);
        }
      }
	    if(Abs(e(i,0))<=BDat::Tolerance()) { lnL(i,0) = 0.0; }
      else 
      {
        if(Xb(i,0)>=0)
        {
	        Lp  = Log(p(i,0));
          L1p = -Xb(i,0)-Log(1+Exp(-Xb(i,0)));
        }
        else 
        {
	        Lp  =  Xb(i,0)-Log(1+Exp( Xb(i,0)));
          L1p = Log(1-p(i,0));
        }
        lnL(i,0) = y(i,0)*Lp + (1-y(i,0))*L1p;
      }
      if(!lnL(i,0).IsFinite())
      {
        Std(BText("\n  Non finite log-likelihood i=")+i+"\ty="+y(i,0)+"\tXb="+Xb(i,0)+"\tp="+p(i,0)+"\tlog(p)="+Lp+"\tlog(1-p)="+L1p+"\tlnL="+lnL(i,0));
      }
      lnLikelyhood += lnL(i,0);
    }
    H = H_;
    //dif	    = MinimumResidualsSolve(H_,G);
      //MinimumResidualsSolve(H_,G, BDat(DEpsilon()));
    dif = gsl_MinimumResidualsSolve(H_,G);
    norm    = G.FrobeniusNorm();
    advance = dif.FrobeniusNorm();
    maxAbsDif = dif.MaxAbsNorm();
    tm_1 = BTimer::Clocks();
    tm_dif = BTimer::ClockToSecond(tm_1-tm_0);
    tm_0 = tm_1;
    //likelyhood = Exp(lnLikelyhood);
    Std(BText("\n  Logit model iteration(")+iter+") "+
	      "	 LogLikelyhood = "+lnLikelyhood+
        //"	 Likelyhood = "+likelyhood+
	      "	 maxAbsDif = "+maxAbsDif+
	      "	 Gradient Norm = "+norm+
	  I2("\tTime : "," Tiempo : ") + tm_dif+I2(" seconds"," segundos"));

    if(advance.IsUnknown()) { break; }
    if(norm	 < DEpsilon()	    ) { break; }
    if(maxAbsDif < BDat::Tolerance()) { break; }
    if(advance	 < DEpsilon()	    ) { break; }
    if(Abs(oldLlh-lnLikelyhood) < DEpsilon()) { break; }
    if(iter==BDat::MaxIter()-1) { break; }
    B -= dif;
  }
  return BTRUE;
}



//--------------------------------------------------------------------
BBool Probit(const BMatrix    <BDat>& y,
	           const BMatrix    <BDat>& X,
		               BMatrix    <BDat>& B,
		               BMatrix    <BDat>& e,
		               BMatrix    <BDat>& p,
		               BMatrix    <BDat>& G,
		               BSymMatrix <BDat>& H,
		               BMatrix    <BDat>& lnL,
		               BDat&	            lnLikelyhood,
		               BDat&	            likelyhood)

/*! Estimates the boolean model
 *
 *		y = F(X * B) + e;
 *
 *		  dim(B) = n   : number of variables
 *		  dim(y) = N   : number of data
 *		  dim(X) = N,n
 *		  dim(e) = N
 *
 *  where y is 0 or 1 and F is the logistic distribution.
 */
//--------------------------------------------------------------------
{
  BInt i, j, k, iter, n=X.Columns(), N=X.Rows();
  BTimer tm(BText("Probit model (")+N+"x"+n+")");
  double chop = Sqrt(DEpsilon()/N);

  BMatrix<BDat> dif, f(N,1), l(N,1), H_(n,n), XB;
  e.Alloc(N,1);
  p.Alloc(N,1);
  lnL.Alloc(N,1);
  B.Alloc(n,1);
  G.Alloc(n,1);
  H.Alloc(n);
  p.Alloc(N,1);
  BDat norm, advance, maxAbsDif, oldLlh;
  B.SetAllValuesTo(0);
  clock_t tm_1, tm_0 = BTimer::Clocks();
  BReal tm_dif;
  lnLikelyhood = BDat::NegInf();
  BDat _p, _1p;
  for(iter=0; iter<BDat::MaxIter(); iter++)
  {
    H_.SetAllValuesTo(0);
    G.SetAllValuesTo(0);
    XB = X*B;
    oldLlh = lnLikelyhood;
    lnLikelyhood=0;
    for(i=0; i<N; i++)
    {
      _1p = BNormalDist::Dist01(-XB(i,0));
       _p = BNormalDist::Dist01( XB(i,0));
      p(i,0) = _p;
      f(i,0) = Exp(-0.5*(XB(i,0)*XB(i,0)))/Sqrt(2.0*BDat::Pi());//BNormalDist::Dens01(XB(i,0));
      l(i,0) = (y(i,0)==0)?-f(i,0)/(_1p):f(i,0)/_p;
      e(i,0) = y(i,0)-_p;
      for(j=0; j<n; j++)
      {
	      G(j,0) += X(i,j)*l(i,0);
	      for(k=0; k<n; k++)
	      {
	        H_(j,k) -= l(i,0)*(l(i,0)+XB(i,0))*X(i,j)*X(i,k);
	      }
      }
	    if(Abs(e(i,0))<=BDat::Tolerance()) { lnL(i,0) = 0.0;	     }
      else
      {
	      lnL(i,0) = y(i,0)*Log(_p)+(1-y(i,0))*Log(_1p);
      }
//    Std(BText("\ny=")+y(i,0)+"\tXB="+XB(i,0)+
//		"\tp="+_p+"\tf="+f(i,0)+"\tl="+l(i,0)+
//		"\te="+e(i,0)+"\tlnL="+lnL(i,0));
      lnLikelyhood += lnL(i,0);
    }
    H = H_;
  //dif	    = MinimumResidualsSolve(H_,G);
    dif       = gsl_MinimumResidualsSolve(H_,G);
    norm      = G.FrobeniusNorm();
    advance   = dif.FrobeniusNorm();
    maxAbsDif = dif.MaxAbsNorm();
    tm_1      = BTimer::Clocks();
    tm_dif    = BTimer::ClockToSecond(tm_1-tm_0);
    tm_0      = tm_1;
//  likelyhood = Exp(lnLikelyhood);
    Std(BText("\n  Probit model iteration(")+iter+") "+
	      "	 LogLikelyhood = "+lnLikelyhood+
//	      "	 Likelyhood = "+likelyhood+
	      "	 maxAbsDif = "+maxAbsDif+
	      "	 Gradient Norm = "+norm+
	     I2("\tTime : "," Tiempo : ") + tm_dif+I2(" seconds"," segundos"));

    if(advance.IsUnknown()) { break; }
    if(norm	     < chop 	          ) { break; }
    if(maxAbsDif < BDat::Tolerance()) { break; }
    if(advance	 < chop     	      ) { break; }
    if(Abs(oldLlh-lnLikelyhood) < DEpsilon()) { break; }   
    if(iter==BDat::MaxIter()-1) { break; }
    B -= dif;
  }
  return BTRUE;
}



//--------------------------------------------------------------------
BInt BooleanModelIteration(const BArray     <BDat>& y,
			   const BMatrix    <BDat>& X,
				 BArray     <BDat>& B,
				 BArray     <BDat>& e,
				 BSymMatrix <BDat>& cov,
				 BArray     <BDat>& p,
				 BArray     <BDat>& yPrev,
				 BDat&              s,
			         BDat&              R2,
			         BDat&              F,
			         BInt&              v1,
			         BInt&              v2,
			         BDat&              alfa,
			         BDat&              resSqSum,
			         BDat               tolerance,
			         BProbDist*         dist)
//--------------------------------------------------------------------
{
  BArray<BDat> T,b,err;
  BInt m = X.Columns();
  BInt n = X.Rows();
  BInt i, j;
  BMatrix<BDat> C = X;
  BArray<BDat>	u = y;
  BArray<BDat>	v = y;
  BDat g, absb;

  BDat sOld = s;
  BDat prop;

  for(i=0; i<n; i++)
  {
    v(i) = Sqrt(p(i)*(1.0-p(i)));
    u(i) = (y(i)-p(i))/v(i);
    for(j=0; j<m; j++)
    {
      C(i,j) *= v(i);
    }
  }
  C*=(-1);
  if(!HouseholderTransformation(T,C))
  {
    return(-1);
  }

	LeastSqrHouseholder(T,C,u,b,err);

  for(i=0; i<m; i++) { B(i) += b(i); }

  e = y;
  prop = s = 0;
  for(i=0; i<n; i++)
  {
    g = 0;
    for(j=0; j<m; j++) { g += B(j) * X(i,j); }
    p(i) = dist->Dist(g);
    yPrev(i) = Round(p(i));
    if(yPrev(i)!=y(i)) { prop+=1; }
    e(i) -= p(i);
    s += e(i) ^ 2;
  }

  prop /= n;
  s /= n;
  s.Sqrt();

  BLowTrMatrix<BDat> Rt(m);

  for(i=0; i<m; i++)
  {
    for(j=0; j<i; j++) { Rt(i,j)=C(j,i); }
    Rt(i,i) = T(i);
  }

  BLowTrMatrix<BDat> Rti;
  Rt.Inverse(Rti);
  cov = MtMSqr(Rti);

  cov *= s*s;
  for(i=0; i<n; i++)
  {
    for(j=0; j<m; j++)
    {
      cov(i,j) /= v(i);
    }
  }
  BArray<BDat> yp = y;
  for(i=0; i<n; i++) { yp(i) += e(i); }
  R2 = R2Coefficient(y,yp);
  v1 = m;
  v2 = n-m-1;
  F  = R2*BDat(v2) / ((1-R2)*BDat(v1));
  alfa = BFSnedecorDist(v1,v2).Dist(F);
  resSqSum = Moment(e,2);


  BDat ds = sOld-s;
  BDat d  = FrobeniusNorm(b);
/*
  Std(BText("\n######################################################\n"));
  Std(BText("\nB=\n")+Name(B));
//Std(BText("\ne=\n")+Name(e));
//Std(BText("\nP=\n")+Name(p));
  Std(BText("\nprop=\n")+prop.Name());
  Std(BText("\ns=\n")+s.Name());
  Std(BText("\nd=\n")+d.Name());
  Std(BText("\ntolerance=\n")+tolerance.Name());
  Std(BText("\n######################################################\n"));
*/
  return((d<=tolerance) && (ds>=0) && (ds<=tolerance));
}



//--------------------------------------------------------------------
BBool BooleanModel(const BArray     <BDat>& y,
		   const BMatrix    <BDat>& X,
		         BArray     <BDat>& B,
		         BArray     <BDat>& e,
		         BSymMatrix <BDat>& cov,
		         BArray     <BDat>& p,
		         BArray     <BDat>& yPrev,
		         BDat&              s,
		         BDat&              R2,
		         BDat&              F,
		         BInt&              v1,
		         BInt&              v2,
		         BDat&              alfa,
		         BDat&              resSqSum,
		         BDat&              totSqSum,
		         BDat               tolerance,
		         BProbDist*         dist)
    
/*! Estimates the boolean model
 *
 *		y = F(X * A) + e;
 *
 *		  dim(B) = m   : number of variables
 *		  dim(y) = n   : number of data
 *		  dim(X) = n,m
 *		  dim(e) = n
 *
 *		where y is 0 or 1.
 */
//--------------------------------------------------------------------
{
  BInt i, j, result;
  BInt n = X.Rows();
  BInt m = X.Columns();
  p.ReallocBuffer(n);
  B.ReallocBuffer(m);
  BInt iter = 0;
  totSqSum = Varianze(y);
  s = Sqrt(totSqSum);

  for(i=0; i<n; i++)
  {
    p(i) = 0.5;
    if((y(i)!=0.0)&&(y(i)!=1.0))
    {
      Error(I2("There non 0 or 1 values for output variable in a Logistic "
	       "Model.",
	       "Existen valores distintos de 0 ó 1 en la variable respuesta "
	       "de un Modelo Logístico."));
      return(BFALSE);
    }
  }
  for(j=0; j<m; j++)
  {
    B(j) = 0.0;
  }

  BTimer tmr(I2("Logistic estimation","Estimación logística"));
  Std(I2("\n Number of variables ", "\nNúmero de variables ") + n);
  Std(I2("\n Number of data ", "\nNúmero de datos ") + m);
  Std(I2("\n Standard deviation "," Desviación estandard ") + s);
  yPrev.ReallocBuffer(n);
  do
  {
    iter++;
    result=BooleanModelIteration(y,X,B,e,cov,p,yPrev,
				 s,R2,F,v1,v2,alfa,resSqSum,tolerance,dist);
    Std(I2("\nIteration ","\nIteración ")+iter+
	I2(" Standard deviation "," Desviación estandard ") + s);
  }
  while(!result && (iter<=BDat::MaxIter()));

  return(result==1);
}




/*
//--------------------------------------------------------------------
   BBool Probit(const BArray	 <BDat>& y,
		const BMatrix	 <BDat>& X,
		      BArray	 <BDat>& B,
		      BArray	 <BDat>& e,
		      BSymMatrix <BDat>& cov,
		      BArray	 <BDat>& p,
		      BArray	 <BDat>& yPrev,
				  BDat & s,
				  BDat & R2,
				  BDat & F,
				  BInt & v1,
				  BInt & v2,
				  BDat & alfa,
				  BDat & resSqSum,
				  BDat & totSqSum,
				  BDat	 tolerance)

// PURPOSE: Estimates the boolean model
//
//		y = F(X * A) + e;
//
//		  dim(B) = m   : number of variables
//		  dim(y) = n   : number of data
//		  dim(X) = n,m
//		  dim(e) = n
//
//		where y is 0 or 1.
//
//--------------------------------------------------------------------
{
  static BNormalDist dist;
  return(BooleanModel(y,X,B,e,cov,p,yPrev,s,
		      R2,F,v1,v2,alfa,resSqSum,totSqSum,tolerance,&dist));
}
*/

