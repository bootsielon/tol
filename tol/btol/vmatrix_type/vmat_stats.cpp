/* vmat_stats.cpp: BVMat statistics methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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
#include <tol/tol_bvmat_impl.h>
#include "gsl_ext.h"
#include <gsl/gsl_math.h>

const double vmat_trunc_eps = 1.0e-12;

#define USE_BTruncatedNormalDist


////////////////////////////////////////////////////////////////////////////////
//Matrix algebra operators
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
double BVMat::Trace() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatTrace")) { return(BDat::Nan()); }
  int k,nzmax;
  const double *x;
  double trace = 0.0;
  int r = Rows();
  int c = Columns();
  int m = Minimum(r,c);
  BVMat aux;
  switch(code_) {
  case(ESC_blasRdense ) :
    if(!StoredData(x, nzmax)) { return(0.0); }
    for(k=0; k<m; k++) 
    { 
      trace += x[k*r+k]; 
    }
    break;
  case(ESC_chlmRsparse ) :
    SubBand(0,0,aux);
    if(!aux.StoredData(x, nzmax)) { return(0.0); }
    for(k=0; k<m; k++, x++) 
    { 
      trace += *x; 
    }
    break;
  default:
    err_cannot_apply("Trace",I2("(Not implemenmted)",
                   "(Metodo no implementado)"),*this);
    return(BDat::Nan()); }

  return(trace);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::LogTrace() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatTrace")) { return(BDat::Nan()); }
  int k,nzmax;
  const double *x;
  double trace = 0.0;
  int r = Rows();
  int c = Columns();
  int m = Minimum(r,c);
  BVMat aux;
  switch(code_) {
  case(ESC_blasRdense ) :
    if(!StoredData(x, nzmax)) { return(0.0); }
    for(k=0; k<m; k++) 
    { 
      trace += log(x[k*r+k]); 
    }
    break;
  case(ESC_chlmRsparse ) :
    SubBand(0,0,aux);
    if(!aux.StoredData(x, nzmax)) { return(0.0); }
    for(k=0; k<m; k++, x++) 
    { 
      trace += log(*x); 
    }
    break;
  default:
    err_cannot_apply("LogTrace",I2("(Not implemenmted)",
                   "(Metodo no implementado)"),*this);
    return(BDat::Nan()); }

  return(trace);
}


////////////////////////////////////////////////////////////////////////////////
double BVMat::Sum() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatSum")) { return(BDat::Nan()); }
  int k,nzmax;
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  double result=0;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      result += x[k];
    }
  }
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Avr() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatAvr")) { return(BDat::Nan()); }
  int k,nzmax;
  int size = Rows()*Columns();
  int n = size;
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  double result=0;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      result += x[k];
    }
    else
    {
      n--;
    }
  }
  if(n==0) { result = BDat::Nan(); }
  else     { result /= n; }
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Moment(int order) const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatMoment")) { return(BDat::Nan()); }
  int k,nzmax;
  int size = Rows()*Columns();
  int n = size;
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  if(order<1) { order=1; }
  double result = 0;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      result += pow(x[k],order);
    }
    else
    {
      n--;
    }
  }
  if(n==0) { result = BDat::Nan(); }
  else     { result /= n; }
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::CenterMoment(int order) const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatCenterMoment")) { return(BDat::Nan()); }
  double avr = Avr();
  int k,nzmax;
  int size = Rows()*Columns();
  int n = size;
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  if(order<1) { order=1; }
  double result = 0;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      result += pow(x[k]-avr,order);
    }
    else
    {
      n--;
    }
  }
  result+=pow(-avr,order)*(size-nzmax);
  if(n==0) { result = BDat::Nan(); }
  else     { result /= n; }
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Var() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatVar")) { return(BDat::Nan()); }
  return(CenterMoment(2));
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::StDs() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatStDs")) { return(BDat::Nan()); }
  return(sqrt(Var()));
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::FrobeniusNorm() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatFrobeniusNorm")) { return(BDat::Nan()); }
  return(sqrt(Moment(2)));
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::SquaredSum() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatSquaredSum")) { return(BDat::Nan()); }
  int k,nzmax;
  int size = Rows()*Columns();
  int n = size;
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  double result = 0;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      result += x[k]*x[k];
    }
    else
    {
      n--;
    }
  }
  return(result);
}
////////////////////////////////////////////////////////////////////////////////
double BVMat::EuclideanNorm() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatEuclideanNorm")) { return(BDat::Nan()); }
  return(sqrt(SquaredSum()));
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Asymmetry() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatAsymmetry")) { return(BDat::Nan()); }
  return(CenterMoment(3)/pow(Var(),3.0/2.0));
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Kurtosis() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatKurtosis")) { return(BDat::Nan()); }
  return((CenterMoment(4)/pow(Var(),2.0))-3);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Max() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatMax")) { return(BDat::Nan()); }
  int k,nzmax;
  int size = Rows()*Columns();
  if(!size) { return(BDat::Nan()); }
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  double result=(size>nzmax)?0.0:BDat::NegInf();
  bool allUnknown = true;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      if(result<x[k]) { result=x[k]; }
      if(allUnknown) { allUnknown=false; }
    }
  }
  if(allUnknown) { result = BDat::Nan(); }
  return(result);
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Min() const
////////////////////////////////////////////////////////////////////////////////
{
  if(!CheckDefined("VMatMin")) { return(BDat::Nan()); }
  int k,nzmax;
  int size = Rows()*Columns();
  if(!size) { return(BDat::Nan()); }
  const double *x;
  if(!StoredData(x, nzmax)) { return(0.0); }
  double result=(size>nzmax)?0.0:BDat::PosInf();
  bool allUnknown = true;
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      if(result>x[k]) { result=x[k]; }
      if(allUnknown) { allUnknown=false; }
    }
  }
  if(allUnknown) { result = BDat::Nan(); }
  return(result);
}

/*
////////////////////////////////////////////////////////////////////////////////
double BVMat::Traze() const
////////////////////////////////////////////////////////////////////////////////
{
}



////////////////////////////////////////////////////////////////////////////////
double BVMat::FrobeniusNorm() const
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::GeometricAvr() const
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::HarmonicAvr() const
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Median() const
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Prod() const
////////////////////////////////////////////////////////////////////////////////
{
}

////////////////////////////////////////////////////////////////////////////////
double BVMat::Quantile() const
////////////////////////////////////////////////////////////////////////////////
{
}

*/

////////////////////////////////////////////////////////////////////////////////
  cholmod_dense_struct* BVMat::bRd_rand(int nrow, int ncol, BProbDist& xu)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  cholmod_dense* dense = CholmodAllocate_dense
  ( nrow, ncol, nrow, CHOLMOD_REAL, common_);
  int k;
  if(dense)
  {
    double* x = (double*)dense->x;
    for(k=0; k<dense->nzmax; k++)
    {
      x[k] = xu.Random().Value(); 
    }
  }
  return(dense);
};
  
////////////////////////////////////////////////////////////////////////////////
  cholmod_triplet_struct* BVMat::cRt_rand(int nrow, int ncol, int nzmax,
                                          BProbDist& dist)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
//BHashPairs hashPairs;
  BUniformDist iu(-0.49999999,nrow-0.49999999);
  BUniformDist ju(-0.49999999,ncol-0.49999999);
  BIntPair p;
  cholmod_triplet* triplet = CholmodAllocate_triplet
  ( nrow, ncol, nzmax, 0, CHOLMOD_REAL, common_);
  if(triplet)
  {
    int k;
    int* i = (int*)triplet->i;
    int* j = (int*)triplet->j;
    double* x = (double*)triplet->x;
    for(k=0; k<nzmax; k++)
    {
      p.r_ = (int)::Round(iu.Random()).Value();
      p.c_ = (int)::Round(ju.Random()).Value();
    //BHashPairs::const_iterator fc = hashPairs.find(p);
    //if(fc==hashPairs.end())
      {
        i[k] = p.r_; 
        j[k] = p.c_; 
        x[k] = dist.Random().Value(); 
        if(x[k]!=0.0)
        { 
          triplet->nnz++; 
        //hashPairs[p] = x[k];
        }
      }
    }
  }
  return(triplet);
};
  

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Rand(int nrow, int ncol,
                   double min, double max, 
                   ECode c, int nzmax)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  if((min==0.0)&&(max==0.0)) { Zeros(nrow, ncol, c); return; }
  Delete();
  int rc = nrow*ncol;
  if(nzmax< 0 ) { nzmax = rc; }
  if(nzmax>=rc) { nzmax = rc; }
  BUniformDist dist(min,max);
  cholmod_triplet_struct* trp;
  switch(c) {
  case(ESC_blasRdense ) :
    code_  = ESC_blasRdense;         
    s_.blasRdense_ = bRd_rand(nrow, ncol, dist);
    break;
  case(ESC_chlmRtriplet ) :
    code_ = ESC_chlmRtriplet;         
    s_.chlmRtriplet_= cRt_rand(nrow,ncol,nzmax,dist);
    break;
  case(ESC_chlmRsparse ) :
    code_  = ESC_chlmRsparse;
    trp = cRt_rand(nrow,ncol,nzmax,dist);
    s_.chlmRsparse_= CholmodTripletToSparse(trp, trp->nnz, common_);
    CholmodFree_triplet(&trp, common_);
    break;
  default: 
    err_cannot_create("Rand",c);  }
};

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Gaussian(int nrow, int ncol,
                       double nu, double sigma)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  Delete();
  BNormalDist dist(nu,sigma);
  code_  = ESC_blasRdense;         
  s_.blasRdense_ = bRd_rand(nrow, ncol, dist);
};


////////////////////////////////////////////////////////////////////////////////
  int BVMat::GetBoundsInPolytope(const BVMat&  D, 
                                 const BVMat&  d_, 
                                       BVMat&  z_,
                                       int     j,
                                       double& lower,
                                       double& upper)
// Calculates minimum and maximum values of z[j] matching
//   D*z <= d  
////////////////////////////////////////////////////////////////////////////////
{
  static char* fName = "GetBoundsInPolytope";
  if(!D .CheckDefined(fName)) { return(-1); }
  if(!d_.CheckDefined(fName)) { return(-1); }
  if(!z_.CheckDefined(fName)) { return(-1); }
  int Dr = D.Rows();
  int Dc = D.Columns();
  int dr = d_.Rows();
  int dc = d_.Columns();
  int zr = z_.Rows();
  int zc = z_.Columns();
  if((Dr!=dr)||(dc!=1)||
     (Dc!=zr)||(zc!=1)|| 
     !Dc||!Dr||!dc||!dr||!zc||!zr)
  {
    err_invalid_dimensions(fName,D,d_,z_);
    return(-1);
  }
  BVMat d(d_,ESC_blasRdense);
  BVMat z(z_,ESC_blasRdense);
  BVMat  b = d-D*z;
  int i, k, h;
  double  aux;
  bool    sp = D.code_ == ESC_chlmRsparse;
  int     n  = z.s_.blasRdense_->nrow;
  int     r  = d.s_.blasRdense_->nrow;
  int     k0 = 0;
  int     k1 = r;
  double* zx = (double*)z.s_.blasRdense_->x; 
  double* dx = (double*)d.s_.blasRdense_->x;
  double* bx = (double*)b.s_.blasRdense_->x;
  double* Dx = (double*)(sp?D.s_.chlmRsparse_->x:D.s_.blasRdense_->x);
  int*    Di = (int*   )(sp?D.s_.chlmRsparse_->i:NULL);
  int*    Dp = (int*   )(sp?D.s_.chlmRsparse_->p:NULL);
  double  negInf = BDat::NegInf();
  double  posInf = BDat::PosInf();
  int     nnz    = D.NonNullCells();
  double  drop   = nnz*vmat_trunc_eps;
  
  double& zj = zx[j];
//Std(BText("\nj=")<<j);
  lower=negInf; 
  upper=posInf;
  if(sp)
  {
    k0 = Dp[j];
    k1 = Dp[j+1];
  }
  else
  {
    h = j*Dr;
  }
  for(k=k0; k<k1; k++)
  {
    i = sp?Di[k]:k;
    const double& Dij = sp?Dx[k]:Dx[h++];
    int Dij_sign = gsl_fcmp(Dij+1.0,1.0,vmat_trunc_eps);
    if(Dij_sign!=0)
    {
      bx[i] += Dij*zj;
    //Std(BText("\nD[")+i+","+j+"]="+Dij+"\tz["+j+"]="+zj+"\tb["+i+"]="+bx[i]+"\td["+i+"]="+dx[i]);
      if(Dij_sign<0) 
      {
        aux = bx[i]/Dij;
        if(lower<aux) { lower = aux; }
      //Std(BText("\naux=")+aux+"\tlower="<<lower);
      }
      else //if(Dij_sign>0) 
      {
        aux = bx[i]/Dij;
        if(upper>aux) { upper = aux; }
      //Std(BText("\naux=")+aux+"\tupper="<<upper);
      }
    }
  }
//Std(BText("\nlower=")<<lower<<"\tupper="<<upper);
  double length = upper-lower;
  int length_sign = gsl_fcmp(length+1.0,1.0,vmat_trunc_eps);
  if(length_sign<=0)
  {
    if(length_sign==0)
    {
      lower = upper;
    }
    else
    {
      err_cannot_apply("GetBoundsInPolytope",
        I2("(empty interval",
           "(intervalo vacio")+" ["+lower+","+upper+"])",z);
      return(-1);
    }
  }
  return(0);
}


////////////////////////////////////////////////////////////////////////////////
  int BVMat::trunc_std_gaussian(const BVMat& D, 
                                const BVMat& d, 
                                      BVMat& b, 
                                      BVMat& z,
                                double borderDistance)
// Generates a set of standarized multinormal z constrained to
//   D*z <= d  
// Value od d-D*z is stored in b and must be initialized by the caller function
////////////////////////////////////////////////////////////////////////////////
{
  int i, j, k, h, hj;
  double  aux;
  bool    sp = D.code_ == ESC_chlmRsparse;
  int     n  = z.s_.blasRdense_->nrow;
  int     r  = d.s_.blasRdense_->nrow;
  int     k0 = 0;
  int     k1 = r;
  double* zx = (double*)z.s_.blasRdense_->x; 
  double* dx = (double*)d.s_.blasRdense_->x;
  double* bx = (double*)b.s_.blasRdense_->x;
  double* Dx = (double*)(sp?D.s_.chlmRsparse_->x:D.s_.blasRdense_->x);
  int*    Di = (int*   )(sp?D.s_.chlmRsparse_->i:NULL);
  int*    Dp = (int*   )(sp?D.s_.chlmRsparse_->p:NULL);
  double  negInf = BDat::NegInf();
  double  posInf = BDat::PosInf();
  double  lower  = BDat::NegInf();
  double  upper  = BDat::PosInf();
  int     nnz    = D.NonNullCells();
  double  drop   = nnz*vmat_trunc_eps;

  for(h=hj=j=0; j<n; j++)
  {
    double& zj = zx[j];
  //Std(BText("\ntrunc_std_gaussian column j=")<<j);
    lower=negInf; 
    upper=posInf;
    if(sp)
    {
      k0 = Dp[j];
      k1 = Dp[j+1];
    }
    else
    {
      hj = h;
    }
    for(k=k0; k<k1; k++)
    {
      i = sp?Di[k]:k;
      const double& Dij = sp?Dx[k]:Dx[h++];
      int Dij_sign = gsl_fcmp(Dij+1.0,1.0,vmat_trunc_eps);
      if(Dij_sign!=0)
      {
        bx[i] += Dij*zj;
        aux = bx[i]/Dij;
      //Std(BText("\n  D[")+i+","+j+"]="+BDat(Dij)+"\tz["+j+"]="+BDat(zj)+"\tb["+i+"]="+BDat(bx[i])+"\td["+i+"]="+BDat(dx[i]));
        if(Dij_sign<0) 
        {
          if(lower<aux) { lower = aux; }
        }
        else //if(Dij_sign>0) 
        {
          if(upper>aux) { upper = aux; }
        }
      }
    }
  //Std(BText("\nlower=")<<BDat(lower)<<"\tupper="<<BDat(upper));
    double length = upper-lower;
    if(fabs(length)<=1.E-8)
    {
      //Si el intervalo es cuasi-vacío estamos rondando la frontera pero
      //el punto es prácticamente factible.  
      Std(BText("(I) TruncStdGaussian [Case A] ")<<
        I2("(cuasi-empty interval",
        "(intervalo cuasi-vacio")+" ["+BDat(lower).Format("%.16lg")+","+
                                       BDat(upper).Format("%.16lg")+"])");
      zj = (lower+upper)*0.5;
    }
    else if(length<0)
    {
      //Si el intervalo es vacío estamos fuera de la región factible 
      warn_cannot_apply("(I) TruncStdGaussian [Case B] ",
        I2("(empty interval",
        "(intervalo vacio")+" ["+BDat(lower).Format("%.16lg")+","+
                                 BDat(upper).Format("%.16lg")+"])",z);
      return(-1);
    }
# ifdef USE_BTruncatedNormalDist
    else if((lower>negInf)||(upper<posInf))
    {
      //A continuación se generará una N(0,1) truncada estrictamente en el
      //interior del intevalo de dominio abierto (lower, upper) para evitar
      //problemas numéricos de frontera.

      //Máximo valor absoluto para el que se usará la distribución N(0,1)
      double K = 6;
      //La probabilidad de estar una N(0,1) fuera del intervalo [-K,K] es 
      //con K=6 es p=1.973175400848959e-009 ~ 1 entre 500 mil millones, por lo que 
      //es despreciable cualquier valor fuera del mismo y nos evitamos problemas 
      //numéricos, pues tanto la cumulativa como su inversa funcionan bien 
      //en este intervalo pero empiezan a fallar a partir de K=8. 
      //Quizás podría ponerse K=7 pero se toma K=6 por dar un margen y porque
      //la pérdida de precisión es irrelevante tan lejos de la moda.
      //Fuera de ahí la distribución se aproximará por la raíz cuadrada de una 
      //uniforme en el dominio reducido interior al original de tal forma que 
      //el que el cociente de verosimilitudes entre los extremos sea menor o 
      //igual a 100, pues en ese rango resulta una buena aproximación de la 
      //normal truncada y es al mismo tiempo tratable numéricamente.
      // 
      //El cociente de densidades entre dos puntos x,y ~ N(0,1) es 
      // 
      //  f(x)/f(y) = Exp(-0.5*(x^2-y^2))
      // 
      //Si queremos un punto y que sea h veces menos probable que x entonces 
      // 
      //  Log(h) = -0.5*(x^2-y^2)
      //  y^2 = x^2 + 2*Log(h)
      //
      //Estas operaciones no comportan riesgos numéricos incluso para valores
      //muy grandes.
      double lowMrg = lower;
      double uppMrg = upper;
      if(lowMrg>K)
      { 
        if(uppMrg>=2*lowMrg) { uppMrg=2*lowMrg; }
        //Si todo el dominio está por encima de K tomamos como nuevos límites
        //tentativos los puntos con densidad 1.1 y 110 veces las del límite inferior.
        //De esta forma el nuevo límite inferior es por construcción 100 veces más 
        //probable que el superior 
        double lowMrg_ = sqrt(pow(lowMrg,2) + 2.0 * log(  1.1) );
        double uppMrg_ = sqrt(pow(uppMrg,2) + 2.0 * log(110.0) );
        //Si los límites tentativos se salen del dominio hay que meterlos dentro 
        if(lowMrg_>=upper)
        {
          lowMrg = 0.99*lowMrg+0.01*uppMrg;
          uppMrg = 0.99*uppMrg+0.01*lowMrg;
        }
        else 
        {
          lowMrg = lowMrg_;
          if(uppMrg_>=upper)
          {
            uppMrg = 0.99*uppMrg+0.01*lowMrg;
          }
          else
          {
            uppMrg = uppMrg_;
          }
        }
        //Tomamos un r uniforme
        double r = BUniformDist::Random01().Value();
        //y calculamos su raíz cuadrada como valor de ponderación de los límites
        //para forzar puntos más cercanos al nuevo límite inferior.
        double u = sqrt(r);
        zj = u*lowMrg + (1.0-u)*uppMrg;
        if((zj<=lower) ||(zj>=upper))
        { 
          warn_cannot_apply("(I) TruncStdGaussian [Case C.1] ", 
            BText(" j=")<<j<<
            " domain:["<<lower<<","<<upper<<"] -> "<<
            " interior domain: ["<<lowMrg<<","<<uppMrg<<"] -> "<<zj,z);
          return(-1);
        }
      }
      else if(upper<-K)
      {
        if(lowMrg<=2*uppMrg) { lowMrg=2*uppMrg; }
        //Si todo el dominio está por debajo de -K tomamos como nuevos límites
        //tentativos los puntos con densidad 1.1 y 110 veces las del límite superior.
        //De esta forma el nuevo límite superior es por construcción 100 veces más 
        //probable que el inferior 
        double uppMrg_ = -sqrt(pow(uppMrg,2) + 2.0 * log(  1.1) );
        double lowMrg_ = -sqrt(pow(lowMrg,2) + 2.0 * log(110.0) );
        //Si los límites tentativos se salen del dominio hay que meterlos dentro 
        if(uppMrg_<=lower)
        {
          lowMrg = 0.99*lowMrg+0.01*uppMrg;
          uppMrg = 0.99*uppMrg+0.01*lowMrg;
        }
        else 
        {
          uppMrg = uppMrg_;
          if(lowMrg_<=lower)
          {
            lowMrg = 0.99*lowMrg+0.01*uppMrg;
          }
          else
          {
            lowMrg = lowMrg_;
          }
        }
        //Tomamos un r uniforme
        double r = BUniformDist::Random01().Value();
        //y calculamos su raíz cuadrada como valor de ponderación de los límites
        //para forzar puntos más cercanos al nuevo límite inferior.
        double u = sqrt(r);
        zj = u*uppMrg + (1.0-u)*lowMrg;
        if((zj<=lower) ||(zj>=upper))
        { 
          warn_cannot_apply("(I) TruncStdGaussian [Case C.2] ", 
            BText(" j=")<<j<<
            " domain:["<<lower<<","<<upper<<"] -> "<<
            " interior domain: ["<<lowMrg<<","<<uppMrg<<"] -> "<<zj,z);
          return(-1);
        }
      }
      else
      {
        //Si alguno de los límites cae fuera de [-K,K] hacemos la intersección
        if(lower<-K) { lowMrg=-K; }
        if(upper>+K) { uppMrg=+K; }
        BNormalDist u01(0,1);
        //Calculamos los cuantiles de la N(0,1) en los extremos
        double lowF01 = u01.Dist(lowMrg).Value();
        double uppF01 = u01.Dist(uppMrg).Value();
        //Calculamos el desplazamiento en la métrica probabilística
        double difF01 = borderDistance*(uppF01-lowF01);
        //Desplazamos los límites hacia el interior
        double lowF01_ = lowF01+difF01;
        double uppF01_ = uppF01-difF01;
        //Tomamos un r uniforme
        double r = BUniformDist::Random01().Value();
        //Tomamos un u normal truncado en el dominio interior
        double u = r*uppF01_ + (1.0-r)*lowF01_;
        //Volvemos a la métrica normalizada   
        zj = u01.Inverse(u).Value(); 
        if((zj<=lower) ||(zj>=upper))
        { 
          warn_cannot_apply("(I) TruncStdGaussian [Case C.1] ", 
            BText(" j=")<<j<<
            " domain:["<<lower<<","<<upper<<"] -> "<<
            " Prob domain["<<lowF01<<","<<uppF01<<"] -> "<<
            " Prob interior domain["<<lowF01_<<","<<uppF01_<<"] -> "<<zj,z);
          return(-1);
        }
      }
    }
    else
    {
      zj = BNormalDist::Random01().Value();
      if((zj<=lower) ||(zj>=upper))
      { 
        warn_cannot_apply("(I) TruncStdGaussian [Case D] ",
          I2("(empty interval",
          "(intervalo vacio")+" ["+BDat(lower).Format("%.16lg")+","+
                                   BDat(upper).Format("%.16lg")+"])",z);
      }
    }


# else
    zj = gsl_rtabnorm_combo(getGslRng(), 0.0, 1.0,
                     lower, upper, 10);
    assert((lower<=zj) && (zj<=upper));
    if(!BDat(zj).IsFinite())
    {
      err_cannot_apply("(II) TruncStdGaussian",
        I2("(empty interval",
           "(intervalo vacio")+" ["+lower+","+upper+"])",z);
      return(-2);
    }
# endif
    if(!sp) { h=hj; }
    for(k=k0; k<k1; k++)
    {
      i = sp?Di[k]:k;
      const double& Dij = sp?Dx[k]:Dx[h++];
      bx[i] -= Dij*zj;
    //Std(BText("\nD[")+i+","+j+"]="+Dij+"\tz["+j+"]="+zj+"\tb["+i+"]="+bx[i]);
    }
  }
  return(0);
};


////////////////////////////////////////////////////////////////////////////////
  void BVMat::TruncStdGaussian(const BVMat& D, const BVMat& d_, 
                               const BVMat& z0, int ncol, int burnin,
                               double borderDistance)
//Matrix instances
////////////////////////////////////////////////////////////////////////////////
{
  static char* fName = "TruncStdGaussian";
  if(!D .CheckDefined(fName)) { return; }
  if(!d_.CheckDefined(fName)) { return; }
  if(!z0.CheckDefined(fName)) { return; }
  int Dr = D.Rows();
  int Dc = D.Columns();
  int dr = d_.Rows();
  int dc = d_.Columns();
  int zr = z0.Rows();
  int zc = z0.Columns();
  if((Dr!=dr)||(dc!=1)||
     (Dc!=zr)||(zc!=1)|| 
     !Dc||!Dr||!dc||!dr||!zc||!zr)
  {
    err_invalid_dimensions(fName,D,d_,z0);
    return;
  }
  double* aux;
  int k, nrow = Dc;
  BVMat d(d_,ESC_blasRdense);
  BVMat z(z0,ESC_blasRdense);
  BVMat Dz = D*z;
  BVMat b  = d-Dz;
  double bMin = b.Min();
  int    nnz  = D.NonNullCells();
  double drop = nnz*vmat_trunc_eps;
  if(bMin<-drop)
  {
    err_cannot_apply(fName,
      I2("(non feasible initial point)",
         "(valor inicial no factible)"),z0);
    return;
  }
  if(burnin<0 ) { burnin = 0; }
  BVMat burn;
  burn.Zeros(nrow, burnin, ESC_blasRdense);
  Zeros(nrow, ncol, ESC_blasRdense);
  int size_row = nrow*sizeof(double);
  switch(D.code_) {
  case(ESC_blasRdense ) :
  case(ESC_chlmRsparse ) :
    aux = (double*)burn.s_.blasRdense_->x;
    for(k=0; k<burnin; k++, aux+=nrow)
    {
      if(trunc_std_gaussian(D, d, b, z, borderDistance)!=0) { return; }  
      memcpy(aux, z.s_.blasRdense_->x, size_row);
    }
    aux = (double*)s_.blasRdense_->x;
    for(k=0; k<ncol; k++, aux+=nrow)
    {
      if(trunc_std_gaussian(D, d, b, z, borderDistance)!=0) { return; }  
      memcpy(aux, z.s_.blasRdense_->x, size_row);
    }
    break;
  default: 
    err_cannot_create(fName,D.code_);  }
};

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Histogram_bRd(const BVMat& M, int parts, double min, double max)
////////////////////////////////////////////////////////////////////////////////
{
  int i,j,p;
  int n = M.s_.blasRdense_->nzmax;
  int r = M.s_.blasRdense_->nrow;
  int c = M.s_.blasRdense_->ncol;
  BlasRDense(parts, c+1);
  double* h = (double*)s_.blasRdense_->x; 
  double step = (max-min)/double(parts);
  for(p=0; p<parts-1; p++)
  {
    *(h++) = min+step*(p+1);
  }
  *(h++) = max;
  memset(h,0,sizeof(double)*parts*c);
  const double* x = (const double*)M.s_.blasRdense_->x; 
//double* y = (double*)s_.blasRdense_->x+parts; 
  for(j=0; j<c; j++)
  {
    double* h0 = ((double*)s_.blasRdense_->x)+parts*(j+1);
    for(i=0; i<r; i++, x++)
    {
      p = (int)((*x-min)/step)-1;
      if(p<0) { p=0; }
      if(p>=parts) { p=parts-1; }
      h0[p] += 1.0/r;
    }
  }
};

/* * /
////////////////////////////////////////////////////////////////////////////////
  void BVMat::Histogram_cRs(const BVMat& M, int parts, double min, double max)
////////////////////////////////////////////////////////////////////////////////
{
  int i,j,p;
  int r = M.s_.cholmodRsparse_->nrow;
  int c = M.s_.cholmodRsparse_->ncol;
  int n = r*c;
  BlasRDense(parts, c+1);
  double* h = (double*)s_.blasRdense_->x; 
  double step = (max-min)/double(parts);
  for(p=0; p<parts-1; p++)
  {
    *(h++) = min+step*(p+1);
  }
  *(h++) = max;
  memset(h,0,sizeof(double)*parts*c);
  const double* x = (const double*)M.s_.blasRdense_->x; 
//double* y = (double*)s_.blasRdense_->x+parts; 
  for(j=0; j<c; j++)
  {
    double* h0 = ((double*)s_.blasRdense_->x)+parts*(j+1);
    for(i=0; i<r; i++, x++)
    {
      p = (int)((*x-min)/step)-1;
      if(p<0) { p=0; }
      if(p>=parts) { p=parts-1; }
      h0[p] += 1.0/r;
    }
  }
};
/* */
////////////////////////////////////////////////////////////////////////////////
  void BVMat::Histogram_bRd(const BVMat& M, int parts)
////////////////////////////////////////////////////////////////////////////////
{
  int k;
  int n = M.s_.blasRdense_->nzmax;
  double* x = (double*)M.s_.blasRdense_->x; 
  double min = BDat::PosInf();
  double max = BDat::NegInf();
  for(k=0; k<n; k++, x++)
  {
    if(*x<min) { min=*x; }
    if(*x>max) { max=*x; }
  }
  Histogram_bRd(M,parts,min,max);
};


////////////////////////////////////////////////////////////////////////////////
  void BVMat::Histogram(const BVMat& M_, int parts, double min, double max)
////////////////////////////////////////////////////////////////////////////////
{
  BVMat *M__=NULL;
  convertIfNeeded_all2bRd(M_, M__, "Histogram");  
  BVMat &M = *M__;
  Histogram_bRd(M,parts,min,max);
  if(M__!=&M_) { delete M__; }
}

////////////////////////////////////////////////////////////////////////////////
  void BVMat::Histogram(const BVMat& M_, int parts)
////////////////////////////////////////////////////////////////////////////////
{
  BVMat *M__=NULL;
  convertIfNeeded_all2bRd(M_, M__, "Histogram");
  BVMat &M = *M__;
  Histogram_bRd(M,parts);
  if(M__!=&M_) { delete M__; }
}
