/* moddiag.cpp: Functions of BModel
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

#include <tol/tol_bmodel.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bsymmat.h>
#include <tol/tol_bltrimat.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bmultvar.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bar.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BModel).
//--------------------------------------------------------------------
BTraceInit("diag.cpp");

static BArray<BProbDist*> decl_1_;


//--------------------------------------------------------------------
BInt BModel::Qualify(BInt n, BDat x, BBool necessary)

/*! Returns the qualify of n-th test with result x if it's neccesary.
 *  For each test there are an accept bound and a refuse bound that
 *  divides the [0,1] interval in three ordered intervals :
 *
 *  [0,Accet], (Accept, Refuse), [Refuse, 1]
 *
 *  The qualify obtained is GOOD if x is in the first interval,
 *  ACCEPTABLE if it's in the second one or REFUSED if it's in the
 *  third one.
 */
//--------------------------------------------------------------------
{
  if(necessary)
  {
    if(testAccept_(n) <   testRefuse_(n))
    {
      if     (x<=testAccept_(n)) { diagQualify_(n) = BDIAGGOOD;  }
      else if(x<=testRefuse_(n)) { diagQualify_(n) = BDIAGACCEPT; }
      else                       { diagQualify_(n) = BDIAGREFUSE; }
    }
    else
    {
      if     (x>=testAccept_(n)) { diagQualify_(n) = BDIAGGOOD;  }
      else if(x>=testRefuse_(n)) { diagQualify_(n) = BDIAGACCEPT; }
      else                       { diagQualify_(n) = BDIAGREFUSE; }
    }
    diagPunct_(n) = x;
  }
  else
  {
    diagQualify_(n) = BDIAGUNNECESSARY;
    diagPunct_   (n) = 0;
  }
  return(diagQualify_(n));
}

//--------------------------------------------------------------------
static BDat AbsNormal(BDat x)
//--------------------------------------------------------------------
{
  return(2*BNormalDist::Dist01(Abs(x)) - BDat(1));
}


//--------------------------------------------------------------------
BInt BModel::FstRegAutCorTest(BInt n)

/*! Runs the test of first regular autocorrelation
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE FstRegAutCorTest "));
  BInt m = 1;
  BDat x,y;
  if(m<resACor_.Rows()) { y = resACor_(m-1,1)*Sqrt(N_); }
  diagValue_(n) = y;
  x = AbsNormal(y);
  return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::FstSeaAutCorTest(BInt n)

/*! Runs the test of first seasonal autocorrelation
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE FstSeaAutCorTest "));
  BInt m = periodicity_;
  BDat x,y;
  BBool necessary = (periodicity_>1);
  if(necessary)
  {
    if(m<resACor_.Rows()) { y = resACor_(m-1,1)*Sqrt(N_); }
    diagValue_(n) = y;
    x = AbsNormal(y);
  }
  return(Qualify(n,x,necessary));
}

//--------------------------------------------------------------------
BInt BModel::SndRegAutCorTest(BInt n)

/*! Runs the test of second regular autocorrelation
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE SndRegAutCorTest "));
  BInt m = 2;
  BDat x,y;
  if(m<resACor_.Rows()) { y = resACor_(m-1,1)*Sqrt(N_); }
  diagValue_(n) = y;
  x = AbsNormal(y);
  return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::SndSeaAutCorTest(BInt n)

/*! Runs the test of second seasonal autocorrelation
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE SndSeaAutCorTest "));
  BInt m = 2*periodicity_;
  BDat x,y;
  BBool necessary = (periodicity_>1);
  if(necessary)
  {
    if(m<resACor_.Rows()) { y = resACor_(m-1,1)*Sqrt(N_); }
    diagValue_(n) = y;
    x = AbsNormal(y);
  }
  return(Qualify(n,x,necessary));
}


//--------------------------------------------------------------------
BInt BModel::BoxPierceLjungTest(BInt n)

/*! Runs the test of the Box-Pierce-Ljung statistic of residuals
 *  autocorrelations
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE BoxPierceLjungTest "));
  BInt  m = aCorNum_;
  BInt  p = arParam_;
  BInt  q = maParam_;
  BoxPierceLjung_ = BoxPierceLjungACF(resACor_.Data(), m, N_);
  BChiSquareDist Chi(m-p-q);
  diagValue_(n) = BoxPierceLjung_;
  BDat x =  Chi.Dist(BoxPierceLjung_);
//Std(BText("\nTRACE BoxPierceLjungTest x=")+x);
//Std(BText("\nBox-Pierce-Ljung = ")+BoxPierceLjung_.Name());
//Std(BText("\nProbability    = ")+x.Name());
  return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::KullbackLeiblerDistanceTest(BInt n)

/*! Runs the test of Kullback-Leibler distance to normal of residuous
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE KullbackLeiblerDistanceTest "));
  static BReal paso = 0.25;
  static BDat  KullbackLeiblerAvr  = -5.349629861;
  static BDat  KullbackLeiblerStDs =  3*0.544121702;

  BArray<BDat> res01 = A_.Data();
  BInt         i,j;
  BDat         max=0;
  for(i=0; i<res01.Size(); i++)
  {
    res01[i] /= standardError_;
    res01[i].Abs();
    if(max < res01[i]) { max = res01[i]; }
  }
  BInt numInterv = BInt(max.Value()/paso);
  BArray<BDat> freq01(numInterv);
  for(j=0; j<freq01.Size(); j++) { freq01[j] = 0; }
  for(i=0; i<res01 .Size(); i++)
  {
    BInt k = (BInt)Floor(res01[i]/paso).Value();
    if(k>=freq01.Size()) { k=freq01.Size()-1; }
    freq01[k] += 1;
  }
  BDat epsilon = 0;
//BDat from  = 0;
  for(j=0; j<numInterv; j++)
  {
    BReal from  = paso*j;
    BReal until = paso*(j+1);
    BDat pt    = 2*(BNormalDist::Dist01(until)-BNormalDist::Dist01(from));
    BDat pe    = freq01[j]/res01.Size();
    BDat delta = pe*Log(pt/pe);
    if(delta.IsKnown()) { epsilon -= delta.Value(); }
    from = until;
  }
  BDat x = (Log(epsilon)-KullbackLeiblerAvr)/KullbackLeiblerStDs;
  x = AbsNormal(x);

//Std(BText("\nNumber of intervals   = ")+numInterv);
//Std(BText("\nKullback-Leibler Distance = ")+epsilon.Name());
//Std(BText("\nProbability     = ")+x.Name());
  return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::FisherDistanceTest(BInt n)

/*! Runs the test of Fisher distance to normal of residuous.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE FisherDistanceTest "));
  BInt numInterv = BInt(Sqrt(N_));
//Std(BText("\nnumInterv=")+numInterv);
  
  BChiSquareDist Chi(numInterv);
  BArray<BDat>  res01  = A_.Data();
  BMatrix<BDat> freq01;
  BInt         i,j;
  for(i=0; i<res01.Size(); i++) { res01[i] = res01[i]/standardError_; }
  Frequency(res01, freq01, numInterv);
  BDat epsilon = 0;
  BDat from    = 0;
  for(j=0; j<numInterv; j++)
  {
    BDat until = 1;
    if(j!=numInterv-1) { until = BNormalDist::Dist01(freq01(j,0)); }
    BDat ft    = res01.Size()*(until-from);
    BDat fe    = freq01(j,1);
    BDat d     = ((fe-ft)*(fe-ft))/ft;
    if(d.IsKnown()) { epsilon += d; }
//  Std(BText("\nj=")+j+"\td="+d.Name());
    from  = until;
  }
  diagValue_(n) = epsilon;
//Std(BText("\nepsilon")=epsilon.Name());
  BDat x;
  if(epsilon.IsKnown()) { x = Chi.Dist(epsilon); }
//Std(BText("\nNumber of intervals = ")+numInterv);
//Std(BText("\Fisher Distance     = ")+epsilon.Name());
//Std(BText("\nProbability     = ")+x.Name());
  return(Qualify(n,x,BTRUE));
}


//--------------------------------------------------------------------
BInt BModel::MinSignificationTest(BInt n)

/*! Runs the test of parameter's signification
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE MinSignificationTest "));
  BTStudentDist T(N_-numParam_);
  BInt i;
  BDat min=BDat::PosInf();
  for(i=0; i<numParam_; i++)
  {
    BDat t = Abs(param_[i]/paramSD_[i]);
    if(!t.IsKnown())
    {
      diagValue_(n) = t;
      return(Qualify(n,1,BTRUE));
    }
    else if(min > t)
    {
      min = t;
      diagValue_(n) = min;
    }
  }
  BDat refuseProb = 2*(1-T.Dist(min)); 
//Std(BText("\nMinimum signification = ")+min.Name());
//Std(BText("\nProbability     = ")+x.Name());
  return(Qualify(n,refuseProb,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::MaxCorrelationTest(BInt n)

/*! Runs the test of parameter's correlations
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE MaxCorrelationTest "));
  BInt  N = paramCor_.Rows();
  if(!N || !paramCor_.Columns())
  {
    diagValue_(n) = BDat::Unknown();
    return(Qualify(n,1,BTRUE));
  }
  BDat  max=0;
  for(BInt i=0; i<N; i++)
  {
    for(BInt j=0; j<i; j++)
    {
      BDat c = Abs(paramCor_(i,j));
      if(max < c)
      {
        diagValue_(n) = c;
        max = c;
      }
    }
  }
  return(Qualify(n,max,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::MixedSignCorrTest(BInt n)

/*! Runs the test of multicolinearity based on minimum significating 
 *  combination of parameters using eigenvector of minimum eigenvalue 
 *  of Jacobian.
 *   
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE MixedSignCorrTest "));
  BTStudentDist T(N_-numParam_);
  BBool necessary = true;
  if(!numParam_ || (D_.Rows()!=numParam_)||(D_.Columns()!=numParam_))
  {
    diagValue_(n) = BDat::Unknown();
    return(Qualify(n,1,BTRUE));
  }
  BDat c = 0;
  int i;
  for(i=0;i<numParam_;i++)
  {
    c+=param_[i]*V_(i,numParam_-1);
  }
  BDat d = D_(numParam_-1,numParam_-1);
  BDat t = d*c/standardError_;
  diagValue_(n) = t;
  BDat refuseProb = 2*(1-T.Dist(Abs(t))); 
  return(Qualify(n,refuseProb,BTRUE));
}



//--------------------------------------------------------------------
BInt BModel::UnitRootsProbTest(BInt n)

/*! Runs the test of unitary roots for ARMA polynomials
 */
//--------------------------------------------------------------------
{
//Std(BText("\nTRACE UnitRootsProbTest "));
  BDat x = 0;
  BInt num = arParam_+maParam_;
  BBool necessary = num!=0; 
  if(necessary)
  {
    if(!paramCor_.Rows() || !paramCor_.Columns())
    {
      diagValue_(n) = BDat::Unknown();
      return(Qualify(n,1,BTRUE));
    }
//  Std(BText("\nARMA Parameters Number = ")+num);
    int facNum = arFactors_.Size();
    BInt m = paramCor_.Rows() - num;
//  Std(BText("\nNON ARMA Parameters Number = ")+m);
    BInt i, j, k, iter;
    BSymMatrix<BDat> cov (num);
    BMatrix<BDat> param(num,1);
    BArray<BDat> unarys(2*facNum);
    BArray<BPol> factors(2*facNum);

    for(j=0;j<facNum;j++)
    {
      factors[j]        = arFactors_[j];
      factors[j+facNum] = maFactors_[j];
      unarys [j]        = 0;
      unarys [j+facNum] = 0;
    };

    i=0;
//  Std(BText("\nFactors =\n"));
    for(i=j=0; j<2*facNum; j++)
    {
//    Std(factors[j].Name());
      for(k=1; k<factors[j].Size(); k++)
      {
        param(i++,0)=-factors[j][k].Coef();
      }
    }
//  Std(BText("\nParam =\n")+param.Name());
    for(i=0; i<num; i++)
    {
      cov(i,i) = paramSD_(m+i)*paramSD_(m+i);
      for(j=0; j<i; j++)
      {
        cov(i,j) = paramSD_(m+i)*paramSD_(m+j)*paramCor_.Get(m+i,m+j);
      }
    }

    BNormalDist n01(0,1);
    BMultivarDist dist(&n01,param,cov);

    BInt maxIter = 1000;
    for(iter=1; iter<=maxIter; iter++)
    {
      BMatrix<BDat> simula = dist.Random();
      for(i=j=0; j<2*facNum; j++)
      {
        for(k=1; k<factors[j].Size(); k++,i++)
        { factors[j][k].PutCoef(-simula(i,0)); }
        if(!(IsStationary(factors[j]))) { unarys[j]+=1; }
      }
    }
    x = 0;
    for(j=0; j<2*facNum; j++)
    {
  //  Std(BText("unarys[")+j+"] = " + unarys[j]);
      unarys[j] /= maxIter;
      if(x<unarys[j]) { x = unarys[j]; }
    }
  /*
    Std(BText("\nRegular  AR unary rooots probability = ")+unarys[0].Name());
    Std(BText("\nSeasonal AR unary rooots probability = ")+unarys[1].Name());
    Std(BText("\nRegular  MA unary rooots probability = ")+unarys[2].Name());
    Std(BText("\nSeasonal MA unary rooots probability = ")+unarys[3].Name());
  */
    diagValue_(n) = x;
  }
  return(Qualify(n,x,BTRUE));
}


//--------------------------------------------------------------------
BInt BModel::RunTest(BInt n)

/*! Runs the n-th test test
 */
//--------------------------------------------------------------------
{
//Std(Out()+"\n\nRunning test "+n+" " +testTitle_(n));
  switch(n)
  {
    case  0 : return(FstRegAutCorTest      (n));
    case  1 : return(FstSeaAutCorTest      (n));
    case  2 : return(SndRegAutCorTest      (n));
    case  3 : return(SndSeaAutCorTest      (n));
    case  4 : return(BoxPierceLjungTest    (n));
    case  5 : return(FisherDistanceTest    (n));
    case  6 : return(MinSignificationTest  (n));
    case  7 : return(MaxCorrelationTest    (n));
    case  8 : return(MixedSignCorrTest     (n));
    case  9 : return(UnitRootsProbTest     (n));
    default : return(-1);
  }
}

static BDat doDiagnostics_ = 1;

BDat& DoDiagnostics () { return(doDiagnostics_); }

//--------------------------------------------------------------------
void BModel::Diagnostics()

/*! Runs all tests
 */
//--------------------------------------------------------------------
{
  if(!doDiagnostics_.Value()) { return; }
  BTimer tms("\nDiagnostics");
  BUserSet* uBounds = USet(GraSet()->FindVariable("DiagnosticsBounds"));
  if(!uBounds)
  {
    Error(I2("Cannot find Set DiagnosticsBounds.",
             "No se encuentra el conjunto DiagnosticsBounds."));
    return;
  }
  BSet& set = Set(uBounds);
  if(testTitle_.Size()!=set.Card())
  {
    Error(I2("Wrong number of elements in Set DiagnosticsBounds.",
             "Error en el número de elementos del conjunto "
             "DiagnosticsBounds."));
    return;
  }
  qualification_ = 0;
  arithmeticQualification_ = 0;
  int n=0, m=1;
  for(; n<testTitle_.Size(); n++, m++)
  {
    BSet& s = Set(set[m]);
    BText name = set[m]->Name();
    if(name=="RegBoxPierceLjungBounds")
    {
      name = "PierceLjungBounds";
    }
    else if(name=="SeaBoxPierceLjungBounds")
    {
      m++;
      s = Set(set[m]);
      name = set[m]->Name();
      continue;
    }
    if(name!=testName_[n])
    {
      Error(I2("Error in name of","Error en el nombre del")+
            " "+(n+1)+"-"+
            I2("th element of","ésimo elemento de")+ 
            " Set DiagnosticsBounds "+
            I2("that would be","que debería ser")+
            " '"+testName_[n]+"' "+
            I2("instead of","en lugar de")+
            " '"+name+"'."   );
    }
    diagValue_  (n) = BDat::Unknown();
    diagPunct_  (n) = 0;
    testAccept_ (n) = Dat(s[1]);
    testRefuse_ (n) = Dat(s[2]);
    diagQualify_(n) = RunTest(n);
    if(qualification_<diagQualify_(n)) { qualification_=diagQualify_(n); }
    BDat x0 = 1.0/3.0;
    BDat y0 = 2.0/3.0;
    BDat x  = testAccept_ (n);
    BDat y  = testRefuse_ (n);
    BDat p  = diagPunct_(n);
    BDat q;
         if(p<=x) { q = p * x0/x; }
    else if(p<=y) { q = x0 + (p-x) * (y0-x0)/(y-x); }
    else          { q = y0 + (p-y) * ( 1-y0)/(1-y); }
    arithmeticQualification_ += q;
/*
    Std( BText("\nBounds[")  + testAccept_ (n) +
         ", "    + testRefuse_ (n) +
         "]; Value   = "  + diagValue_  (n) +
         "; Prob   = "  + diagPunct_  (n) +
         "; Qualify = "  + qualifTitle_[diagQualify_ (n)] +";"+
         "; Qualify(1/3,2/3) = " + q.Name() +";\n");
*/
  }
  if(periodicity_>1) { arithmeticQualification_ /= testTitle_.Size(); }
  else               { arithmeticQualification_ /= (testTitle_.Size()-3); }
/*
  Std(BText("\nModel Average Qualify(1/3, 2/3) = ") +
      arithmeticQualification_.Name() +";\n");
*/
}
