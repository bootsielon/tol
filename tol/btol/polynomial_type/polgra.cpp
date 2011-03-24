/* polgra.cpp: Polynomials Grammar's functions.
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

#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bout.h>
#include <tol/tol_bunitroot.h>


//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("polgra.cpp");
template<>
BGrammar* BGraContensBase<BPol>::ownGrammar_ = NIL;
#if 0
BPol	  BPol::unknown_;  // it's defined in unitroot.cpp
#endif

DefineContensCommonOperators(BPol, "Polyn");

//--------------------------------------------------------------------
TOL_API BPol EvalPolyn(const BText& expr, const BPol& defVal) 
//--------------------------------------------------------------------
{
  int stackPos = BGrammar::StackSize();
  BSyntaxObject* obj = GraPolyn()->EvaluateExpr(expr);
  BPol result = defVal;
  BGrammar::IncLevel();
  if(obj && (obj->Grammar()==GraPolyn())) 
  { 
    result = Pol(obj);    
  }
  BGrammar::DestroyStackUntil(stackPos, obj);    
  DESTROY(obj);
  BGrammar::DecLevel();
  return(result);
}


//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BPol>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}


template<>
BPol& BPol::Unknown()
{
    BMonome<BDat> unkmon(BDat::Unknown(), 0);
    static BPolyn<BDat> unknown_(unkmon);
    return(unknown_); 
}

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BPol>::InitInstances()
{
    BTraceInit("BGraContens<BPol>::InitInstances");
    BSystemPol* b_= new BSystemPol
	(
	  "B",
	  BMonome<BDat>(1.0,	1),
	  I2("Backward operator.", "Operador de retardo.")
        );
    BSystemPol* f_= new BSystemPol
	(
	  "F",
	  BMonome<BDat>(1.0, -1),
	  I2("Forward operator.",  "Operador de adelanto.")
	);
    BSystemPol* d_= new BSystemPol
	(
	  "Delta",
	  1-BPol::X(),
          I2("Regular difference operator.", "Operador de diferencia regular.")
	);
    BSystemPol* unknown_= new BSystemPol
	(
	  "UnknownPolyn",
	  BMonome<BDat>(BDat::Unknown(), 0),
	  I2("Unknown Polyn.", "Polinomio desconocido.")
	);
    OwnGrammar()->PutDefect(unknown_);
}


//--------------------------------------------------------------------
class BPolFromReal : public BPolTemporary

/*! Casting from real.
 */
//--------------------------------------------------------------------
{
public:
    BPolFromReal(BList* arg) : BPolTemporary(arg) { }
    ~BPolFromReal() { }
    void CalcContens() { contens_ = Dat(Arg(1))* BPol::One(); }
};


//--------------------------------------------------------------------
/*! Returns a valid constant BPol for name.
 */
template<>
BSyntaxObject* BGraContensBase<BPol>::FindConstant(const BText& name)
{
    BSyntaxObject* pol = NIL;
    BUserDat* uDat = (BUserDat*)(GraReal()->FindConstant(name));
    if(uDat) { pol = (BSyntaxObject*) new BPolFromReal(NCons(uDat)); }
    return(pol);
}


//--------------------------------------------------------------------
/*! Returns a valid polynomial for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BPol>::Casting(BSyntaxObject* obj)
{
    if(!obj)			       { return(NIL); }
    if(obj->Grammar()==OwnGrammar())   { return(obj); }
    if(obj->Grammar()->Name()=="Real") {return(new BPolFromReal(NCons(obj)));}
    return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BPol>::New(FILE* fil)
{
    return 0;
}


//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolIdentity);
DefIntOpr(1, BPolIdentity, " + ", 1, 1,
  "(Polyn p)",
  I2("Returns the same polynomial.",
     "Devuelve el mismo polinomio."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolIdentity::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolMinus);
DefIntOpr(1, BPolMinus, " - ",1, 1,
  "(Polyn p)",
  I2("Returns the polynomial with the opposite sign.",
     "Devuelve el polinomio con el signo contrario."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolMinus::CalcContens()
//--------------------------------------------------------------------
{ contens_ = -Pol(Arg(1)); }


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPolMonomes);
DefExtOpr(1, BSetPolMonomes, "Monomes", 1, 1, "Polyn",
  "(Polyn p)",
  I2("Returns the set of all not null monomes of p.",
     "Devuelve el conjunto de todos  los monomios no nulos de p."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BSetPolMonomes::CalcContens()
//--------------------------------------------------------------------
{
    BPol p = Pol(Arg(1));
    BList* lst = NIL;
    for(BInt n=p.Size()-1; n>=0; n--)
    {
	if(p[n].Coef()!=0)
	{
	    BPol mon(p[n]);
      lst = Cons(BContensPol::New("",mon,mon.Name()), lst);
	}
    }
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolSum);
DefIntOpr(1, BPolSum, "+", 2, 2,
  "p1 + p2 {Polyn p1, Polyn p2}",
  I2("Returns the summe of both polinomials.",
     "Devuelve la suma de ambos polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolSum::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) + Pol(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolDifference);
DefIntOpr(1, BPolDifference, "-", 2, 2,
  "p1 - p2 {Polyn p1, Polyn p2}",
  I2("Returns the difference between both polynomials.",
     "Devuelve la diferencia entre ambos polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolDifference::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) - Pol(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolProduct);
DefIntOpr(1, BPolProduct, "*", 2, 2,
  "p1 * p2 {Polyn p1, Polyn p2}",
  I2("Returns the product of both polynomials.",
     "Devuelve el producto de ambos polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolProduct::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) * Pol(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolChangeBDegree);
DefExtOpr(1, BPolChangeBDegree, "ChangeBDegree", 2, 2, "Polyn Real",
  "(Polyn p, Real x)",
  I2("Change the B by B^x.",
     "Cambia la B por B^x."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolChangeBDegree::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Pol(Arg(1));
    BDat x = Dat(Arg(2));
    for(BInt i=0; i<contens_.Size(); i++)
    {
	BReal deg = contens_[i].Degree();
	contens_[i].PutDegree((int) Round(deg*x).Value());
    }
    contens_.Aggregate();
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolChangeB);
DefIntOpr(1, BPolChangeB, "ChangeB", 2, 2,
  "(Polyn p, Polyn x)",
  I2("Change the B by the x polinomial.",
     "Cambia la B por el polinomio x."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolChangeB::CalcContens()
//--------------------------------------------------------------------
{
    BPol p = Pol(Arg(1));
    BPol x = Pol(Arg(2));
    BText oldFormat = BDat::RealFormat();
    BDat::PutRealFormat("%.20lg");
    BText pTxt = p.Name();
    BText xTxt = BText("(")+x.Name()+")";
    BDat::PutRealFormat(oldFormat);
    pTxt.Replace('B','@');
    pTxt.Replace("@",xTxt);
    contens_ = EvalPolyn(pTxt,BPol::Zero());
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolChangeBF);
DefIntOpr(1, BPolChangeBF, "ChangeBF", 1, 1,
  "(Polyn p)",
  I2("Change the B by the F and conversely.",
     "Cambia la B por la F y viceversa."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolChangeBF::CalcContens()
//--------------------------------------------------------------------
{
    contens_.ChangeBF(Pol(Arg(1)));
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolQuotient);
DefExtOpr(1, BPolQuotient, "/", 2, 2, "Polyn Real",
  "p / x {Polyn p, Real x}",
  I2("Returns the quotient of a polynomial and a number.",
     "Devuelve el cociente entre un polinomio y un número."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolQuotient::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) / Dat(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolExpansion);
DefExtOpr(1, BPolExpansion, "Expand", 2, 2, "Ratio Real",
  "(Ratio r, Real x)",
  I2("Returns the finite expansion of degree n of a polynomials reason. "
     "Given\n\n"
     "P(B) = 1+p1*B+p2*B^2+...+pr*B^r;\n\n"
     "Q(B) = 1+q1*B+q2*B^2+...+qs*B^s;\n\n"
     "It is defined the finite expansion of degree n of P(B)/Q(B) as "
     "the polynomial\n\n"
     "C(B) = 1+c1*B+c2*B^2+...+cn*B^n;\n\n"
     "such that, P(B)- Q(B)*C(B) is zero or multiple of B^(n+1), "
     "or rather, that the terms of Q(B)*C(B) coincide with those "
     "of P(B) until the degree n at least. The calculation method "
     "is to solve the system\n\n"
     "p1 = q1 + c1;\n"
     "p2 = q2 + q1*c1 + c2;\n"
     "...\n"
     "pn = qn + q(n-1)*c1 + ... + q1*c(n-1) + cn;\n",
     "Devuelve la expansión finita de grado n de una razón de polinomios. "
     "Dados \n\n"
     "P(B) = 1+p1*B+p2*B^2+...+pr*B^r;\n"
     "Q(B) = 1+q1*B+q2*B^2+...+qs*B^s;\n\n"
     "Se define la expansión finita de grado n de P(B)/Q(B) como el "
     "polinomio\n\n"
     "C(B) = 1+c1*B+c2*B^2+...+cn*B^n;\n\n"
     "tal que, P(B)-Q(B)*C(B) es cero o múltiplo de B^(n+1), o sea, "
     "que los términos de Q(B)*C(B) coinciden con los de P(B) hasta "
     "el grado n por lo menos. El método de cálculo es resolver el "
     "sistema\n\n"
     "p1 = q1 + c1;\n"
     "p2 = q2 + q1*c1 + c2;\n"
     "...\n"
     "pn = qn + q(n-1)*c1 + ... + q1*c(n-1) + cn;\n"),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolExpansion::CalcContens()
//--------------------------------------------------------------------
{
    BRat& r = Rat(Arg(1));
    BInt d = (BInt)Real(Arg(2));
    contens_ = r%d;
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolRationalQuotient);
DefExtOpr(1, BPolRationalQuotient, "Quotient", 1, 1, "Ratio",
  "(Ratio r)",
  I2("Returns the quotient of a polynomials reason if it is exact. If the "
     "quotient of a polynomials reason is exact, coincides with the finite "
     "expansion of equal degree to the difference between the degrees of "
     "the numerator and the numerator. Making use of this is obtained a "
     "very rapid method for the synthetic polynomials division when this is "
     "exact.",
     "Devuelve el cociente de una razón de polinomios si es exacto. "
     "Si el cociente de una razón de polinomios es exacto, coincide "
     "con la expansión finita de grado igual a la diferencia entre los "
     "grados del numerador y el numerador. Haciendo uso de esto se obtiene "
     "un método muy rápido para la división sintética de polinomios cuando "
     "ésta es exacta."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolRationalQuotient::CalcContens()
//--------------------------------------------------------------------
{
    BRat& r = Rat(Arg(1));
    contens_ = r.ExactQuotient();
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolPow);
DefExtOpr(1, BPolPow, "^", 2, 2, "Polyn Real",
  "p^n {Polyn p, Real n}",
  I2("Returns the power of a polynomial increased to an entire number.",
     "Devuelve la potencia de un polinomio elevado a un número entero.")+
  I2("Also it can be used the operator ** or the function Pow.",
     "También se puede utilizar el operador ** o la función Pow."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolPow::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) ^ (BInt)Real(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolNumerator);
DefExtOpr(1, BPolNumerator, "Numerator", 1, 1, "Ratio",
  "(Ratio r)",
  I2("Returns the numerator of a polynomial reason.",
     "Devuelve el numerador de una razón de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolNumerator::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Rat(Arg(1)).Numerator(); }

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolDenominator);
DefExtOpr(1, BPolDenominator, "Denominator", 1, 1, "Ratio",
  "(Ratio r)",
  I2("Returns the denominator of a polynomial reason.",
     "Devuelve el denominador de una razón de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolDenominator::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Rat(Arg(1)).Denominator(); }

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolMatPol);
DefExtOpr(1, BPolMatPol, "MatPol", 1, 1, "Matrix",
  "(Matrix m)",
  I2(".",
     "Devuelve el polinomio cuyos coeficientes son los términos de una "
     "matriz fila. El primer término de la matriz es el coeficiente de "
     "grado 0 del polinomio, el segundo el de grado 1 y así sucesivamente."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BPolMatPol::CalcContens()
//--------------------------------------------------------------------
{
    BMat& M = Mat(Arg(1));
    BInt	c = M.Columns();
    BInt	i;
    contens_.ReallocBuffer(c-1);
    contens_.ReallocBuffer(0);
    contens_ = BPol::Zero();
    for(i=0; i<c; i++)
    {
	if(M(0,i)!=0) { contens_ += BMonome<BDat>(M(0,i),i); }
    }
    contens_.Aggregate();
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolRandStationary);
DefExtOpr(1, BPolRandStationary, "RandStationary", 1, 2, "Real Real",
  "(Real d [, Real p = 1])",
  I2("Returns a random stationary polinomial as \n",
     "Devuelve un polinomio estacionario aleatorio de la forma \n") +
     " 1 - C1 * B^p - C2 * B^(2*p) - ... - Cd * B^(d*p).",
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolRandStationary::CalcContens()
//--------------------------------------------------------------------
{
  BInt degree = (BInt)Real(Arg(1));
  BInt period = 1;
  BInt n2 = degree/2;
  BInt n1 = degree-2*n2;
  BPol p1 = BPol::One();
  BPol p2 = BPol::One();
  BUniformDist u(-1+2*DEpsilon(),1-2*DEpsilon());
  BDat a, b;
    
  if(Arg(2)) { period = (BInt)Real(Arg(2)); }
  BPol BP = BPol::X() ^ period;
  if(n1)
  {
    p1 = BPol::One()-u.Random()*BP;
  };
  for(BInt k=0; k<n2; k++)
  {
    do
    {
      a = u.Random();
      b = u.Random()*2;
    } while ( (a>=1-b) || (a>=1+b) );
    p2 *= (BPol::One()-b*BP-a*(BP^2));
  }
  contens_ = p1 * p2;
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolExtractPeriod);
DefExtOpr(1, BPolExtractPeriod, "ExtractPeriod", 2, 2, "Polyn Real",
  I2("(Polyn pol, Real period)",
     "(Polyn pol, Real periodo)"),
  I2("",
     "Devuelve el polinomio formado por los monomios de pol de grado "
     "múltiplo de un periodo dado."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolExtractPeriod::CalcContens()
//--------------------------------------------------------------------
{
    BPol pol = Pol(Arg(1));
    BInt s   = (BInt)Real(Arg(2));
    BInt n   = (pol.Degree() - pol[0].Degree()) / s;
    contens_.ReallocBuffer(1+n);
//Std(BText("\ns=")+s+" n="+n);
    BInt k, j;
    for(k=j=0; k<pol.Size(); k++)
    {
	BInt d  = pol[k].Degree();
//  Std(BText("\nMonome ")+k+" : "+d+"%" + s + "=" +(d%s));
	if(d%s==0)
	{
//    Std(BText("\t -> Adding ")+j+ " : " +(d/s));
	    contens_[j].PutDegree(d/s);
	    contens_[j].PutCoef  (pol[k].Coef());
	    j++;
	}
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolSub);
DefExtOpr(1, BPolSub, "Sub", 3, 3, "Polyn Real Real",
  I2("(Polyn pol, Real minDegree, Real maxDegree)",
     "(Polyn pol, Real minGrado, Real maxGrado)"),
  I2("",
     "Devuelve el polinomio formado por los monomios de pol de grados "
     "comprendidos entre los límites dados."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolSub::CalcContens()
//--------------------------------------------------------------------
{
    BPol pol = Pol(Arg(1));
    BInt m   = (BInt)Real(Arg(2));
    BInt M   = (BInt)Real(Arg(3));
    contens_.ReallocBuffer(M - m + 1);
//Std(BText("\ns=")+s+" n="+n);
    BInt k, j;
    for(k=j=0; k<pol.Size(); k++)
    {
	BInt d  = pol[k].Degree();
//  Std(BText("\nMonome ")+k+" : "+d+"%" + s + "=" +(d%s));
	if((d>=m) && (d<=M))
	{
//    Std(BText("\t -> Adding ")+j+ " : " +(d/s));
	    contens_[j].PutDegree(d);
	    contens_[j].PutCoef  (pol[k].Coef());
	    j++;
	}
    }
    contens_.ReallocBuffer(j);   
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolDer);
DefExtOpr(1, BPolDer, "Der", 1, 2, "Polyn Real",
  "(Polyn p [, Real n=1])",
  I2("Returns the n-th derivative of a polynomial.",
     "Devuelve la derivada n-ésima de un polinomio."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolDer::CalcContens()
//--------------------------------------------------------------------
{
    BPol p = Pol(Arg(1));
    BInt n = 1;
    BPol w = p;
    BInt i,j;
    if(Arg(2)) { n = (BInt)Real(Arg(2)); }
    for(i=1; i<=n; i++)
    {
	for(j=0; j<w.Size(); j++)
	{
	    BInt d = w[j].Degree();
	    w[j].PutDegree(d-1);
	    w[j].PutCoef(d*w[j].Coef());
	}
	w.Aggregate();
    }
    contens_ = w;
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolSetSum);
DefExtOpr(1, BPolSetSum, "SetSum", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global sum of all polynomials of a set.",
     "Devuelve el sumatorio de todos los polinomios de un conjunto."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolSetSum::CalcContens()
//--------------------------------------------------------------------
{
    BSet& set = Set(Arg(1));
    contens_ = BPol::Zero();
    for(BInt n=1; n<=set.Card(); n++)
    {
	if(set[n]->Grammar()==GraPolyn())
	{
	    contens_ += Pol(set[n]);
	}
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolSetProd);
DefExtOpr(1, BPolSetProd, "SetProd", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global product of all polynomials of a set.",
     "Devuelve el productorio de todos los polinomios de un conjunto."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolSetProd::CalcContens()
//--------------------------------------------------------------------
{
    BSet& set = Set(Arg(1));
    contens_ = BPol::One();
    for(BInt n=1; n<=set.Card(); n++)
    {
	if(set[n]->Grammar()==GraPolyn())
	{
	    contens_ *= Pol(set[n]);
	}
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolInverseNonStationaryRoots);
DefExtOpr(1, BPolInverseNonStationaryRoots, "InverseNonStationaryRoots",
	  1, 2, "Polyn Real",
  I2("(Polyn p [, Real periodicity = 1 ])",
     "(Polyn p [, Real periodicidad = 1 ])"),
  I2("Changes the non stationary roots of a polynomial for its inverses.",
     "Cambia las raíces no estacionarias de un polinomio por sus inversas."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolInverseNonStationaryRoots::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Pol(Arg(1));
    BInt p = 1;
    if(Arg(2)) { p = (BInt)Real(Arg(2)); }
    InverseNonStationaryRoots(contens_,p,"");
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolDerivate);
DefExtOpr(1, BPolDerivate, "Derivate", 1, 1, "Polyn",
  "(Polyn p)",
  I2("Calculates the derivate of a polinomial.",
     "Calcula la derivada de un polinomio."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolDerivate::CalcContens()
//--------------------------------------------------------------------
{
    BPol p = Pol(Arg(1));
    contens_ = p.Derivate();
    Std("\nOK DERIVATE");
}

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolIntegrate);
DefExtOpr(1, BPolIntegrate, "Integrate", 1, 1, "Polyn",
  "(Polyn p)",
  I2("Calculates the integrate of a polinomial.",
     "Calcula la integral de un polinomio."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BPolIntegrate::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Pol(Arg(1));
    contens_ = contens_.Integrate();
}
