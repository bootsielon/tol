/* tolgsl_usrapi_real.cpp: 

   Copyright (C) 2003 - Bayes Decisión, SL (Spain [EU])

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

/*!
  \file   tolgsl_usrapi_real.cpp
  \author vdebuen@tol-project.org
  \date   2007-02-05 12:00:05.00

  \brief  TOL Built-in functions to allow TOL users to work with GSL functions 
          which both returning and arguments are all of numeric type 
*/

//////////////////////////////////////////////////////////////////////////////
//
// ATTENTION!! NOTE FOR DEVELOPPERS:
//
//   This code has been automatically generated with TOL
//   Please don´t edit it. Any change should be made in TOL files
// 
//     tolgsl_api_real_def.tol
//     tolgsl_api_real_build.tol
// 
//////////////////////////////////////////////////////////////////////////////

/*

There are 387 functions in this TOL user's API about these GSL chapters 

  -Special Functions (http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html):
    -3-j Symbols : 1 functions;
    -6-j Symbols : 1 functions;
    -9-j Symbols : 1 functions;
    -Airy Functions : 4 functions;
    -Arctangent Integral : 1 functions;
    -Associated Legendre Polynomials and Spherical Harmonics : 2 functions;
    -Beta Functions : 2 functions;
    -Carlson Forms : 4 functions;
    -Circular Trigonometric Functions : 0 functions;
    -Clausen Functions : 1 functions;
    -Complementary Error Function : 1 functions;
    -Complete Fermi-Dirac Integrals : 8 functions;
    -Conical Functions : 6 functions;
    -Conversion Functions : 0 functions;
    -Coulomb Wave Function Normalization Constant : 0 functions;
    -Coulomb Wave Functions : 0 functions;
    -Dawson Function : 1 functions;
    -Debye Functions : 4 functions;
    -Derivatives of Airy Functions : 4 functions;
    -Digamma Function : 3 functions;
    -Ei(x) : 1 functions;
    -Ei_3(x) : 1 functions;
    -Error Function : 1 functions;
    -Eta Function : 2 functions;
    -Exponential Function : 0 functions;
    -Exponential Integral : 2 functions;
    -Factorials : 7 functions;
    -Gamma Functions : 4 functions;
    -Gegenbauer Functions : 4 functions;
    -Hurwitz Zeta Function : 1 functions;
    -Hyperbolic Integrals : 2 functions;
    -Hyperbolic Trigonometric Functions : 1 functions;
    -Hypergeometric Functions : 8 functions;
    -Incomplete Beta Function : 1 functions;
    -Incomplete Fermi-Dirac Integrals : 1 functions;
    -Incomplete Gamma Functions : 3 functions;
    -Irregular Bessel Functions - Fractional Order : 1 functions;
    -Irregular Cylindrical Bessel Functions : 3 functions;
    -Irregular Modified Bessel Functions - Fractional Order : 3 functions;
    -Irregular Modified Cylindrical Bessel Functions : 6 functions;
    -Irregular Modified Spherical Bessel Functions : 4 functions;
    -Irregular Spherical Bessel Functions : 4 functions;
    -Laguerre Functions : 4 functions;
    -Lambert W Functions : 2 functions;
    -Legendre Form of Complete Elliptic Integrals : 2 functions;
    -Legendre Form of Incomplete Elliptic Integrals : 4 functions;
    -Legendre Polynomials : 7 functions;
    -Log Complementary Error Function : 1 functions;
    -Logarithm and Related Functions : 4 functions;
    -Normalized Hydrogenic Bound States : 2 functions;
    -Pochhammer Symbol : 3 functions;
    -Polygamma Function : 1 functions;
    -Power Function : 1 functions;
    -Probability functions : 3 functions;
    -Radial Functions for Hyperbolic Space : 3 functions;
    -Real Argument : 1 functions;
    -Regular Bessel Function - Fractional Order : 1 functions;
    -Regular Cylindrical Bessel Functions : 3 functions;
    -Regular Modified Bessel Functions - Fractional Order : 2 functions;
    -Regular Modified Cylindrical Bessel Functions : 6 functions;
    -Regular Modified Spherical Bessel Functions : 4 functions;
    -Regular Spherical Bessel Functions : 4 functions;
    -Relative Exponential Functions : 4 functions;
    -Restriction Functions : 0 functions;
    -Riemann Zeta Function : 2 functions;
    -Riemann Zeta Function Minus One : 2 functions;
    -Synchrotron Functions : 2 functions;
    -Transport Functions : 4 functions;
    -Trigamma Function : 1 functions;
    -Trigonometric Integrals : 2 functions;
    -Zeros of Airy Functions : 2 functions;
    -Zeros of Derivatives of Airy Functions : 2 functions;
    -Zeros of Regular Bessel Functions : 3 functions;

  -Random Number Distributions (http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html):
    -The Beta Distribution : 6 functions;
    -The Bernoulli Distribution : 2 functions;
    -The Binomial Distribution : 4 functions;
    -The Bivariate Gaussian Distribution : 1 functions;
    -The Cauchy Distribution : 6 functions;
    -The Chi-squared Distribution : 6 functions;
    -General Discrete Distributions : 0 functions;
    -The Exponential Distribution : 6 functions;
    -The Exponential Power Distribution : 4 functions;
    -The F-distribution : 6 functions;
    -The Flat (Uniform) Distribution : 6 functions;
    -The Gamma Distribution : 7 functions;
    -The Gaussian Distribution : 15 functions;
    -The Gaussian Tail Distribution : 4 functions;
    -The Geometric Distribution : 4 functions;
    -The Type-1 Gumbel Distribution : 6 functions;
    -The Type-2 Gumbel Distribution : 6 functions;
    -The Hypergeometric Distribution : 4 functions;
    -The Landau Distribution : 2 functions;
    -The Laplace Distribution : 6 functions;
    -The Levy alpha-Stable Distributions : 1 functions;
    -The Levy skew alpha-Stable Distribution : 1 functions;
    -The Logarithmic Distribution : 2 functions;
    -The Logistic Distribution : 6 functions;
    -The Lognormal Distribution : 6 functions;
    -The Negative Binomial Distribution : 4 functions;
    -The Pareto Distribution : 6 functions;
    -The Pascal Distribution : 4 functions;
    -The Poisson Distribution : 4 functions;
    -The Rayleigh Distribution : 6 functions;
    -The Rayleigh Tail Distribution : 2 functions;
    -The t-distribution : 6 functions;
    -The Weibull Distribution : 6 functions;
    -Shuffling and Sampling : 0 functions;

*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bprdist_internal.h>
#include <tol/tol_gsl.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_cdf.h>
#include <tol/tol_bmatgra.h>


//--------------------------------------------------------------------
// specialized macro for GSL reals to real functions declaration
//--------------------------------------------------------------------

//Numerical representation of GSL string version
#define GSL_VERSION_NUM 10800

#define DecDefIntOprDatGsl(ORD, NAME, MINARG, MAXARG, LISTARGS, DESC)   \
  DeclareContensClass(BDat, BDatTemporary, BDat_##NAME); \
  DefIntOpr(ORD, BDat_##NAME, #NAME, MINARG, MAXARG, LISTARGS, DESC, BOperClassify::RealArythmetic_GSL_);

#define DecDefIntOprMatGsl(ORD, NAME, MINARG, MAXARG, LISTARGS, DESC)   \
  DeclareContensClass(BMat, BMatTemporary, BMat_##NAME); \
  DefIntOpr(ORD, BMat_##NAME, #NAME, MINARG, MAXARG, LISTARGS, DESC, BOperClassify::RealArythmetic_GSL_);

#define  ErrorGslFVer(GSLFVER, funName) \
  Error(I2("Needed GSL version ", "Se necesita la version de GSL ") << GSLFVER<< \
        I2(" in order to use function ", " para usar la función ") << funName);

BTraceInit("tolgsl_usrapi_real.cpp");

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_coupling_3j, 6, 6, 
  "(Real two_ja, Real two_jb, Real two_jc, Real two_ma, Real two_mb, Real two_mc)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [3-j Symbols]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/3_002dj-Symbols.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/Wigner3j-Symbol.html");
  void BDat_gsl_sf_coupling_3j::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_coupling_3j((int)Real(Arg(1)), (int)Real(Arg(2)), (int)Real(Arg(3)), (int)Real(Arg(4)), (int)Real(Arg(5)), (int)Real(Arg(6))); 
#else
  ErrorGslFVer(10400,"gsl_sf_coupling_3j");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_coupling_6j, 6, 6, 
  "(Real two_ja, Real two_jb, Real two_jc, Real two_jd, Real two_je, Real two_jf)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [6-j Symbols]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/6_002dj-Symbols.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/Wigner6j-Symbol.html");
  void BDat_gsl_sf_coupling_6j::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_coupling_6j((int)Real(Arg(1)), (int)Real(Arg(2)), (int)Real(Arg(3)), (int)Real(Arg(4)), (int)Real(Arg(5)), (int)Real(Arg(6))); 
#else
  ErrorGslFVer(10400,"gsl_sf_coupling_6j");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_coupling_9j, 9, 9, 
  "(Real two_ja, Real two_jb, Real two_jc, Real two_jd, Real two_je, Real two_jf, Real two_jg, Real two_jh, Real two_ji)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [9-j Symbols]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/9_002dj-Symbols.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/Wigner9j-Symbol.html");
  void BDat_gsl_sf_coupling_9j::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_coupling_9j((int)Real(Arg(1)), (int)Real(Arg(2)), (int)Real(Arg(3)), (int)Real(Arg(4)), (int)Real(Arg(5)), (int)Real(Arg(6)), (int)Real(Arg(7)), (int)Real(Arg(8)), (int)Real(Arg(9))); 
#else
  ErrorGslFVer(10400,"gsl_sf_coupling_9j");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Ai, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Ai::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Ai(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Ai");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Ai_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Ai_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Ai_scaled(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Ai_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Bi, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Bi::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Bi(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Bi");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Bi_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Bi_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Bi_scaled(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Bi_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_atanint, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Arctangent Integral]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Arctangent-Integral.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ArctangentIntegral.html");
  void BDat_gsl_sf_atanint::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_atanint(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_atanint");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_Plm, 3, 3, 
  "(Real l, Real m, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Associated Legendre Polynomials and Spherical Harmonics]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Associated-Legendre-Polynomials-and-Spherical-Harmonics.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LegendrePolynomial.html");
  void BDat_gsl_sf_legendre_Plm::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_Plm((int)Real(Arg(1)), (int)Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_Plm");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_sphPlm, 3, 3, 
  "(Real l, Real m, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Associated Legendre Polynomials and Spherical Harmonics]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Associated-Legendre-Polynomials-and-Spherical-Harmonics.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LegendrePolynomial.html");
  void BDat_gsl_sf_legendre_sphPlm::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_sphPlm((int)Real(Arg(1)), (int)Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_sphPlm");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_beta, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Beta Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Beta-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/BetaFunction.html");
  void BDat_gsl_sf_beta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_beta(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_beta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lnbeta, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Beta Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Beta-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/BetaFunction.html");
  void BDat_gsl_sf_lnbeta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lnbeta(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lnbeta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_RC, 2, 2, 
  "(Real x, Real y)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Carlson Forms]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Carlson-Forms.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_RC::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_RC(Real(Arg(1)), Real(Arg(2)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_RC");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_RD, 3, 3, 
  "(Real x, Real y, Real z)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Carlson Forms]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Carlson-Forms.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_RD::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_RD(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_RD");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_RF, 3, 3, 
  "(Real x, Real y, Real z)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Carlson Forms]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Carlson-Forms.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_RF::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_RF(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_RF");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_RJ, 4, 4, 
  "(Real x, Real y, Real z, Real p)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Carlson Forms]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Carlson-Forms.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_RJ::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_RJ(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), Real(Arg(4)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_RJ");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_clausen, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Clausen Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Clausen-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ClausenFunction.html");
  void BDat_gsl_sf_clausen::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_clausen(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_clausen");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_erfc, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complementary Error Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complementary-Error-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/Erfc.html");
  void BDat_gsl_sf_erfc::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_erfc(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_erfc");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_3half, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_3half::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_3half(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_3half");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_half, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_half::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_half(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_half");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_int, 2, 2, 
  "(Real j, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_int((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_m1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_m1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_m1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_m1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_mhalf, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Complete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Complete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Complete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_mhalf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_mhalf(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_mhalf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_0, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_0(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_1, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_cyl_reg, 3, 3, 
  "(Real m, Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_cyl_reg::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_cyl_reg((int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_cyl_reg");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_half, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_half::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_half(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_half");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_mhalf, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_mhalf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_mhalf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_mhalf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_conicalP_sph_reg, 3, 3, 
  "(Real l, Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Conical Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Conical-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/ConicalFunction.html");
  void BDat_gsl_sf_conicalP_sph_reg::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_conicalP_sph_reg((int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_conicalP_sph_reg");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_dawson, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Dawson Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Dawson-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Dawson_function");
  void BDat_gsl_sf_dawson::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_dawson(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_dawson");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_debye_1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Debye Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Debye-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Debye_function");
  void BDat_gsl_sf_debye_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_debye_1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_debye_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_debye_2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Debye Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Debye-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Debye_function");
  void BDat_gsl_sf_debye_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_debye_2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_debye_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_debye_3, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Debye Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Debye-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Debye_function");
  void BDat_gsl_sf_debye_3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_debye_3(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_debye_3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_debye_4, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Debye Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Debye-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Debye_function");
  void BDat_gsl_sf_debye_4::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_debye_4(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_debye_4");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Ai_deriv, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Ai_deriv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Ai_deriv(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Ai_deriv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Ai_deriv_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Ai_deriv_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Ai_deriv_scaled(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Ai_deriv_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Bi_deriv, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Bi_deriv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Bi_deriv(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Bi_deriv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_Bi_deriv_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_Bi_deriv_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_Bi_deriv_scaled(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_Bi_deriv_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_psi, 1, 1, 
  "(Real y)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Digamma Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Digamma-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Digamma_function");
  void BDat_gsl_sf_psi::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_psi(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_psi");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_psi_1piy, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Digamma Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Digamma-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Digamma_function");
  void BDat_gsl_sf_psi_1piy::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_psi_1piy((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_psi_1piy");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_psi_int, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Digamma Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Digamma-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Digamma_function");
  void BDat_gsl_sf_psi_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_psi_int((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_psi_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_expint_Ei, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Ei(x)]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Ei_0028x_0029.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_integral");
  void BDat_gsl_sf_expint_Ei::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_expint_Ei(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_expint_Ei");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_expint_3, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Ei_3(x)]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Ei_005f3_0028x_0029.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_integral");
  void BDat_gsl_sf_expint_3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_expint_3(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_expint_3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_erf, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Error Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Error-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Error_function");
  void BDat_gsl_sf_erf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_erf(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_erf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_eta, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Eta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Eta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Dedekind_eta_function");
  void BDat_gsl_sf_eta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_eta(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_eta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_eta_int, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Eta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Eta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Dedekind_eta_function");
  void BDat_gsl_sf_eta_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_eta_int((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_eta_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_expint_E1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Exponential Integral]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Exponential-Integral.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_integral");
  void BDat_gsl_sf_expint_E1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_expint_E1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_expint_E1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_expint_E2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Exponential Integral]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Exponential-Integral.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_integral");
  void BDat_gsl_sf_expint_E2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_expint_E2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_expint_E2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_choose, 2, 2, 
  "(Real n, Real m)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_choose::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_choose((unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_choose");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_doublefact, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_doublefact::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_doublefact((unsigned int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_doublefact");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fact, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_fact::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fact((unsigned int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fact");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lnchoose, 2, 2, 
  "(Real n, Real m)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_lnchoose::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lnchoose((unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lnchoose");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lndoublefact, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_lndoublefact::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lndoublefact((unsigned int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lndoublefact");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lnfact, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_lnfact::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lnfact((unsigned int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lnfact");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_taylorcoeff, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Factorials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Factorials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Factorial");
  void BDat_gsl_sf_taylorcoeff::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_taylorcoeff((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_taylorcoeff");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gamma, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_function");
  void BDat_gsl_sf_gamma::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gamma(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gamma");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gammainv, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_function");
  void BDat_gsl_sf_gammainv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gammainv(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gammainv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gammastar, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_function");
  void BDat_gsl_sf_gammastar::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gammastar(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gammastar");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lngamma, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_function");
  void BDat_gsl_sf_lngamma::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lngamma(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lngamma");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gegenpoly_1, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gegenbauer Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gegenbauer-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gegenbauer_polynomials");
  void BDat_gsl_sf_gegenpoly_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gegenpoly_1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gegenpoly_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gegenpoly_2, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gegenbauer Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gegenbauer-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gegenbauer_polynomials");
  void BDat_gsl_sf_gegenpoly_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gegenpoly_2(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gegenpoly_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gegenpoly_3, 2, 2, 
  "(Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gegenbauer Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gegenbauer-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gegenbauer_polynomials");
  void BDat_gsl_sf_gegenpoly_3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gegenpoly_3(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gegenpoly_3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gegenpoly_n, 3, 3, 
  "(Real n, Real lambda, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Gegenbauer Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Gegenbauer-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gegenbauer_polynomials");
  void BDat_gsl_sf_gegenpoly_n::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gegenpoly_n((int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gegenpoly_n");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hzeta, 2, 2, 
  "(Real s, Real q)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hurwitz Zeta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hurwitz-Zeta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hurwitz_zeta_function");
  void BDat_gsl_sf_hzeta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hzeta(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hzeta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_Chi, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hyperbolic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hyperbolic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hyperbolic_function");
  void BDat_gsl_sf_Chi::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_Chi(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_Chi");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_Shi, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hyperbolic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hyperbolic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hyperbolic_function");
  void BDat_gsl_sf_Shi::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_Shi(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_Shi");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lnsinh, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hyperbolic Trigonometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hyperbolic-Trigonometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hyperbolic_function");
  void BDat_gsl_sf_lnsinh::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lnsinh(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lnsinh");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_0F1, 2, 2, 
  "(Real c, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_0F1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_0F1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_0F1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_1F1, 3, 3, 
  "(Real a, Real b, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_1F1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_1F1(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_1F1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_1F1_int, 3, 3, 
  "(Real m, Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_1F1_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_1F1_int((int)Real(Arg(1)), (int)Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_1F1_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_2F0, 3, 3, 
  "(Real a, Real b, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_2F0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_2F0(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_2F0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_2F1, 4, 4, 
  "(Real a, Real b, Real c, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_2F1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_2F1(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), Real(Arg(4))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_2F1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_2F1_renorm, 4, 4, 
  "(Real a, Real b, Real c, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_2F1_renorm::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_2F1_renorm(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), Real(Arg(4))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_2F1_renorm");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_U, 3, 3, 
  "(Real a, Real b, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_U::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_U(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_U");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hyperg_U_int, 3, 3, 
  "(Real m, Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Hypergeometric Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Hypergeometric-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/HypergeometricFunction.html");
  void BDat_gsl_sf_hyperg_U_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hyperg_U_int((int)Real(Arg(1)), (int)Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hyperg_U_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_beta_inc, 3, 3, 
  "(Real a, Real b, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Incomplete Beta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Incomplete-Beta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_function");
  void BDat_gsl_sf_beta_inc::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_beta_inc(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_beta_inc");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_fermi_dirac_inc_0, 2, 2, 
  "(Real x, Real b)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Incomplete Fermi-Dirac Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Incomplete-Fermi_002dDirac-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Incomplete_Fermi-Dirac_integral");
  void BDat_gsl_sf_fermi_dirac_inc_0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_fermi_dirac_inc_0(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_fermi_dirac_inc_0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gamma_inc, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Incomplete Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Incomplete-Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Incomplete_gamma_function");
  void BDat_gsl_sf_gamma_inc::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gamma_inc(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gamma_inc");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gamma_inc_P, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Incomplete Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Incomplete-Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Incomplete_gamma_function");
  void BDat_gsl_sf_gamma_inc_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gamma_inc_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gamma_inc_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_gamma_inc_Q, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Incomplete Gamma Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Incomplete-Gamma-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Incomplete_gamma_function");
  void BDat_gsl_sf_gamma_inc_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_gamma_inc_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_gamma_inc_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Ynu, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Ynu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Ynu(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Ynu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Y0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Y0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Y0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Y0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Y1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Y1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Y1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Y1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Yn, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Yn::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Yn((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Yn");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Knu, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Knu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Knu(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Knu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Knu_scaled, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Knu_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Knu_scaled(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Knu_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_lnKnu, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_lnKnu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_lnKnu(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_lnKnu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_K0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_K0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_K0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_K0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_K0_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_K0_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_K0_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_K0_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_K1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_K1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_K1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_K1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_K1_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_K1_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_K1_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_K1_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Kn, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Kn::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Kn((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Kn");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Kn_scaled, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Kn_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Kn_scaled((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Kn_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_k0_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_k0_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_k0_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_k0_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_k1_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_k1_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_k1_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_k1_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_k2_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_k2_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_k2_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_k2_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_kl_scaled, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_kl_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_kl_scaled((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_kl_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_y0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_y0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_y0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_y0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_y1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_y1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_y1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_y1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_y2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_y2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_y2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_y2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_yl, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Irregular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Irregular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_yl::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_yl((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_yl");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_laguerre_1, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Laguerre Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Laguerre-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LaguerrePolynomial.html");
  void BDat_gsl_sf_laguerre_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_laguerre_1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_laguerre_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_laguerre_2, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Laguerre Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Laguerre-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LaguerrePolynomial.html");
  void BDat_gsl_sf_laguerre_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_laguerre_2(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_laguerre_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_laguerre_3, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Laguerre Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Laguerre-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LaguerrePolynomial.html");
  void BDat_gsl_sf_laguerre_3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_laguerre_3(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_laguerre_3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_laguerre_n, 3, 3, 
  "(Real n, Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Laguerre Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Laguerre-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/LaguerrePolynomial.html");
  void BDat_gsl_sf_laguerre_n::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_laguerre_n((int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_laguerre_n");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lambert_W0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Lambert W Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Lambert-W-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Lambert's_W_function");
  void BDat_gsl_sf_lambert_W0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lambert_W0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lambert_W0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lambert_Wm1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Lambert W Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Lambert-W-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Lambert's_W_function");
  void BDat_gsl_sf_lambert_Wm1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lambert_Wm1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lambert_Wm1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_Ecomp, 1, 1, 
  "(Real k)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Complete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Complete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_Ecomp::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_Ecomp(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_Ecomp");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_Kcomp, 1, 1, 
  "(Real k)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Complete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Complete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_Kcomp::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_Kcomp(Real(Arg(1)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_Kcomp");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_D, 3, 3, 
  "(Real phi, Real k, Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Incomplete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Incomplete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_D::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_D(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_D");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_E, 2, 2, 
  "(Real phi, Real k)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Incomplete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Incomplete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_E::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_E(Real(Arg(1)), Real(Arg(2)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_E");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_F, 2, 2, 
  "(Real phi, Real k)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Incomplete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Incomplete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_F::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_F(Real(Arg(1)), Real(Arg(2)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_F");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_ellint_P, 3, 3, 
  "(Real phi, Real k, Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Form of Incomplete Elliptic Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Form-of-Incomplete-Elliptic-Integrals.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Elliptic_integral");
  void BDat_gsl_sf_ellint_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_ellint_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), GSL_PREC_DOUBLE); 
#else
  ErrorGslFVer(10400,"gsl_sf_ellint_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_P1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_P1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_P1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_P1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_P2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_P2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_P2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_P2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_P3, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_P3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_P3(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_P3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_Pl, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_Pl::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_Pl((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_Pl");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_Q0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_Q0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_Q0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_Q0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_Q1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_Q1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_Q1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_Q1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_Ql, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Legendre Polynomials]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Legendre-Polynomials.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Legendre_polynomials");
  void BDat_gsl_sf_legendre_Ql::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_Ql((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_Ql");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_log_erfc, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Log Complementary Error Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Log-Complementary-Error-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Error_function");
  void BDat_gsl_sf_log_erfc::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_log_erfc(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_log_erfc");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_log, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Logarithm and Related Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Logarithm-and-Related-Functions.html\n"
  "");
  void BDat_gsl_sf_log::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_log(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_log");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_log_1plusx, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Logarithm and Related Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Logarithm-and-Related-Functions.html\n"
  "");
  void BDat_gsl_sf_log_1plusx::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_log_1plusx(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_log_1plusx");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_log_1plusx_mx, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Logarithm and Related Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Logarithm-and-Related-Functions.html\n"
  "");
  void BDat_gsl_sf_log_1plusx_mx::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_log_1plusx_mx(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_log_1plusx_mx");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_log_abs, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Logarithm and Related Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Logarithm-and-Related-Functions.html\n"
  "");
  void BDat_gsl_sf_log_abs::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_log_abs(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_log_abs");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hydrogenicR, 4, 4, 
  "(Real n, Real l, Real Z, Real r)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Normalized Hydrogenic Bound States]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Normalized-Hydrogenic-Bound-States.html\n"
  "");
  void BDat_gsl_sf_hydrogenicR::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hydrogenicR((int)Real(Arg(1)), (int)Real(Arg(2)), Real(Arg(3)), Real(Arg(4))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hydrogenicR");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hydrogenicR_1, 2, 2, 
  "(Real Z, Real r)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Normalized Hydrogenic Bound States]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Normalized-Hydrogenic-Bound-States.html\n"
  "");
  void BDat_gsl_sf_hydrogenicR_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hydrogenicR_1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hydrogenicR_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_lnpoch, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Pochhammer Symbol]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Pochhammer-Symbol.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pochhammer_symbol");
  void BDat_gsl_sf_lnpoch::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_lnpoch(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_lnpoch");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_poch, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Pochhammer Symbol]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Pochhammer-Symbol.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pochhammer_symbol");
  void BDat_gsl_sf_poch::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_poch(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_poch");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_pochrel, 2, 2, 
  "(Real a, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Pochhammer Symbol]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Pochhammer-Symbol.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pochhammer_symbol");
  void BDat_gsl_sf_pochrel::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_pochrel(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_pochrel");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_psi_n, 2, 2, 
  "(Real m, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Polygamma Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Polygamma-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Polygamma_function");
  void BDat_gsl_sf_psi_n::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_psi_n((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_psi_n");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_pow_int, 2, 2, 
  "(Real x, Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Power Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Power-Function.html\n"
  "");
  void BDat_gsl_sf_pow_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_pow_int(Real(Arg(1)), (int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_pow_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_erf_Q, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Probability functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Probability-functions.html\n"
  "");
  void BDat_gsl_sf_erf_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_erf_Q(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_erf_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_erf_Z, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Probability functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Probability-functions.html\n"
  "");
  void BDat_gsl_sf_erf_Z::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_erf_Z(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_erf_Z");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_hazard, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Probability functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Probability-functions.html\n"
  "");
  void BDat_gsl_sf_hazard::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_hazard(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_hazard");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_H3d, 3, 3, 
  "(Real l, Real lambda, Real eta)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Radial Functions for Hyperbolic Space]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Radial-Functions-for-Hyperbolic-Space.html\n"
  "");
  void BDat_gsl_sf_legendre_H3d::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_H3d((int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_H3d");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_H3d_0, 2, 2, 
  "(Real lambda, Real eta)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Radial Functions for Hyperbolic Space]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Radial-Functions-for-Hyperbolic-Space.html\n"
  "");
  void BDat_gsl_sf_legendre_H3d_0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_H3d_0(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_H3d_0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_legendre_H3d_1, 2, 2, 
  "(Real lambda, Real eta)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Radial Functions for Hyperbolic Space]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Radial-Functions-for-Hyperbolic-Space.html\n"
  "");
  void BDat_gsl_sf_legendre_H3d_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_legendre_H3d_1(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_legendre_H3d_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_dilog, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Real Argument]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Real-Argument.html\n"
  "");
  void BDat_gsl_sf_dilog::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_dilog(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_dilog");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Jnu, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Bessel Function - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Bessel-Function-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Jnu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Jnu(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Jnu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_J0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_J0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_J0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_J0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_J1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_J1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_J1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_J1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Jn, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Jn::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Jn((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Jn");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Inu, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Inu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Inu(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Inu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_Inu_scaled, 2, 2, 
  "(Real nu, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Bessel Functions - Fractional Order]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Bessel-Functions-_002d-Fractional-Order.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_Inu_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_Inu_scaled(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_Inu_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_I0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_I0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_I0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_I0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_I0_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_I0_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_I0_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_I0_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_I1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_I1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_I1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_I1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_I1_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_I1_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_I1_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_I1_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_In, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_In::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_In((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_In");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_In_scaled, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Cylindrical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Cylindrical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_In_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_In_scaled((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_In_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_i0_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_i0_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_i0_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_i0_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_i1_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_i1_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_i1_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_i1_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_i2_scaled, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_i2_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_i2_scaled(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_i2_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_il_scaled, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Modified Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Modified-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_il_scaled::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_il_scaled((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_il_scaled");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_j0, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_j0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_j0(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_j0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_j1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_j1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_j1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_j1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_j2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_j2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_j2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_j2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_jl, 2, 2, 
  "(Real l, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Regular Spherical Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Regular-Spherical-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_jl::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_jl((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_jl");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_expm1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Relative Exponential Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Relative-Exponential-Functions.html\n"
  "");
  void BDat_gsl_sf_expm1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_expm1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_expm1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_exprel, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Relative Exponential Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Relative-Exponential-Functions.html\n"
  "");
  void BDat_gsl_sf_exprel::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_exprel(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_exprel");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_exprel_2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Relative Exponential Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Relative-Exponential-Functions.html\n"
  "");
  void BDat_gsl_sf_exprel_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_exprel_2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_exprel_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_exprel_n, 2, 2, 
  "(Real n, Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Relative Exponential Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Relative-Exponential-Functions.html\n"
  "");
  void BDat_gsl_sf_exprel_n::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_exprel_n((int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_exprel_n");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_zeta, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Riemann Zeta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Riemann-Zeta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Riemann_zeta_function");
  void BDat_gsl_sf_zeta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_zeta(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_zeta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_zeta_int, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Riemann Zeta Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Riemann-Zeta-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Riemann_zeta_function");
  void BDat_gsl_sf_zeta_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_zeta_int((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_zeta_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_zetam1, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Riemann Zeta Function Minus One]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Riemann-Zeta-Function-Minus-One.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Riemann_zeta_function");
  void BDat_gsl_sf_zetam1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_sf_zetam1(Real(Arg(1))); 
#else
  ErrorGslFVer(10600,"gsl_sf_zetam1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_zetam1_int, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Riemann Zeta Function Minus One]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Riemann-Zeta-Function-Minus-One.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Riemann_zeta_function");
  void BDat_gsl_sf_zetam1_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_sf_zetam1_int((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10600,"gsl_sf_zetam1_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_synchrotron_1, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Synchrotron Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Synchrotron_function");
  void BDat_gsl_sf_synchrotron_1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_synchrotron_1(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_synchrotron_1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_synchrotron_2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Synchrotron Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Synchrotron-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Synchrotron_function");
  void BDat_gsl_sf_synchrotron_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_synchrotron_2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_synchrotron_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_transport_2, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Transport Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Transport-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Transport_function");
  void BDat_gsl_sf_transport_2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_transport_2(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_transport_2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_transport_3, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Transport Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Transport-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Transport_function");
  void BDat_gsl_sf_transport_3::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_transport_3(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_transport_3");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_transport_4, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Transport Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Transport-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Transport_function");
  void BDat_gsl_sf_transport_4::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_transport_4(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_transport_4");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_transport_5, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Transport Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Transport-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Transport_function");
  void BDat_gsl_sf_transport_5::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_transport_5(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_transport_5");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_psi_1_int, 1, 1, 
  "(Real n)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Trigamma Function]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Trigamma-Function.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Trigamma_function");
  void BDat_gsl_sf_psi_1_int::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_psi_1_int((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_psi_1_int");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_Ci, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Trigonometric Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Trigonometric-Integrals.html\n"
  "");
  void BDat_gsl_sf_Ci::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_Ci(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_Ci");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_Si, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Trigonometric Integrals]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Trigonometric-Integrals.html\n"
  "");
  void BDat_gsl_sf_Si::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_Si(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_Si");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_zero_Ai, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_zero_Ai::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_zero_Ai((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_zero_Ai");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_zero_Bi, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_zero_Bi::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_zero_Bi((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_zero_Bi");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_zero_Ai_deriv, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_zero_Ai_deriv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_zero_Ai_deriv((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_zero_Ai_deriv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_airy_zero_Bi_deriv, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Derivatives of Airy Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Derivatives-of-Airy-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://mathworld.wolfram.com/AiryFunctions.html");
  void BDat_gsl_sf_airy_zero_Bi_deriv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_airy_zero_Bi_deriv((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_airy_zero_Bi_deriv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_zero_J0, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Regular Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Regular-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_zero_J0::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_zero_J0((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_zero_J0");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_zero_J1, 1, 1, 
  "(Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Regular Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Regular-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_zero_J1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_zero_J1((int)Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_zero_J1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_sf_bessel_zero_Jnu, 2, 2, 
  "(Real nu, Real s)",
  "[GSL: GNU Scientific Library] [C1_SpecialFun] [Zeros of Regular Bessel Functions]\n"
  "GSL chapter: Special Functions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Special-Functions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Zeros-of-Regular-Bessel-Functions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bessel_function");
  void BDat_gsl_sf_bessel_zero_Jnu::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_sf_bessel_zero_Jnu(Real(Arg(1)), (int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_sf_bessel_zero_Jnu");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_bernoulli, 1, 1, 
  "(Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Bernoulli Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Bernoulli-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bernoulli_distribution");
  void BDat_gsl_ran_bernoulli::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_bernoulli(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_bernoulli");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_bernoulli_pdf, 2, 2, 
  "(Real k, Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Bernoulli Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Bernoulli-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Bernoulli_distribution");
  void BDat_gsl_ran_bernoulli_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_bernoulli_pdf((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_bernoulli_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_beta_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_cdf_beta_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_beta_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_beta_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_beta_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_cdf_beta_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_beta_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_beta_Pinv");
#endif
}

#define dMat(arg) ((DMat&)Mat(arg))
#define b2dMat(M) ((DMat&)(M))

//--------------------------------------------------------------------
  DecDefIntOprMatGsl(1, gsl_M_cdf_beta_P, 3, 3, 
  "(Matrix P, Matrix a, Matrix b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BMat_gsl_M_cdf_beta_P::CalcContens()
//--------------------------------------------------------------------
{ 
  DMat& P = dMat(Arg(1));
  DMat& A = dMat(Arg(2));
  DMat& B = dMat(Arg(3));
  double *p, *a, *b, *q;
  int i;
  int r = P.Rows();
  int c = P.Columns();
  int n = r*c;
  b2dMat(contens_) = P;
  p = P.GetData().GetBuffer();
  a = A.GetData().GetBuffer();
  b = B.GetData().GetBuffer();
  q = b2dMat(contens_).GetData().GetBuffer();
  for(i=0; i<r; i++, p++, a++, b++, q++)
  {
    *q = gsl_cdf_beta_P(*p, *a, *b);
  }
}

//--------------------------------------------------------------------
  DecDefIntOprMatGsl(1, gsl_M_cdf_beta_Pinv, 3, 3, 
  "(Matrix P, Matrix a, Matrix b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BMat_gsl_M_cdf_beta_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  DMat& P = dMat(Arg(1));
  DMat& A = dMat(Arg(2));
  DMat& B = dMat(Arg(3));
  double *p, *a, *b, *q;
  int i;
  int r = P.Rows();
  int c = P.Columns();
  int n = r*c;
  b2dMat(contens_) = P;
  p = P.GetData().GetBuffer();
  a = A.GetData().GetBuffer();
  b = B.GetData().GetBuffer();
  q = b2dMat(contens_).GetData().GetBuffer();
  for(i=0; i<r; i++, p++, a++, b++, q++)
  {
  //*q = gsl_cdf_beta_Pinv(*p, *a, *b);
    *q = BBetaDist::InvInc(*p,*a,*b).Value();
  }
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_beta_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_cdf_beta_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_beta_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_beta_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_beta_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_cdf_beta_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_beta_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_beta_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_beta, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_ran_beta::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_beta(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_beta");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_beta_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Beta Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Beta-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Beta_distribution");
  void BDat_gsl_ran_beta_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_beta_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_beta_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_binomial_P, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Binomial_distribution");
  void BDat_gsl_cdf_binomial_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_binomial_P((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_binomial_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_binomial_Q, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Binomial_distribution");
  void BDat_gsl_cdf_binomial_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_binomial_Q((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_binomial_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_binomial, 2, 2, 
  "(Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Binomial_distribution");
  void BDat_gsl_ran_binomial::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_binomial(getGslRng(), Real(Arg(1)), (unsigned int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_binomial");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_binomial_pdf, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Binomial_distribution");
  void BDat_gsl_ran_binomial_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_binomial_pdf((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_binomial_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_bivariate_gaussian_pdf, 5, 5, 
  "(Real x, Real y, Real sigma_x, Real sigma_y, Real rho)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Bivariate Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Bivariate-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Multivariate_normal_distribution");
  void BDat_gsl_ran_bivariate_gaussian_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_bivariate_gaussian_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3)), Real(Arg(4)), Real(Arg(5))); 
#else
  ErrorGslFVer(10400,"gsl_ran_bivariate_gaussian_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_cauchy_P, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_cdf_cauchy_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_cauchy_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_cauchy_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_cauchy_Pinv, 2, 2, 
  "(Real P, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_cdf_cauchy_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_cauchy_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_cauchy_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_cauchy_Q, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_cdf_cauchy_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_cauchy_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_cauchy_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_cauchy_Qinv, 2, 2, 
  "(Real Q, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_cdf_cauchy_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_cauchy_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_cauchy_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_cauchy, 1, 1, 
  "(Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_ran_cauchy::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_cauchy(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_cauchy");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_cauchy_pdf, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Cauchy Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Cauchy-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Cauchy_distribution");
  void BDat_gsl_ran_cauchy_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_cauchy_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_cauchy_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_chisq_P, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_cdf_chisq_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_chisq_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_chisq_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_chisq_Pinv, 2, 2, 
  "(Real P, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_cdf_chisq_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_chisq_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_chisq_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_chisq_Q, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_cdf_chisq_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_chisq_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_chisq_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_chisq_Qinv, 2, 2, 
  "(Real Q, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_cdf_chisq_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_chisq_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_chisq_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_chisq, 1, 1, 
  "(Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_ran_chisq::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_chisq(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_chisq");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_chisq_pdf, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Chi-squared Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Chi_002dsquared-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Chi-square_distribution");
  void BDat_gsl_ran_chisq_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_chisq_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_chisq_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exponential_P, 2, 2, 
  "(Real x, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_cdf_exponential_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_exponential_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_exponential_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exponential_Pinv, 2, 2, 
  "(Real P, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_cdf_exponential_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_exponential_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_exponential_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exponential_Q, 2, 2, 
  "(Real x, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_cdf_exponential_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_exponential_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_exponential_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exponential_Qinv, 2, 2, 
  "(Real Q, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_cdf_exponential_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_exponential_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_exponential_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_exponential, 1, 1, 
  "(Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_ran_exponential::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_exponential(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_exponential");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_exponential_pdf, 2, 2, 
  "(Real x, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_distribution");
  void BDat_gsl_ran_exponential_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_exponential_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_exponential_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exppow_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Power Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Power-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_power_distribution");
  void BDat_gsl_cdf_exppow_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_exppow_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_exppow_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_exppow_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Power Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Power-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_power_distribution");
  void BDat_gsl_cdf_exppow_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_exppow_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_exppow_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_exppow, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Power Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Power-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_power_distribution");
  void BDat_gsl_ran_exppow::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_exppow(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_exppow");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_exppow_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Exponential Power Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Exponential-Power-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Exponential_power_distribution");
  void BDat_gsl_ran_exppow_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_exppow_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_exppow_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_fdist_P, 3, 3, 
  "(Real x, Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_cdf_fdist_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_fdist_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_fdist_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_fdist_Pinv, 3, 3, 
  "(Real P, Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_cdf_fdist_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_fdist_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_fdist_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_fdist_Q, 3, 3, 
  "(Real x, Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_cdf_fdist_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_fdist_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_fdist_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_fdist_Qinv, 3, 3, 
  "(Real Q, Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_cdf_fdist_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_fdist_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_fdist_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_fdist, 2, 2, 
  "(Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_ran_fdist::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_fdist(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_fdist");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_fdist_pdf, 3, 3, 
  "(Real x, Real nu1, Real nu2)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The F-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-F_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/F-distribution");
  void BDat_gsl_ran_fdist_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_fdist_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_fdist_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_flat_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_cdf_flat_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_flat_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_flat_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_flat_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_cdf_flat_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_flat_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_flat_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_flat_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_cdf_flat_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_flat_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_flat_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_flat_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_cdf_flat_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_flat_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_flat_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_flat, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_ran_flat::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_flat(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_flat");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_flat_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Flat (Uniform) Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Flat-_0028Uniform_0029-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Uniform_distribution");
  void BDat_gsl_ran_flat_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_flat_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_flat_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gamma_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_cdf_gamma_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gamma_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gamma_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gamma_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_cdf_gamma_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gamma_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gamma_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gamma_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_cdf_gamma_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gamma_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gamma_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gamma_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_cdf_gamma_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gamma_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gamma_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gamma, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_ran_gamma::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gamma(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gamma");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gamma_mt, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_ran_gamma_mt::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_ran_gamma_mt(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10600,"gsl_ran_gamma_mt");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gamma_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gamma Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gamma-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Gamma_distribution");
  void BDat_gsl_ran_gamma_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gamma_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gamma_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gaussian_P, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_gaussian_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gaussian_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gaussian_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gaussian_Pinv, 2, 2, 
  "(Real P, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_gaussian_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gaussian_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gaussian_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gaussian_Q, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_gaussian_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gaussian_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gaussian_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gaussian_Qinv, 2, 2, 
  "(Real Q, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_gaussian_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gaussian_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gaussian_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_ugaussian_P, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_ugaussian_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_ugaussian_P(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_ugaussian_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_ugaussian_Pinv, 1, 1, 
  "(Real P)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_ugaussian_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_ugaussian_Pinv(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_ugaussian_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_ugaussian_Q, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_ugaussian_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_ugaussian_Q(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_ugaussian_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_ugaussian_Qinv, 1, 1, 
  "(Real Q)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_cdf_ugaussian_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_ugaussian_Qinv(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_ugaussian_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian, 1, 1, 
  "(Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gaussian(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gaussian");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian_pdf, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gaussian_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gaussian_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian_ratio_method, 1, 1, 
  "(Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian_ratio_method::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gaussian_ratio_method(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gaussian_ratio_method");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian_ziggurat, 1, 1, 
  "(Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian_ziggurat::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_ran_gaussian_ziggurat(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10600,"gsl_ran_gaussian_ziggurat");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_ugaussian, 1, 1, 
  "(Real unused)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_ugaussian::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_ugaussian(getGslRng()); 
#else
  ErrorGslFVer(10400,"gsl_ran_ugaussian");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_ugaussian_pdf, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_ugaussian_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_ugaussian_pdf(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_ugaussian_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_ugaussian_ratio_method, 1, 1, 
  "(Real unused)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_ugaussian_ratio_method::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_ugaussian_ratio_method(getGslRng()); 
#else
  ErrorGslFVer(10400,"gsl_ran_ugaussian_ratio_method");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian_tail, 2, 2, 
  "(Real a, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian_tail::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gaussian_tail(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gaussian_tail");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gaussian_tail_pdf, 3, 3, 
  "(Real x, Real a, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_gaussian_tail_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gaussian_tail_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gaussian_tail_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_ugaussian_tail, 1, 1, 
  "(Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_ugaussian_tail::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_ugaussian_tail(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_ugaussian_tail");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_ugaussian_tail_pdf, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Gaussian Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Gaussian-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Normal_distribution");
  void BDat_gsl_ran_ugaussian_tail_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_ugaussian_tail_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_ugaussian_tail_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_geometric_P, 2, 2, 
  "(Real k, Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Geometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Geometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Geometric_distribution");
  void BDat_gsl_cdf_geometric_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_geometric_P((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_geometric_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_geometric_Q, 2, 2, 
  "(Real k, Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Geometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Geometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Geometric_distribution");
  void BDat_gsl_cdf_geometric_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_geometric_Q((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_geometric_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_geometric, 1, 1, 
  "(Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Geometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Geometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Geometric_distribution");
  void BDat_gsl_ran_geometric::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_geometric(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_geometric");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_geometric_pdf, 2, 2, 
  "(Real k, Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Geometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Geometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Geometric_distribution");
  void BDat_gsl_ran_geometric_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_geometric_pdf((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_geometric_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_hypergeometric_P, 4, 4, 
  "(Real k, Real n1, Real n2, Real t)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Hypergeometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Hypergeometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hypergeometric_distribution");
  void BDat_gsl_cdf_hypergeometric_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_hypergeometric_P((unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2)), (unsigned int)Real(Arg(3)), (unsigned int)Real(Arg(4))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_hypergeometric_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_hypergeometric_Q, 4, 4, 
  "(Real k, Real n1, Real n2, Real t)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Hypergeometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Hypergeometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hypergeometric_distribution");
  void BDat_gsl_cdf_hypergeometric_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_hypergeometric_Q((unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2)), (unsigned int)Real(Arg(3)), (unsigned int)Real(Arg(4))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_hypergeometric_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_hypergeometric, 3, 3, 
  "(Real n1, Real n2, Real t)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Hypergeometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Hypergeometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hypergeometric_distribution");
  void BDat_gsl_ran_hypergeometric::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_hypergeometric(getGslRng(), (unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_hypergeometric");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_hypergeometric_pdf, 4, 4, 
  "(Real k, Real n1, Real n2, Real t)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Hypergeometric Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Hypergeometric-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Hypergeometric_distribution");
  void BDat_gsl_ran_hypergeometric_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_hypergeometric_pdf((unsigned int)Real(Arg(1)), (unsigned int)Real(Arg(2)), (unsigned int)Real(Arg(3)), (unsigned int)Real(Arg(4))); 
#else
  ErrorGslFVer(10400,"gsl_ran_hypergeometric_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_landau, 1, 1, 
  "(Real unused)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Landau Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Landau-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Landau_distribution");
  void BDat_gsl_ran_landau::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_landau(getGslRng()); 
#else
  ErrorGslFVer(10400,"gsl_ran_landau");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_landau_pdf, 1, 1, 
  "(Real x)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Landau Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Landau-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Landau_distribution");
  void BDat_gsl_ran_landau_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_landau_pdf(Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_landau_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_laplace_P, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_cdf_laplace_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_laplace_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_laplace_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_laplace_Pinv, 2, 2, 
  "(Real P, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_cdf_laplace_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_laplace_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_laplace_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_laplace_Q, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_cdf_laplace_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_laplace_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_laplace_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_laplace_Qinv, 2, 2, 
  "(Real Q, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_cdf_laplace_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_laplace_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_laplace_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_laplace, 1, 1, 
  "(Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_ran_laplace::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_laplace(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_laplace");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_laplace_pdf, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Laplace Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Laplace-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Laplace_distribution");
  void BDat_gsl_ran_laplace_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_laplace_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_laplace_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_levy, 2, 2, 
  "(Real c, Real alpha)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Levy alpha-Stable Distributions]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Levy-alpha_002dStable-Distributions.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Stable_distributions");
  void BDat_gsl_ran_levy::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_levy(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_levy");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_levy_skew, 3, 3, 
  "(Real c, Real alpha, Real beta)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Levy skew alpha-Stable Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Levy-skew-alpha_002dStable-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Stable_distributions");
  void BDat_gsl_ran_levy_skew::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_levy_skew(getGslRng(), Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_levy_skew");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_logarithmic, 1, 1, 
  "(Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logarithmic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logarithmic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logarithmic_distribution");
  void BDat_gsl_ran_logarithmic::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_logarithmic(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_logarithmic");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_logarithmic_pdf, 2, 2, 
  "(Real k, Real p)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logarithmic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logarithmic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logarithmic_distribution");
  void BDat_gsl_ran_logarithmic_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_logarithmic_pdf((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_logarithmic_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_logistic_P, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_cdf_logistic_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_logistic_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_logistic_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_logistic_Pinv, 2, 2, 
  "(Real P, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_cdf_logistic_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_logistic_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_logistic_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_logistic_Q, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_cdf_logistic_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_logistic_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_logistic_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_logistic_Qinv, 2, 2, 
  "(Real Q, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_cdf_logistic_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_logistic_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_logistic_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_logistic, 1, 1, 
  "(Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_ran_logistic::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_logistic(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_logistic");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_logistic_pdf, 2, 2, 
  "(Real x, Real a)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Logistic Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Logistic-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Logistic_distribution");
  void BDat_gsl_ran_logistic_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_logistic_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_logistic_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_lognormal_P, 3, 3, 
  "(Real x, Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_cdf_lognormal_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_lognormal_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_lognormal_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_lognormal_Pinv, 3, 3, 
  "(Real P, Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_cdf_lognormal_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_lognormal_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_lognormal_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_lognormal_Q, 3, 3, 
  "(Real x, Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_cdf_lognormal_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_lognormal_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_lognormal_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_lognormal_Qinv, 3, 3, 
  "(Real Q, Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_cdf_lognormal_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_lognormal_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_lognormal_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_lognormal, 2, 2, 
  "(Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_ran_lognormal::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_lognormal(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_lognormal");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_lognormal_pdf, 3, 3, 
  "(Real x, Real zeta, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Lognormal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Lognormal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Log-normal_distribution");
  void BDat_gsl_ran_lognormal_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_lognormal_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_lognormal_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_negative_binomial_P, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Negative Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Negative-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_cdf_negative_binomial_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_negative_binomial_P((unsigned int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_negative_binomial_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_negative_binomial_Q, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Negative Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Negative-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_cdf_negative_binomial_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_negative_binomial_Q((unsigned int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_negative_binomial_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_negative_binomial, 2, 2, 
  "(Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Negative Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Negative-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_ran_negative_binomial::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_negative_binomial(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_negative_binomial");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_negative_binomial_pdf, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Negative Binomial Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Negative-Binomial-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_ran_negative_binomial_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_negative_binomial_pdf((unsigned int)Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_negative_binomial_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pareto_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_cdf_pareto_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_pareto_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_pareto_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pareto_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_cdf_pareto_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_pareto_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_pareto_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pareto_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_cdf_pareto_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_pareto_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_pareto_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pareto_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_cdf_pareto_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_pareto_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_pareto_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_pareto, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_ran_pareto::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_pareto(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_pareto");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_pareto_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pareto Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pareto-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Pareto_distribution");
  void BDat_gsl_ran_pareto_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_pareto_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_pareto_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pascal_P, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pascal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pascal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_cdf_pascal_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_pascal_P((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_pascal_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_pascal_Q, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pascal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pascal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_cdf_pascal_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_pascal_Q((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_pascal_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_pascal, 2, 2, 
  "(Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pascal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pascal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_ran_pascal::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_pascal(getGslRng(), Real(Arg(1)), (unsigned int)Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_pascal");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_pascal_pdf, 3, 3, 
  "(Real k, Real p, Real n)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Pascal Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Pascal-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Negative_binomial_distribution");
  void BDat_gsl_ran_pascal_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_pascal_pdf((unsigned int)Real(Arg(1)), Real(Arg(2)), (unsigned int)Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_pascal_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_poisson_P, 2, 2, 
  "(Real k, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Poisson Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Poisson-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Poisson_distribution");
  void BDat_gsl_cdf_poisson_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_poisson_P((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_poisson_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_poisson_Q, 2, 2, 
  "(Real k, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Poisson Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Poisson-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Poisson_distribution");
  void BDat_gsl_cdf_poisson_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10600)
  contens_ = gsl_cdf_poisson_Q((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10600,"gsl_cdf_poisson_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_poisson, 1, 1, 
  "(Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Poisson Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Poisson-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Poisson_distribution");
  void BDat_gsl_ran_poisson::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_poisson(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_poisson");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_poisson_pdf, 2, 2, 
  "(Real k, Real mu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Poisson Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Poisson-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Poisson_distribution");
  void BDat_gsl_ran_poisson_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_poisson_pdf((unsigned int)Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_poisson_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_rayleigh_P, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_cdf_rayleigh_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_rayleigh_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_rayleigh_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_rayleigh_Pinv, 2, 2, 
  "(Real P, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_cdf_rayleigh_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_rayleigh_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_rayleigh_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_rayleigh_Q, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_cdf_rayleigh_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_rayleigh_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_rayleigh_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_rayleigh_Qinv, 2, 2, 
  "(Real Q, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_cdf_rayleigh_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_rayleigh_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_rayleigh_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_rayleigh, 1, 1, 
  "(Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_ran_rayleigh::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_rayleigh(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_rayleigh");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_rayleigh_pdf, 2, 2, 
  "(Real x, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_ran_rayleigh_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_rayleigh_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_rayleigh_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_rayleigh_tail, 2, 2, 
  "(Real a, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_ran_rayleigh_tail::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_rayleigh_tail(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_rayleigh_tail");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_rayleigh_tail_pdf, 3, 3, 
  "(Real x, Real a, Real sigma)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Rayleigh Tail Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Rayleigh-Tail-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Rayleigh_distribution");
  void BDat_gsl_ran_rayleigh_tail_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_rayleigh_tail_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_rayleigh_tail_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel1_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel1_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel1_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel1_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel1_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel1_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel1_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel1_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel1_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel1_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel1_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel1_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel1_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel1_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel1_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel1_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gumbel1, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_ran_gumbel1::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gumbel1(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gumbel1");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gumbel1_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-1 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d1-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-1_Gumbel_distribution");
  void BDat_gsl_ran_gumbel1_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gumbel1_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gumbel1_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel2_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel2_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel2_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel2_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel2_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel2_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel2_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel2_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel2_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel2_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel2_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel2_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_gumbel2_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_cdf_gumbel2_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_gumbel2_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_gumbel2_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gumbel2, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_ran_gumbel2::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gumbel2(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gumbel2");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_gumbel2_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Type-2 Gumbel Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Type_002d2-Gumbel-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Type-2_Gumbel_distribution");
  void BDat_gsl_ran_gumbel2_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_gumbel2_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_gumbel2_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_weibull_P, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_cdf_weibull_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_weibull_P(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_weibull_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_weibull_Pinv, 3, 3, 
  "(Real P, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_cdf_weibull_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_weibull_Pinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_weibull_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_weibull_Q, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_cdf_weibull_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_weibull_Q(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_weibull_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_weibull_Qinv, 3, 3, 
  "(Real Q, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_cdf_weibull_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_weibull_Qinv(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_weibull_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_weibull, 2, 2, 
  "(Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_ran_weibull::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_weibull(getGslRng(), Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_weibull");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_weibull_pdf, 3, 3, 
  "(Real x, Real a, Real b)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The Weibull Distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-Weibull-Distribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Weibull_distribution");
  void BDat_gsl_ran_weibull_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_weibull_pdf(Real(Arg(1)), Real(Arg(2)), Real(Arg(3))); 
#else
  ErrorGslFVer(10400,"gsl_ran_weibull_pdf");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_tdist_P, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_cdf_tdist_P::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_tdist_P(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_tdist_P");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_tdist_Pinv, 2, 2, 
  "(Real P, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_cdf_tdist_Pinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_tdist_Pinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_tdist_Pinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_tdist_Q, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_cdf_tdist_Q::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_tdist_Q(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_tdist_Q");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_cdf_tdist_Qinv, 2, 2, 
  "(Real Q, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_cdf_tdist_Qinv::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_cdf_tdist_Qinv(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_cdf_tdist_Qinv");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_tdist, 1, 1, 
  "(Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_ran_tdist::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_tdist(getGslRng(), Real(Arg(1))); 
#else
  ErrorGslFVer(10400,"gsl_ran_tdist");
#endif
}

//--------------------------------------------------------------------
  DecDefIntOprDatGsl(1, gsl_ran_tdist_pdf, 2, 2, 
  "(Real x, Real nu)",
  "[GSL: GNU Scientific Library] [C2_ProbDistFun] [The t-distribution]\n"
  "GSL chapter: Random Number Distributions \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/Random-Number-Distributions.html\n"
  "You can see a description of correspondent C function at GSL manual page: \n"
  "  http://www.gnu.org/software/gsl/manual/html_node/The-t_002ddistribution.html\n"
  "Mathematical concepts about this function are exposed at : \n"
  "  http://en.wikipedia.org/wiki/Student's_t-distribution");
  void BDat_gsl_ran_tdist_pdf::CalcContens()
//--------------------------------------------------------------------
{ 
  contens_.PutKnown(false);
  for(int n=1; n<=NumArgs(); n++) { if(Dat(Arg(n)).IsUnknown()) { return; } }
#if (GSL_VERSION_NUM >= 10400)
  contens_ = gsl_ran_tdist_pdf(Real(Arg(1)), Real(Arg(2))); 
#else
  ErrorGslFVer(10400,"gsl_ran_tdist_pdf");
#endif
}
