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
  double result=(size>nzmax)?0.0:x[0];
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      if(result<x[k]) { result=x[k]; }
    }
  }
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
  double result=(size>nzmax)?0.0:x[0];
  for(k=0; k<nzmax; k++)
  {
    if(!IS_NAN(x[k]))
    {
      if(result>x[k]) { result=x[k]; }
    }
  }
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
  cholmod_dense* dense = cholmod_allocate_dense
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
  cholmod_triplet* triplet = cholmod_allocate_triplet
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
    s_.chlmRsparse_= cholmod_triplet_to_sparse(trp, trp->nnz, common_);
    cholmod_free_triplet(&trp, common_);
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
  double length = lower-upper;
  int length_sign = gsl_fcmp(length+1.0,1.0,vmat_trunc_eps);
  if(length_sign>=0)
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
                                      BVMat& z)
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
      //Std(BText("\n  D[")+i+","+j+"]="+BDat(Dij)+"\tz["+j+"]="+BDat(zj)+"\tb["+i+"]="+BDat(bx[i])+"\td["+i+"]="+BDat(dx[i]));
        if(Dij_sign<0) 
        {
          aux = bx[i]/Dij;
          if(lower<aux) { lower = aux; }
        //Std(BText("\n  aux=")+BDat(aux)+"\tlower="<<BDat(lower));
        }
        else //if(Dij_sign>0) 
        {
          aux = bx[i]/Dij;
          if(upper>aux) { upper = aux; }
        //Std(BText("\n  aux=")+BDat(aux)+"\tupper="<<BDat(upper));
        }
      }
    }
  //Std(BText("\nlower=")<<BDat(lower)<<"\tupper="<<BDat(upper));
    double length = lower-upper;
    int length_sign = gsl_fcmp(length+1.0,1.0, vmat_trunc_eps);
    if(length_sign>=0)
    {
      if(length_sign==0)
      {
        lower = upper;
      }
      else
      {
        warn_cannot_apply("(I) TruncStdGaussian",
          I2("(empty interval",
          "(intervalo vacio")+" ["+BDat(lower).Format("%.16lg")+","+
                                   BDat(upper).Format("%.16lg")+"])",z);
        return(-1);
      }
    }
# ifdef USE_BTruncatedNormalDist
    if(lower>=upper)
    {
      zj = lower;
    }
    else if((lower>negInf)||(upper<posInf))
    {
      BTruncatedNormalDist tn(lower,upper);
      zj = tn.Random().Value();
      assert((lower<=zj) && (zj<=upper));
    //Std(BText("\nBTruncatedNormalDist z[")+j+"]="+zj);
    }
    else
    {
      zx[j] = BNormalDist::Random01().Value();
    //Std(BText("\nBNormalDist z[")+j+"]="+zj);
    }
# else
    zj = gsl_rtabnorm_combo(BProbDist::rng(), 0.0, 1.0,
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
                               const BVMat& z0, int ncol, int burnin)
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
  if(burnin<=0 ) { burnin = 1; }
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
      if(trunc_std_gaussian(D, d, b, z)!=0) { return; }  
      memcpy(aux, z.s_.blasRdense_->x, size_row);
    }
    aux = (double*)s_.blasRdense_->x;
    for(k=0; k<ncol; k++, aux+=nrow)
    {
      if(trunc_std_gaussian(D, d, b, z)!=0) { return; }  
      memcpy(aux, z.s_.blasRdense_->x, size_row);
    }
    break;
  default: 
    err_cannot_create(fName,D.code_);  }
};


