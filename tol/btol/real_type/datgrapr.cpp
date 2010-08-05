/* datgrapr.cpp: Real Grammar's arithmetic functions
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

#include <tol/tol_bdatgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bprdist_internal.h>
#include <tol/tol_gslmat.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_randist.h>
#include "gsl_ext.h"

//--------------------------------------------------------------------
// forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraProbability() { return(BTRUE); }

BTraceInit("datgrapr.cpp");

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGetRandomSeed);
  DefIntOpr(1, BDatGetRandomSeed, "GetRandomSeed", 1, 1,
  "(Real unused)",
  I2("Returns the value of the lastest used seed for random numbers "
     "generator. The argument will not be used.",
     "Devuelve el valor de la última semilla utilizada por el generador "
     "de números aleatorios. El argumento de la función no se usa."),
  BOperClassify::Probability_);
  void BDatGetRandomSeed::CalcContens()
//--------------------------------------------------------------------
{
  unsigned long int oldSeed = BProbDist::GetRandomSeed();
  contens_ = oldSeed;
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutRandomSeed);
  DefIntOpr(1, BDatPutRandomSeed, "PutRandomSeed", 1, 1,
  "(Real seed)",
  I2("Initializes the random number generator with a seed. If this seed "
     "is 0 then a machine clock based seed is stablished."
     "The returned value is the lastest used seed",
     "Inicializa el generador de números aleatorios con una semilla."
     "Si la semilla es 0 entonces se establece una semilla en función"
     "del relog de la máquina. El valor devuelto es el de la última "
     "semilla utilizada."),
  BOperClassify::Probability_);
  void BDatPutRandomSeed::CalcContens()
//--------------------------------------------------------------------
{
  unsigned long int oldSeed = BProbDist::GetRandomSeed();
  contens_ = oldSeed;
  BProbDist::PutRandomSeed((unsigned long int)Real(Arg(1)));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandom);
  DefIntOpr(1, BDatRandom, "Rand", 2, 2,
  "(Real min, Real max)",
  I2("Returns an uniform distributed random variable U(min,max).",
     "Devuelve una variable aleatoria con distribución uniforme U(min,max)."),
  BOperClassify::Probability_);
  void BDatRandom::CalcContens()
//--------------------------------------------------------------------
{
  BUniformDist u(Dat(Arg(1)), Dat(Arg(2)));
  contens_ = (u.Random());
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGaussian);
  DefIntOpr(1, BDatGaussian, "Gaussian", 2, 2,
  I2("(Real average, Real deviation)",
     "(Real media, Real desviación)"),
  I2("Returns an normal distributed random variable "
     "N(average, stdandard Deviation).",
     "Devuelve una realización de una variable aleatoria con distribución "
     "normal N[media, desviación típica]."),
  BOperClassify::Probability_);
  void BDatGaussian::CalcContens()
//--------------------------------------------------------------------
{
  BNormalDist nor(Dat(Arg(1)), Dat(Arg(2)));
  contens_ = (nor.Random());
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRTN);
  DefIntOpr(1, BDatRTN, "DrawTruncatedNormal", 4, 4,
            "(Real nu, Real s, Real low, Real high)",
            I2("Returns a truncated normal distributed random variable "
               "N[nu, s] in the interval (low,high).\n"
               "Example:\n\n"
               "    Real x1 = DrawTruncatedNormal(0, 2, -0.5, 0);\n"
               "    Real x2 = DrawTruncatedNormal(0, 2, -0.5, +inf);\n"
               "    Real x3 = DrawTruncatedNormal(0, 2, -inf, 1);\n"
               "    Real x4 = DrawTruncatedNormal(0, 2, -inf, +inf);\n",
               "Devuelve una realización de una variable aleatoria con distribución "
               "normal truncada N[nu, s] en el intervalo (low,high).\n"
               "Ejemplo:\n\n"
               "    Real x1 = DrawTruncatedNormal(0, 2, -0.5, 0);\n"
               "    Real x2 = DrawTruncatedNormal(0, 2, -0.5, +inf);\n"
               "    Real x3 = DrawTruncatedNormal(0, 2, -inf, 1);\n"
               "    Real x4 = DrawTruncatedNormal(0, 2, -inf, +inf);\n"),
            BOperClassify::Probability_);
//--------------------------------------------------------------------
  void BDatRTN::CalcContens()
//--------------------------------------------------------------------
{
  double nu, s, low, high;
  BDat * d;

  d = &Dat(Arg(1));
  if (!d->IsKnown()) {
    Error(I2("Argument nu in DrawTruncatedNormal must be known",
             "El argumento nu en DrawTruncatedNormal debe ser conocido"));
    contens_.PutKnown(0);
    return;
  }
  nu = d->Value();
  d = &Dat(Arg(2));
  s = d->Value();
  if (!d->IsKnown() || s <= 0) {
    Error(I2("Argument s in DrawTruncatedNormal must be positive",
             "El argumento s en DrawTruncatedNormal debe ser positivo"));
    contens_.PutKnown(0);
    return;
  }
  d = &Dat(Arg(3));
  if (!d->IsKnown() ) {
    Error(I2("Argument low in DrawTruncatedNormal must be known",
             "El argumento low en DrawTruncatedNormal debe ser conocido"));
    contens_.PutKnown(0);
    return;
  }
  low = d->Value();
  d = &Dat(Arg(4));
  high = d->Value();
  if (!d->IsKnown() || low>high) {
    Error(I2("Argument high in DrawTruncatedNormal must be known and low < high",
             "El argumento high en DrawTruncatedNormal debe ser conocido y low < high"));
    contens_.PutKnown(0);
    return;
  }
  const int iter = 10;
  if (low == GSL_NEGINF)
    if (high == GSL_POSINF) {
      //Std("USANDO no acotada\n");
      contens_.PutValue(nu+gsl_ran_gaussian(getGslRng(),s));
    } else {
      //Std("USANDO acotada por arriba\n");      
      contens_.PutValue(gsl_rtanorm_combo(getGslRng(),
                                          nu, s, high, iter));
    } else
      if (high == GSL_POSINF) {
        //Std("USANDO acotada por abajo\n");
        contens_.PutValue(gsl_rtbnorm_combo(getGslRng(),
                                            nu, s, low, iter));
      } else {
        //Std("USANDO acotada por ambos extremos\n");
        contens_.PutValue(gsl_rtnorm_combo(getGslRng(),
                                           nu, s, low, high));
      }
  
  contens_.PutKnown(1);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatUniformDens);
  DefIntOpr(1, BDatUniformDens, "DensUniform", 1, 3,
  "(Real x [, Real min=0, Real max=1])",
  I2("Uniform(min,max) probability density",
     "Densidad de probabilidad uniforme U(min,max) donde "
     "los parámetros min y max definen el intervalo de la distribución.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatUniformDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x_ = Dat(Arg(1));
  BDat min_ = 0;
  BDat max_ = 1;
  if (NumArgs()>1) { min_ = Dat(Arg(2)); } 
  if (NumArgs()>2) { max_ = Dat(Arg(3)); } 
  if(x_.IsUnknown()||min_.IsUnknown()||max_.IsUnknown()) 
  { 
    contens_ = BDat::Unknown(); 
  }
  else
  {
    contens_.PutValue(gsl_ran_flat_pdf(x_.Value(), min_.Value(), max_.Value()));
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatUniformDist);
  DefIntOpr(1, BDatUniformDist, "DistUniform", 1, 3,
  "(Real x [, Real min=0, Real max=1])",
  I2("Uniform(min,max) probability distribution.",
     "Distribución de probabilidad uniforme U(min,max) donde "
     "los parámetros min y max definen el intervalo de la distribución.\n"
     "Devuelve Prob(U(min,max)<=x)\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatUniformDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat min    = 0; if(Arg(2)) { min    = Dat(Arg(2)); }
  BDat max = 1; if(Arg(3)) { max = Dat(Arg(3)); }
  BUniformDist uni(min, max);
  contens_ = uni.GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatInvUniformDist);
  DefIntOpr(1, BDatInvUniformDist, "DistUniformInv", 1, 3,
  "(Real y [, Real min=0, Real max=1])",
  I2("Inverse of Uniform(min,max) probability distribution ."
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(U(min,max)<=x)=y.",
     "Inversa de la distribución de probabilidad uniforme U(min,max) donde "
     "los parámetros min y max definen el intervalo de la distribución.\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(U(min,max)<=x)=y.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatInvUniformDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat min    = 0; if(Arg(2)) { min    = Dat(Arg(2)); }
  BDat max = 1; if(Arg(3)) { max = Dat(Arg(3)); }
  BUniformDist uni(min, max);
  contens_ = uni.Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNormalDens);
  DefIntOpr(1, BDatNormalDens, "DensNormal", 1, 3,
  "(Real x [, Real nu=0, Real sigma=1])",
  I2("Normal(nu,sigma) probability density",
     "Densidad de probabilidad normal N(nu,sigma) donde "
     "nu es la media de la distribución y sigma su desviación.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatNormalDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BNormalDist nor(nu, sigma);
  contens_ = nor.GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNormalDist);
  DefIntOpr(1, BDatNormalDist, "DistNormal", 1, 3,
  "(Real x [, Real nu=0, Real sigma=1])",
  I2("Normal(nu,sigma) probability distribution.",
     "Distribución de probabilidad normal N(nu,sigma) donde "
     "nu es la media de la distribución y sigma su desviación.\n"
     "Devuelve Prob(N(nu,sigma)<=x)\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatNormalDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BNormalDist nor(nu, sigma);
  contens_ = nor.GetDist(x);
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatInvNormalDist);
  DefIntOpr(1, BDatInvNormalDist, "DistNormalInv", 1, 3,
  "(Real y [, Real nu=0, Real sigma=1])",
  I2("Inverse of Normal(nu,sigma) probability distribution ."
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución de probabilidad normal N(nu,sigma) donde "
     "nu es la media de la distribución y sigma su desviación.\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(N(nu,sigma)<=x)=y.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatInvNormalDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BNormalDist nor(nu, sigma);
  contens_ = nor.Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTruncatedNormalDens);
  DefIntOpr(1, BDatTruncatedNormalDens, "DensTruncatedNormal", 3, 5,
  "(Real x, Real A, Real B [, Real nu=0, Real sigma=1])",
  I2("TruncatedNormal(A,B,nu,sigma) probability density",
     "Densidad de probabilidad normal truncada TN(A,B,nu,sigma) donde "
     "A y B son los límites de truncamiento, nu es la media de la "
     "distribución normal y sigma su desviación.\n"
     "Si A es -Inf o B es +Inf la distribución es semitruncada." 
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTruncatedNormalDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat A     = Dat(Arg(2));
  BDat B     = Dat(Arg(3));
  BDat nu    = 0; if(Arg(4)) { nu    = Dat(Arg(4)); }
  BDat sigma = 1; if(Arg(5)) { sigma = Dat(Arg(5)); }
  BTruncatedNormalDist aux(A, B, nu, sigma);
  contens_ = aux.GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTruncatedNormalDist);
  DefIntOpr(1, BDatTruncatedNormalDist, "DistTruncatedNormal", 3, 5,
  "(Real x, Real A, Real B [, Real nu=0, Real sigma=1])",
  I2("TruncatedNormal(A,B,nu,sigma) probability distribution",
     "Distribución de probabilidad normal truncada TN(A,B,nu,sigma) donde "
     "A y B son los límites de truncamiento, nu es la media de la "
     "distribución normal y sigma su desviación.\n"
     "Si A es -Inf o B es +Inf la distribución es semitruncada." 
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTruncatedNormalDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat A     = Dat(Arg(2));
  BDat B     = Dat(Arg(3));
  BDat nu    = 0; if(Arg(4)) { nu    = Dat(Arg(4)); }
  BDat sigma = 1; if(Arg(5)) { sigma = Dat(Arg(5)); }
  BTruncatedNormalDist aux(A, B, nu, sigma);
  contens_ = aux.GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTruncatedNormalInvDist);
  DefIntOpr(1, BDatTruncatedNormalInvDist, "DistTruncatedNormalInv", 3, 5,
  "(Real x, Real A, Real B [, Real nu=0, Real sigma=1])",
  I2("Inverse of TruncatedNormal(A,B,nu,sigma) probability distribution",
     "Inversa de la Distribución de probabilidad normal truncada "
     "TN(A,B,nu,sigma) donde "
     "A y B son los límites de truncamiento, nu es la media de la "
     "distribución normal y sigma su desviación.\n"
     "Si A es -Inf o B es +Inf la distribución es semitruncada." 
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTruncatedNormalInvDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat A     = Dat(Arg(2));
  BDat B     = Dat(Arg(3));
  BDat nu    = 0; if(Arg(4)) { nu    = Dat(Arg(4)); }
  BDat sigma = 1; if(Arg(5)) { sigma = Dat(Arg(5)); }
  BTruncatedNormalDist aux(A, B, nu, sigma);
  contens_ = aux.Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLogNormalDens);
  DefIntOpr(1, BDatLogNormalDens, "DensLogNormal", 1, 3,
  "(Real x [, Real nu=0, Real sigma=1])",
  I2("LogNormal(nu,sigma) probability density",
     "Densidad de probabilidad logarítmico normal LN(nu,sigma) donde "
     "nu es la media de la distribución transformada y sigma su desviación.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatLogNormalDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BLogNormalDist nor(nu, sigma);
  contens_ = nor.GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLogNormalDist);
  DefIntOpr(1, BDatLogNormalDist, "DistLogNormal", 1, 3,
  "(Real x [, Real nu=0, Real sigma=1])",
  I2("LogNormal(nu,sigma) probability distribution.",
     "Distribución de probabilidad logarítmico normal LN(nu,sigma) donde "
     "nu es la media de la distribución transformada y sigma su desviación.\n"
     "Devuelve Prob(LN(nu,sigma)<=x)\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatLogNormalDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BLogNormalDist nor(nu, sigma);
  contens_ = nor.GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatInvLogNormalDist);
  DefIntOpr(1, BDatInvLogNormalDist, "DistLogNormalInv", 1, 3,
  "(Real y [, Real nu=0, Real sigma=1])",
  I2("Inverse of LogNormal(nu,sigma) probability distribution ."
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(LN(nu,sigma)<=x)=y.",
     "Inversa de la distribución de probabilidad logarítmico normal "
     "LN(nu,sigma) donde nu es la media de la distribución transformada "
     "y sigma su desviación.\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(LN(nu,sigma)<=x)=y.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatInvLogNormalDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat(Arg(1));
  BDat nu    = 0; if(Arg(2)) { nu    = Dat(Arg(2)); }
  BDat sigma = 1; if(Arg(3)) { sigma = Dat(Arg(3)); }
  BLogNormalDist nor(nu, sigma);
  contens_ = nor.Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandLogNormal);
  DefIntOpr(1, BDatRandLogNormal, "RandLogNormal", 2, 2,
  "(Real l, Real s)",
  I2("Returns a random variate from the lognormal "
     "distribution with  location l and shape s",
     "Retorna una variable aleatoria de la distribución "
     "de probabilidad LogNormal con parámetros de localización y forma l y s respectivamente. "),
  BOperClassify::Probability_);
  void BDatRandLogNormal::CalcContens()
//--------------------------------------------------------------------
{
    BDat * l = &Dat(Arg(1));
	BDat * s = &Dat(Arg(2));
    if (!l->IsKnown() || !s->IsKnown()) {
  	  Error(I2("wrong parameters in RandLogNormal",
	  	   "parámetros incorrectos en RandLogNormal"));
	  contens_.PutKnown(0);
	  return;
    }
      
    contens_.PutValue(gsl_ran_lognormal(getGslRng(),l->Value(),s->Value()));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandChisq);
  DefIntOpr(1, BDatRandChisq, "RandChisq", 1, 1,
  "(Real n)",
  I2("Returns a random variate from the chi-squared "
     "distribution with  probability distribution with n degrees of freedom.",
     "Retorna una variable aleatoria de la distribución "
     "de probabilidad chi-cuadrado con n grados de libertad "),
  BOperClassify::Probability_);
  void BDatRandChisq::CalcContens()
//--------------------------------------------------------------------
{
    BDat * n = &Dat(Arg(1));

    if (!n->IsKnown() || n->Value() < 1.0) {
	Error(I2("parameter n < 1 in RandChisq",
		 "parámetro n < 1 en RandChisq"));
	contens_.PutKnown(0);
	return;
    }
    contens_.PutValue(gsl_ran_chisq(getGslRng(),n->Value()));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatChiDens);
  DefIntOpr(1, BDatChiDens, "DensChi", 1, 2,
  "(Real x [, Real n=1])",
  I2("Chi-square density distribution with n degrees of freedom.",
     "Densidad de probabilidad chi-cuadrado con n grados de libertad "
     "Chi(n).\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatChiDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BChiSquareDist(deg).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatChiDist);
  DefIntOpr(1, BDatChiDist, "DistChi", 1, 2,
  "(Real x [, Real n=1])",
  I2("Chi-square probability distribution with n degrees of freedom.",
     "Distribución de probabilidad chi-cuadrado con n grados de libertad "
     "Chi(n).\n"
     "Devuelve Prob(Chi(n)<=x)\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatChiDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BChiSquareDist(deg).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatChiInv);
  DefIntOpr(1, BDatChiInv, "DistChiInv", 1, 2,
  "(Real y [, Real n=1])",
  I2("Inverse of Chi-square distribution with n degrees of freedom. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución  chi-cuadrado con n grados de libertad "
     "Chi(n).\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(Chi<=x)=y.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatChiInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BChiSquareDist(deg).Inverse(x);
}

/*
 *  richisq = rigamma(nu/2,0.5) = (n*s)/chisq(n)
 *
 *  rigamma = rgamma(alpha, beta)^-1
 */

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandIChisq);
  DefIntOpr(1, BDatRandIChisq, "RandIChisq", 1, 2,
  "(Real n [, Real s2=1])",
  I2("Returns a random variate from the inverse chi-squared "
     "distribution with  probability distribution with n degrees of freedom and s2.",
     "Retorna una variable aleatoria de la distribución. See "
     "de probabilidad chi-cuadrado inversa con n grados de libertad y s2. Ver ")+
     "Bayesian Data Analysis, Ap. A",
  BOperClassify::Probability_);
  void BDatRandIChisq::CalcContens()
//--------------------------------------------------------------------
{
  BDat * d = &Dat(Arg(1));
    
  int n = int(d->Value());
  if (!d->IsKnown() || n < 1.0) 
  {	
    Error(I2("parameter n < 1 in RandIChisq",
		         "parámetro n < 1 en RandIChisq"));
	  contens_.PutKnown(0);
	  return;
  }
  double s2 = 1.0;
  if (Arg(2)) 
  {
    d = &Dat(Arg(2));
    if (!d->IsKnown()) 
    {
      Error(I2("scale parameter in RandIChisq is unknown",
               "scale es desconocido en RandIChisq"));
 	    contens_.PutKnown(0);
	    return;       
    }
    s2 = d->Value();
  }
    
//     contens_.PutValue(1.0/gsl_ran_gamma(getGslRng(),n->Value()/2,0.5));
    contens_.PutValue((n*s2)/gsl_ran_chisq(getGslRng(), n));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTStudentDens);
  DefIntOpr(1, BDatTStudentDens, "DensT", 1, 2,
  "(Real x [, Real n=1])",
  I2("t-Student probability density with n degrees of freedom.",
     "Densidad de probabilidad t-Student con n grados de libertad T(n).\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTStudentDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BTStudentDist(deg).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTStudentDist);
  DefIntOpr(1, BDatTStudentDist, "DistT", 1, 2,
  "(Real x [, Real n=1])",
  I2("t-Student with n degrees of freedom probability distribution of "
     "a real number.",
     "Distribución de probabilidad t-Student con n grados de libertad T(n).\n"
     "Devuelve Prob(T(n)<=x)\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTStudentDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BTStudentDist(deg).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTStudentInv);
  DefIntOpr(1, BDatTStudentInv, "DistTInv", 1, 2,
  "(Real y [, Real n=1])",
  I2("Inverse of t-Student distribution with n degrees of freedom. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución t-Student con n grados de libertad T(n). "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(T(n)<=x)=y.\n"
     "Los parámetros entre corchetes pueden omitirse, en cuyo caso se toman "
     "los valores por defecto."),
  BOperClassify::Probability_);
  void BDatTStudentInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt deg = 1; if(Arg(2)) { deg = (BInt)Real(Arg(2)); }
  contens_ = BTStudentDist(deg).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCTDens);
  DefIntOpr(1, BDatNCTDens, "DensNCT", 3, 3,
  "(Real y, Real n, Real d)",
  I2("Non central t-Student probability density with n degrees of freedom "
     "and non centrality parameter d.",
     "Densidad de probabilidad t-Student no central con n grados de "
     "libertad y parámetro de no centralización d  T'(n,d).\n"),
  BOperClassify::Probability_);
  void BDatNCTDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt deg   = (BInt)Real(Arg(2));
  BDat delta = Dat (Arg(3));
  contens_ = BNCTDist(deg,delta).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCTDist);
  DefIntOpr(1, BDatNCTDist, "DistNCT", 3, 3,
  "(Real y, Real n, Real d)",
  I2("Non central t-Student with n degrees of freedom "
     "and non centrality parameter d probability distribution of "
     "a real number."
     "Returns Prob(T'(n,d)<=x)\n",
     "Distribución de probabilidad t-Student no central con n grados de "
     "libertad y parámetro de no centralización d  T'(n,d).\n"
     "Devuelve Prob(T'(n,d)<=x)\n"),
  BOperClassify::Probability_);
  void BDatNCTDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt deg   = (BInt)Real(Arg(2));
  BDat delta = Dat (Arg(3));
  contens_ = BNCTDist(deg,delta).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCTInv);
  DefIntOpr(1, BDatNCTInv, "DistNCTInv", 3, 3,
  "(Real y, Real n, Real d)",
  I2("Inverse of non central t-Student distribution with n degrees of "
     "freedom and non centrality parameter d T'(n,d)."
     "For each y between 0 and 1, it returns the real number x such that \n"
     "Prob(T'(n,d)<=x)=y.",
     "Inversa de la distribución t-Student no central con n grados de "
     "libertad y parámetro de no centralización d  T'(n,d). "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que \n"
     "Prob(T'(n,d)<=x)=y.\n"),
  BOperClassify::Probability_);
  void BDatNCTInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt deg   = (BInt)Real(Arg(2));
  BDat delta = Dat (Arg(3));
  contens_ = BNCTDist(deg,delta).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFSnedecorDens);
  DefIntOpr(1, BDatFSnedecorDens, "DensF", 3, 3,
  "(Real x, Real v1, Real v2)",
  I2("F-Snedecor probability density with n and m degrees of freedom F(n,m).",
     "Densidad de probabilidad F-Snedecor con n y m grados de libertad "
     "F(n,m).\n"
     "Devuelve Prob(T(n)<=x)\n"
     ),
  BOperClassify::Probability_);
  void BDatFSnedecorDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BFSnedecorDist(m,n).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFSnedecorDist);
  DefIntOpr(1, BDatFSnedecorDist, "DistF", 3, 3,
  "(Real x, Real v1, Real v2)",
  I2("F-Snedecor probability distribution with v1 and v2 degrees of freedom.",
     "Distribución de probabilidad F-Snedecor con v1 y v2 grados de libertad"
     "F(v1,v2).\n"
     "Devuelve Prob(F(v1,v2)<=x)\n"),
  BOperClassify::Probability_);
  void BDatFSnedecorDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BFSnedecorDist(m,n).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFSnedecorInv);
  DefIntOpr(1, BDatFSnedecorInv, "DistFInv", 3, 3,
  "(Real y, Real v1, Real v2)",
  I2("Inverse of F-Snedecor distribution with v1 and v2 degrees of freedom. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución F-Snedecor con v1 y v2 grados de libertad"
     "F(v1,v2)/n. "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(F(v1,v2)<=x)=y."),
  BOperClassify::Probability_);
  void BDatFSnedecorInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BFSnedecorDist(m,n).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCFDens);
  DefIntOpr(1, BDatNCFDens, "DensNCF", 4, 4,
  "(Real y, Real n, Real m, Real d)",
  I2("Non central F-Snedecor probability density with n and m degrees of "
     "freedom and non centrality parameter d.",
     "Densidad de probabilidad F-Snedecor no central con n y m grados de "
     "libertad y parámetro de no centralización d  F'(n,d).\n"),
  BOperClassify::Probability_);
  void BDatNCFDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt n     = (BInt)Real(Arg(2));
  BInt m     = (BInt)Real(Arg(3));
  BDat delta = Dat (Arg(4));
  contens_ = BNCFDist(n,m,delta).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCFDist);
  DefIntOpr(1, BDatNCFDist, "DistNCF", 4, 4,
  "(Real y, Real n, Real m, Real d)",
  I2("Non central F-Snedecor with n and m degrees of freedom "
     "and non centrality parameter d probability distribution of "
     "a real number."
     "Returns Prob(F'(n,d)<=x)\n",
     "Distribución de probabilidad F-Snedecor no central con n y m grados de "
     "libertad y parámetro de no centralización d  F'(n,d).\n"
     "Devuelve Prob(F'(n,d)<=x)\n"),
  BOperClassify::Probability_);
  void BDatNCFDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt n     = (BInt)Real(Arg(2));
  BInt m     = (BInt)Real(Arg(3));
  BDat delta = Dat (Arg(4));
  contens_ = BNCFDist(n,m,delta).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNCFInv);
  DefIntOpr(1, BDatNCFInv, "DistNCFInv", 4, 4,
  "(Real y, Real n, Real m, Real d)",
  I2("Inverse of non central F-Snedecor distribution with n and m degrees of "
     "freedom and non centrality parameter d F'(n,d)."
     "For each y between 0 and 1, it returns the real number x such that \n"
     "Prob(F'(n,d)<=x)=y.",
     "Inversa de la distribución F-Snedecor no central con n y m grados de "
     "libertad y parámetro de no centralización d  F'(n,d). "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que \n"
     "Prob(F'(n,d)<=x)=y.\n"),
  BOperClassify::Probability_);
  void BDatNCFInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x     = Dat (Arg(1));
  BInt n     = (BInt)Real(Arg(2));
  BInt m     = (BInt)Real(Arg(3));
  BDat delta = Dat (Arg(4));
  contens_ = BNCFDist(n,m,delta).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatZFisherDens);
  DefIntOpr(1, BDatZFisherDens, "DensZ", 3, 3,
  "(Real x, Real v1, Real v2)",
  I2("z-Fisher probability density with n and m degrees of freedom.",
     "Densidad de probabilidad z-Fisher con n y m grados de libertad "
     "Z(v1,v2)."),
  BOperClassify::Probability_);
  void BDatZFisherDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BZFisherDist(m,n).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatZFisherDist);
  DefIntOpr(1, BDatZFisherDist, "DistZ", 3, 3,
  "(Real x, Real v1, Real v2)",
  I2("z-Fisher probability distribution with n and m degrees of freedom.",
     "Distribución de probabilidad z-Fisher con n y m grados de libertad."
     "Z(v1,v2).\n"
     "Devuelve Prob(Z(v1,v2)<=x)\n"),
  BOperClassify::Probability_);
  void BDatZFisherDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BZFisherDist(m,n).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatZFisherInv);
  DefIntOpr(1, BDatZFisherInv, "DistZInv", 3, 3,
  "(Real y, Real v1, Real v2)",
  I2("Inverse of z-Fisher distribution with n and m degrees of freedom."
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución z-Fisher con v1 y v2 grados de libertad "
     "Z(v1,v2)\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(Z(v1,v2)<=x)=y."),
  BOperClassify::Probability_);
  void BDatZFisherInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BInt	m = (BInt)Real(Arg(2));
  BInt	n = (BInt)Real(Arg(3));
  contens_ = BZFisherDist(m,n).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBetaDens);
  DefIntOpr(1, BDatBetaDens, "DensBeta", 3, 3,
  "(Real x, Real p, Real q)",
  I2("Beta(p,q) probability density.",
     "Densidad de probabilidad beta con paramétros p y q B(p,q)."),
  BOperClassify::Probability_);
  void BDatBetaDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat p = Dat(Arg(2));
  BDat q = Dat(Arg(3));
  contens_ = BBetaDist(p,q).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBetaDist);
  DefIntOpr(1, BDatBetaDist, "DistBeta", 3, 3,
  "(Real x, Real p, Real q)",
  I2("Beta(p,q) probability distribution.",
     "Distribución de probabilidad beta con paramétros p y q B(p,q)\n."
     "Devuelve Prob(B(p,q)<=x)\n"),
  BOperClassify::Probability_);
  void BDatBetaDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat p = Dat(Arg(2));
  BDat q = Dat(Arg(3));
  contens_ = BBetaDist(p,q).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBetaInv);
  DefIntOpr(1, BDatBetaInv, "DistBetaInv", 3, 3,
  "(Real y, Real p, Real q)",
  I2("Inverse of Beta(p,q) distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución  beta con paramétros p y q B(p,q).\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(B(p,q)<=x)=y."),
  BOperClassify::Probability_);
  void BDatBetaInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat p = Dat(Arg(2));
  BDat q = Dat(Arg(3));
  contens_ = BBetaDist(p,q).Inverse(x);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandGamma);
  DefIntOpr(1, BDatRandGamma, "RandGamma", 1, 3,
	    "(Real a [, Real b=1.0, Real is_scale = 1])",
	    I2("Returns a random variate from the Gamma "
	       "distribution with parameters a (shape) and b. Depending on the value of is_scale b is taken as scale parameter (is_scale!=0) or as rate parameter (is_scale==0). "
               "Parameters a and b must be both positive",
	       "Retorna una variable  aleatoria a partir "
	       "de una distribución Gamma con parámetros a (shape) y b. Dependiendo del valor de is_scale el parámetro b se considera como 'scale' (is_scale!=0) o como 'rate' (is_scale==0). "
               "Los parámetros a y b deben ser ambos positivos"),
	    BOperClassify::Probability_);
void BDatRandGamma::CalcContens()
//--------------------------------------------------------------------
{
  BDat * ptr_dat;
  double shape;
  double b = 1.0;
  int is_scale = 1;
  
  /* shape parameter */
  ptr_dat  = &Dat(Arg(1));
  shape = ptr_dat->Value();
  if (!ptr_dat->IsKnown() || shape <= 0.0) {
    Error(I2("shape parameter a <= 0 in RandGamma",
             "parámetro 'shape' a <= en RandGamma"));
    contens_.PutKnown(0);
    return;
  }
  /* end shape parameter */
  
  if (NumArgs()>1) 
  {
    /* b parameter */
    ptr_dat = &Dat(Arg(2));
    b = ptr_dat->Value();
    if (!ptr_dat->IsKnown() || b <= 0.0) {
      Error(I2("parameter b <= 0 in RandGamma",
               "parámetro b <= en RandGamma"));
      contens_.PutKnown(0);
      return;
    }
    /* end b parameter */
    
    /* is_scale parameter */
    if (NumArgs()>2) 
    {
      ptr_dat = &Dat(Arg(3));
      is_scale = int(ptr_dat->Value());
      if (!ptr_dat->IsKnown() || (is_scale!=0 && is_scale!=1)) {
        Error(I2("parameter is_scale must be 0 or 1 in RandGamma",
                 "parámetro is_scale debe ser 0 ó 1 en RandGamma"));
        contens_.PutKnown(0);
        return;
      }
    }
    /* end is_scale parameter */
  }
  /* remember that gsl_ran_gamma is expecting for rate parameter
   * rate = 1/scale */
  contens_.PutValue(gsl_ran_gamma(getGslRng(),
                                  shape, is_scale?1/b:b));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGammaDens);
  DefIntOpr(1, BDatGammaDens, "DensGamma", 2, 3,
  "(Real x, Real a [, Real s=1.0])",
  I2("Gamma(r,a) probability density. The parameter 'a' is known as shape, 's' is known as scale.\n\n"
     "f(x;a,s) = 1/(s^a Gamma(a)) x^(a-1) e^-(x/s)\n\n"
     "0 <= x <= +inf, a > 0, s > 0",
     "Densidad de probabilidad gamma G(a,s). El parámetro 'a' es conocido como 'shape', 's' is conocido como 'scale'.\n\n"
     "f(x;a,s) = 1/(s^a Gamma(a)) x^(a-1) e^-(x/s)\n\n"
     "0 <= x <= +inf, a > 0, s > 0"),
  BOperClassify::Probability_);
  void BDatGammaDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x, a, s;

  x = Dat(Arg(1));
  if (x >= 0) {
    a = Dat(Arg(2));
    if (a > 0) {
      if (NumArgs() == 3) {
	s = Dat(Arg(3));
	if (s <= 0) return;
      } else {
	s = 1.0;
      }
    }
    contens_.PutValue(gsl_ran_gamma_pdf(x.Value(), a.Value(), s.Value()));
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGammaDist);
  DefIntOpr(1, BDatGammaDist, "DistGamma", 2, 2,
  "(Real x, Real p)",
  I2("Gamma(p) probability distribution.",
     "Distribución de probabilidad gamma com parámetro p G(p)."
     "Devuelve Prob(G(q)<=x)\n"),
  BOperClassify::Probability_);
  void BDatGammaDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat p = Dat(Arg(2));
  contens_ = BGammaDist(p).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGammaInv);
  DefIntOpr(1, BDatGammaInv, "DistGammaInv", 2, 2,
  "(Real y, Real p)",
  I2("Inverse of Gamma distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución  gamma com parámetro p G(p).\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(G(p)<=x)=y."),
  BOperClassify::Probability_);
  void BDatGammaInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat p = Dat(Arg(2));
  contens_ = BGammaDist(p).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatExpDens);
  DefIntOpr(1, BDatExpDens, "DensExp", 1, 2,
  "(Real x [, Real mu = 1.0])",
  I2("Exponential probability density with scale paremeter 'mu'.\n\n"
     "f(x;r) = 1/mu * e^(- x/mu), x >= 0",
     "Densidad de probabilidad exponencial X con parámetro mu.\n\n"
     "f(x;r) = 1/mu * e^(- x/mu), x >= 0"),
  BOperClassify::Probability_);
  void BDatExpDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x, mu;

  x = Dat(Arg(1));
  if (x >= 0) {
    if (NumArgs()==2) {
      mu = Dat(Arg(2));
    } else {
      mu = 1.0;
    }
    contens_.PutValue(gsl_ran_exponential_pdf(x.Value(), mu.Value()));
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatExpDist);
  DefIntOpr(1, BDatExpDist, "DistExp", 1, 1,
  "(Real x)",
  I2("Exponential probability distribution.",
     "Distribución de probabilidad exponencial X.\n"
     "Devuelve Prob(X<=x)\n"),
  BOperClassify::Probability_);
  void BDatExpDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BExpDist().GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatExpInv);
  DefIntOpr(1, BDatExpInv, "DistExpInv", 1, 1,
  "(Real y)",
  I2("Inverse of Exponential distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución exponencial X.\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(X<=x)=y."),
  BOperClassify::Probability_);
  void BDatExpInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BExpDist().Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandExp);
  DefIntOpr(1, BDatRandExp, "RandExp", 1, 1,
  "(Real mu)",
  I2("Returns a random variate from the exponential "
     "distribution with  mean = mu.",
     "Retorna una variable aleatoria de la distribución "
     "de probabilidad exponencial con media mu "),
  BOperClassify::Probability_);
  void BDatRandExp::CalcContens()
//--------------------------------------------------------------------
{
    BDat * mu = &Dat(Arg(1));

    if (!mu->IsKnown() || mu->Value() <= 0.0) {
	Error(I2("parameter mu <= 0 in RandExp",
		 "parámetro mu <= 0 en RandExp"));
	contens_.PutKnown(0);
	return;
    }
    contens_.PutValue(gsl_ran_exponential(getGslRng(),mu->Value()));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWeibullDens);
  DefIntOpr(1, BDatWeibullDens, "DensWeibull", 2, 2,
  "(Real x, Real c)",
  I2("Weibull(c) probability density",
     "Densidad de probabilidad de Weibull con paraámetro c W(c)."),
  BOperClassify::Probability_);
  void BDatWeibullDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  contens_ = BWeibullDist(a).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWeibullDist);
  DefIntOpr(1, BDatWeibullDist, "DistWeibull", 2, 2,
  "(Real x, Real c)",
  I2("Weibull(c) probability distribution.",
     "Distribución de probabilidad de Weibull con paraámetro c W(c)."
     "Devuelve Prob(W(c)<=x)\n"),
  BOperClassify::Probability_);
  void BDatWeibullDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  contens_ = BWeibullDist(a).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWeibullInv);
  DefIntOpr(1, BDatWeibullInv, "DistWeibullInv", 2, 2,
  "(Real y, Real c)",
  I2("Inverse of Weibull(c) distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x)=y.",
     "Inversa de la distribución de Weibull con paraámetro c W(c). "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(W(c)<=x)=y."),
  BOperClassify::Probability_);
  void BDatWeibullInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  contens_ = BWeibullDist(a).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLaplaceDens);
  DefIntOpr(1, BDatLaplaceDens, "DensLaplace", 1, 1,
  "(Real x)",
  I2("Laplace probability density.",
     "Densidad de probabilidad de Laplace L."),
  BOperClassify::Probability_);
  void BDatLaplaceDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BLaplaceDist().GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLaplaceDist);
  DefIntOpr(1, BDatLaplaceDist, "DistLaplace", 1, 1,
  "(Real x)",
  I2("Laplace probability distribution.",
     "Distribución de probabilidad de Laplace L.\n"
     "Devuelve Prob(L<=x)\n"),
  BOperClassify::Probability_);
  void BDatLaplaceDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BLaplaceDist().GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLaplaceInv);
  DefIntOpr(1, BDatLaplaceInv, "DistLaplaceInv", 1, 1,
  "(Real y)",
  I2("Inverse of Laplace distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(L<=x)=y.",
     "Inversa de la distribución de Laplace L."
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(L<=x)=y."),
  BOperClassify::Probability_);
  void BDatLaplaceInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BLaplaceDist().Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCauchyDens);
  DefIntOpr(1, BDatCauchyDens, "DensCauchy", 1, 1,
  "(Real x)",
  I2("Cauchy probability density.",
     "Densidad de probabilidad de Cauchy C."),
  BOperClassify::Probability_);
  void BDatCauchyDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BCauchyDist().GetDens(x);
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCauchyDist);
  DefIntOpr(1, BDatCauchyDist, "DistCauchy", 1, 1,
  "(Real x)",
  I2("Cauchy probability distribution.",
     "Distribución de probabilidad de Cauchy C.\n"
     "Devuelve Prob(C<=x)\n"),
  BOperClassify::Probability_);
  void BDatCauchyDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BCauchyDist().GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCauchyInv);
  DefIntOpr(1, BDatCauchyInv, "DistCauchyInv", 1, 1,
  "(Real y)",
  I2("Inverse of Cauchy distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(C<=x)=y.",
     "Inversa de la distribución de Cauchy C.\n"
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(C<=x)=y."),
  BOperClassify::Probability_);
  void BDatCauchyInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  contens_ = BCauchyDist().Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRandCauchy);
  DefIntOpr(1, BDatRandCauchy, "RandCauchy", 1, 1,
  "(Real a)",
  I2("Returns a random variate from the Cauchy "
     "distribution with  probability distribution with scale a.",
     "Retorna una variable aleatoria de la distribución "
     "de probabilidad de Cauchy con escala a "),
  BOperClassify::Probability_);
  void BDatRandCauchy::CalcContens()
//--------------------------------------------------------------------
{
    BDat * a = &Dat(Arg(1));

    if (!a->IsKnown() || a->Value() < 0.0) {
	Error(I2("parameter a < 0 in RandCauchy",
		 "parámetro a < 0 en RandCauchy"));
	contens_.PutKnown(0);
	return;
    }
    contens_.PutValue(gsl_ran_cauchy(getGslRng(),a->Value()));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatParetoDens);
  DefIntOpr(1, BDatParetoDens, "DensPareto", 3, 3,
  "(Real x , Real a, Real k)",
  I2("Pareto(a,k) probability density p(x) = (a*k^a)/(x^(a-1)).",
     "Densidad de probabilidad de Pareto con parámetros a y k P(a,k)\n."),
  BOperClassify::Probability_);
  void BDatParetoDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  BDat k = Dat(Arg(3));
  contens_ = BParetoDist(a,k).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatParetoDist);
  DefIntOpr(1, BDatParetoDist, "DistPareto", 3, 3,
  "(Real x , Real a, Real k)",
  I2("Pareto probability distribution. Prob(X<=x)=1-(k/x)^a.",
     "Distribución de probabilidad de Pareto con parámetros a y k P(a,k)\n."
     "Devuelve Prob(P(a,k)<=x)."),
  BOperClassify::Probability_);
  void BDatParetoDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  BDat k = Dat(Arg(3));
  contens_ = BParetoDist(a,k).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatParetoInv);
  DefIntOpr(1, BDatParetoInv, "DistParetoInv", 3, 3,
  "(Real y , Real a, Real k)",
  I2("Inverse of Pareto distribution. "
     "For each y between 0 and 1, it returns the real number x such that "
     "Prob(X<=x) = 1-(k/x)^a = y.",
     "Inversa de la distribución de Pareto con parámetros a y k P(a,k)\n. "
     "Para cada valor y entre 0 y 1, devuelve el número real x tal que "
     "Prob(P(a,k)<=x) = y."),
  BOperClassify::Probability_);
  void BDatParetoInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat(Arg(1));
  BDat a = Dat(Arg(2));
  BDat k = Dat(Arg(3));
  contens_ = BParetoDist(a,k).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDiscreteUniformDens);
  DefIntOpr(1, BDatDiscreteUniformDens, "ProbDiscreteUniform", 3, 3,
  "(Real k, Real min, Real max)",
  I2("Discrete Uniform(min,max) probability function.",
     "Función de probabilidad Discreta Uniforme entre min y max\n."
     "Devuelve Prob(DU(min,max)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatDiscreteUniformDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt min = (BInt)Real(Arg(2));
  BInt max = (BInt)Real(Arg(3));
  contens_ = BDiscreteUniformDist(min,max).Dens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDiscreteUniformDist);
  DefIntOpr(1, BDatDiscreteUniformDist, "DistDiscreteUniform", 3, 3,
  "(Real k, Real N, Real p)",
  I2("Discrete Uniform(min,max) probability distribution.",
     "Distribución de probabilidad Discreta Uniforme entre min y max\n."
     "Devuelve Prob(DU(min,max)<=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatDiscreteUniformDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt min = (BInt)Real(Arg(2));
  BInt max = (BInt)Real(Arg(3));
  contens_ = BDiscreteUniformDist(min,max).Dist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDiscreteUniformInv);
  DefIntOpr(1, BDatDiscreteUniformInv, "DistDiscreteUniformInv", 3, 3,
  "(Real y, Real N, Real p)",
  I2("Inverse of Discrete Uniform(min,max) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=n)=y.",
     "Inversa de la distribución Discreta Uniforme entre min y max\n."
     "Para cada valor y entre 0 y 1, devuelve el número entero k tal que "
     "Prob(DU(min,max)<=k)=y."),
  BOperClassify::Probability_);
  void BDatDiscreteUniformInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt min = (BInt)Real(Arg(2));
  BInt max = (BInt)Real(Arg(3));
  contens_ = BDiscreteUniformDist(min,max).Inverse(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBinomialDens);
  DefIntOpr(1, BDatBinomialDens, "ProbBinomial", 3, 3,
  "(Real k, Real N, Real p)",
  I2("Binomial(N,p) probability function.",
     "Función de probabilidad binomial de parámetros N y p B(N,p)\n."
     "Devuelve Prob(B(N,p)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatBinomialDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = BBinomialDist(N,p).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBinomialDist);
  DefIntOpr(1, BDatBinomialDist, "DistBinomial", 3, 3,
  "(Real k, Real N, Real p)",
  I2("Binomial(N,p) probability distribution.",
     "Distribución de probabilidad binomial de parámetros N y p B(N,p)\n."
     "Devuelve Prob(B(N,p)<=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatBinomialDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = BBinomialDist(N,p).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBinomialInv);
  DefIntOpr(1, BDatBinomialInv, "DistBinomialInv", 3, 3,
  "(Real y, Real N, Real p)",
  I2("Inverse of Binomial(N,p) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=n)=y.",
     "Inversa de la distribución binomial de parámetros N y p B(N,p)\n."
     "Para cada valor y entre 0 y 1, devuelve el número entero k tal que "
     "Prob(B(N,p)<=k)=y."),
  BOperClassify::Probability_);
  void BDatBinomialInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = Floor(BBinomialDist(N,p).Inverse(x));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNegBinomialDens);
  DefIntOpr(1, BDatNegBinomialDens, "ProbNegBinomial", 3, 3,
  "(Real k, Real N, Real p)",
  I2("Negative-Binomial (N,p) probability function.",
     "Función de probabilidad binomial negativa de parámetros N y p "
     "BN(N,p)\n."
     "Devuelve Prob(BN(N,p)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatNegBinomialDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = BNegBinomialDist(N,p).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNegBinomialDist);
  DefIntOpr(1, BDatNegBinomialDist, "DistNegBinomial", 3, 3,
  "(Real k, Real N, Real p)",
  I2("Negative-Binomial(N,p) probability distribution.",
     "Distribución de probabilidad binomial negativa de parámetros N y p "
     "BN(N,p)\n."
     "Devuelve Prob(BN(N,p)<=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatNegBinomialDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = BNegBinomialDist(N,p).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNegBinomialInv);
  DefIntOpr(1, BDatNegBinomialInv, "DistNegBinomialInv", 3, 3,
  "(Real y, Real N, Real p)",
  I2("Inverse of Negative-Binomial(N,p) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=n)=y.",
     "Inversa de la distribución binomial negativa de parámetros N y p "
     "BN(N,p)\n."
     "Para cada valor y entre 0 y 1, devuelve el número entero k tal que "
     "Prob(BN(N,p)<=k)=y."),
  BOperClassify::Probability_);
  void BDatNegBinomialInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BDat p = Dat (Arg(3));
  contens_ = Floor(BNegBinomialDist(N,p).Inverse(x));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGeometricDens);
  DefIntOpr(1, BDatGeometricDens, "ProbGeometric", 2, 2,
  "(Real k, Real p)",
  I2("Geometric(p) probability function.",
     "Función de probabilidad geométrica de parámetro p G(p)\n."
     "Devuelve Prob(G(p)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatGeometricDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat p = Dat (Arg(2));
  contens_ = BGeometricDist(p).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGeometricDist);
  DefIntOpr(1, BDatGeometricDist, "DistGeometric", 2, 2,
  "(Real k, Real p)",
  I2("Geometric(p) probability distribution.",
     "Distribución de probabilidad  geométrica de parámetro p G(p)\n."
     "Devuelve Prob(G(p)<=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatGeometricDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat p = Dat (Arg(2));
  contens_ = BGeometricDist(p).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGeometricInv);
  DefIntOpr(1, BDatGeometricInv, "DistGeometricInv", 2, 2,
  "(Real y, Real p)",
  I2("Inverse of Geometric(p) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=n)=y.",
     "Inversa de la distribución geométrica de parámetro p G(p)\n."
     "Para cada valor y entre 0 y 1, devuelve el número entero k tal que "
     "Prob(G(p)<=k)=y."),
  BOperClassify::Probability_);
  void BDatGeometricInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat p = Dat (Arg(2));
  contens_ = Floor(BGeometricDist(p).Inverse(x));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPoissonDens);
  DefIntOpr(1, BDatPoissonDens, "ProbPoisson", 2, 2,
  "(Real k, Real fi)",
  I2("Poisson(fi) probability function.",
     "Función de probabilidad de Poisson de media fi P(fi).\n."
     "Devuelve Prob(P(fi)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatPoissonDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat fi = Dat (Arg(2));
  contens_ = BPoissonDist(fi).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPoissonDist);
  DefIntOpr(1, BDatPoissonDist, "DistPoisson", 2, 2,
  "(Real k, Real fi)",
  I2("Poisson(fi) probability distribution.",
     "Distribución de probabilidad de Poisson de media fi P(fi).\n."
     "Devuelve Prob(P(fi)<=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatPoissonDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat fi = Dat (Arg(2));
  contens_ = BPoissonDist(fi).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPoissonInv);
  DefIntOpr(1, BDatPoissonInv, "DistPoissonInv", 2, 2,
  "(Real y, Real fi)",
  I2("Inverse of Poisson(fi) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=n)=y.",
     "Inversa de la distribución de Poisson de media fi P(fi).\n."
     "Para cada valor y entre 0 y 1, devuelve el número entero k tal que "
     "Prob(P(fi)<=k)=y."),
  BOperClassify::Probability_);
  void BDatPoissonInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BDat fi = Dat (Arg(2));
  contens_ = Floor(BPoissonDist(fi).Inverse(x));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHypergeometricDens);
  DefIntOpr(1, BDatHypergeometricDens, "ProbHyperG", 4, 4,
  "(Real k, Real N, Real P, Real n)",
  I2("Hypergeometric(N,P,n) probability function.",
     "Función de probabilidad hipergeométrica de parámetros N, p y n "
     "HG(N,P,n).\n"
     "Devuelve Prob(HG(N,P,n)=k).\n"
     "Si k no es entero se toma su parte entera."),
  BOperClassify::Probability_);
  void BDatHypergeometricDens::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BInt P = (BInt)Real(Arg(3));
  BInt n = (BInt)Real(Arg(4));
  contens_ = BHypergeometricDist(N,P,n).GetDens(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHypergeometricDist);
  DefIntOpr(1, BDatHypergeometricDist, "DistHyperG", 4, 4,
  "(Real k, Real N, Real P, Real n)",
  I2("Hypergeometric(N,P,n) probability distribution.",
     "Distribución de probabilidad hipergeométrica de parámetros N, p y n "
     "HG(N,P,n).\n"
     "Devuelve Prob(HG(N,P,n)<=k).\n"),
  BOperClassify::Probability_);
  void BDatHypergeometricDist::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BInt P = (BInt)Real(Arg(3));
  BInt n = (BInt)Real(Arg(4));
  contens_ = BHypergeometricDist(N,P,n).GetDist(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHypergeometricInv);
  DefIntOpr(1, BDatHypergeometricInv, "DistHyperGInv", 4, 4,
  "(Real y, Real N, Real P, Real n)",
  I2("Inverse of Hypergeometric(N,P,n) distribution. "
     "For each y between 0 and 1, it returns the integer number n such that "
     "Prob(X<=k)=y.",
     "Inversa de la distribución hipergeométrica de parámetros N, p y n.\n"
     "Para cada valor y entre 0 y 1, devuelve el número entero n tal que "
     "Prob(HG(N,P,n)<=k)=y."),
  BOperClassify::Probability_);
  void BDatHypergeometricInv::CalcContens()
//--------------------------------------------------------------------
{
  BDat x = Dat (Arg(1));
  BInt N = (BInt)Real(Arg(2));
  BInt P = (BInt)Real(Arg(3));
  BInt n = (BInt)Real(Arg(4));
  contens_ = BHypergeometricDist(N,P,n).Inverse(x);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDensMultiNormal);
  DefExtOpr(1, BDatDensMultiNormal, "DensMultiNormal", 3, 4,
            "Matrix Matrix Matrix Real",
            "(Matrix x, Matrix mean, Matrix cov [, Real log=1])",
            I2("Evaluate the density of a multivariate normal.\n"
               "x    --> variable where to evaluate density"
               "mean --> is the vector of means expressed as a column matrix.\n"
               "cov  --> is the variance-covariance matrix.\n"
               "log  --> optional argument. If log==1 then the log of the density is returned in other case the density is returned.\n\n"
               "Example:\n\n"
               "Matrix cov =\n"
               "((3.7779406,   0.8044385,  -3.518737,  0.7422526, -3.463709),\n"
               " (0.8044385,   1.8806651,  -3.127749,  1.0173059, -1.652422),\n"
               " (-3.5187373, -3.1277487,   8.993765, -1.8112513,  6.750959),\n"
               " (0.7422526,   1.0173059,  -1.811251,  1.9202310, -1.249161),\n"
               " (-3.4637086, -1.6524219,   6.750959, -1.2491613,  5.803521));\n\n"
               "Real log_dens = DensMultiNormal(Col(0,0,0,0,0),Col(0,0,0,0,0),cov);\n",
               "Evalua la density of una normal multivariada.\n"
               "x    --> variable donde evaluar la densidad"
               "mean --> es el vector de medias expresado como una matriz columna.\n"
               "cov  --> es la matriz de varianzas-covarianzas.\n"
               "log  --> argumento opcional. Si log==1 entonces se retorna el log de la densidad.\n\n"
               "Ejemplo:\n\n"
               "Matrix cov =\n"
               "((3.7779406,   0.8044385,  -3.518737,  0.7422526, -3.463709),\n"
               " (0.8044385,   1.8806651,  -3.127749,  1.0173059, -1.652422),\n"
               " (-3.5187373, -3.1277487,   8.993765, -1.8112513,  6.750959),\n"
               " (0.7422526,   1.0173059,  -1.811251,  1.9202310, -1.249161),\n"
               " (-3.4637086, -1.6524219,   6.750959, -1.2491613,  5.803521));\n\n"
               "Real log_dens = DensMultiNormal(Col(0,0,0,0,0),Col(0,0,0,0,0),cov);\n"),
            BOperClassify::Probability_);
void BDatDensMultiNormal::CalcContens()
//--------------------------------------------------------------------
{
  gsl_vector *x = NULL;
  gsl_vector *mean = NULL;
  gsl_matrix *cov = NULL;
  BMat *mat_x = &Mat(Arg(1));
  BMat *mat_mean = &Mat(Arg(2));
  BMat *mat_cov = &Mat(Arg(3));
  
  int M,N, VS;
  double density;

  M = mat_cov->Rows();
  N = mat_cov->Columns();

  if (M!=N) {
    Error(I2("DensMultiNormal: variance-covariance matrix should be square",
             "DensMultiNormal: la matriz de varianza-covarianza debe ser cuadrada"));
    return;
  }
  VS = mat_mean->Rows();
  if (VS != M || mat_mean->Columns() != 1) {
    Error(I2("DensMultiNormalTrunc: 'mean' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormalTrunc: el argumento 'mean' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  VS = mat_x->Rows();
  if (VS != M || mat_x->Columns() != 1) {
    Error(I2("DensMultiNormal: 'x' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormal: el argumento 'x' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  bmat_to_gsl(*mat_x, x);
  bmat_to_gsl(*mat_mean, mean);
  bmat_to_gsl(*mat_cov, cov);
  gsl_mvn_workspace_t mvn_ws;
  gsl_mvn_init(cov, 1, &mvn_ws);
  density = gsl_log_mvn_density(x, mean, &mvn_ws);
  if (Arg(4)) {
    BDat * log_flag = &Dat(Arg(4));
    if (!log_flag->IsKnown()) {
      Warning(I2("DensMultiNormal: argument 'log' is unknown default to 1",
                 "DensMultiNormal: argumento 'log' es desconocido asumiendo 1"));
    } else {
      if (!int(log_flag->Value()))
        density = exp(density);
    }
  }
  contens_.PutValue(density);
  gsl_mvn_done(&mvn_ws);  
  gsl_vector_free(x);
  gsl_vector_free(mean);
  gsl_matrix_free(cov);  
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDensMultiNormalTrunc);
  DefExtOpr(1, BDatDensMultiNormalTrunc, "DensMultiNormalTrunc", 5, 6,
            "Matrix Matrix Matrix Matrix Matrix Real",
            "(Matrix x, Matrix mean, Matrix cov, Matrix xl, Matrix xu [, Real log=1])",
            I2("Evaluate the density of a truncated multivariate normal.\n"
               "x    --> variable where to evaluate density"
               "mean --> is the vector of means expressed as a column matrix.\n"
               "cov  --> is the variance-covariance matrix.\n"
               "log  --> optional argument. If log==1 then the log of the density is returned in other case the density is returned.\n"
               "xl,xu --> define the truncation region as an hyperrectangle. Any point x belongs to this region iff xl[i]<= x[i] <= xu[i] for all 1 <= i <= dim(x).\n\n"
               "Example:\n\n"
               "Matrix cov =\n"
               "((3.7779406,   0.8044385,  -3.518737,  0.7422526, -3.463709),\n"
               " (0.8044385,   1.8806651,  -3.127749,  1.0173059, -1.652422),\n"
               " (-3.5187373, -3.1277487,   8.993765, -1.8112513,  6.750959),\n"
               " (0.7422526,   1.0173059,  -1.811251,  1.9202310, -1.249161),\n"
               " (-3.4637086, -1.6524219,   6.750959, -1.2491613,  5.803521));\n\n"
               "Real log_dens = DensMultiNormalTrunc(Col(0,0,0,0,0),Col(0,0,0,0,0),cov,\n"
               "                                     Col(0,0,0,0,0),Col(1,1,1,1,1));\n",
               "Evalua la density of una normal multivariada truncada.\n"
               "x    --> variable donde evaluar la densidad"
               "mean --> es el vector de medias expresado como una matriz columna.\n"
               "cov  --> es la matriz de varianzas-covarianzas.\n"
               "log  --> argumento opcional. Si log==1 entonces se retorna el log de la densidad.\n\n"
               "xl,xu --> define la región truncada como un hiperrectángullo. Cualquier punto x pertenece a esta región sii xl[i]<= x[i] <= xu[i] para todo 1 <= i <= dim(x).\n\n"
               "Ejemplo:\n\n"
               "Matrix cov =\n"
               "((3.7779406,   0.8044385,  -3.518737,  0.7422526, -3.463709),\n"
               " (0.8044385,   1.8806651,  -3.127749,  1.0173059, -1.652422),\n"
               " (-3.5187373, -3.1277487,   8.993765, -1.8112513,  6.750959),\n"
               " (0.7422526,   1.0173059,  -1.811251,  1.9202310, -1.249161),\n"
               " (-3.4637086, -1.6524219,   6.750959, -1.2491613,  5.803521));\n\n"
               "Real log_dens = DensMultiNormalTrunc(Col(0,0,0,0,0),Col(0,0,0,0,0),cov\n"
               "                                     Col(0,0,0,0,0),Col(1,1,1,1,1));\n"),
            BOperClassify::Probability_);
void BDatDensMultiNormalTrunc::CalcContens()
//--------------------------------------------------------------------
{
  BMat *mat_x = &Mat(Arg(1));
  BMat *mat_mean = &Mat(Arg(2));
  BMat *mat_cov = &Mat(Arg(3));
  BMat *mat_xl = &Mat(Arg(4));
  BMat *mat_xu = &Mat(Arg(5));
  gsl_vector *x = NULL;
  gsl_vector *mean = NULL;
  gsl_matrix *cov = NULL;
  gsl_vector *xl=NULL, *xu=NULL;
  int M,N, VS;
  double density;

  M = mat_cov->Rows();
  N = mat_cov->Columns();

  if (M!=N) {
    Error(I2("DensMultiNormalTrunc: variance-covariance matrix should be square",
             "DensMultiNormalTrunc: la matriz de varianza-covarianza debe ser cuadrada"));
    return;
  }
  VS = mat_mean->Rows();
  if (VS != M || mat_mean->Columns() != 1) {
    Error(I2("DensMultiNormalTrunc: 'mean' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormalTrunc: el argumento 'mean' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  VS = mat_x->Rows();
  if (VS != M || mat_x->Columns() != 1) {
    Error(I2("DensMultiNormalTrunc: 'x' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormalTrunc: el argumento 'x' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  VS = mat_xl->Rows();
  if (VS != M || mat_xl->Columns() != 1) {
    Error(I2("DensMultiNormalTrunc: 'xl' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormalTrunc: el argumento 'xl' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  VS = mat_xu->Rows();
  if (VS != M || mat_xu->Columns() != 1) {
    Error(I2("DensMultiNormalTrunc: 'xu' argument must be a column vector compatible with argument 'cov'",
             "DensMultiNormalTrunc: el argumento 'xu' debe ser un vector columna compatible con el argumento 'cov'"));
    return;
  }
  bmat_to_gsl(*mat_x, x);
  bmat_to_gsl(*mat_mean, mean);
  bmat_to_gsl(*mat_cov, cov);
  bmat_to_gsl(*mat_xl, xl);
  bmat_to_gsl(*mat_xu, xu);
  gsl_tmvn_workspace_t tmvn_ws;
  gsl_tmvn_init(mean, cov, 1, xl->data, xu->data, getGslRng(), 10000, &tmvn_ws);
  density = gsl_log_tmvn_density(x, mean, &tmvn_ws, xl->data, xu->data);
  if (Arg(6)) {
    BDat * log_flag = &Dat(Arg(6));
    if (!log_flag->IsKnown()) {
      Warning(I2("DensMultiNormalTrunc: argument 'log' is unknown default to 1",
                 "DensMultiNormalTrunc: argumento 'log' es desconocido asumiendo 1"));
    } else {
      if (!int(log_flag->Value()))
        density = exp(density);
    }
  }
  contens_.PutValue(density);
  gsl_tmvn_done(&tmvn_ws);  
  gsl_vector_free(x);
  gsl_vector_free(mean);
  gsl_matrix_free(cov);
  gsl_vector_free(xl);
  gsl_vector_free(xu);  
}
