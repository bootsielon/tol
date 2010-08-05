/* tol_bprdist.h: Definition of class BProbDist (prd).
                  GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decisi�, SL (Spain [EU])

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

#ifndef TOL_BPRDIST_H
#define TOL_BPRDIST_H 1

#include <limits>

#include <tol/tol_bmatfun.h>
#include <tol/tol_brealfun.h>
//#include <gsl/gsl_nan.h>
//#include <gsl/gsl_rng.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class TOL_API BNormalDist;
class TOL_API BProbDist;
class TOL_API BInvDFunction;
class TOL_API BDensFunction;
class TOL_API BDensFunction;

class TOL_API BInvDFunction : public BInitialValueProblem
{
    BProbDist* probDist_;
public :
    BInvDFunction(BProbDist* probDist)
		: BInitialValueProblem(), probDist_(probDist) { }
    BDat Dy(BDat x, BDat y);
};

class TOL_API BDensFunction : public BRRFunction
{
    BProbDist* probDist_;
public :
    BDensFunction(BProbDist* probDist)
		: BRRFunction(), probDist_(probDist) { }
    void Evaluate(BDat& y, const BDat& x);
};

class TOL_API BDistFunction : public BRRFunction
{
    BProbDist* probDist_;
    BDat oldx_;
    BDat oldy_;
	
public :
    BDistFunction(BProbDist* probDist)
		: BRRFunction(), probDist_(probDist), oldx_(), oldy_() { }
    void Evaluate(BDat& y, const BDat& x);
};

//--------------------------------------------------------------------
class TOL_API BProbDist

/*! Abstract class for derive probability distribution classes.
 *
 * NOTE: At least one of Varianze or Desviation member functions must be
 *	 overladed in each inheritage subclasse. In other case you can fall
 *	 in an infinite cycle.
 */
//--------------------------------------------------------------------
{
protected:
  static BNormalDist n01_;
  static void * rng_;  // es un gsl_rng
  static unsigned long int seed_;
  static void InitGSLRand(long unsigned int);
  
  BInvDFunction * invD_;
  BDensFunction * density_;
  BDistFunction * distribution_;
  bool wrongParameter_;
  BDat min_;
  BDat max_;
  
  void SetFunctions()
  {
    if(!invD_)
    {
      invD_         = new BInvDFunction(this);
      density_      = new BDensFunction(this);
      distribution_ = new BDistFunction(this);
    }
  }

public:
  static bool InitializeClass();
  // contructors and destructors:
  //! Does nothing for static members initialization

  BProbDist()
  : invD_(NULL), density_(NULL), distribution_(NULL),
  wrongParameter_(BFALSE),min_(BDat::NegInf()),max_(BDat::PosInf())
  {
  }
  virtual ~BProbDist() 
  {
    if(invD_)
    {
      delete invD_;
      delete density_;
      delete distribution_;
    }
  }
  
  bool WrongParameter() const { return(wrongParameter_); } 
  const BDat& Min() const { return(min_); }
  const BDat& Max() const { return(max_); }

  // Implementation:
  virtual BDat Random	    ();
  virtual BDat Average	    ()= 0;
  virtual BDat Varianze	    () { return(Desviation()^2); }
  virtual BDat Desviation   () { return(Sqrt(Varianze())); }
  virtual BDat NIntegrate   (BDat from, BDat until);
  virtual BDat InverseAprox (BDat prob, BDat tolerance);
  BInt InverseFrom  (BDat& x, BDat prob, BDat tolerance);
  BDat InverseFib   (BDat& x, BDat prob, BDat tolerance);
  virtual BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  virtual BDat Dist	    (BDat) = 0;
  virtual BDat Dens	    (BDat) = 0;
  virtual BDat CalcProb	    (BDat a, BDat b)
  {
    return(NIntegrate(a,b));
  }
  BDat Prob(BDat a, BDat b)
  {
    if(a.IsKnown() && b.IsKnown() && (a<b))
    {
      if(min_.IsKnown() && (a<min_)) { a = min_; }
      if(max_.IsKnown() && (b>max_)) { b = max_; }
      return(CalcProb(a,b));
    }
    return(0);
  }
  virtual BDat GetDist (BDat x) 
  { 
    SetFunctions();
    return((*distribution_)[x]); 
  }
  virtual BDat GetDens (BDat x) 
  { 
    SetFunctions();
    return((*density_)[x]); 
  }
  
  static void * rng();
  static void PutRandomSeed(unsigned long int seed);
  static unsigned long int GetRandomSeed();
};

//--------------------------------------------------------------------
// continuous distributions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
class TOL_API BUniformDist : public BProbDist

/*! Uniform probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat length_;

public:
  // contructors and destructors:
  //! Does nothing for static members initialization
  BUniformDist(BDat min=0, BDat max=1) : length_(max-min)
  {
    min_ = min;
    max_ = max;
  }

  // Implementation: prd.cpp
  static BDat Dist01  (BDat x);
  static BDat Dens01  (BDat x);
  static BDat Random01();

  BDat Average	 ()	   { return(min_+length_/2.0); }
  BDat Varianze	 ()	   { return(length_*length_/12.0); }
  BDat Random	 ()	   { return(InvNorm(Random01())); }
  BDat Norm	 (BDat x)  { return((x-min_)/length_); }
  BDat InvNorm	 (BDat y)  { return(y*length_+min_); }
  BDat Min	 ()	   { return(min_); }
  BDat Max	 ()	   { return(max_); }
  BDat Length	 ()	   { return(length_); }
  BDat Dist	 (BDat x)  { return(Dist01(Norm(x))); }
  BDat Dens	 (BDat x)  { return(Dens01(Norm(x))); }
  BDat GetDist (BDat x)	   { return(Dist(x)); }
  BDat GetDens (BDat x)	   { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return((b-a)/length_); }
  BDat Inverse (BDat x, BDat tolerance=0.1);
};


//--------------------------------------------------------------------
class TOL_API BNormalDist : public BProbDist

/*! Normal probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat nu_;
  BDat sigma_;

public:
  // contructors and destructors:
  //! Does nothing for static members initialization
  BNormalDist(const BStaticInit& unused): nu_(unused),sigma_(unused) { }
  BNormalDist(BDat nu=0, BDat sigma=1);

  // Implementation: prd.cpp
  static BDat Random01	 ();
  static BDat Inverse01	 (BDat p);
  static BDat Dist01	 (BDat x);
  static BDat Dens01	 (BDat x);

  BDat Random	 ()	  { return(InvNorm(Random01())); }
  BDat Average	 ()	  { return(nu_); }
  BDat Varianze	 ()	  { return(sigma_^2); }
  BDat Desviation()	  { return(sigma_); }
  BDat Nu	 ()	  { return(nu_); }
  BDat Sigma	 ()	  { return(sigma_); }
  BDat Norm	 (BDat x) { return((x-nu_)/sigma_); }
  BDat InvNorm	 (BDat y) { return(y*sigma_+nu_); }
  BDat Dist	 (BDat x) 
  { 
    if(wrongParameter_ || x.IsUnknown()) { return(BDat::Unknown()); }
    return(Dist01(Norm(x))); 
  }
  BDat Dens	 (BDat x); 
  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) 
  { 
    if(a<=nu_) { return(Dist( b)-Dist( a)); }
    else       { return(Dist(-a)-Dist(-b)); } 
  }
  BDat Inverse	 (BDat prob,
		  BDat tolerance = BDat::Tolerance()); 
  static BDat I(BInt n, BDat x, BDat tol=BDat::Tolerance());
};


//--------------------------------------------------------------------//
class TOL_API BTruncatedDist : public BProbDist

/*! Truncates any probability distribution
 */
//--------------------------------------------------------------------//
{
protected:
  BDat& A_;  //left bound
  BDat& B_;  //right bound
  BDat fA_; //density at left bound 
  BDat fB_; //density at right bound 
  BDat FA_; //cumulated probability at left bound 
  BDat FB_; //cumulated probability at right bound 
  BDat F1A_; //complemntary of cumulated probability at left bound 
  BDat F1B_; //complemntary of cumulated probability at right bound 
  BDat FAB_; //interval probability
  bool useLeft_;

public:
  BTruncatedDist();
 ~BTruncatedDist() { }
  virtual bool SetBounds(BDat A, BDat B);
  virtual BProbDist& NonTruncated() = 0;

  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Dist	     (BDat x);
  BDat Dens	     (BDat x);
  BDat Inverse	 (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Random	   ();
};

//--------------------------------------------------------------------//
class TOL_API BTruncatedNormalDist : public BTruncatedDist

/*! Truncated normal probability distribution definition & code
 *  for some methods. 
 *  Based on N. Johnson, S.Kotz, Continuous Univariate Distributions, 
 *  Chapter 13
 */
//--------------------------------------------------------------------//
{
public:
  BNormalDist normal_;
  // contructors and destructors:
  BTruncatedNormalDist(BDat A, BDat B, BDat nu=0, BDat sigma=1);
  // Implementation:
  BProbDist& NonTruncated() { return(normal_); }

  BDat Average	 ();
  BDat Varianze	 ();
  BDat Desviation()	  { return(Sqrt(Varianze())); }
  BDat Nu	       ()	  { return(normal_.Nu()); }
  BDat Sigma	   ()	  { return(normal_.Sigma()); }
  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
};


//--------------------------------------------------------------------//
class TOL_API BClosestInsideDist : public BTruncatedNormalDist

/*! Deterministic distribution limit of truncated normal when sigma
 *  trends to 0
 *  Random method returns allways the closest point to average inside
 *  the truncation interval.
 */
//--------------------------------------------------------------------//
{
protected:
  BDat C_;
public:
  BClosestInsideDist(BDat A, BDat B, BDat nu=0);
 ~BClosestInsideDist() { }
  bool SetBounds(BDat A, BDat B);

  BDat Dist	     (BDat x);
  BDat Dens	     (BDat x);
  BDat Inverse	 (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Random	   ();
};

/*! Truncated normal probability distribution definition & code
 *  for some methods. 
 *  Based on N. Johnson, S.Kotz, Continuous Univariate Distributions, 
 *  Chapter 13
 */
/*
//--------------------------------------------------------------------//
class TOL_API BTruncatedNormalDist : public BProbDist

//--------------------------------------------------------------------//
{
private:
  BDat sigma_;
  BDat nu_;
  bool ok_;
public:
  // contructors and destructors:
  BTruncatedNormalDist(BDat A, BDat B, BDat nu=0, BDat sigma=1);
  // Implementation:
  BDat Average	 ();
  BDat Varianze	 ();
  BDat Desviation()	  { return(Sqrt(Varianze())); }
  BDat Nu	       ()	  { return(nu_); }
  BDat Sigma	   ()	  { return(sigma_); }
  BDat Norm	     (BDat x) { return((x-nu_)/sigma_); }
  BDat InvNorm	 (BDat y) { return(y*sigma_+nu_); }
  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Dist	     (BDat x);
  BDat Dens	     (BDat x);
  BDat Inverse	 (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Random	   ();
};

*/

//--------------------------------------------------------------------//
class TOL_API BLogNormalDist : public BProbDist

/*! Logarithmic normal probability distribution definition & code
 *  for some methods. 
 *  Based on N. Johnson, S.Kotz, Continuous Univariate Distributions, 
 *  Chapter 14
 */
//--------------------------------------------------------------------//
{
private:
  BDat nu_;
  BDat sigma_;

public:
  // contructors and destructors:
  BLogNormalDist(BDat nu=0, BDat sigma=1)
  : nu_(nu), sigma_(sigma) { min_ = 0; }

  
  // Implementation:
  BDat Random	 ()	  { return(InvNorm(n01_.Random())); }
  BDat Average	 ()	  { return(Exp(nu_+ (sigma_^2)/2)); }
  BDat Varianze	 ()	  { return(Average()*(Exp(sigma_^2)-BDat(1) )); }
  BDat Desviation()	  { return(Sqrt(Varianze())); }
  BDat Nu	 ()	  { return(nu_); }
  BDat Sigma	 ()	  { return(sigma_); }
  BDat Norm	 (BDat x) { return((Log(x)-nu_)/sigma_);} 
  BDat InvNorm	 (BDat y) { return(Exp(y*sigma_+nu_)); }
  BDat Dist	 (BDat x);
  BDat Dens	 (BDat x);
  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	 (BDat prob,
		  BDat tolerance = BDat::Tolerance())
  {
    if(wrongParameter_ || prob.IsUnknown()) { return(BDat::Unknown()); }
    return(InvNorm(n01_.Inverse(prob,tolerance)));
  }
};


//--------------------------------------------------------------------
class TOL_API BLogisticDist : public BProbDist

/*! Logistic probability distribution definition.
 */
//--------------------------------------------------------------------
{
public:
  // contructors and destructors:
  BLogisticDist() {}

  BDat Average	 ()	  { return(0); }
  BDat Varianze	 ()	  { return(BDat::Pi()*BDat::Pi()/3); }
  BDat Desviation()	  { return(BDat::Pi()/sqrt(3.0)); }
  BDat Dist	 (BDat x) { return(1.0/(1.0+Exp(-x))); }
  BDat Dens	 (BDat x) { return(0.25/(CosH(x/2)^2)); }
  BDat GetDist	 (BDat x) { return(Dist(x)); }
  BDat GetDens	 (BDat x) { return(Dens(x)); }
  BDat CalcProb	 (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	 (BDat prob,
		  BDat tolerance = BDat::Tolerance())
  {
    return(Log(prob/(1.0-prob)));
  }
};


//--------------------------------------------------------------------
class TOL_API BGammaDist : public BProbDist

/*! Gamma probability distribution definition.
 */
//--------------------------------------------------------------------
{
protected:
  BDat a_;
  BDat sigma_;
  BDat nu_;
  BDat k_;


public:
  // ructors and destructors:
  BGammaDist() {}
  BGammaDist(BDat a, BDat nu=0.0, BDat sigma=1.0);

  static BDat Inc(BDat a, BDat z);
  static BDat InvInc(BDat a, BDat z);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	 (BDat prob,
		  BDat tolerance = BDat::Tolerance());
  BDat Average ();
  BDat Varianze();
};


//--------------------------------------------------------------------
class TOL_API BExpDist : public BGammaDist

/*! Exponential probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat sigma_;
  BDat nu_;

public:
  // contructors and destructors:
  BExpDist(BDat nu=0.0, BDat sigma=1.0);

};


//--------------------------------------------------------------------
class TOL_API BChiSquareDist : public BGammaDist

/*! ChiSquare probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt degree_;
  BDat g_;

public:
  // contructors and destructors:
  BChiSquareDist(BInt degree=1);
  BDat Random();
//BDat InverseAprox (BDat prob, BDat tolerance);
};


//--------------------------------------------------------------------
class TOL_API BBetaDist : public BProbDist

/*! Beta probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat p_;
  BDat q_;
  BDat k_;

public:
  // contructors and destructors:
  BBetaDist(BDat p=1, BDat q=1);

  static BDat Inc(BDat a, BDat b, BDat z);
  static BDat InvInc(BDat a, BDat b, BDat z);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average ();
  BDat Varianze();
};


//--------------------------------------------------------------------
class TOL_API BTStudentDist : public BProbDist

/*! t-Student probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt degree_;
  BDat k_;
  BDat d_;

public:
  // contructors and destructors:
  BTStudentDist(BInt degree=1);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average	() { return(0); }
  BDat Varianze () { return(BDat(degree_)/(degree_-2)); }
  BDat Random();
};


//--------------------------------------------------------------------
class TOL_API BNCTDist : public BProbDist

/*! Non central t-Student probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt degree_;
  BDat delta_;
  BDat v_;
  BDat nu_;
  BDat var_;
  BDat prob0_;
  BDat k_;
  BDat d_;

public:
  // contructors and destructors:
  BNCTDist(BInt degree=1, BDat delta=0);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat InverseAprox (BDat prob, BDat tolerance);
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average	() { return(nu_); }
  BDat Varianze () { return(var_); }
};


//--------------------------------------------------------------------
class TOL_API BFSnedecorDist : public BProbDist

/*! F-Snedecor probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt m_;
  BInt n_;
  BDat mn_;
  BDat g1_;
  BDat g2_;
  BDat g3_;

public:
  // contructors and destructors:
  BFSnedecorDist(BInt m, BInt n);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average ();
  BDat Varianze();
};


//--------------------------------------------------------------------
class TOL_API BNCFDist : public BProbDist

/*! Non central F-Snedecor probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt n_;
  BInt m_;
  BDat delta_;

public:
  // contructors and destructors:
  BNCFDist(BInt n_=1, BInt m_=1, BDat delta=0);

  // Implementation: prd.cpp
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average	();
  BDat Varianze ();
};


//--------------------------------------------------------------------
class TOL_API BZFisherDist : public BProbDist

/*! z-Fisher probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt m_;
  BInt n_;
  BFSnedecorDist F_;

public:
  // contructors and destructors:
  BZFisherDist(BInt m=1, BInt n=1);

  // Implementation: prd.cpp
  BDat Dist	    (BDat x);
  BDat Dens	    (BDat x);
  BDat GetDist	    (BDat x) { return(Dist(x)); }
  BDat GetDens	    (BDat x) { return(Dens(x)); }
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
  BDat Inverse	    (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average ();
  BDat Varianze();
};


//--------------------------------------------------------------------
class TOL_API BWeibullDist : public BProbDist

/*! Weibull probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat a_;
  BDat sigma_;
  BDat nu_;
  BDat k_;

  BDat Dist   (BDat x);
  BDat Dens   (BDat x);

public:
  // contructors and destructors:
  BWeibullDist(BDat a=1, BDat nu=0.0, BDat sigma=1.0);

  // Implementation: prd.cpp
  BDat InverseAprox (BDat prob, BDat tolerance = BDat::Tolerance());
  BDat Average ();
  BDat Varianze();
};


//--------------------------------------------------------------------
class TOL_API BLaplaceDist : public BProbDist

/*! Laplace probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat sigma_;
  BDat nu_;

public:
  // contructors and destructors:
  BLaplaceDist(BDat nu=0.0, BDat sigma=1.0);

  // Implementation: prd.cpp
  BDat InverseAprox (BDat prob, BDat tolerance);
  BDat Inverse(BDat prob);
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x); 
};


//--------------------------------------------------------------------
class TOL_API BCauchyDist : public BProbDist

/*! Cauchy probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat sigma_;
  BDat nu_;

public:
  // contructors and destructors:
  BCauchyDist(BDat nu=0.0, BDat sigma=1.0);

  // Implementation: prd.cpp
  BDat InverseAprox (BDat prob, BDat tolerance);
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
};


//--------------------------------------------------------------------
class TOL_API BParetoDist : public BProbDist

/*! Pareto probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat a_;
  BDat k_;

public:
  // contructors and destructors:
  BParetoDist(BDat a=1.0, BDat k=1.0);

  // Implementation: prd.cpp
  BDat InverseAprox (BDat prob, BDat tolerance);
  BDat Inverse(BDat x);
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
};



//--------------------------------------------------------------------
// discrete distributions
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class TOL_API BDiscreteDist : public BProbDist

/*! Discrete probability distribution definition.
 */
//--------------------------------------------------------------------
{
public :
  BDiscreteDist() : BProbDist()	 { }

  virtual BDat Summ (BInt from, BInt until);

  BDat CalcProb(BDat a, BDat b)
  {
    return(Summ((BInt)(a.Value()),(BInt)(b.Value())));
  }

  BDat Inverse (BDat prob, BDat tolerance = 0.01)
  {
    return(Round(BProbDist::Inverse(prob, 0.01)));
  }
};


//--------------------------------------------------------------------
class TOL_API BDiscreteUniformDist : public BDiscreteDist

/*! Discrete uniform probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt min_;
  BInt max_;
  BInt len_;

public:
  // contructors and destructors:
  BDiscreteUniformDist(BInt min, BInt max);

  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
  BDat Inverse	(BDat prob, BDat tolerance = BDat::Tolerance());
  BDat CalcProb	    (BDat a, BDat b) { return(Dist(b)-Dist(a)); }
};


//--------------------------------------------------------------------
class TOL_API BBinomialDist : public BDiscreteDist

/*! Binomial probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt N_;
  BDat p_;
  BDat q_;
  BDat logp_;
  BDat logq_;
  BDat logGammaN_;

public:
  // contructors and destructors:
  BBinomialDist(BInt N, BDat p);

    // Implementation: prd.cpp
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
  BDat Inverse  (BDat prob, BDat tolerance = 0.1);
};


//--------------------------------------------------------------------
class TOL_API BNegBinomialDist : public BDiscreteDist

/*! NegBinomial probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt N_;
  BDat p_;
  BDat q_;
  BDat P_;
  BDat Q_;
  BDat logp_;
  BDat nu_;
  BDat invQ_;

public:
  // contructors and destructors:
  BNegBinomialDist(BInt N, BDat P);

  // Implementation: prd.cpp
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
  BDat Inverse  (BDat prob, BDat tolerance = 0.1);
};


//--------------------------------------------------------------------
class TOL_API BGeometricDist : public BNegBinomialDist

/*! Geometric probability distribution definition.
 */
//--------------------------------------------------------------------
{
public:
  // contructors and destructors:
  BGeometricDist(BDat P);
};


//--------------------------------------------------------------------
class TOL_API BPoissonDist : public BDiscreteDist

/*! Poisson probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BDat fi_;
  BDat logFi_;

public:
  // contructors and destructors:
  BPoissonDist(BDat fi);

  // Implementation: prd.cpp
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
  BDat Inverse (BDat prob, BDat tolerance = 0.1);
};


//--------------------------------------------------------------------
class TOL_API BHypergeometricDist : public BDiscreteDist

/*! Hypergeometric probability distribution definition.
 */
//--------------------------------------------------------------------
{
private:
  BInt N_;
  BInt n_;
  BInt P_;
  BInt Q_;
  BDat k_;

public:
  // contructors and destructors:
  BHypergeometricDist(BInt N, BInt P, BInt n);

  // Implementation: prd.cpp
  BDat Average	();
  BDat Varianze ();
  BDat Dist	(BDat x);
  BDat Dens	(BDat x);
};

double slice_sampler_1D(const void *rng,
                        /* initial point */
                        double x0,
                        /* log of the probability density (plus constant) */
                        double (*g)(double,void*),
                        /* client data for g */
                        void *gdata=NULL,
                        /* gx0 = g(x0,gdata) */
                        double *gx0=NULL,
                        /* step's size for creating interval (default 1) */
                        double w=1.0,
                        /* Limit on steps (negative means infinite) */
                        int m=-1,
                        /* lower bound on support of the distribution */
                        double lower=std::numeric_limits<double>::min(),
                         /* upper bound on support of the distribution */   
                        double upper=std::numeric_limits<double>::max());

#endif	// TOL_BPRDIST_H
