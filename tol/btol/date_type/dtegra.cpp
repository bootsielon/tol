/* dtegra.cpp: Date data type functionality.
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

#include <tol/tol_bdtegra.h>
#include <tol/tol_btimer.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdir.h>


//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("dtegra.cpp");
template<>
BGrammar* BGraContensBase<BDate>::ownGrammar_ = NIL;


static BDateFormat dateFormat_[] =
{
    BDateFormat("y%Ym%md%dh%hi%is%s"),
    BDateFormat("a%Ym%md%dh%hi%is%s"),
    BDateFormat("%Y/%m/%d:%h:%i:%s")
};

static BInt dateFormatNumber_ = sizeof(dateFormat_) / sizeof(BDateFormat);

DefineContensCommonOperators(BDate, "Date");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BDate>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}

static BParamDate* f_ = NULL;
static BParamDate* l_ = NULL;

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<> 
void BGraContensBase<BDate>::InitInstances()
{
    BTraceInit("BGraContens<BDate>::InitInstances");
    BSystemDate* unknown_ = new BSystemDate
	(
	    "UnknownDate" ,
	    BDate::Unknown(),
	    I2("The unknown date",
	       "La fecha desconocida")
        );
    BSystemDate* b_  = new BSystemDate
	(
	    "TheBegin",
	    BDate::Begin(),
	    I2("The begining of the time",
	       "El comienzo de los tiempos")
	);
    BSystemDate* e_ = new BSystemDate
	(
	    "TheEnd"  ,
	    BDate::End(),
	    I2("The end of the time",
	       "El final de los tiempos")
	);
    f_  = new BParamDate
	(
	    "DefFirst",
	    BDate::DefaultFirst(),
	    I2("The default first date",
	       "La fecha inicial por defecto")
         );
    l_  = new BParamDate
	(
	    "DefLast" ,
	    BDate::DefaultLast(),
	    I2("The default last date",
	       "La fecha final por defecto")
	);
    
    BDteToday*  today_   = new BDteToday;
    BDteNow*    now_     = new BDteNow;
    BDteNowGmt* nowGmt_  = new BDteNowGmt;
    OwnGrammar()->PutDefect(unknown_);
}


//--------------------------------------------------------------------
template<> 
BSyntaxObject* BGraContensBase<BDate>::FindConstant (const BText& name)

/*! Returns a valid constant time set for name.
 */
//--------------------------------------------------------------------
{
    BUserDate* uDte = NIL;
    BDate dte;
    
    if((name.Length()>=5) &&
       (name(0)=='y')	  &&
       isdigit(name(1))	  &&
       isdigit(name(2))	  &&
       isdigit(name(3))	  &&
       isdigit(name(4))	    )
    {
	dte =  dateFormat_[0].TextToDate(name);
    }
  if(dte.IsTheEnd()) 
  { uDte = new BContensDate(BDate::End()); }
  else if(dte.IsTheBegin()) 
  { uDte = new BContensDate(BDate::Begin()); }
  else if(dte.HasValue()) { uDte = new BContensDate(dte); }
  //else { Error(name+" "+I2("is not a valid constant date","no es una fecha constante válida")); }
    return((BSyntaxObject*)uDte);
}




//--------------------------------------------------------------------
/*! Returns a valid date for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BDate>::Casting(BSyntaxObject* obj)
{
    if(!obj)			     { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContens<BDate>::New(FILE* fil)
{
    return 0;
}


//--------------------------------------------------------------------
// algebraic temporary class declarations
//--------------------------------------------------------------------


//--------------------------------------------------------------------
class BPutDefaultDates: public BExternalOperator
{
  public:
    BPutDefaultDates() : BExternalOperator
    (
      "PutDefaultDates",GraDate(),"Date Date", NIL,2,2,
      I2("(Date first, Date last)",
	 "(Date inicial, Date final)"),
      I2("Changes the initial and final dates by defect of the objects of "
	 "time that specify them.\n\n"
	 "Example:\n"
	 "Date PutDefaultDates(y1996m1d1,y2010m12d31);\n",
	 "Cambia las fechas inicial y final por defecto de "
	 "los objetos de tiempo que las precisen.\n\n"
	 "Ejemplo:\n"
	 "Date PutDefaultDates(y1996m1d1,y2010m12d31);\n"),
  BOperClassify::TimeSeriesAlgebra_
    )
    { IncNRefs(); }
    BSyntaxObject* Evaluator(BList* argTrees) const;
};

//--------------------------------------------------------------------
BSyntaxObject* BPutDefaultDates::Evaluator(BList* arg) const
//--------------------------------------------------------------------
{
    BDate d1 = Date(Car(arg));
    BDate d2 = Date(Car(Cdr(arg)));
    BDate::PutDefaultFirst(d1);
    BDate::PutDefaultLast (d2);
    f_->Contens()=d1;
    l_->Contens()=d2;
    DESTROY(arg);
    return(NIL);
}

#if !defined(USE_DELAY_INIT)
  static BPutDefaultDates putDefaultDates_;
#else

void * cloneputDefaultDates_()
{
  return new BPutDefaultDates;
}

static BPutDefaultDates * putDefaultDates_ = (BPutDefaultDates*)(__delay_init((void**)(&putDefaultDates_),
									      &cloneputDefaultDates_));

#endif

//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteMin);
DefIntOpr(1, BDteMin, "Min", 2, 0,
  I2("(Date date1, Date date2, [Date date3, ...])",
     "(Date fecha1, Date fecha2, [Date fecha3, ...])"),
  I2("Returns the minimal from among two or more dates.\n\n"
     "Example:\n"
     "Date x = Min(y1996m1d1, y1971m12d31, y1971m12d30, y1997m12d31);\n\n"
     "Result:\n"
     "x == y1971m12d30",
     "Devuelve la mínima de entre dos o más fechas.\n\n"
     "Ejemplo:\n"
     "Date x = Min(y1996m1d1, y1971m12d31, y1971m12d30, y1997m12d31);\n\n"
     "Resultado:\n"
     "x == y1971m12d30"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteMin::CalcContens()
//--------------------------------------------------------------------
{
    BUserDate* uDte;
    BInt	     n	  = 2;
    
    contens_  = Date(Arg(1));
    
    while((uDte = UDate(Arg(n++))))
    {
	if(contens_ > uDte->Contens())  { contens_ = uDte->Contens(); }
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteMax);
DefIntOpr(1, BDteMax, "Max", 2, 0,
  I2("(Date date1, Date date2, [Date date3, ...])",
     "(Date fecha1, Date fecha2, [Date fecha3, ...])"),
  I2("Returns the maximal from among two or more dates.\n\n"
     "Example:\n"
     "Date x = Min(y1996m1d1, y1971m12d31, y1971m12d30, y1997m12d31);\n\n"
     "Result:\n"
     "x == y1997m12d31",
     "Devuelve la máxima de entre dos o más fechas.\n\n"
     "Ejemplo:\n"
     "Date x = Min(y1996m1d1, y1971m12d31, y1971m12d30, y1997m12d31);\n\n"
     "Resultado:\n"
     "x == y1997m12d31"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteMax::CalcContens()
//--------------------------------------------------------------------
{
    BUserDate* uDte;
    BInt	     n	  = 2;
    
    contens_  = Date(Arg(1));
    while((uDte = UDate(Arg(n++))))
    {
	if(contens_ < uDte->Contens())  { contens_ = uDte->Contens(); }
    }
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteSuccessor);
DefExtOpr(1, BDteSuccessor, "Succ", 2,3, "Date TimeSet Real",
  I2("(Date date, TimeSet dating, [Real n=1])",
     "(Date fecha1, TimeSet fechado, [Real n=1])"),
  I2("Returns the umpteenth succeeding date of a date in a dating. If n is "
     "negative turns the umpteenth previous date.\n\n"
     "Example:\n"
     "Date x = Succ(y1996m1d1, Monthly, 5);\n\n"
     "Result:\n"
     "x == y1996m6d1",
     "Devuelve la enésima fecha posterior de una fecha en un fechado. Si n "
     "es negativo devuelve la enésima fecha anterior.\n\n"
     "Ejemplo:\n"
     "Date x = Succ(y1996m1d1, Mensual, 5);\n\n"
     "Resultado:\n"
     "x == y1996m6d1"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteSuccessor::CalcContens()
//--------------------------------------------------------------------
{
    BDate		dte = Date(Arg(1));
    BUserTimeSet* tms = Tms(Arg(2));
    BReal		num = 1;
    
    if(Arg(3)) { num = Real(Arg(3)); }
    contens_ = tms->Next(dte,(BInt)num);
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteFirst);
DefExtOpr(1, BDteFirst, "First", 1, 1, "Serie",
  "(Serie ser)",
  I2("Returns the first date of a serie.",
     "Devuelve la fecha inicial de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BDteFirst::CalcContens()
//--------------------------------------------------------------------
{
    BUserTimeSerie* tsr = Tsr(Arg(1));
    contens_ = tsr->FirstDate();
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteLast);
DefExtOpr(1, BDteLast, "Last", 1, 1, "Serie",
  "(Serie ser)",
  I2("Returns the last date of a serie.",
     "Devuelve la fecha final de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BDteLast::CalcContens()
//--------------------------------------------------------------------
{
    BUserTimeSerie* tsr = Tsr(Arg(1));
    contens_ = tsr->LastDate();
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteCreate);
DefExtOpr(1, BDteCreate, "YMD", 1, 6, "Real Real Real Real Real Real",
  I2("(Real year, "
     "[Real month=1, Real day=1, Real hour=0, Real minute=0, Real second=0])",
     "(Real año, "
     "[Real mes=1, Real dia=1, Real hora=0, Real minuto=0, Real segundo=0])"),
  I2("Builds a date from the corresponding coordinates.",
     "Construye una fecha a partir de sus coordenadas correspondientes."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteCreate::CalcContens()
//--------------------------------------------------------------------
{
    BInt	year = (BInt)Real(Arg(1)), month=1, day=1, hour=0, minute=0;
    BReal second = 0;
    if(Arg(2)) { month  = (BInt)Real(Arg(2)); }
    if(Arg(3)) { day    = (BInt)Real(Arg(3)); }
    if(Arg(4)) { hour   = (BInt)Real(Arg(4)); }
    if(Arg(5)) { minute = (BInt)Real(Arg(5)); }
    if(Arg(6)) { second = (BInt)Real(Arg(6)); }
    contens_ = BDate(year,(BMonth)month,day,hour,minute,second);
}


//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteFirstNotEqual);
DefExtOpr(1, BDteFirstNotEqual, "FirstNotEqual", 3, 3, "Serie Date Real",
  I2("(Serie ser, Date from, Real value)",
     "(Serie ser, Date desde, Real valor)"),
  I2("Returns the first subsequent date to a given date, for the one which "
     "the value of a series is different of a given value.",
     "Devuelve la primera fecha a partir de una dada para la que el valor de "
     "una serie es distinto de un valor dado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteFirstNotEqual::CalcContens()
//--------------------------------------------------------------------
{
    BUserTimeSerie* ser	= Tsr (Arg(1));
    BUserTimeSet*	  tms	= ser->Dating();
    BDate		  first = Date (Arg(2));
    BDat		  val	= Dat  (Arg(3));
    
    if(!tms) { contens_ = BDate::Unknown(); return; }
    while((*ser)[first]==val) { first+=tms; } ;
    contens_ = first;
}


//--------------------------------------------------------------------
DeclareContensClass(BDte, BDteTemporary, BDteIndexToDate);
DefExtOpr(1, BDteIndexToDate, "IndexToDate", 1, 1, "Real",
  "(Real x)",
  I2("Returns the corresponding date to add x days to date y1900m1d1h0i0s0.",
     "Devuelve la fecha correspondiente a sumar x días a la fecha "
     "y1900m1d1h0i0s0."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteIndexToDate::CalcContens()
//--------------------------------------------------------------------
{
    BReal x = Real(Arg(1));
    contens_ = IndexToDte(x);
}


//--------------------------------------------------------------------
DeclareContensClass(BDte, BDteTemporary, BDteTextToDate);
DefExtOpr(1, BDteTextToDate, "TextToDate", 1, 2, "Text Text",
  "(Text txt [, Text format=\"y%Ym%md%dh%hi%is%s\"])",
  I2("Convers to date a formatted text. You can specify a date format as ",
     "Convierte a fecha un texto formateado. Se puede especificar un formato de fecha como ")+
    "\n  \"y%Ym%md%dh%hi%is%s\""
    "\n  \"%Y-%m-%d\""
    "\n  \"%Y/%m/%d:%h:%i:%s\""+
  I2("\nWARNING: Not all formats are inversible. "
     "For example, format strings with literals (DEC, sunday, ...) "
     "they must be separated with a specified separator character.",
     "\nCUIDADO: No todas las cadenas de formato tienen porqué ser inversibles. "
     "Por ejemplo, si usan literales (DIC, domingo, ...) "
     "éstos deben estar separados del resto por un caracter especificado."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteTextToDate::CalcContens()
//--------------------------------------------------------------------
{
  BText& txt = Text(Arg(1));
  if(!Arg(2))
  {
    contens_ = ConstantDate(txt);
  }
  else
  {
    BText& fmt = Text(Arg(2));
    BDateFormat format(fmt);
    contens_ = format.TextToDate(txt);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BDte, BDteTemporary, BDteInc);
DefExtOpr(1, BDteInc, "+", 2, 2, "Date Real",
  "d + x {Date d [, Real x=1]}",
  I2("Returns the corresponding date to add x days (or fraction) to the "
     "given date.",
     "Devuelve la fecha correspondiente a sumar x días (o fracción) a la "
     "fecha dada."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteInc::CalcContens()
//--------------------------------------------------------------------
{
    BDate d = Date(Arg(1));
    BReal x = 1;
    if(Arg(2)) { x = Real(Arg(2)); }
    contens_ = d+x;
}


//--------------------------------------------------------------------
DeclareContensClass(BDte, BDteTemporary, BDteDec);
DefExtOpr(1, BDteDec, "-", 2, 2, "Date Real",
  "d - x {Date d [, Real x=1]}",
  I2("Returns the corresponding date to rest x days (or fraction) to the "
     "given date.",
     "Devuelve la fecha correspondiente a restar x días (o fracción) a la "
     "fecha dada."),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BDteDec::CalcContens()
//--------------------------------------------------------------------
{
    BDate d = Date(Arg(1));
    BReal x = 1;
    if(Arg(2)) { x = Real(Arg(2)); }
    contens_ = d-x;
}



//--------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteFileTime);
DefExtOpr(1, BDteFileTime, "FileTime", 1, 1, "Text",
  I2("(Text path)","(Text camino)"),
  I2("Returns last modified of given file.",
     "Devuelve la fecha de última modificación del fichero dado."),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BDteFileTime::CalcContens()
//--------------------------------------------------------------------
{
    BDir dir = Text(Arg(1));
    if(dir.Exist())
    {
	contens_ = BTimer::TimeToDate(dir.Time());
    }
}
