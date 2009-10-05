/* bcmpgra.cpp: Complex Grammar's arithmetic functions
                GNU/TOL Language

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

//static const char* bayesVersionId =

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcmpgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bout.h>

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("cmpgra.cpp");
template<>
BGrammar* BGraContensBase<BCmp>::ownGrammar_ = NIL;


DefineContensCommonOperators(BCmp, "Complex");
//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BCmp>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BCmp>::InitInstances()
{
    BParamCmp* i_ = new BParamCmp
	(
	    "i",
	    Ci,
	    I2("Imaginary unit, it's to say, square root of -1.",
	       "La unidad imaginaria, o sea, raíz cuadrada de -1.")
	    );
    BSystemCmp* unknown_ = new BSystemCmp("Unknown", BCmp::Unknown(),
					  I2("Unknown value",
					     "Valor desconocido"));
    OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
/*! Casting from real.
 */
class BCmpFromReal : public BCmpTemporary
{
public:
    BCmpFromReal(BList* arg) : BCmpTemporary(arg) { }
    ~BCmpFromReal() { }
	void CalcContens() { contens_ = BComplex::RC(Dat(Arg(1))); }
  RedeclareClassNewDelete(BCmpFromReal);
};

//--------------------------------------------------------------------
/*! Returns a valid constant complex for name.
 */
template<>
BSyntaxObject* BGraContensBase<BCmp>::FindConstant(const BText& name)
{
    BSyntaxObject* result = NIL;
    BUserCmp* uCmp = (BUserCmp*)(GraReal()->FindConstant(name));
    if(uCmp) { result = (BSyntaxObject*) new BCmpFromReal(NCons(uCmp)); }
    return(result);
}

//--------------------------------------------------------------------
/*! Returns a valid complex for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BCmp>::Casting(BSyntaxObject* obj)
{
    if(!obj)			       { return(NIL); }
    if(obj->Grammar()==OwnGrammar())   { return(obj); }
    if(obj->Grammar()->Name()=="Real") {return(new BCmpFromReal(NCons(obj)));}
    return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContens<BCmp>::New(FILE* fil)
{
    return 0;
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpIdentity);
DefIntOpr(1, BCmpIdentity, " + ", 1, 1,
	  "(Complex x)",
	  I2("Returns the same complex number.",
	     "Devuelve el mismo número complejo."),
	  BOperClassify::ComplexArythmetic_);
void BCmpIdentity::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpMinus);
DefIntOpr(1, BCmpMinus, " - ", 1, 1,
	  "(Complex x)",
	  I2("Returns the complex number width the oposite sign.",
	     "Devuelve el argumento con el signo contrario."),
	  BOperClassify::ComplexArythmetic_);
void BCmpMinus::CalcContens()
//--------------------------------------------------------------------
{ contens_ = -Cmp(Arg(1)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpConjugated);
DefIntOpr(1, BCmpConjugated, "~", 1, 1,
	  "(Complex x)",
	  I2("Returns the conjugated of a complex number.",
	     "Devuelve el conjugado de un número complejo."),
	  BOperClassify::ComplexArythmetic_);
void BCmpConjugated::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Conj( Cmp(Arg(1)) ); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpSqrt);
DefIntOpr(1, BCmpSqrt, "SqRt", 1, 1,
	  "(Complex x)",
	  I2("Returns the square root of x.",
	     "Devuelve la raíz cuadrada de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpSqrt::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Sqrt(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpLog);
DefIntOpr(1, BCmpLog, "Log", 1, 1,
	  "(Complex x)",
	  I2("Returns the natural logarithm of x.",
	     "Devuelve el logaritmo natural de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpLog::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Log(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpLogBase);
DefIntOpr(1, BCmpLogBase, "LogBase", 2, 2,
	  "(Complex x, Complex b)",
	  I2("Returns the b-based logarithm of x.",
	     "Devuelve el logaritmo en base b de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpLogBase::CalcContens()
//--------------------------------------------------------------------
{  contens_ = Log(Cmp(Arg(1)))/Log(Cmp(Arg(2)));
}

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpExp);
DefIntOpr(1, BCmpExp, "Exp", 1, 1,
	  "(Complex x)",
	  I2("Returns the exponential of x.",
	     "Devuelve la exponencial de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpExp::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Exp(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpSin);
DefIntOpr(1, BCmpSin, "Sin", 1, 1,
	  "(Complex x)",
	  I2("Returns the trigonometric sine of x.",
	     "Devuelve el seno trigonométrico de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpSin::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Sin(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpCos);
DefIntOpr(1, BCmpCos, "Cos", 1, 1,
	  "(Complex x)",
	  I2("Returns the trigonometric cosine of x.",
	     "Devuelve el coseno trigonométrico de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpCos::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cos(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpTan);
DefIntOpr(1, BCmpTan, "Tan", 1, 1,
	  "(Complex x)",
	  I2("Returns the trigonometric tangent of x.",
	     "Devuelve la tangente trigonométrica de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpTan::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Tan(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpASin);
DefIntOpr(1, BCmpASin, "ASin", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich trigonometric sine is x.",
	     "Devuelve el número cuyo seno trigonométrico es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpASin::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ASin(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpACos);
DefIntOpr(1, BCmpACos, "ACos", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich trigonometric cosine is x.",
	     "Devuelve el número cuyo coseno trigonométrico es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpACos::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ACos(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpATan);
DefIntOpr(1, BCmpATan, "ATan", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich trigonometric tangent is x.",
	     "Devuelve el número cuya tangente trigonométrica es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpATan::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ATan(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpASinH);
DefIntOpr(1, BCmpASinH, "ASinH", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich hyperbolic sine is x.",
	     "Devuelve el número cuyo seno hiperbólico es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpASinH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ASinH(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpACosH);
DefIntOpr(1, BCmpACosH, "ACosH", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich hyperbolic cosine is x.",
	     "Devuelve el número cuyo coseno hiperbólico es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpACosH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ACosH(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpATanH);
DefIntOpr(1, BCmpATanH, "ATanH", 1, 1,
	  "(Complex x)",
	  I2("Returns the number wich hyperbolic tangent is x.",
	     "Devuelve el número cuya tangente hiperbólica es x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpATanH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ATanH(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpSinH);
DefIntOpr(1, BCmpSinH, "SinH", 1, 1,
	  "(Complex x)",
	  I2("Returns the hyperbolic sine of x.",
	     "Devuelve el seno hiperbólico de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpSinH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = SinH(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpCosH);
DefIntOpr(1, BCmpCosH, "CosH", 1, 1,
	  "(Complex x)",
	  I2("Returns the hyperbolic cosine of x.",
	     "Devuelve el coseno hiperbólico de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpCosH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = CosH(Cmp(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpTanH);
DefIntOpr(1, BCmpTanH, "TanH", 1, 1,
	  "(Complex x)",
	  I2("Returns the hyperbolic tangent of x.",
	     "Devuelve la tangente hiperbólica de x."),
	  BOperClassify::ComplexArythmetic_);
void BCmpTanH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = TanH(Cmp(Arg(1))); }

/*
//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpLogGamma);
  DefIntOpr(1, BCmpLogGamma, "LogGamma", 1, 1,
  "(Complex p)",
  I2("Returns the logarythm of the Euler's Gamma function of p>0.",
     "Devuelve el logaritmo de la función Gamma de Euler de p>0."),
  BOperClassify::ComplexArythmetic_);
  void BCmpLogGamma::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogGamma(Cmp(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpLogBeta);
  DefIntOpr(1, BCmpLogBeta, "LogBeta", 2, 2,
  "(Complex p, Complex q)",
  I2("Returns the logarythm of the Euler's Beta function of p,q>0.",
     "Devuelve el logaritmo de la función Beta de Euler de p,q>0."),
  BOperClassify::ComplexArythmetic_);
  void BCmpLogBeta::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogBeta(Cmp(Arg(1)),Cmp(Arg(2))); }


//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpGamma);
  DefIntOpr(1, BCmpGamma, "Gamma", 1, 1,
  "(Complex p)",
  I2("Returns the Euler's Gamma function of p>0.",
     "Devuelve la función Gamma de Euler de p>0."),
  BOperClassify::ComplexArythmetic_);
  void BCmpGamma::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Gamma(Cmp(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpBeta);
  DefIntOpr(1, BCmpBeta, "Beta", 2, 2,
  "(Complex p, Complex q)",
  I2("Returns the Euler's Beta function of p,q>0.",
     "Devuelve la función Beta de Euler de p,q>0."),
  BOperClassify::ComplexArythmetic_);
  void BCmpBeta::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Beta(Cmp(Arg(1)),Cmp(Arg(2))); }

*/

//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpSum2);
DefIntOpr(1, BCmpSum2, "+", 2, 2,
	  "x1 + x2 {Complex x1, Complex x2}",
	  I2("Returns the summe of both complex numbers.",
	     "Devuelve la suma de ambos números complejos."),
	  BOperClassify::ComplexArythmetic_);
void BCmpSum2::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)) + Cmp(Arg(2)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpDifference);
DefIntOpr(1, BCmpDifference, "-", 2, 2,
	  "x1 - x2 {Complex x1, Complex x2}",
	  I2("Returns the difference between both complex numbers.",
	     "Devuelve la diferencia entre ambos números complejos."),
	  BOperClassify::ComplexArythmetic_);
void BCmpDifference::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)) - Cmp(Arg(2)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpProduct2);
DefIntOpr(1, BCmpProduct2, "*", 2, 2,
	  "x1 * x2 {Complex x1, Complex x2}",
	  I2("Returns the product of both complex numbers.",
	     "Devuelve el producto de ambos números complejos."),
	  BOperClassify::ComplexArythmetic_);
void BCmpProduct2::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)) * Cmp(Arg(2)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpQuotient);
DefIntOpr(1, BCmpQuotient, "/", 2, 2,
	  "x1 / x2 {Complex x1, Complex x2}",
	  I2("Returns the quotient of both complex numbers.",
	     "Devuelve el cociente entre ambos números complejos."),
	  BOperClassify::ComplexArythmetic_);
void BCmpQuotient::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)) / Cmp(Arg(2)); }

//--------------------------------------------------------------------
DeclareContensClass(BCmp, BCmpTemporary, BCmpPow);
DefIntOpr(2, BCmpPow, "^", 2, 2,
	  "x1 ^ x2 {Complex x1, Complex x2}",
	  I2("Returns the power of x1 increased to x2.",
	     "Devuelve la potencia de x1 elevado a x2.")+
	  I2("Also it can be used the operator ** or the function Pow.",
	     "También se puede utilizar el operador ** o la función Pow."),
	  BOperClassify::ComplexArythmetic_);
void BCmpPow::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cmp(Arg(1)) ^ Cmp(Arg(2)); }

/*
//--------------------------------------------------------------------
  DeclareContensClass(BCmp, BCmpTemporary, BCmpEvalPol);
  DefExtOpr(1, BCmpEvalPol, "EvalPol", 2, 2, "Polyn Complex",
  "(Polyn p, Complex x)",
  I2(".",
     "Evalúa la función polinomial correspondiente en el valor complejo dado."),
     BOperClassify::RetardPolynomial_);
  void BCmpEvalPol::CalcContens()
//--------------------------------------------------------------------
{
  BPol rPol = Pol(Arg(1));
  BPolyn<BComplex> pol;
  pol.ReallocBuffer(rPol.Size());

  BComplex x = Cmp(Arg(2));
  contens_ = pol.Eval(x);
}
*/
