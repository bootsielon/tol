/* prd.cpp: BProbDist class and all deriving classes.
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

#include <tol/tol_bcommon.h>
#include <tol/tol_bprdist_internal.h>
#include <tol/tol_bfibonac.h>
#include <tol/tol_bout.h>
#include <tol/tol_butil.h>
#include <tol/tol_btimer.h>
#include <tol/tol_cdflib.h>

#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf_result.h>
#include <gsl/gsl_sf_gamma.h>
#include "kmlocal/KMrand.h"

BTraceInit("prd.cpp");


/* DQCComments: La siguiente línea da problemas si la versión instalada
 * de gsl es anterior a 1.4.  Si es así, comentarla. Ello supone hacer
 * un pequeño cambio más adelante, está documentado y marcado tbén con
 * DQCComments
 */
#include <gsl/gsl_cdf.h> 


//--------------------------------------------------------------------
// Static variable for BProbDist.
//--------------------------------------------------------------------


BNormalDist BProbDist::n01_(StaticInit());
void *   BProbDist::rng_ = NULL;
unsigned long int BProbDist::seed_ = 0;

//--------------------------------------------------------------------
  DefIsAlreadyInitilialized(BProbDist);
  bool BProbDist::InitializeClass()
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BProbDist)) 
  {
    BDat::InitializeClass();
    n01_ = BNormalDist(0.0,1.0);
    seed_ = 0;
    InitGSLRand(0);
  }
  return(true);
}


//--------------------------------------------------------------------
void BProbDist::InitGSLRand(unsigned long int seed)

/*! Initializes random GSL function. If the seed is 0 then a
 */
//--------------------------------------------------------------------
{
  static unsigned long int last_rnd_seed_ = 0;
  if(!seed) 
  {
    double t = (double)time(NULL);
    double c = BTimer::ClockToMiliSecond(clock());
    seed = (unsigned long)(t+c+last_rnd_seed_);
    for(int n=1; n<=5; n++) { seed = (seed*16807)%2147483647; }
#  ifdef TRACE_LEVEL
  //Std(BText("\nInitGSLRand seed: ")<<
              " t="<<BDat(t).Format("%.0lf")<<
              " c="<<BDat(c).Format("%.0lf")<<
              " m="<<BDat(m).Format("%.0lf")<<
              " l="<<BDat(last_rnd_seed_).Format("%.0lf")<<
              " seed="<<BDat(seed).Format("%.0lf")<<"\n");
#  endif
    last_rnd_seed_ = seed;  
  } 
  BProbDist::seed_ = gsl_rng_default_seed = seed;
  BProbDist::rng_  = gsl_rng_alloc(gsl_rng_default);
  //Std(BText("\nInitGSLRand seed=")<<(int)gsl_rng_default_seed<<"\n");
}

//--------------------------------------------------------------------
void * BProbDist::rng()

/*! Initializes random GSL environment variable.
 */
//--------------------------------------------------------------------
{
  if(!rng_) { InitializeClass(); }
  return(rng_);
}

//--------------------------------------------------------------------
  void BProbDist::PutRandomSeed(unsigned long int seed) 

/*! Forzes GSL random generator to use a given seed.
 */
//--------------------------------------------------------------------
{
  InitGSLRand(seed);
  kmInitialice(seed);
};

//--------------------------------------------------------------------
  unsigned long int BProbDist::GetRandomSeed()

/*! Returns the current seed of the GSL random generator.
 */
//--------------------------------------------------------------------
{
  return(BProbDist::seed_);
};

//--------------------------------------------------------------------
static BDat LogFact(BInt n)
//--------------------------------------------------------------------
{
    return(LogGamma(n+1));
}

//--------------------------------------------------------------------
BDat BProbDist::Random()

/*! Returns a random value with this distribution function.
 */
//--------------------------------------------------------------------
{
  BDat r = BUniformDist::Random01();   
  return(Inverse(r));
}


BDat BInvDFunction::Dy(BDat x, BDat y)
{
  if(x.IsUnknown() || y.IsUnknown()) { return(BDat::Unknown()); }
  return(1/(probDist_->Dens(y)));
}

void BDensFunction::Evaluate(BDat& y, const BDat& x)
{
  if(x.IsUnknown()) { y = BDat::Unknown(); }
  else              { y = probDist_->Dens(x); }
}

//--------------------------------------------------------------------
void BDistFunction::Evaluate(BDat& y, const BDat& x)
//--------------------------------------------------------------------
{
    if(oldx_.IsKnown() && oldy_.IsKnown())
    {
	if(oldx_==x) { y = oldy_; }
	if(oldx_<x)  { y = oldy_ + probDist_->Prob(oldx_, x); }
	else	     { y = oldy_ - probDist_->Prob(x, oldx_); }
    }
    else
    {
	y = probDist_->Dist(x);
    }
    if(y>1) { y=1; }
    if(y<0) { y=0; }
    oldx_ = x;
    oldy_ = y;
}


//--------------------------------------------------------------------
BDat BProbDist::InverseAprox(BDat prob, BDat tolerance)

/*! Returns the number whose probability is aprox. prob with this
 *  distribution.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    BDat x  = BNormalDist::Inverse01(1-prob);
    BDat m  = Average();
    BDat s  = Sqrt(Varianze());
    x = m+s*x;
    if(x.IsKnown()) { return(x); }
    
    BDat ini, fin, p;
    
    if(max_.IsKnown()&&min_.IsKnown())
    {
	ini = min_;
	fin = max_;
	p	= prob;
    }
    else if(max_.IsKnown()&&min_.IsUnknown())
    {
	fin = max_;
	ini = -1;
	do
	{
	    ini *= 2;
	    p = GetDist(ini);
	    if(p>prob) { fin = ini; }
	}
	while (p.IsKnown() && (p>prob));
    }
    else if(min_.IsKnown()&&max_.IsUnknown())
    {
	ini = min_;
	fin = 1;
	do
	{
	    fin *= 2;
	    p = GetDist(fin);
	    if(p<prob) { ini = fin; }
	}
	while (p.IsKnown() && (p<prob));
    }
    x = 0;
    if(p.IsKnown())
    { 
      SetFunctions();
      x = BFibonacci::Solve(distribution_, prob, ini, fin , tolerance); 
    }
    return(x);
}


//--------------------------------------------------------------------
BDat BProbDist::Inverse(BDat prob, BDat tolerance)

/*! Returns the number whose probability is prob with this
 *  distribution.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    BBool ok = BTRUE;
    if(prob==0) { return(min_); }
    if(prob==1) { return(max_); }
    BDat	x  = InverseAprox(prob,tolerance);
    BInt	n1, n2, n3, n4, n5;
    n1 = InverseFrom(x, prob, tolerance);
    if(n1==0)
    {
	n2 = InverseFrom(x, prob, tolerance);
	if(n2==0)
	{
	    n3 = InverseFrom(x, prob, tolerance);
	    if(n3==0)
	    {
		n4 = InverseFrom(x, prob, tolerance);
		if(n4==0)
		{
		    n5 = InverseFrom(x, prob, tolerance);
		}
	    }
	}
    }
    if((n1<0)||(n2<0)||(n3<0)||(n4<0)||(n5<0))
    {
	InverseFib(x, prob, tolerance);
    }
    return(x);
}


//--------------------------------------------------------------------
BInt BProbDist::InverseFrom(BDat& x, BDat prob, BDat tolerance)

/*! Returns the number whose probability is prob with this
 *  distribution by Newton Method.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { x = BDat::Unknown(); return(0); }
    BInt N = (BInt)BDat::MaxIter().Value();
    if(prob>=1) { x = max_; return(N); }
    if(prob<=0) { x = min_; return(N); }
//BBool ok=BTRUE;
    BDat dif=0.0, density=0.0, probability=0.0;
    BInt n=-1;
//BArray<BDat> old, pOld;
//old.ReallocBuffer(1+BDat::MaxIter().Value());
//pOld.ReallocBuffer(1+BDat::MaxIter().Value());
    BDat difP;
    do
    {
	n++;
//  old.ReallocBuffer(n+1);
//  pOld.ReallocBuffer(n+1);
#     ifdef UNIX // VBR : Error inexplicable del GNU
	probability = BNormalDist::Dist01(x);
	density	= BNormalDist::Dens01(x);
#     else
	probability = GetDist(x);
	density	= GetDens(x);
#     endif
//  old[n] = x;
//  pOld[n] = probability;
	difP = Abs(probability-prob);
	if(probability==prob) { return(N-n); }
	if(probability.IsUnknown() || density.IsUnknown() ||
	   (probability==0) || (probability==1) || (density==0) )
	{
	    dif/=(-2);
	    x	 += dif;
	}
	else
	{
	    dif = (probability-prob)/density;
	    x	 -= dif;
	}
/*
  if(max_.IsKnown() && (x>max_)) { x = max_+tolerance/2; }
  if(min_.IsKnown() && (x<min_)) { x = min_-tolerance/2; }
  for(BInt i=0; (i<n) && ok; i++) { if(x==old[i]) { ok = BFALSE; } }
*/
    }
    while(((difP>tolerance)||(Abs(dif)>tolerance))&&(N>n));
/*
  while(ok&&(Abs(dif)>tolerance)&&(N>n));
  if(!ok)
  {
  Warning(I2("\nInverse distribution metod failed for ",
  "\nEl método de inversión de la distribución fallo para ")+prob+
  I2( " at iteration "," en la iteración ") + n  );
  return(-1);
  }
  else
*/
    { return(N-n); }
}



//--------------------------------------------------------------------
BDat BProbDist::InverseFib(BDat& x, BDat prob, BDat tol)

/*! Returns the number whose probability is prob with this
 *  distribution by Fibonacci method.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    BDat p = 0, q = 1;
    BDat ini = min_;
    BDat fin = max_;
    BDat px  = GetDist(x);
    if(px==prob) { return(x); }
    if(fin.IsUnknown())
    {
	fin = x;
	q   = px;
	while (q.IsKnown() && (q<=prob))
	{
	    fin += Desviation();
	    q = GetDist(fin);
	    if(q==prob) { return(fin); }
	    if((q<prob) && ini.IsUnknown() || (ini>fin))  { ini = fin; }
	}
    }
    if(ini.IsUnknown())
    {
	ini = x;
	p   = px;
	while (p.IsKnown() && (p>=prob))
	{
	    ini -= Desviation();
	    p = GetDist(ini);
	    if(p==prob) { return(ini); }
	    if((p>prob) && fin.IsUnknown() || (ini>fin))  { fin = ini; }
	}
    }
    if((p<=prob)&&(q>=prob))
    {
//  x = BFibonacci::Solve(&distribution_, prob, ini, fin , tolerance);
	do
	{
	    x = (fin+ini)/2;
	    px = GetDist(x);
	    if(px>prob) { fin = x; }
	    else	  { ini = x; }
//  //Std(BText("\nx=")+x.Name()+"\tpx="+px.Name()+"\tprob="+prob.Name());
	}
	while((Abs(fin-ini)>tol)&&(Abs(prob-px)>tol));
    }
    return(x);
}

//--------------------------------------------------------------------
  BDat BDiscreteDist::Inverse (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  BText _MID = "BDiscreteDist::Inverse";
  if(wrongParameter_|| prob.IsUnknown()) { return(BDat::Unknown());}
  if(prob+1.0<1.0) { return(BDat::Unknown()); }
  if(prob-1.0>0.0) { return(BDat::Unknown()); }
  if(prob+1.0==1.0) { return(min_); }
  if(prob-1.0==0.0) { return(max_); }
  bool has_lower_bound = !min_.IsNegInf();
  bool has_upper_bound = !max_.IsPosInf();
  double a = min_.Value();
  double b = max_.Value();
  double pa = has_lower_bound ? Dist(a).Value() : 0;
  if(prob<=pa) { return(a); }
  double pb = 1.0;
  double h = 1.0;
  double h_factor = 10.0;
  if(!has_lower_bound || !has_upper_bound)
  {
    double avr = round(Average().Value());
    if(!IS_FINITE(avr))
    {
      Error(_MID+"Cannot handle with distribution which average is not finite."); 
      return(BDat::Unknown());
    }
    double pdf_avr = Dist(avr).Value();
    if(prob+1==pdf_avr+1) {  return(avr); }
    else if(pdf_avr>prob)
    {
      b = avr;
      pb = pdf_avr;
      if(!has_lower_bound)
      {
        a = b;
        pa = pb;
      }
    }
    else // if(pdf_avr<prob)
    {
      a = avr;
      pa = pdf_avr;
      if(!has_upper_bound)
      {
        b = a;
        pb = pa;
      }
    }   
  };
  has_lower_bound = IS_FINITE(a)!=0;
  has_upper_bound = IS_FINITE(b)!=0;
  if(!has_lower_bound || !has_upper_bound)
  {
    Error(_MID+"Sorry, cannot find a finite searching interval for p="+prob); 
    return(BDat::Unknown());
  }
  while(pa>prob)
  {
    a -= h;
    pa = Dist(a).Value(); 
    if(prob+1==pa+1) {  return(a); }
    h *= h_factor;
  }
  while(pb<prob)
  {
    b += h;
    pb = Dist(b).Value(); 
    if(prob+1==pb+1) {  return(b); }
    h *= h_factor;
  }
  double pa1 = (min_>=a-1)?0.0:Dist(a-1).Value(); 
  double pb1 =                 Dist(b-1).Value(); 
       if((pa1 < prob) && (prob <=pa)) {  return(a); }
  else if((pb1 < prob) && (prob <=pb)) {  return(b); }

  if((a>=b)||(pa>=pb)||(pa>prob)||(pb<prob))
  {
    Error(_MID+"Sorry, cannot find a valid searching interval for p="+prob); 
    return(BDat::Unknown());
  }
  double x;
  double px; 
  double px1; 
  while(a<b-1)
  {
    x = round((a+b)/2);
    px = Dist(x).Value(); 
    px1 = Dist(x-1).Value(); 
         if((px1 < prob) && (prob <=px)) {  return(x); }
    else if(prob> px1)                   {  a = x; pa = px; pa1 = px1; }
    else                                 {  b = x; pb = px; pb1 = px1; }
  }
       if((pa1 < prob) && (prob <= pa)) {  return(a); }
  else if((pb1 < prob) && (prob <= pb)) {  return(b); }
  else
  {
    Error(_MID+"Sorry, cannot find a valid searching interval for p="+prob); 
    return(BDat::Unknown());
  }

}

//--------------------------------------------------------------------
BDat BDiscreteDist::Summ(BInt from, BInt until)

/*! Returns the summ of density function between two bounds
 */
//--------------------------------------------------------------------
{
    BDat summ = 0.0;
    BDat sign = 1.0;
    if(from>until)
    {
	sign = -1.0;
	BInt a = from;
	from = until;
	until = a;
    }
    for(BInt n=from; n<=until; n++)
    {
	summ += Dens(n);
    }
    return(summ*sign);
}


//--------------------------------------------------------------------
BDat BProbDist::NIntegrate(BDat from, BDat until)

/*! Returns the numerical integral of density function between two
 *  bounds
 */
//--------------------------------------------------------------------
{
  SetFunctions();
  if(wrongParameter_ || from.IsUnknown() || until.IsUnknown()) { return(BDat::Unknown()); }
    return(density_->AdaptiveSimpson(from,until,BDat::Tolerance()));
//return(density_.Integral(from,until,5));
}




//--------------------------------------------------------------------
BDat BUniformDist::Dens01  (BDat x)
//--------------------------------------------------------------------
{
  if(x.IsUnknown()) { return(BDat::Unknown()); }
         if(x<=0) { return(0); }
    else if(x>=1) { return(0); }
    else          { return(1); }
}


//--------------------------------------------------------------------
BDat BUniformDist::Dist01  (BDat x)
//--------------------------------------------------------------------
{
  if(x.IsUnknown()) { return(BDat::Unknown()); }
         if(x<=BDat::Zero()) { return(0.0); }
    else if(x>=1.0)	     { return(1.0); }
    else		     { return(x); }
}

//--------------------------------------------------------------------
BDat BUniformDist::Random01()

/*! Generates a pseudo-random series of uniformely distributed in
 *  [0,1] and independent real numbers using the multiplicative
 *  congruential method IBM System/360 Uniform Random Number
 *  Generator. See
 *
 *  Simulation and Montecarlo Method. Page 25.
 *  Wiley series in Probability and Mathematical Statistics.
 */
//--------------------------------------------------------------------
{
    return gsl_rng_uniform(getGslRng());
  /*
  double y;
  drand(&y);
  if(y<1) { y = 1; }
  if(y>2) { y = 2; }
  return(y-1.0);
  */
/*
  static unsigned long startRand_ = clock();
  startRand_ = (16807*startRand_)%2147483647;
  BDat r = BDat(startRand_)/BDat(2147483646.0);
  if(startRand_==0) { startRand_ = clock(); }
  return(r);
*/
}


//--------------------------------------------------------------------
BDat BUniformDist::Inverse(BDat x, BDat tolerance)

/*! Returns the  cumulative distribution inverse value for x 
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if((x>1)||(x<0)) return(BDat::Unknown()); 
    return(x*length_ + min_);
}



//--------------------------------------------------------------------
BNormalDist::BNormalDist(BDat nu, BDat sigma)
//--------------------------------------------------------------------
    : BProbDist(), nu_(nu), sigma_(sigma)
{
    if((sigma<=0)||(nu.IsUnknown())||(sigma.IsUnknown()))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a normal distribution with parameters",
		   "No se puede usar una distribucion normal con parametros")+
		" ("+nu.Name()+", "+sigma.Name()+")");
    }

}


//--------------------------------------------------------------------
BDat BNormalDist::Dens01(BDat x)

/*! Returns the normal(0,1) probability of any x.
 */
//--------------------------------------------------------------------
{
  if(x.IsUnknown()) { return(BDat::Unknown()); }
#ifdef _MSC_VER
  static BDat sqrt2PI_ = Sqrt(BDat(2.0*BDat::Pi()));
#else
  static BDat sqrt2PI_ = Sqrt(BDat(2.0*BDat::Pi()));
#endif
  return(Exp(-0.5*(x^2.0))/sqrt2PI_);
}


//--------------------------------------------------------------------
BDat BNormalDist::Dist01(BDat x)

/*! Returns the normal(0,1) probability of any x.
 *  Abramowitz page 933. 26.2.19
 */
//--------------------------------------------------------------------
{
  if(x.IsUnknown()) { return(BDat::Unknown()); }
  return(BDat(gsl_cdf_ugaussian_P(x.Value())));
}


//--------------------------------------------------------------------
BDat BNormalDist::Random01()

/*! Generates pairs of independent unit normal variables from pairs
 *  of indepentent uniformly distributed random variables using the
 *  Box-Muller method.
 *  Returns one of the two normal variables of the pair alternatively.
 */
//--------------------------------------------------------------------
{
    return(BDat(gsl_ran_ugaussian(getGslRng())));
}


//--------------------------------------------------------------------
BDat BNormalDist::Inverse01	(BDat p)

/*! Abramowitz page 933. 26.2.33
 */
//--------------------------------------------------------------------
{
  if(p.IsUnknown()) { return(BDat::Unknown()); }
  return(BDat(gsl_cdf_ugaussian_Pinv(p.Value())));
}

//--------------------------------------------------------------------
BDat BNormalDist::I(BInt n, BDat x, BDat tol)

/*! The (n+1)-th repeated integral of the normal density function
 *
 *	      Abramowitz page 934. 26.2.40
 */
//--------------------------------------------------------------------
{
    return(Sqrt((2^(n-2)))*Erfc(n,Sqrt(2)*x,tol*0.1));
}

//--------------------------------------------------------------------
BDat BNormalDist::Dens(BDat x)
//--------------------------------------------------------------------
{ 
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
	return(Dens01(Norm(x))/sigma_);    
}

//--------------------------------------------------------------------
BDat BNormalDist::Inverse(BDat prob, BDat tol)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
  if (prob <=0) return(-BDat::PosInf());
  if (prob >= 1) return(BDat::PosInf());
  return(InvNorm(Inverse01(prob)));
}


//--------------------------------------------------------------------
BDat BLogNormalDist::Dens(BDat x)

/*! Returns LogNormal(i, nu_, sigma_)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if (x>0)
	return(n01_.Dens(Norm(x))/(x*sigma_));  
    else
	return 0;
}

//--------------------------------------------------------------------
BDat BLogNormalDist::Dist(BDat x)

/*! Returns Sum{0..x}.LogNormal(i, nu_, sigma_)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if (x>0)  
	return(n01_.Dist(Norm(x)));
    else
	return 0;   
}

//--------------------------------------------------------------------
  BTruncatedDist::BTruncatedDist()
//--------------------------------------------------------------------
: BProbDist(),
  A_(min_),
  B_(max_)
{ 
}

//--------------------------------------------------------------------
  bool BTruncatedDist::SetBounds(BDat A, BDat B)
//--------------------------------------------------------------------
{ 
  A_ = A;
  B_ = B;
  wrongParameter_ = NonTruncated().WrongParameter();
  if(B_<A_) { wrongParameter_ = true; }
  if(wrongParameter_) { return(false); }
  if(A_.IsUnknown()) { A_ = BDat::NegInf(); }
  if(B_.IsUnknown()) { B_ = BDat::PosInf   (); }
  if(A_<=NonTruncated().Min()) 
  { 
    A_   = NonTruncated().Min();
    fA_  = 0; 
    FA_  = 0; 
    F1A_ = 1; 
  }
  else                   
  { 
    fA_  = NonTruncated().Dens( A_); 
    FA_  = NonTruncated().Dist( A_); 
    F1A_ = 1.0-NonTruncated().Dist(A_); 
  }   
  if(B_>=NonTruncated().Max()) 
  { 
    B_   = NonTruncated().Max(); 
    fB_  = 0; 
    FB_  = 1; 
    F1B_ = 0; 
  }
  else 
  { 
    fB_  = NonTruncated().Dens( B_); 
    FB_  = NonTruncated().Dist( B_); 
    F1B_ = 1.0-NonTruncated().Dist(B_); 
  }   
  useLeft_ = fB_ <= fA_;
  if(useLeft_)
  {
    FAB_ = (FB_-FA_+1.E+4)-1.E+4;
  }
  else
  {
    FAB_ = (F1A_-F1B_+1.E+4)-1.E+4;
  }
  if(FAB_<0)
  {
    wrongParameter_ = true; 
  }
  return(!wrongParameter_);
}

//--------------------------------------------------------------------
BDat BTruncatedDist::Dist(BDat x) 
//--------------------------------------------------------------------
{ 
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  else if(x<A_) { return(0); }
  else if(x>B_) { return(1); }
  else if(!FAB_) { return((x-A_)/(B_-A_)); }
  else 
  { 
    BDat Fx = NonTruncated().Dist(x);
    return((Fx-FA_)/FAB_); 
  } 
//else if(useLeft_) { return((NonTruncated().Dist(x)-FA_)/FAB_); } 
//else              { return(1.0-(FB_-NonTruncated().Dist(x))/FAB_); } 
}

//--------------------------------------------------------------------
BDat BTruncatedDist::Dens(BDat x) 
//--------------------------------------------------------------------
{ 
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  else if(x<A_) { return(0); }
  else if(x>B_) { return(0); }
  else if(!FAB_) { return(1.0/(B_-A_)); }
  else { return(NonTruncated().Dens(x)/FAB_); } 
}

//--------------------------------------------------------------------
BDat BTruncatedDist::Inverse(BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
  if(A_+1.0==B_+1.0)  { return(A_); }
  if(FAB_<1.E-8) 
  { 
    BDat C = NonTruncated().Average();
         if(A_+1.0>C+1.0) { return(A_); }
    else if(B_+1.0<C+1.0) { return(B_); }
    {
      BDat a = A_;
      BDat b = B_;
      if(a==BDat::NegInf()) { a = -1.0/DEpsilon(); }
      if(b==BDat::PosInf()) { b =  1.0/DEpsilon(); }
      return(prob*(b-a)+a); 
    }
  }
  if(useLeft_) { return(NonTruncated().Inverse(FA_+     prob *FAB_)); }
  else         { return(NonTruncated().Inverse(FB_-(1.0-prob)*FAB_)); }
}

//--------------------------------------------------------------------
BDat BTruncatedDist::Random()
//--------------------------------------------------------------------
{ 
  if(A_+1.0==B_+1.0)  { return(A_); }
  return(Inverse(BUniformDist::Random01())); 
}


//--------------------------------------------------------------------
  BTruncatedNormalDist::BTruncatedNormalDist(BDat A, BDat B, 
                                             BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BTruncatedDist(), normal_(nu, sigma)
{ 
  SetBounds(A,B);
}

//--------------------------------------------------------------------
BDat BTruncatedNormalDist::Average	 ()	  
//--------------------------------------------------------------------
{
  if(!FAB_) { return((A_-B_)/2); }
  //VBR: Unimplemented!
  return(BDat::Unknown()); 
}

//--------------------------------------------------------------------
BDat BTruncatedNormalDist::Varianze	 ()	  
//--------------------------------------------------------------------
{
  if(!FAB_) { return(((B_-A_)^2)/12); }
  //VBR: Unimplemented!
  return(BDat::Unknown()); 
}

//--------------------------------------------------------------------
BDat BTruncatedNormalDist::Random01(
  BDat lower_, BDat upper_, BDat borderDistance_)
//--------------------------------------------------------------------
{
  double lower = lower_.Value();
  double upper = upper_.Value();
  double borderDistance = borderDistance_.Value();
  double  negInf = BDat::NegInf();
  double  posInf = BDat::PosInf();
  double tn;
  if((lower>negInf)||(upper<posInf))
  {
    //Checking different cases
    //lower = 66.8902; upper = posInf;
    //upper = -66.8902; lower = negInf;

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
      double uppMrg_ = sqrt(pow(lowMrg,2) + 2.0 * log(110.0) );
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
      tn = u*lowMrg + (1.0-u)*uppMrg;
      if((tn<=lower) ||(tn>=upper))
      { 
        Warning(BText("(I) TruncStdGaussian [Case C.1] ")<< 
          " domain:["<<lower<<","<<upper<<"] -> "<<
          " interior domain: ["<<lowMrg<<","<<uppMrg<<"] -> "<<tn);
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
      double lowMrg_ = -sqrt(pow(uppMrg,2) + 2.0 * log(110.0) );
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
      tn = u*uppMrg + (1.0-u)*lowMrg;
      if((tn<=lower) ||(tn>=upper))
      { 
        Warning(BText("(I) TruncStdGaussian [Case C.2] ") <<
          " domain:["<<lower<<","<<upper<<"] -> "<<
          " interior domain: ["<<lowMrg<<","<<uppMrg<<"] -> "<<tn);
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
      tn = u01.Inverse(u).Value(); 
      if((tn<=lower) ||(tn>=upper))
      { 
        Warning(BText("(I) TruncStdGaussian [Case C.1] ")<<
          " domain:["<<lower<<","<<upper<<"] -> "<<
          " Prob domain["<<lowF01<<","<<uppF01<<"] -> "<<
          " Prob interior domain["<<lowF01_<<","<<uppF01_<<"] -> "<<tn);
        return(-1);
      }
    }
  }
  else
  {
    tn = BNormalDist::Random01().Value();
    if((tn<=lower) ||(tn>=upper))
    { 
      Warning(BText("(I) TruncStdGaussian [Case D] ")<<
        I2("(empty interval",
        "(intervalo vacio")+" ["+BDat(lower).Format("%.16lg")+","+
                                 BDat(upper).Format("%.16lg")+"])");
    }
  }
  return(tn);
}

//--------------------------------------------------------------------
  BDat BTruncatedNormalDist::Random()
//--------------------------------------------------------------------
{
  BDat nu = normal_.Nu();
  BDat sigma = normal_.Sigma();
  BDat lower = (A_-nu)/sigma;
  BDat upper = (B_-nu)/sigma;
  BDat borderDistance = DEpsilon();
  BDat tn01 = Random01(lower,upper,borderDistance);
  BDat tn = sigma*tn01+nu;
  return(tn);
}

//--------------------------------------------------------------------
  BClosestInsideDist::BClosestInsideDist(BDat A, BDat B, BDat nu)
//--------------------------------------------------------------------
: BTruncatedNormalDist(A, B, nu, DEpsilon()), C_(nu)
{ 
}

//--------------------------------------------------------------------
  bool BClosestInsideDist::SetBounds(BDat A, BDat B)
//--------------------------------------------------------------------
{ 
  A_ = A;
  B_ = B;
  C_ = normal_.Nu();
  wrongParameter_ = false;
  if(B_<A_) { wrongParameter_ = true; }
  else
  {
    if(C_<=A_) { C_=A_; }
    if(C_>=B_) { C_=B_; }
  }
  return(!wrongParameter_);
}


//--------------------------------------------------------------------
BDat BClosestInsideDist::Dist(BDat x) 
//--------------------------------------------------------------------
{ 
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  if(x<C_) { return(0); }
  else     { return(1); }
}

//--------------------------------------------------------------------
BDat BClosestInsideDist::Dens(BDat x) 
//--------------------------------------------------------------------
{ 
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  if(x==C_) { return(1); }
  else      { return(0); }
}

//--------------------------------------------------------------------
BDat BClosestInsideDist::Inverse(BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
  return(C_);
}

//--------------------------------------------------------------------
BDat BClosestInsideDist::Random()
//--------------------------------------------------------------------
{ 
  if(wrongParameter_) { return(BDat::Unknown()); }
  return(C_);
}

//--------------------------------------------------------------------
BGammaDist::BGammaDist(BDat a, BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BProbDist(), a_(a), sigma_(sigma), nu_(nu)
{
    if((a_<=0)||(sigma_<=0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a gamma distribution with parameters",
		   "No se puede usar una distribucion gamma con parametros")+
		" ("+a.Name()+", "+sigma.Name()+", "+nu.Name()+")");
    }
    min_ = nu;
    k_ = (sigma_^a_)*Gamma(a_);
}


//--------------------------------------------------------------------
BDat BGammaDist::Inc(BDat a, BDat z)

/*! Incomplete Gamma Function Ratio.
 */
//--------------------------------------------------------------------
{
    double p, q, x, shape, scale = 1.0, bound;
    int which, status;
    
    shape = a.Value();
    x = z.Value();
    which = 1;
    cdfgam(&which, &p, &q, &x, &shape, &scale, &status, &bound);
    return p;
    
  //return(igam(a.Value(),z.Value()));
}


//--------------------------------------------------------------------
BDat BGammaDist::InvInc(BDat a, BDat p)

/*! Inverse of Incomplete Gamma Function Ratio.
 */
//--------------------------------------------------------------------
{
    double _p, q, x, shape, scale = 1.0, bound;
    int which, status;
    
    shape = a.Value();
    _p = p.Value();
    q = 1 - _p;
    which = 2;
    cdfgam(&which, &_p, &q, &x, &shape, &scale, &status, &bound);
    return x;
    //  return(igami(a.Value(),1-p.Value()));
}


//--------------------------------------------------------------------
BDat BGammaDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x<nu_) { return(0); }
    else      { return((((x-nu_)^(a_-1.0))*Exp(-(x-nu_)/sigma_))/k_); }
}


//--------------------------------------------------------------------
BDat BGammaDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x.IsUnknown())   { return(x); }
    if(x==BDat::PosInf())        { return(1); }
    if(x<=nu_)	        { return(0); }
    BDat u = (x-nu_)/sigma_;
    return(Inc(a_, u));
}


//--------------------------------------------------------------------
BDat BGammaDist::Inverse(BDat prob, BDat tolerance)

/*! Returns the number whose probability is prob with this
 *  distribution.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown())  { return(BDat::Unknown()); }
  if(prob.IsUnknown()) { return(BDat::Unknown()); }
  if(prob>=1)	         { return(BDat::Unknown()); }
  if(prob<0)	         { return(nu_); }
  BDat u = InvInc(a_,prob);
  BDat x = nu_+sigma_*u;
  return(x);
}


//--------------------------------------------------------------------
BDat BGammaDist::Average   ()
//--------------------------------------------------------------------
{
    return(nu_+a_*sigma_);
}


//--------------------------------------------------------------------
BDat BGammaDist::Varianze  ()
//--------------------------------------------------------------------
{
    return(a_*sigma_*sigma_);
}


//--------------------------------------------------------------------
BExpDist::BExpDist(BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BGammaDist()
{
    nu_	   = nu;
    sigma_ = sigma;
    min_   = nu;
    k_	   = (sigma_^a_)*Gamma(a_);
    if((sigma<=0.0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use an Exponential distribution with parameters",
		   "No se puede usar una distribucion exponencial con "+
		   "parametros")+ " ("+nu.Name()+", "+sigma.Name()+")");
    }
}


//--------------------------------------------------------------------
BChiSquareDist::BChiSquareDist(BInt degree)
//--------------------------------------------------------------------
    : BGammaDist(), degree_(degree)
{
  if(degree<=0)
  {
  	wrongParameter_ = BTRUE;
	  Warning(I2("Cannot use a chi-Square distribution with parameters",
		   "No se puede usar una distribucion chi-Square con "
		   "parametros")+" ("+degree+")");
  }
  else
  {
	  sigma_ = 2;
	  nu_    = 0;
	  a_     = BDat(degree_)/2;
	  min_   = 0;
	  k_     = Gamma(a_);
  }
}

//--------------------------------------------------------------------
BDat BChiSquareDist::Random()
//--------------------------------------------------------------------
{
    BDat r=0;
    if(degree_<=30)
    {
	for(BInt n=1; n<=degree_; n++) { r += n01_.Random()^2; }
    }
    else
    {
	r = ((n01_.Random()+Sqrt(2*degree_-1))^2)/2;
    }
    return(r);
}


//--------------------------------------------------------------------
BDat BBetaDist::Inc(BDat u, BDat p, BDat q)

/*! Incomplete Beta Function Ratio.
 */
//--------------------------------------------------------------------
{
    double _p, _q, x, y, a, b, bound;
    int which, status;
    
    a = p.Value();
    b = q.Value();
    x = u.Value();
    y = 1 - x;
    which = 1;
    cdfbet(&which, &_p, &_q, &x, &y, &a, &b, &status, &bound);
    return _p;
    //  return(incbet(p.Value(),q.Value(),u.Value()));
}


//--------------------------------------------------------------------
BDat BBetaDist::InvInc(BDat prob, BDat p, BDat q)

/*! Inverse of Incomplete Beta Function Ratio.
 */
//--------------------------------------------------------------------
{
    double _p, _q, x, y, a, b, bound;
    int which, status;
    
    a = p.Value();
    b = q.Value();
    _p = prob.Value();
    _q = 1 - _p;
    which = 2;
    cdfbet(&which, &_p, &_q, &x, &y, &a, &b, &status, &bound);
    return x;
    // return(incbi(p.Value(),q.Value(),prob.Value()));
}


//--------------------------------------------------------------------
BBetaDist::BBetaDist(BDat p, BDat q)
//--------------------------------------------------------------------
: BProbDist(), p_(p), q_(q)
{
    min_ = 0;
    max_ = 1;
    if(p_.IsUnknown() || q_.IsUnknown() || (p_<=0.0)||(q_<=0.0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use an Beta distribution with parameters",
		   "No se puede usar una distribucion beta con parametros")+
		" ("+p_+", "+q_+")");
    }
    else
    {
	//k_ = 1.0/Beta(p_,q_);
	gsl_sf_result result;
	int status;
	
	if ((status=gsl_sf_beta_e(p_.Value(), q_.Value(), &result)) != 
	    GSL_SUCCESS) {
	    BText warn(gsl_strerror(status));
	    Warning(warn);
	    wrongParameter_ = BTRUE;      
	} else {
	    k_ = 1.0 / result.val;
	}
    }
}


//--------------------------------------------------------------------
BDat BBetaDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown())  { return(BDat::Unknown()); }
    if((x<0.0)||(x>1.0)) { return(0); }
    else
    {
	if((p_==1)&&(q_==1)) { return(k_); }
	if((p_!=1)&&(q_==1)) { return(k_*(x^(p_-1.0))); }
	if((p_==1)&&(q_!=1)) { return(k_*((1.0-x)^(q_-1.0))); }
	return(k_*(x^(p_-1.0))*((1.0-x)^(q_-1.0)));
    }
}


//--------------------------------------------------------------------
BDat BBetaDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x<0.0) { return(0); }
    if(x>1.0) { return(1); }
    return(Inc(x,p_,q_));
}

//--------------------------------------------------------------------
BDat BBetaDist::Inverse (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    return(InvInc(prob,p_,q_));
}

//--------------------------------------------------------------------
BDat BBetaDist::Average   ()
//--------------------------------------------------------------------
{ return(p_/(p_+q_)); }


//--------------------------------------------------------------------
BDat BBetaDist::Varianze  ()
//--------------------------------------------------------------------
{
    return((p_*q_)/((p_+q_)*(p_+q_)*(p_+q_+1.0)));
}


//--------------------------------------------------------------------
BTStudentDist::BTStudentDist(BInt degree)
//--------------------------------------------------------------------
:  BProbDist(), degree_(degree)
{
    if(degree<=0)
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a t-Student distribution with parameters",
		   "No se puede usar una distribucion t-Student con "+
		   "parametros")+" ("+degree+")");
    }
    else
    {
	BDat g1 = LogGamma(BDat(degree_+1)/2.0);
	BDat g2 = LogGamma(BDat(degree_)/2.0);
	BDat g3 = Sqrt(degree_*BDat::Pi());
	k_ = Exp(g1-g2)/g3;
	d_ = (BDat(degree_+1)/2.0);
    }
}


//--------------------------------------------------------------------
BDat BTStudentDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    return(k_/((BDat(1)+(x*x)/degree_)^d_));
}


//--------------------------------------------------------------------
BDat BTStudentDist::Dist(BDat t)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || t.IsUnknown()) { return(BDat::Unknown()); }
    int which, status;
    double p, q, _t, df = degree_, bound;
    
    which = 1;
    _t = t.Value();
    
    cdft(&which, &p, &q, &_t, &df, &status, &bound);
    if(status==0)
	return p;
    else
	return BDat::Unknown();
    /*
      if(t<0) { return(1-Dist(-t)); }
      else	  { return(stdtr(degree_,t.Value())); }
    */
}


//--------------------------------------------------------------------
BDat BTStudentDist::Inverse(BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    int which, status;
    double p, q, t, df = degree_, bound;
    
    if(prob<=0) { return(BDat::NegInf()); }
    if(prob>=1) { return(BDat::PosInf()); }
    p = prob.Value();
    q = 1 - p;
    which = 2;
    cdft(&which, &p, &q, &t, &df, &status, &bound);
    return t;
    /*
      if(prob<=0) { return(InfNeg()); }
      if(prob>=1) { return(Inf()); }
      return(stdtri(degree_,prob.Value()));
    */
}


//--------------------------------------------------------------------
BDat BTStudentDist::Random()
//--------------------------------------------------------------------
{
    return gsl_ran_tdist(getGslRng(), degree_);
    
    // return(n01_.Random()/Sqrt(BChiSquareDist(degree_).Random()/degree_));
}


//--------------------------------------------------------------------
BNCTDist::BNCTDist(BInt degree, BDat delta)
//--------------------------------------------------------------------
: BProbDist(), degree_(degree), delta_(delta)
{
    if(degree<=0)
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a non central t distribution with parameters",
		   "No se puede usar una distribucion t no central con "
		   "parametros" )+ " ("+degree+")");
    }
    else
    {
	v_ = degree_;
	if(delta_!=0.0)
	{
	    nu_= Exp(LogGamma((v_-1.0)/2.0)-LogGamma(v_/2.0))*
		delta_*Sqrt(v_/2.0);
	}
	else
	{
	    nu_=0;
	}
	var_ = (v_/(v_-2.0))*(1.0+delta_*delta_)-nu_*nu_;
	BDat g1 = LogGamma((v_+1.0)/2.0);
	BDat g2 = LogGamma(v_/2.0);
  BDat g3 = Sqrt(degree_*BDat::Pi());
	k_ = Exp(g1-g2)/g3;
	d_ = (BDat(degree_+1.0)/2.0);
	prob0_ = BNormalDist::Dist01(-delta_);
    }
}

/*
//--------------------------------------------------------------------
  BDat BNCTDist::Dens(BDat t)
//--------------------------------------------------------------------
{
  BDat add, sum=0;
  BDat d2 = delta_*delta_;
  BDat x = (t*delta_*Sqrt(2.0))/Sqrt(v_+t*t);
  BDat lgv = LogGamma(0.5*(1+v_));
  BInt j=0;
  BDat lax = Log(Abs(x));
  BDat sx  = Sign(x);
  BDat sxj = 1;
  BDat tol = BDat::Tolerance()^2.0;
  BDat b   = Log(v_/(v_+t*t))*(0.5*(v_+1))-
	     (0.5*d2+Log(Sqrt(BDat::Pi()*v_))+LogGamma(0.5*v_));
  BDat h = -lax-lgv;
  BDat g = Exp(b+lax+lgv);
  do
  {
    BDat a1 = LogGamma(0.5*(v_+j+1.0))-LogGamma(j+1);
    if(x!=0)
    {
      BDat a2 = j*lax+a1;
      BDat a3 = a2+h;
      add = sxj*Exp(a3);
      sum += add;
    }
    else
    {
      sum = Exp(a1+h);
      add = 0;
    }
    j++;
    sxj *= sx;
  }
  while( add.IsKnown() && ( (Abs(add*g)>tol) || (j<20) ) );
  BDat p = sum*g;
  return(p);
}


//--------------------------------------------------------------------
   static BDat NCFDistEven(BInt v1, BInt v2, BDat lambda, BDat F)

// PURPOSE: Non Central Distribution Function with v2 even
//
//	      Abramowitz page 948. 26.6.22
//
//--------------------------------------------------------------------
{
  BDat x   = v2/(v1*F+v2);
  BInt N   = v2/2 - 1;
  BInt v12 = v1+v2;
  BInt i   = 0;
  BDat Lx  = lambda*x;
  BDat L1x = lambda*(1-x);
  BDat T0  = 1;
  BDat T1  = 0.5*(v12-2+Lx)*(1-x)/x;
  BDat sum = T0+T1;
  for(i=2; i<=N; i++)
  {
    BDat T2 = ((v12-2*i+Lx)*T1+L1x*T0)*(1-x)/(2*i);
    sum += T2;
    T0=T1;
    T1=T2;
  }
  BDat c = 0.5*((v12-2)*Log(x) - L1x);
  sum *= Exp(c);
  return(sum);
}
*/

//--------------------------------------------------------------------
BDat BNCTDist::Dens(BDat t)

/*! Non Central T Density Function
 *     Abramowitz page 949. 26.7.9
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || t.IsUnknown()) { return(BDat::Unknown()); }
    BDat u    = (t*(1-0.25/v_)-delta_)/Sqrt(1+0.5*t*t/v_);
    BDat noru = n01_.Dens(u);
    return(noru);
/*
  BDat d2  = delta_*delta_;
  BDat t2  = t*t;
  BDat vt2 = v_/(v_+t2);
  BDat c   = LogGamma(v_+1)-LogGamma(v_*0.5)-0.5*(v_-1)*Log(2)
	     - 0.5*vt2*d2 + 0.5*(v_+1)*Log(vt2);
  BDat h   = Hh(degree_,-(delta_*t)/Sqrt(v_+t2));
  BDat p   = (Exp(c)*h)/Sqrt(PI*v_);
  return(p);
*/
}


//--------------------------------------------------------------------
BDat BNCTDist::Dist(BDat t)

/*! Non Central T Distribution Function
 *
 *	    Continuous Univariate Distribution-2. Page 205.
 *	    Wiley series in Probability and Mathematical Statistics.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || t.IsUnknown()) { return(BDat::Unknown()); }
//return(ncstdtr(degree_,delta_.Value(),t.Value()));

  static BDat spi = Sqrt(1.0/BDat::Pi());
  if(wrongParameter_ || t.IsUnknown()) { return(BDat::Unknown()); }
  BDat tol  = BDat::Tolerance();
  BDat v    = degree_;

  BDat u    = (t*(1-0.25/v)-delta_)/Sqrt(1+0.5*t*t/v);
  BDat noru = n01_.Dist(u);
  if(v>60)
  {
    return(noru);
  }

  BDat d2   = delta_*delta_;
  BDat t2   = t*t;
  BDat x    = t/Sqrt(v+t2);
  BDat y    = delta_*Sqrt(v/(v+t2));
  BDat x2   = x*x;
  BDat y2   = y*y;
  BDat z    = -y2/2;
  BDat R    = x*spi*Exp(LogGamma(0.5*(v+1))-LogGamma(0.5*v));
  BDat S    = y*spi/Sqrt(2);
  BDat add  = tol;
  BDat ad;
  BDat ez    = Exp(z);

//BDat MR1   = MCHyp(0.5,0.5,z,tol*0.00001); //ez
//BDat MR2   = MCHyp(1.5,0.5,z,tol*0.00001); //ez*(1+2*z);
//BDat MS1   = MCHyp(0.5,1.5,z,tol*0.00001);
//BDat MS2   = MCHyp(1.5,1.5,z,tol*0.00001); //ez

  BDat MR1   = ez;
  BDat MR2   = ez*(1+2*z);
  BDat MS1   = MCHyp(0.5,1.5,z,tol*0.00001);
  BDat MS2   = ez;

  BDat sum   = n01_.Dist(-y)+R*MR1;
  BInt j;
  for(j=1; sum.IsKnown() && (add>=tol); j++)
  {
    BDat J  = j;
    BDat a  = J-0.5;
    BDat JR = J-0.5*v;
    BDat JS = J+0.5*(1-v)-1;
    BDat XJ = x2/J;
    BDat oldSum = sum;
    if(R!=0) { R *= JR*XJ; }
    if(S!=0) { S *= JS*XJ; }
    if(R!=0)
    {
      BDat b  = 0.5;
      BDat m = MR2;
      if(j>2)	   { MR2 = (b/a)*(MR1-MR2)+(2*MR2-MR1)+(z/a)*MR2; }
//    else if(j>1)	{ MR2 = MCHyp(a+1,b,z); }
      else if(j>1) { MR2 = ez*(1+4*z+4*z*z/3); } //MCHyp(a+1,b,z); }
      MR1 = m;
      sum += R*MR2/(2.0*J+1.0);
//  //Std(BText("\nj=")+j+"\ta="+a.Name()+"\tR="+R.Name()+"\tMsum="+sum.Name());
//  //Std(BText("\nMCHyp(a,b,z)=")+MCHyp(a+1,b,z).Name()+"\tMR2="+MR2.Name());
    }
    if(S!=0)
    {
      BDat b  = 1.5;
      BDat m = MS2;
      if(j>2)	   { MS2 = (b/a)*(MS1-MS2)+(2*MS2-MS1)+(z/a)*MS2; }
//    else if(j>1)	{ MS2 = MCHyp(a+1,b,z); }
      else if(j>1) { MS2 = ez*(1+2*z/3); } //MCHyp(a+1,b,z); }
      MS1 = m;
//    MS2 = MCHyp(a,b,z);
      sum -= S*MS2;
//  //Std(BText("\nj=")+j+"\ta="+a.Name()+"\tS="+S.Name()+"\tMsum="+sum.Name());
//  //Std(BText("\nMCHyp(a,b,z)=")+MCHyp(a+1,b,z).Name()+"\tMS2="+MS2.Name());
    }
    add = Abs(sum-oldSum);
  }
//Std(BText("\nBNCTDist::Dist\t")+t.Name()+"\t"+degree_+"\t"+
//    delta_.Name()+"\t"+z.Name()+"\t"+(j-1)+"\n");
  return(sum);

}


//--------------------------------------------------------------------
BDat BNCTDist::InverseAprox (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    BDat U  = BNormalDist::Inverse01(1-prob);
    BDat U2 = U*U;
    BDat U3 = U*U2;
    BDat U4 = U*U3;
    BDat U5 = U*U4;
    BDat d  = delta_;
    BDat d2 = d*d;
    BDat d3 = d*d2;
    BDat d4 = d*d3;
    BDat t  = U+d+
	(U3+U+(2.0*U2+1.0)*d+U*d2)/(v_*4.0)+
	(
	    5.0*U5+16.0*U3+3.0*U+
	    3.0*d*(4.0*U4+12.0*U2+1.0)+
	    6.0*d2*(U3+4.0*U)-
	    4.0*d3*(U2-1.0)-
	    3.0*d4*U
        )/(v_*v_*96.0);
    
    return(t);
}


//--------------------------------------------------------------------
BDat BNCTDist::Inverse(BDat prob, BDat tolerance)

/*! Returns the number whose probability is prob with this
 *  distribution.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    if(prob<=0) { return(BDat::NegInf()); }
    if(prob>=1) { return(BDat::PosInf()); }
//return(ncstdtri(degree_,delta_.Value(),prob.Value()));
//return(BProbDist::Inverse(prob,tolerance));
    
    if(prob==0) { return(min_); }
    if(prob==1) { return(max_); }
    BDat	x  = InverseAprox(prob,tolerance);
    InverseFib(x, prob, 0.01*tolerance);
    return(x);
    
}


//--------------------------------------------------------------------
BFSnedecorDist::BFSnedecorDist(BInt m, BInt n)
//--------------------------------------------------------------------
: BProbDist(), m_(m), n_(n)
{
    min_ = 0;
    if((m<1)||(n<1))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a F-Snedecor distribution with "
		   "freedom degrees less than 1",
		   "No se puede usar una distribucion F-Snedecor con "
		   "grados de libertad menores que 1 ")+
		" ("+m+", "+n+")");
    }
    else
    {
	BDat n2  = BDat(n) / BDat(2.0);
	BDat m2  = BDat(m) / BDat(2.0);
	mn_ = BDat(m) / BDat(n);
	g1_ = m2*Log(BDat(m)) + n2*Log(BDat(n)) - LogBeta(m2,n2);
	g2_ = m2 - BDat(1.0);
	g3_ = -(m2+n2);
    }
}


//--------------------------------------------------------------------
BDat BFSnedecorDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat dens = 0.0;
    if(x>0.0)
    {
	BDat lx    = Log(x);
	BDat nmx   = BDat(n_) + x * BDat(m_);
	BDat lnmx  = Log(nmx);
	BDat ldens = g1_+(g2_*lx)+(g3_*lnmx);
	dens = Exp(ldens);
////Std(BText("\nBFSnedecorDist::Dens(")+x.Name()+")="+dens.Name());
    }
    return(dens);
}


//--------------------------------------------------------------------
BDat BFSnedecorDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x<=0)	        { return(0.0); }
    else { 
	int which, status;
	double p, q, f, dfn, dfd, bound;
	
	dfn = m_;
	dfd = n_;
	f = x.Value();
	which = 1;
	cdff(&which, &p, &q, &f, &dfn, &dfd, &status, &bound);
	return p;
	//return(fdtr(n_,m_,x.Value())); 
    }
}


//--------------------------------------------------------------------
BDat BFSnedecorDist::Inverse(BDat prob, BDat tolerance)

/*! Returns the number whose probability is aprox. prob with this
 *  distribution.
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    if(prob<=0)         { return(0); }
    if(prob>=1)         { return(BDat::PosInf()); }
    
    int which, status;
    double p, q, f, dfn, dfd, bound;
    
    p = prob.Value();
    q = 1- p;
    dfn = m_;
    dfd = n_;
    which = 2;
    cdff(&which, &p, &q, &f, &dfn, &dfd, &status, &bound);
    return f;
    
    //return(fdtri(n_,m_,1-prob.Value()));
}


//--------------------------------------------------------------------
BDat BFSnedecorDist::Average	 ()
//--------------------------------------------------------------------
{
    BDat avr;
    if(n_>2)
    {
	avr = BDat(2*n_)/BDat(n_-2);
    }
    return(avr);
}


//--------------------------------------------------------------------
BDat BFSnedecorDist::Varianze	 ()
//--------------------------------------------------------------------
{
    BDat avr;
    if(n_>4)
    {
	avr = BDat(4*n_*n_*(m_+n_-2))/BDat(n_*(n_-2)*(n_-2)*(n_-4));
    }
    return(avr);
}


//--------------------------------------------------------------------
BNCFDist::BNCFDist(BInt m, BInt n, BDat delta)
//--------------------------------------------------------------------
: BProbDist(), n_(n), m_(m), delta_(delta)
{
    if((m<1)||(n<1))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a non central F-Snedecor distribution with "
		   "freedom degrees less than 1",
		   "No se puede usar una distribucion F-Snedecor no "
		   "central con grados de libertad menores que 1 ")+
		" ("+m+", "+n+")");
    }
}


//--------------------------------------------------------------------
BDat BNCFDist::Dens(BDat x)
//--------------------------------------------------------------------
{
  Error("BNCFDist:Dens unimplemented");
    return(BDat::Unknown());
}

//--------------------------------------------------------------------
BDat BNCFDist::Dist(BDat x)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    int which, status;
    double p, q, f, dfn, dfd, phonc, bound;
    
    f = x.Value();
    dfn = n_;
    dfd = m_;
    phonc = delta_.Value();
    which = 1;
    cdffnc(&which, &p, &q, &f, &dfn, &dfd, &phonc, &status, &bound);
    return p;
    
    // return(ncfdtr(m_,n_,delta_.Value(),x.Value()));
}

//--------------------------------------------------------------------
BDat BNCFDist::Inverse(BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    if(prob<=0) { return(0); }
    if(prob>=1) { return(BDat::PosInf()); }
    int which, status;
    double p, q, f, dfn, dfd, phonc, bound;
    
    p = prob.Value();
    q = 1 - p;
    dfn = n_;
    dfd = m_;
    phonc = delta_.Value();
    which = 2;
    cdffnc(&which, &p, &q, &f, &dfn, &dfd, &phonc, &status, &bound);
    return f;
    
    // return(ncfdtri(m_,n_,delta_.Value(),prob.Value()));
}

//--------------------------------------------------------------------
BDat BNCFDist::Average()
//--------------------------------------------------------------------
{
    return(BDat::Unknown());
}

//--------------------------------------------------------------------
BDat BNCFDist::Varianze()
//--------------------------------------------------------------------
{
    return(BDat::Unknown());
}

//--------------------------------------------------------------------
BZFisherDist::BZFisherDist(BInt m, BInt n)
//--------------------------------------------------------------------
: BProbDist(), m_(m), n_(n), F_(m_,n_)
{
    if((m<=3)||(n<=3))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a z-Fisher distribution with parameters "
		   "less than 2 ",
		   "No se puede usar una distribucion z-Fisher con "
		   "parametros menores que 2")+
		" ("+m+", "+n+")");
    }
}


//--------------------------------------------------------------------
BDat BZFisherDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = Exp(BDat(2.0)*x);
    return(BDat(2.0)*y*F_.Dens(y));
}


//--------------------------------------------------------------------
BDat BZFisherDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = Exp(BDat(2.0)*x);
    return(F_.GetDist(y));
}

//--------------------------------------------------------------------
BDat BZFisherDist::Inverse (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
  return(0.5*Log(F_.Inverse(prob)));
}

//--------------------------------------------------------------------
  BDat BZFisherDist::Average   ()
//--------------------------------------------------------------------
{ return(BDat(n_)/BDat(n_-2)); }


//--------------------------------------------------------------------
BDat BZFisherDist::Varianze  ()
//--------------------------------------------------------------------
{
    return(BDat(2*n_*n_*(m_+n_-2))/BDat(n_*(n_-2)*(n_-2)*(n_-4)));
}


//--------------------------------------------------------------------
BWeibullDist::BWeibullDist(BDat a, BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BProbDist(), a_(a), sigma_(sigma), nu_(nu)
{
  min_ = nu;
  if((a_<=0)||(sigma_<=0))
  {
    wrongParameter_ = BTRUE;
    Warning(I2("Cannot use a Weibull distribution with parameters",
	       "No se puede usar una distribucion de Weibull con parametros")+
	       " ("+a.Name()+", "+sigma.Name()+", "+nu.Name()+")");
  }
  k_ = a_/sigma_;
}


//--------------------------------------------------------------------
BDat BWeibullDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x<nu_) { return(0); }
    BDat y  = (x-nu_)/sigma_;
    BDat ya = y^a_;
    return((ya/y)*Exp(-ya)*k_);
}


//--------------------------------------------------------------------
BDat BWeibullDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x<nu_) { return(0); }
    BDat y  = (x-nu_)/sigma_;
    BDat ya = y^a_;
    return(1-Exp(-ya));
}

//--------------------------------------------------------------------
BDat BWeibullDist::InverseAprox (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
    return(Average());
}

//--------------------------------------------------------------------
BDat BWeibullDist::Average   ()
//--------------------------------------------------------------------
{
  Error("BWeibullDist::Average Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BDat BWeibullDist::Varianze  ()
//--------------------------------------------------------------------
{
  Error("BWeibullDist::Average Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BLaplaceDist::BLaplaceDist(BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BProbDist(), sigma_(sigma), nu_(nu)
{
    if((sigma_<=0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Laplace distribution with parameters",
	       "No se puede usar una distribucion de Laplace con parametros")+
		" ("+sigma.Name()+", "+nu.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BLaplaceDist::Dens(BDat x)

/*! Returns Laplace(x)
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = (x-nu_)/sigma_;
    return(Exp(-Abs(y))/(sigma_*2));
}


//--------------------------------------------------------------------
BDat BLaplaceDist::Dist(BDat x)

/*! Returns the Cumulative-Laplace(x)
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = (x-nu_)/sigma_;
  //if(y<=0) { return(Exp(y)/BDat(2)*sigma_); }
  //else  { return(1 - Exp(-y)/BDat(2)); }
 
  // Based on Continuous Univariate Distributions-2
  // N.L. Johnson, S.Kotz
 
    if(y<=nu_) { return(Exp(y)/BDat(2)*sigma_); }
    else  { return(1 - Exp(-y)/BDat(2)); }
}


//--------------------------------------------------------------------
BDat BLaplaceDist::InverseAprox (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  Error("BLaplaceDist::InverseAprox Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BDat BLaplaceDist::Inverse (BDat y)
//! PURPOSE:Returns de inverse of the cumulative density function for y
//--------------------------------------------------------------------
{
    if(wrongParameter_ || y.IsUnknown()) { return(BDat::Unknown()); }
    if (y <= 0.5)
	return(sigma_*Log(2*y)+nu_);
    else
	return(nu_ - sigma_*Log(2-2*y));
}


//--------------------------------------------------------------------
BDat BLaplaceDist::Average   ()
//--------------------------------------------------------------------
{
    //Not implemented
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BDat BLaplaceDist::Varianze  ()
//--------------------------------------------------------------------
{
    //Not implemented
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BCauchyDist::BCauchyDist(BDat nu, BDat sigma)
//--------------------------------------------------------------------
: BProbDist(), sigma_(sigma), nu_(nu)
{
    if((sigma_<=0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Cauchy distribution with parameters",
	       "No se puede usar una distribucion de Cauchy con parametros")+
		" ("+sigma.Name()+", "+nu.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BCauchyDist::Dens(BDat x)

/*! Returns Cauchy density function value in x
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = (x-nu_)/sigma_;
    return(BDat(1)/(BDat::Pi()*sigma_*(BDat(1)+(y^2))));
}


//--------------------------------------------------------------------
BDat BCauchyDist::Dist(BDat x)

/*! Returns Cauchy cumulative function value in x
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BDat y = (x-nu_)/sigma_;
    return(BDat(0.5)+ATan(y)/BDat::Pi());
}

//--------------------------------------------------------------------
BDat BCauchyDist::InverseAprox (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  Error("BCauchyDist::InverseAprox Not implemented");
    return(BDat::Unknown());
}

//--------------------------------------------------------------------
BDat BCauchyDist::Average   ()
//--------------------------------------------------------------------
{
  Error("BCauchyDist::Average Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BDat BCauchyDist::Varianze  ()
//--------------------------------------------------------------------
{
  Error("BCauchyDist::Varianze Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BParetoDist::BParetoDist(BDat a, BDat k)
//--------------------------------------------------------------------
: BProbDist(), a_(a), k_(k)
{
    min_ = k;
    if((a_<=0)||(k_<=0))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Pareto distribution with parameters",
	       "No se puede usar una distribucion de Pareto con parametros")+
		" ("+a.Name()+", "+k.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BParetoDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x>=k_) { return((a_/x)*((k_/x)^a_)); }
    else	    { return(0); }
}


//--------------------------------------------------------------------
BDat BParetoDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    if(x>=k_) { return(BDat(1)-((k_/x)^a_)); }
    else	    { return(0); }
}


//--------------------------------------------------------------------
BDat BParetoDist::InverseAprox (BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
  Error("BParetoDist::InverseAprox Not implemented");
    return(BDat::Unknown());
}

//--------------------------------------------------------------------
BDat BParetoDist::Inverse (BDat prob)
//--------------------------------------------------------------------
{
  if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
  if(prob<=0) return k_; 
  if(prob>=1) return BDat::PosInf();

//DQCComments: Como no se incluye gsl_cdf.h (para ello es necesaria la versión 1.4.de gsl)
//debe comentarse la siguiente línea
  return(gsl_cdf_pareto_Pinv(prob.Value(), a_.Value(), k_.Value())); 
// Y descomentar la siguiente
  //  return(BDat::Unknown());
}
  

//--------------------------------------------------------------------
BDat BParetoDist::Average   ()
//--------------------------------------------------------------------
{
  Error("BParetoDist::Average Not implemented");
    return(BDat::Unknown());
}


//--------------------------------------------------------------------
BDat BParetoDist::Varianze  ()
//--------------------------------------------------------------------
{
  Error("BParetoDist::Varianze Not implemented");
    return(BDat::Unknown());
}



//--------------------------------------------------------------------
BDiscreteUniformDist::BDiscreteUniformDist(BInt min, BInt max)
//--------------------------------------------------------------------
: BDiscreteDist(), min_(min), max_(max), len_(max_-min_+1)
{
    if(min_>=max_)
    {
	wrongParameter_ = BTRUE;
       Warning(I2("Cannot use a discrete uniform distribution with parameters",
		  "No se puede usar una distribucion de uniforme discreta "
		  "con parametros")+
	       " ("+min_+", "+max_+")");
    }
}

//--------------------------------------------------------------------
BDat BDiscreteUniformDist::Average()
//--------------------------------------------------------------------
{
    return(BDat(min_ + max_) / 2 );
}

//--------------------------------------------------------------------
BDat BDiscreteUniformDist::Varianze()
//--------------------------------------------------------------------
{
    return(BDat(1 + 3*len_ + 2*len_*len_)/6.0);
}

//--------------------------------------------------------------------
BDat BDiscreteUniformDist::Dens(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BInt k = (BInt)Floor(x).Value();
    if((k<min_)||(k>max_)) { return(0); }
    return(1.0/BDat(len_));
}

//--------------------------------------------------------------------
BDat BDiscreteUniformDist::Dist(BDat x)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BInt k = (BInt)Floor(x).Value();
    if(k< min_) { return(0); }
    if(k>=max_) { return(1); }
    return(BDat(k-min_+1)/BDat(len_));
}

//--------------------------------------------------------------------
BDat BDiscreteUniformDist::Inverse(BDat prob, BDat tolerance)
//--------------------------------------------------------------------
{
    if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    return(BInt(Round(min_ + len_ * prob - 0.5).Value()));
}


//--------------------------------------------------------------------
BBinomialDist::BBinomialDist(BDat N, BDat p)

/*! Creates an object aimed to work whith a binomial distribution. 
 *  A binomial distribution counts the number of successes from a 
 *  fixed number of trials whith a fixed success probability.
 * \param N Number of trials.
 * \param p Success probability.
 */
//--------------------------------------------------------------------
: BDiscreteDist(), N_(N), p_(p), q_(1-p)
{
    min_ = 0;
    max_ = N;
    logp_ = Log(p_);
    logq_ = Log(q_);
    logGammaN_ = LogGamma(1+N_);
    if((N_<=0)||(p_<0)||(p_>1))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Binomial distribution with parameters",
	      "No se puede usar una distribucion de Binomial con parametros")+
		" ("+N_+", "+p.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BBinomialDist::Dens(BDat x)

/*! Returns Prob(x successes) for N_ trials with p_ success probability
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BInt k = (BInt)Floor(x).Value();
    if((k<0)||(k>N_)) { return(0); }
    return(Exp(logGammaN_-LogGamma(1+N_-k)-LogFact(k)+k*logp_+(N_-k)*logq_));
}


//--------------------------------------------------------------------
BDat BBinomialDist::Dist(BDat x)

/*! Returns Sum_{0}^{x} ProbBinomial(i,N_,p_)
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    int k = (int)Floor(x).Value();
    if(k<0)	{ return(0); }
    else if(k>N_) { return(1); }
    else		{
	int which, status;
	double p, q, s, xn, pr, ompr, bound;
	
	s = k;
	xn = N_.Value();
	pr = p_.Value();
	ompr = 1 - pr;
	which = 1;
	cdfbin(&which, &p, &q, &s, &xn, &pr, &ompr, &status, &bound);
	return p;
    }
    
}


//--------------------------------------------------------------------
BDat BBinomialDist::Inverse(BDat x, BDat tolerance)
/*! Returns The Inverse from Sum_{0}^{x} ProbBinomial(i,N_,p_)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  if(x<=Dens(0)) { return(0); }
    double p, q, s, xn, pr, ompr, bound;
    int which, status;
    //Using dcdflib 
    p = x.Value();
    q = 1-p;
    xn = N_.Value();
    pr = p_.Value();
    ompr = 1-pr;
    which = 2;
    cdfbin(&which, &p, &q, &s, &xn, &pr, &ompr, &status, &bound);
    if(status!=0)
	return(BDat::Unknown());
    else {
	double x0 = floor(s);
	double fx0;
	double x1 = x0 + 1.0;
	double fx1 ;
	
	if (x1>xn) {
	    return x0;
	}
	cumbin(&x0,&xn,&pr,&ompr,&fx0,&q);
	/* may be, there is an incremental way */
	cumbin(&x1,&xn,&pr,&ompr,&fx1,&q);
	int K1 = 1;
	return fabs(fx1-p)<=spmpar(&K1) ? x1 : x0;
    }
}   


//--------------------------------------------------------------------
BDat BBinomialDist::Average ()

/*! Returns E[x]
 */
//--------------------------------------------------------------------
{
    return(N_*p_);
}


//--------------------------------------------------------------------
BDat BBinomialDist::Varianze ()

/*! Returns var(x)
 */
//--------------------------------------------------------------------
{
  return(N_*p_*q_);
}


//--------------------------------------------------------------------
BNegBinomialDist::BNegBinomialDist(BDat N, BDat P)

/*! Creates an object aimed to work whith a negative binomial distribution. 
 *  A negative binomial distribution counts the number of successes
 *  before a fixed number of failures whith a fixed success probability.
 * \param N Number of succeses.
 * \param P Success probability.
 */
//--------------------------------------------------------------------
:  BDiscreteDist(), N_(N), P_(P)
{
    min_ = 0;
    max_ = N;
    Q_ = BDat(1)+P_;
    p_ = P_/Q_;
    q_ = BDat(1)-P_;
    invQ_	 = BDat(1)/Q_;
    logp_	 = Log(p_);
    nu_ = N_*Log(q_)-LogGamma(N_);
    if((N_<=0)||(P_<0)||(P_>1))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a NegBinomial distribution with parameters",
		   "No se puede usar una distribucion de NegBinomial con "
		   "parametros")+
		" ("+N+", "+P_.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BNegBinomialDist::Dens(BDat x)

/*! Returns NegBinomial(x,N_,P_) 
 *  = prob of getting x succeses before the N_-th failures
 *
 *    ALGORITHM: NB(x,N_,P_) = P_ * DensBinomial(N_-1,k+N_-1,P_)
 *    DensBinomial uses a correct & efficient algorithm.
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  BInt k = (BInt)Floor(x).Value();
  return(gsl_ran_negative_binomial_pdf(k,P_.Value(),N_.Value()));
}


//--------------------------------------------------------------------
BDat BNegBinomialDist::Dist(BDat x)

/*! Returns the cumulative negative binomial distribution value for x
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
  BInt k = (BInt)Floor(x).Value();
//return(1-gsl_sf_beta_inc (k+1, N_.Value(), 1-P_.Value()));
//return(gsl_cdf_negative_binomial_P(k,P_.Value(),N_.Value()));
/* */
    double p, q, s, xn, pr, ompr, bound;
    int which, status; 
    s = x.Value();
    xn = N_.Value();
    pr = P_.Value();
    ompr = 1-pr;
    which = 1;
    cdfnbn(&which,&p,&q,&s,&xn,&pr,&ompr,&status,&bound);
    if(status==0) 
	return(p);
    else
	return(BDat::Unknown());
/* */
}

//--------------------------------------------------------------------
BDat BNegBinomialDist::Inverse(BDat x, BDat tolerance)

/*! Returns The Inverse from Sum_{0}^{x} ProbNegBinomial(i,N_,p_)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
//return(BDiscreteDist::Inverse(x,tolerance));
  if(x<=Dens(0)) { return(0); }

    double p, q, s, xn, pr, ompr, bound;
    int which, status; 
    p = x.Value();
    q = 1-p;
    xn = N_.Value();
    pr = P_.Value();
    ompr = 1-pr;
    which = 2;
    cdfnbn(&which,&p,&q,&s,&xn,&pr,&ompr,&status,&bound);
    if(status!=0) 
	return(BDat::Unknown());
    else {
	double x0 = floor(s);
	double fx0;
	double x1 = x0 + 1.0;
	double fx1 ;
	
	cumnbn(&x0,&xn,&pr,&ompr,&fx0,&q);
	cumnbn(&x1,&xn,&pr,&ompr,&fx1,&q);
	int K1 = 1;
	return fabs(fx1-p)<=spmpar(&K1) ? x1 : x0;
    }

}


//--------------------------------------------------------------------
BDat BNegBinomialDist::Average   ()

/*! PURPOSE: Returns E[x]
 */
//--------------------------------------------------------------------
{
    return(N_*(1-P_)/P_);
}


//--------------------------------------------------------------------
BDat BNegBinomialDist::Varianze  ()

/*! Returns var(x)
 */
//--------------------------------------------------------------------
{
    return(N_*(1-P_)/(P_*P_));
}



//--------------------------------------------------------------------
BGeometricDist::BGeometricDist(BDat P)
//--------------------------------------------------------------------
: BNegBinomialDist(1,P)
{
}


//--------------------------------------------------------------------
BPoissonDist::BPoissonDist(BDat fi)

/*! Creates an object aimed to deal with a Poisson distrobution
 * A Poisson distribution counts how many time an event takes place.
 * \param fi: Ocurrence ratio.
 */
//--------------------------------------------------------------------
    :  BDiscreteDist(), fi_(fi)
{
    min_ = 0;
    logFi_ = Log(fi);
    if(fi_<=0)
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Poisson distribution with parameters",
	       "No se puede usar una distribucion de Poisson con parametros")+
		" ("+fi.Name()+")");
    }
}


//--------------------------------------------------------------------
BDat BPoissonDist::Dens(BDat x)

/*! Returns Poisson(x,fi)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_|| x.IsUnknown()) { return(BDat::Unknown()); }
  BInt k = (BInt)Floor(x).Value();
  if(k<0) { return(0); }
  else    { return(Exp(k*logFi_-LogFact(k)-fi_)); }
}


//--------------------------------------------------------------------
BDat BPoissonDist::Dist(BDat x)

/*! Returns Sum{0..x}.Poisson(i,fi)
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown());}
  double p, q, s, xlam, bound;
  int which, status; 
  s = round((double) x.Value());
  xlam = (double) fi_.Value();
  which = 1;
  cdfpoi(&which, &p, &q, &s, &xlam, &status, &bound);
  if(status==0) 
  {
	  return((p<=0.5)?p:1.0-q);
  }
  else if(status==1) 
  {
	  return(0.0);
  }
  else if(status==2) 
  {
	  return(1.0);
  }
  else 
  {
    Error(BText("[BPoissonDist::Dist] cdfpoi fails at ")+
    "\n  x="+x.Name()+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(BDat::Unknown());
  }
/*
  else if(status<0) 
  {
    Warning(BText("[BPoissonDist::Dist] cdfpoi bound exceeded at ")+
    "\n  x="+x.Name()+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(bound);
  }
  else 
  {
    Error(BText("[BPoissonDist::Dist] cdfpoi fails at ")+
    "\n  p="+x.Name()+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(BDat::Unknown());
  }
*/
}


//--------------------------------------------------------------------
BDat BPoissonDist::Inverse(BDat p, BDat tolerance)

/*! Returns the maximum x matching Poisson(x,fi) <= p
 */
//--------------------------------------------------------------------
{
  if(wrongParameter_ || p.IsUnknown()) { return(BDat::Unknown()); }
  if(p<=Dens(0)) { return(0); }
  return(BDiscreteDist::Inverse(p,tolerance));
/*
  if(wrongParameter_|| x.IsUnknown()) { return(BDat::Unknown());}


  double p, q, s, xlam, bound=0.0;
  int which, status=0; 
  
  p = (double) x.Value();
  q = 1.0-p;
  which = 2;
  xlam=(double) fi_.Value();
  cdfpoi(&which, &p, &q, &s, &xlam, &status, &bound);
  if(status==0) 
  {
	  return(s);
  }
  else if(status==1) 
  {
    Warning(BText("[BPoissonDist::Dist] the answer of cdfpoi appears to be lower than lowest search bound ")+
    "\n  p="+x.Name()+
    "\n  s="+s+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(s);
  }
  else if(status==2) 
  {
    Warning(BText("[BPoissonDist::Dist] the answer of cdfpoi appears to be higher than greatest search bound ")+
    "\n  p="+x.Name()+
    "\n  s="+s+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(BDat::PosInf());
  }
  else 
  {
    Error(BText("[BPoissonDist::Dist] cdfpoi fails at ")+
    "\n  p="+x.Name()+
    "\n  s="+s+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(BDat::Unknown());
  }
/*
  else if(status<0) 
  {
    Warning(BText("[BPoissonDist::Inverse] cdfpoi bound exceeded at ")+
    "\n  p="+x.Name()+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(bound);
  }
  else 
  {
    Error(BText("[BPoissonDist::Inverse] cdfpoi fails at ")+
    "\n  p="+x.Name()+
    "\n  status="+status+
    "\n  bound="+bound);
	  return(BDat::Unknown());
  }
*/
}


//--------------------------------------------------------------------
BDat BPoissonDist::Average   ()

/*! Return E[x]
 */
//--------------------------------------------------------------------
{
    return(fi_);
}


//--------------------------------------------------------------------
BDat BPoissonDist::Varianze  ()

/*! Returns var(x)
 */
//--------------------------------------------------------------------
{
    return(fi_);
}




//--------------------------------------------------------------------
BGenCountDist::BGenCountDist(BDat a, BDat v)

/*! Creates an object aimed to deal with a Generic Counting distribution
 *  depending just in average and variance parameters and that could be 
 *    - Binomial:                 average>variance
 *    - Poisson:                  average=variance
 *    - Negative Binomial:        average<variance
 */
//--------------------------------------------------------------------
:  BDiscreteDist(), a_(a), v_(v), 
   cd_(NULL), m_(BDat::Unknown()), p_(BDat::Unknown())
{
  subType_ = Sign(a_-v_).Value();
  m_ = a_*a_/Abs(a_-v_);
  p_ = 1.0 - min(v_/a_,a_/v_);
  if(subType_==1)
  {
    cd_ = new BBinomialDist(m_.Value(),p_);
  }
  else if(subType_==0)
  {
    cd_ = new BPoissonDist(a_);
  }
  if(subType_==-1)
  {
    cd_ = new BNegBinomialDist(m_.Value(),1-p_);
  }
}

//--------------------------------------------------------------------
 BGenCountDist::~BGenCountDist()
//--------------------------------------------------------------------
{
  if(cd_) { delete cd_; }
}

//--------------------------------------------------------------------
BDat BGenCountDist::Dens(BDat x)

/*! Returns PDF
 */
//--------------------------------------------------------------------
{
  BDat d;
  if(x>=0 && cd_) { d = cd_->Dens(x); }
  return(d);
}


//--------------------------------------------------------------------
BDat BGenCountDist::Dist(BDat x)

/*! Returns CDF
 */
//--------------------------------------------------------------------
{
  BDat d;
  if(x>=0 && cd_) { d = cd_->Dist(x); }
  return(d);
}

//--------------------------------------------------------------------
BDat BGenCountDist::Inverse(BDat p, BDat tolerance)

/*! Returns the maximum x matching Poisson(x,fi) <= p
 */
//--------------------------------------------------------------------
{
  BDat i;
  if(cd_) { i = cd_->Inverse(p,tolerance); }
  return(i);
}

//--------------------------------------------------------------------
BZeroInflGenCountDist::BZeroInflGenCountDist(BDat p0, BDat a, BDat v)

/*! Creates an object aimed to deal with a Zero-Inflated Generic 
 *  Counting distribution depending just in probability of zero, 
 *  average and variance parameters. Non zeroes values minus one
 *  has one of these distributions
 *    - Binomial:                 average>variance
 *    - Poisson:                  average=variance
 *    - Negative Binomial:        average<variance
 */
//--------------------------------------------------------------------
:  BDiscreteDist(), p0_(p0), 
   gc_(NULL)
{
  gc_ = new BGenCountDist(a,v);
}

//--------------------------------------------------------------------
 BZeroInflGenCountDist::~BZeroInflGenCountDist()
//--------------------------------------------------------------------
{
  if(gc_) { delete gc_; }
}

//--------------------------------------------------------------------
BDat BZeroInflGenCountDist::Dens(BDat x)

/*! Returns PDF
 */
//--------------------------------------------------------------------
{
  BDat d;
  if(x>=0 && x<1) { d=p0_; }
  else if(gc_) { d = (1-p0_)*gc_->Dens(x); }
  return(d);
}


//--------------------------------------------------------------------
BDat BZeroInflGenCountDist::Dist(BDat x)

/*! Returns CDF
 */
//--------------------------------------------------------------------
{
  BDat d;
  if(x>=0 && x<1) { d=p0_; }
  else if(gc_) { d = p0_+(1-p0_)*gc_->Dist(x-1); }
  return(d);
}

//--------------------------------------------------------------------
BDat BZeroInflGenCountDist::Inverse(BDat p, BDat tolerance)

/*! Returns the maximum x matching Poisson(x,fi) <= p
 */
//--------------------------------------------------------------------
{
  BDat i;
  if(p<=p0_) { i=0; }
  else if(gc_) { i = gc_->Inverse((p-p0_)/(1-p0_),tolerance)+1; }
  return(i);
}

//--------------------------------------------------------------------
BHypergeometricDist::BHypergeometricDist(BInt N, BInt P, BInt n)

/*! Creates an object aimed to work whith an hypergeometric distribution. 
 *  An hypergeometric distribution counts the number of successful 
 *  extractions from a pool with a fixed number of items and a fixed 
 *  initial success probability.  
 * \param N Number of items.
 * \param P Numbre of succesful items.
 * \param n Number of extractions
 */
//--------------------------------------------------------------------
: BDiscreteDist(), N_(N), n_(n), P_(P), Q_(N-P), k_()
{
    //k_= LogFact(P_)+LogFact(Q_)+LogFact(N_-n_)+LogFact(n_)-LogFact(N_);
    min_ = n_-Q_;
    if(min_<0) { min_ = 0; }
    max_ = P_;
    if(max_>n_) {max_ = n_; }
    if((N_<=0)||(P_<0)||(Q_<0)||(n_<0)||(n_>N_))
    {
	wrongParameter_ = BTRUE;
	Warning(I2("Cannot use a Hypergeometric distribution with parameters",
		   "No se puede usar una distribucion de Hypergeometric con "
		   "parametros")+
		" ("+N_+", "+P_+", "+n_+")");
    }
}


//--------------------------------------------------------------------
BDat BHypergeometricDist::Dens(BDat x)

/*! Resturns HyperG(x,N_,P_,n_)
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    
    BInt k = (BInt)Floor(x).Value();
    BDat aux1,aux2,aux3;
    aux1 = LogFact(P_)- LogFact(P_-k)- LogFact(k);
    aux2 = LogFact(Q_)- LogFact(Q_-n_+k)- LogFact(n_-k);
    aux3 = LogFact(N_)- LogFact(N_-n_)- LogFact((int)(n_));
    return(Exp(aux1+aux2-aux3));
}


//--------------------------------------------------------------------
BDat BHypergeometricDist::Dist(BDat x)

/*! Resturns Sum{0..x}.HyperG(i,N_,P_,n_)
 * Bad computational behaviour, it must be changed when gsl available
 */
//--------------------------------------------------------------------
{
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    BInt k = (BInt)Floor(x).Value();
    if(k<min_)	    { return(0); }
    else if(k>max_) { return(1); }
    else	    { return(Summ((BInt)min_.Value(), k)); }
}


//--------------------------------------------------------------------
BDat BHypergeometricDist::Average   ()

/*! Returns E[x]
 */
//--------------------------------------------------------------------
{
    return(n_*P_/N_);
}


//--------------------------------------------------------------------
BDat BHypergeometricDist::Varianze  ()

/*! Returns var(x)
 */
//--------------------------------------------------------------------
{
    return((BDat(n_*P_*Q_*(N_-n_))/BDat((N_-1)*N_*N_)) );
}


//--------------------------------------------------------------------
// BDat BHypergeometricDist::Inverse(BDat x, BDat tolerance)
//  Not implemented, waiting for gsl development
//--------------------------------------------------------------------


