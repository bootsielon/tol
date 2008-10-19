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
#include <tol/tol_bout.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bstat.h>
#include <tol/tol_btest.h>

#ifdef __USE_TC__
#  include <tol/tol_bctsrgrav.h>
#endif

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
BDatCStatistic::BDatCStatistic(BList* arg)
    : BDatTemporary(arg), cser_(Arg(1))
{
    if(arg)
    {
	if(Arg(2)) { ini_ = CTime(Arg(2)); }

	if(!ini_.isNormal() || (ini_<CTsrR(cser_).FirstCTime()))
	{ ini_=CTsrR(cser_).FirstCTime(); }
	
	if(!ini_.isNormal()) 
	{ 
	    ini_ = BCTime::fillNew(CTsrR(cser_).Dating()->getGranul(), 
				   LV, 1, BCTime::Now().year());
	}
	
	if(Arg(3)) { fin_ = CTime(Arg(3)); }
	
	if(!fin_.isNormal() || (fin_>CTsrR(cser_).LastCTime()))
	{ fin_ = CTsrR(cser_).LastCTime(); }
	
	if(!fin_.isNormal()) 
	{ 
	    fin_ = BCTime::fillNew(CTsrR(cser_).Dating()->getGranul(), 
				   HV, 1, BCTime::Now().year());
	}
    }

    PutArgs(cser_, ini_, fin_);
}

//--------------------------------------------------------------------
void BDatCStatistic::PutArgs(BSyntaxObject* cser,
			     const BCTime& ini,
			     const BCTime& fin)
{
    if(!cser) { return; }

    cser_ = cser;
    BUserCTimeSeries* cseries = CTsr(cser_);
    if(!cseries->Dating()) { return; }

    if(cser && cseries->Dating())
    {
	ini_ = cseries->Dating()->nonSmaller(ini);
	fin_ = fin;
	cseries->GetData(vec_, ini_, fin_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatFirst, "CFirstS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries ser [, CTime from, CTime until])",
	     "(CSeries ser [, CTime desde, CTime hasta])"),
	  I2("Returns the first value of a serie between two dates. "
	     "If they do not appear, it is assumed the calculation with the "
	     "first and last dates of the series. Finally, if the series "
	     "lacks dates, is calculated with the dates by defect.",
	     "Devuelve el primer valor de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatFirst::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = vec_[0];
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatLast, "CLastS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the last value of a serie between two dates. If they "
	     "do not appear, it is assumed the calculation with the first "
	     "and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve el último valor de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatLast::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = vec_[vec_.Size()-1];
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatCount, "CCountS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the dates number of a serie between two dates. If they "
	     "do not appear, it is assumed the calculation with the first "
	     "and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect. ",
	     "Devuelve el número de datos de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatCount::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = vec_.Size();
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatMax, "CMaxS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the maximum value of a serie between two dates. If they "
	     "do not appear, it is assumed the calculation with the first "
	     "and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve el máximo valor de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatMax::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Maximum(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatMin, "CMinS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the minimum value of a serie between two dates. If they "
	     "do not appear, it is assumed the calculation with the first "
	     "and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve el mínimo valor de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatMin::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Minimum(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatSum, "CSumS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the sum of a serie between two dates. If they do not "
	     "appear, it is assumed the calculation with the first and "
	     "last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve la suma de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatSum::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Sum(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatAverage, "CAvrS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the average of a serie between two dates. If they do "
	     "not appear, it is assumed the calculation with the first and "
	     "last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve la media de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatAverage::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Average(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatVarianze,"CVarS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the varianze of a serie between two dates. If they do "
	     "not appear, it is assumed the calculation with the first and "
	     "last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve la varianza de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatVarianze::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Varianze(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatStDs, "CStDsS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the standard deviation of a serie between two dates. If "
	     "they do not appear, it is assumed the calculation with the first"
	     " and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve la desviación tipica de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatStDs::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = StandardDeviation(vec_);
    }
}
#endif

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatSubSampleAverage, "CSubSampleAvrS", 
	  1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the average of a serie filtered by anomalies greater "
	     "than 3 sigmas between two dates. If they do not appear, it is "
	     "assumed the calculation with the first and last dates of the "
	     "series. Finally, if the series lacks dates, is calculated "
	     "with the dates by defect.",
	     "Devuelve la media filtrada de anómalos mayores de 3 sigmas de "
	     "una serie entre dos fechas. " 
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatSubSampleAverage::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = SubSampleAverage(Average,vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatSubSampleVarianze,"CSubSampleVarS", 
	  1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the varianze of a serie filtered by anomalies greater "
	     "than 3 sigmas between two dates. If they do not appear, it is "
	     "assumed the calculation with the first and last dates of the "
	     "series. Finally, if the series lacks dates, is calculated with "
	     "the dates by defect.",
	     "Devuelve la varianza filtrada de anómalos mayores de 3 sigmas "
	     "de una serie entre dos fechas. Si no aparecen, se asume el "
	     "cálculo con las fechas primera y última de la serie. Por "
	     "último, si la serie carece de fechas, se calcula con las "
	     "fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatSubSampleVarianze::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = SubSampleAverage(Varianze,vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatSubSampleStDs, "CSubSampleStDsS", 
	  1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the standard deviation of a serie filtered by "
	     "anomalies greater than 3 sigmas between two dates. If they "
	     "do not appear, it is assumed the calculation with the first "
	     "and last dates of the series. Finally, if the series lacks "
	     "dates, is calculated with the dates by defect.",
	     "Devuelve la desviación tipica filtrada de anómalos mayores "
	     "de 3 sigmas de una serie entre dos fechas. Si no aparecen, "
	     "se asume el cálculo con las fechas primera y última de la "
	     "serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatSubSampleStDs::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Sqrt(SubSampleAverage(Varianze,vec_));
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatAsimetry, "CAsimetryS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the asymmetry coefficient of a serie between two dates. "
	     "If they do not appear, it is assumed the calculation with the "
	     "first and last dates of the series. Finally, if the series "
	     "lacks dates, is calculated with the dates by defect.",
	     "Devuelve el coeficiente de asimetría de una serie entre dos "
	     "fechas. Si no aparecen, se asume el cálculo con las fechas "
	     "primera y última de la serie. Por último, si la serie carece "
	     "de fechas, se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatAsimetry::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = AsymmetryCoefficient(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatKurtosis, "CKurtosisS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the kurtosis of a serie between two dates. If they do "
	     "not appear, it is assumed the calculation with the first and "
	     "last dates of the series. Finally, if the series lacks dates, "
	     "is calculated with the dates by defect.",
	     "Devuelve el coeficiente de curtosis de una serie entre dos "
	     "fechas. Si no aparecen, se asume el cálculo con las fechas "
	     "primera y última de la serie. Por último, si la serie carece "
	     "de fechas, se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatKurtosis::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Kurtosis(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatMedian, "CMedianS", 1, 3, "CSeries CTime CTime",
	  I2("(CSeries cser [, CTime from, CTime until])",
	     "(CSeries cser [, CTime desde, CTime hasta])"),
	  I2("Returns the median of a serie between two dates. If they do "
	     "not appear, it is assumed the calculation with the first and "
	     "last dates of the series. Finally, if the series lacks dates, "
	     "is calculated with the dates by defect.",
	     "Devuelve la mediana de una serie entre dos fechas. "
	     "Si no aparecen, se asume el cálculo con las fechas primera "
	     "y última de la serie. Por último, si la serie carece de fechas, "
	     "se calcula con las fechas por defecto."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatMedian::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	contens_ = Median(vec_);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatQuantile, "CQuantileS",  4, 4, 
	  "CSeries CTime CTime Real",
	  I2("(CSeries cser, CTime from, CTime until, Real p)",
	     "(CSeries cser, CTime desde, CTime hasta, Real p)"),
	  I2("Returns the quantile of a given probability of a serie "
	     "between two CTimes. ",
	     "Devuelve el cuantil de probabilidad p de una serie entre dos "
	     "CTimes. "),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatQuantile::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	BDat q   = Dat(Arg(4));
	contens_ = Quantile(vec_,q);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatMoment, "CMomentS", 4, 4, "CSeries CTime CTime Real",
	  I2("(CSeries cser, CTime from, CTime until, Real n)",
	     "(CSeries cser, CTime desde, CTime hasta, Real n)"),
	  I2("Returns the moment of a given order of a series between two "
	     "CTimes. ",
	     "Devuelve el momento de orden n de una serie entre dos CTimes."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatMoment::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	BInt  m  = (BInt)Real(Arg(4));
	contens_ = Moment(vec_,m);
    }
}
#endif // _USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
DefExtOpr(1, BDatCStatCenterMoment, "CCenterMomentS", 4, 4,
	  "CSeries CTime CTime Real",
	  I2("(CSeries cser, CTime from, CTime until, Real n)",
	     "(CSeries cser, CTime desde, CTime hasta, Real n)"),
	  I2("Returns the center moment of a given order of a series between "
	     "two CTimes.", "Devuelve el momento centrado de orden n de "
	     "una serie entre dos CTimes."),
	  BOperClassify::Statistic_);
//--------------------------------------------------------------------
void BDatCStatCenterMoment::CalcContens()
{
    if(cser_ && CTsrR(cser_).Dating())
    {
	BInt  m  = (BInt)Real(Arg(4));
	contens_ = CenterMoment(vec_,m);
    }
}
#endif // __USE_TC__

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

#ifdef __USE_TC__
//--------------------------------------------------------------------
BDat CStatFirst(BSyntaxObject* cser)
{ return(BDatCStatFirst (NCons(cser)).Contens()); }

BDat CStatLast(BSyntaxObject* cser)
{ return(BDatCStatLast (NCons(cser)).Contens()); }

BDat CStatCount(BSyntaxObject* cser)
{ return(BDatCStatCount (NCons(cser)).Contens()); }

BDat CStatMax(BSyntaxObject* cser)
{ return(BDatCStatMax (NCons(cser)).Contens()); }

BDat CStatMin(BSyntaxObject* cser)
{ return(BDatCStatMin (NCons(cser)).Contens()); }

BDat CStatSum(BSyntaxObject* cser)
{ return(BDatCStatSum (NCons(cser)).Contens()); }

BDat CStatAverage(BSyntaxObject* cser)
{ return(BDatCStatAverage (NCons(cser)).Contens()); }

BDat CStatStDs(BSyntaxObject* cser)
{ return(BDatCStatStDs (NCons(cser)).Contens()); }

BDat CStatVarianze(BSyntaxObject* cser)
{ return(BDatCStatVarianze (NCons(cser)).Contens()); }

BDat CStatAsimetry(BSyntaxObject* cser)
{ return(BDatCStatAsimetry (NCons(cser)).Contens()); }

BDat CStatKurtosis(BSyntaxObject* cser)
{ return(BDatCStatKurtosis (NCons(cser)).Contens()); }

BDat CStatMedian(BSyntaxObject* cser)
{ return(BDatCStatMedian (NCons(cser)).Contens()); }

BDat CStatMoment(BSyntaxObject* cser, int order)
{
    BUserDat*   d  = new BContensDat(BDat(order));
    BUserCTime* f  = new BContensCTime(CTsr(cser)->FirstCTime());
    BUserCTime* l  = new BContensCTime(CTsr(cser)->LastCTime());
    BList*       arg = LstStack(NIL,cser,f,l,d,NIL);
    BDatStatMoment mom(arg);
    return(mom.Contens());
}

BDat CStatCenterMoment(BSyntaxObject* cser, int order)
{
    BUserDat*  d   = new BContensDat(BDat(order));
    BUserCTime* f  = new BContensCTime(CTsr(cser)->FirstCTime());
    BUserCTime* l  = new BContensCTime(CTsr(cser)->LastCTime());
    BList*       arg = LstStack(NIL,cser,f,l,d,NIL);
    BDatStatCenterMoment mom(arg);
    return(mom.Contens());
}

BDat CStatQuantile(BSyntaxObject* cser, BDat q)
{
    BUserDat*  d   = new BContensDat(q);
    BUserCTime* f  = new BContensCTime(CTsr(cser)->FirstCTime());
    BUserCTime* l  = new BContensCTime(CTsr(cser)->LastCTime());
    BList*       arg = LstStack(NIL,cser,f,l,d,NIL);
    BDatStatQuantile mom(arg);
    return(mom.Contens());
}
#endif // __USE_TC__

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatBoxPierceLjung);
DefExtOpr(1, BDatBoxPierceLjung, "BoxPierceLjung", 4, 4,
	  "Serie Real Real Real",
	  "(Serie ser, Real m, Real p, Real q)",
	  I2("Returns the Box-Pierce-Ljung statistic of a serie with m "
	     "autocorrelations, p AR-parameters and q MA-parameters",
	     "Devuelve el estadístico de Box-Pierce-Ljung de una serie con m "
	     "autocorrelaciones, p parámetros AR y q parámetros MA"),
	  BOperClassify::Sthocastic_);
void BDatBoxPierceLjung::CalcContens()
//--------------------------------------------------------------------
{
    BSerieTable table;
    table.AddSerie(Tsr(Arg(1)));
    table.Fill();
    BInt m = (BInt)Real(Arg(2));
    BInt p = (BInt)Real(Arg(3));
    BInt q = (BInt)Real(Arg(4));
    contens_ = BoxPierceLjung(table.Data().Data(), m, p, q);
}

#ifdef __USE_TC__
//--------------------------------------------------------------------
/*
DeclareContensClass(BDat, BDatTemporary, BDatCBoxPierceLjung);
DefExtOpr(1, BDatCBoxPierceLjung, "CBoxPierceLjung", 4, 4,
	  "CSeries Real Real Real",
	  "(CSeries cser, Real m, Real p, Real q)",
	  I2("Returns the Box-Pierce-Ljung statistic of a series with m "
	     "autocorrelations, p AR-parameters and q MA-parameters",
	     "Devuelve el estadístico de Box-Pierce-Ljung de una serie con m "
	     "autocorrelaciones, p parámetros AR y q parámetros MA"),
	  BOperClassify::Sthocastic_);
*/
//--------------------------------------------------------------------
/*
void BDatCBoxPierceLjung::CalcContens()
{
    BSet s; s.RobElement(NCons(Arg(1)));
    int m = (BInt)Real(Arg(2));
    int p = (BInt)Real(Arg(3));
    int q = (BInt)Real(Arg(4));
    BCSeriesTable table(s);
    contens_ = BoxPierceLjung(table.Data().Data(), m, p, q);
}
*/
#endif // __USE_TC__


