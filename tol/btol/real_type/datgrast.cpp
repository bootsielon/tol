/* datgrast.cpp: Real Grammar's arithmetic functions
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

#define DEFAULT_INIT_PRIORITY 30

#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bstat.h>
#include <tol/tol_btest.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraStatistic() { return(BTRUE); }

BTraceInit("datgrast.cpp");

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMin);
  DefIntOpr(1, BDatMin, "Min", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the minimum from among all the arguments.",
     "Devuelve el mínimo de entre todos los argumentos."),
     BOperClassify::Statistic_);
  void BDatMin::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Minimum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMax);
  DefIntOpr(1, BDatMax, "Max", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the maximum from among all the arguments.",
     "Devuelve el máximo de entre todos los argumentos."),
     BOperClassify::Statistic_);
  void BDatMax::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Maximum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAvr);
  DefIntOpr(1, BDatAvr, "Avr", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the arithmetic average of all arguments.",
     "Devuelve la media aritmética de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatAvr::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Average(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHarmonicAvr);
  DefIntOpr(1, BDatHarmonicAvr, "HarmonicAvr", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the harmonic average of all arguments.",
     "Devuelve la media armónica de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatHarmonicAvr::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = HarmonicAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGeometricAvr);
  DefIntOpr(1, BDatGeometricAvr, "GeometricAvr", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the geometric average of all arguments.",
     "Devuelve la media geométrica de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatGeometricAvr::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = GeometricAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatVar);
  DefIntOpr(1, BDatVar, "Var", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the varianze of all arguments.",
     "Devuelve la varianza de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatVar::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Varianze(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatStDs);
  DefIntOpr(1, BDatStDs, "StDs", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the standard deviation of all arguments.",
     "Devuelve la desviación estandar de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatStDs::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = StandardDeviation(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAsymmetry);
  DefIntOpr(1, BDatAsymmetry, "Asymmetry", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the asymmetry ceficient of all arguments.",
     "Devuelve el coeficiente de asimetría de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatAsymmetry::CalcContens()
//--------------------------------------------------------------------
{

  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = AsymmetryCoefficient(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatKurtosis);
  DefIntOpr(1, BDatKurtosis, "Kurtosis", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the kurtosis of all arguments.",
     "Devuelve la curtosis de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatKurtosis::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Kurtosis(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMedian);
  DefIntOpr(1, BDatMedian, "Median", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the median of all arguments.",
     "Devuelve la mediana de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatMedian::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Median(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSum);
  DefIntOpr(1, BDatSum, "Sum", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the global sum of all arguments.",
     "Devuelve el sumatorio de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatSum::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Sum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatProduct);
  DefIntOpr(1, BDatProduct, "Prod", 1, 0,
  "(Real x1 [, Real x2, ...])",
  I2("Returns the global product of all arguments.",
     "Devuelve el productorio de todos sus argumentos."),
     BOperClassify::Statistic_);
  void BDatProduct::CalcContens()
//--------------------------------------------------------------------
{
  BArray<BDat> vec(NumArgs());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Product(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMoment);
  DefIntOpr(1, BDatMoment, "Moment", 2, 0,
  "(Real n, Real x1 [, Real x2, ...])",
  I2("Returns the n-moment of x1,x2,...",
     "Devuelve el momento de orden n de los valores x1,x2,..."),
     BOperClassify::Statistic_);
  void BDatMoment::CalcContens()
//--------------------------------------------------------------------
{
  BInt order = (BInt)Real(Arg(1));
  BArray<BDat> vec(NumArgs());
  for(BInt n=1; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Moment(vec, order);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCenterMoment);
  DefIntOpr(1, BDatCenterMoment, "CenterMoment", 2, 0,
  "(Real n, Real x1 [, Real x2, ...])",
  I2("Returns the n-centered moment of x1,x2,....",
     "Devuelve el momento centrado de orden n de x1,x2,..."),
     BOperClassify::Statistic_);
  void BDatCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  BInt order = (BInt)Real(Arg(1));
  BArray<BDat> vec(NumArgs());
  for(BInt n=1; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = CenterMoment(vec, order);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatQuantile);
  DefIntOpr(1, BDatQuantile, "Quantile", 2, 0,
  "(Real p, Real x1 [, Real x2, ...])",
  I2("Returns the quantile of probabilty p of x1,x2,....",
     "Devuelve el cuantil de probabilidad p de x1,x2,..."),
     BOperClassify::Statistic_);
  void BDatQuantile::CalcContens()
//--------------------------------------------------------------------
{
  BDat p = Dat(Arg(1));
  BArray<BDat> vec(NumArgs());
  for(BInt n=1; n<vec.Size(); n++) { vec[n] = Dat(Arg(n+1)); }
  contens_ = Quantile(vec, p);
}


//--------------------------------------------------------------------
  static void ErrorNoRealElement(const BText& function)
//--------------------------------------------------------------------
{
  Error(I2("Elements of arguments for ",
           "Los elementos de los argumentos de ")+function+
        I2(" must be of Real type",
           " deben ser de tipo Real"));
}

//--------------------------------------------------------------------
  static bool CheckRealElement(const BText& function, const BSet& arg)
//--------------------------------------------------------------------
{
  int n;
  bool ok = true;
  for(n=1; n<=arg.Card(); n++)
  {
    if(arg[n]->Grammar()!=GraReal()) { ok = false; }
  }
  if(!ok) { ErrorNoRealElement(function); }
  return(ok);
}

//--------------------------------------------------------------------
  static bool CheckRealElement(const BText& function, 
                               const BSet& arg1, 
                               const BSet& arg2)
//--------------------------------------------------------------------
{
  bool ok = CheckRealElement(function, arg1);
  if(ok) { ok = CheckRealElement(function, arg2); }
  return(ok);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatCount);
  DefExtOpr(1, BDatMatCount, "MatCount", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the number of cells of a matrix (rows x columns).",
     "Devuelve el número de celdas de una matriz (filas x columnas)."),
     BOperClassify::Statistic_);
  void BDatMatCount::CalcContens()
//--------------------------------------------------------------------
{
  BMat& x = Mat(Arg(1));
  contens_ = x.Rows() * x.Columns();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatKnown);
  DefExtOpr(1, BDatMatKnown, "MatKnown", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the number of cells of a matrix (rows x columns).",
     "Devuelve el número de celdas de una matriz (filas x columnas)."),
     BOperClassify::Statistic_);
  void BDatMatKnown::CalcContens()
//--------------------------------------------------------------------
{
  BMat& x = Mat(Arg(1));
  contens_ = Known(x.Data());
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetKnown);
  DefExtOpr(1, BDatSetKnown, "SetKnown", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global sum of all elements of a set of numbers.",
     "Devuelve el número de elementos no omitidos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetKnown::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetSum",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) 
  { 
    vec[n] = Dat(set[n+1]); 
  }
  contens_ = Known(vec);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetSum);
  DefExtOpr(1, BDatSetSum, "SetSum", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global sum of all elements of a set of numbers.",
     "Devuelve el sumatorio de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetSum::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetSum",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) 
  { 
    vec[n] = Dat(set[n+1]); 
  }
  contens_ = Sum(vec);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatSum);
  DefExtOpr(1, BDatMatSum, "MatSum", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the global sum of all elements of a matrix.",
     "Devuelve el sumatorio de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatSum::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Sum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetProduct);
  DefExtOpr(1, BDatSetProduct, "SetProd", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the global product of all elements of a set of numbers.",
     "Devuelve el productorio de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetProduct::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetProd",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Product(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatProd);
  DefExtOpr(1, BDatMatProd, "MatProd", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the global product of all elements of a matrix.",
     "Devuelve el producto de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatProd::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Product(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetMedian);
  DefExtOpr(1, BDatSetMedian, "SetMedian", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the median of all elements of a set of numbers.",
     "Devuelve la mediana de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetMedian::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetMedian",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Median(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatMedian);
  DefExtOpr(1, BDatMatMedian, "MatMedian", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the global median of all elements of a matrix.",
     "Devuelve la mediana de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatMedian::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Median(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetMin);
  DefExtOpr(1, BDatSetMin, "SetMin", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the minimum of all elements of a set of numbers.",
     "Devuelve el mínimo de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetMin::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetMin",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Minimum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatMin);
  DefExtOpr(1, BDatMatMin, "MatMin", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the global minimum of all elements of a matrix.",
     "Devuelve el mínimo de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatMin::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Minimum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetMax);
  DefExtOpr(1, BDatSetMax, "SetMax", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the maximum of all elements of a set of numbers.",
     "Devuelve la máximo de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetMax::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetMax",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Maximum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatMax);
  DefExtOpr(1, BDatMatMax, "MatMax", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the global maximum of all elements of a matrix.",
     "Devuelve el máximo de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatMax::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Maximum(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetAvr);
  DefExtOpr(1, BDatSetAvr, "SetAvr", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the arithmetic average of all elements of a set of numbers.",
     "Devuelve la media aritmética de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetAvr::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetAvr",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Average(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatAvr);
  DefExtOpr(1, BDatMatAvr, "MatAvr", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the arithmetic average of all elements of a matrix.",
     "Devuelve la media aritmética de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatAvr::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Average(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetHarmonicAvr);
  DefExtOpr(1, BDatSetHarmonicAvr, "SetHarmonicAvr", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the harmonic average of all elements of a set of numbers.",
     "Devuelve la media armónica de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetHarmonicAvr::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetHarmonicAvr",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = HarmonicAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatHarmonicAvr);
  DefExtOpr(1, BDatMatHarmonicAvr, "MatHarmonicAvr", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the harmonic average of all elements of a matrix.",
     "Devuelve la media armónica de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatHarmonicAvr::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = HarmonicAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetGeometricAvr);
  DefExtOpr(1, BDatSetGeometricAvr, "SetGeometricAvr", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the geometric average of all elements of a set of numbers.",
     "Devuelve la media geométrica de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetGeometricAvr::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetGeometricAvr",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = GeometricAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatGeometricAvr);
  DefExtOpr(1, BDatMatGeometricAvr, "MatGeometricAvr", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the geometric average of all elements of a matrix.",
     "Devuelve la media geométrica de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatGeometricAvr::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = GeometricAverage(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetQuantile);
  DefExtOpr(1, BDatSetQuantile, "SetQuantile", 2, 2, "Set Real",
  I2("(Set set, Real p)","(Set cto, Real p)"),
  I2("Returns the quantile of probability p of all elements of a set of numbers.",
     "Devuelve el cuantil de probabilidad p de todos los elementos "
     "de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetQuantile::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetQuantile",set)) { return; }
  BDat	q   = Dat(Arg(2));
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Quantile(vec, q);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatQuantile);
  DefExtOpr(1, BDatMatQuantile, "MatQuantile", 2, 2, "Matrix Real",
  "(Matrix mat, Real p)",
  I2("Returns the quantile of probabilty p of all elements of a matrix.",
     "Devuelve el cuantil de probabilidad p de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatQuantile::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  BDat& p = Dat(Arg(2));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Quantile(vec, p);
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetStDs);
  DefExtOpr(1, BDatSetStDs, "SetStDs", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the standard deviation of all elements of a set of numbers.",
     "Devuelve la desviación típica de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetStDs::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetStDs",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = StandardDeviation(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatStDs);
  DefExtOpr(1, BDatMatStDs, "MatStDs", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the standard deviation of all elements of a matrix.",
     "Devuelve la desviación típica de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatStDs::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = StandardDeviation(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetVar);
  DefExtOpr(1, BDatSetVar, "SetVar", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the varianze of all elements of a set of numbers.",
     "Devuelve la varianza de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetVar::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetVar",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Varianze(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatVar);
  DefExtOpr(1, BDatMatVar, "MatVar", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the varianze of all elements of a matrix.",
     "Devuelve la varianza de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatVar::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Varianze(vec);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatMoment);
  DefExtOpr(1, BDatMatMoment, "MatMoment", 2, 2, "Matrix Real",
  "(Matrix mat, Real n)",
  I2("Returns the n-th moment of all cells of a matrix.",
     "Devuelve el momento de orden n de todas las celdas de una "
     "matriz."),
     BOperClassify::Statistic_);
  void BDatMatMoment::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  BInt	m   = (BInt)Real(Arg(2));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Moment(vec,m);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatCenterMoment);
  DefExtOpr(1, BDatMatCenterMoment, "MatCenterMoment", 2, 2, "Matrix Real",
  "(Matrix mat, Real n)",
  I2("Returns the n-th centered moment of all cells of a matrix.",
     "Devuelve el momento centrado de orden n de todas las celdas de una "
     "matriz."),
     BOperClassify::Statistic_);
  void BDatMatCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  BInt	m   = (BInt)Real(Arg(2));
  const BArray<BDat>& vec = mat.Data();
  contens_ = CenterMoment(vec,m);
}

/*
//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetMoment);
  DefExtOpr(1, BDatSetMoment, "MatMoment", 2, 2, "Set Real",
  I2("(Set set, Real n)","(Set cto, Real n)"),
  I2("Returns the n-th moment of all elements of a set of numbers.",
     "Devuelve el momento de orden n de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetMoment::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set (Arg(1));
  if(!CheckRealElement("SetMoment",set)) { return; }
  BInt	m   = (BInt)Real(Arg(2));
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Moment(vec,m);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetCenterMoment);
  DefExtOpr(1, BDatSetCenterMoment, "SetCenterMoment", 2, 2, "Set Real",
  I2("(Set set, Real n)","(Set cto, Real n)"),
  I2("Returns the n-th centered moment of all elements of a set of numbers.",
     "Devuelve el momento centrado de orden n de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set (Arg(1));
  if(!CheckRealElement("SetCenterMoment",set)) { return; }
  BInt	m   = (BInt)Real(Arg(2));
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = CenterMoment(vec,m);
}
*/

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetKurtosis);
  DefExtOpr(1, BDatSetKurtosis, "SetKurtosis", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the kurtosis of all elements of a set of numbers.",
     "Devuelve la curtosis de todos los elementos de un conjunto de números."),
     BOperClassify::Statistic_);
  void BDatSetKurtosis::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetKurtosis",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Kurtosis(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatKurtosis);
  DefExtOpr(1, BDatMatKurtosis, "MatKurtosis", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the kurtosis of all elements of a matrix.",
     "Devuelve la curtosis de todos los elementos de una matriz."),
     BOperClassify::Statistic_);
  void BDatMatKurtosis::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = Kurtosis(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetAsymmetry);
  DefExtOpr(1, BDatSetAsymmetry, "SetAsymmetry", 1, 1, "Set",
  I2("(Set set)","(Set cto)"),
  I2("Returns the asymmetry coefficient of all elements of a set of numbers.",
     "Devuelve el coeficiente de asimetría de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetAsymmetry::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  if(!CheckRealElement("SetAsymmetry",set)) { return; }
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = AsymmetryCoefficient(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatAsymmetry);
  DefExtOpr(1, BDatMatAsymmetry, "MatAsymmetry", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the asymmetry coefficient of all elements of a matrix.",
     "Devuelve el coeficiente de asimetría de todos los elementos de una "
     "matriz."),
     BOperClassify::Statistic_);
  void BDatMatAsymmetry::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat(Arg(1));
  const BArray<BDat>& vec = mat.Data();
  contens_ = AsymmetryCoefficient(vec);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetMoment);
  DefExtOpr(1, BDatSetMoment, "SetMoment", 2, 2, "Set Real",
  I2("(Set set, Real n)","(Set cto, Real n)"),
  I2("Returns the n-th moment of all elements of a set of numbers.",
     "Devuelve el momento de orden n de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetMoment::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set (Arg(1));
  if(!CheckRealElement("SetMoment",set)) { return; }
  BInt	m   = (BInt)Real(Arg(2));
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = Moment(vec,m);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetCenterMoment);
  DefExtOpr(1, BDatSetCenterMoment, "SetCenterMoment", 2, 2, "Set Real",
  I2("(Set set, Real n)","(Set cto, Real n)"),
  I2("Returns the n-th centered moment of all elements of a set of numbers.",
     "Devuelve el momento centrado de orden n de todos los elementos de un "
     "conjunto."),
     BOperClassify::Statistic_);
  void BDatSetCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set (Arg(1));
  if(!CheckRealElement("SetCenterMoment",set)) { return; }
  BInt	m   = (BInt)Real(Arg(2));
  BArray<BDat> vec(set.Card());
  for(BInt n=0; n<vec.Size(); n++) { vec[n] = Dat(set[n+1]); }
  contens_ = CenterMoment(vec,m);
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetCorrelation);
  DefExtOpr(1, BDatSetCorrelation, "SetCorrelation", 2, 2, "Set Set",
  "(Set s1, Set s2)",
  I2("Returns the correlation coefficient between two sets.",
     "Devuelve el coeficiente de correlación entre dos conjuntos."),
     BOperClassify::Statistic_);
  void BDatSetCorrelation::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s1 = Set(Arg(1));
  BSet& s2 = Set(Arg(2));
  if(!CheckRealElement("SetCorrelation",s1,s2)) { return; }
  BArray<BDat> v1(s1.Card());
  BArray<BDat> v2(s2.Card());
  for(BInt n=0; n<v1.Size(); n++)
  {
    v1[n] = Dat(s1[n+1]);
    v2[n] = Dat(s2[n+1]);
  }
  contens_ = Correlation(v1, v2);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetCovarianze);
  DefExtOpr(1, BDatSetCovarianze, "SetCovarianze", 2, 2, "Set Set",
  "(Set s1, Set s2)",
  I2("Returns the covarianze between two sets.",
     "Devuelve la covarianza entre dos conjuntos."),
     BOperClassify::Statistic_);
  void BDatSetCovarianze::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s1 = Set(Arg(1));
  BSet& s2 = Set(Arg(2));
  if(!CheckRealElement("SetCovarianze",s1,s2)) { return; }
  BArray<BDat> v1(s1.Card());
  BArray<BDat> v2(s2.Card());
  for(BInt n=0; n<v1.Size(); n++)
  {
    v1[n] = Dat(s1[n+1]);
    v2[n] = Dat(s2[n+1]);
  }
  contens_ = Covarianze(v1, v2);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetPearsonCoef);
  DefExtOpr(1, BDatSetPearsonCoef, "SetPearsonCoef", 2, 2, "Set Set",
  "(Set x, Set y)",
  I2("Returns the coefficient of Pearson between two sets.",
     "Devuelve el coeficiente de Pearson entre dos conjuntos de números "
     "reales con el mismo número de datos."
     "El coeficiente de correlación producto o momento r de "
     "Pearson, R, es un índice adimensional acotado entre -1,0 y 1,0 que "
     "refleja el grado de dependencia lineal entre dos conjuntos de datos."),
     BOperClassify::Statistic_);
  void BDatSetPearsonCoef::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s1 = Set(Arg(1));
  BSet& s2 = Set(Arg(2));
  if(!CheckRealElement("SetPearsonCoef",s1,s2)) { return; }
  BArray<BDat> v1(s1.Card());
  BArray<BDat> v2(s2.Card());
  for(BInt n=0; n<v1.Size(); n++)
  {
    v1[n] = Dat(s1[n+1]);
    v2[n] = Dat(s2[n+1]);
  }
  contens_ = PearsonCoefficient(v1, v2);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSetR2Coef);
  DefExtOpr(1, BDatSetR2Coef, "SetR2Coef", 2, 2, "Set Set",
  "(Set x, Set y)",
  I2("Returns the square R coefficient between two sets.",
     "Devuelve el coeficiente R cuadrado entre dos conjuntos de números "
     "reales con el mismo número de datos. "
     "El coeficiente de determinación R2 se puede interpretar como la "
     "proporción de la varianza de y, que puede atribuirse a la varianza "
     "de x. El coeficiente de determinación R2 es el cuadrado del "
     "coeficiente de Pearson. Ver función SetPearsonCoef."),
     BOperClassify::Statistic_);
  void BDatSetR2Coef::CalcContens()
//--------------------------------------------------------------------
{
  BSet& s1 = Set(Arg(1));
  BSet& s2 = Set(Arg(2));
  if(!CheckRealElement("SetR2Coef",s1,s2)) { return; }
  BArray<BDat> v1(s1.Card());
  BArray<BDat> v2(s2.Card());
  for(BInt n=0; n<v1.Size(); n++)
  {
    if((s1[n+1]->Grammar()==GraReal()) &&
       (s2[n+1]->Grammar()==GraReal()))
    {
      v1[n] = Dat(s1[n+1]);
      v2[n] = Dat(s2[n+1]);
    }
    else   
    {
      Error(I2("Elements of arguments for SetR2Coef must be of Real type",
               "Los elementos de los argumentos de SetR2Coef deben ser de tipo Real"));
    }
  }
  contens_ = R2Coefficient(v1, v2);
}


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetHannShapiroWTest);
  DefExtOpr(1, BSetHannShapiroWTest, "HannShapiroWTest", 2, 2, "Set Real",
  I2("(Set set, Real alfa)",
     "(Set conjunto, Real alfa)"),
  I2(".",
     "Contruye el test de normalidad de Hann-Shapiro para una muestra con el "
     "nivel de siginficación alfa. Este test está recomendado si la muestra "
     "consta de menos de 50 elementos."
     "Devuelve un conjunto con tres valores: \n"
     "	1) estadístico b2 de Hann-Shapiro\n"
     "	2) estadístico W de Hann-Shapiro\n"
     "	3) el resultado del test propiamente dicho , es decir, "
     "	   1 si se acepta la hipótesis de normalidad y 0 de lo contrario.\n"
     "Ver ANSI N15.15-1974."),
     BOperClassify::Statistic_);
  void BSetHannShapiroWTest::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set  = Set(Arg(1));
  if(!CheckRealElement("HannShapiroWTest",set)) { return; }
  BDat	alfa = Dat(Arg(2));
  BArray<BDat> vec;
  vec.ReallocBuffer(set.Card());
  for(BInt n=0; n<vec.Size(); n++)
  {
    vec[n] = Dat(set[n+1]);
  }
  BDat	b2;
  BDat W = HannShapiroW(vec,b2);
  BDat Test = HannShapiroWTest(W,vec.Size(),alfa);
  BList* lst	= NIL;
  BList* aux	= NIL;
  LstFastAppend(lst,aux, BContensDat::New("b2", b2,   "b2 Hann-Shapiro statistic"));
  LstFastAppend(lst,aux, BContensDat::New("W",  W,    "W Hann-Shapiro statistic"));
  LstFastAppend(lst,aux, BContensDat::New("t",  Test, "Test boolean result"));
  contens_.RobElement(lst);
}


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetDAgostinoDTest);
  DefExtOpr(1, BSetDAgostinoDTest, "DAgostinoDTest", 2, 2, "Set Real",
  I2("(Set set, Real alfa)",
     "(Set conjunto, Real alfa)"),
  I2(".",
     "Contruye el test de normalidad de DAgostinoD para una muestra con el "
     "nivel de siginficación alfa. Este test está recomendado si la muestra "
     "consta de al menos 50 elementos."
     "Devuelve un conjunto con tres valores: \n"
     "	1) estadístico T de D'Agostino\n"
     "	2) estadístico D de D'Agostino\n"
     "	3) el resultado del test propiamente dicho , es decir, "
     "	   1 si se acepta la hipótesis de normalidad y 0 de lo contrario.\n"
     "Ver ANSI N15.15-1974."),
     BOperClassify::Statistic_);
  void BSetDAgostinoDTest::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set  = Set(Arg(1));
  BDat	alfa = Dat(Arg(2));
  if(!CheckRealElement("DAgostinoDTest",set)) { return; }
  BArray<BDat> vec;
  vec.ReallocBuffer(set.Card());
  for(BInt n=0; n<vec.Size(); n++)
  {
    vec[n] = Dat(set[n+1]);
  }
  BDat T;
  BDat D = DAgostinoD(vec, T);
  BDat Test = DAgostinoDTest(D,vec.Size(),alfa);
  BList* lst	= NIL;
  BList* aux	= NIL;
  LstFastAppend(lst,aux, BContensDat::New("T", T,    "T D'Agostino statistic"));
  LstFastAppend(lst,aux, BContensDat::New("D", D,    "D D'Agostino statistic"));
  LstFastAppend(lst,aux, BContensDat::New("t", Test, "Test boolean value"));
  contens_.RobElement(lst);
}



