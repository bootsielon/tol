/* maia.cpp: Functions of BMultAia
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

//#define TRACE_LEVEL 10

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_matrix.h>
#include <tol/tol_bmaia.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bmatfun.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bmatrix.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_btsrgrav.h>
#include <tol/tol_bdatgra.h>


//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BAia).
//--------------------------------------------------------------------
BTraceInit("maia.cpp");

int BMultOutlier::optMaxOrder_ = 1;

static BMultOutlier::BInput _inputConstant_ =
{
 -1,
  BAia::stepOut_,
  0,
  0
};

//--------------------------------------------------------------------
  BMultOutlier::BMultOutlier() 
//--------------------------------------------------------------------
: input_    (optMaxOrder_),
   maxIndex_(-1)
{
  input_.ReallocBuffer(0);
};

//--------------------------------------------------------------------
  bool BMultOutlier::IsDisjoint(const BMultOutlier& a)
//--------------------------------------------------------------------
{
  int i, j;
  bool ok = true;
  for(j=0; ok&&(j<a.input_.Size()); j++)
  {
    ok = _inputConstant_!=a.input_[j];
  }
  for(i=0; ok && (i<input_.Size()); i++)
  {
    for(j=0; ok&&(j<a.input_.Size()); j++)
    {
      ok = input_[i]!=a.input_[j];
    }
  }
  return(ok);
}

//--------------------------------------------------------------------
void  BMultOutlier::Add(int index, BOutlier* outlier, int t, BDat w)
//--------------------------------------------------------------------
{
  int s = input_.Size();
  input_.ReallocBuffer(s+1);
  input_[s].index_   = index;
  input_[s].outlier_ = outlier;
  input_[s].t_       = t;
  input_[s].w0_      = w;
  if(maxIndex_<index) { maxIndex_ = index; }
}


//--------------------------------------------------------------------
void  BMultOutlier::Add(const BMultOutlier& mo)
//--------------------------------------------------------------------
{
  int s1 =    input_.Size();
  int s2 = mo.input_.Size();
  int s  = s1+s2;
  input_.ReallocBuffer(s);
  input_.ReallocBuffer(s1);
  for(int i=0; i<s2; i++) 
  { 
    Add
    (
      mo.input_[i].index_, 
      mo.input_[i].outlier_, 
      mo.input_[i].t_, 
      mo.input_[i].w0_
    );
  }
}

//--------------------------------------------------------------------
void  BMultOutlier::Copy(const BMultOutlier& mo)
//--------------------------------------------------------------------
{
  input_          = mo.input_;
  x_              = mo.x_;
  w_              = mo.w_;
  e_              = mo.e_;
  sigma_          = mo.sigma_;
  resLogDens_     = mo.resLogDens_;
  nonZeroParamLogProb_ = mo.nonZeroParamLogProb_;
  logDens_        = mo.logDens_;
  bic_            = mo.bic_;
  minNonZeroParamProb_ = mo.minNonZeroParamProb_;
}

//--------------------------------------------------------------------
  BText BMultOutlier::GetExpression()
//--------------------------------------------------------------------
{
  int s = input_.Size();
  BText txt = "\nBMultOutlier{";
  for(int i=0; i<s; i++) 
  { 
    txt += BText("\n  ")+
      input_[i].outlier_->GetExpression(input_[i].t_,input_[i].w0_);
  }
  txt+="\n}\n";
  return(txt);
}

static double _logPi_   = log(3.14159265358979323846);
static double _logInv2_ = log(0.5);


//--------------------------------------------------------------------
bool BMultOutlier::Estimate(const BMatrix<BDat>& y, 
                            const BMatrix<BDat>& C, 
                            int n0,
                            BDat optMaxEigenValueRelativeRange_)
/*! Estimates the coefient w0 of this outlier at index t for the
 *  current model mod_ and returns true if the corresponding t-student
 *  statistic value is great or equal than minOutlierLikelihood_
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BOutlier::Estimate");
#endif
  TRACE_SHOW_HIGH(fun,"BEGIN");
  int i;
  int s = input_.Size();
  int n = n0+s;
  int N = y.Rows();
  if(!s||!N||
     (y.Columns()!=1)||
     (C.Columns()!=1)||
     (C.Rows()!=N))
  {
    nonZeroParamLogProb_=BDat::NegInf();
    minNonZeroParamProb_=0;
    return(false);
  }
  assert(N==C.Rows());
  TRACE_SHOW_HIGH(fun,BText("s=")+s+" N="+N+" n="+n);
  BMatrix<BDat> xt(s+1,N);
  for(i=0; i<s; i++) 
  { 
    input_[i].outlier_->Evaluate(input_[i].t_, xt, i, input_[i].sqrSum_);
  }
  for(i=0; i<N; i++) { xt(s,i)=C(i,0); }
  x_ = xt.T();
  w_ = gsl_MinimumResidualsSolve(x_,y);
  e_ = y-x_*w_;

  TRACE_SHOW_TOP(fun,BText("  x=")<<    x_.Name());
  TRACE_SHOW_TOP(fun,BText("  y=")<<    y .Name());
  TRACE_SHOW_TOP(fun,BText("  w=")<<    w_.Name());
  TRACE_SHOW_TOP(fun,BText("  e=")<<    e_.Name());

  sigma_ = StandardDeviation(e_.Data());
  BDat S2 = Moment(e_.Data(),2);
  double N_2 = 0.5*N;
  resLogDens_ = _logInv2_+LogGamma(N_2)-N_2*(_logPi_+Log(S2/2));
  BTStudentDist tProbDist(N-s);
  minNonZeroParamProb_ = BDat::Unknown();
  BSymMatrix<BDat> M;
  BMatrix<BDat> L;
  MTMSquare(x_,M);
  {
    BLowTrMatrix<BDat> L_;
    Choleski(M,L_);
    L = L_;
  }
  TRACE_SHOW_HIGH(fun,BText("  M=\n")+M.Name());
  TRACE_SHOW_HIGH(fun,BText("  L=\n")+L.Name());

  nonZeroParamLogProb_ = BDat::Unknown();
  BDat detL;
  BDat logDetL;
  BDat minEigenVal;
  BDat maxEigenVal;
  BDat eigenValueRelativeRange;
  BMat t(0,0);
  if(L.Rows())
  { 
    logDetL = 0;
    BDat minLogEigenVal;
    BDat maxLogEigenVal;
    for(i=0; i<s; i++) 
    { 
      BDat lev = 2*Log(L(i,i));
      logDetL += lev;
      if(!i) { minLogEigenVal = maxLogEigenVal = lev; }
      else if(lev < minLogEigenVal) { minLogEigenVal = lev; }
      else if(lev > maxLogEigenVal) { maxLogEigenVal = lev; }
    }
    eigenValueRelativeRange = Exp(maxLogEigenVal-minLogEigenVal);
    detL = Exp(logDetL);
    t = L.T()*w_/sigma_;
    TRACE_SHOW_HIGH(fun,BText("  t=\n")+t.Name());
  }
  TRACE_SHOW_HIGH(fun,BText("  logDetL=")+logDetL);
  TRACE_SHOW_HIGH(fun,BText("  DetL=\n")+detL);
  TRACE_SHOW_HIGH(fun,BText("  eigenValueRelativeRange=\n")+eigenValueRelativeRange);
  
  if((w_.Columns()==1)&&
     (t .Columns()==1)&&
     (e_.Columns()==1)&&
     (w_.Rows()==x_.Columns())&&
     (t .Rows()==x_.Columns())&&
     (e_.Rows()==x_.Rows   ())&&
      eigenValueRelativeRange.IsKnown() && 
     (eigenValueRelativeRange<optMaxEigenValueRelativeRange_))
  {
    nonZeroParamLogProb_ = 0;
    for(i=0; i<s; i++) 
    { 
      input_[i].w0_ = w_(i,0);
      input_[i].tStudent_ = input_[i].w0_/(sigma_/Sqrt(input_[i].sqrSum_));
    //BDat tDens = tProbDist.Dens(input_[i].tStudent_);
      BDat tAbs  = Abs(t(i,0));
      BDat tProbNonZero = 1.0-2.0*tProbDist.Dist(-tAbs);
      nonZeroParamLogProb_ += Log(tProbNonZero);
      if(minNonZeroParamProb_.IsUnknown() || (tProbNonZero<minNonZeroParamProb_)) 
      { 
        minNonZeroParamProb_ = tProbNonZero; 
      }
      TRACE_SHOW_HIGH(fun,BText("")+
       " i="+i+
       " tStudent="+t(i,0)+
       " tProbNonZero="+tProbNonZero);
    }
  }
  if(nonZeroParamLogProb_.IsUnknown() ||
     minNonZeroParamProb_.IsUnknown()) 
  { 
    nonZeroParamLogProb_=BDat::NegInf();
    minNonZeroParamProb_=0;
    return(false);
  }
  logDens_  = resLogDens_ + nonZeroParamLogProb_;
  bic_      = -2.0*logDens_ + n*Log(N);
  TRACE_SHOW_HIGH(fun,GetExpression());
  TRACE_SHOW_HIGH(fun,BText("  sigma_=")+sigma_);
  TRACE_SHOW_HIGH(fun,BText("  resLogDens_=")+resLogDens_);
  TRACE_SHOW_HIGH(fun,BText("  nonZeroParamLogProb_=")+nonZeroParamLogProb_);
  TRACE_SHOW_HIGH(fun,BText("  logDens_=")+logDens_);
  TRACE_SHOW_HIGH(fun,BText("  bic_=")+bic_);
  TRACE_SHOW_HIGH(fun,BText("  minNonZeroParamProb_=")+minNonZeroParamProb_);
  TRACE_SHOW_HIGH(fun,"END");

/*
  BLowTrMatrix<BDat> Li;
  BLowTrMatrix<BDat> L;
  BSymMatrix  <BDat> cov;
  BSymMatrix  <BDat> cor;
  BDat s;
  BDat R2;
  BDat F;
  BInt v1;
  BInt   v2;
  BDat alfa;
  BDat resSqSum;
  BDat totSqSum;
  LinearRegression(z_,x_,w_,e_,Li,L,cov,cor,
                   s,R2,F,v1,v2,alfa,resSqSum,totSqSum)
*/
  return(true);
}


//--------------------------------------------------------------------
bool BMultOutlier::Estimate(const BMultAia& aia)

/*! Estimates the coefient w0 of this outlier at index t for the
 *  current model mod_ and returns true if the corresponding t-student
 *  statistic value is great or equal than minOutlierLikelihood_
 */
//--------------------------------------------------------------------
{
  return(Estimate(aia.y_, aia.constant_, aia.n_,
                  aia.optMaxEigenValueRelativeRange_));
}

//--------------------------------------------------------------------
int BMultOutlier::CompareBicPtr(const void* v1, const void* v2)

/*! Compair the bic_ field of two pointers to BMultOutlier
 */
//--------------------------------------------------------------------
{
  BMultOutlier* mo1 = *((BMultOutlier**)v1);
  BMultOutlier* mo2 = *((BMultOutlier**)v2);
  double bicDif = (mo1->bic_-mo2->bic_).Value();
  return((bicDif<0)?-1:(bicDif>1)?1:0);
}


//--------------------------------------------------------------------
BMultAia::BMultAia(      BUserTimeSerie*    res,
                   const BRat&              rat,
                   const BArray<BOutlier*>& userOutliers,
                         int                optMaxOrder,
                         int                optMaxCrossNum,
                         BDat               optMinNonZeroParamProb,  
                         BDat               optMaxEigenValueRelativeRange)
    
/*! BMultAia constructor
 */
//--------------------------------------------------------------------
: BAia                          (),
  optMaxOrder_                  (optMaxOrder),
  optMaxCrossNum_               (optMaxCrossNum),
  optMinNonZeroParamProb_       (optMinNonZeroParamProb),
  optMaxEigenValueRelativeRange_(optMaxEigenValueRelativeRange)
{
  res_ = res;
  rat_ = rat;
  userOutliers_ = userOutliers;
  input_=NIL;
  BMultOutlier::optMaxOrder_ = optMaxOrder;
  Initialize();
}

//--------------------------------------------------------------------
void BMultAia::SetResiduals(const BArray<BDat>& y)

/*! Put aia_ member to this for each used BOutlier.
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BOutlier::SetResiduals");
#endif
  assert(y.Size()==res_->Data().Size());
  avr_   = Average(y);
  for(int i=0; i<N_; i++) { y_(i,0) = y[i]; }
  res_->GetDataBuffer() = y_.Data(); 
  double N_2    = 0.5*N_;
  sigma_        = StandardDeviation(y_.Data());
  resLogDens_   = _logInv2_+LogGamma(N_2)-N_2*(_logPi_+2.0*Log(sigma_));
  nonZeroParamLogProb_ = 0;
  minNonZeroParamProb_ = 1;
  logDens_      = resLogDens_ + nonZeroParamLogProb_;
  bic_          = -2.0*logDens_ + BDat(n_)*Log(N_);
  TRACE_SHOW_LOW(fun,BText(" N=")+N_+
                           " n=" +n_+
                           " avr=" +avr_+
                           " sigma=" +sigma_+
                           " resLogDens=" +resLogDens_+
                           " bic=" +bic_);
}

//--------------------------------------------------------------------
void BMultAia::Initialize()

/*! Put aia_ member to this for each used BOutlier.
 */
//--------------------------------------------------------------------
{
  BInt o;
  BDat f   = (*res_)[res_->FirstDate()]; //Forzes residuous calculation
  N_ = res_->Data().Size();
  n_ = 0;
//Std(BText("\nResiduous dates ")+res_->FirstDate().Name()+", "+
//          res_->LastDate ().Name()+", "+N_);
  y_.Alloc(N_,1);
  SetResiduals(res_->Data());

//Std("\nUsing outlier's types: ");
  BAia::stepOut_->PutAia(this);
  for(o=0; o<userOutliers_.Size(); o++)
  {
    if(userOutliers_[o]!=BAia::stepOut_)
    {
      userOutliers_[o]->PutAia(this);
    }
  //Std(BText("\n  ")+userOutliers_[o]->Name());
  }
  BDat sqrSum;
  BMatrix<BDat> ct(1,N_);
  BAia::stepOut_->Evaluate(0,ct,0,sqrSum);
  constant_=ct.T();
}


//--------------------------------------------------------------------
void BMultAia::RecalcResiduous(const BMultOutlier& mo)

/*! Substract the effect
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BOutlier::RecalcResiduous");
#endif
  int s = mo.input_.Size();
  n_+=s;
  BMatrix<BDat> res = mo.e_;//+mo.w_(s,0);
  SetResiduals(res.Data());
/*
  sigma_        = mo.sigma_;
  resLogDens_   = mo.resLogDens_;
  nonZeroParamLogProb_ = mo.nonZeroParamLogProb_;
  minNonZeroParamProb_ = mo.minNonZeroParamProb_;
  logDens_      = mo.logDens_;
  bic_          = mo.bic_; 
*/
}


//--------------------------------------------------------------------
bool BMultAia::SearchMaxAbsRes(BArray<BMaxRes>& mr)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BAia::SearchMaxAbsRes");
#endif
  if(optMaxOrder_>=N_) { return(false); }
  mr.AllocBuffer(optMaxOrder_);
  int i,j,t;
  for(i=0; i<optMaxOrder_; i++)
  {
    mr[i].t_      = -1;
    mr[i].absRes_ =  0; 
  }
  for(t=0; t<N_; t++)
  {
    BDat absRes = Abs(y_(t,0));
    TRACE_SHOW_HIGH(fun,BText("   ")+absRes);
    for(i=0; i<optMaxOrder_; i++)
    {
      if(absRes>mr[i].absRes_)
      {
        for(j=optMaxOrder_-1; j>i; j--)
        {
          mr[j].absRes_ = mr[j-1].absRes_;
          mr[j].t_      = mr[j-1].t_;
        }
        mr[i].absRes_ = absRes;
        mr[i].t_      = t;
        break;
      }
    }
  }
  bool found = false;
  for(i=0; i<optMaxOrder_; i++)
  {
    if(mr[i].t_>=0) { found=true; }
    TRACE_SHOW_LOW(fun,BText("MaxAbsRes[")+i+","+mr[i].t_+"]="+mr[i].absRes_);
  }
  return(found);
}

//--------------------------------------------------------------------
bool BMultAia::SearchRefObs(BArray<BMultOutlier>& mo)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BAia::SearchRefObs");
#endif
  int maxNearRefObs = 3;
  int i,j,k,r;
  BArray<BMaxRes> mr;
  bool ok = SearchMaxAbsRes(mr);
  mo.ReallocBuffer(mr.Size()*userOutliers_.Size()*maxNearRefObs);
  for(i=r=0; i<mr.Size(); i++)
  {
    for(j=0; j<userOutliers_.Size(); j++)
    {
      int m = userOutliers_[j]->nearMaxAbs_.Size();
      if(maxNearRefObs<m) { m = maxNearRefObs; }
      for(k=0; k<m; k++)
      {
        if(mr[i].t_>=userOutliers_[j]->nearMaxAbs_[k])
        {
          BMultOutlier& mor = mo[r];
          int t = mr[i].t_-userOutliers_[j]->nearMaxAbs_[k];
          mor.Add(r,userOutliers_[j],t,0);
          r++;
        }
      }
    }
  }
  mo.ReallocBuffer(r);
  TRACE_SHOW_MEDIUM(fun,BText("Number of candidates to single outlier=")+r);
  return ok;
};

//--------------------------------------------------------------------
bool BMultAia::SearchNextMultOutlier(BMultOutlier& minMo)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BAia::SearchNextMultOutlier");
#endif
  BComparingInfo minimum;
  minimum.o_=-1;
  minimum.h_=-1;
  minimum.bic_=bic_;
  BArray<BMultOutlier>moAux_0;
  bool ok = bic_.IsKnown() && SearchRefObs(moAux_0);
  if(!ok) { return(false); }
  ok = false;
  int K = moAux_0.Size();
  BArray< BArray<BMultOutlier> > mo(optMaxOrder_);
  int i,j,k,J;
  int o=0;
  TRACE_SHOW_MEDIUM(fun,BText("K=")+K);
  TRACE_SHOW_MEDIUM(fun,BText("PHASE 1.a: Estimating single outliers"));
  TRACE_SHOW_HIGH(fun,BText("minimum.bic_=")+minimum.bic_);
  for(i=k=0; i<optMaxOrder_; i++)
  {
    for(j=0; j<userOutliers_.Size(); j++, k++)
    {
      BMultOutlier& aux = moAux_0[k];
      if(aux.Estimate(*this)&&
         (aux.minNonZeroParamProb_>optMinNonZeroParamProb_) && 
         (minimum.bic_>aux.bic_)) 
      { 
        minimum.bic_=aux.bic_; 
      }
      TRACE_SHOW_HIGH(fun,BText("minimum.bic_[")+i+","+j+","+k+"]="+minimum.bic_);
    }
  }
  TRACE_SHOW_MEDIUM(fun,BText("PHASE 1.b: Filtering most probable single outliers (a posteriori)"));
  BDat bound = bic_;
  TRACE_SHOW_MEDIUM(fun,BText("posterior bound=")+bound);
  mo[o].AllocBuffer(K);
  for(k=j=0; k<K; k++)
  {
    if(moAux_0[k].bic_<=bound) 
    { 
      BMultOutlier& mooj = mo[o][j];
      mooj.Copy(moAux_0[k]);
      TRACE_SHOW_HIGH(fun,BText("Selected[")+o+","+j+"] "
                      "with BIC="+mooj.bic_+" "+
                      mooj.GetExpression());
      if((mooj.bic_==minimum.bic_)&&
         (mooj.minNonZeroParamProb_>optMinNonZeroParamProb_))
      {
        ok = true;
        minimum.o_=o;
        minimum.h_=j;
        TRACE_SHOW_MEDIUM(fun,BText("current minimum changed to[")+o+","+j+"] "
                          "with BIC="+mooj.bic_+" "+
                          "and minNonZeroParamProb_="+mooj.minNonZeroParamProb_+" "+
                          mooj.GetExpression());

      }
      j++;
    }
  }
  mo[o].ReallocBuffer(J=j);
  TRACE_SHOW_MEDIUM(fun,BText("  preselected single outliers:")+J+" of "+K);
  for(o=1; o<optMaxOrder_; o++)
  {
    TRACE_SHOW_MEDIUM(fun,BText("PHASE ")+(o+1)+".1: Filtering most probable "+(o+1)+"-tuples of outliers (a priori)");
    int H = K*mo[o-1].Size();
    BArray<BMergingInfo>mi(H);
    BDat minBicAux;
    for(i=k=0; i<mo[o-1].Size(); i++)
    {
      for(j=mo[o-1][i].maxIndex_+1; j<K; j++, k++)
      {
        mi[k].i_=i;
        mi[k].j_=j;
        mi[k].k_=k;
        mi[k].isDisjoint_ = mo[o-1][i].IsDisjoint(moAux_0[j]);
        if(mi[k].isDisjoint_)
        {
          mi[k].bic_=mo[o-1][i].bic_+moAux_0[j].bic_;
          if(minBicAux.IsUnknown() || (minBicAux>mi[k].bic_)) { minBicAux = mi[k].bic_; }
          TRACE_SHOW_HIGH(fun,BText("bic[")+i+","+j+","+k+"]="+
                          mo[o-1][i].bic_+"+"+moAux_0[j].bic_+"="+mi[k].bic_+"; "+
                          "minBicAux="+minBicAux);
        }
        else
        {
          mi[k].bic_=BDat::PosInf();
        }
      }
    }
    BDat boundAux = minBicAux;
    TRACE_SHOW_MEDIUM(fun,BText("  wellformed tuples:")+k+" of "+H);
    TRACE_SHOW_MEDIUM(fun,BText("  prior bound=")+boundAux);
    TRACE_SHOW_MEDIUM(fun,BText("PHASE ")+(o+1)+".2: Estimating selected "+(o+1)+"-tuples of outliers");
    BArray<BMultOutlier*>moAux_o(H);
    for(k=j=0;k<H; k++)
    {
    //if(mi[k].bic_<=boundAux) 
      { 
        moAux_o[j] = new BMultOutlier;
        BMultOutlier& aux  = *moAux_o[j];
        BMultOutlier& aux1 = mo[o-1][mi[k].i_];
        BMultOutlier& aux2 = moAux_0[mi[k].j_];
        aux.Copy(aux1); 
        aux.Add (aux2); 
        aux.Estimate(*this);
        if((aux.minNonZeroParamProb_>optMinNonZeroParamProb_) && 
           (minimum.bic_>aux.bic_)) 
        { 
          minimum.bic_ = aux.bic_; 
        }
        j++;
      }
    }
    moAux_o.ReallocBuffer(H=j);
    if(H>optMaxCrossNum_)
    {
      moAux_o.Sort(BMultOutlier::CompareBicPtr);
      H = optMaxCrossNum_;
    }
    bound = minimum.bic_;
    TRACE_SHOW_MEDIUM(fun,BText("  estimated tuples:")+H+" of "+moAux_o.Size());
    TRACE_SHOW_MEDIUM(fun,BText("  posterior bound=")+bound);
    TRACE_SHOW_MEDIUM(fun,BText("PHASE ")+(o+1)+".3: Filtering most probable among "+(o+1)+"-tuples of outliers (a posteriori)");
    mo[o].AllocBuffer(H);
    for(k=j=0;k<H; k++)
    {
      if(minimum.bic_<=bound) 
      { 
        BMultOutlier& mooj = mo[o][j];
        mooj.Copy(*moAux_o[k]);
        TRACE_SHOW_HIGH(fun,BText("Selected[")+o+","+j+"] "
                        "with BIC="+mooj.bic_+" "+
                        mooj.GetExpression());
        if((mooj.bic_==minimum.bic_)&&
           (mooj.minNonZeroParamProb_>optMinNonZeroParamProb_)) 
        {
          ok = true;
          minimum.o_=o;
          minimum.h_=j;
          TRACE_SHOW_MEDIUM(fun,BText("current minimum changed to [")+o+","+j+"] "
                            "with BIC="+mooj.bic_+" "+
                            "and minNonZeroParamProb_="+mooj.minNonZeroParamProb_+" "+
                            mooj.GetExpression());
        }
        j++;
      }
    }
    for(j=0; j<moAux_o.Size(); j++) { delete moAux_o[j]; }

    TRACE_SHOW_MEDIUM(fun,BText("  selected tuples:")+j+" of "+H);
    mo[o].ReallocBuffer(j);
  }
  minMo.Copy(mo[minimum.o_][minimum.h_]);
  BDat minBound = bic_+Log(.10);
  if(ok)
  {
    TRACE_SHOW_LOW(fun,BText("Outliers tuple FOUND ")+minMo.GetExpression());
    TRACE_SHOW_LOW(fun,BText("  sigma_=")+minMo.sigma_);
    TRACE_SHOW_LOW(fun,BText("  resLogDens_=")+minMo.resLogDens_);
    TRACE_SHOW_LOW(fun,BText("  nonZeroParamLogProb_=")+minMo.nonZeroParamLogProb_);
    TRACE_SHOW_LOW(fun,BText("  minNonZeroParamProb_=")+minMo.minNonZeroParamProb_);
    TRACE_SHOW_LOW(fun,BText("  logDens_=")+minMo.logDens_);
    TRACE_SHOW_LOW(fun,BText("  bic_=")+minMo.bic_);
    TRACE_SHOW_LOW(fun,BText("  minimum.bic_=")+minimum.bic_+
                             "  minBound="+minBound+
                             "  ok="+(int)ok);
  }
  else
  {
    TRACE_SHOW_LOW(fun,BText("Outliers tuple NOT FOUND "));
    TRACE_SHOW_LOW(fun,BText("  bic_=")+minMo.bic_);
    TRACE_SHOW_LOW(fun,BText("  minimum.bic_=")+minimum.bic_+
                             "  minBound="+minBound+
                             "  ok="+(int)ok);
  }
  return(ok);
}

//#define BMAIA_REESTIMATE_ITER

//--------------------------------------------------------------------
BList* BMultAia::Input()
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("  BAia::Input");
#endif
  if(!input_)
  {
    BInt i, iter;
    BMultOutlier minMo;
    BMatrix<BDat> y0 = y_;
    for(iter=0; iter<N_-2; iter++)
    { 
      if(n_+optMaxOrder_>=N_) { break; }
      TRACE_SHOW_LOW(fun,BText("Iteration [")+iter+"]");
      if(!SearchNextMultOutlier(minMo)) { break; }
      assert(minMo.input_.Size());
      if(!iter)
      {
        mo_.Copy(minMo);
      }
      else
      {
        if(!mo_.IsDisjoint(minMo)) { break; }
        mo_.Add(minMo);
        #ifdef BMAIA_REESTIMATE_ITER
        mo_.Estimate(constant_, y0, 0);
        #endif
        TRACE_SHOW_LOW(fun,BText("Cumulated outliers : ")+mo_.GetExpression());
      }
      n_ = mo_.input_.Size();
      #ifdef BMAIA_REESTIMATE_ITER
      SetResiduals(mo_.e_.Data());
      #else
      SetResiduals(minMo.e_.Data());
      #endif
      if(sigma_<=Sqrt(DEpsilon())) { break; }
    }
    for(i=0; i<mo_.input_.Size(); i++)
    {
      BMultOutlier::BInput& input = mo_.input_[i];
      BDat w = input.w0_; 
      BSyntaxObject* inp = input.outlier_->GetInputDef(input.t_,w);
      assert(inp);
      input_ = Cons(inp, input_);
    }
  }
  return(input_);
}

