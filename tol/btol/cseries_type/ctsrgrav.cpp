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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bctsrgrav.h>
//#include <tol/tol_bctsrgrp.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bctmigra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bout.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("ctsrgrav.cpp");

//--------------------------------------------------------------------
// non const date reference
//--------------------------------------------------------------------
//#define NCDate(tmi) (*((BCTime*)(&tmi)))

bool ForzeLinkerCTsrGraVarious() { return(BTRUE); }

//--------------------------------------------------------------------
BUserCTimeSet* DefaultCDating(BSyntaxObject* dating)
{
    static BUserCTimeSet* defaultDating = NIL;
    if(defaultDating==NIL) { defaultDating = CTms("CTInDays"); }

    if(dating) { return CTms(dating);  }
    else       { return defaultDating; }
}



//--------------------------------------------------------------------
DefExtOpr(1, BCTsrRandom, "Rand", 2, 3, "Real Real CTimeSet",
	  I2("(Real min, Real max [, CTimeSet dating])",
	     "(Real min, Real max [, CTimeSet fechado])"),
	  I2("Returns an uniform distributed random serie U(min,max).",
	     "Devuelve una serie aleatoria con distribución uniforme "
	     "U(min,max)."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrRandom::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BUniformDist var(Real(Arg(1)),Real(Arg(2)));
    return(var.Random());
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrGaussian, "Gaussian", 2, 3, "Real Real CTimeSet",
	  I2("(Real nu, Real sigma [, CTimeSet dating])",
	     "(Real nu, Real sigma [, CTimeSet fechado])"),
	  I2("Returns an normal distributed random serie "
	     "N[nu, sigma].",
	     "Devuelve una serie aleatoria con distribución normal "
	     "N[nu, sigma]."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BCTsrGaussian::BCTsrGaussian(BList* arg) 
    : BCTsrTemporary(arg), 
      dist_(Real(Arg(1)), Real(Arg(2)))
{}

//--------------------------------------------------------------------
BDat BCTsrGaussian::GetDat(const BCTime& tmi)
{
    BDat newValue;
    std::map<BCTime, BDat>::const_iterator keyIterator;
    
    if(!Dating()) newValue = BDat::Unknown();
    else {
	keyIterator = valuesMap_.find(tmi);
	if(keyIterator == valuesMap_.end()) {
	    newValue = dist_.Random();
	    valuesMap_.insert(std::pair<BCTime, BDat>(tmi, newValue));
	} else newValue = keyIterator->second;
    }
    
    return(newValue);
}


//--------------------------------------------------------------------
void BCTsrDummy::CalcObject()
{
    BUserCTimeSet* dating = NIL;
    if(Arg(2))   { dating = CTms(Arg(2)); }
    if(!dating)  { dating = CTms("CTInDays"); }
    PutDating(dating);
    center_ = CTime(Arg(1));
    postCenter_ = Center() + Dating();
    
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrPulse, "Pulse", 1, 2, "CTime CTimeSet",
	  I2("(CTime ctime, CTimeSet dating)",
	     "(CTime ctime, CTimeSet fechado)"),
	  I2("Returns a series in a dated given with value one for the "
	     "given date and zero in any other date.",
	     "Devuelve una serie en un fechado dado con valor uno para "
	     "la fecha dada y cero en cualquier otra fecha.")+
	  "(..., 0,0,0,<1>,0,0,0 ...)",
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrPulse::GetDat(const BCTime& tmi)
{
    if(!Dating()) return BDat::Unknown();

    if((tmi==Center()) || Center().includes(tmi)) 
    { return BDat(1.0); }
    else 
    { return BDat(0.0); }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrCompens, "Compens", 1, 2, "CTime CTimeSet",
	  I2("(CTime ctime, CTimeSet dating)",
	     "(CTime ctime, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, with value 1 in a "
	     "given date, - 1 in the following, and 0 in the rest",
	     "Devuelve una serie en el fechado dado, con valor 1 en una "
	     "fecha dada, -1 en la siguiente, y 0 en el resto")+
	  "(..., 0,0,0,<1>,-1,0,0 ...)",
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrCompens::GetDat(const BCTime& tmi)
{
    if(!Dating()) return(BDat::Unknown());

    if((tmi == Center()) || Center().includes(tmi)) 
    { return(BDat( 1.0)); } 
    else if((tmi == PostCenter()) || PostCenter().includes(tmi)) 
    { return(BDat(-1.0)); }
    else 
    { return(BDat( 0.0)); }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrStep, "Step", 1, 2, "CTime CTimeSet",
	  I2("(CTime ctime, CTimeSet dating)",
	     "(CTime ctime, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, with value zero for "
	     "each date less than a given date and one in the rest.",
	     "Devuelve una serie en el fechado dado, con valor cero para "
	     "cada fecha menor que una fecha dada y uno en el resto.")+
	  "(..., 0,0,0,<1>,1,1,1 ...).",
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrStep::GetDat(const BCTime& tmi)
{
    if(!Dating())       { return(BDat::Unknown()); }
    if(tmi >= Center()) { return(BDat(1.0));       }
    else		{ return(BDat(0.0));       }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrTrend, "Trend", 1, 2, "CTime CTimeSet",
	  I2("(CTime ctime, CTimeSet dating)",
	     "(CTime ctime, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, with value zero for "
	     "each CTime less than or included in the given CTime, and that "
	     "goes increasing by one starting in such CTime.",
	     "Devuelve una serie en el fechado dado, con valor cero para "
	     "cada CTime menor que el CTime dado, y que se va "
	     "incrementando en uno a partir de dicho CTime.") +
	  "(..., 0,0,0,<1>,2,3,4 ...).",
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrTrend::GetDat(const BCTime& tmi)
{
    BCTime aux(Center());
    if(aux.getGranul()>Dating()->getGranul()) 
	aux.setGranul(Dating()->getGranul());

    if(!Dating()) { return(BDat::Unknown()); }

    if(tmi < aux)
	return(BDat(0.0)); 
    else {
	aux = Dating()->nonSmaller(aux);
	return(1 + Dating()->distance(aux, tmi));
    }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrLine, "Line", 4, 5, "CTime Real CTime Real CTimeSet",
	  I2("(CTime t1, Real x1, CTime t2, Real x2, CTimeSet dating)",
	     "(CTime t1, Real x1, CTime t2, Real x2, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, with values in the "
	     "rectum that goes through two points whose coordinates "
	     "are (d1, x1) and (d2,x2).",
	     "Devuelve una serie en el fechado dado, con valores en la "
	     "recta que pasa por dos puntos cuyas coordenadas son "
	     "(d1, x1) y (d2,x2)."),
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrLine::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(t0!=t1)
	if(y1!=y0)
	{
	    BReal pendient = (y1-y0)/Dating()->distance(t0, t1);
	    dat = y0 + pendient * Dating()->distance(t0, tmi);
	}
	else dat = y0;

    return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrCalendary, "CalVar", 1, 2, "CTimeSet CTimeSet",
	  I2("(CTimeSet T, CTimeSet dating)",
	     "(CTimeSet T, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, whose value in each "
	     "date is the cardinal of T between such date and its "
	     "succeeding date within dating.",
	     "Devuelve una serie en el fechado dado, cuyo valor en cada "
	     "fecha es el cardinal de T entre dicha fecha y su fecha "
	     "sucesora dentro del fechado."),
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrCalendary::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    
    if(center_)
    {
	BCTime from(tmi);
//	BCTime next = (tmi + Dating());
	BCTime next = Dating()->maximalSucc(tmi);

	if(from.getGranul() > center_->getGranul())
	    from.setGranul(center_->getGranul());

	if(next.getGranul() > center_->getGranul())
	    next.setGranul(center_->getGranul());

	dat = center_->distance(from, next);
    }

    return(dat);
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrIndicator, "CalInd", 1, 2, "CTimeSet CTimeSet",
	  I2("(CTimeSet ctms, CTimeSet dating)",
	     "(CTimeSet conjuntoTemporal, CTimeSet fechado)"),
	  I2("Returns a series in the given dating, whose value in each "
	     "CTime is one if belongs to T and zero in other case.",
	     "Devuelve una serie en el fechado dado, cuyo valor en "
	     "cada CTime es uno si pertenece a T y cero en otro caso."),
	  BOperClassify::CalendarVariables_);
//--------------------------------------------------------------------
BDat BCTsrIndicator::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(center_)
    {
	dat = center_->includes(tmi);
    }
    return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrDatingChange, "DatCh", 3, 3, "CSeries CTimeSet Code",
	  I2("(CSeries S, CTimeSet dating, Code statistic)",
	     "(CSeries S, CTimeSet fechado, Code estadistico)"),
	  I2("Transform a series into other in the given dating, whose value "
	     "in each date is the application of a statistics function to "
	     "the original series between such date and its succeeding date "
	     "within dating. So that the operation is accomplished correctly "
	     "it is necessary that the exit dating will be a subset of the "
	     "entry dating, or said of other manner, that it will be harmonic "
	     "with this. Some of the most used statistics are the first "
	     "(FirstS), the sum (SumS), the average (AvrS), ...",
	     "Transforma una serie en otra en el fechado dado, cuyo valor en "
	     "cada fecha es la aplicación de una función estadística a la "
	     "serie original entre dicha fecha y su fecha sucesora dentro del "
	     "fechado. Para que la operación se realize correctamente es "
	     "necesario que el fechado de salida sea un subconjunto del "
	     "fechado de entrada, o dicho de otro modo, que sea armónico con "
	     "aquél. Algunos de los estadísticos más usados son el primero "
	     "(FirstS), la suma (SumS), la media (AvrS), ..."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BCTsrDatingChange::CalcObject()
{
    harmonical_ = true;
    if(//(Stat()->MinArg()!=1)			 ||
	(Stat()->MaxArg()!=3)			      ||
	(Stat()->Grammar()->Name()!="Real")	      ||
	(Stat()->GrammarForArg(1)->Name()!="CSeries") ||
	(Stat()->GrammarForArg(2)->Name()!="CTime")   ||
	(Stat()->GrammarForArg(2)->Name()!="CTime"))
    {
	Error(Stat()->Name()+
	      I2(" is not a valid statistic for time series.",
		 " no es un estadístico válido para series temporales."));
    }
}

//--------------------------------------------------------------------
BCTime BCTsrDatingChange::FirstCTime()
{
    if(firstCTime_.isNormal()) { return(firstCTime_); }
    if(Ser() && Ser()->Dating()) 
    { firstCTime_ = Dating()->nonGreater(Ser()->FirstCTime()); }

    return firstCTime_;
}

//--------------------------------------------------------------------
BCTime BCTsrDatingChange::LastCTime()
{
    if(lastCTime_.isNormal()) { return(lastCTime_); }
    if(Ser() && Ser()->Dating())
    { lastCTime_ = Dating()->nonGreater(Ser()->LastCTime()); }

    return lastCTime_;
}

//--------------------------------------------------------------------
BStandardOperator* BCTsrDatingChange::Stat()  const
{
    return(Code(Arg(3)).Operator());
}

//--------------------------------------------------------------------
BDat BCTsrDatingChange::GetDat(const BCTime& tmi)
{
    if(!Dating()||!Ser()||!Ser()->Dating()) { return(BDat::Unknown()); }
    BCTime tmi2 = tmi  + Dating();
           tmi2 = tmi2 - Ser()->Dating();

    BUserCTime* uTmi1 = new BUserCTime("",tmi ,"");
    BUserCTime* uTmi2 = new BUserCTime("",tmi2,"");
    
    BList* lst = Cons(Ser(),Cons(uTmi1,NCons(uTmi2)));
    BDatStatistic* uDat = (BDatStatistic*)(Stat()->Evaluator(lst));
    BDat dat = uDat->Contens();
    DESTROY(uDat);

    return dat;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrInverseDatingChange, "InvCh", 2, 2, "CSeries CSeries",
	  "(CSeries S1, CSeries S2)",
	  I2("It is defined T1 as the dating of the series S1, and T2 as the "
	     "dating of the series S2. For each CTime of T2 that belongs to "
	     "T1, is returned the value of S1 in such CTime. For each CTime "
	     "of T2 that not belongs to T1, is returned the value of S2 in "
	     "such CTime. Returns the value in second serie.",
	     "Se define T1 como el fechado de la serie S1, y T2 como el "
	     "fechado de la serie S2. Para cada CTime de T2 que pertenezca "
	     "a T1, se devuelve el valor de S1 en dicho CTime. Para cada "
	     "CTime de T2 que no pertenezca a T1, se devuelve el valor de "
	     "S2 en dicho CTime."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BCTime BCTsrInverseDatingChange::FirstCTime()
{
    if(firstCTime_.isNormal()) { return(firstCTime_); }
    if(Ser(1) && Dating())
    { firstCTime_ = Dating()->nonSmaller(Ser(1)->FirstCTime()); }

    return firstCTime_;
}

//--------------------------------------------------------------------
BCTime BCTsrInverseDatingChange::LastCTime()
{
    if(lastCTime_.isNormal()) { return(lastCTime_); }
    if(Ser(1) && Dating())
    { lastCTime_ = Dating()->nonGreater(Ser(1)->LastCTime()); }

    return lastCTime_;
}

//--------------------------------------------------------------------
BDat BCTsrInverseDatingChange::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(Ser(1) && Ser(2))
    {
	if(Ser(1)->Dating()->includes(tmi)) { dat = (*Ser(1))[tmi]; }
	else				    { dat = (*Ser(2))[tmi]; }
    }
    return(dat);
}

// cExpand doesn't works, as the older function Expand (in tsrgrav.cpp)
//--------------------------------------------------------------------
DefExtOpr(1, BCTsrExpand, "Expand", 1, 2, "CSeries Real",
	  "(CSeries S, Real val)",
	  I2("Transform a series through the replacement of each equal "
	     "value to x by the last different value of x.",
	     "Transforma una serie mediante el	reemplazo de cada valor "
	     "igual a x por el último valor diferente de x."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrExpand::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }

    BDat dat;

    if(Dating() && (tmi>=FirstCTime()) && tmi<=LastCTime())
    {
	if(Data().HasValue()) { dat = data_[GetIndex(tmi)]; }
	else
	{
	    int n = 0;
	    BCTime d = FirstCTime();
	    BDat lastNotEqual = val_;
	    
	    GetLength();
	    Ser()->GetData(data_,FirstCTime(),LastCTime(),Length());
	    
	    for(n=0; n<Length(); n++)
	    {
		if(data_[n] == val_) 
		    data_[n] = lastNotEqual;
		else lastNotEqual = data_[n];
		
		if(d==tmi) { dat = data_[n]; }
		d += Dating();
	    }
	}
    }
    
    return dat;
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrSubSerie, "SubSer", 3, 3, "CSeries CTime CTime",
	  "(CSeries S, CTime t1, CTime t2)",
	  I2("Returns a series S with the values of a given series, between "
	     "the CTimes t1 and t2. If the initial CTime of S is subsequent "
	     "to t1, or well, its final CTime is prior to t2, is returned "
	     "the value done not know (?) for each lost CTime.",
	     "Devuelve una serie S con los valores de una serie dada, entre "
	     "los CTime t1 y t2. Si la fecha inicial de S es posterior a t1, "
	     "o bien, su fecha final es anterior a t2, se devuelve el valor "
	     "desconocido (?) para cada CTime perdido."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BCTsrSubSerie::BCTsrSubSerie(BList* arg)
    : BCTsrTemporary(arg)
{
    BUserCTimeSeries* uctmser = CTsr(Arg(1));
    BCTime first = CTime(Arg(2));
    BCTime last = CTime(Arg(3));
    
    if(uctmser->FirstCTime().isNormal() && first<uctmser->FirstCTime())
	firstCTime_ = uctmser->FirstCTime();

    if(uctmser->LastCTime().isNormal() && last>uctmser->LastCTime())
	lastCTime_ = uctmser->LastCTime();
}

//--------------------------------------------------------------------
BUserCTimeSeries* BCTsrSubSerie::Ser() const
{
    return(CTsr(Arg(1)));
}

//--------------------------------------------------------------------
BUserCTimeSet* BCTsrSubSerie::Dating() const
{
    return(DefaultCDating(Ser()->Dating()));
}

//--------------------------------------------------------------------
BCTime BCTsrSubSerie::FirstCTime()
{
    if(firstCTime_.isNormal()) { return(firstCTime_); }
    if(Dating()) { firstCTime_ = Dating()->nonSmaller(CTime(Arg(2))); }
    return firstCTime_;
}

//--------------------------------------------------------------------
BCTime BCTsrSubSerie::LastCTime()
{
    if(lastCTime_.isNormal()) { return(lastCTime_); }
    if(Dating()) { lastCTime_ = Dating()->nonGreater(CTime(Arg(3))); }
    return lastCTime_;
}

//--------------------------------------------------------------------
BDat BCTsrSubSerie::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;

    if((tmi>=FirstCTime()) && (tmi<=LastCTime()))
    {
	if(Data().HasValue()) 
	{ 
	    bool wasTheBegin = false;
	    
	    if(FirstCTime()==BCTime::Begin()) {
		wasTheBegin = true;
		firstCTime_ = BCTime::fillNew(Dating()->getGranul(), LV,
					      BCTime::Now().year());
	    }
	    
	    dat = data_[GetIndex(tmi)]; 
	    
	    if(wasTheBegin) firstCTime_ = BCTime::Begin();
	}
	else
	{
	    GetLength();
	    Ser()->GetData(data_,FirstCTime(),LastCTime(),Length());
	    
	    int n=0;
	    BCTime d = FirstCTime();
	    if(d==BCTime::Begin()) d = BCTime::fillNew(Dating()->getGranul(), 
						       LV, 
						       BCTime::Now().year());
	    
	    for(n=0; n<Length(); n++)
	    {
		if(d == tmi) { dat = data_[n]; }
		d+=Dating();
	    }

	    length_ = Dating()->distance(FirstCTime(), LastCTime());
	    CompactData();
	}
    }
    
    return(dat);
}


//--------------------------------------------------------------------
DefExtOpr(1, BCTsrPolyn, ":", 2, 2, "Polyn CSeries",
	  "P:S  {Polyn P, CSeries S}",
	  I2("Applies a polynomial P of delay to a series."
	     "Each monomial c*++B^d  of P is applied delaying the CTime in "
	     "the dating of the series d periods  and multiplying the value "
	     "of the series in the new CTime by c. Each monomial c*++F^d "
	     "of P is applied advancing the date in the dating of the "
	     "series d periods and multiplying the value of the series in "
	     "the new date by c. Finally, to apply a polynomial to a series "
	     "are added the results of applying to such series each one of "
	     "their/its monomials. So that the calculation will be efficient "
	     "is convenient to use this operation only with series with fixed "
	     "CTimes. (Always they can be fixed the dates of a series with "
	     "the function SubSer). Note that if P consists of an only "
	     "monomial simply is moved the series and is multiplied by a "
	     "number, while if P is formed by two or more monomials is lost "
	     "a number of data that it is equal to the difference between the "
	     "maximum degree and the minimal degree.",
	     "Aplica un polinomio P de retardo a una serie. Cada monomio "
	     "c*B^d de P se aplica retrasando la fecha en el fechado de la "
	     "serie d períodos  y multiplicando el valor de la serie en la "
	     "nueva fecha por c. Cada monomio c*F^d  de P se aplica "
	     "adelantando la fecha en el fechado de la serie d períodos y "
	     "multiplicando el valor de la serie en la nueva fecha por c. "
	     "Por último, para aplicar un polinomio a una serie se suman los "
	     "resultados de aplicar a dicha serie cada uno de sus monomios. "
	     "Para que el cálculo sea eficiente es conveniente utilizar esta "
	     "operación solamente con series con fechas fijadas. (Siempre "
	     "se pueden fijar las fechas de una serie con la función SubSer)."
	     " Nótese que si P consta de un sólo monomio simplemente se "
	     "traslada la serie y se multiplica por un número, mientras que "
	     "si P está formado por dos o más monomios se pierde un número de "
	     "datos que es igual a la diferencia entre el grado máximo y el "
	     "grado mínimo."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BCTsrPolyn::CalcObject()
{
    if(Arg(1))
    {
	pol_	  = Pol(Arg(1));
	ser_	  = CTsr(Arg(2));
	backward_ = pol_.Degree();
	foreward_ = pol_[0].Degree();
	PutDating(ser_->Dating());
	PutFirstCTime(GetFirstCTime());
	PutLastCTime (GetLastCTime());
	GetLength();
    }
}

//--------------------------------------------------------------------
BCTsrPolyn::BCTsrPolyn(const BPol& pol, BUserCTimeSeries* ser)
    : BCTsrTemporary(),
      pol_(pol),
      ser_(ser),
      backward_(pol_.Degree()),
      foreward_(pol_[0].Degree())
{
    PutDating(ser_->Dating());
    PutFirstCTime(GetFirstCTime());
    PutLastCTime (GetLastCTime());
    GetLength();
}

//--------------------------------------------------------------------
BDat BCTsrPolyn::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(!Length())
    {
	dat = 0.0;
	for(int n = 0; n<pol_.Size(); n++)
	{
	    BCTime d = ser_->Dating()->prev(tmi, pol_[n].Degree());
	    BDat  x = (*ser_)[d];
	    dat += pol_[n].Coef() * x;
	}
    }
    else if(Data().HasValue()) { dat = data_[GetIndex(tmi)]; }
    else if((tmi>=firstCTime_) && tmi<=lastCTime_)
    {
	BData serData;
	ser_->GetData(serData);
	
	data_.ReallocBuffer(1 + Dating()->distance(firstCTime_,lastCTime_));
	BCTime pivot(FirstCTime());

	for(int n=0; n<Length(); n++)
	{
	    data_[n] = 0.0;

	    for(int p=0; p<pol_.Size(); p++)
	    {
		int  m	  = n-pol_[p].Degree() + backward_;
		BDat x	  = serData[m];
		data_[n] += pol_[p].Coef() * x;
	    }

	    if(pivot==tmi) { dat = data_[n]; }
	    pivot += Dating();
	}
	
	CompactData();
    }

    return dat;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrDifEq, "DifEq", 2, 3, "Ratio CSeries CSeries",
	  "(Ratio R, CSeries S, CSeries I)",
	  I2("Returns the series Z solution of the equation in "
	     "differences \n\n"
	     "Q(B):Z = P(B):S \n\n"
	     "where \n\n"
	     "	R(B)=P(B)/Q(B) \n\n"
	     ", or said of other manner, \n\n"
	     "Z(t) + q1*Z(t-1) + ... +	qs*Z(t-s) = "
	     "S(t) + p1*S(t-1) + pr*S(t-r) \n\n"
	     "Of this equation is obtained \n\n"
	     "Z(t)  =  S(t) + p1*S(t-1) + pr*S(t-s) - (q1*Z(t-1) + ... "
	     "+	 qs*Z(t-r)) \n\n"
	     ",that is to say, \n\n"
	     "Z = P(B):S + (1- Q(B)):Z \n\n"
	     "It is clear that it can be calculated each value of Z once "
	     "they are known r previous values of Z. The series I gives "
	     "the r necessary initial values to calculate the first r "
	     "values of Z, therefore it must have data for the r dates prior "
	     "to the initial date of S. Obviously, the solution will will "
	     "have the calculation dates of S, therefore S can not be an "
	     "infinite series.(If it does not occur such is necessary to "
	     "cut the series with the function SubSer.) "
	     "If is not used the third argument is understood I as zero.",
	     "Devuelve la serie Z solución de la ecuación en diferencias \n\n"
	     "Q(B):Z = P(B):S \n\n"
	     "donde \n\n"
	     "	R(B)=P(B)/Q(B) \n\n"
	     ", o dicho de otro modo, \n\n"
	     "Z(t) + q1*Z(t-1) + ... +	qs*Z(t-s) = "
	     "S(t) + p1*S(t-1) + pr*S(t-r) \n\n"
	     "De esta ecuación se obtiene \n\n"
	     "Z(t)  =  S(t) + p1*S(t-1) + pr*S(t-s) - (q1*Z(t-1) + ... +  "
	     "qs*Z(t-r)) \n\n"
	     ",es decir, \n\n"
	     "Z = P(B):S + (1- Q(B)):Z \n\n"
	     "Es claro que se puede calcular cada valor de Z una vez son "
	     "conocidos r valores anteriores de Z . "
	     "La serie I da los r valores iniciales necesarios para calcular "
	     "los primeros r valores de Z, por lo que debe tener datos para "
	     "las r fechas anteriores a la fecha inicial de S. Obviamente, "
	     "la solución tendrá las fechas de cálculo de S, por lo que S no "
	     "puede ser una serie infinita.(Si no ocurre así es necesario "
	     "cortar la serie con la función SubSer.) "
	     "Si no se utiliza el tercer argumento se sobreentiende I como "
	     "cero." ),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BCTsrDifEq::CalcObject()
{
    if(Arg(1))
    {
	BRat& rat = Rat(Arg(1));
	pol_ = 1-rat.Denominator();
	ser_ = BCTsrPolyn(rat.Numerator(), CTsr(Arg(2)));
	ini_ = CTsr(Arg(3));
	if(!ini_) 
	{ 
	    ini_ = new BCTsrFromReal(BDat(0.0)); 
	    destroyIni_  = true;
	}
	PutDating(ser_.Dating());
	if(dating_)
	{
	    PutFirstCTime(dating_->prev(ser_.FirstCTime(), pol_.Degree()));
	    PutLastCTime(ser_.LastCTime());
	    GetLength();
	}
    }
}

//--------------------------------------------------------------------
BCTsrDifEq::BCTsrDifEq(const BRat& rat,
		       BUserCTimeSeries* ser,
		       BUserCTimeSeries* ini)
    : BCTsrTemporary(), destroyIni_(false)
{
    pol_ = 1-rat.Denominator();
    ser_ = BCTsrPolyn(rat.Numerator(), ser);
    ini_ = ini;
    if(!ini_) 
    { 
	ini_ = new BCTsrFromReal(BDat(0.0)); 
	destroyIni_  = true;
    }
    PutDating(ser_.Dating());
    if(Dating())
    {
	PutFirstCTime(Dating()->prev(ser_.FirstCTime(), pol_.Degree()));
	PutLastCTime (ser_.LastCTime());
	GetLength();
    }
}

//--------------------------------------------------------------------
BCTsrDifEq::~BCTsrDifEq()
{
    if(destroyIni_ && ini_) { DESTROY(ini_); }
}

//--------------------------------------------------------------------
BDat BCTsrDifEq::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(firstCTime_.isNormal() && lastCTime_.isNormal() &&
       (tmi>=firstCTime_) && (tmi<=lastCTime_))
    {
	if(Data().HasValue()) { dat = data_[GetIndex(tmi)]; }
	else
	{
	    int deg = pol_.Degree();
	    int n = 0;
	    int length = 1 + Dating()->distance(firstCTime_,lastCTime_);
	    data_.ReallocBuffer(length);
	    BCTime pivot(FirstCTime());
	    
	    for(n=0; (n<Length()) && (pivot<ser_.FirstCTime()); n++)
	    {
		data_[n] = (*ini_)[pivot];
		if(pivot==tmi) { dat = data_[n]; }
		pivot += Dating();
	    }
	    
	    for(; n<length; n++)
	    {
		data_[n] = ser_[pivot];
		for(int p = 0; p<pol_.Size(); p++)
		{
		    int m     = n-pol_[p].Degree();
		    BDat x    = Data()[m];
		    data_[n] += pol_[p].Coef()*x;
		}
		if(pivot==tmi) { dat = data_[n]; }
		pivot += Dating();
	    }

	    CompactData();
	}
    }

    return dat;
}


//--------------------------------------------------------------------
BUserCTimeSet* BCTsrConcat::Dating() const
{
    BUserCTimeSet* dating = NIL;
    if(LeftSer() && RightSer())
    {
	dating = LeftSer()->Dating();
	if(!dating) { dating = RightSer()->Dating(); }
	else if(dating!=RightSer()->Dating())
	    if(!dating->isCompatibleWith(*RightSer()->Dating()))
		dating = NIL;
	
	if(!dating)
	{
	    Error(I2("Cannot concat series with incompatible dating or "
		     "without dating.",
		     "No se puede concatenar series con fechado incompatible "
		     "o sin fechado.")+
		  "\nSeries:\n" +
		  LeftSer()->Identify() + ";\n"+
		  RightSer()->Identify() + ";\n");
	}
    }
    return(dating);
}


//--------------------------------------------------------------------
BCTime BCTsrConcat::FirstCTime()
{
    if(firstCTime_.isNormal()) { return(firstCTime_); }
    if(Dating()) { firstCTime_ = LeftSer()->FirstCTime(); }
    return firstCTime_;
}

//--------------------------------------------------------------------
BCTime BCTsrConcat::LastCTime()
{
    if(lastCTime_.isNormal()) { return(lastCTime_); }
    if(Dating()) { lastCTime_ = RightSer()->LastCTime(); }
    return lastCTime_;
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrCenterConcat, "Concat", 3, 3, "CSeries CSeries CTime",
	  "(CSeries S1, CSeries S2, CTime tmi)",
	  I2("Returns a series with the values of S1 for each previous CTime"
	     "or just as tmi and those of the S2 in the rest.",
	     "Devuelve una serie con los valores de S1 para cada CTime "
	     "anterior o igual que tmi y los de la S2 en el resto."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrCenterConcat::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    if(!LeftSer() || !RightSer()) { return(BDat::Unknown()); }
    if(tmi<=Center())		  { return((*LeftSer()) [tmi]); }
    else			  { return((*RightSer())[tmi]); }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrLeftConcat, "<<", 2, 2, "CSeries CSeries",
	  "leftCSeries << rightcSeries  "
	  "{CSeries leftSerie, CSeries rightSerie}",
	  I2("Returns a series with the values of the S2 for each subsequent "
	     "or equal CTime to its initial date and those of S1 in the rest.",
	     "Devuelve una serie con los valores de la S2 para cada CTime "
	     "posterior o igual a su CTime inicial y los de S1 en el resto."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrLeftConcat::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    if(!LeftSer() || !RightSer())     { return(BDat::Unknown());    }
    if(tmi>=RightSer()->FirstCTime()) { return((*RightSer())[tmi]); }
    else			      { return((*LeftSer ())[tmi]); }
}

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrRightConcat, ">>", 2, 2, "CSeries CSeries",
	  "leftSeries >> rightSeries  {CSeries leftSerie, CSeries rightSerie}",
	  I2("Returns a series with the values of S1 each previous or equal "
	     "CTime to its final date and those of S2 in the rest.",
	     "Devuelve una serie con los valores de S1 cada CTime anterior "
	     "o igual a su CTime final y los de S2 en el resto."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrRightConcat::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    if(!LeftSer() || !RightSer())   { return(BDat::Unknown());    }
    if(tmi<=LeftSer()->LastCTime()) { return((*LeftSer ())[tmi]); }
    else			    { return((*RightSer())[tmi]); }
}

//--------------------------------------------------------------------
class BCTsrOmmitFilter : public BCTsrTemporary
{
private:
    BRat              rat_;
    BUserCTimeSeries* ser_;
    BUserCTimeSeries* ommit_;
public:
    BCTsrOmmitFilter(BList* arg) : BCTsrTemporary(arg)
	{
	    rat_   = Rat(Arg(1));
	    ser_   = CTsr(Arg(2));
	    ommit_ = CTsr(Arg(3));
	    PutDating(ser_->Dating());
	    PutFirstCTime(ser_->FirstCTime());
	    PutLastCTime (ser_->LastCTime());
	}
    BDat operator[](const BCTime& tmi)
	{
	    if(!Dating()) { return(BDat::Unknown()); }
	    return(GetDat(tmi));
	}
    BDat GetDat(const BCTime& tmi);
};
DeclareEvaluator(BCTsrOmmitFilter);

//--------------------------------------------------------------------
DefExtOpr(1, BCTsrOmmitFilter, "OmmitFilter", 3, 3, "Ratio CSeries CSeries",
	  I2("(Ratio piWeights, CSeries residuous, CSeries closed)",
	     "(Ratio piWeights, CSeries residuos, CSeries cerrado)"),
	  I2("Filters omitted values from a serie of residues .",
	     "Filtra valores omitidos de una serie de residuos."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
BDat BCTsrOmmitFilter::GetDat(const BCTime& tmi)
{
    if(!Dating()) { return(BDat::Unknown()); }
    BDat dat;
    if(Data().HasValue()) { dat = data_[GetIndex(tmi)]; }
    else
    {
	int len = GetLength();
	data_.ReallocBuffer(len);
	int n, ind;
	BData expand(len);
	BPol p = rat_ % (len-1);
	BCTime d, pivot(FirstCTime());

	for(n=0; n<len; n++)
	{
	    expand[n] = p.Coef(n);
	    data_[n] = (*ser_)[pivot];
	    pivot += Dating();
	}
	
	pivot = FirstCTime();
	for(n=0; n<len; n++)
	{
	    if((*ommit_)[d]!=0)
	    {
		BDat pivot = data_[n];
		for(int m = n; m<Length(); m++) data_[m]-=expand[m-n]*pivot;
	    }
	    if(pivot==tmi) { ind = n; }
	    pivot += Dating();
	}
	dat = data_[ind];
    }

    return dat;
}

#endif // __USE_TC__
