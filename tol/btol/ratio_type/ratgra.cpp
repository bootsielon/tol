/* ratgra.cpp: BAtom basic class, attributes and methods.
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

#include <tol/tol_bratgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_matrix.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_bmfstpro.h>

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("ratgra.cpp");
template<> 
BGrammar* BGraContensBase<BRat>::ownGrammar_ = NIL;

DefineContensCommonOperators(BRat, "Ratio");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BRat>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}


template<>
BRat& BRat::Unknown()
{
    BMonome<BDat> unkmon(BDat::Unknown(),0);
    BPolyn<BDat> unkpol(unkmon);
    static BRat zero_(unkpol, unkpol);
    return zero_;
}

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BRat>::InitInstances()
{
    BTraceInit("BGraContens<BRat>::InitInstances");
    BSystemRat* id_ = new BSystemRat
	(
	    "IDelta",
	    BPol::One()/(BPol::One()-BPol::X()),
	    I2("Inverse of Delta operator.","Inverse of Delta operator.")
	);
    BPol unk = BPol::Unknown();
    BPol one = BPol::One();
    BSystemRat* unknown_ = new BSystemRat
	(
	    "UnknownRatio",
	    BRational<BDat>(unk,one),
	    I2("Unknown Ratio.", "Función racional desconocida")
	    );
    OwnGrammar()->PutDefect(unknown_);
}


//--------------------------------------------------------------------
class BRatFromPol : public BRatTemporary

/*! Casting from polyn.
 */
//--------------------------------------------------------------------
{
public:
    BRatFromPol(BList* arg) : BRatTemporary(arg) { }
    ~BRatFromPol() { }
    void CalcContens() { contens_ = Pol(Arg(1))/BPol::One(); }
};


//--------------------------------------------------------------------
/*! Returns a valid constant BRat for name.
 */
template<>
BSyntaxObject* BGraContensBase<BRat>::FindConstant(const BText& name)
{ return(NIL); }


//--------------------------------------------------------------------
/*! Returns a valid rational for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BRat>::Casting(BSyntaxObject* obj)
{
  if(!obj)			     { return(NIL); }
  if(obj->Grammar()==OwnGrammar())   { return(obj); }
  if(obj->Grammar()->Name()=="Polyn"){ return(new BRatFromPol(NCons(obj))); }
  return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BRat>::New(FILE* fil)
{
    return 0;
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatIdentity);
DefIntOpr(1, BRatIdentity, " + ", 1, 1,
	  "(Ratio r)",
	  I2("Returns the same polynomial reasons.",
	     "Devuelve la misma razón de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatIdentity::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Rat(Arg(1)); }


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatMinus);
DefIntOpr(1, BRatMinus, " - ",1, 1,
  "(Ratio r)",
  I2("Returns the polynomial reason width the oposite sign.",
     "Devuelve la razón de polinomios con el signo contrario."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatMinus::CalcContens()
//--------------------------------------------------------------------
{ contens_ = -Rat(Arg(1)); }


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatSum);
DefIntOpr(1, BRatSum, "+", 2, 2,
  "r1 + r2 {Ratio r1, Ratio r2}",
  I2("Returns the summe of both polynomial reasons.",
     "Devuelve la suma de ambas razones de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatSum::CalcContens()
//--------------------------------------------------------------------
{
    BRat q1 = Rat(Arg(1));
    BRat q2 = Rat(Arg(2));
    q1 += q2;
    contens_ = q1;
}


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatDifference);
DefIntOpr(1, BRatDifference, "-", 2, 2,
  "r1 - r2 {Ratio r1, Ratio r2}",
  I2("Returns the difference between both polynomial reasons.",
     "Devuelve la diferencia entre ambas razones de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatDifference::CalcContens()
//--------------------------------------------------------------------
{
    BRat q1 = Rat(Arg(1));
    BRat q2 = Rat(Arg(2));
    q1 -= q2;
    contens_ = q1;
}


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatProduct);
DefIntOpr(1, BRatProduct, "*", 2, 2,
  "r1 * r2 {Ratio r1, Ratio r2}",
  I2("Returns the product of both polynomial reasons.",
     "Devuelve el producto de ambas razones de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatProduct::CalcContens()
//--------------------------------------------------------------------
{
    BRat q1 = Rat(Arg(1));
    BRat q2 = Rat(Arg(2));
    q1 *= q2;
    contens_ = q1;
}


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatQuotient);
DefIntOpr(1, BRatQuotient, ":", 2, 2,
  "q1:q2 {Ratio q1, Ratio q2}",
  I2("Returns the quotient between two polynomial reasons.",
     "Devuelve el cociente entre dos razones de polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatQuotient::CalcContens()
//--------------------------------------------------------------------
{
    BRat q1 = Rat(Arg(1));
    BRat q2 = Rat(Arg(2));
    q1 /= q2;
    contens_ = q1;
}


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatPolQuotient);
DefExtOpr(1, BRatPolQuotient, "/", 2, 2, "Polyn Polyn",
	  "r1 / r2 {Polyn r1, Polyn r2}",
  I2("Returns the quotient between two polynomial.",
     "Devuelve el cociente entre dos polinomios."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatPolQuotient::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Pol(Arg(1)) / Pol(Arg(2)); }


//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatPow);
DefExtOpr(2, BRatPow, "^", 2, 2, "Ratio Real",
  "r1^n {Ratio r1, Real n}",
  I2("Returns the power of a polynomials reason increased to an entire "
     "number. "
     "Also it can be used the operator ** or the function Pow.",
     "Devuelve la potencia de una razón de polinomios elevada a un nmero "
     "entero. También se puede utilizar el operador ** o la función Pow."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatPow::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Rat(Arg(1)) ^ (BInt)Real(Arg(2)); }



//--------------------------------------------------------------------
DeclareContensClass(BRat, BRatTemporary, BRatAdjustQuotient);
DefExtOpr(2, BRatAdjustQuotient, "AdjustQuotient", 3, 3, "Polyn Real Real",
  "(Polyn pol, Real p, Real q)",
  I2(".",
     "."),
	  BOperClassify::RetardPolynomial_);
//--------------------------------------------------------------------
void BRatAdjustQuotient::CalcContens()
//--------------------------------------------------------------------
{
  BPol pol = Pol(Arg(1));
  BInt p   = (BInt)Real(Arg(3));
  BInt q   = (BInt)Real(Arg(2));
  BInt n   = pol.Degree();

  if(n<p+q)
  {
    Error(I2("Insuficient freedom's degrees to apply AdjustQuotient.",
	     "Grados de libertad insuficientes para aplicar AdjustQuotient.")+
	     " (p="+q+")+(q="+p+ ")>(n="+n+")");
    contens_ = BPol::Zero() / BPol::One();
  }
  else
  {
    if(!p && !q)
    {
      contens_ = BPol::One() / BPol::One();
    }
    else if(p && !q)
    {
      BPol x = (BPol::One()/pol)%p;
      contens_ = BPol::One()/x;
    }
    else if(!p && q)
    {
      BPol x = pol;
      x.ReallocBuffer(q+1);
      contens_ = x/BPol::One();
    }
    else
    {
      BInt i,j;
      BPol num, den;
      BMatrix<BDat> Y(n-q,1);
      BMatrix<BDat> X(n-q,p);
      BMatrix<BDat> U(q,1);
      BMatrix<BDat> V(q,p);
      BMatrix<BDat> Xt, XtY, fi, teta;
      BSymMatrix<BDat> XtX, XtXi;

      BArray<BDat> psi;
      psi.ReallocBuffer(n+1);
      psi[0] = 1;
      for(i=1; i<=n; i++) { psi[i]  = pol.Coef(i); }
      for(i=0; i<q;  i++)
      {
	U(i,0) = psi[i+1];
	for(j=0; j<p; j++)
	{
	  if(j<=i) { V(i,j) = psi[i-j]; }
	  else	   { V(i,j) = 0; }
	}
      }
      for(; i<n;  i++)
      {
	Y(i-q,0) = -psi[i+1];
	for(j=0; j<p; j++)
	{
	  if(j<=i) { X(i-q,j) = psi[i-j]; }
	  else	   { X(i-q,j) = 0; }
	}
      }
//    TRZ(U); TRZ(V); TRZ(Y); TRZ(X);
      XtX = MtMSqr(X);

      Inverse(XtX,XtXi);
      Xt  = X.T();
      XtY = Xt*Y;
//    TRZ(XtX); TRZ(XtXi); TRZ(XtY);
      fi = XtXi*XtY;
      teta = U + (V*fi);
//    TRZ(fi); TRZ(teta);
      num.ReallocBuffer(p+1);
      num[0].PutDegree(0);
      num[0].PutCoef  (1);
      for(i=1; i<=p; i++)
      {
	num[i].PutDegree(i);
	num[i].PutCoef	(fi(i-1,0));
      }
      den.ReallocBuffer(q+1);
      den[0].PutDegree(0);
      den[0].PutCoef  (1);
      for(i=1; i<=q; i++)
      {
	den[i].PutDegree(i);
	den[i].PutCoef	(teta(i-1,0));
      }

      contens_ = den / num;
    }
  }
}
