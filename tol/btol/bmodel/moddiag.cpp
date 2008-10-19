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
	if(testAccept_(n) <	 testRefuse_(n))
	{
	    if     (x<=testAccept_(n)) { diagQualify_(n) = BDIAGGOOD;	}
	    else if(x<=testRefuse_(n)) { diagQualify_(n) = BDIAGACCEPT; }
	    else		       { diagQualify_(n) = BDIAGREFUSE; }
	}
	else
	{
	    if     (x>=testAccept_(n)) { diagQualify_(n) = BDIAGGOOD;	}
	    else if(x>=testRefuse_(n)) { diagQualify_(n) = BDIAGACCEPT; }
	    else		       { diagQualify_(n) = BDIAGREFUSE; }
	}
	diagPunct_(n) = x;
    }
    else
    {
	diagQualify_(n) = BDIAGUNNECESSARY;
	diagPunct_	 (n) = 0;
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
    diagValue_(n) = resACor_.Get(0,1);
    BDat y = diagValue_(n)*Sqrt(N_);
    BDat x = AbsNormal(y);
//Std(BText("\nFirst regular autocorrelation = ")+y.Name());
//Std(BText("\nProbability		     = ")+x.Name());
    return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::FstSeaAutCorTest(BInt n)

/*! Runs the test of first seasonal autocorrelation
 */
//--------------------------------------------------------------------
{
    BDat x;
    BBool necessary = (periodicity_>1);
    if(necessary)
    {
	diagValue_(n) = resACor_.Get(periodicity_-1,1);
	BDat y = diagValue_(n)*Sqrt(N_);
	x = AbsNormal(y);
//  Std(BText("\nFirst seasonal autocorrelation = ")+y.Name());
//  Std(BText("\nProbability			= ")+x.Name());
    }
    return(Qualify(n,x,necessary));
}

//--------------------------------------------------------------------
BInt BModel::SndRegAutCorTest(BInt n)

/*! Runs the test of second regular autocorrelation
 */
//--------------------------------------------------------------------
{
    diagValue_(n) = resACor_.Get(1,1);
    BDat y = diagValue_(n)*Sqrt(N_);
    BDat x = AbsNormal(y);
//Std(BText("\nSecond regular autocorrelation = ")+y.Name());
//Std(BText("\nProbability		      = ")+x.Name());
    return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::SndSeaAutCorTest(BInt n)

/*! Runs the test of second seasonal autocorrelation
 */
//--------------------------------------------------------------------
{
    BDat x;
    BBool necessary = (periodicity_>1);
    if(necessary)
    {
	BInt m = Minimum(2*periodicity_-1,resACor_.Rows()-1);
	BDat y = resACor_(m,1)*Sqrt(N_);
	diagValue_(n) = y;
	x = AbsNormal(y);
//  Std(BText("\nSecond seasonal autocorrelation = ")+y.Name());
//  Std(BText("\nProbability			 = ")+x.Name());
    }
    return(Qualify(n,x,necessary));
}


//--------------------------------------------------------------------
BInt BModel::RegBoxPierceLjungTest(BInt n)

/*! Runs the test of the Box-Pierce-Ljung statistic of regular
 *  autocorrelations
 */
//--------------------------------------------------------------------
{
    BInt	m = aCorNum_;
    BInt	p = arParam_;
    BInt	q = maParam_;
    RegularBoxPierceLjung_ = BoxPierceLjung(residuous_, m, p, q);
    BChiSquareDist Chi(m-p-q);
    diagValue_(n) = RegularBoxPierceLjung_;
    BDat x =  Chi.Dist(RegularBoxPierceLjung_);
//Std(BText("\nRegular Box-Pierce-Ljung = ")+RegularBoxPierceLjung_.Name());
//Std(BText("\nProbability		= ")+x.Name());
    return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::SeaBoxPierceLjungTest(BInt n)

/*! Runs the test of the Box-Pierce-Ljung statistic of seasonal
 *  autocorrelations
 */
//--------------------------------------------------------------------
{
    BBool necessary = (periodicity_ > 1) &&
	(residuous_.Size()/periodicity_ > 2);
    BDat x;
    if(necessary)
    {
	BInt  m = aCorNum_/periodicity_;
	BInt  p = arFactors_[1].Size()-1;
	BInt  q = maFactors_[1].Size()-1;
	BArray<BDat> seaRes(N_/periodicity_);
/*
  Std(BText("\nm = ")+m);
  Std(BText("\np = ")+p);
  Std(BText("\nq = ")+q);
*/
	for(BInt i=0; i<seaRes.Size(); i++)
	{
	    seaRes[i] = residuous_[i*periodicity_];
//    Std(BText("\nseaRes[")+i+"]="+seaRes[i]);
	}
	SeasonalBoxPierceLjung_ = BoxPierceLjung(seaRes, m, p, q);
	diagValue_(n) = SeasonalBoxPierceLjung_;
	BChiSquareDist Chi(m-p-q);
	x =	 Chi.Dist(SeasonalBoxPierceLjung_);
/*
  Std(BText("\nSeasonal Box-Pierce-Ljung = ")+
  SeasonalBoxPierceLjung_.Name());
  Std(BText("\nProbability		   = ")+x.Name());
*/
    }
    return(Qualify(n,x,necessary));
}

//--------------------------------------------------------------------
BInt BModel::KullbackLeiblerDistanceTest(BInt n)

/*! Runs the test of Kullback-Leibler distance to normal of residuous
 */
//--------------------------------------------------------------------
{
  static BReal paso = 0.25;
  static BDat  KullbackLeiblerAvr  = -5.349629861;
  static BDat  KullbackLeiblerStDs =  3*0.544121702;

  BArray<BDat> res01 = residuous_;
  BInt	       i,j;
  BDat	       max=0;
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
    BReal from	= paso*j;
    BReal until = paso*(j+1);
    BDat pt    = 2*(BNormalDist::Dist01(until)-BNormalDist::Dist01(from));
    BDat pe    = freq01[j]/res01.Size();
    BDat delta = pe*Log(pt/pe);
    if(delta.IsKnown()) { epsilon -= delta.Value(); }
    from = until;
  }
  BDat x = (Log(epsilon)-KullbackLeiblerAvr)/KullbackLeiblerStDs;
  x = AbsNormal(x);

//Std(BText("\nNumber of intervals	 = ")+numInterv);
//Std(BText("\nKullback-Leibler Distance = ")+epsilon.Name());
//Std(BText("\nProbability		 = ")+x.Name());
  return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::FisherDistanceTest(BInt n)

/*! Runs the test of Fisher distance to normal of residuous.
 */
//--------------------------------------------------------------------
{
    BInt numInterv = BInt(Sqrt(N_));
//Std(BText("\nnumInterv=")+numInterv);
    
    BChiSquareDist Chi(numInterv);
    BArray<BDat>	res01  = residuous_;
    BMatrix<BDat> freq01;
    BInt	       i,j;
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
//Std(BText("\Fisher Distance	   = ")+epsilon.Name());
//Std(BText("\nProbability	   = ")+x.Name());
    return(Qualify(n,x,BTRUE));
}


//--------------------------------------------------------------------
BInt BModel::MinSignificationTest(BInt n)

/*! Runs the test of parameter's signification
 */
//--------------------------------------------------------------------
{
    static BTStudentDist T(1);
    BInt	i;
    BDat	min=1000000;
    for(i=0; i<param_.Size(); i++)
    {
	BDat t = Abs(param_[i]/paramSD_[i]);
	if(t.IsKnown() && (min > t))
	{
	    min = t;
	    diagValue_(n) = param_[i]/paramSD_[i];
	}
    }
    BDat x = 2*(1-T.Dist(min));
//Std(BText("\nMinimum signification = ")+min.Name());
//Std(BText("\nProbability	   = ")+x.Name());
    return(Qualify(n,x,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::MaxCorrelationTest(BInt n)

/*! Runs the test of parameter's correlations
 */
//--------------------------------------------------------------------
{
    BDat	max=0;
    BInt	N = paramCor_.Rows();
    for(BInt i=0; i<N; i++)
    {
	for(BInt j=0; j<i; j++)
	{
	    BDat c = Abs(paramCor_(i,j));
	    if(max < c)
	    {
		diagValue_(n) = paramCor_(i,j);
		max = c;
	    }
	}
    }
//BDat t = Sqrt(N-2)*max/Sqrt(1-(max^2));
//BTStudentDist T(N-2);
//BDat x = 1-2*T.Dist(Abs(t));
//Std(BText("\nMaximum correlation = ")+max.Name());
//Std(BText("\nProbability	   = ")+x.Name());
    return(Qualify(n,max,BTRUE));
}

//--------------------------------------------------------------------
BInt BModel::MixedSignCorrTest(BInt n)

/*! Runs the test of mixed signification and correlation of parameters
 */
//--------------------------------------------------------------------
{
    static BTStudentDist T(1);
    BBool necessary = (diagQualify_[7] != BDIAGREFUSE) &&
	(diagQualify_[8] != BDIAGREFUSE);
    BDat maxMixed = 0;
    if(necessary)
    {
	for(BInt i=0; i<paramCor_.Rows(); i++)
	{
	    BDat	corr=0;
	    for(BInt j=0; j<paramCor_.Rows(); j++)
	    {
		if(i!=j)
		{
		    BDat c = Abs(paramCor_(i,j));
		    if(corr < c) { corr = c; }
		}
	    }
	    BDat sign	 = 2*(1-T.Dist(Abs(param_[i]/paramSD_[i])));
	    BDat mixed = Sqrt(Abs(sign*corr));
	    if(maxMixed<mixed) { maxMixed = mixed; }
	}
	diagValue_(n) = maxMixed;
//  Std(BText("\nMaximum mixed correlation-signification = ")+
//	maxMixed.Name());
    }
    return(Qualify(n,maxMixed,necessary));
}


//--------------------------------------------------------------------
BInt BModel::UnitRootsProbTest(BInt n)

/*! Runs the test of unitary roots for ARMA polynomials
 */
//--------------------------------------------------------------------
{
//Std("\nBModel::UnitRootsProbTest");
  BDat			 x = 0;
  BInt			 num = arParam_+maParam_;
  BBool necessary = 0; /*num!=0; */
  if(necessary)
  {
//  Std(BText("\nARMA Parameters Number = ")+num);
    BInt		   m   = paramCor_.Rows() - num;
//  Std(BText("\nNON ARMA Parameters Number = ")+m);
    BInt		   i, j, k, iter;
    BSymMatrix<BDat>	   cov (num);
    BMatrix<BDat>	   param(num,1);
    BArray<BDat>	   unarys(4);
    BArray<BPol>	   factors(4);

    factors[0] = arFactors_[0];
    factors[1] = arFactors_[1];
    factors[2] = maFactors_[0];
    factors[3] = maFactors_[1];
    unarys [0] = 0;
    unarys [1] = 0;
    unarys [2] = 0;
    unarys [3] = 0;

    i=0;
//  Std(BText("\nFactors =\n"));
    for(i=j=0; j<4; j++)
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

    BInt maxIter = 100;
    for(iter=1; iter<=maxIter; iter++)
    {
      BMatrix<BDat> simula = dist.Random();
      for(i=j=0; j<4; j++)
      {
	for(k=1; k<factors[j].Size(); k++,i++)
	{ factors[j][k].PutCoef(-simula(0,i)); }
//	  Std(BText("\nFactor[")+j+"]"+factors[j].Name() +
//		  " Stationary = " + factors[j].Stationary());
	if(!(factors[j].IsStationary())) { unarys[j]+=1; }
      }
    }
    x = 0;
    for(j=0; j<4; j++)
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
	case  0 : return(FstRegAutCorTest	   (n));
	case  1 : return(FstSeaAutCorTest	   (n));
	case  2 : return(SndRegAutCorTest	   (n));
	case  3 : return(SndSeaAutCorTest	   (n));
	case  4 : return(RegBoxPierceLjungTest     (n));
	case  5 : return(SeaBoxPierceLjungTest     (n));
	case  6 : return(FisherDistanceTest	   (n));
	case  7 : return(MinSignificationTest      (n));
	case  8 : return(MaxCorrelationTest	   (n));
	case  9 : return(MixedSignCorrTest	   (n));
	case 10 : return(UnitRootsProbTest	   (n));
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
  for(BInt n = 0; n<testTitle_.Size(); n++)
  {
    BSet& s = Set(set[n+1]);
    diagValue_	(n) = BDat::Unknown();
    diagPunct_	(n) = 0;
    testAccept_ (n) = Dat(s[1]);
    testRefuse_ (n) = Dat(s[2]);
    diagQualify_(n) = RunTest(n);
    if(qualification_<diagQualify_(n)) { qualification_=diagQualify_(n); }
    BDat x0 = BDat(1)/3;
    BDat y0 = BDat(2)/3;
    BDat x  = testAccept_ (n);
    BDat y  = testRefuse_ (n);
    BDat a  = (y0-x0)/(y-x);
    BDat b  = x0 - a*x;
    BDat q  = a*diagPunct_(n)+b;
    arithmeticQualification_ += q;
/*
    Std( BText("\nBounds[")	+ testAccept_ (n) +
	       ", "		+ testRefuse_ (n) +
	       "]; Value   = "	+ diagValue_  (n) +
	       "; Prob	 = "	+ diagPunct_  (n) +
	       "; Qualify = "	+ qualifTitle_[diagQualify_ (n)] +";"+
	       "; Qualify(1/3,2/3) = " + q.Name() +";\n");
*/
  }
  if(periodicity_>1) { arithmeticQualification_ /= testTitle_.Size(); }
  else		     { arithmeticQualification_ /= (testTitle_.Size()-3); }
/*
  Std(BText("\nModel Average Qualify(1/3, 2/3) = ") +
	    arithmeticQualification_.Name() +";\n");
*/
}
