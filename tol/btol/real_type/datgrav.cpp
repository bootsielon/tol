/* datgrav.cpp: Real Grammar's varous functions
                GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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
#include <tol/tol_bdir.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#ifdef __USE_TC__
#  include <tol/tol_bctmigra.h>
#  include <tol/tol_bctmsgra.h>
#  include <tol/tol_bctsrgra.h>
#endif
#include <tol/tol_blinalg.h>
#include <tol/tol_bfibonac.h>
#include <tol/tol_barma.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_gsl.h>
#include <gsl/gsl_integration.h>

#include "llkarma.h"

#if defined(_MSC_VER)
#  include <windows.h>
#  include <winbase.h>
#  include <windows.h>
#endif


// BDat	  BMat ::nullValue_ = 0;


//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraVarious() { return(BTRUE); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatShowErrors);
  DefExtOpr(1, BDatShowErrors, "Show", 1, 2, "Real Text",
  I2("(Real show [, Text type=\"ALL\"])",
     "(Real mostrar [, Text type=\"ALL\"])"),
  I2(".",
     "A partir de la llamada a esta función el sistem mostrará o no los "
     "mensajes del tipo dado según sea cierto o no el parámetro <mostrar>."
     "Los tipos posibles son : ALL, STANDARD, ERROR, WARNING, USER, TRACE"
     "Devuelve el estado anterior a la llamada."),
     BOperClassify::TimeAlgebra_);
  void BDatShowErrors::CalcContens()
//--------------------------------------------------------------------
{
  BBool sh   = (BBool)Real(Arg(1));
  BText type = ToUpper(Text(Arg(2)));
  BBool all  = (type=="ALL");
  BBool err  = all || (type=="ERROR");
  BBool war  = all || (type=="WARNING");
  BBool inf  = all || (type=="USER");
  BBool std  = all || (type=="STANDARD");
  BBool trc  = all || (type=="TRACE");
  BBool oldSh = BTRUE;
  if(std)
  {
    oldSh = oldSh && BOut::StdTerm();
    BOut::PutStdTerm(sh);
    BOut::PutStdHci(sh);
  }
  if(err)
  {
    oldSh = oldSh && BOut::ErrorTerm();
    BOut::PutErrorTerm(sh);
    BOut::PutErrorHci(sh);
  }
  if(war)
  {
    oldSh = oldSh && BOut::WarningTerm();
    BOut::PutWarningTerm(sh);
    BOut::PutWarningHci(sh);
  }
  if(trc)
  {
    oldSh = oldSh && BOut::TraceTerm();
    BOut::PutTraceTerm(sh);
    BOut::PutTraceHci(sh);
  }
  if(inf)
  {
    oldSh = oldSh && BOut::InfoTerm();
    BOut::PutInfoTerm(sh);
    BOut::PutInfoHci(sh);
  }
  contens_ = oldSh;
}

#ifdef __USE_TC__

//- CTime functions --------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatJulianDay);
DefExtOpr(1, BDatJulianDay, "cJulianDay", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the modified Julian day number corresponding to date "
	     "given as a CTime tmi parameter. Every calendar date has "
	     "a modified Julian day number. The day number increases "
	     "by 1 every day. Day number 0 is 17 November 1858. Day number "
	     "51604 is 1 March 2000.\n"
	     "Parameter CTime tmi must have at least a DAILY granularity.",
	     "Devuelve el día Juliano modificado que corresponde con la "
	     "fecha dada como parámetro de tipo CTime. A cada fecha del "
	     "calendario le corresponde un número Juliano modificado. El "
	     "número se incrementa una unidad cada día. El día 0 corresponde "
	     "con el 17 de Noviembre de 1858. El día 51604 corresponde con "
	     "el 1 de Marzo de 2000.\nEl parámetro CTime tmi debe tener al "
	     "menos granularidad Diaria (DAILY)."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatJulianDay::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>DAYS) {
	uTmi.setGranul(DAYS);
	Std(I2("To perform the result, CTime granularity has changed to "
	       "DAILY\n","Se ha cambiado la granularidad a DAILY para poder "
	       "obtener el resultado\n"));
    }
    BDat result(static_cast<double>(uTmi.julianDay()));
    contens_ = result;
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatIsLeapYear);
DefIntOpr(1, BDatIsLeapYear, "cIsLeapYear", 1, 1,
	  I2("(Real year)","(Real año)"),
	  I2("Returns 1 if the given year parameter corresponds to a leap "
	     "year, otherwise returns 0.",
	     "Devuelve 1 si el año especificado como parámetro corresponde "
	     "a un año bisiesto, en otro caso devuelve 0."),
	  BOperClassify::System_);
//--------------------------------------------------
void BDatIsLeapYear::CalcContens()
{
    if(BCTime::isLeapYear(static_cast<long int>(Real(Arg(1))))) 
	 contens_ = 1.0;
    else contens_ = 0.0;
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetYear);
DefExtOpr(1, BDatGetYear, "cGetYear", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi year","Devuelve el año del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetYear::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    BDat result(static_cast<double>(uTmi.year()));
    contens_ = result;
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetMonth);
DefExtOpr(1, BDatGetMonth, "cGetMonth", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi month","Devuelve el mes del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetMonth::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>MONTHS) // Could be a YEARS Time
	contens_ = BDat::Unknown();
    else contens_ = BDat(static_cast<double>(uTmi.month()));
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetDay);
DefExtOpr(1, BDatGetDay, "cGetDay", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi day","Devuelve el día del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetDay::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>DAYS) // Could be a MONTHS Time
	contens_ = BDat::Unknown();
    else contens_ = BDat(static_cast<double>(uTmi.day()));
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetHour);
DefExtOpr(1, BDatGetHour, "cGetHour", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi hour","Devuelve la hora del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetHour::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>HOURS) // Could be a DAYS Time
	contens_ = BDat::Unknown();
    else contens_ = BDat(static_cast<double>(uTmi.hour()));
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetMinute);
DefExtOpr(1, BDatGetMinute, "cGetMinute", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi minute","Devuelve el minuto del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetMinute::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>MINUTES) // Could be a HOURS Time
	contens_ = BDat::Unknown();
    else contens_ = BDat(static_cast<double>(uTmi.minute()));
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatGetSecond);
DefExtOpr(1, BDatGetSecond, "cGetSecond", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the tmi second","Devuelve el segundo del tmi"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatGetSecond::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    if(uTmi.getGranul()>SECONDS) // Could be a MINUTES Time
	contens_ = BDat::Unknown();
    else contens_ = BDat(static_cast<double>(uTmi.second()));
}

//--------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatIsEmpty);
DefExtOpr(1, BDatIsEmpty, "cIsEmpty", 1, 1, "CTimeSet",
	  "(CTimeSet ctms)",
	  I2("Returns 1 whether CTimeSet ctms is empty",
	     "Devuelve 1 si el CTimeSet ctms está vacío"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------
void BDatIsEmpty::CalcContens()
{
    BUserCTimeSet *ctms = CTms(Arg(1));
    if((ctms->getType()==CTmsVoid) ||
       (ctms->succ(BCTime::Begin())==BCTime::End()))
	contens_ = 1;
    else contens_ = 0;
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatBelongCTimeSet);
DefExtOpr(1, BDatBelongCTimeSet, "cBelong", 2, 2, "CTime CTimeSet",
	  I2("(CTime tmi, CTimeSet timeSet)",
	     "(CTime tmi, CTimeSet timeSet)"),
	  I2("Returns true if tmi belongs to timeSet.",
	     "Devuelve verdadero si tmi pertenece al conjunto temporal."),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------
void BDatBelongCTimeSet::CalcContens()
{ contens_  = CTms(Arg(2))->includes(CTime(Arg(1))); }

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatDistance);
DefExtOpr(1, BDatDistance, "cDistance", 2, 2, "CTime CTime",
	  "(CTime tmi1, CTime tmi2)",
	  I2("Distance between both CTime values in their lowest granularity.",
	     "Distancia entre los dos valores CTime en su granularidad más "
	     "baja"),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------
void BDatDistance::CalcContens()
{ 
    BCTime uTmi1 = CTime(Arg(1));
    BCTime uTmi2 = CTime(Arg(2));

    // Granularity adjustment
    if(uTmi1.getGranul() != uTmi2.getGranul()) 
    {
	if(uTmi1.getGranul() < uTmi2.getGranul()) 
	    uTmi2.setGranul(uTmi1.getGranul());
	else uTmi1.setGranul(uTmi2.getGranul());
    }

    contens_  = uTmi1.distanceTo(uTmi2); 
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatDistanceIn);
DefExtOpr(1, BDatDistanceIn, "cDistanceIn", 3, 3, "CTimeSet CTime CTime",
	  "(CTimeSet ctms, CTime tmi1, CTime tmi2)",
	  I2("Distance between both CTime values in the given CTimeSet.",
	     "Distancia entre los dos valores CTime en el CTimeSet dado."),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------
void BDatDistanceIn::CalcContens()
{ 
    BUserCTimeSet *uCtms = CTms(Arg(1));
    BCTime uTmi1 = CTime(Arg(2));
    BCTime uTmi2 = CTime(Arg(3));

    if(uTmi1.getGranul() < uTmi2.getGranul())
	uTmi2.setGranul(uTmi1.getGranul());
    else if(uTmi1.getGranul()<uTmi2.getGranul())
	uTmi1.setGranul(uTmi2.getGranul());

    contens_  = uCtms->distance(uTmi1, uTmi2);
}

#endif /* __USE_TC__ */
	     
//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDateToIndex);
  DefExtOpr(1, BDatDateToIndex, "DateToIndex", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the number of elapsed days and fraction from y1900m1d1 until "
     "a date.",
     "Devuelve el número de días y fracción transcurridos desde y1900m1d1 "
     "hasta una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatDateToIndex::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Date(Arg(1)).Index(); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBelongTimeSet);
  DefExtOpr(1, BDatBelongTimeSet, "Belong", 2, 2, "Date TimeSet",
  I2("(Date date, TimeSet timeSet)",
     "(Date fecha, TimeSet conjuntoTemporal)"),
  I2("Returns true if a date belongs to a temporary set.",
     "Devuelve verdadero si una fecha pertenece a un conjunto temporal."),
     BOperClassify::SetAlgebra_);
  void BDatBelongTimeSet::CalcContens()
//--------------------------------------------------------------------
{ contens_  = Tms(Arg(2))->Contain(Date(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBelong);
  DefExtOpr(1, BDatBelong, "<:", 2, 2, "Anything Set",
  "x <: A {Anything x, Set A}",
  I2("Returns true if x is an element of A.",
     "Devuelve verdadero si x es un elemento de A."),
     BOperClassify::SetAlgebra_);
  void BDatBelong::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Set(Arg(2)).Includes(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOfSerie);
  DefExtOpr(1, BDatOfSerie, "SerDat", 2, 2, "Serie Date",
  I2("(Serie ser, Date date)",
     "(Serie ser, Date fecha)"),
  I2("Returns the value of a serie in a a date.",
     "Devuelve el valor de una serie en una fecha."),
     BOperClassify::TimeSeriesAlgebra_);
  void BDatOfSerie::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSerie& ser = *Tsr(Arg(1));
  BDate		  dte =	 Date(Arg(2));
  BDat		  dat =	 ser[dte];
  contens_ = (dat);
}

#ifdef __USE_TC__
//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatOfCSeries);
DefExtOpr(1, BDatOfCSeries, "CSerDat", 2, 2, "CSeries CTime",
	  "(CSeries cser, CTime tmi)",
	  I2("Returns the value of a CSeries in a given CTime.",
	     "Devuelve el valor de una CSeries en una fecha dada."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BDatOfCSeries::CalcContens()
{
    BUserCTimeSeries& cser = *CTsr(Arg(1));
    BCTime	      tmi  = CTime(Arg(2));
    BDat	      dat  = cser[tmi];
    contens_ = (dat);
}
#endif // __USE_TC__

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutSerDat);
  DefExtOpr(1, BDatPutSerDat, "PutSerDat", 3, 3, "Serie Date Real",
  I2("(Serie ser, Date d, Real newValue)",
     "(Serie ser, Date d, Real nuevoValor)"),
  I2("Changes the value of an element of a time series and returns the old "
      "value.",
     "Cambia el valor de un elemento de una serie temporal y devuelve el "
     "valor que tenía anteriormente."),
     BOperClassify::Conversion_);
  void BDatPutSerDat::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSerie& ser = *Tsr(Arg(1));
  BDate		  dte = Date(Arg(2));
  BDat		  x   = Dat(Arg(3));
  contens_ = ser[dte];
  ser.PutDat(dte,x);
}

#ifdef __USE_TC__
//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatPutCSerDat);
DefExtOpr(1, BDatPutCSerDat, "PutCSerDat", 3, 3, "CSeries CTime Real",
	  "(CSeries cser, CTime tmi, Real newValue)",
	  I2("Changes the value of an element of a time series and returns "
	     "the old value.",
	     "Cambia el valor de un elemento de una serie temporal y "
	     "devuelve el valor que tenía anteriormente."),
	  BOperClassify::Conversion_);
void BDatPutCSerDat::CalcContens()
//--------------------------------------------------------------------
{
    BUserCTimeSeries& cser = *CTsr(Arg(1));
    BCTime	      tmi  = CTime(Arg(2));
    BDat	      x    = Dat(Arg(4));
    contens_ = cser[tmi];
    cser.PutDat(tmi, x);
}
#endif // __USE_TC__

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCard);
  DefExtOpr(1, BDatCard, "Card", 1, 1, "Set",
  I2("(Set set)",
     "(Set conjunto)"),
  I2("Returns the cardinal of a set.",
     "Devuelve el cardinal de un conjunto."),
     BOperClassify::SetAlgebra_);
  void BDatCard::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Set(Arg(1)).Card()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDateDif);
  DefExtOpr(1, BDatDateDif, "DateDif", 3, 3, "TimeSet Date Date",
  I2("(TimeSet dating, Date from, Date until)",
     "(TimeSet fechado, Date desde, Date hasta)"),
  I2("Returns the number of dates between two dates throughout a dating.",
     "Devuelve el número de fechas entre dos fechas a lo largo de un fechado."),
     BOperClassify::TimeAlgebra_);
  void BDatDateDif::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSet* d = Tms (Arg(1));
  BDate&	f = Date(Arg(2));
  BDate&	l = Date(Arg(3));
  contens_ = d->Difference(f,l);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatYear);
  DefExtOpr(1, BDatYear, "Year", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the year of a date.",
     "Devuelve el año de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatYear::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Year()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMonth);
  DefExtOpr(1, BDatMonth, "Month", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the month of a date.",
     "Devuelve el mes de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatMonth::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Month()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDay);
  DefExtOpr(1, BDatDay, "Day", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the day in month of a date.",
     "Devuelve el día del mes de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatDay::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Day()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHour);
  DefExtOpr(1, BDatHour, "Hour", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the hour of a date.",
     "Devuelve la hora de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatHour::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Hour()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMinute);
  DefExtOpr(1, BDatMinute, "Minute", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the minute of a date.",
     "Devuelve el minuto de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatMinute::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Minute()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSecond);
  DefExtOpr(1, BDatSecond, "Second", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the second of a date.",
     "Devuelve el segundo de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatSecond::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Second()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFraction);
  DefExtOpr(1, BDatFraction, "Fraction", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the fraction of day of a date.",
     "Devuelve la fracción de día de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatFraction::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Fraction()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatHash);
  DefExtOpr(1, BDatHash, "Hash", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the number yyyymmdd of a date.",
     "Devuelve el número yyyymmdd de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatHash::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).Hash()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWeekDay);
  DefExtOpr(1, BDatWeekDay, "WeekDay", 1, 1, "Date",
  I2("(Date date)",
     "(Date fecha)"),
  I2("Returns the day of the week of a date.",
     "Devuelve el día de la semana de una fecha."),
     BOperClassify::TimeAlgebra_);
  void BDatWeekDay::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Date(Arg(1)).WeekDay()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDegree);
  DefExtOpr(1, BDatDegree, "Degree", 1, 1, "Polyn",
  "(Polyn pol)",
  I2("Returns the degree of a polynomial.",
     "Devuelve el grado de un polinomio."),
     BOperClassify::RetardPolynomial_);
  void BDatDegree::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Pol(Arg(1)).Degree()); }

/*
//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPolSize);
  DefExtOpr(1, BDatPolSize, "PolSize", 1, 1, "Polyn",
  "(Polyn pol)",
  I2("Returns the number of non null monomials of a polynomial.",
     "Devuelve el número de monomios no nulos de un polinomio."),
     BOperClassify::RetardPolynomial_);
  void BDatPolSize::CalcContens()
//--------------------------------------------------------------------
{ contens_ = (Pol(Arg(1)).Size()); }
*/

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSystem);
  DefExtOpr(1, BDatSystem, "System", 1, 1, "Text",
  "(Text txt)",
  I2("Calls the operative system to execute a command line and returns false "
     "on error.",
     "Llama al sistema operativo para ejecutar una línea de comando y "
     "devuelve falso en caso de error."),
     BOperClassify::System_);
  void BDatSystem::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = (BReal)BSys::System(Text(Arg(1)));
}


#ifndef UNIX
//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatShellExecute);
  DefExtOpr(1, BDatShellExecute, "ShellExecute", 2, 2, "Text Text",
  I2("(Text fileName, Text action)",
     "(Text nombreFichero, Text acción)"),
  I2("Calls the Windows operative system to do an action over a file."
     "The action and the file extension must be known by Windows."
     "For example, if you write ShellExecute(\"f.txt\",\"open\") "
     "then windows will open the file with the default text editor "
     "associated to \".txt\" extension. Another valid action is \"print\".",
     "Llama al sistema operativo Windows para ejecutar una acción sobre un "
     "fichero. Tanto la extensión del fichero como la acción deben ser "
     "reconocidas por Windows. Por ejemplo, si se escribe "
     "ShellExecute(\"f.txt\",\"open\")	Windows abrirá el fichero con el "
     "editor de texto por defecto asociado a la extensión \".txt\". Otra "
     "acción posible es \"print\" (imprimir)."),
     BOperClassify::System_);
  void BDatShellExecute::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = (BInt)ShellExecute
  (
    0,			   // handle to parent window
    Text(Arg(2)).String(), // string that specifies operation to perform
    Text(Arg(1)).String(), // filename string
    NIL,		   // string that specifies executable-file parameters
    NIL,		   // string that specifies default directory
    SW_SHOWNORMAL	       // whether file is shown when opened
  );
}

#define ShwMd(a,b) BText("\n\n	  ")+BInt(a)+"\t: "+b

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWinSystem);
  DefExtOpr(1, BDatWinSystem, "WinSystem", 2, 3, "Text Real Real",
  "(Text command, Real showMode [, Real wait=FALSE])",
  I2("Calls the operative system to execute a command line "
     "and returns false on error. In Windows, 'showMode' indicates the window "
     "display mode and if 'wait' is true then it wait until process ends."
     ,
     "Llama al sistema operativo para ejecutar una línea de comando "
     "y devuelve falso en caso de error. En windows, el parámetro 'showMode' "
     "indica la forma de visualización de la ventana y si 'wait' es cierto "
     "se espera a que acabe el proceso.")+"\n\n"+
     "-showMode valid values: "+
      ShwMd(SW_HIDE             ,"Hides the window and activates another window. ")
     +ShwMd(SW_MAXIMIZE         ,"Maximizes the specified window. ")
     +ShwMd(SW_MINIMIZE         ,"Minimizes the specified window and activates the next top-level window in the Z order. ")
     +ShwMd(SW_RESTORE          ,"Activates and displays the window. If the window is minimized or maximized, Windows restores it to its original size and position. An application should specify this flag when restoring a minimized window. ")
     +ShwMd(SW_SHOW             ,"Activates the window and displays it in its current size and position. ")
     +ShwMd(SW_SHOWDEFAULT      ,"Sets the show state based on the SW_ flag specified in the STARTUPINFO structure passed to the CreateProcess function by the program that started the application. ")
     +ShwMd(SW_SHOWMAXIMIZED    ,"Activates the window and displays it as a maximized window. ")
     +ShwMd(SW_SHOWMINIMIZED    ,"Activates the window and displays it as a minimized window. ")
     +ShwMd(SW_SHOWMINNOACTIVE  ,"Displays the window as a minimized window. The active window remains active. ")
     +ShwMd(SW_SHOWNA           ,"Displays the window in its current state. The active window remains active. ")
     +ShwMd(SW_SHOWNOACTIVATE   ,"Displays a window in its most recent size and position. The active window remains active. ")
     +ShwMd(SW_SHOWNORMAL       ,"Activates and displays a window. If the window is minimized or maximized, Windows restores it to its original size and position. An application should specify this flag when displaying the window for the first time. ")
     ,
     BOperClassify::System_);
  void BDatWinSystem::CalcContens()
//--------------------------------------------------------------------
{
  const BText& command = Text(Arg(1));
  int    show    = int(Real(Arg(2)));
  bool   wait    = Arg(3)?Real(Arg(3))!=0:false;
  contens_ = (BReal)BSys::WinSystem(command,show,wait);
}

#endif

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatChildProcess);
  DefExtOpr(1, BDatChildProcess, "ChildProcess", 1, 1, "Text",
  "(Text txt)",
  I2("Calls the operative system to execute a command line as a child "
     "process and returns false on error.",
     "Llama al sistema operativo para ejecutar una línea de comando como "
     "un proceso hijo y devuelve falso en caso de error."),
     BOperClassify::System_);
  void BDatChildProcess::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = (BReal)BSys::ChildProcess(Text(Arg(1)));
}


//--------------------------------------------------------------------
  bool GetGrammarOptions(BSyntaxObject* g, BArray<BGrammar*>& graOpt)
//--------------------------------------------------------------------
{
  int i;
  BSyntaxObject* graObj = NULL;
  BGrammar* gra = NULL;
  if(g->Grammar()==GraText())
  {
    graOpt.AllocBuffer(1);
    graOpt[0] = Gram(Text(g));
  }
  else if(g->Grammar()==GraSet())
  {
    BSet& set = Set(g);
    graOpt.AllocBuffer(set.Card());
    for(i=0; i<graOpt.Size(); i++)
    {
      graObj = set[i+1];
      if(!graObj || (graObj->Grammar()!=GraText()))
      {
        Error(I2("Element number ", "El elemento número ")+(i+1)+
          ((graObj)?(BText(" {")+graObj->Identify()+"} "):BText(" "))+
          I2(" of argument 'grammar' for FunctionExist, is not a Text ",
          " del arguemnto 'gramatica' para FunctionExist, no es un Text "));
        return(false);
      }
      else
      {
        graOpt[i] = Gram(Text(graObj));
        if(!graOpt[i])
        {
          Error(I2("Element number ", "El elemento número ")+(i+1)+
            " {"+graObj->Dump()+"} "+
            I2("of argument 'grammar' for FunctionExist, is not a valid "
               "grammar name.",
               "del arguemnto 'gramatica' para FunctionExist, no es un "
               "nombre valido de gramatica "));
          return(false);
        }
      }
    }
  }
  else
  {
    assert(1);
    return(false);
  }
  return(true);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatObjectExist);
  DefExtOpr(1, BDatObjectExist, "ObjectExist", 2, 2, "Text Text",
  I2("(Text grammar, Text name)","(Text gramatica, Text nombre)"),
  I2("Returns true if exists an object with the given grammar and name.",
     "Devuelve verdadero si existe un objeto con la gramática y el nombre"
     "dados."),
     BOperClassify::System_);
  void BDatObjectExist::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject* g = Arg(1);
  const BText&  name = Text(Arg(2));
  BArray<BGrammar*> graOpt;
  int i;
  BSyntaxObject* result = NULL;
  BSyntaxObject* graObj = NULL;
  BGrammar* gra = NULL;
  contens_ = false;
  if(!GetGrammarOptions(g, graOpt)) { return; }
  for(i=0; !result && (i<graOpt.Size()); i++)
  {
    gra = graOpt[i];
    result = gra->FindOperand(name, false);
    if(!result) 
    { 
      bool oldEnabled = BOut::Disable();
      result = gra->LeftEvaluateExpr(name); 
      if(oldEnabled) { BOut::Enable(); }
    }
    if(result && (gra!=GraAnything()) && (result->Grammar()!=gra)) { result=NULL; }
  }
  contens_ = result!=NULL;  
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFunctionExist);
  DefExtOpr(1, BDatFunctionExist, "FunctionExist", 2, 2, 
    "{Text|Set} Text",
    I2("({Text|Set} grammar, Text reference)",
       "({Text|Set} gramatica, Text reference)"),
  I2("Returns true if exists an function returning one the given grammars "
     "accesible as the specified reference, I.E., a TOL name or a TOL "
     "expression involving just accessing operators",
     "Devuelve verdadero si existe una función que devuelva alguna de las "
     "gramáticas dadas acesible mediante la refencia especificada, es decir, "
     "un nombre TOL o una expresión TOL involucrando exclusivamente los "
     "operadores de acceso")+" [], Element, ->, Field, ::",
     BOperClassify::System_);
  void BDatFunctionExist::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject* g = Arg(1);
  const BText&  name = Text(Arg(2));
  BArray<BGrammar*> graOpt;
  int i;
  BSyntaxObject* result = NULL;
  BSyntaxObject* graObj = NULL;
  BGrammar* gra = NULL;
  contens_ = false;
  if(!GetGrammarOptions(g, graOpt)) { return; }
  for(i=0; !result && (i<graOpt.Size()); i++)
  {
    gra = graOpt[i];
    result = gra->FindOperator(name);
    if(!result) 
    { 
      bool oldEnabled = BOut::Disable();
      result = GraCode()->LeftEvaluateExpr(name); 
      if(oldEnabled) { BOut::Enable(); }
      if(result)
      {
        BUserCode* uCode = (BUserCode*)result;
        BOperator* opr = GetOperator(uCode);
        if(!opr || ((gra!=GraAnything()) && (gra!=opr->Grammar())))
        {
          result = NULL;
        }
      }
    }
  }
  contens_ = result!=NULL;
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFileBytes);
  DefExtOpr(1, BDatFileBytes, "FileBytes", 1, 1, "Text",
  I2("(Text path)","(Text camino)"),
  I2("Returns size of given file in bytes.",
     "Devuelve el tamaño del fichero dado en bytes."),
     BOperClassify::System_);
  void BDatFileBytes::CalcContens()
//--------------------------------------------------------------------
{
  BDir dir(Text(Arg(1)));
  contens_ = dir.Exist()? dir.Bytes() : -1;
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDirExist);
  DefExtOpr(1, BDatDirExist, "DirExist", 1, 1, "Text",
  I2("(Text path)","(Text camino)"),
  I2("Returns true if exists the directory with the indicated path.",
     "Devuelve verdadero si existe el directorio con el camino indicado."),
     BOperClassify::System_);
  void BDatDirExist::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = BDir::CheckIsDir(Text(Arg(1)));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFileExist);
  DefExtOpr(1, BDatFileExist, "FileExist", 1, 1, "Text",
  I2("(Text path)","(Text camino)"),
  I2("Returns true if exists the file with the indicated path.",
     "Devuelve verdadero si existe el fichero con el camino indicado."),
     BOperClassify::System_);
  void BDatFileExist::CalcContens()
//--------------------------------------------------------------------
{
  BDir dir = Text(Arg(1));
  contens_ = dir.Exist() && dir.IsFile();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFileDelete);
  DefExtOpr(1, BDatFileDelete, "FileDelete", 1, 1, "Text",
  I2("(Text file)","(Text fichero)"),
  I2("Deletes a existent file. On error returns FALSE, else returns TRUE.",
     "Borra un fichero existente. En caso de error devuelve FALSO y si no "
     "CIERTO."),
     BOperClassify::System_);
  void BDatFileDelete::CalcContens()
//--------------------------------------------------------------------
{
  const BText& file = Text(Arg(1));
  contens_ = BSys::Remove(file);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFileRename);
  DefExtOpr(1, BDatFileRename, "FileRename", 2, 2, "Text Text",
  I2("(Text oldName, Text newName)","(Text nombreViejo, Text nombreNuevo)"),
  I2("Renames a existent file. On error returns FALSE, else returns TRUE.",
     "Renombra un fichero existente. En caso de error devuelve FALSO y si no "
     "CIERTO."),
     BOperClassify::System_);
  void BDatFileRename::CalcContens()
//--------------------------------------------------------------------
{
  const BText& oldFile = Text(Arg(1));
  const BText& newFile = Text(Arg(2));
  contens_ = !rename(oldFile.String(),newFile.String());
#ifdef	 UNIX
#else // WINDOWS
  if(contens_==0)
  {
    Error(BText("\n[")+errno+"]=" + strerror(errno) + "\n" +
	  I2("Cannot execute the command: ",
	     "No se pudo ejecutar el mandato: ") +
	     "\nFileRename("+oldFile+","+newFile+")");
  }
#endif
}

  //--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFileCat);
  DefExtOpr(1, BDatFileCat, "FileCat", 2, 2, "Set Text",
  "(Set origin, Text target)",
  I2("Cats a set of files into target. On error returns FALSE, else returns TRUE.",
     "Copia un conjunto de ficheros en otro. En caso de error devuelve FALSO y si no "
     "CIERTO."),
     BOperClassify::System_);
  void BDatFileCat::CalcContens()
//--------------------------------------------------------------------
{
  const BSet& origin = Set(Arg(1));
  const BText& target = Text(Arg(2));
  BArray<BText> in(origin.Card());
  int i;
  BSyntaxObject* obj;
  for(i=0; i<in.Size(); i++)
  {
    obj = origin[i+1];
    if(obj && (obj->Grammar()==GraText()))
    {
      in[i] = Text(obj);
    }
    else
    {
      Error(BText("[FileCat] ")+
        I2("Argument 'origin' must have just Text elements",
           "El argumento 'origin' debe tener sólo elementos "
           "de tipo Text"));
    }
  }
  contens_ = BSys::Cat(in, target);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMkDir);
  DefExtOpr(1, BDatMkDir, "MkDir", 1, 1, "Text",
  I2("(Text newDirectory)","(Text nuevoDirectorio)"),
  I2("Makes a new directory. Returns TRUE if success.",
     "Crea un directorio nuevo. Devuelve CIERTO en caso de éxito."),
     BOperClassify::System_);
  void BDatMkDir::CalcContens()
//--------------------------------------------------------------------
{
  const BText& dir = Text(Arg(1));
  contens_  = BSys::MkDir(dir,true);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatExit);
  DefIntOpr(1, BDatExit, "Exit", 1, 1,
  I2("(Real notUsed)","(Real noUsado)"),
  I2("Aborts TOL program execution.",
     "Aborta la ejecución del programa TOL."),
     BOperClassify::System_);
  void BDatExit::CalcContens()
//--------------------------------------------------------------------
{
  exit((BInt)Real(Arg(1)));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatFrobeniusNorm);
  DefExtOpr(1, BDatMatFrobeniusNorm, "MatFrobeniusNorm", 1, 1, "Matrix",
  I2("(Matrix mat)",
     "(Matrix mat)"),
  I2("Returns the Frobenius norm of a matrix, it's to say, the "
     "standard desviation of its elements",
     "Devuelve la norma de Frobenious de una matriz que es la desviación "
     "típica de sus elementos."),
     BOperClassify::Conversion_);
  void BDatMatFrobeniusNorm::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)).FrobeniusNorm();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatTraze);
  DefExtOpr(1, BDatMatTraze, "MatTraze", 1, 1, "Matrix",
  I2("(Matrix mat)",
     "(Matrix mat)"),
  I2("Returns the traze of a matrix, it's to say, the sum of the elements "
     "along its diagonal.",
     "Devuelve la traza de una matriz, esto es, la suma de los elementos "
     "de su diagonal."),
     BOperClassify::Conversion_);
  void BDatMatTraze::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = MatTraze(Mat(Arg(1)));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatDat);
  DefExtOpr(1, BDatMatDat, "MatDat", 3, 3, "Matrix Real Real",
  I2("(Matrix mat, Real row, Real column)",
     "(Matrix mat, Real fila, Real columna)"),
  I2("Returns an element of a matrix.",
     "Devuelve un elemento de una matriz."),
     BOperClassify::Conversion_);
  void BDatMatDat::CalcContens()
//--------------------------------------------------------------------
{
  BUserMat* uM = (BUserMat*)(Arg(1));
  BInt i = (BInt)Real(Arg(2))-1;
  BInt j = (BInt)Real(Arg(3))-1;
  if((i>=0) && (i<uM->Contens().Rows   ()) &&
     (j>=0) && (j<uM->Contens().Columns()))
  {
    contens_ = uM->Contens()(i,j);
  }
  else
  {
    BText id = Arg(1)->Identify();
    Warning(I2("Attempt to invalid access of function MatDat(",
         "Intento de acceso no válido de la función MatDat(")+
      id+","+Arg(2)->Dump()+","+Arg(3)->Dump()+")");
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutMatDat);
  DefExtOpr(1, BDatPutMatDat, "PutMatDat", 4, 4, "Matrix Real Real Real",
  I2("(Matrix mat, Real row, Real column, Real newValue)",
     "(Matrix mat, Real fila, Real columna, Real nuevoValor)"),
  I2("Changes the value of an element of a matrix and returns the old value.",
     "Cambia el valor de un elemento de una matriz y devuelve el valor "
     "que tenía anteriormente."),
     BOperClassify::Conversion_);
  void BDatPutMatDat::CalcContens()
//--------------------------------------------------------------------
{
  BUserMat* uM = (BUserMat*)(Arg(1));
  BInt i  = (BInt)Real(Arg(2))-1;
  BInt j  = (BInt)Real(Arg(3))-1;
  BDat d  = Dat(Arg(4));
  contens_ = uM->Contens().Get(i,j);
  if((i>=0) && (i<uM->Contens().Rows   ()) &&
     (j>=0) && (j<uM->Contens().Columns()))
  {
    uM->Contens()(i,j)=d;
  }
  else
  {
    BText id = Arg(1)->Identify();
    Warning(I2("Attempt to invalid access of function PutMatDat(",
         "Intento de acceso no válido de la función PutMatDat(")+
      id+","+Arg(2)->Dump()+","+Arg(3)->Dump()+","+Arg(4)->Dump()+")");
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatRows);
  DefExtOpr(1, BDatMatRows, "Rows", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the number of rows of a matrix.",
     "Devuelve el número de filas de una matriz."),
     BOperClassify::MatrixAlgebra_);
  void BDatMatRows::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)).Rows();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatColumns);
  DefExtOpr(1, BDatMatColumns, "Columns", 1, 1, "Matrix",
  "(Matrix mat)",
  I2("Returns the number of columns of a matrix.",
     "Devuelve el número de columnas de una matriz."),
     BOperClassify::MatrixAlgebra_);
  void BDatMatColumns::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Mat(Arg(1)).Columns();
}




//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsStationary);
  DefExtOpr(1, BDatIsStationary, "IsStationary", 1, 1, "Polyn",
  "(Polyn pol)",
  I2("Returns true if all the roots of the polynomial are would be of the "
     "unit circle.",
     "Devuelve verdadero si todas las raíces del polinomio están fuera del "
     "círculo unidad."),
     BOperClassify::RetardPolynomial_);
  void BDatIsStationary::CalcContens()
//--------------------------------------------------------------------
{
  BPol p = Pol(Arg(1));
  p.Aggregate();
  contens_ = p.IsStationary();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatStationaryValue);
  DefExtOpr(1, BDatStationaryValue, "StationaryValue", 1, 1, "Polyn",
  "(Polyn pol)",
  I2("Returns a measure of the distence to the unit circle of the "
     "polynomial roots .",
     "Devuelve una medida de la distancia al círculo unidad de las raíces de "
     "un polinomio."),
     BOperClassify::RetardPolynomial_);
  void BDatStationaryValue::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Pol(Arg(1)).StationaryValue();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatStationaryProb);
  DefExtOpr(1, BDatStationaryProb, "StationaryProb", 4, 4,
  "Polyn Matrix Real Real",
  "(Polyn pol, Matrix L, Real v, Real numIter)",
  I2("Returns the probability of a polynomial should be stationary.",
     "Devuelve la probabilidad de que un polinomio sea estacionario."
     "El parámetro pol es una estimación del polinomio con v grados de "
     "libertad y Cov = L*Tra(L)"),
     BOperClassify::RetardPolynomial_);
  void BDatStationaryProb::CalcContens()
//--------------------------------------------------------------------
{
  BPol p = Pol(Arg(1));
  BMat L = Mat(Arg(2));
  BInt v = (BInt)Real(Arg(3));
  BInt N = (BInt)Real(Arg(4));
  BInt m = p.Degree();
  BMat P(m,1);
  BMat T(m,1);
  BInt i,k;

  BNormalDist	 nor(0,1);
  BDat X2k;

  BDat sumOk = 0;

  for(i=0; i<m; i++) { P(i,0) = -p.Coef(i+1); }
  for(k=1; k<N; k++)
  {
    //Matriz aleatoria con distribución t-Student m-dimensional estandar
    //con v grados de libertad
    if(v<=100)
    {
      BChiSquareDist X2(v);
      X2k = X2.Random();
    }
    else
    {
      BDat U = nor.Random();
      BDat K = 2.0/(9.0*v);
      BDat C = U*Sqrt(K) + 1 - K;
      X2k = v*C*C*C;
    }
    for(i=0; i<m; i++)
    {
      T(i,0) = nor.Random()*Sqrt(X2k/v);
    }
    BMat Pk = P + L*T;
    BPol pk = BPol::One();
    for(i=0; i<m; i++)
    {
      pk -= BMonome<BDat>(Pk(i,0),i+1);
    }
    pk.Aggregate();
    BInt ok = pk.IsStationary();
//  Std(BText("\n")+(ok?" OK   ":" FAIL ")+pk.Name());
    sumOk += ok;
  }
  contens_ = sumOk/N;
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCoef);
  DefExtOpr(1, BDatCoef, "Coef", 2, 2, "Polyn Real",
  "(Polyn p, Real n)",
  I2("Returns the coefficient corresponding to the monome of degree n "
     "in the polynmial p.",
     "Devuelve el coeficiente correspondiente al monomio de grado n "
     "dentro del polinomio p."),
     BOperClassify::RetardPolynomial_);
  void BDatCoef::CalcContens()
//--------------------------------------------------------------------
{
  BPol pol = Pol (Arg(1));
  BInt deg = (BInt)Real(Arg(2));
  contens_ = pol.Coef(deg);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatEvalPol);
  DefExtOpr(1, BDatEvalPol, "EvalPol", 2, 2, "Polyn Real",
  "(Polyn p, Real x)",
  I2(".",
     "Evalúa la función polinomial correspondiente en el valor dado."),
     BOperClassify::RetardPolynomial_);
  void BDatEvalPol::CalcContens()
//--------------------------------------------------------------------
{
  BPol pol = Pol (Arg(1));
  BDat x   = Dat(Arg(2));
  contens_ = pol.Eval(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatEvalRat);
  DefExtOpr(1, BDatEvalRat, "EvalRat", 2, 2, "Ratio Real",
  "(Ratio r, Real x)",
  I2(".",
     "Evalúa la función racional correspondiente en el valor dado."),
     BOperClassify::RetardPolynomial_);
  void BDatEvalRat::CalcContens()
//--------------------------------------------------------------------
{
  BRat rat = Rat(Arg(1));
  BDat x   = Dat(Arg(2));
  contens_ = rat.Eval(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPutCoef);
  DefExtOpr(1, BDatPutCoef, "PutCoef", 3, 3, "Polyn Real Real",
  I2("(Polyn pol, Real degree, Real newValue)",
     "(Polyn pol, Real grado, Real nuevoValor)"),
  I2("Changes the value of an element of a polyn and returns the old value.",
     "Cambia el valor de un elemento de un polinomio y devuelve el valor "
     "que tenía anteriormente."),
     BOperClassify::Conversion_);
  void BDatPutCoef::CalcContens()
//--------------------------------------------------------------------
{
  BPol& P    = Pol(Arg(1));
  BInt  deg  = (BInt)Real(Arg(2));
  BDat& coef = Dat(Arg(3));
  contens_   = P.Coef(deg);
  P.PutCoef(deg,coef);
//Std(BText("\nPutCoef -> ")+P.Name());
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIntegrateQAG);
  DefExtOpr(1, BDatIntegrateQAG, "IntegrateQAG", 3, 4, "Code Real Real Real",
  I2("(Code f, Real from, Real until [, Real tolerance])",
     "(Code f, Real desde, Real hasta [, Real tolerancia])"),
  I2("Returns the numerical integrate of a function between two bounds "
     "with a given tolerance. f should a R->R function.\n"
     "Use a 21 point Gauss-Kronrod rule,\n"
     "see 'QAG adaptive integration' in GSL documentation.\n"
     "(In a future, Gauss-Kronrod rule used could be also given as a parameter)",
     "Devuelve la integral numérica de una función entre dos límites con una "
     "tolerancia dada. f debe ser una función R->R.\n"
     "Utiliza la regla Gauss-Kronrod de 21 puntos,\n"
     "ver 'QAG adaptive integration' en documentación de GSL.\n"
     "(En un futuro, la regla Gauss-Kronrod utilizada puede ser dada como parámetro)"),
     BOperClassify::NumericalAnalysis_);
  void BDatIntegrateQAG::CalcContens()
//--------------------------------------------------------------------
{
  BText english("In IntegrateQAG, ");
  BText spanish("En IntegrateQAG, ");
  BCode*  code = &Code(Arg(1));

  /* check user function */

  if (!code->Operator()) {
    english += "invalid code argument";
    spanish += "argumento código inválido";
    Error(I2(english,spanish));
    return;
  }

  /* check user function's grammar */

  if (code->Grammar() != GraReal()) {
    english += "invalid type ";
    english += code->Grammar()->Name();
    english += " for f argument: should be Real";
    spanish += " tipo inválido ";
    spanish += code->Grammar()->Name();
    spanish += " para argumento f: debe ser Real";
    Error(I2(english,spanish));
    return;
  } 
  
  /* check user function's number of arguments */
  if (code->Operator()->MinArg()!=1 && 
      code->Operator()->MaxArg()!=1) {
    english += "invalid argument number for f argument: should be 1";
    spanish += "número de argumentos inválidos para argumento f: debe ser 1";
    Error(I2(english,spanish));  
    return;
  }
  
  BDat	  a    = Dat (Arg(2));
  BDat	  b    = Dat (Arg(3));
  BDat	  t    = BDat::Tolerance();
  if(NumArgs()>3) { t = Dat (Arg(4)); }

  /* do QAG integration */

  RealGSLFunctionR1 funeval(code, NULL);
  gsl_integration_workspace * w = gsl_integration_workspace_alloc (1000);
  double result, error;
  gsl_function F; 
  F.function = &RealGSLFunction::StaticEvalReal;
  F.params = &funeval;
  gsl_integration_qag (&F, a.Value(), b.Value(),
		       0, t.Value(), 1000, GSL_INTEG_GAUSS21, w, &result, &error);  
  gsl_integration_workspace_free(w);
  contens_.PutValue(result);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNFirstDer);
  DefExtOpr(1, BDatNFirstDer, "FirstDer", 2, 3, "Code Real Real",
  I2("(Code f, Real x [, Real fx])",
     "(Code f, Real x [, Real fx])"),
  I2("Returns the first derivative of a function in x."
     "with a given tolerance",
     "Devuelve la derivada primera numérica de una función en x."),
     BOperClassify::NumericalAnalysis_);
  void BDatNFirstDer::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat	  x    = Dat (Arg(2));
  BDat	  fx;
  BRRCode f(code);
  if(Arg(3)) { fx = Dat (Arg(3)); }
  else       { fx = f[x]; } 
  contens_ = f.FirstDerivative(x,fx);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNSecondDer);
  DefExtOpr(1, BDatNSecondDer, "SecondDer", 2, 2, "Code Real",
  I2("(Code f, Real x)",
     "(Code f, Real x)"),
  I2("Returns the numérical second derivative of a function in x.",
     "Devuelve la derivada segunda numérica de una función en x."),
     BOperClassify::NumericalAnalysis_);
  void BDatNSecondDer::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat	  x    = Dat (Arg(2));
  BDat	  t    = BDat::Tolerance();
  if(Arg(3)) { t = Dat (Arg(3)); }
  BRRCode f(code);
  contens_ = f.SecondDerivative(x);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFibonacciSolve);
  DefExtOpr(1, BDatFibonacciSolve, "FibonacciSolve", 5, 5,
  "Code Real Real Real Real",
  "(Code f, Real y, Real min, Real max, Real tolerance)",
  I2("Solves the equation f(x) = y using the Fibonacci method between min "
     "and max with the given tolerance.",
     "Resuelve la ecuación f(x) = y usando el método de Fibonacci con valor "
     "inicial x0."),
     BOperClassify::NumericalAnalysis_);
  void BDatFibonacciSolve::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat	  y    = Dat (Arg(2));
  BDat	  min  = Dat (Arg(3));
  BDat	  max  = Dat (Arg(4));
  BDat	  tol  = Dat (Arg(5));
  BRRCode f(code);
  contens_ = BFibonacci::Solve(&f,y,min,max,tol);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFibonacciMin);
  DefExtOpr(1, BDatFibonacciMin, "FibonacciMin", 5, 5,
  "Code Real Real Real Real",
  "(Code f, Real y, Real a, Real b, Real tolerance)",
  I2("Minimizes the function f(x) between a and b using the Fibonacci method",
     "Minimiza la función f(x) entre a y b por el método de Fibonacci."),
     BOperClassify::NumericalAnalysis_);
  void BDatFibonacciMin::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat&	  y    = Dat (Arg(2));
  BDat	  min  = Dat (Arg(3));
  BDat	  max  = Dat (Arg(4));
  BDat	  tol  = Dat (Arg(5));
  BRRCode f(code);
  contens_ = BFibonacci::Minimum(&f,y,min,max,tol);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNewtonSolve);
  DefExtOpr(1, BDatNewtonSolve, "NewtonSolve", 2, 3, "Code Real Real",
  "(Code f, Real x0 [, Real y=0])",
  I2("Solves the equation f(x) = y using the Newton method begining at x0.",
     "Resuelve la ecuación f(x) = y usando el método de Newton con valor "
     "inicial x0."),
     BOperClassify::NumericalAnalysis_);
  void BDatNewtonSolve::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat	  x    = Dat (Arg(2));
  BDat	  y    = 0; if(Arg(3)) { y = Dat(Arg(3)); }
  BRRCode f(code);
  contens_ = f.NewtonSolve(x,y);
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNewtonMaxMin);
  DefExtOpr(1, BDatNewtonMaxMin, "NewtonMaxMin", 2, 2, "Code Real",
  I2("(Code f, Real x)",
     "(Code f, Real x)"),
  I2("Solves the equation f'(x) = 0 using the Newton method.",
     "Resuelve la ecuacion f'(x) = 0 usando el método de Newton."),
     BOperClassify::NumericalAnalysis_);
  void BDatNewtonMaxMin::CalcContens()
//--------------------------------------------------------------------
{
  BCode&  code = Code(Arg(1));
  BDat	  x    = Dat (Arg(2));
  BDat	  t    = BDat::Tolerance();
  if(Arg(3)) { t = Dat (Arg(3)); }
  BRRCode f(code);
  BDat dist = 0;
  BInt n = 0;
  do
  {
    n++;
    BDat x0 = x;
    BDat y0 = f[x];
    BDat y1 = f.FirstDerivative(x,y0);
    BDat y2 = f.SecondDerivative(x);
    x = x0 - y1/y2;
    dist = Abs(x-x0);
  }
  while(dist.IsKnown() && (dist>BDat::Tolerance()) && (n<=BDat::MaxIter()));
  contens_ = x;
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTextLength);
  DefExtOpr(1, BDatTextLength, "TextLength", 1, 1, "Text",
  "(Text txt)",
  I2("Returns the number of characters of a text variable.",
     "Devuelve el número de caracteres de una variable de texto."),
     BOperClassify::Text_);
  void BDatTextLength::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Text(Arg(1)).Length();
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFindText);
  DefExtOpr(1, BDatFindText, "TextFind", 2, 5, "Text Text Real Real Real",
  I2("(Text chain, Text searched [, Real from = 1 [, Real ocurrence = 1 "
     "[, Real direction = 1]]])",
     "(Text cadena, Text buscado [, Real desde = 1 [, Real ocurrencia = 1 "
	 "[, Real direccion = 1]]])"),
  I2("Returns the position of n-th occurrence of the searched text, "
     "as of an initial position within given chain.\n"
     "If is not found the sought text, zero is returned.\n"
	 "  chain: text to search.\n"
	 "  searched: text to find.\n"
	 "  [from=1]: position from of the original text from where "
	             "one begins to look for.\n"
	 "  [ocurrence=1]: indicates the number of ocurrecia of the search.\n"
	 "  [direction=1]: [1|-1] indicates the direction of the search.\n"
	 "    1 of left to right, -1 of right to left.\n"
     "    To specify another number one returns omitted."
     ,
     "Devuelve la posición de la enésima ocurrencia del texto buscado, "
     "a partir de una posición inicial dentro de la cadena dada.\n"
     "Si no se encuentra el texto buscado, se devuelve cero.\n"
     "  cadena: texto donde buscar.\n"
	 "  buscado: texto que buscamos.\n"
	 "  [desde=1]: posición desde del texto original desde donde se "
	              "empieza a buscar.\n"
	 "  [ocurrencia=1]: indica el número de ocurrecia de la busqueda.\n"
	 "  [direccion=1]: [1|-1] indica la dirección de la busqueda.\n"
	 "    1 de izquierda a derecha, -1 de derecha a izquierda.\n"
     "    De especificar otro número se retorna omitido."),
     BOperClassify::Text_);
  void BDatFindText::CalcContens()
//--------------------------------------------------------------------
{
  BInt  from      = 1;
  BInt  ocurrence = 1;
  BInt  direction = 1;
  if(Arg(3)) { from      = (BInt) Real(Arg(3)); }
  if(Arg(4)) { ocurrence = (BInt) Real(Arg(4)); }
  if(Arg(5)) { direction = (BInt) Real(Arg(5)); }
  // check direction
  if ((direction != 1) && (direction != -1)) {
    Warning(I2(
      "TextFind. Nonvalid value for the parameter 'direction'"
	  "It was expected  [-1 | 1] and it has been received: "<< direction
	  ,
      "TextFind. Valor no válido para el parámetro 'direccion'."
	  "Se esperaba [-1 | 1] y se ha recibido: "<< direction));
	contens_ = BDat::Unknown ();
  } else {
    if(from     <1) { from     =1; }
    if(ocurrence<1) { ocurrence=1; }
    if((ocurrence>1) || (direction==-1)) {
      contens_ = 
        1+Text(Arg(1)).FindAdv(Text(Arg(2)), from-1, ocurrence, direction);
	} else {
      contens_ = 1+Text(Arg(1)).Find(Text(Arg(2)), from-1);
	}
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTextOccurrences);
  DefExtOpr(1, BDatTextOccurrences, "TextOccurrences", 2, 2, "Text Text",
  I2("(Text chain, Text searched)",
     "(Text cadena, Text buscado)"),
  I2("Returns the number of occurrences of the searched text, within the "
     "given chain.",
     "Devuelve el número de ocurrencias del texto buscado, dentro de la "
     "cadena dada."),
     BOperClassify::Text_);
  void BDatTextOccurrences::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Text(Arg(1)).Occurrences(Text(Arg(2)));
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTextBeginWith);
  DefExtOpr(1, BDatTextBeginWith, "TextBeginWith", 2, 2, "Text Text",
  I2("(Text chain, Text searched)",
     "(Text cadena, Text buscado)"),
  I2("Returns certain if the searched text is exactly equal to the beginning "
     "of the given chain, inclusive if both are equal."
     "In other case returns false.",
     "Devuelve cierto si el texto buscado es exactamente igual al comienzo "
     "de la cadena dada, inclusive si ambas son iguales."
     "En otro caso devuelve falso."),
     BOperClassify::Text_);
  void BDatTextBeginWith::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Text(Arg(1)).BeginWith(Text(Arg(2)));
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTextEndAt);
  DefExtOpr(1, BDatTextEndAt, "TextEndAt", 2, 2, "Text Text",
  I2("(Text chain, Text searched)",
     "(Text cadena, Text buscado)"),
  I2("Returns certain if the searched text is exactly equal to the end "
     "of the given chain, inclusive if both are equal."
     "In other case returns false.",
     "Devuelve cierto si el texto buscado es exactamente igual al final "
     "de la cadena dada, inclusive si ambas son iguales."
     "En otro caso devuelve falso."),
     BOperClassify::Text_);
  void BDatTextEndAt::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Text(Arg(1)).EndAt(Text(Arg(2)));
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMatchText);
  DefExtOpr(1, BDatMatchText, "TextMatch", 2, 3, "Text Text Real",
  I2("(Text chain, Text pattern [, Real case = 0 ])",
     "(Text cadena, Text modelo [, Real modo = 0 ])"),
  I2(
     "Returns true if the pattern makes match with the given chain."
     "Works or not in a case sensitive way."
     "If it is not specified, case non sensitive is assumed."
     "The character * can match with any, one or more characters."
     "The character * only can be used at the begining or end of the "
     "pattern. \n"
     "Examples: \n"
     "	Match (\"Alfa\", \"Alfa\")  = TRUE; \n"
     "	Match (\"Alfa\", \"Alf*\")  = TRUE; \n"
     "	Match (\"Alfa\", \"*lfa\")  = TRUE; \n"
     "	Match (\"Alfa\", \"*lf*\")  = TRUE; \n"
     "	Match (\"Alfa\", \"*Alfa\") = TRUE; \n"
     "	Match (\"Alfa\", \"*lfa*\") = TRUE; \n"
     "	Match (\"Alfa\", \"*lfaX\") = FALSE; \n"
     "	Match (\"Alfa\", \"*l*f*\") = FALSE; \n"
     ,
     "Devuelve verdadero si el modelo o patrón es equiparable con una cadena "
     "dada. Trabaja o no de modo sensible a mayúsculas y minúsculas. "
     "Si no es especificado dicho modo, se sobreentiende el caso no sensible."
     "El carácter comodín * puede equipararse a cualquier combinación de uno "
     "o más caracteres. El carácter comodín * único puede usarse al "
     "comienzo o al final del modelo.\n"
     "Ejemplos: \n"
     "	Match (\"Alfa\", \"Alfa\")  = CIERTO; \n"
     "	Match (\"Alfa\", \"Alf*\")  = CIERTO; \n"
     "	Match (\"Alfa\", \"*lfa\")  = CIERTO; \n"
     "	Match (\"Alfa\", \"*lf*\")  = CIERTO; \n"
     "	Match (\"Alfa\", \"*Alfa\") = CIERTO; \n"
     "	Match (\"Alfa\", \"*lfa*\") = CIERTO; \n"
     "	Match (\"Alfa\", \"*lfaX\") = FALSO; \n"
     "	Match (\"Alfa\", \"*l*f*\") = FALSO; \n"
    ),
     BOperClassify::Text_);
  void BDatMatchText::CalcContens()
//--------------------------------------------------------------------
{
  BInt mode = 0;
  if(Arg(3)) { mode = (BInt)Real(Arg(3)); }
  contens_ = (BReal)(Text(Arg(1)).Match(Text(Arg(2)), mode));
}


  #ifdef UNIX

  #else // WINDOWS
    #include <dos.h>
  #endif

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSleep);
  DefExtOpr(1, BDatSleep, "Sleep", 1, 1, "Real",
  I2("(Real seconds)",
     "(Real segundos)"),
  I2("Sleeps TOL process while the specified time.",
     "El proceso TOL se queda en estado de espera no activa durante el "
     "tiempo especificado. El proceso no consumirá CPU en este periodo."),
     BOperClassify::Conversion_);
  void BDatSleep::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Dat(Arg(1));
#ifdef _MSC_VER
  Sleep((BInt)contens_.Value()*1000);
#else
  sleep((BInt)contens_.Value());
#endif
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatASCII);
  DefExtOpr(1, BDatASCII, "ASCII", 1, 2, "Text Real",
  I2("(Text chain [, Real n=1])",
     "(Text cadena [, Real n=1])"),
  I2("Returns the ASCII code of the n-th character of the given chain.",
     "Devuelve el código ASCII del enésimo caracter de la cadena dada."),
     BOperClassify::Conversion_);
  void BDatASCII::CalcContens()
//--------------------------------------------------------------------
{
  BInt n = 1;
  if(Arg(2)) { n = (BInt)Real(Arg(2)); }
  if(n<1) { n=1; }
  unsigned char ch = (unsigned char)Text(Arg(1)).Get(n-1);
  contens_ = ch;
}



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNumArg);
  DefExtOpr(1, BDatNumArg, "NumArg", 1, 1, "Code",
  I2("(Code f)",
     "(Code f)"),
  I2("Returns the number of arguments of a function.",
     "Devuelve el número de argumentos de una función."),
     BOperClassify::General_);
  void BDatNumArg::CalcContens()
//--------------------------------------------------------------------
{
  BUserCode*  uCode = UCode(Arg(1));
  contens_ = GetOperator(uCode)->MaxArg();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCReal);
  DefExtOpr(1, BDatCReal, "CReal", 1, 1, "Complex",
  I2("(Complex z)",
     "(Complex z)"),
  I2("Returns the real part of a complex number.",
     "Devuelve la parte real de un número complejo."),
     BOperClassify::ComplexArythmetic_);
  void BDatCReal::CalcContens()
//--------------------------------------------------------------------
{
  BComplex cmp = Cmp(Arg(1));
  contens_ = cmp.X();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCImag);
  DefExtOpr(1, BDatCImag, "CImag", 1, 1, "Complex",
  I2("(Complex z)",
     "(Complex z)"),
  I2("Returns the imaginary part of a complex number.",
     "Devuelve la parte imaginaria de un número complejo."),
     BOperClassify::ComplexArythmetic_);
  void BDatCImag::CalcContens()
//--------------------------------------------------------------------
{
  BComplex cmp = Cmp(Arg(1));
  contens_ = cmp.Y();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCAbs);
  DefExtOpr(1, BDatCAbs, "CAbs", 1, 1, "Complex",
  I2("(Complex z)",
     "(Complex z)"),
  I2("Returns the absolute value of a complex number.",
     "Devuelve el valor absoluto de un número complejo."),
     BOperClassify::ComplexArythmetic_);
  void BDatCAbs::CalcContens()
//--------------------------------------------------------------------
{
  BComplex cmp = Cmp(Arg(1));
  contens_ = cmp.R();
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCArg);
  DefExtOpr(1, BDatCArg, "CArg", 1, 1, "Complex",
  I2("(Complex z)",
     "(Complex z)"),
  I2("Returns the argument of a complex number.",
     "Devuelve el argumento de un número complejo."),
     BOperClassify::ComplexArythmetic_);
  void BDatCArg::CalcContens()
//--------------------------------------------------------------------
{
  BComplex cmp = Cmp(Arg(1));
  contens_ = cmp.A();
}


#if defined(UNIX)
  //incuir cabecera de sockets UNIX
#else
  #include<winsock.h>

#ifdef WRCMD_TRACE
  #include<stdio.h>
#endif // WRCMD_TRACE

//--------------------------------------------------------------------
// code
//--------------------------------------------------------------------

//-----------------------------------------------------------------------------
void SockDllStartup(void)

/*! Auxiliary procedure in order to initialize WinSock.DLL
 */
//-----------------------------------------------------------------------------
{
    WSAData Data;

    if (WSAStartup(0x0101,&Data))
    {
#     ifdef WRCMD_TRACE
	int err = WSAGetLastError();
	printf("TRACE: library startup failed: %d\n",err);
#     endif
	abort();
    }
}


//-----------------------------------------------------------------------------

// strunct save info WinRmt
struct ReturnWinRmt
{
   int   result;
   char  buf[4096];
} ;

//-----------------------------------------------------------------------------
struct ReturnWinRmt CallWinRmt( const char         *ServerName,
                                unsigned short int Port,
                                const char         *Command
                              )

/*! Executes a windows command on a remote host
 *
 * Return values:
 *  -1 whether remote service is unreachable.
 *   1 whether remote service was reached but application failed to execute.
 *   0 whether everything is Ok.
 */
//-----------------------------------------------------------------------------
{
  SOCKET         ClientSocket;
  SOCKADDR_IN    SocketAddr;
  unsigned int   addr;
  struct hostent *hp;
  struct ReturnWinRmt strWinRmt;    
  
  try {
    // Initialize socket library
#ifndef UNIX
    SockDllStartup();
#endif // UNIX

    if (isalpha(ServerName[0]))
    {   // server address is a name
      hp = gethostbyname(ServerName);
    }
    else
    { // Convert nnn.nnn address to a usable one 
      addr = inet_addr(ServerName);
      hp = gethostbyaddr((char *)&addr,4,AF_INET);
    }
    if (hp == NULL ) {
#ifdef WRCMD_TRACE
      int err = WSAGetLastError();
      printf("TRACE: cannot resolve address [%s]: %d\n", ServerName, err);
#endif
	  strWinRmt.result = -1;
	  strWinRmt.buf[0] = '\0';
      return strWinRmt;
    }
    //
    // Copy the resolved information into the sockaddr_in structure
    //
    memset(&SocketAddr, 0, sizeof(SocketAddr));
    memcpy(&(SocketAddr.sin_addr), hp->h_addr, hp->h_length);
    SocketAddr.sin_family = hp->h_addrtype; // PF_INET
    SocketAddr.sin_port   = htons(Port);

    // Create socket
    ClientSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_IP);
    if (ClientSocket == INVALID_SOCKET)
    {
#ifdef WRCMD_TRACE
      int err = WSAGetLastError();
      printf("TRACE: socket creation failed: %d\n",err);
#endif
	  strWinRmt.result = -1;
	  strWinRmt.buf[0] = '\0';
      return strWinRmt;
    }

    unsigned long int dummy = 0;
    if (ioctlsocket(ClientSocket,FIONBIO,&dummy)==SOCKET_ERROR)
    {
#ifdef WRCMD_TRACE
      int err = WSAGetLastError();
      printf("TRACE: socket blockage failed: %d\n",err);
#endif
      closesocket(ClientSocket);
	  strWinRmt.result = -1;
	  strWinRmt.buf[0] = '\0';
      return strWinRmt;
    }

    // Connect to peer
    if (connect(ClientSocket,(PSOCKADDR)&SocketAddr,sizeof(SocketAddr))
	== SOCKET_ERROR)
    {
#ifdef WRCMD_TRACE
    	int err = WSAGetLastError();
    	printf("TRACE: socket connection failed: %d\n",err);
#endif
    	closesocket(ClientSocket);
	    strWinRmt.result = -1;
	    strWinRmt.buf[0] = '\0';
        return strWinRmt;
    }

    // WRCMD Protocol
    WORD CmdLength = (WORD)(strlen(Command) + 1L);

	int rc = 0;
    rc = send(ClientSocket,(char *)&CmdLength,sizeof(CmdLength),MSG_DONTROUTE);
	if (rc == SOCKET_ERROR)
    {
#ifdef WRCMD_TRACE
    	int err = WSAGetLastError();
    	printf("TRACE: protocol error %d at step 1\n",err);
#endif
    	closesocket(ClientSocket);
	    strWinRmt.result = -1;
	    strWinRmt.buf[0] = '\0';
        return strWinRmt;
    }
	rc = 0;
    rc = send(ClientSocket,Command,CmdLength,MSG_DONTROUTE);
    if (rc == SOCKET_ERROR)
    {
#ifdef WRCMD_TRACE
    	int err = WSAGetLastError();
    	printf("TRACE: command string not sent due to error %d\n",err);
#endif
    	closesocket(ClientSocket);
	    strWinRmt.result = -1;
	    strWinRmt.buf[0] = '\0';
        return strWinRmt;
    }

    strWinRmt.result = 0;
    strWinRmt.result = 
      recv(ClientSocket, strWinRmt.buf, sizeof(strWinRmt.buf), 0);
	if (strWinRmt.result == SOCKET_ERROR)
    {
#ifdef WRCMD_TRACE
      int err = WSAGetLastError();
      printf("TRACE: result value not received due to error %d\n",err);
#endif
      closesocket(ClientSocket);
	  strWinRmt.result = -1;
	  strWinRmt.buf[0] = '\0';
      return strWinRmt;
    }
    strWinRmt.buf[strWinRmt.result] = '\0'; 
    // Disconnect
    closesocket(ClientSocket);
    // return
    return strWinRmt;
  }

  catch (...)
  {
    strWinRmt.result = -1;
	strWinRmt.buf[0] = '\0';
    return strWinRmt;
  }
}

//-----------------------------------------------------------------------------
int WinRmtExec( const char         *ServerName,
                unsigned short int Port,
                const char         *Command,
                const char         *Mode)

/*! Executes a windows command on a remote host
 *
 * Return values:
 *  -1 whether remote service is unreachable.
 *   1 whether remote service was reached but application failed to execute.
 *   0 whether everything is Ok.
 */
//-----------------------------------------------------------------------------
{  
  BText command( Command );
  BInt  spPos, slashPos;
  
  // Build newCommand as WRCMD 2.0 syntax 

  spPos = command.Find(' '); 
  if (spPos == -1)
    spPos = command.Length();
  BText path = command.SubString(0, spPos-1);
  BText arg  = command.SubString(spPos+1, command.Length());
 
  slashPos = path.FindLast('/');
  BText pathFile = path.SubString(0, slashPos-1);
  BText File     = path.SubString(slashPos+1, path.Length());
  
  BText newCommand("$$RUN,");
  newCommand += File + "," + pathFile + "," + arg + "," + Mode  + "$$";

  struct ReturnWinRmt strWinRmt;

  strWinRmt = CallWinRmt( ServerName, Port, newCommand.String());
  // check result
  if (strWinRmt.result != -1) {
    if (strWinRmt.buf[0] > 31) {
 	  strWinRmt.result = 0;
	}
    else { 
	  strWinRmt.result = 1;
	}
  }
  return (strWinRmt.result);
}


//-----------------------------------------------------------------------------
int WinRmtKill(const char         *ServerName,
			   unsigned short int Port,
               unsigned short int PID)

/*! Kills a windows process on a remote host specified by its PID
 *
 * Return values:
 *    -1 whether remote service is unreachable.
 *     1 whether remote service was reached but application failed to kill.
 *     0 whether everything is Ok.
 */
//-----------------------------------------------------------------------------
{  
  // Build newCommand as WRCMD 2.0 syntax 
  
  BChar txtPID[8];
  sprintf(txtPID,"%d",PID);
  BText newCommand ("$$KILL,");
  newCommand += BText(txtPID) +"$$";

  struct ReturnWinRmt strWinRmt;
  strWinRmt = CallWinRmt( ServerName, Port, newCommand.String());
  // check result
  if (strWinRmt.result != -1) {
    if (strWinRmt.buf[strWinRmt.result - 1] == 0) {
 	  strWinRmt.result = 1;
	}
    else { 
	  strWinRmt.result = 0;
	}
  }
  return (strWinRmt.result);
}


//-----------------------------------------------------------------------------
int WinRmtProcessAlive(const char         *ServerName,
		               unsigned short int Port,
   		               unsigned short int PID)

/*! Check the activite one process on a remote host specified by its PID
 *
 * Return values:
 *  -1 if the remote server is not active or the comunication can't be
 *     established;
 *   0 if the process does not exist or process has been killed
 *   1 if the process is alive
 */
//-----------------------------------------------------------------------------
{  
  // Build newCommand as WRCMD 2.0 syntax 
  
  BChar txtPID[8];
  sprintf(txtPID,"%d",PID);
  BText newCommand ("$$MEMORIA$$");

  struct ReturnWinRmt strWinRmt;
  strWinRmt = CallWinRmt( ServerName, Port, newCommand.String() );
  BText allAliveCmd = BText(strWinRmt.buf)+",";
//Std(BText("\nWinRmtProcessAlive strWinRmt.buf='")+strWinRmt.buf+"'");
  // check exists process with PID ?
  if (strWinRmt.result==-1) {
    return -1;
  } 
  else 
  {
	  if (strWinRmt.buf[0] == '\0') 
    {
      strWinRmt.result = 0;
    } 
    else 
    {
      char strPID1[sizeof(PID)+2];
      sprintf(strPID1,",%d,", PID);
    //Std(BText("\nWinRmtProcessAlive strPID1='")+strPID1+"'");
      char * pos1 = strstr(allAliveCmd.Buffer(), strPID1);
    //Std(BText("\nWinRmtProcessAlive pos1='")+pos1+"'");
	    char strPID2[sizeof(PID)+3];		 
	    sprintf(strPID2,",%d$$", PID);
    //Std(BText("\nWinRmtProcessAlive strPID2='")+strPID2+"'");
      char * pos2 = strstr(allAliveCmd.Buffer(), strPID2);
    //Std(BText("\nWinRmtProcessAlive pos2='")+pos2+"'");
      if ((pos1 == NULL) && (pos2 == NULL)) 
      {
        strWinRmt.result = 0;
	    } 
      else 
      {
        strWinRmt.result = 1;
	    }		 
    }
  }
  return (strWinRmt.result);
}



//-----------------------------------------------------------------------------


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWinRmtSystem);
  DefExtOpr(1, BDatWinRmtSystem, "WinRmtSystem", 3, 4, "Text Real Text Text",
  I2("(Text ServerName|IP, Real Port, Text Command [, Text Mode=\"min\"])",
     "(Text NombreServidor|IP, Real Puerto, Text Comando [, Text Modo=\"min\"])"),
  I2("Executes Windows Command at remote location identified by ServerName "
     "(or IP Address) and Port. Such location must keep running a remote "
     "command server. There are several modes of execution:\n"
     " Min (Default) Command is executed in a minimized window.\n"
     " Max           Command is executed in a maximized window.\n"
     " Nor           Command is executed in a normal window.\n"
     " Hid           Command is executed hidden.\n"
     "Return values:\n"
     " -1 whether server can not be reached.\n"
     "  1 whether server was reached but command failed to execute.\n"
     "  0 whether everything Ok."
     ,
     "Ejecuta un comando Windows en una maquina remota identificada por "
     "su NombreServidor (o DireccionIP) y Puerto. Dicha maquina debe disponer "
     "de un servidor de comandos remotos en dicho puerto. Hay varios modos"
     "de ejecución:\n"
     " Min (Por defecto) El comando se ejecuta en una ventana minimizada.\n"
     " Max               El comando se ejecuta en una ventana maximizada.\n"
     " Nor               El comando se ejecuta en una ventana normal.\n"
     " Hid               El comando se ejecuta oculto (en segundo plano).\n"
     "Retorna:\n"
     " -1 si el servidor no esta activo o hubo error en la comunicacion.\n"
     "  1 si el comando no se pudo ejecutar.\n"
     "  0 si la ejecucion pudo llevarse a cabo sin problemas."
     ),
     BOperClassify::System_);
  void BDatWinRmtSystem::CalcContens()
//--------------------------------------------------------------------
{
  const BChar* serverName = Text(Arg(1)).String();
  const BInt   port       = (BInt)Real(Arg(2));
  const BChar* command    = Text(Arg(3)).String();
  const BChar* mode       = "Min";

  if(Arg(4)) { mode = Text(Arg(4)).String(); }
  contens_ = WinRmtExec(serverName, port, command, mode);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWinRmtKill);
  DefExtOpr(1, BDatWinRmtKill, "WinRmtKill", 3, 3, "Text Real Real",
  I2("(Text ServerName|IP, Real Port, Real PID)",
     "(Text NombreServidor|IP, Real Puerto, Real PID)"),
  I2("Kills a Windows Process at remote location identified by ServerName "
     "(or IP Address) and Port. Such location must keep running a remote "
     "command server.\n"
     "Return values:\n"
     " -1 whether server can not be reached.\n"
     "	1 whether server was reached but failed to kill the process.\n"
     "	0 whether everything Ok."
     ,
     "Elimina un proceso Windows de una maquina remota identificada por "
     "su NombreServidor (o DireccionIP) y Puerto. Dicha maquina debe disponer "
     "de un servidor de comandos remotos en dicho puerto.\n"
     "Retorna:\n"
     " -1 si el servidor no esta activo o hubo error en la comunicacion.\n"
     "	1 si el proceso no se pudo eliminar.\n"
     "	0 si la ejecucion pudo llevarse a cabo sin problemas."
     ),
     BOperClassify::System_);
  void BDatWinRmtKill::CalcContens()
//--------------------------------------------------------------------
{   
    contens_ = WinRmtKill(Text(Arg(1)).String(), 
			  (BInt)Real(Arg(2)), 
			  (BInt)Real(Arg(3)) );
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatWinRmtProcessAlive);
  DefExtOpr(1, BDatWinRmtProcessAlive, "WinRmtProcessAlive", 3, 3, 
	        "Text Real Real",
  I2("(Text ServerName|IP, Real Port, Real PID)",
     "(Text NombreServidor|IP, Real Puerto, Real PID)"),
  I2("Check a Windows Process at remote location identified by ServerName "
     "(or IP Address) and Port is active. Such location must keep running "
	 "a remote command server.\n"
     "Return values:\n"
     " -1 if the remote server is not active or the comunication can't be "
         "established\n"
     "  0 if the process does not exist or process has been killed\n"
     "  1 if the process is alive"
     ,
     "Comprueba que un proceso Windows de una maquina remota identificada por "
     "su NombreServidor (o DireccionIP) y Puerto esta activo. Dicha maquina "
	 "debe disponer de un servidor de comandos remotos en dicho puerto.\n"
     "Retorna:\n"
     " -1 si el servidor no esta activo o hubo error en la comunicacion.\n"
     "	0 si el proceso no existe o ha sido eliminado.\n"
     "	1 si el proceso está vivo."
     ),
     BOperClassify::System_);
  void BDatWinRmtProcessAlive::CalcContens()
//--------------------------------------------------------------------
{   
    contens_ = WinRmtProcessAlive(Text(Arg(1)).String(), 
				  (BInt)Real(Arg(2)), 
				  (BInt)Real(Arg(3)) );
}
#endif // UNIX

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatLLKR);
DefExtOpr(1, BDatLLKR, "LLKNoise", 3, 5, "Matrix Polyn Polyn Real Real",
          "(Matrix R, Polyn pAR, Polyn pMA [,Real sigma2=0, Real k=0])",
          I2("Compute loglikelihood for Noise R where R follows an ARMA model.\n"
             "Example:\n\n"
             "    Matrix w = Gaussian(350,1,0,1);\n"
             "    Polyn pAR = 1;\n"
             "    Polyn pMA = 1+0.2*B+0.5*B^2;\n"
             "    Ration pR = pMA/pAR;\n"
             "    Matrix N = DifEq(pR, w);\n"
             "    Real llk = LLKNoise(N,pAR,pMA);\n",
             "Calcula el loglikelihood de un ruido R con estructura ARMA.\n"
             "Ejemplo:\n\n"
             "    Matrix w = Gaussian(350,1,0,1);\n"
             "    Polyn pAR = 1;\n"
             "    Polyn pMA = 1+0.2*B+0.5*B^2;\n"
             "    Ration pR = pMA/pAR;\n"
             "    Matrix N = DifEq(pR, w);\n"
             "    Real llk = LLKNoise(N,pAR,pMA);\n"),
          BOperClassify::Conversion_);
void BDatLLKR::CalcContens()
//--------------------------------------------------------------------
{
  BMat * R = &Mat(Arg(1));
  BPol * pAR = &Pol(Arg(2));
  BPol * pMA = &Pol(Arg(3));
  BDat * pS2 = NULL;
  BDat * pK = NULL;
  
  double sigma2 = 0.0;
  int k = 0;
  
  if (!R || R->Columns()!=1 || R->Rows()==0) {
    Error(I2("Invalid R matrix, should be a column vector",
             "Matriz R invalida, debe ser un vector columna"));
    return;
  }
  if (!pAR) {
    Error(I2("Invalid pAR polynomial",
             "Polinomio pAR inválido"));
    return;
  }
  if (!pMA) {
    Error(I2("Invalid pMA polynomial",
             "Polinomio pMA inválido"));
    return;
  }
  if (NumArgs()>3) {
    pS2 = &Dat(Arg(4));
    sigma2 = pS2->Value();
    if (!pS2->IsKnown() || sigma2<0) {
      Warning(I2("In LLKNoise: sigma2 argument must non negative. Assuming 0",
                 "En LLKNoise: argumento k debe ser no negativo. Asumiendo 0"));
      sigma2 = 0.0;
    }    
    if (NumArgs()>4) {
      pK = &Dat(Arg(5));
      k = int(pK->Value());
      if (!pK->IsKnown() || k<0) {
        Warning(I2("In LLKNoise: k argument must be non negative. Assuming 0",
                   "En LLKNoise: argumento k debe ser no negativo. Asumiendo 0"));
        k = 1;
      }
    }
  }
  
  gsl_vector * gsl_R = gsl_vector_alloc(R->Rows());
  gsl_vector * gsl_pAR = gsl_vector_alloc(pAR->Degree()+1);
  gsl_vector * gsl_pMA = gsl_vector_alloc(pMA->Degree()+1);
  int r;
  
  for (r = 0; r < R->Rows(); r++)
    gsl_vector_set(gsl_R, r, (*R)(r,0).Value());
  gsl_vector_set_zero(gsl_pAR);
  gsl_vector_set(gsl_pAR,0,(*pAR)(0).Coef().Value());
  for (r = 1; r < pAR->Size(); r++)
    gsl_vector_set(gsl_pAR, (*pAR)(r).Degree(), -(*pAR)(r).Coef().Value());
  gsl_vector_set_zero(gsl_pMA);
  gsl_vector_set(gsl_pMA,0,(*pMA)(0).Coef().Value());
  for (r = 1; r < pMA->Size(); r++)
    gsl_vector_set(gsl_pMA, (*pMA)(r).Degree(), -(*pMA)(r).Coef().Value());
  contens_.PutValue(LLKR(gsl_R, gsl_pAR, gsl_pMA, sigma2, k));
  gsl_vector_free(gsl_R);
  gsl_vector_free(gsl_pAR);
  gsl_vector_free(gsl_pMA);
}
