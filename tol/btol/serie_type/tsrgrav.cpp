/* tsrgrav.cpp: Time Serie Grammar's various operators.
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

#include <map>

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_btsrgrav.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bout.h>

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("tsrgrav.cpp");

//--------------------------------------------------------------------
// non const date reference
//--------------------------------------------------------------------
#define NCDate(dte) (*((BDate*)(&dte)))

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerTsrGraVarious() { return(BTRUE); }

//--------------------------------------------------------------------
BUserTimeSet* DefaultDating(BSyntaxObject* dating)
//--------------------------------------------------------------------
{
  static BUserTimeSet* defaultDating = NIL;
  if(defaultDating==NIL) { defaultDating = Tms("C"); }
  if(dating) { return(Tms(dating)); }
  else       { return(defaultDating); }
}

//--------------------------------------------------------------------
BText WarnInfiniteRandom()
//--------------------------------------------------------------------
{
  return(I2(
  "NOTE: An infinite random series cannot be stored nor have a deterministic expression to be built." 
  "So each time you call a serie created with this function it will return a different realzation."
  "If you want to fix a constant random series then you need to bound dates with SubSer.",
  "NOTA: Una serie aleatoria infinita no puede ser almacenada ni existe una expresión determinista para construirla."
  "Por lo tanto cada vez que se le pidan datos a una serie creada con esta función devolverá una realización diferente."
  "Si se desea almacenar una realización concreta se debe aplicar SubSer al resultado."));
}

//--------------------------------------------------------------------
DefExtOpr(1, BTsrRandom, "Rand", 2, 3, "Real Real TimeSet",
  I2("(Real min, Real max [, TimeSet dating])",
     "(Real min, Real max [, TimeSet fechado])"),
  I2("Returns an uniform distributed random serie U(min,max).",
     "Devuelve una serie aleatoria con distribución uniforme U(min,max).")+
  WarnInfiniteRandom(),
  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrRandom::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    if(!Dating()) { return(BDat::Unknown()); }
    BUniformDist var(Real(Arg(1)),Real(Arg(2)));
    return(var.Random());
}


//--------------------------------------------------------------------
class BTsrGaussian : public BTsrTemporary
//--------------------------------------------------------------------
{
 private:
    BNormalDist dist_;
    std::map<double, BDat> valuesMap_;
 public:
    BTsrGaussian(BList* arg);
    BUserTimeSet* Dating() const { return(DefaultDating(Arg(3))); }
    BDat operator[](const BDate& dte) { return(GetDat(dte)); }
    BDat GetDat(const BDate& dte);
};
DeclareEvaluator(BTsrGaussian);

//--------------------------------------------------------------------
DefExtOpr(1, BTsrGaussian, "Gaussian", 2, 3, "Real Real TimeSet",
  I2("(Real nu, Real sigma [, TimeSet dating])",
     "(Real nu, Real sigma [, TimeSet fechado])"),
  I2("Returns an normal distributed random serie "
     "N[nu, sigma].",
     "Devuelve una serie aleatoria con distribución normal "
     "N[nu, sigma].")+
  WarnInfiniteRandom(),
  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BTsrGaussian::BTsrGaussian(BList* arg) 
    : BTsrTemporary(arg), 
      dist_(Real(Arg(1)), Real(Arg(2)))
{}

//--------------------------------------------------------------------
BDat BTsrGaussian::GetDat(const BDate& dte)
{
    BDat newValue;
    std::map<double, BDat>::const_iterator keyIterator;

    if(!Dating()) newValue = BDat::Unknown();
    else {
  keyIterator = valuesMap_.find(dte.Hash());
  if(keyIterator == valuesMap_.end()) {
      newValue = dist_.Random();
      valuesMap_.insert(std::pair<double, BDat>(dte.Hash(), newValue));
  } else newValue = keyIterator->second;
    }

    return(newValue);
}



//--------------------------------------------------------------------
DefExtOpr(1, BTsrPulse, "Pulse", 1, 2, "Date TimeSet",
  I2("(Date date, TimeSet dating)",
     "(Date fecha, TimeSet fechado)"),
  I2("Returns a series in a dated given with value one for the given date "
     "and zero in any other date.",
     "Devuelve una serie en un fechado dado con valor uno para la fecha "
     "dada y cero en cualquier otra fecha.")+
     "(..., 0,0,0,<1>,0,0,0 ...)",
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrPulse::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  if(dte == Center()) { return(BDat(1.0)); }
  else          { return(BDat(0.0)); }
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrCompens, "Compens", 1, 2, "Date TimeSet",
  I2("(Date date, TimeSet dating)",
     "(Date fecha, TimeSet fechado)"),
  I2("Returns a series in the given dating, with value 1 in a given date, "
     "- 1 in the following, and 0 in the rest",
     "Devuelve una serie en el fechado dado, con valor 1 en una fecha dada, "
     "-1 en la siguiente, y 0 en el resto")+
     "(..., 0,0,0,<1>,-1,0,0 ...)",
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrCompens::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
       if(dte == Center())     { return(BDat( 1.0)); }
  else if(dte == PostCenter()) { return(BDat(-1.0)); }
  else             { return(BDat( 0.0)); }
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrStep, "Step", 1, 2, "Date TimeSet",
  I2("(Date date, TimeSet dating)",
     "(Date fecha, TimeSet fechado)"),
  I2("Returns a series in the given dating, with value zero for each date "
     "less than a given date and one in the rest.",
     "Devuelve una serie en el fechado dado, con valor cero para cada fecha "
     "menor que una fecha dada y uno en el resto.")+
     "(..., 0,0,0,<1>,1,1,1 ...).",
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrStep::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  if(dte >= Center()) { return(BDat(1.0)); }
  else          { return(BDat(0.0)); }
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrTrend, "Trend", 1, 2, "Date TimeSet",
  I2("(Date date, TimeSet dating)",
     "(Date fecha, TimeSet fechado)"),
  I2("Returns a series in the given dating, with value zero for each date "
     "less than a given date, and that goes increasing in one as of such "
     "date.",
     "Devuelve una serie en el fechado dado, con valor cero para cada fecha "
     "menor que una fecha dada, y que se va incrementando en uno a partir de "
     "dicha fecha.")+
     "(..., 0,0,0,<1>,2,3,4 ...).",
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrTrend::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  BDate d;
  if(!Dating()) { return(BDat::Unknown()); }
  if(dte<Center()) { return(BDat(0.0)); }
  else {
   d = Dating()->FirstNoLess (Center());
   return(Dating()->Difference(d, dte)+1);
 }
}



//--------------------------------------------------------------------
DefExtOpr(1, BTsrLine, "Line", 4, 5, "Date Real Date Real TimeSet",
  I2("(Date d1, Real x1, Date d2, Real x2, TimeSet dating)",
     "(Date d1, Real x1, Date d2, Real x2, TimeSet fechado)"),
  I2("Returns a series in the given dating, with values in the rectum that "
     "goes through two points whose coordinates are (d1, x1) and (d2,x2).",
     "Devuelve una serie en el fechado dado, con valores en la recta que "
     "pasa por dos puntos cuyas coordenadas son (d1, x1) y (d2,x2)."),
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrLine::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(d0!=d1)
  {
    if(y1!=y0)
    {
      BReal pendient = (y1-y0)/Dating()->Difference(d0, d1);
      dat = y0 + pendient*Dating()->Difference(d0, dte);
    }
    else
    {
      dat = y0;
    }
  }
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrCalendary, "CalVar", 1, 2, "TimeSet TimeSet",
  I2("(TimeSet T, TimeSet dating)",
     "(TimeSet T, TimeSet fechado)"),
  I2("Returns a series in the given dating, whose value in each date is the "
     "cardinal of T between such date and its succeeding date within dating.",
     "Devuelve una serie en el fechado dado, cuyo valor en cada fecha es el "
     "cardinal de T entre dicha fecha y su fecha sucesora dentro del "
     "fechado."),
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrCalendary::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(center_)
  {
    BDate next = dte  + Dating();
    if(next.HasValue())
    {
      BDate prev = next - Dating();
      if(prev.HasValue())
      {
        BHash hash; center_->GetHashBetween(hash, dte, next);
        dat = hash.Size();
        if(hash[hash.Size()-1]==next.Hash()) { dat -= 1; }
      }
    }
/*
    Std(BText("\nCalVar(")+center_->Identify()+","+Dating()->Identify()+")"+
        "["+dte+"], next="+next+", value="+dat.Name());
*/
  }
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrIndicator, "CalInd", 1, 2, "TimeSet TimeSet",
  I2("(TimeSet timeSet, TimeSet dating)",
     "(TimeSet conjuntoTemporal, TimeSet fechado)"),
  I2("Returns a series in the given dating, whose value in each date is one "
     "if belongs to T and zero in other case.",
     "Devuelve una serie en el fechado dado, cuyo valor en cada fecha es uno "
     "si pertenece a T y cero en otro caso."),
    BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BTsrIndicator::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(center_)
  {
    dat = center_->Contain(dte);
  }
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrDatingChange, "DatCh", 3, 3, "Serie TimeSet Code",
  I2("(Serie S, TimeSet dating, Code statistic)",
     "(Serie S, TimeSet fechado, Code estadistico)"),
  I2("Transform a series into other in the given dating, whose value in each "
     "date is the application of a statistics function to the original "
     "series between such date and its succeeding date within dating."
     "So that the operation is accomplished correctly it is necessary that "
     "the exit dating will be a subset of the entry dating, or said of other "
     "manner, that it will be harmonic with this."
     "Some of the most used statistics are the first (FirstS), "
     "the sum (SumS), the average (AvrS), ..."
     "You can define your own statistic function returning Real with these 3 parameters: \n"
     "  Real MyStatistic(Series ser, Date datIni, Date datEnd)",
     "Transforma una serie en otra en el fechado dado, cuyo valor en cada "
     "fecha es la aplicación de una función estadística a la serie original "
     "entre dicha fecha y su fecha sucesora dentro del fechado."
     "Para que la operación se realize correctamente es necesario que el "
     "fechado de salida sea un subconjunto del fechado de entrada, o dicho "
     "de otro modo, que sea armónico con aquél."
     "Algunos de los estadísticos más usados son el primero (FirstS), "
     "la suma (SumS), la media (AvrS), ..."
     "Es posible definir nuevos estadísticos como funciones de tipo Real con estos tres argumentos: \n"
     "  Real MyStatistic(Series ser, Date datIni, Date datEnd)"),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDate BTsrDatingChange::FirstDate() const
//--------------------------------------------------------------------
{
  BUserTimeSerie* ser = Ser();
  if(firstDate_.HasValue()) { return(firstDate_); }
  if(ser && ser->Dating()) 
  { 
    BDate f = ser->FirstDate(); 
    if(!f.IsTheBegin()) 
    { 
      f = Dating()->FirstNoGreat(ser->FirstDate()); 
      if(f.IsTheBegin()) 
      {
        f = Dating()->FirstNoLess(ser->FirstDate()); 
      }
    }
    NCDate(firstDate_) = f;
  }
  return(firstDate_);
}
//--------------------------------------------------------------------
BDate BTsrDatingChange::LastDate() const
//--------------------------------------------------------------------
{
  BUserTimeSerie* ser = Ser();
  if(lastDate_.HasValue()) { return(lastDate_); }
  if(ser && ser->Dating())
  { 
    BDate l = ser->LastDate(); 
    if(!l.IsTheEnd()) 
    { 
      l = Dating()->FirstNoGreat(ser->LastDate()); 
      if(l.IsTheEnd()) 
      {
        l = Dating()->FirstNoLess(ser->LastDate()); 
      }
    }
    NCDate(lastDate_) = l;
  }
  return(lastDate_);
}
//--------------------------------------------------------------------
BStandardOperator* BTsrDatingChange::Stat()  const
//--------------------------------------------------------------------
{
  return(Code(Arg(3)).Operator());
}
//--------------------------------------------------------------------
BDat BTsrDatingChange::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()||!Ser()||!Ser()->Dating()) { return(BDat::Unknown()); }
  BDate dte2 = dte  + Dating() - Ser()->Dating();

  BUserDate* uDte1 = new BContensDate(dte );
  BUserDate* uDte2 = new BContensDate(dte2);

  BList* lst = Cons(Ser(),Cons(uDte1,NCons(uDte2)));
  BDatStatistic* uDat = (BDatStatistic*)(Stat()->Evaluator(lst));
  BDat dat;
  if(uDat)
  {
    dat = uDat->Contens();
    DESTROY(uDat);
  }
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrInverseDatingChange, "InvCh", 2, 2, "Serie Serie",
  "(Serie S1, Serie S2)",
  I2("It is defined T1 as the dating of the series S1, "
     "and T2 as the dating of the series S2. "
     "For each date of T2 that belongs to T1, is returned the value of "
     "S1 in such date. "
     "For each date of T2 that not belongs to T1, is returned the value of "
     "S2 in such date."
     "returns the value in second serie.",
     "Se define T1 como el fechado de la serie S1, "
     "y T2 como el fechado de la serie S2. "
     "Para cada fecha de T2 que pertenezca a T1, se devuelve el valor de "
     "S1 en dicha fecha. "
     "Para cada fecha de T2 que no pertenezca a T1, se devuelve el valor de "
     "S2 en dicha fecha."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BDate BTsrInverseDatingChange::FirstDate() const
//--------------------------------------------------------------------
{
    if(firstDate_.HasValue()) { return(firstDate_); }
    if(Ser(1)&&Dating())
    { NCDate(firstDate_)=Dating()->FirstNoLess(Ser(1)->FirstDate()); }
    return(firstDate_);
}

//--------------------------------------------------------------------
BDate BTsrInverseDatingChange::LastDate()  const
//--------------------------------------------------------------------
{
    if(lastDate_.HasValue()) { return(lastDate_); }
    if(Ser(1)&&Dating())
    { NCDate(lastDate_)=Dating()->FirstNoGreat(Ser(1)->LastDate()); }
    return(lastDate_);
}

//--------------------------------------------------------------------
BDat BTsrInverseDatingChange::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(Ser(1)&&Ser(2))
    {
  if(Ser(1)->Dating()->Contain(dte)) { dat = (*Ser(1))[dte]; }
  else           { dat = (*Ser(2))[dte]; }
    }
    return(dat);
}



//--------------------------------------------------------------------
DefExtOpr(1, BTsrExpand, "Expand", 1, 2, "Serie Real",
  "(Serie S, Real val)",
  I2("Transform a series through the  replacement of each equal value to x "
     "by the last different value of x.",
     "Transforma una serie mediante el  reemplazo de cada valor igual a x "
     "por el último valor diferente de x."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrExpand::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
//InitTotalTime("BTsrExpand::GetDat");
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  BDat val = 0;
  if(Arg(2)) { val = Dat(Arg(2)); }
  if(Dating() && (dte>=FirstDate()) && dte<=LastDate())
  {
    if(Data().HasValue()) { dat = data_[GetIndex(dte)]; }
    else
    {
      BInt n=0;
      BDate d=FirstDate();
      BDat  lastNotEqual = val;

      GetLength();
      Ser()->GetData(data_, FirstDate(),LastDate(),Length());

      BHash hash; Dating()->GetHashBetween(hash, FirstDate(),LastDate());
      BReal h = dte.Hash();
      for(; n<Length(); n++)
      {
  if(data_[n]==val)
  {
    data_[n] = lastNotEqual;
  }
  else      { lastNotEqual = data_[n];   }
  if(hash[n]==h) { dat = data_[n]; }
  d+=Dating();
      }
    }
  }
//SumPartialTime;

  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrSubSerie, "SubSer", 3, 3, "Serie Date Date",
  "(Serie S, Date D1, Date D2)",
  I2("Returns a series S with the values of a given series, between the "
     "dates D1 and D2. If the initial date of S is subsequent to D1, "
     "or well, its final date is prior to D2, the series will be "
     "truncated to konw dates.",
     "Devuelve una serie S con los valores de una serie dada, entre las "
     "fechas D1 y D2. Si la fecha inicial de S es posterior a D1, o bien, "
     "su fecha final es anterior a D2, la serie se truncará entre las "
     "fechas conocidas."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------


//--------------------------------------------------------------------
BTsrSubSerie::BTsrSubSerie(BList* arg)
//--------------------------------------------------------------------
: BTsrTemporary(arg)
{
  BUserTimeSerie* utmser = Tsr(Arg(1));
  BDate first = Date(Arg(2));
  BDate last  = Date(Arg(3));
  PutDating(DefaultDating(Ser()->Dating()));
  if(utmser->FirstDate().HasValue() && first<utmser->FirstDate())
  {
    first = utmser->FirstDate();
  }
  if(utmser->LastDate().HasValue() && last>utmser->LastDate())
  {
    last = utmser->LastDate();
  }
  if(first.HasValue() && Dating())
  {
    first = Dating()->FirstNoLess(first);
  }
  if(last.HasValue() && Dating())
  {
    last = Dating()->FirstNoGreat(last);
  }
  NCDate(firstDate_) = first;
  NCDate(lastDate_ ) = last;
  Do();
}

//--------------------------------------------------------------------
BUserTimeSerie* BTsrSubSerie::Ser () const
//--------------------------------------------------------------------
{
  return(Tsr(Arg(1)));
}


//--------------------------------------------------------------------
BDate   BTsrSubSerie::FirstDate() const
//--------------------------------------------------------------------
{
  return(firstDate_);
}

//--------------------------------------------------------------------
BDate   BTsrSubSerie::LastDate()  const
//--------------------------------------------------------------------
{
  return(lastDate_);
}

//--------------------------------------------------------------------
BDat BTsrSubSerie::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if((dte>=FirstDate()) && (dte<=LastDate()))
  {
    if(!IsStochastic())
    {
      dat =  Ser()->GetDat(dte);
    }
    else if(Data().HasValue()) 
    { 
      bool wasTheBegin = false;
      if(FirstDate()==BDate::Begin()) 
      {
        wasTheBegin = true;
        firstDate_ = BDate::DefaultFirst();
      }
      dat = data_[GetIndex(dte)]; 
      if(wasTheBegin) firstDate_ = BDate::Begin();
    }
    else
    {
      GetLength();
      Ser()->GetData(data_,FirstDate(),LastDate(),Length());
      
      BInt n=0;
      BDate d = FirstDate();
      if(d==BDate::Begin()) d = BDate::DefaultFirst();
      BHash hash; Dating()->GetHashBetween(hash, FirstDate(),LastDate());
      BReal h = dte.Hash();
      
      for(; n<hash.Size(); n++) 
      {
        hash[n] = d.Hash();
        if(hash[n]==h) { dat = data_[n]; }
        d+=Dating();
      }
      length_ = hash.Size();
      CompactData();
    }
  }
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrPolyn, ":", 2, 2, "Polyn Serie",
  "P:S  {Polyn P, Serie S}",
  I2("Applies a polynomial P of delay to a series."
     "Each monomial c*++B^d  of P is applied delaying the date in the dating "
     "of the series d periods  and multiplying the value of the series in "
     "the new date by c. Each monomial c*++F^d  of P is applied advancing "
     "the date in the dating of the series d periods  and multiplying the "
     "value of the series in the new date by c. Finally, to apply a "
     "polynomial to a series are added the results of applying to such "
     "series each one of their/its monomials. So that the calculation will "
     "be efficient is convenient to use this operation only with series with "
     "fixed dates. (Always they can be fixed the dates of a series with the "
     "function SubSer). Nótese that if P consists of an only monomial "
     "simply is moved the series and is multiplied by a number, while if P "
     "is formed by two or more monomials is lost a number of data that it "
     "is equal to the difference between the maximum degree and the minimal "
     "degree.",
     "Aplica un polinomio P de retardo a una serie."
     "Cada monomio c*B^d de P se aplica retrasando la fecha en el fechado "
     "de la serie d períodos  y multiplicando el valor de la serie en la "
     "nueva fecha por c. Cada monomio c*F^d  de P se aplica adelantando la "
     "fecha en el fechado de la serie d períodos  y multiplicando el valor "
     "de la serie en la nueva fecha por c. Por último, para aplicar un "
     "polinomio a una serie se suman los resultados de aplicar a dicha "
     "serie cada uno de sus monomios. Para que el cálculo sea eficiente "
     "es conveniente utilizar esta operación solamente con series con fechas "
     "fijadas. (Siempre se pueden fijar las fechas de una serie con la "
     "función SubSer). Nótese que si P consta de un sólo monomio simplemente "
     "se traslada la serie y se multiplica por un número, mientras que si P "
     "está formado por dos o más monomios se pierde un número de datos que "
     "es igual a la diferencia entre el grado máximo y el grado mínimo."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BTsrPolyn::BTsrPolyn(const BPol& pol, BUserTimeSerie* ser)
//--------------------------------------------------------------------
    : BTsrTemporary(),
      pol_(pol),
      ser_(ser),
      backward_(pol_.Degree()),
      foreward_(pol_[0].Degree())
{
    PutDating(ser_->Dating());
    PutFirstDate(GetFirstDate());
    PutLastDate (GetLastDate());
    GetLength();
}


//--------------------------------------------------------------------
BDat BTsrPolyn::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
//InitTotalTime("BTsrPolyn::GetDat");
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(!Length())
  {
    dat = 0.0;
    for(BInt n = 0; n<pol_.Size(); n++)
    {
      BDate d = ser_->Dating()->Prev(dte, pol_[n].Degree());
      BDat  x = (*ser_)[d];
      dat    += pol_[n].Coef()*x;
    }
  }
  else if(Data().HasValue()) { dat = data_[GetIndex(dte)]; }
  else if((dte>=firstDate_) && dte<=lastDate_)
  {
    BData serData;
    ser_->GetData(serData);


    BHash hash; Dating()->GetHashBetween(hash, firstDate_,lastDate_);
    Realloc(hash.Size());
    BInt  n;
    BDate d;
    BReal h = dte.Hash();

    for(n=0; n<Length(); n++)
    {
      data_[n] = 0.0;
      for(BInt p = 0; p<pol_.Size(); p++)
      {
  BInt m    = n-pol_[p].Degree()+backward_;
  BDat  x    = serData[m];
  data_[n] += pol_[p].Coef()*x;
      }
      if(hash[n]==h) { dat = data_[n]; }

    }
    CompactData();
  }
//SumPartialTime;
  return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrDifEq, "DifEq", 2, 3, "Ratio Serie Serie",
  "(Ratio R, Serie S, Serie I)",
  I2("Returns the series Z solution of the equation in differences \n\n"

     "Q(B):Z = P(B):S \n\n"

     "where \n\n"

     "  R(B)=P(B)/Q(B) \n\n"

     ", or said of other manner, \n\n"

     "Z(t) + q1*Z(t-1) + ... +  qs*Z(t-s) = S(t) + p1*S(t-1) + pr*S(t-r) \n\n"

     "Of this equation is obtained \n\n"

     "Z(t)  =  S(t) + p1*S(t-1) + pr*S(t-s) - (q1*Z(t-1) + ... "
     "+   qs*Z(t-r)) \n\n"

     ",that is to say, \n\n"

     "Z = P(B):S + (1- Q(B)):Z \n\n"

     "It is clear that it can be calculated each value of Z once they are "
     "known r previous values of Z ."
     "The series I gives the r necessary initial values to calculate the "
     "first r values of Z, therefore it must have data for the r dates prior "
     "to the initial date of S. Obviously, the solution will will have the "
     "calculation dates of S, therefore S can not be an infinite series.(If "
     "it does not occur such is necessary to cut the series with the "
     "function SubSer.) "
     "If is not used the third argument is understood I as zero.",


     "Devuelve la serie Z solución de la ecuación en diferencias \n\n"

     "Q(B):Z = P(B):S \n\n"

     "donde \n\n"

     "  R(B)=P(B)/Q(B) \n\n"

     ", o dicho de otro modo, \n\n"

     "Z(t) + q1*Z(t-1) + ... +  qs*Z(t-s) = S(t) + p1*S(t-1) + pr*S(t-r) \n\n"

     "De esta ecuación se obtiene \n\n"

     "Z(t)  =  S(t) + p1*S(t-1) + pr*S(t-s) - (q1*Z(t-1) + ... +  "
     "qs*Z(t-r)) \n\n"

     ",es decir, \n\n"

     "Z = P(B):S + (1- Q(B)):Z \n\n"

     "Es claro que se puede calcular cada valor de Z una vez son "
     "conocidos r valores anteriores de Z . "
     "La serie I da los r valores iniciales necesarios para calcular los "
     "primeros r valores de Z, por lo que debe tener datos para las r "
     "fechas anteriores a la fecha inicial de S. Obviamente, la solución "
     "tendrá las fechas de cálculo de S, por lo que S no puede ser una "
     "serie infinita.(Si no ocurre así es necesario cortar la serie con la "
     "función SubSer.) "
     "Si no se utiliza el tercer argumento se sobreentiende I como cero." ),
    BOperClassify::TimeSeriesAlgebra_);

//--------------------------------------------------------------------
BDat BTsrDifEq::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
//nitTotalTime("BTsrDifEq::GetDat");
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(firstDate_.HasValue() && lastDate_.HasValue() &&
     (dte>=firstDate_) && (dte<=lastDate_))
  {
    if(Data().HasValue()) { dat = data_[GetIndex(dte)]; }
    else
    {
      BInt n=0;
      BHash hash; Dating()->GetHashBetween(hash, firstDate_,lastDate_);
      Realloc(hash.Size());
      BReal h0 = ser_->FirstDate().Hash();
      BReal h  = dte.Hash();

      for(n=0; (n<Length()) && (hash[n]<h0); n++)
      {
  data_[n] = (*ini_)[HashToDte(hash[n])];
  if(hash[n]==h) { dat = data_[n]; }
      }

      for(; n<Length(); n++)
      {
  data_[n] = (*ser_)[HashToDte(hash[n])];
  for(BInt p = 0; p<pol_.Size(); p++)
  {
    BInt m    = n-pol_[p].Degree();
    BDat  x   = data_[m];
    data_[n] += pol_[p].Coef()*x;
  }
  if(hash[n]==h) { dat = data_[n]; }
      }
      CompactData();
    }
  }
//SumPartialTime;
  return(dat);
}


//--------------------------------------------------------------------
void BTsrConcat::CalcDating  () 
//--------------------------------------------------------------------
{
  if(LeftSer()&&RightSer())
  {
    BUserTimeSet* left  = LeftSer ()->Dating();
    BUserTimeSet* right = RightSer()->Dating();
    BUserTimeSet* dating = NULL;
    dating = left;
    if(!dating) { dating = right; }
    else if(dating!=right)
    {
      if(!dating->IsCompatibleWith(*right))
      {
        dating = NULL;
      }
    }
    if(!dating)
    {
      BText no_dating = "NO_DATING";
      BText lName = left ?left ->Identify():no_dating;
      BText rName = right?right->Identify():no_dating;
      Error(I2("Cannot concat series with incompatible dating or without "
         "dating.",
         "No se puede concatenar series con fechado incompatible "
         "o sin fechado.")+
      "\nSeries:\n" +
      LeftSer ()->Identify() + " -> "+lName+";\n"+
      RightSer()->Identify() + " -> "+rName+";\n");
    }
    PutDating(dating);
  }
}


//--------------------------------------------------------------------
BDate BTsrCenterConcat::FirstDate() const
//--------------------------------------------------------------------
{
  if(firstDate_.HasValue()) { return(firstDate_); }
  if(Dating()) { NCDate(firstDate_)=LeftSer()->FirstDate(); }
  return(firstDate_);
}

//--------------------------------------------------------------------
BDate BTsrCenterConcat::LastDate()  const
//--------------------------------------------------------------------
{
  if(lastDate_.HasValue()) { return(lastDate_); }
  if(Dating()) { NCDate(lastDate_)=RightSer()->LastDate(); }
  return(lastDate_);
}


//--------------------------------------------------------------------
DefExtOpr(1, BTsrCenterConcat, "Concat", 3, 3, "Serie Serie Date",
  "(Serie S1, Serie S2, Date D)",
  I2("Returns a series with the values of S1 for each previous date or just "
     "as D and those of the S2 in the rest.",
     "Devuelve una serie con los valores de S1 para cada fecha anterior o "
     "igual que D y los de la S2 en el resto."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrCenterConcat::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  if(!LeftSer() || !RightSer()) { return(BDat::Unknown()); }
  if(dte<=Center())    { return((*LeftSer()) [dte]); }
  else        { return((*RightSer())[dte]); }
}


//--------------------------------------------------------------------
BDate BTsrLeftConcat::FirstDate() const
//--------------------------------------------------------------------
{
  if(firstDate_.HasValue()) { return(firstDate_); }
  if(Dating()) 
  { 
    BDate r = RightSer()->FirstDate();
    BDate l = LeftSer ()->FirstDate();
    NCDate(firstDate_)= Minimum(r,l); 
  }
  return(firstDate_);
}

//--------------------------------------------------------------------
BDate BTsrLeftConcat::LastDate()  const
//--------------------------------------------------------------------
{
  if(lastDate_.HasValue()) { return(lastDate_); }
  if(Dating()) { NCDate(lastDate_)=RightSer()->LastDate(); }
  return(lastDate_);
}

//--------------------------------------------------------------------
DefExtOpr(1, BTsrLeftConcat, "<<", 2, 2, "Serie Serie",
  I2("leftSerie << rightSerie  {Serie leftSerie, Serie rightSerie}",
     "(Serie serieIzquierda, Serie serieDerecha)"),
  I2("Returns a series with the values of the S2 for each subsequent or "
     "equal date to its initial date and those of S1 in the rest.",
     "Devuelve una serie con los valores de la S2 para cada fecha posterior "
     "o igual a su fecha inicial y los de S1 en el resto."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrLeftConcat::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  if(!LeftSer() || !RightSer())     { return(BDat::Unknown()); }
  if(dte>=RightSer()->FirstDate()) { return((*RightSer())[dte]); }
  else           { return((*LeftSer ())[dte]); }
}


//--------------------------------------------------------------------
BDate BTsrRightConcat::FirstDate() const
//--------------------------------------------------------------------
{
  if(firstDate_.HasValue()) { return(firstDate_); }
  if(Dating()) { NCDate(firstDate_)=LeftSer()->FirstDate(); }
  return(firstDate_);
}

//--------------------------------------------------------------------
BDate BTsrRightConcat::LastDate()  const
//--------------------------------------------------------------------
{
  if(lastDate_.HasValue()) { return(lastDate_); }
  if(Dating()) 
  { 
    BDate r = RightSer()->LastDate();
    BDate l = LeftSer ()->LastDate();
    NCDate(lastDate_)= Maximum(r,l); 
  }
  return(lastDate_);
}

//--------------------------------------------------------------------
DefExtOpr(1, BTsrRightConcat, ">>", 2, 2, "Serie Serie",
  I2("leftSerie >> rightSerie  {Serie leftSerie, Serie rightSerie}",
     "(Serie serieIzquierda, Serie serieDerecha)"),
  I2("Returns a series with the values of S1 each previous or equal date to "
     "its final date and those of S2 in the rest.",
     "Devuelve una serie con los valores de S1 cada fecha anterior o igual "
     "a su fecha final y los de S2 en el resto."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrRightConcat::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  if(!LeftSer() || !RightSer())   { return(BDat::Unknown()); }
  if(dte<=LeftSer()->LastDate()) { return((*LeftSer ())[dte]); }
  else         { return((*RightSer())[dte]); }
}


//--------------------------------------------------------------------
class BTsrOmmitFilter : public BTsrTemporary
//--------------------------------------------------------------------
{
private:
  BRat      rat_;
  BUserTimeSerie* ser_;
  BUserTimeSerie* ommit_;
public:
  BTsrOmmitFilter(BList* arg) : BTsrTemporary(arg)
  {
    rat_   = Rat(Arg(1)),
    ser_   = Tsr(Arg(2)),
    ommit_ = Tsr(Arg(3)),
    PutDating  (ser_->Dating());
    PutFirstDate(ser_->FirstDate());
    PutLastDate (ser_->LastDate ());
  }
 ~BTsrOmmitFilter() {}
  BDat operator[](const BDate& dte)
  {
    if(!Dating()) { return(BDat::Unknown()); }
    return(GetDat(dte));
  }
  BDat GetDat(const BDate& dte);
};
DeclareEvaluator(BTsrOmmitFilter);
//--------------------------------------------------------------------
DefExtOpr(1, BTsrOmmitFilter, "OmmitFilter", 3, 3, "Ratio Serie Serie",
  I2("(Ratio piWeights, Serie residuous, Serie closed)",
     "(Ratio piWeights, Serie residuos, Serie cerrado)"),
  I2("Filters omitted values from a serie of residues .",
     "Filtra valores omitidos de una serie de residuos."),
    BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BTsrOmmitFilter::GetDat(const BDate& dte)
//--------------------------------------------------------------------
{
  if(!Dating()) { return(BDat::Unknown()); }
  BDat dat;
  if(Data().HasValue()) { dat = data_[GetIndex(dte)]; }
  else
  {
    BInt len = GetLength();
    Realloc(len);
    BDate d;
    BInt  n;
    BInt  ind=-1;
    BData expand(len);
    BPol  p  = rat_ % (len-1);
    BHash hash; Dating()->GetHashBetween(hash, FirstDate(),LastDate());
    BReal h = dte.Hash();
  //Std("\nExpansion = ");
    for(n=0; n<len; n++)
    {
      expand[n] = p.Coef(n);
      data_[n]=(*ser_)[HashToDte(hash[n])];
    //Std(expand[n].Name());
    //Std(data_[n].Name());
    }
    for(n=0; n<len; n++)
    {
    //Std(BText("\nOmmit[")+d+"]="+(*ommit_)[d]);
      if((*ommit_)[d]!=0)
      {
        BDat pivot = data_[n];
        for(BInt m=n; m<Length(); m++)
        {
          data_[m] -= expand[m-n]*pivot;
        }
    //Std(BText("\ndata_[")+n+"]="+data_[n].Name());
      }
      if(hash[n]==h) { ind = n; }
    }
    if(ind>=0) { dat = data_[ind]; }
  }
  return(dat);
}
