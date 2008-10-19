/* tsrgrai.cpp: Time Serie Grammar's internal operators.
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

#include <tol/tol_btsrgrai.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bstat.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerTsrGraInternal() { return(BTRUE); }


//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("tsrgrai.cpp");

//--------------------------------------------------------------------
BTsrInternal::BTsrInternal(BList* arg)

/*! BTsrInternal constructor
 */
//--------------------------------------------------------------------
    : BTsrTemporary(arg)
{
    BSerieGroup group;
    group.AddList(arg);
    group.PrepareOperate();
    PutDating(group.Dating());
    if(Dating())
    {
	PutFirstDate(group.MaxFirst());
	PutLastDate (group.MinLast());
    }
    if(group.HasVolatileDating())  { setDatingType(DATING_VOLATILE); }
}


DeclareInternalBTsrClass(BTsrMinus);
DeclareInternalBTsrClass(BTsrIsUnknown);
DeclareInternalBTsrClass(BTsrIsFinite);
DeclareInternalBTsrClass(BTsrIsPosInf);
DeclareInternalBTsrClass(BTsrIsNegInf);
DeclareInternalBTsrClass(BTsrNot);
DeclareInternalBTsrClass(BTsrSign);
DeclareInternalBTsrClass(BTsrAbs);
DeclareInternalBTsrClass(BTsrRound);
DeclareInternalBTsrClass(BTsrFloor);
DeclareInternalBTsrClass(BTsrSqrt);
DeclareInternalBTsrClass(BTsrLog10);
DeclareInternalBTsrClass(BTsrLog);
DeclareInternalBTsrClass(BTsrExp);
DeclareInternalBTsrClass(BTsrSin);
DeclareInternalBTsrClass(BTsrCos);
DeclareInternalBTsrClass(BTsrTan);
DeclareInternalBTsrClass(BTsrASin);
DeclareInternalBTsrClass(BTsrACos);
DeclareInternalBTsrClass(BTsrATan);
DeclareInternalBTsrClass(BTsrSinH);
DeclareInternalBTsrClass(BTsrCosH);
DeclareInternalBTsrClass(BTsrTanH);
DeclareInternalBTsrClass(BTsrASinH);
DeclareInternalBTsrClass(BTsrACosH);
DeclareInternalBTsrClass(BTsrATanH);
DeclareInternalBTsrClass(BTsrSum2);
DeclareInternalBTsrClass(BTsrProduct2);
DeclareInternalBTsrClass(BTsrQuotient);
DeclareInternalBTsrClass(BTsrPow);
DeclareInternalBTsrClass(BTsrSum);
DeclareInternalBTsrClass(BTsrProduct);
DeclareInternalBTsrClass(BTsrAnd);
DeclareInternalBTsrClass(BTsrOr);
DeclareInternalBTsrClass(BTsrIf);
DeclareInternalBTsrClass(BTsrGt);
DeclareInternalBTsrClass(BTsrLt);
DeclareInternalBTsrClass(BTsrGe);
DeclareInternalBTsrClass(BTsrLe);
DeclareInternalBTsrClass(BTsrEq);
DeclareInternalBTsrClass(BTsrNe);
DeclareInternalBTsrClass(BTsrMin);
DeclareInternalBTsrClass(BTsrMax);
DeclareInternalBTsrClass(BTsrNormalDist);
DeclareInternalBTsrClass(BTsrNormalDens);
DeclareInternalBTsrClass(BTsrInvNormalDist);

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DefIntOpr(1, BTsrIdentity, " + ", 1, 1,
  "(Serie S)",
  I2("Returns the same series.",
     "Devuelve la misma serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrIdentity::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(TsrR(Arg(1))[dte]);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrMinus, " - ", 1, 1,
  "(Serie S)",
  I2("Returns the series width the opposite sign.",
     "Devuelve la serie con el signo contrario."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrMinus::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(-(TsrR(Arg(1))[dte]));
}

//--------------------------------------------------------------------
DefIntOpr(2, BTsrNot, "Not", 1, 1,
  "(Serie S)",
  I2("For each date returns false if S is true in such date.",
     "Para cada fecha devuelve falso si S es verdadera en dicha "
     "fecha."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrNot::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  return(Not(BDat(TsrR(Arg(1))[dte])));
}


//--------------------------------------------------------------------
DefIntOpr(2, BTsrIsUnknown, "IsUnknown", 1, 1,
  "(Serie S)",
  I2("For each date returns true if S has an unknown value in such "
     "date.",
     "Para cada fecha devuelve true si S tiene un valor  desconocido "
     "en dicha fecha."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrIsUnknown::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BDat((TsrR(Arg(1))[dte]).IsUnknown()));
}


//--------------------------------------------------------------------
DefIntOpr(2, BTsrIsFinite, "IsFinite", 1, 1,
  "(Serie S)",
  I2("For each date returns true if S has a finite value in such "
     "date.",
     "Para cada fecha devuelve true si S tiene un valor finito en "
     "dicha fecha."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrIsFinite::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BDat((TsrR(Arg(1))[dte]).IsFinite()));
}

//--------------------------------------------------------------------
DefIntOpr(2, BTsrIsPosInf, "IsPosInf", 1, 1,
  "(Serie S)",
  I2("For each date returns true if S has a positive infinite value "
     "in such date.",
     "Para cada fecha devuelve true si S tiene un valor infinito "
     "positivo en dicha fecha."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrIsPosInf::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BDat((TsrR(Arg(1))[dte]).IsPosInf()));
}

//--------------------------------------------------------------------
DefIntOpr(2, BTsrIsNegInf, "IsNegInf", 1, 1,
  "(Serie S)",
  I2("For each date returns true if S has a negative infinite value "
     "in such date.",
     "Para cada fecha devuelve true si S tiene un valor infinito "
     "negativo en dicha fecha."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrIsNegInf::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BDat((TsrR(Arg(1))[dte]).IsNegInf()));
}
//--------------------------------------------------------------------
DefIntOpr(1, BTsrSign, "Sign", 1, 1,
  "(Serie S)",
  I2("Returns - 1, 0, or 1 according to S will be smaller, equal or greater "
     "than zero.",
     "Devuelve -1, 0, o 1 según S sea menor, igual o mayor que "
     "cero"),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSign::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Sign(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrAbs, "Abs", 1, 1,
  "(Serie S)",
  I2("Returns the absolute value of S.",
     "Devuelve el valor absoluto of S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrAbs::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Abs(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrRound, "Round", 1, 1,
  "(Serie S)",
  I2("Returns the series of the nearest entire numbers to each datum of "
     "a given series.",
     "Devuelve la serie de los números enteros mas cercanos a cada dato de "
     "una serie dada."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrRound::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Round(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrFloor, "Floor", 1, 1,
  "(Serie S)",
  I2("For each datum of S, returns its entire part.",
     "Para cada dato de S, devuelve su parte entera."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrFloor::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Floor(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrSqrt, "Sqrt", 1, 1,
  "(Serie S)",
  I2("Returns the square root of S.",
     "Devuelve la raíz cuadrada de S."),
  BOperClassify::TimeSeriesAlgebra_);
  DefIntOpr(2, BTsrSqrt, "SqRt", 1, 1,
  "(Serie S)",
  I2("Returns the square root of S.",
     "Devuelve la raíz cuadrada de S."),
	    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSqrt::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  return(Sqrt(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrLog10, "Log10", 1, 1,
  "(Serie S)",
  I2("Returns the decimal logarithm of S.",
     "Devuelve el logaritmo decimal de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrLog10::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Log10(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrLog, "Log", 1, 1,
  "(Serie S)",
  I2("Returns the natural logarithm of S.",
     "Devuelve el logaritmo natural de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrLog::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Log(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrExp, "Exp", 1, 1,
  "(Serie S)",
  I2("Returns the exponential of S.",
     "Devuelve la exponencial de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrExp::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Exp(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrSin, "Sin", 1, 1,
  "(Serie S)",
  I2("Returns the trigonometric sine of S.",
     "Devuelve el seno trigonométrico de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSin::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Sin(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrCos, "Cos", 1, 1,
  "(Serie S)",
  I2("Returns the trigonometric cosine of S.",
     "Devuelve el coseno trigonométrico de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrCos::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  return(Cos(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrTan, "Tan", 1, 1,
  "(Serie S)",
  I2("Returns the trigonometric tangent of S.",
     "Devuelve la tangente trigonométrica de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrTan::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(Tan(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrASin, "ASin", 1, 1,
  "(Serie S)",
  I2("Returns the series wich trigonometric sine is S.",
     "Devuelve la serie cuyo seno trigonométrico es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrASin::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(ASin(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrACos, "ACos", 1, 1,
  "(Serie S)",
  I2("Returns the series wich trigonometric cosine is S.",
     "Devuelve la serie cuyo coseno trigonométrico es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrACos::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(ACos(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrATan, "ATan", 1, 1,
  "(Serie S)",
  I2("Returns the series wich trigonometric tangent is S.",
     "Devuelve la serie cuya tangente trigonométrica es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrATan::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(ATan(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrASinH, "ASinH", 1, 1,
  "(Serie S)",
  I2("Returns the series wich hyperbolic sine is S.",
     "Devuelve la serie cuyo seno hiperbólico es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrASinH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat x = TsrR(Arg(1))[dte];
    return(Log(x) + Log(x^BDat(2) + BDat(1)) / BDat(2));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrACosH, "ACosH", 1, 1,
  "(Serie S)",
  I2("Returns the sereis wich hyperbolic cosine is S.",
     "Devuelve la serie cuyo coseno hiperbólico es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrACosH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat x = TsrR(Arg(1))[dte];
    return(Log(x) + Log(x^BDat(2) - BDat(1)) / BDat(2));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrATanH, "ATanH", 1, 1,
  "(Serie S)",
  I2("Returns the series wich hyperbolic tangent is S.",
     "Devuelve la serie cuya tangente hiperbólica es S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrATanH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat x = TsrR(Arg(1))[dte];
    return(Log((BDat(1)+x)/(BDat(1)-x))/2);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrSinH, "SinH", 1, 1,
  "(Serie S)",
  I2("Returns the hyperbolic sine of the S.",
     "Devuelve el seno hiperbólico de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSinH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(SinH(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrCosH, "CosH", 1, 1,
  "(Serie S)",
  I2("Returns the hyperbolic cosine of S.",
     "Devuelve el coseno hiperbólico de ."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrCosH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(CosH(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrTanH, "TanH", 1, 1,
  "(Serie S)",
  I2("Returns the hyperbolic tangent of S.",
     "Devuelve la tangente hiperbólica de S."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrTanH::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(TanH(TsrR(Arg(1))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrSum2, "+", 2, 2,
  "S1 + S2 {Serie S1, Serie S2}",
  I2("Returns the summe of both series.",
     "Devuelve la suma de ambas series."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSum2::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return((TsrR(Arg(1))[dte]) + (TsrR(Arg(2))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrDifference, "-", 2, 2,
  "S1 - S2 {Serie S1, Serie S2}",
  I2("Returns the difference between both series.",
     "Devuelve la diferencia entre ambas series."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrDifference::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return((TsrR(Arg(1))[dte]) - (TsrR(Arg(2))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrProduct2, "*", 2, 2,
  "S1 * S2 {Serie S1, Serie S2}",
  I2("Returns the product of both series.",
     "Devuelve el producto de ambas series."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrProduct2::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return((TsrR(Arg(1))[dte]) * (TsrR(Arg(2))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrQuotient, "/", 2, 2,
  "S1 / S2 {Serie S1, Serie S2}",
  I2("Returns the quotient of both series.",
     "Devuelve el cociente entre ambas series."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrQuotient::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return((TsrR(Arg(1))[dte]) / (TsrR(Arg(2))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(2, BTsrPow, "^", 2, 2,
  "S1^S2 {Serie S1, Serie S2}",
  I2("Returns the power of S1 increased to S2.",
     "Devuelve la potencia de S1 elevada a S2.")+
  I2("Also it can be used the operator ** or the function Pow.",
     "También se puede utilizar el operador ** o la función Pow."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrPow::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return((TsrR(Arg(1))[dte]) ^ (TsrR(Arg(2))[dte]));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrSum, "Sum", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns the global sum of all arguments.",
     "Devuelve el sumatorio de todos sus argumentos."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrSum::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BUserTimeSerie* S;
    BDat dat = TsrR(Arg(1))[dte];
    BInt n = 2;
    while((dat.Known()) && (S = Tsr(Arg(n++)))) { dat += (*S)[dte]; }
    return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrProduct, "Prod", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns the global product of all arguments.",
     "Devuelve el productorio de todos sus argumentos."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrProduct::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BUserTimeSerie* S;
    BDat dat = TsrR(Arg(1))[dte];
    BInt n = 2;
    while((dat.Known()) && (S = Tsr(Arg(n++))))  { dat *= (*S)[dte]; }
    return(dat);
}


//--------------------------------------------------------------------
class BTsrSetStat: public BTsrTemporary
//--------------------------------------------------------------------
{
public:
  BSerieGroup group_;
  BData vec_;

  BTsrSetStat(BList* arg)
  : BTsrTemporary(arg)
  {
    group_.AddSet(Set(Car(arg)));
    group_.PrepareOperate();
    PutDating(group_.Dating());
    if(Dating())
    {
      PutFirstDate(group_.MaxFirst());
      PutLastDate (group_.MinLast());
    }
    vec_.AllocBuffer(group_.NumSeries());
  }

 ~BTsrSetStat() {}

  void GetVec(const BDate& dte)
  {
    for(BInt n=0; n<vec_.Size(); n++)
    {
    	vec_[n] = TsrR(group_[n+1])[dte];
    }
  }
};


#define DecSerStat(NAME,STAT,ENGLISH,SPANISH)				     \
									     \
class BTsrSet##STAT: public BTsrSetStat				     \
{									     \
public:									     \
  BTsrSet##STAT(BList* arg) : BTsrSetStat(arg) {}			     \
 ~BTsrSet##STAT() {}							     \
									     \
  BDat GetDat(const BDate& dte);					     \
};									     \
DeclareEvaluator(BTsrSet##STAT)						     \
DefExtOpr(1, BTsrSet##STAT, NAME, 1, 1, "Set",				     \
"(Set series)",								     \
I2("Returns "ENGLISH" of a set of series.",				     \
   "Devuelve "SPANISH" de un conjunto de series."),			     \
BOperClassify::TimeSeriesAlgebra_);					     \
									     \
BDat BTsrSet##STAT::GetDat(const BDate& dte)				     \
{									     \
  GetVec(dte);								     \
  return(STAT(vec_));							     \
}

DecSerStat("SetSum",Sum,"the sum","la suma");
DecSerStat("SetProd",Product,"the product","el producto");
DecSerStat("SetAvr",Average,"the average","la media");
DecSerStat("SetHarmonicAvr",HarmonicAverage,"the harmonic average","la media armónica");
DecSerStat("SetGeometricAvr",GeometricAverage,"the geometric average","la media geométrica");
DecSerStat("SetVar",Varianze,"the varianze","la varianza");
DecSerStat("SetStDs",StandardDeviation,"the standard deviation","la desviación estándar");
DecSerStat("SetAsymmetry",AsymmetryCoefficient,"the asymmetry coefficient","el coeficiente de asimetría");
DecSerStat("SetKurtosis",Kurtosis,"the kurtosis","la kurtosis");
DecSerStat("SetMax",Maximum,"the maximum","el máximo");
DecSerStat("SetMin",Minimum,"the minimum","el mínimo");
DecSerStat("SetMedian",Median,"the median","la mediana");


//--------------------------------------------------------------------
DefIntOpr(1, BTsrIf, "IfSer", 3, 3,
  I2("(Serie condition, Serie S1, Serie S2)",
     "(Serie condición, Serie S1, Serie S2)"),
  I2("For each date returns the value of S1 if condition is true. Else "
     "returns the value of S2.",
     "Para cada fecha devuelve el valor de S1 si la condición se cumple."
     "En otro caso devuelve el valor de S2."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrIf::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BDat	cond = TsrR(Arg(1))[dte];
    BDat	dat;
    if(cond.IsKnown())
    {
	if(cond.Value()==0.0)  { dat = TsrR(Arg(3))[dte]; }
	else		       { dat = TsrR(Arg(2))[dte]; }
    }
    return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrAnd, "And", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true just if all arguments are true.",
     "Devuelve cierto si todos sus argumentos son ciertos."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrAnd::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  BUserTimeSerie* S;
  BDat	      dat = TsrR(Arg(1))[dte];
  BInt	      n	  = 2;
  while((dat.Known()) && (dat.Value()!=0.0) && (S = Tsr(Arg(n++))))
  { dat.And((*S)[dte]); }
  return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrOr, "Or", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if at least one argument is true.",
     "Devuelve cierto si al menos uno de sus argumentos es cierto."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrOr::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  BUserTimeSerie* S;
  BDat	      dat = TsrR(Arg(1))[dte];
  BInt	      n	  = 2;
  while((dat.Known()) && (dat.Value()!=1.0) && (S = Tsr(Arg(n++))))
  { dat.Or((*S)[dte]); }
  return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrGt, "GT", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is greater than the following.",
     "Devuelve cierto si cada argumento es mayor que el siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrGt::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(LogicShort(BDat::IsGreaterThan, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrLt, "LT", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is less than the following.",
     "Devuelve cierto si cada argumento es menor que el siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrLt::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(LogicShort(BDat::IsLessThan, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrGe, "GE", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is greater or just as the following.",
     "Devuelve cierto si cada argumento es igual o mayor que el siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrGe::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  return(LogicShort(BDat::IsGreaterOrEqualThan, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrLe, "LE", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is less or just as the following.",
     "Devuelve cierto si cada argumento es igual o menor que el siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrLe::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(LogicShort(BDat::IsLessOrEqualThan, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrEq, "Eq", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is just as the following.",
     "Devuelve cierto si cada argumento es igual que el siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrEq::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  return(LogicShort(BDat::IsEqualAs, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrNe, "NE", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns true if each argument is different from following.",
     "Devuelve cierto si cada argumento es diferente al siguiente."),
	  BOperClassify::TimeSeriesLogic_);
//--------------------------------------------------------------------
BDat BTsrNe::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(LogicShort(BDat::IsNotEqualAs, dte));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrMin, "Min", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns the series of the minima from among two or more series.",
     "Devuelve la serie de los mínimos de entre dos o más series."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrMin::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BUserTimeSerie* S;
    BDat dat = TsrR(Arg(1))[dte];
    BInt n = 2;
    
    while((dat.Known()) && (S = Tsr(Arg(n++))))
    {
	if((!(*S)[dte].IsKnown()) || (dat > (*S)[dte])) {
	    dat = (*S)[dte]; 
	}
    }
    return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrMax, "Max", 1, 0,
  "(Serie S1 [, Serie S2, ...])",
  I2("Returns the series of the maxima from among two or more series.",
     "Devuelve la serie de los máximos de dos o mas numeros."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrMax::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  BUserTimeSerie* S;
  BDat	      dat = TsrR(Arg(1))[dte];
  BInt	      n	  = 2;

  while((dat.Known()) && (S = Tsr(Arg(n++))))
  {
    if((!(*S)[dte].IsKnown()) || (dat < (*S)[dte]))
    { dat = (*S)[dte]; }
  }
  return(dat);
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrNormalDist, "F01", 1, 1,
  "(Serie S)",
  I2("Normal(0,1) probability distribution of a serie.",
     "Distribución de probabilidad Normal(0,1) de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrNormalDist::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BNormalDist::Dist01(TsrR(Arg(1))[dte].Value()));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrNormalDens, "f01", 1, 1,
  "(Serie S)",
  I2("Normal(0,1) probability density of a serie.",
     "Densidad de probabilidad Normal(0,1) de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrNormalDens::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    return(BNormalDist::Dens01(TsrR(Arg(1))[dte].Value()));
}


//--------------------------------------------------------------------
DefIntOpr(1, BTsrInvNormalDist, "InvF01", 1, 1,
  "(Serie S)",
  I2("Normal(0,1) inverse probability distribution of a serie.",
     "Inversa de la distribucion de probabilidad Normal(0,1) de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrInvNormalDist::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    BNormalDist nor(0, 1);
    return(nor.Inverse(TsrR(Arg(1))[dte].Value()));
}

//--------------------------------------------------------------------
class BTsrEvalSerie: public BTsrInternal 
//--------------------------------------------------------------------
{      
private:
  BUserTimeSerie* ser_;
  BCode* code_;
  
public:				    
  BTsrEvalSerie(BList* arg)		    
  : BTsrInternal(arg)  
  { 
    ser_  = Tsr(Arg(1));
    code_ = &Code(Arg(2));
    
    BOperator* opr = code_?code_->Operator():NULL; 
    if(!opr ||
       (opr->NumArg()!=1)||
       (opr->GrammarForArg(1)!=GraReal())||
       (opr->Grammar()!=GraReal()))
    {
    	Error(I2("Wrong argument function in EvalSet function calling."
		           "It must have just one argument of Real type and return another Real.",
		           "Función argumento errónea en llamada a la función EvalSet."
		           "Ésta debe tener exactamente un argumento de tipo Real y devolver un Real."));
      code_ = NULL;
	    return;
    }
  }		    
  ~BTsrEvalSerie() {}			    
  BDat GetDat(const BDate& dte);	    
};					    
DeclareEvaluator(BTsrEvalSerie)

//--------------------------------------------------------------------
DefExtOpr(1, BTsrEvalSerie, "EvalSerie", 2, 2, "Serie Code",
  "(Serie S, Code realToRealFunction)",
  I2("Returns a time serie with the same dating and dates of given one "
     "applying a real to real function to each datum",
     "Devuelve una serie con el mismo fechado y fechas que la serie "
     "dada aplicando una función de real a real para cada dato."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrEvalSerie::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!code_) { return(BDat::Unknown());  }
  BDat ser_dat((*ser_)[dte]);
  BParamDat* x_ = new BParamDat("",ser_dat,"");
  BList* lst = NCons(x_);
 	BSyntaxObject* objCode = code_->Evaluator(lst);
  if(!objCode || (objCode->Grammar()!=GraReal()))
  {
    Error(I2("EvalSet failed. A Real was expected.","Fallo en EvalSerie. Se esperaba un Real."));
    DESTROY(objCode);
    return(BDat::Unknown());
  }
  BDat y = Dat(objCode);
  DESTROY(objCode);
  return(y);
}

