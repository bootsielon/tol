/* datgsrst.cpp: Real Grammar's Time Series Statistics functions
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
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bstat.h>
#include <tol/tol_btest.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraSeriesStatistic() { return(BTRUE); }


//--------------------------------------------------------------------
BDatStatistic::BDatStatistic(BList* arg)
//--------------------------------------------------------------------
    : BDatTemporary(arg), ser_(Arg(1))
{
    if(arg)
    {
	if(Arg(2))
	{ ini_ = Date(Arg(2)); }

	if(!ini_.HasValue() || (ini_<TsrR(ser_).FirstDate()))
	{ ini_=TsrR(ser_).FirstDate(); }
	
	if(!ini_.HasValue()) 
	{ ini_= BDate::DefaultFirst(); }
	
	if(Arg(3))           
	{ fin_ = Date(Arg(3)); }
	
	if(!fin_.HasValue() || (fin_>TsrR(ser_).LastDate()))
	{ fin_=TsrR(ser_).LastDate(); }
	
	if(!fin_.HasValue()) 
	{ fin_= BDate::DefaultLast(); }
    }
    PutArgs(ser_,ini_,fin_);
}


//--------------------------------------------------------------------
void BDatStatistic::PutArgs(BSyntaxObject* ser,
			    const BDate& ini,
			    const BDate& fin)
//--------------------------------------------------------------------
{
    if(!ser) { return; }
    ser_=ser;
    BUserTimeSerie* serie = Tsr(ser_);
    if(!serie->Dating()) { return; }
    if(ser && serie->Dating())
    {
	ini_=serie->Dating()->FirstNoLess (ini);
	fin_=serie->Dating()->FirstNoGreat(fin);
  if(!ini_.HasValue()) { ini_ = BDate::DefaultFirst(); }
  if(!fin_.HasValue()) { fin_ = BDate::DefaultLast (); }
	//Std(BText("\nPutArgs(")+ini+","+fin+") -> ("+ini_+","+fin_+") ");
	serie->GetData(vec_,ini_,fin_);
    }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatFirst,	"FirstS", 1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the first value of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el primer valor de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatFirst::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = vec_[0]; //TsrR(ser_)[ini_];
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatLast,	"LastS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the last value of a serie between two dates."
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el último valor de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatLast::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = vec_[vec_.Size()-1]; //TsrR(ser_)[fin_];
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatCount,	"CountS", 1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the dates number of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect. ",
     "Devuelve el número de datos de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatCount::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    if(TsrR(ser_).FirstDate()>TsrR(ser_).LastDate())
    {
      contens_ = 0;
    }
    else
    {
      contens_ = vec_.Size(); //1 + TsrR(ser_).Dating()->Difference(ini_, fin_);
    }
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatMax,	"MaxS", 1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the maximum value of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el máximo valor de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatMax::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Maximum(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatMin,	"MinS", 1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the minimum value of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el mínimo valor de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatMin::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Minimum(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatSum,	"SumS", 1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the sum of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la suma de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatSum::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Sum(vec_);
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatAverage, "AvrS",	  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the average of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la media de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatAverage::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Average(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatVarianze,"VarS",	  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the varianze of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la varianza de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatVarianze::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Varianze(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatStDs,	"StDsS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the standard deviation of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la desviación tipica de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatStDs::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = StandardDeviation(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatSubSampleAverage, "SubSampleAvrS",   1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the average of a serie filtered by anomalies greater than 3 sigmas "
     "between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la media filtrada de anómalos mayores de 3 sigmas de "
	 "una serie entre dos fechas. " 
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatSubSampleAverage::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = SubSampleAverage(Average,vec_);
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatSubSampleVarianze,"SubSampleVarS",   1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the varianze of a serie filtered by anomalies greater than 3 sigmas "
     "between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la varianza filtrada de anómalos mayores de 3 sigmas de "
	 "una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatSubSampleVarianze::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = SubSampleAverage(Varianze,vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatSubSampleStDs,	 "SubSampleStDsS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the standard deviation of a serie filtered by anomalies greater "
     "than 3 sigmas between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la desviación tipica filtrada de anómalos mayores de 3 sigmas de "
	 "una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatSubSampleStDs::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Sqrt(SubSampleAverage(Varianze,vec_));
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatAsimetry,    "AsimetryS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the asymmetry coefficient of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el coeficiente de asimetría de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatAsimetry::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = AsymmetryCoefficient(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatKurtosis,    "KurtosisS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the kurtosis of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve el coeficiente de curtosis de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatKurtosis::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Kurtosis(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatMedian,	  "MedianS",  1, 3, "Serie Date Date",
  I2("(Serie ser [, Date from, Date until])",
     "(Serie ser [, Date desde, Date hasta])"),
  I2("Returns the median of a serie between two dates. "
     "If they do not appear, it is assumed the calculation with the "
     "first and last dates of the series. Finally, if the series lacks "
     "dates, is calculated with the dates by defect.",
     "Devuelve la mediana de una serie entre dos fechas. "
     "Si no aparecen, se asume el cálculo con las fechas primera "
     "y última de la serie. Por último, si la serie carece de fechas, "
     "se calcula con las fechas por defecto."),
     BOperClassify::Statistic_);
  void BDatStatMedian::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    contens_ = Median(vec_);
  }
}


//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatQuantile, "QuantileS",  4, 4, "Serie Date Date Real",
  I2("(Serie ser, Date from, Date until, Real p)",
     "(Serie ser, Date desde, Date hasta, Real p)"),
  I2("Returns the quantile of a given probability of a serie between two "
     "dates. ",
     "Devuelve el cuantil de probabilidad p de una serie entre dos "
     "fechas. "),
     BOperClassify::Statistic_);
  void BDatStatQuantile::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    BDat q   = Dat(Arg(4));
    contens_ = Quantile(vec_,q);
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1, BDatStatMoment,	  "MomentS",  4, 4, "Serie Date Date Real",
  I2("(Serie ser, Date from, Date until, Real n)",
     "(Serie ser, Date desde, Date hasta, Real n)"),
  I2("Returns the moment of a given order of a serie between two dates. ",
     "Devuelve el momento de orden n de una serie entre dos fechas. "),
     BOperClassify::Statistic_);
  void BDatStatMoment::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    BInt  m  = (BInt)Real(Arg(4));
    contens_ = Moment(vec_,m);
  }
}

//--------------------------------------------------------------------
  DefExtOpr(1,BDatStatCenterMoment,"CenterMomentS",4,4,"Serie Date Date Real",
  I2("(Serie ser, Date from, Date until, Real n)",
     "(Serie ser, Date desde, Date hasta, Real n)"),
  I2("Returns the center moment of a given order of a serie between two dates. ",
     "Devuelve el momento centrado de orden n de una serie entre dos fechas."),
     BOperClassify::Statistic_);
  void BDatStatCenterMoment::CalcContens()
//--------------------------------------------------------------------
{
  if(ser_ && TsrR(ser_).Dating())
  {
    BInt  m  = (BInt)Real(Arg(4));
    contens_ = CenterMoment(vec_,m);
  }
}

//--------------------------------------------------------------------
BDat StatFirst(BSyntaxObject* ser)
{ return(BDatStatFirst (NCons(ser)).Contens()); }

BDat StatLast(BSyntaxObject* ser)
{ return(BDatStatLast (NCons(ser)).Contens()); }

BDat StatCount(BSyntaxObject* ser)
{ return(BDatStatCount (NCons(ser)).Contens()); }

BDat StatMax(BSyntaxObject* ser)
{ return(BDatStatMax (NCons(ser)).Contens()); }

BDat StatMin(BSyntaxObject* ser)
{ return(BDatStatMin (NCons(ser)).Contens()); }

BDat StatSum(BSyntaxObject* ser)
{ return(BDatStatSum (NCons(ser)).Contens()); }

BDat StatAverage(BSyntaxObject* ser)
{ return(BDatStatAverage (NCons(ser)).Contens()); }

BDat StatStDs(BSyntaxObject* ser)
{ return(BDatStatStDs (NCons(ser)).Contens()); }

BDat StatVarianze(BSyntaxObject* ser)
{ return(BDatStatVarianze (NCons(ser)).Contens()); }

BDat StatAsimetry(BSyntaxObject* ser)
{ return(BDatStatAsimetry (NCons(ser)).Contens()); }

BDat StatKurtosis(BSyntaxObject* ser)
{ return(BDatStatKurtosis (NCons(ser)).Contens()); }

BDat StatMedian(BSyntaxObject* ser)
{ return(BDatStatMedian (NCons(ser)).Contens()); }

BDat StatMoment(BSyntaxObject* ser, BInt order)
{
    BUserDat*  d   = new BContensDat(BDat(order));
    BUserDate* f   = new BContensDate(Tsr(ser)->FirstDate());
    BUserDate* l   = new BContensDate(Tsr(ser)->LastDate());
    BList*       arg = LstStack(NIL,ser,f,l,d,NIL);
    BDatStatMoment mom(arg);
    return(mom.Contens());
}

BDat StatCenterMoment(BSyntaxObject* ser, BInt order)
{
    BUserDat*  d   = new BContensDat (BDat(order));
    BUserDate* f   = new BContensDate(Tsr(ser)->FirstDate());
    BUserDate* l   = new BContensDate(Tsr(ser)->LastDate());
    BList*       arg = LstStack(NIL,ser,f,l,d,NIL);
    BDatStatCenterMoment mom(arg);
    return(mom.Contens());
}
BDat StatQuantile(BSyntaxObject* ser, BDat q)
{
    BUserDat*  d   = new BContensDat (q);
    BUserDate* f   = new BContensDate(Tsr(ser)->FirstDate());
    BUserDate* l   = new BContensDate(Tsr(ser)->LastDate());
    BList*       arg = LstStack(NIL,ser,f,l,d,NIL);
    BDatStatQuantile mom(arg);
    return(mom.Contens());
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatBoxPierceLjungACF);
DefExtOpr(1, BDatBoxPierceLjungACF, "BoxPierceLjungACF", 3, 3,
    "Matrix Real Real", 
    "(Matrix acf, Real m, Real s)",
    I2("Returns the Box-Pierce-Ljung statistic for first ´m´  "
       "autocorrelations in vector ´acf´ from a time series of length ´s´",
       "Devuelve el estadístico de Box-Pierce-Ljung para las ´s´ primeras "
       "autocorrelaciones del vector ´acf´ de una serie temporal de "
       "longitud ´s´."),
    BOperClassify::Sthocastic_);
void BDatBoxPierceLjungACF::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& ACF = Mat(Arg(1)); 
  BInt m = (BInt)Real(Arg(2)); 
  BInt s = (BInt)Real(Arg(3));
  contens_ = BoxPierceLjungACF(ACF.Data(), m, s);
}
