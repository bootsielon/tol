/* arma.cpp: Generic statistical functions.
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
#include <tol/tol_bvmat.h>
#include <tol/tol_barma.h>
#include <tol/tol_bstat.h>
#include <tol/tol_btimer.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>
#include <tol/tol_bsvdsep.h>
#include <tol/tol_butil.h>
#include <tol/tol_bvmat.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bar.h>
#include <tol/tol_bprdist.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("arma.cpp");
static BArray<BARIMAFactor> imp_1_;


//--------------------------------------------------------------------
static BDat GetBackwardValue(const BPolyn  <BDat>& P,
			     const BMatrix <BDat>& X,
			     BInt i, BInt j, BInt k)
//--------------------------------------------------------------------
{
  BDat xk = 0;
  i-=P[k].Degree();
  if(i>=0) { xk = X (i,j); }
  else     { xk = 0; }
  return(P[k].Coef() * xk);
}

//--------------------------------------------------------------------
static BDat GetBackwardValue(const BPolyn  <BDat>& P,
			     const BMatrix <BDat>& X0,
			     const BMatrix <BDat>& X,
			     BInt i, BInt j, BInt k)
//--------------------------------------------------------------------
{
  BDat xk = 0;
  i-=P[k].Degree();
  if(i>=0) { xk = X (i,j); }
  else     { xk = X0.Get(X0.Rows()+i,j); }
  return(P[k].Coef() * xk);
}

//--------------------------------------------------------------------
static BDat GetForwardValue(const BPolyn  <BDat>& P,
			    const BMatrix <BDat>& X,
			    BInt i, BInt j, BInt k)
//--------------------------------------------------------------------
{
  BDat xk = 0;
  i+=P[k].Degree();
  if(i<X.Rows()) { xk = X (i,j); }
  else	   { xk = 0; }
  return(P[k].Coef() * xk);
}

//--------------------------------------------------------------------
static BDat GetForwardValue(const BPolyn  <BDat>& P,
			    const BMatrix <BDat>& X0,
			    const BMatrix <BDat>& X,
			    BInt i, BInt j, BInt k)
//--------------------------------------------------------------------
{
  BDat xk = 0;
  i+=P[k].Degree();
  if(i<X.Rows()) { xk = X (i,j); }
  else	   { xk = X0.Get(i-X.Rows(),j); }
  return(P[k].Coef() * xk);
}

//--------------------------------------------------------------------
void Backward(const BPolyn  <BDat>& P,
       	      const BMatrix <BDat>& X0,
	            const BMatrix <BDat>& X,
	                  BMatrix <BDat>& Y)
//--------------------------------------------------------------------
{
  BInt i,j,k;
  BInt r  = X.Rows();
  BInt c  = X.Columns();
  Y.Alloc(r,c);
  for(j=0; j<c; j++)
  {
   	for(i=0; i<r; i++)
	   {
	     Y(i,j) = 0;
	     for(k=0; k<P.Size(); k++)
	     {
		      Y(i,j) += GetBackwardValue(P,X0,X,i,j,k);
	     }
	   }
  }
}


//--------------------------------------------------------------------
void MatBackwardDifEq(const BRational <BDat>& R,
		                  const BMatrix	  <BDat>& X,
		                        BMatrix	  <BDat>& Y)
//--------------------------------------------------------------------
{
  BInt i,j,k;
  BInt r  = X.Rows();
  BInt c  = X.Columns();
  Y.Alloc(r,c);
  for(j=0; j<c; j++)
  {
	   for(i=0; i<r; i++)
	   {
	     BDat y = 0;
	     for(k=0; k<R.Numerator().Size(); k++)
	     {
		      y += GetBackwardValue(R.Numerator(),X,i,j,k);
	     }
	     for(k=1; k<R.Denominator().Size(); k++)
	     {
		      y -= GetBackwardValue(R.Denominator(),Y,i,j,k);
	     }
	     Y(i,j) = y;
	   }
  }
}

/*
 *  G( B ) X = H( B ) Y
 *  R = G / H
 *  a partir de los valores conocidos de la variable independiente X
 *  de donde queremos despejar la inc�gnita Y a partir de los valores
 *  conocidos de la variable independiente X y de g los valores iniciales
 *  conocidos de X a los que llamamos X0 y los h valores iniciales
 *  conocidos de Y a los que llamamos Y0. Siendo g el grado de G( B ) y
 *  h el de H( B ). Obs�rvese que los datos devueltos en Y ser�n tantos
 *  como los contenidos en X y referidos al mismo �ndice posicional. Si
 *  se trata de series temporales, los valores Y(t) y X(t) ser�n
 *  contempor�neos, mientras que las matrices X0 e Y0 son contempor�neas
 *  en orden inverso. La relaci�n de contemporaneidad se establece entre
 *  X0( h-k ) ~ Y0( g-k ) con k=1..min(h,g), X( t ) ~ Y( t ), t = 1..N
 */

//--------------------------------------------------------------------
void MatBackwardDifEq(const BRational <BDat>& R,
		                  const BMatrix	<BDat>& X0,
		                  const BMatrix	<BDat>& X,
		                  const BMatrix	<BDat>& Y0,
		                        BMatrix	<BDat>& Y)
//--------------------------------------------------------------------
{
  BInt i,j,k;
  BInt r  = X.Rows();
  BInt c  = X.Columns();
  Y.Alloc(r,c);
  for(j=0; j<c; j++)
  {
	   for(i=0; i<r; i++)
	   {
	     BDat y = 0;
	     for(k=0; k<R.Numerator().Size(); k++)
	     {
		      y += GetBackwardValue(R.Numerator(),X0,X,i,j,k);
	     }
	     for(k=1; k<R.Denominator().Size(); k++)
	     {
		      y -= GetBackwardValue(R.Denominator(),Y0,Y,i,j,k);
	     }
	     Y(i,j) = y;
	   }
  }
}


//--------------------------------------------------------------------
void MatForwardDifEq(const BRational <BDat>& R,
		                 const BMatrix   <BDat>& X,
		                       BMatrix   <BDat>& Y)
//--------------------------------------------------------------------
{
  BInt i,j,k;
  BInt r  = X.Rows();
  BInt c  = X.Columns();
  Y.Alloc(r,c);
  for(j=0; j<c; j++)
  {
    for(i=r-1; i>=0; i--)
    {
      BDat y = 0;
      for(k=0; k<R.Numerator().Size(); k++)
      {
        y += GetForwardValue(R.Numerator(),X,i,j,k);
      }
      for(k=1; k<R.Denominator().Size(); k++)
      {
        y -= GetForwardValue(R.Denominator(),Y,i,j,k);
      }
      Y(i,j) = y;
    }
  }
}


//--------------------------------------------------------------------
void MatForwardDifEq(const BRational <BDat>& R,
              		   const BMatrix   <BDat>& X0,
		                 const BMatrix   <BDat>& X,
		                 const BMatrix   <BDat>& Y0,
		                       BMatrix   <BDat>& Y)
//--------------------------------------------------------------------
{
  BInt i,j,k;
  BInt r  = X.Rows();
  BInt c  = X.Columns();
  Y.Alloc(r,c);
  for(j=0; j<c; j++)
  {
   	for(i=r-1; i>=0; i--)
	   {
	     BDat y = 0;
	     for(k=0; k<R.Numerator().Size(); k++)
	     {
		      y += GetForwardValue(R.Numerator(),X0,X,i,j,k);
	     }
	     for(k=1; k<R.Denominator().Size(); k++)
	     {
		      y -= GetForwardValue(R.Denominator(),Y0,Y,i,j,k);
	     }
	     Y(i,j) = y;
	   }
  }
}


//--------------------------------------------------------------------
void ForecastingToNull(const BRational <BDat>& R,
		                   const BMatrix	  <BDat>& Z,
		                         BMatrix	  <BDat>& A,
		                         BMatrix	  <BDat>& ZF)
//--------------------------------------------------------------------
{
//BTimer tm("ForecastingToNull");
  BPolyn<BDat>	  ar = R.Numerator();
  BPolyn<BDat>	  ma = R.Denominator();
  BRational<BDat> Q  = ma/ar;
  BInt p = ar.Degree();
  BInt q = ma.Degree();
  BInt i, N = Z.Rows(), m = N-1, k = m /*, iter = 0*/;
  BMatrix<BDat> ZERON (N,1); for(i=0; i<N; i++) { ZERON(i,0)=0; }
  BMatrix<BDat> ZFN;
    
  MatBackwardDifEq(R, ZERON,	 Z, ZERON,   A);
  MatBackwardDifEq(Q,	  A, ZERON,	Z, ZFN);
  for(k=m;(k>p+q)&&(Abs(ZFN(k,0))<=0.0001);k--) { }
  ZF.Alloc(k,1);
  //  for(i=0; i<m; i++) { ZF(i,0)=ZFN(i,0); }
  // parece tener mas logica
  // recuerda revisar la teoria de back - forcasting!
  for(i=0; i<k; i++) { ZF(i,0)=ZFN(i,0); }
}

//--------------------------------------------------------------------
void ARMABackasting(const BRational <BDat>& R,
		                      BMatrix   <BDat>& A0,
		                      BMatrix   <BDat>& A,
		                      BMatrix   <BDat>& Z0,
		                const BMatrix   <BDat>& Z)
//--------------------------------------------------------------------
{
  BMatrix<BDat> A0A;
  ARMABackastingHist(R, A0A, A0, A, Z0, Z,3);
}

//--------------------------------------------------------------------
void ARMABackastingHist(const BRational <BDat>& R,
	                   		      BMatrix	  <BDat>& A0A,
			                        BMatrix	  <BDat>& A0,
			                        BMatrix	  <BDat>& A,
			                        BMatrix	  <BDat>& Z0,
			                  const BMatrix	  <BDat>& Z,
			                        BInt		  refination)
//--------------------------------------------------------------------
{
//BTimer tm("ARMABackasting");
  BPolyn<BDat>	 ar = R.Numerator();
  BPolyn<BDat>	 ma = R.Denominator();
  ar.Aggregate();
  ma.Aggregate();
  BRational<BDat>  Q  = ma/ar;
  BInt p = ar.Degree();
  BInt q = ma.Degree();
  BInt N = Z.Rows();
  BInt i;

  BMatrix<BDat> E, ZF, ZB;
  ForecastingToNull(R,Z,A,ZF);

  BInt m = ZF.Rows();
  BMatrix<BDat> ZEROp (p,1); for(i=0; i<p; i++) { ZEROp(i,0)=0; }
  BMatrix<BDat> ZEROq (q,1); for(i=0; i<q; i++) { ZEROq(i,0)=0; }
  BMatrix<BDat> ZEROm (m,1); for(i=0; i<m; i++) { ZEROm(i,0)=0; }
  A0A = ZEROm << A;

  MatForwardDifEq  (R, ZEROp, Z<<ZF, ZEROq, E);
  MatForwardDifEq  (Q, E,     ZEROm, Z<<ZF, ZB);
  MatBackwardDifEq (R, ZEROp, ZB<<Z, ZEROq, A0A);

  if(IsStationary(ar) && IsStationary(ma))
  {
    BMatrix<BDat> oldA0A;
    BDat maxDif = 0;
    BDat oldMaxDif = 0;
    for(BInt i=1; (i<=refination)&&(maxDif>0.001)&&(maxDif<=oldMaxDif);	 i++)
    {
      oldA0A = A0A;
      oldMaxDif = maxDif;
      MatBackwardDifEq (Q, A0A,	  ZEROm, ZB<<Z, ZF);
      MatForwardDifEq  (R, ZEROp, Z<<ZF, ZEROq, E);
      MatForwardDifEq  (Q, E,	  ZEROm, Z<<ZF, ZB);
      MatBackwardDifEq (R, ZEROp, ZB<<Z, ZEROq, A0A);

      BMatrix<BDat> dif = A0A-oldA0A;
      maxDif = 0;
      for(i=0; i<A0A.Rows(); i++)
      {
         //Std(BText("\n") + dif[i]);
	       BDat absDif = Abs(dif(i,0));
	       if(maxDif<absDif) { maxDif = absDif; }
      }

    //Std(BText("\noldStDs = ")+Sqrt(Moment(oldA0A.T().Data()[0],2)));
    //Std(BText("\nStDs	   = ")+Sqrt(Moment(   A0A.T().Data()[0],2)));
    //Std(BText("\nmaxDif = ") + maxDif);

    }
  }

  Z0.Alloc(p,1); for(i=0; i<p; i++) { Z0(i,0) = ZB (m-p+i,0); }
  A0.Alloc(q,1); for(i=0; i<q; i++) { A0(i,0) = A0A(m-q+i,0); }
  A .Alloc(N,1); for(i=0; i<N; i++) { A (i,0) = A0A(m  +i,0); }
}


//--------------------------------------------------------------------
BMatrix<BDat> Pol2Mat(BInt r, BInt c, const BPolyn<BDat> pol)
//--------------------------------------------------------------------

{
  BDat x;
  BInt d, i, j, k;
  BInt	m   = pol.Size();
  BMatrix<BDat> P(r,c);
  for(j=0; j<c; j++)
  {
    for(i=0; i<r; i++) { P(i,j) = 0; }
  }
  for(k=0; k<m; k++)
  {
    x = pol[k].Coef();
    d = pol[k].Degree();
    i = j = 0;
    if(d>=0) { i= d; }
    else     { j=-d; }
    for(;(i<r)&&(j<c); i++,j++)
    {
      P(i,j) = x;
    }
  }
  return(P);
}

//--------------------------------------------------------------------
BPolyn<BDat> Reverse(const BPolyn<BDat>& a)
//--------------------------------------------------------------------
{
  BInt p = a.Degree();
  BPolyn<BDat> Bp = (BPolyn<BDat>::B())^p;
  BPolyn<BDat> b;
  b.ChangeBF(a);
  b*=Bp;
  return(b);
}


//--------------------------------------------------------------------
BMat PolynSecTriangInverse(const BPolyn<BDat> a)
//--------------------------------------------------------------------
{
  BInt i,j,k,d,p = a.Degree();
  BDat x;
  BPolyn<BDat> b = Reverse((BPolyn<BDat>::One()/Reverse(a))%p);
//TRZ(a);  TRZ(b);
  BMat X(p+1,p+1);
  X.SetAllValuesTo(0);
  for(k=0; k<b.Size(); k++)
  {
    d = b(k).Degree();
	   x = b(k).Coef  ();
	   for(i=0; i<=d; i++)
	   {
	     j = d-i;
	     X(p-i,p-j) = x;
	   }
  }
  return(X);
}

//--------------------------------------------------------------------
BInt MaxComDiv(BInt p, BInt q)
//--------------------------------------------------------------------
{
  if(p==0) { return(q); }
  if(q==0) { return(p); }
  if(p==q) { return(p); }
  if((p==1)||(q==1)) { return(1); }
  if(p< q) { return(MaxComDiv(q,p)); }
  BInt c = p/q;
  BInt r = p-q*c;
  if(r==0) { return(c); }
  return(MaxComDiv(q,c));
}



//--------------------------------------------------------------------
void BARIMAFactor::SetOne()
//--------------------------------------------------------------------
{
  s_   = 1;
  dif_ = ar_ = ma_ = BPolyn<BDat>::One();
}

//--------------------------------------------------------------------
BARIMAFactor& BARIMAFactor::operator =(const BARIMAFactor& a)
//--------------------------------------------------------------------
{
  s_   = a.s_;
  dif_ = a.dif_;
  ar_  = a.ar_;
  ma_  = a.ma_;
  return(*this);
}

//--------------------------------------------------------------------
BARIMAFactor& BARIMAFactor::operator*=(const BARIMAFactor& a)
//--------------------------------------------------------------------
{
  s_	= MaxComDiv(s_, a.s_);
  dif_ *= a.dif_;
  ar_  *= a.ar_;
  ma_  *= a.ma_;
  return(*this);
}
//--------------------------------------------------------------------
BBool BARIMAFactor::HasValue()
//--------------------------------------------------------------------
{
  return((dif_.Size()>1)||(ar_.Size()>1)||(ma_.Size()>1));
}


//--------------------------------------------------------------------
BText BARIMAFactor::Name()
//--------------------------------------------------------------------
{
  return(BText("(")+s_+", "+dif_.Name()+", "+ar_.Name()+", "+ma_.Name()+")");
}

//--------------------------------------------------------------------
BInt ArrayARIMAFactorCmp(const void* v1, const void* v2)
//--------------------------------------------------------------------
{
  BARIMAFactor& id1 = *((BARIMAFactor*)v1);
  BARIMAFactor& id2 = *((BARIMAFactor*)v2);
  int dif = id1.s_ - id2.s_;
  if(dif<0) { return(-1); }
  if(dif>0) { return( 1); }
  return(0);
}


//--------------------------------------------------------------------
  bool ARMAAutoCovarianzeVector(BArray<BDat>& gn,
                                 const BPolyn<BDat>& ar,
                                 const BPolyn<BDat>& ma,
                                 BInt n0, BDat sigma)
//--------------------------------------------------------------------
{
  if(!n0) { return(false); }
  bool ok = true;
  BInt i,j,k,d,s;
  BInt p = ar.Degree();
  BInt q = ma.Degree();
  BInt n = n0;
//BTimer tm(BText("ARMAAutoCovarianze(p=")+p+",n="+n);
//TRZ(ar);TRZ(ma); 
  if(n<=p) { n=p+1; }
  if(n<=q) { n=q+1; }
  gn.Replicate(0,n);

  if(!p)
  {
    BPolyn<BDat> psiB, psiF, psiBF;
    psiB = (ma/ar).Expand(n,BTRUE);
    psiF.ChangeBF(psiB);
    psiBF = psiB * psiF;
  //TRZ(psiB); TRZ(psiF); TRZ(psiBF);
    s = psiBF.Size();
  //if(n<psiBF(s-1).Degree()) { n=psiBF(s-1).Degree(); }
    gn.Replicate(0,n);
    for(i=0; i<s; i++) 
    { 
      j = psiBF(i).Degree();
      if((j>=0) && (j<n))
      {
        gn(j) = psiBF(i).Coef();
      }
    //Std(BText("\nk=")+k+"\ti="+i+"\tj="+j+"\tgn(j)="+gn(j));
    }
  }
  else
  {
    BPolyn<BDat> psi;
    { //BTimer tm(BText("Expand((")<<ma.Name()<<")/("<<ar.Name()<<"),"<<n<<","<<BFALSE<<")");
      psi = (ma/ar).Expand(n,BFALSE); }
    if(psi.Size()!=n+1)
    {  
      Error(I2("Cannot build psi-weights: ARMA structure could be not stationary. ",
        "No se pudieron construir los psi-weights: La estructura ARMA podr�a no ser estacionaria."));
      return(false);
    }
    BMatrix<BDat> m(p+1,1);
    BDat x,v = sigma^2;
    { //BTimer tm(BText("Calculating m-weights"));
    for(k=0; k<=p; k++)
    {
      m(k,0) = 0;
      for(j=0; j<ma.Size(); j++) 
      { 
        i = ma(j).Degree();
        if(i>=k)
        {
          m(k,0) += ma(j).Coef()*psi(i-k).Coef(); 
        }
      }
      m(k,0) *= v;
    } }
    BMatrix<BDat> A(p+1,p+1);
    A.SetAllValuesTo(0);

    for(k=0; k<ar.Size(); k++)
    {
      d = ar(k).Degree();
      x = ar(k).Coef();
      for(i=0; i<=d; i++) { A(i,d-i) += x; }
      if(d<p) { for(i=d+1; i<=p; i++) { A(i,i-d) += x; } }
    }
    BMat gp, test;
    BDat err;
    
    BVMat ms, As, Ls, gps;
    As.DMat2sparse((const BMatrix<double>&)A);
    ms.DMat2sparse((const BMatrix<double>&)m);
    BVMat::CholeskiFactor(As,Ls,"XtX",true,false,false);
    if(Ls.Code()==BVMat::ESC_undefined)
    {
      err  = BDat::Unknown();
    }
    else
    {
      BVMat::CholeskiSolve(Ls,(ms.T()*As).T(),gps,"PtLLtP");
      gps.GetDMat((BMatrix<double>&)gp);
      test = A*gp-m;
      err  = test.FrobeniusNorm();
    }
    // unused
    // bool closeToNonStationary = false;
    if(err.IsUnknown()  || (err>Sqrt((1+p)*DEpsilon()))) 
    {
      // unused
      //closeToNonStationary = true;
      BText method = BText("Golub_Reinsch_Mod");
      BMat U, V;
      BDiagMatrix<BDat> D, Dp; 
      D.Alloc(p+1);
      U.Alloc(p+1,p+1);
      V.Alloc(p+1,p+1);
      gsl_SingularValueDecomposition(A,U,D,V, method);
      Dp = D.P(Sqrt(DEpsilon()));
      gp = V*Dp*((m.T()*U).T());
      test = A*gp-m;
      err  = test.FrobeniusNorm();
    }
    if(err.IsUnknown())
    {  
      Error(I2("ARMA structure is not stationary. ",
               "La estructura ARMA no es estacionaria."));
      return(false);
    } /*
    else if(closeToNonStationary)
    { 
      Warning(I2("ARMA structure is not numerically stationary. "
                 "Lost precision for autocovarianze matrix: ",
                 "La estructura ARMA no es num�ricamente estacionaria."
                 "P�rdida de precisi�n en la matriz de autocovarianzas: ")<<
                 ""<<err.Format("%.15lg")+"\n"
                 "  ar="+ar.Name()+"\n"
                 "  ma="+ma.Name());
    } */
    for(k=0; (k<=p)&&(k<n0); k++) 
    { 
      gn(k) = gp(k,0); 
    //Std(BText("\n 1- ACov[")<<k<<"]="+gn(k));
    }
    for(; k<n0; k++)
    {
      BDat mk = 0;
      for(i=ma.Size()-1; i>=0; i--) 
      { 
        int j = ma(i).Degree();
        if(j<k) { break; }
        mk += ma(i).Coef()*psi(j-k).Coef(); 
      }
      mk *= v;
      gn(k) = mk;
      for(i=1; i<ar.Size(); i++)
      {
        int j = ar(i).Degree();
        gn(k) -= ar(i).Coef()*gn(k-j);
      }
    //Std(BText("\n 2- ACov[")<<k<<"]="+gn(k)+"\tmk="+mk);
    }
  }
  gn.ReallocBuffer(n0);
  return(ok);
}

////////////////////////////////////////////////////////////////////////////
 BPolyn<BDat> ARMAACovGamma(const BPolyn<BDat>& ar,
                            const BPolyn<BDat>& ma,
                            int n)
////////////////////////////////////////////////////////////////////////////
{
  BArray<BDat> gn;
  ARMAAutoCovarianzeVector(gn, ar, ma, n, 1);
  assert(gn.Size()==n);
  BPolyn<BDat> gamma;
  if(!n) 
  { return(gamma); }
  gamma.AllocBuffer(2*n-1);
  register int n1 = n-1;
  register int i=0;
  register BMonome<BDat>* c = gamma.GetBuffer();
  register BMonome<BDat>* a = c+n1;
  register BMonome<BDat>* b = a;
  register const   BDat * g = gn.Buffer();
#ifndef NDEBUG
  const BMonome<BDat>*  b_end = c+(2*n-1);
  const BDat*           g_end = g+n;
#endif
  a->PutDegree(0);
  a->PutCoef(*g);
  while(i<n1)
  {
    i++; a--; b++; g++;
    assert(a>=c);
    assert(b<b_end);
    assert(g<g_end);
    a->PutDegree(-i);
    b->PutDegree( i);
    a->PutCoef(*g);
    b->PutCoef(*g);
  }
  gamma.Aggregate();
  return(gamma);
};

//--------------------------------------------------------------------
BSymMatrix<BDat> ARMAAutoCovarianze(const BPolyn<BDat> ar,
	                            			const BPolyn<BDat> ma,
				                            BInt n0, BDat sigma)
//--------------------------------------------------------------------
{
  int i, j;
  BArray<BDat> gn;
//BBool isStationary = IsStationary(ar) && IsStationary(ma);
  ARMAAutoCovarianzeVector(gn, ar, ma, n0, sigma);
  BSymMatrix<BDat> G(n0);
  for(i=0; i<n0; i++)
  {
    for(j=0; j<=i; j++)
    {
      G(i,j) = gn(i-j);
    }
  }
  return(G);
}


//////////////////////////////////////////////////////////////////////////////
  BBool YuleWalkerDurbin(const BArray<BDat>& r,
                               BArray<BDat>& y,
                               BArray<BDat>& PACF,
                                      BDat & logDet,
                               int period)

//PURPOSE: Solves the Yule-Walker equations
//
//         T*y = -r
//
//       where T is the symmetric toeplitz matrix generated by the column
//       vector r and r(0) = 1.
//       This function uses the Durbin algorithm.
//       (Matrix Computation G.H. Golub & C.F. Van Loan, p�gina 194) 
//
//////////////////////////////////////////////////////////////////////////////
{
  BInt i,k,n=r.Size()-1;
  if(n<=0) { return(BFALSE); }
//BTimer tm(BText("Yule-Walker eq. Durbin Algorithm(p=")+period+",n="+n);
  BDat lb = 0, a = -r(1);
  BArray<BDat> z(n+1);
  y.AllocBuffer(n+1);
  PACF.AllocBuffer(n+1); 
  for(i=0; i<=n; i++) { y(i) = PACF(i) = 0; }
  y(0) = 1;
  y(1) = a;
  PACF(0) =  1;
  PACF(1) = -a;
  logDet  = 0;

  int nullCycle = Maximum(10,period);
  int nullCount = 0;
//k = 0;
//Std(BText("\nYuleWalkerDurbin r[k=")+(k+1)+"]="+r(k+1)+"\tb="+b+"\ta="+a);

  for(k=1; k<n; k++)
  {
    if((1.0-a<=0)||(1.0+a<=0))
    { 
      Warning(I2("ARMA structure is not numerically inversible.",
                 "La estructura ARMA no es num�ricamente invertible."));
      return(BFALSE); 
    }
    lb += Log(1.0-a)+Log(1.0+a);
    BDat b = Exp(lb);
    logDet += lb;
    if(k==n-1) { break; }
    if(Abs(a)>0*k*DEpsilon()) { nullCount = 0; }
    else                      { nullCount++;   }
    if(nullCount<=nullCycle)
    {
      a = -r(k+1);
      for(i=1; i<=k; i++) { a -= r(i)*y(k-i+1); }
      a/=b;
      for(i=1; i<=k; i++) { z(i) = y(i) + a*y(k-i+1); }
    //Se puede acelerar la copia parcial de vectores
    //for(i=1; i<=k; i++) { y(i) = z(i); }
    //mediante la siguiente operaci�n de memoria en bloque
      memcpy(y.GetBuffer(),z.Buffer(),sizeof(double)*(k+1));
    //Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
    //          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);
    }
    y(k+1) = a;
    PACF(k+1) = - a;
  }

//Std(BText("\nYuleWalkerDurbin r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
//          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);

//Trace(BText("\nsign/n=")+sign);
//Trace(BText("\nlogDet/n=")+(logDet/double(n)));
  return(BTRUE);
}

//////////////////////////////////////////////////////////////////////////////
  int Levinson(const BArray<double>& r,
               const BArray<double>& c,
                     BArray<double>& x,
                     BArray<double>& y,
                     BArray<double>& PACF,
                     double        & logDet,
                     int             period)

//PURPOSE: Solves the general symmetric Toeplitz system
//
//         T*x = -c
//
//         and the Yule-Walker equations
//
//         T*y = -r
//
//       where T is the symmetric toeplitz matrix generated by the column
//       vector r and r(0) = 1.
//       This function uses the Levinson algorithm.
//       (Matrix Computation G.H. Golub & C.F. Van Loan, p�gina 196) 
//
//////////////////////////////////////////////////////////////////////////////
{
  BInt i,k=0,n=r.Size()-1;
  if(n<=0) { return(BFALSE); }
//BTimer tm(BText("Levinson Algorithm(p=")+period+",n="+n);
  BArray<double> z(n+1);
  double lb = 0, a = -r(1), nu;
  x.AllocBuffer(n+1);
  y.AllocBuffer(n+1);
  PACF.AllocBuffer(n+1); 
  for(i=0; i<=n; i++) { y(i) = PACF(i) = 0; }
  y   (0) = 1;
  y   (1) = a;
  PACF(0) = 1;
  PACF(1) = -a;
  x   (1) = c(1);
  logDet  = 0;

  int nullCycle = Maximum(10,period);
  int nullCount = 0;
//Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a);
  int ix=1;
  BInt rx_nn=0, a_nn=0;
  for(k=1; k<n; k++)
  {
    if((1.0-a<=0)||(1.0+a<=0))
    { 
      Warning(I2("ARMA structure is not numerically inversible.",
                 "La estructura ARMA no es num�ricamente invertible."));
      return(BFALSE); 
    }
    lb += log(1.0-a)+log(1.0+a);
    double b = exp(lb);
    logDet += lb;
    double rx = 0;
    // unused
    // BInt ir_f=0, ir_u=k;
    
    for(i=1; i<=k; i++,rx_nn++) 
    { 
      rx += r(i)*x(k-i+1); 
    }
      
    //Std(BText(" ")<<(r(i)*x(k-i+1))); 
    nu = c(k+1) - rx;
    
    nu/=b;
    if(fabs(nu)>k*DEpsilon())
    {
      for(i=ix; i<=k; i++) 
      { 
        x(i) += nu*y(k-i+1); 
      }
    }
  //Std(BText("\nk=")<<k<<" ix="<<ix<<" rx="<<rx<<" rx_0="<<rx_0<<" rx_f="<<rx_f<<" rx_u="<<rx_u);
    x(k+1) = nu;
    if(k==n-1) { break; }
    if(fabs(a)>0*k*DEpsilon()) { nullCount = 0; }
    else                       { nullCount++;   a_nn++; }
    if(nullCount<=nullCycle)
    {
      a = -r(k+1);
      for(i=1; i<=k; i++) { a -= r(i)*y(k-i+1); }
      a/=b;
      for(i=1; i<=k; i++) { z(i) = y(i) + a*y(k-i+1); }
    //Se puede acelerar la copia parcial de vectores
    //for(i=1; i<=k; i++) { y(i) = z(i); }
    //mediante la siguiente operaci�n de memoria en bloque
      memcpy(y.GetBuffer(),z.Buffer(),sizeof(double)*(k+1));
    //Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
    //          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);
    }
    y   (k+1) =   a;
    PACF(k+1) = - a;
  }

//Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
//          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);

//Std(BText("\nlogDet/n=")+(logDet/double(n)));
  return(k+1);
}

//////////////////////////////////////////////////////////////////////////////
  int Levinson(const BArray<BDat>& r,
               const BArray<BDat>& c,
                     BArray<BDat>& x,
                     BArray<BDat>& y,
                     BArray<BDat>& PACF,
                     BDat        & logDet,
                     int           period)

//PURPOSE: Solves the general symmetric Toeplitz system
//
//         T*x = -c
//
//         and the Yule-Walker equations
//
//         T*y = -r
//
//       where T is the symmetric toeplitz matrix generated by the column
//       vector r and r(0) = 1.
//       This function uses the Levinson algorithm.
//       (Matrix Computation G.H. Golub & C.F. Van Loan, p�gina 196) 
//
//////////////////////////////////////////////////////////////////////////////
{
//Std(BText("\nr=")+Name(r)+"\nc="+Name(c));
  BArray<double> r_, c_, x_, y_, PACF_;
  double logDet_;
  BArrBDat2Double(r,r_);
  BArrBDat2Double(c,c_);
  int result = Levinson(r_, c_, x_, y_, PACF_, logDet_, period);
  logDet = logDet_;
  BArrDouble2BDat(x_,x);
  BArrDouble2BDat(y_,y);
  BArrDouble2BDat(PACF_,PACF);
  return(result);
}


//////////////////////////////////////////////////////////////////////////////
  int LevinsonARMA(const BArray<double>& r,
                   const BArray<double>& c,
                         BArray<double>& x,
                         BArray<double>& y,
                         BArray<double>& PACF,
                         double        & logDet,
                         int           period,
                   const BPolyn<BDat>& ar,
                   const BPolyn<BDat>& ma)

//PURPOSE: Solves the general symmetric Toeplitz system
//
//         T*x = -c
//
//         and the Yule-Walker equations
//
//         T*y = -r
//
//       where T is the symmetric toeplitz matrix generated by the column
//       vector r and r(0) = 1.
//       This function uses the Levinson algorithm.
//       (Matrix Computation G.H. Golub & C.F. Van Loan, p�gina 196) 
//
//////////////////////////////////////////////////////////////////////////////
{
  int n=r.Size()-1;
  if(n<=0) { return(BFALSE); }
  int p = ar.Degree();
  int q = ma.Degree();
//BTimer tm(BText("Levinson Sparse Algorithm(p=")<<period<<",n="<<n<<")");
  int i,j,k=0, nullCycle=Maximum(10,period), nullCount=0, a_nn=0, ix=1;
  BArray<double> z(n+1);
  double b = 1, a = -r(1), nu, zero=DEpsilon();
  BArray<BInt> rNotNull(n+1);
//BArray<BInt> yNotNull(n+1); int j_nny=0;

  x.AllocBuffer(n+1);
  y.AllocBuffer(n+1);
  PACF.AllocBuffer(n+1); 
  for(i=0; i<=n; i++) { y(i) = PACF(i) = 0; }
  y   (0) = 1;
  y   (1) = a;
  PACF(0) = 1;
  PACF(1) = -a;
  x   (1) = c(1);
  logDet  = 0;

//Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a);
  j=0;
  rNotNull(0)=0;
  for(k=1; k<n; k++) 
  { 
    if(fabs(r(k))>zero) 
    { 
      rNotNull(j++) = k;
    } 
  }
  rNotNull.ReallocBuffer(j);
//Std(BText("\nrNotNull.Size()=")<<rNotNull.Size());
//yNotNull(0)=0; yNotNull(1)=1;  j_nny = 2;
  for(k=1; k<n; k++)
  {
    b = (1.0-a*a)*b;
  //Std(BText("k=")+k+", b="+b+"\n");
    if(b<0) 
    { 
      Warning(I2("ARMA structure is not numerically inversible.",
                 "La estructura ARMA no es num�ricamente invertible."));
      break; 
    }
    logDet += log(b);
    double rx = 0;

    for(j=0; (j<rNotNull.Size()) && ((i=rNotNull(j))<=k); j++) 
    {
      rx += r(i)*x(k-i+1); 
    }
      
    //Std(BText(" ")<<(r(i)*x(k-i+1))); 
    nu = c(k+1) - rx;
    nu/=b;
    if(fabs(nu)>zero)
    {
    //Std(BText("\nx="));
/* */
      if(0*k>p+q)
      {
        for(i=ix; i<=k; i++) 
        { 
          double dx  = nu*y(k-i+1);
          x(i) += dx; 
          if((dx<=zero) && (dx>=-zero)) { ix++; }
        }
      }
      else
      {
        for(i=1; i<=k; i++) 
        { 
          x(i) += nu*y(k-i+1); 
        }
      }
/*
      for(j=0; (j<j_nny) && ((i=yNotNull(j))<=k); j++) 
      { 
        x(k-i+1) += nu*y(i); 
      }
*/
    }
    x(k+1) = nu;
    if(k==n-1) { break; }
    if(fabs(a)>zero) { nullCount = 0; }
    else             { nullCount++;   a_nn++; }
/* */
    if(nullCount<=nullCycle)
    {

        a = -r(k+1);
        for(i=1; i<=k; i++) { a -= r(i)*y(k-i+1); }
      //for(j=0; (j<rNotNull.Size()) && ((i=rNotNull(j))<=k); j++) { a -= r(i)*y(k-i+1); }
        a/=b;
        if(k>1+p+q)
        {
          double a_ = 0;
          for(j=1; j<ma.Size(); j++)
          {
            a_ += ma(j).Coef().Value()*PACF(k+1-ma(j).Degree());
          }
        //Std(BText("\n")<<k<<"\t"<<a<<"\t"<<a_);
        }

/*
      //En un proceso ARMA las MA(B):PACF = 0 a partir del orden 1+p+q
      //lo cual permite acelerar los c�lculos a partir de dicho orden
      if(k<=1+p+q) //c�lculo de Levinson
      {
        a = -r(k+1);
        for(i=1; i<=k; i++) { a -= r(i)*y(k-i+1); }
      //for(j=0; (j<rNotNull.Size()) && ((i=rNotNull(j))<=k); j++) { a -= r(i)*y(k-i+1); }
        a/=b;
      }
      else //C�lculo ARMA
      {
        a = 0;
        for(j=1; j<ma.Size(); j++)
        {
          a += ma(j).Coef().Value()*PACF(k+1-ma(j).Degree());
        }
      }
*/
      for(i=1; i<=k; i++) { z(i) = y(i) + a*y(k-i+1); }
    //Se puede acelerar la copia parcial de vectores
    //for(i=1; i<=k; i++) { y(i) = z(i); }
    //mediante la siguiente operaci�n de memoria en bloque
      memcpy(y.GetBuffer(),z.Buffer(),sizeof(double)*(k+1));

    //Std(BText("\nLevinson ARMA r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
    //          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);
    }
    y   (k+1) =   a;
    PACF(k+1) = - a;
  }

//Std(BText("\n  k=")<<k<<" nex="<<(k-ix)<<" nna="<<(k-a_nn));

//Std(BText("\nLevinson r[k=")+(k)+"]="+r(k)+"\tb="+b+"\ta="+a+
//          "\tlogDet/k="+(logDet/k)+"\tnullCount="+nullCount);

//Std(BText("\nlogDet/n=")+(logDet/double(n)));
  return(k+1);
}

//////////////////////////////////////////////////////////////////////////////
  int LevinsonARMA(const BArray<BDat>& r,
                   const BArray<BDat>& c,
                         BArray<BDat>& x,
                         BArray<BDat>& y,
                         BArray<BDat>& PACF,
                         BDat        & logDet,
                         int           period,
                   const BPolyn<BDat>& ar,
                   const BPolyn<BDat>& ma)

//PURPOSE: Solves the general symmetric Toeplitz system
//
//         T*x = -c
//
//         and the Yule-Walker equations
//
//         T*y = -r
//
//       where T is the symmetric toeplitz matrix generated by the column
//       vector r and r(0) = 1.
//       This function uses the Levinson algorithm.
//       (Matrix Computation G.H. Golub & C.F. Van Loan, p�gina 196) 
//
//////////////////////////////////////////////////////////////////////////////
{
  BArray<double> r_, c_, x_, y_, PACF_;
  double logDet_;
  BArrBDat2Double(r,r_);
  BArrBDat2Double(c,c_);
  int result = LevinsonARMA(r_, c_, x_, y_, PACF_, logDet_, period, ar, ma);
  logDet = logDet_;
  BArrDouble2BDat(x_,x);
  BArrDouble2BDat(y_,y);
  BArrDouble2BDat(PACF_,PACF);
  return(result);
}


//--------------------------------------------------------------------
void BARIMA::PutFactors(const BArray<BARIMAFactor>& factor)
//--------------------------------------------------------------------
{
  BInt i,j;
  if(&factor_ == &factor) { return; }
  if(!factor.Size())
  {
    factor_.AllocBuffer(0);
    grFact_   = BArray<BARIMAFactor>(0);
    acumFact_ = BArray<BARIMAFactor>(0);
    coef_.AllocBuffer(0);
    d_ = 0;
    p_ = 0;
    q_ = 0;
    return;
  }
  factor_.AllocBuffer(factor.Size());
  for(i=j=n_=0;i<factor.Size();i++)
  {
    if(factor(i).HasValue())
    {
      factor_(j++) = factor(i);
      n_+=(factor(i).ar_.Size()-1)+(factor(i).ma_.Size()-1);
    }
  }
  if(!j)
  {
    factor_(j++) = factor(0);
  }
  factor_.ReallocBuffer(j);
  factor_.Sort(ArrayARIMAFactorCmp);
  coef_.ReallocBuffer(n_);
  FactorUpdated();
}

//--------------------------------------------------------------------
void BARIMA::FactorUpdated()
//--------------------------------------------------------------------
{
  BInt i,j;
  GetCoef();
  grFact_   = BArray<BARIMAFactor>(factor_.Size());
  acumFact_ = BArray<BARIMAFactor>(factor_.Size());
//prod_.SetOne();
  prod_ = grFact_(0) = factor_(0);
//Std(BText("\nprod_=")+prod_.Name());
  j=0;
//Std(BText("\nfactor_(")+0+")="+factor_(0).Name());
  for(i=1;i<factor_.Size();i++)
  {
    if(factor_(i).s_==grFact_(j).s_)
    {
      grFact_(j) *= factor_(i);
    //Std(BText("\nfactor_(")+i+")="+factor_(i).Name());
    //Std(BText("\ngrFact_(")+j+")="+grFact_(j).Name());
    }
    else
    {
      acumFact_(j) = prod_;
      j++;
      grFact_(j) = factor_(i);
    //Std(BText("\nfactor_(")+i+")="+factor_(i).Name());
    //Std(BText("\nacumFact_(")+j+")="+acumFact_(j).Name());
    }
    prod_ *= factor_(i);
//  Std(BText("\nprod_=")+prod_.Name());
  }
  acumFact_(j) = prod_;
  grFact_.ReallocBuffer(j+1);
  acumFact_.ReallocBuffer(j+1);
  d_ = prod_.dif_.Degree();
  p_ = prod_.ar_ .Degree();
  q_ = prod_.ma_ .Degree();
}

//--------------------------------------------------------------------
const BArray<BDat>& BARIMA::GetCoef() const
//--------------------------------------------------------------------
{
  int i,j,k,r;
  for(r=k=0;k<factor_.Size();k++)
  {
    BPolyn<BDat>& ar = factor_(k).ar_;
    BPolyn<BDat>& ma = factor_(k).ma_;
    for(i=1;i<ar.Size();i++)
    {
      coef_[r++] = -ar[i].Coef();
    }
    for(j=1;j<ma.Size();j++)
    {
      coef_[r++] = -ma[j].Coef();
    }
  }
  return(coef_);
}

//--------------------------------------------------------------------
void BARIMA::PutCoef(const BArray<BDat>& coef)
//--------------------------------------------------------------------
{
  int i,j,k,r;
  if(&coef_ != &coef) { coef_ = coef; }
  for(r=k=0;k<factor_.Size();k++)
  {
    BPolyn<BDat>& ar = factor_(k).ar_;
    BPolyn<BDat>& ma = factor_(k).ma_;
    for(i=1;i<ar.Size();i++)
    {
      ar[i].PutCoef(-coef[r++]);
    }
    for(j=1;j<ma.Size();j++)
    {
      ma[j].PutCoef(-coef[r++]);
    }
  }
  FactorUpdated();
}

//--------------------------------------------------------------------
void BARIMA::PutOutputData(const BMatrix<BDat>& z)
//--------------------------------------------------------------------
{
  z_ = z;
  OutputDataUpdated();
}

//--------------------------------------------------------------------
void BARIMA::OutputDataUpdated()
//--------------------------------------------------------------------
{
//TRZ(z_);
  BMat z0 = z_.Sub(0,0,d_,z_.Columns());
  BMat z1 = z_.Sub(d_,0,z_.Rows()-d_,z_.Columns());
  Backward(prod_.dif_,z0,z1,w_);
  m_ = w_.Rows();
//TRZ(FrobeniusNormU(z0.Data())); TRZ(FrobeniusNormU(z1.Data()));TRZ(FrobeniusNormU(w_.Data()));
//TRZ(w_);
  BMat w0 = w_.Sub(0,0,p_,w_.Columns());
  BMat w1 = w_.Sub(p_,0,w_.Rows()-p_,w_.Columns());
  Backward(prod_.ar_,w0,w1,r_);
//TRZ(r_);
//MatForwardDifEq (BPolyn<BDat>::One()/prod_.ma_,r_,maiFr_);
//TRZ(mair_);
}

//--------------------------------------------------------------------
void BARIMA::CalcResiduals()
//--------------------------------------------------------------------
{
  MatBackwardDifEq(prod_.ar_/prod_.ma_,w0_,w_,a0_,a_);
}

//--------------------------------------------------------------------
void BARIMA::CalcResidualsJacobian(BMatrix<BDat>& J)
//--------------------------------------------------------------------
{
  int i,r,k,j,d;
  BMatrix<BDat> b;
  
  J.Alloc(m_,n_);
  for(r=k=0;k<factor_.Size();k++)
  {
    BPolyn<BDat>& ar = factor_(k).ar_;
    BPolyn<BDat>& ma = factor_(k).ma_;
    
    for(i=1;i<ar.Size();i++)
    {
      d = ar[i].Degree();
      for(j=0; j<m_; j++) 
      { 
        J(j,r) = (j<d)?0:-a_(j-d,0); 
      } 
      r++;
    } 
    MatBackwardDifEq(BPol::One()/ma,a_,b);
    for(i=1;i<ma.Size();i++)
    {
      d = ma[i].Degree();
      for(j=0; j<m_; j++) 
      {
        J(j,r) = (j<d)?0:b(j-d,0); 
      } 
      r++;
    }
  }
/*  
  for(r=0;r<n_;r++)
  {
    for(j=0; j<m_; j++) 
    {
      if(J(j,r).IsUnknown())
        printf("");
    } 
  }
*/
}

//--------------------------------------------------------------------
  bool BARIMA::CalcAutoCovarianze(BInt N)
//--------------------------------------------------------------------
{
  bool ok = ARMAAutoCovarianzeVector(aCov_, prod_.ar_, prod_.ma_,N,1);
  if(!ok) { SetError(); }
  return(ok);
}

//--------------------------------------------------------------------
  bool BARIMA::CalcAutoCorrelation()
//--------------------------------------------------------------------
{
  int i, N = aCov_.Size()-1;
  aCor_.ReallocBuffer(N+1);
  aCor_(0) = 1;
  for(i=1;i<=N;i++)
  {
    aCor_(i) = aCov_(i)/aCov_(0);
  }
  return(true);
}

//--------------------------------------------------------------------
  void BARIMA::SetError()
//--------------------------------------------------------------------
{
  a_ .SetAllValuesTo(BDat::PosInf());
  a0_.SetAllValuesTo(BDat::PosInf());
  w0_.SetAllValuesTo(BDat::PosInf());
  logLikelihood_ = BDat::NegInf(); 
  likelihoodCoef_ = aCovDetN_ = waCoef_ = ata_ = BDat::PosInf(); 
}

//--------------------------------------------------------------------
  bool BARIMA::CheckStationary(bool allowNonStat)
//--------------------------------------------------------------------
{
  bool arOk = true;
  bool maOk = true;
  int i;
  for(i=0; i<factor_.Size(); i++)
  {
    BPolyn<BDat> ar_i = factor_(i).ar_; ar_i.Aggregate();
    BPolyn<BDat> ma_i = factor_(i).ma_; ma_i.Aggregate();
    arOk = arOk && IsStationary(ar_i);
    maOk = maOk && IsStationary(ma_i);
  }
  bool ok = arOk && maOk;
  if(!allowNonStat && !ok)
  {
    Warning(I2("ARMA polynomials are not all stationary.",
               "Los polinomios ARMA no son todos estacionarios."));
    for(i=0; i<factor_.Size(); i++)
    {
      BPolyn<BDat> ar_i = factor_(i).ar_;
      BPolyn<BDat> ma_i = factor_(i).ma_;
      ar_i.Aggregate();
      ma_i.Aggregate();
      bool arOk = IsStationary(ar_i)!=0;
      bool maOk = IsStationary(ma_i)!=0;
      Std(BText("\nStationary?[")<<(arOk?"YES":"NOT")<<"] AR("<<factor_(i).s_<<")="<<factor_(i).ar_.Name());
      Std(BText("\nStationary?[")<<(maOk?"YES":"NOT")<<"] MA("<<factor_(i).s_<<")="<<factor_(i).ma_.Name());
    }
    SetError();
  }
  return(ok);
}


//--------------------------------------------------------------------
  bool BARIMA::CalcLikelihood_Levinson(BDat sigma, BInt N, BBool calcDet, BBool calcInitValues)
//--------------------------------------------------------------------
{
//BTimer tm(BText("CalcLikelihood_Levinson(")<<N<<","<<calcDet<<")");
  BDat aCorLogDet;
  int i, period = 1;
  int mpq = Maximum(p_,q_); 
  for(i=0;i<factor_.Size();i++)
  {
    if(period<factor_(i).s_) { period = factor_(i).s_; }
  }
  if(N<=0) { N = w_.Rows(); }
  if(!CheckStationary(true)) { return(false); }
  int extra = (calcInitValues)?mpq:1;
  //Std(BText("\nEXTRA = ")<<extra);
  if(!CalcAutoCovarianze(N+extra)) { return(false); }
  BArray<BDat> x, w(N+1);
  aCor_.ReallocBuffer(N+1);
  w(0) = 0;
  CalcAutoCorrelation();
  for(i=1;i<=N;i++)
  {
    w(i) = w_(i-1,0);
  }
  aCor_.ReallocBuffer(N+1);
  int k;
  k = Levinson      (aCor_, w, x, aCorI_, paCor_, aCorLogDet, period);
//k = LevinsonARMA  (aCor_, w, x, aCorI_, paCor_, aCorLogDet, period, prod_.ar_, prod_.ma_);
  aCor_ .ReallocBuffer(N);
  aCorI_.ReallocBuffer(N); 
  paCor_.ReallocBuffer(N);
  if((k>=N) && aCorLogDet.IsKnown())
  {
  //TRZ(w0_); TRZ(ata_); TRZ(a0ta0); TRZ(zaCoef_);
    aCoviw_.Alloc(N,1);
    wtCoviw_ = 0;
    for(i=1;i<=N;i++) 
    { 
      aCoviw_(i-1,0) = x(i)/aCov_(0);
      wtCoviw_  += aCoviw_(i-1,0)*w(i); 
    }
    MatForwardDifEq(prod_.ma_/prod_.ar_,aCoviw_,a_);
    if(calcInitValues && mpq)
    { 
      int j;
      BMatrix<BDat> covWW0(mpq,N);
      for(i=0; i<mpq; i++) 
      {
        for(j=0; j<N; j++) 
        {
          covWW0(i,j) = aCov_(mpq-i+j);
        }
      }
      w0_ = covWW0*aCoviw_;
    //Std(BText("\ncovWW0=")<<covWW0.Name());
    //Std(BText("\naCoviw_=")<<aCoviw_.Name());
    //Std(BText("\nuB_=")<<w0_.Name());
/*
      a0_.Alloc(mpq,1);
      BMatrix<BDat> W = w0_ << w_;
      BMatrix<BDat> A = a0_ << a_;
      BDat aux;
      for(k=mpq-1; k>=0; k--) 
      {
        aux = 0;
        for(j=0; j<prod_.ma_.Size()-1; j++) 
        {
          d = prod_.ma_[j].Degree();
          aux-=A(k+mpq-d,0)*prod_.ma_[j].Coef();
        }
        for(j=0; j<prod_.ar_.Size(); j++) 
        {
          d = prod_.ar_[j].Degree();
          aux+=W(k+mpq-d,0)*prod_.ar_[j].Coef();
        }
        aux /= prod_.ma_[prod_.ma_.Size()-1].Coef();
        a0_(k,0) = A(k,0) = aux;
      }
*/
      BPolyn<BDat> psi_B, psi_F;
      psi_B = (prod_.ma_/prod_.ar_).Expand(N+mpq,BFALSE); 
      psi_F.ChangeBF(psi_B);
      bool Pol2Mat(const BPol& pol, int r, int c, BMat& contens_);
      BMat PSI_t, PSI_t_a0w;
      Pol2Mat(psi_F, mpq, N+mpq, PSI_t);
      PSI_t_a0w = PSI_t.Sub(0,mpq,mpq,N); 
      a0_ = PSI_t_a0w*aCoviw_;
    //Std(BText("\naB_=")<<a0_.Name());
    }
    ata_ = (a_.Rows()&&a_.Columns())?MtMSqr(a_)(0,0):BDat::Unknown();
  //ta_ = MtMSqr(a0_)(0,0);
    waCoef_ = Sqrt(wtCoviw_/ata_);
    aCovDetN_    = aCov_(0)*Exp(aCorLogDet/N);
    BDat aCovDetNSqrt = Sqrt(aCovDetN_);
    likelihoodCoef_ = aCovDetNSqrt*waCoef_;
    BDat s2; 
    if(sigma.IsUnknown())
    {
      s2 = ata_/N;
    }
    else
    {
      s2 = sigma^2;
    }
    logLikelihood_ = -0.5*(N*Log(2.0*BDat::Pi()*s2)+N*Log(aCovDetN_)+wtCoviw_/s2);
    if(logLikelihood_.IsUnknown()) { logLikelihood_ = BDat::NegInf(); }

  //#define NOCOMPILE
    #ifdef NOCOMPILE
    TRZ(FrobeniusNormU(aCov_)); 
    TRZ(FrobeniusNormU(aCor_)); 
    TRZ(FrobeniusNormU(z_.Data())); 
    TRZ(FrobeniusNormU(w_.Data())); 
    TRZ(FrobeniusNormU(w)); 
    TRZ(FrobeniusNormU(x)); 
    TRZ(FrobeniusNormU(aCorI_)); 
    TRZ(FrobeniusNormU(aCoviw_.Data())); 
  //TRZ(BMat(1,N,x.Buffer())); 
  //TRZ(BMat(1,N,y.Buffer())); 
  //TRZ(aCoviw_.T()); 
  //TRZ(BMat(1,N,c.Buffer())); 
  //TRZ(BMat(1,N,x.Buffer())); 
  //TRZ(BMat(1,N,y.Buffer())); 
  //TRZ(aCoviw_); 
    Std(BText("\nN                = ")<<N);
    Std(BText("\taCov_(0)         = ")<<aCov_(0));
    Std(BText("\taCovDetN_        = ")<<aCovDetN_);
    Std(BText("\taCOVDetNSqrt     = ")<<aCovDetNSqrt);
    Std(BText("\tS2w=w'*InvCov*w  = ")<<wtCoviw_);
    Std(BText("\tS2a=a'*          = ")<<ata_);
    Std(BText("\tSqrt(S2w/S2a)    = ")<<waCoef_);
    Std(BText("\tlikelihoodCoef   = ")<<likelihoodCoef_);
    #endif
    return(true);
  }
  else
  {
    SetError();
    return(false);
  }

//Std(BText("\nCalcACov(")<<N<<", "<<calcDet<<")");
}


//--------------------------------------------------------------------
  bool BARIMA::CalcLikelihood_Almagro(BDat sigma)
//--------------------------------------------------------------------
{
  int mxpq = Maximum(p_,q_); 
  int m = w_.Rows(); 
  BDat s2 = sigma^2;
  if(mxpq==0)
  {
    a_ = w_;
    a0_.Alloc(0,0);
    w0_.Alloc(0,0);
    ata_ = MtMSqr(a_)(0,0);
    wtCoviw_ = ata_ ;
    aCovDetN_ = 0;
    if(s2.IsUnknown()) { s2 = ata_/m; }
    logLikelihood_ = -0.5*( m*Log(2*BDat::Pi()*s2)+wtCoviw_/s2);
    return(true);
  }
  if(!CheckStationary(true)) { return(false); }
  if(!CalcAutoCovarianze(mxpq)) { return(false); }

  BPolyn<BDat>& ar = prod_.ar_;
  BPolyn<BDat>& ma = prod_.ma_;
  BVMat z; z.DMat2VMat((BMatrix<double>&)w_);
  int q_p = q_+p_;

  BRational<BDat> ma_ar = ma/ar;
  BRational<BDat> ar_ma = ar/ma;
  BRational<BDat> _1_ma = BPolyn<BDat>::One()/ma;
  BPolyn<BDat> gamma = ARMAACovGamma(ar,ma,p_);
  BPolyn<BDat> psi = ma_ar % mxpq;
  BPolyn<BDat> ar0 = (BPolyn<BDat>::F()^p_)*(BPolyn<BDat>::One()-ar);
  BPolyn<BDat> ma0 = (BPolyn<BDat>::F()^q_)*(ma-BPolyn<BDat>::One());

//Std(BText("\nTRACE BARIMA::CalcLikelihood_Almagropsi=\n")<<psi.Name());
  BVMat cov_u;

  if(p_ && q_)
  {
    BVMat cov_zz; cov_zz.BPol2sparse(gamma, p_, p_);
  //Std(BText("\nTRACE BARIMA::CalcLikelihood_Almagrocov_zz=\n")<<cov_zz.GetDMat().Name());
    BVMat cov_aa; cov_aa.Eye(q_);

    BPolyn<BDat> psi_;
    psi_.ChangeBF(psi);
    psi_ = psi_ * (BPolyn<BDat>::B()^Maximum(0,q_-p_));
  //Std(BText("\nTRACE BARIMA::CalcLikelihood_Almagropsi_=\n")<<psi_.Name());
    BVMat cov_za; 
    if(q_>=p_)
    {
      cov_za.BPol2sparse(psi_, q_, p_);
    }
    else
    {
      BVMat zeros;
      zeros.Zeros(q_,p_-q_,BVMat::ESC_chlmRsparse);
      cov_za.BPol2sparse(psi_, q_, q_);
      cov_za = zeros | cov_za;
    }
    BVMat cov_az; BVMat::T(cov_za, cov_az);
  //Std(BText("\nTRACE BARIMA::CalcLikelihood_Almagrocov_za=\n")<<cov_za.GetDMat().Name());
    cov_u =  (cov_zz | cov_az) <<
             (cov_za | cov_aa);
  //Std(BText("\nTRACE BARIMA::CalcLikelihood_Almagrocov_u=\n")<<cov_u.GetDMat().Name()+"\n");
  }
  else if(!p_ && q_)
  {
    cov_u.Eye(q_);
  } 
  if(p_ && !q_)
  {
    cov_u.BPol2sparse(gamma, p_, p_);
  }

  BVMat eye_qp; eye_qp.Eye(q_p);
  BVMat L_u; BVMat::CholeskiFactor(cov_u,L_u,BVMat::ECFO_X,true,true,true);
  BVMat covi_u_; BVMat::CholeskiSolve(L_u,eye_qp,covi_u_,BVMat::ECSS_LLt);
  BVMat covi_u = (covi_u_+covi_u_.T())/2.0;
  BVMat L_u_sp; L_u_sp.Convert(L_u,BVMat::ESC_chlmRsparse);
  double log_det_cov_u = 2*L_u_sp.LogTrace();

  BVMat H1; H1.BPol2sparse(ar0, m, p_);
  BVMat H2; H2.BPol2sparse(ma0, m, q_);
  BVMat H = H1 |H2;
  BVMat G; BVMat::DifEq(_1_ma,H,G,0.8,1.E-17);
  BVMat GtG; BVMat::MtMSqr(G,GtG);
  BVMat cov_G = covi_u + GtG;
  BVMat L_G; BVMat::CholeskiFactor(cov_G,L_G,BVMat::ECFO_X,true,true,true);
  BVMat L_G_sp; L_G_sp.Convert(L_G,BVMat::ESC_chlmRsparse);
  double log_det_cov_G = 2*L_G_sp.LogTrace();
  double log_det_cov_z = log_det_cov_u + log_det_cov_G;

  BVMat piz; BVMat::DifEq(ar_ma,z,piz,0.8,1.E-17);
  BVMat G_pi_z = (piz.T()*G).T();
  BVMat u; BVMat::CholeskiSolve(L_G,G_pi_z,u,BVMat::ECSS_LLt);
  
  BVMat w0; u.Sub(0,  0, p_, 1, w0);
  BVMat a0; u.Sub(p_, 0, q_, 1, a0);
  a0.GetDMat((BMatrix<double>&)a0_);
  w0.GetDMat((BMatrix<double>&)w0_);

  BVMat v; BVMat::CholeskiSolve(L_u,u,v,BVMat::ECSS_L);
  BVMat a; BVMat::DifEq(ar_ma,w0,z,a0,a,0.8,1.E-17);
  a.GetDMat((BMatrix<double>&)a_);

  ata_ = a.Moment(2)*m;
  double vtv = v.Moment(2)*(q_p);
  wtCoviw_ = ata_ + vtv;
  aCovDetN_ = Exp(log_det_cov_z/m);
  if(s2.IsUnknown()) { s2 = ata_/m; }
  logLikelihood_ = -0.5*( m*Log(2*BDat::Pi()*s2)+log_det_cov_z+wtCoviw_/s2);

  return(true);
}


//--------------------------------------------------------------------
  bool BARIMA::CalcLikelihood(BDat sigma)
//--------------------------------------------------------------------
{
  return(CalcLikelihood_Levinson(sigma));
//return(CalcLikelihood_Almagro (sigma));
}

//--------------------------------------------------------------------
#define AcvfFltAr(MSG) \
  c.Replicate(BDat::Unknown(),0); \
  Error(BText("[ARMA ACVF AR-filter] ")+MSG); \
  return(false); 
//--------------------------------------------------------------------



//--------------------------------------------------------------------
  bool BARIMA::ACVF_ARFilter(
    const BArray<BDat>& acvf,
    const BPolyn<BDat>& phi,
    BArray<BDat>& c)
/*
  

*/
//--------------------------------------------------------------------
{
  int s = acvf.Size();
  if(s<=0) { AcvfFltAr(BText(
    "Insuficient autocovariances ")+s+"<"+1); }
  if(acvf(0)<=0) { AcvfFltAr(BText(
    "First autocovariance is negative: ")+acvf(0)); }
  int p = phi.Degree();
  int q = s-p-1;
  if(q<0) { AcvfFltAr(BText(
    "Insuficient autocovariances ")+s+"<"+(p+1)); }
  if(!p) { c=acvf; return(true); }
  c.AllocBuffer(q+1);
  int i,j,k;

  //In p>0 we need to filter the original autocovariances to build
  //the corresponding ACVF for the corresponding pure MA(q) 
  //
  //  c(B+F) = (1-phi(B))*(1-phi(F))*acvf(B+F)
  //
  BArray<BDat> phi_(p+1);
  for(k=0; k<=p; k++)
  {
    phi_(k) = -phi.Coef(k);
  }
  for(j=0; j<=q; j++)
  {
    c(j) = 0;
    for(k=0; k<=p; k++)
    {
      for(i=0; i<=p; i++)
      {
        c(j) += phi_(i)*phi_(k)*acvf(abs(j+i-k));
        assert(c(j).IsKnown());            
      }
    }
  }
  return(true);
}


//--------------------------------------------------------------------
#define PrlmEstErr(MSG) \
  phi.Replicate(BDat::Unknown(),p); \
  theta.Replicate(BDat::Unknown(),q); \
  Error(BText("[ARMA prelim. estim.] ")+MSG); \
  return(false); 
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  bool BARIMA::PreliminaryEstimation(
    const BArray<BDat>& acvf,
    int p,
    int q,
    int N,
    int maxIter,
    BDat eps,
    BDat& variance,
    BArray<BDat>& phi,
    BArray<BDat>& theta)
/*
  Estimates a preliminary AR(p)MA(q) non seassonal model

   (1-phi(B)) z = (1-theta(B)) a 
 
   a ~ Normal(0,variance)

  using just the first 1+p+q sampling autocovariances (acvf) of noise (z)

  The algorithm is given by Box and Jenkins in their book
    Time Series Analysis, forecasting and control
    Revised edition, January 1976
    pages 498-500

*/
//--------------------------------------------------------------------
{
  phi.Replicate(BDat::Unknown(),p);
  theta.Replicate(BDat::Unknown(),q);
  int s = acvf.Size();
  if(p+q+1>s) { PrlmEstErr(BText(
    "Insuficient autocovariances ")+s+"<"+(p+q+1)); }
  if(acvf(0)<=0) { PrlmEstErr(BText(
    "First autocovariance is negative: ")+acvf(0)); }
  int i,j,k;
  if(!p && !q)
  {
    //In a white noise model the only unknown is the variance
    variance = acvf(0);
    return(true);
  }
  for(i=0; i<=p+q; i++)
  {
    if(acvf(i).IsUnknown()) { PrlmEstErr(BText(
      "Unknown autocovariance are not allowed")); }
  }
  if(p)
  {
    //The (acvf[q+1], ..., acvf[q+p]) autocovariances of an AR(p)MA(q)
    //process match the q-displaced Yule-Walker equations 
    // 
    //  A*phi = x
    // 
    //  A[i,j] = acvf[|q+i-j|]
    // 
    //  x[i] = acvf[i]
    // 
    //  i,j = 1 ... p
    //
    BMatrix<BDat> A(p,p), x(p,1);
    for(i=1; i<=p; i++)
    {
      x(i-1,0) = acvf(q+i);
      for(j=1; j<=p; j++)
      {
        k = abs(q+i-j);
        assert((k>=0)&&(k<=p+q));
        A(i-1,j-1) = acvf(k);
      }  
    }
  //phi = CholeskiMinimumResidualsSolve(A,x).Data();
  //phi = gsl_MinimumResidualsSolve(A,x).Data();
    BMat phi_ = SvdMinimumResidualsSolve(A,x);
    phi = phi_.Data();
    BMat err = A*phi_ - x;
  //Std(BText("[ARMA prelim. estim.] Yule-Walker A=\n")+A.Name()+"\n x=\n"+x.Name()+"\n phi=\n"+phi_.Name()+"\n err=\n"+err.Name()+"\n");

    if(phi.Size()!=p) { PrlmEstErr(BText(
    "Yule-Walker equations are singular or are wrong defined")); }
  }
  if(!q)
  {
    //In a pure AR(p), the variance is (1-phi(B)):acvf
    variance = acvf(0);
    for(i=1; i<=p; i++)
    {
      variance -= acvf(i)*phi(i-1);
    }
    if(variance<=0) { PrlmEstErr(BText(
    "Yule-Walker equations returns negative variance")); }
  }
  else
  {
    if(maxIter <= 0) { maxIter = 100*q; }
    if(eps <= 0) { eps = 0.1/Sqrt(N); }
    BArray<BDat> c(q+1);
    //In a AR(p)MA(q) we will estimate the theta coefficients fitting the 
    //AR-filtered autocovariances non linear equations
    //
    //  c(B+F) = (1-phi(B))*(1-phi(F))*acvf(B+F) = (1-theta(B))*(1-theta(F))
    //
    if(!p)
    {
      //In a pure MA(q) we will have directly 
      // 
      // c(B+F) = acvf(B+F)
      // 
      for(j=0; j<=q; j++)
      {
        c(j) = acvf(j);
      }
    }
    else
    {
      //In p>0 we need to filter the original autocovariances to build
      //the corresponding ACVF for the corresponding pure MA(q) 
      //
      //  c(B+F) = (1-phi(B))*(1-phi(F))*acvf(B+F)
      //
      BArray<BDat> phi_(p+1);
      phi_(0) = -1;
      for(k=1; k<=p; k++)
      {
        phi_(k) = phi(k-1);
      }
      for(j=0; j<=q; j++)
      {
        c(j) = 0;
        for(k=0; k<=p; k++)
        {
          for(i=0; i<=p; i++)
          {
            c(j) += phi_(i)*phi_(k)*acvf(abs(j+i-k));
            assert(c(j).IsKnown());            
          }
        }
      }
    }
    //We will use multivariate Newton-Raphson to solve the non linear equations
    // 
    //  t <- t - h
    //  T*h = f
    //  f = (1-phi(B))*(1-phi(F))*acvf(B+F) - c(B+F)
    //  T = phi(B) + phi(F)
    // 
    BDat F, minF=BDat::PosInf();
    BMatrix<BDat> t(q+1,1), tMin(q+1,1), h(q+1,1), f(q+1,1), T(q+1,q+1);
    //Initializes vector t with values corresponding to white noise
    t.SetAllValuesTo(0);
    t(0,0) = Sqrt(c(0));
    //Iteration counter
    int iter = 0;
    //Newton-Raphson cycle
  //Std(BText("[ARMA prelim. estim.] iter[")+iter+"] t=\n"+t.Name()+"\n");
    do
    {
    //if(iter>=maxIter) { PrlmEstErr(BText("Exceeded maximum number of Newton iterations")); }
      iter++;
      //Calculates vector f, and its module F = |f|
      F = 0;
      for(j=0; j<=q; j++) 
      { 
        BDat fj = -c(j); 
        for(i=0; i<=q-j; i++) 
        { 
          fj += t(i,0)*t(i+j,0); 
        }
        f(j,0) = fj;
        fj = Abs(fj);
        if(fj>F) { F = fj; }
      }
      if(F<minF)
      {
        minF = F;
        tMin = t;
      }  
      //Calculates matrix T
      T.SetAllValuesTo(0);
      for(i=0; i<=q; i++) 
      { 
        for(j=i; j<=q; j++) 
        { 
          T(i,j) += t(j-i,0);
        }
        for(j=0; j<=q-i; j++) 
        { 
          T(i,j) += t(j+i,0);
        }
      }
      //Solves linear system T*h = f
    //h = CholeskiMinimumResidualsSolve(T,f);
    //h = gsl_MinimumResidualsSolve(T,f);
      h = SvdMinimumResidualsSolve(T,f);
      if(!h.Rows()|| !h.IsKnown()) { PrlmEstErr(BText(
        "Linear step of Newton iteration has a singular or wrong defined coefficient matrix")); }
      //Newton update
      t -= h;
    //Std(BText("[ARMA prelim. estim.] iter[")+iter+"] f=\n"+f.Name()+"\n |f|="+F+"\n T=\n"+T.Name()+"\n h=\n"+h.Name()+" t=\n"+t.Name()+"\n\n");
    }
    while(F>eps && iter<=maxIter );
    //Retrieve variance as square of first term of vector t
    variance = tMin(0,0)^2;
    //Retrieve theta coefficients as rest of oposite of scaled values on vector t
    for(j=1; j<=q; j++) 
    {   
      theta(j-1) = -tMin(j,0)/tMin(0,0);
    }
  }
  return(true);
}



//------------------------------------------------------------------
BARIMACondLeastSqr::BARIMACondLeastSqr(BARIMA* model)
: BRnRmFunction(), M_(model)
//------------------------------------------------------------------
{
  SetDimensions(M_->n_, M_->m_);
};

//------------------------------------------------------------------
void BARIMACondLeastSqr::Evaluate(      BArray<BDat>& a,
              const BArray<BDat>& p)
//------------------------------------------------------------------
{
  M_->PutCoef(p);
  M_->CalcResiduals();
  a = M_->a_.Data();
};

//------------------------------------------------------------------
void BARIMACondLeastSqr::Jacobian(const BArray<BDat>&  x,
              const BArray<BDat>&  y,
                    BMatrix<BDat>& J)
//------------------------------------------------------------------
{
  M_->PutCoef(x);
  M_->CalcResidualsJacobian(J);
};

//------------------------------------------------------------------
BBool BARIMACondLeastSqr::Marquardt()
//------------------------------------------------------------------
{
  BDiagMatrix<BDat> Dp, D;
  BMatrix<BDat> U,V,Vt;
  x.Alloc(n_,1);
  G.Alloc(n_,1);
  H.Alloc(n_,n_);
  U.Alloc(m_, n_);
  V.Alloc(n_, n_);
  D.Alloc(n_);
  M_->stdErr_ = LeastSqrMarquardt(M_->coef_, M_->a_.GetData(), M_->Ja_);
  stdErr = M_->stdErr_;
  BDat var = stdErr^2;
  x.GetData() = M_->coef_;
  gsl_SingularValueDecomposition(M_->Ja_, U, D, V,
  BText("Golub_Reinsch_Mod"));
  Vt  = V.T();
  Dp  = D.P(Sqrt(D.Rows()*BDat::Zero()));
  H = (V*D*D*Vt)/var;
  S = (V*Dp*Dp*Vt)*var;
  SLt = V*Dp*stdErr;
  return(stdErr.IsKnown());
};

//------------------------------------------------------------------
BDat BARIMACondLeastSqr::StationarityProb(int sampleLength)
//------------------------------------------------------------------
{
  int iter, t, nonStationaryCount=0;
  BMatrix<BDat> e(n_,1), z(n_,1);
  for(iter=1; iter<= sampleLength; iter++)
  {
    for(t=0; t<n_; t++)
    {
      e(t,0) = BNormalDist::Random01();
    }
    z = x + SLt * e;
    M_->PutCoef(z.Data());
    nonStationaryCount += !M_->CheckStationary(true);  
  }
  return(double(nonStationaryCount)/sampleLength);
};


/* */
