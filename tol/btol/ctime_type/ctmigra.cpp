/* ctmigra.cpp: CTime Tol Data Type functionality.
                GNU/TOL Language.
 
   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])
   author: Daniel Rus <danirus@tol-project.org>

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

#include <tol/tol_bctmigra.h>
#include <tol/tol_bctmsgra.h>
#include <tol/tol_bctsrgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdir.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
// static variables
template<>
BGrammar* BGraContensBase<BCTime>::ownGrammar_ = NIL;

DefineContensCommonOperators(BCTime, "CTime");

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BCTime>::InitInstances()
{
    BSystemCTime* unknown_ = new BSystemCTime
	(
	    "UnknownCTime", 
	    BCTime::Unknown(),
	    I2("The unknown time", "El momento desconocido")
        );

    BSystemCTime* b_ = new BSystemCTime
	(
	    "TheBegin", 
	    BCTime::Begin(),
	    I2("The begining of Time", "El comienzo de los tiempos")
	);

    BSystemCTime* e_ = new BSystemCTime
	(
	    "TheEnd", 
	    BCTime::End(),
	    I2("The end of Times", "El final de los tiempos")
	);

    BCTimeToday*  today_ = new BCTimeToday;
    BCTimeNow*    now_   = new BCTimeNow;
    OwnGrammar()->PutDefect(unknown_);
}

//--------------------------------------------------------------------
/*! Returns a valid constant date set for name.
 */
template<>
BSyntaxObject* BGraContensBase<BCTime>::FindConstant (const BText& name)
{
    BUserCTime* uTime = NIL;
    std::string strName(name.String());
    
    if(strName.at(0)=='"') strName.erase(0, 1);
    try {
	BCTime aBCTime;
	strName >> aBCTime;
	if(!aBCTime.isNormal()) 
	    Std(I2("\nGregorian Calendar limits exeeded. "
		   "(y1582m3d1..y4999999m12d31)\n"
		   "CTime operations out of range are not reliable.\n",
		   "\nSobrepasados los límites del Calendario Gregoriano. "
		   "(y1582m3d1..y4999999m12d31)\n"
		   "Las operaciones fuera de rango no son fiables.\n"));
	else uTime = new BUserCTime("", aBCTime);
    } 
    catch(BException& exc) {
	Std("\n");
	Std(I2(exc.enReason().c_str(), exc.esReason().c_str()));
    }
    
    return ((BSyntaxObject*) uTime);
}

//--------------------------------------------------------------------
/*! Returns a valid date for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BCTime>::Casting(BSyntaxObject* obj)
{
    if(!obj) return (NIL);
    if(obj->Grammar()==OwnGrammar()) return (obj);
    return (NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BCTime>::New(FILE* fil)
{
    BUserCTime* ret = 0;
    BCTime tmi;
    BText name;

    tmi.BinRead(fil);
    name.BinRead(fil);

    ret = new BUserCTime("", tmi);
    if(name!="-") ret->PutName(name);

    return ret;
}

//--------------------------------------------------------------------
// algebraic temporary class declarations
//--------------------------------------------------------------------

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeSmallest);
DefIntOpr(1, BCTimeSmallest, "Smallest", 2, 0,
	  "(CTime tmi1, CTime tmi2, [CTime tmi3, ...])",
	  I2("Returns the smallest time in the given list. When two or more "
	     "parameters have the same size returns the first one of all "
	     "of them.\n\nExample:\n"
	     "CTime x = Smallest(y1996m1d1, y1986m10d5h22, y1952, y1973m9d5);"
	     "\n\nResult: x == y1986m10d5h22",
	     "Devuelve el instante de tiempo más pequeño de la lista. "
	     "Cuando dos o más parámetros tienen el mismo tamaño, devuelve "
	     "el primero de ellos.\n\nEjemplo:\n"
	     "CTime x = Smallest(y1996m1d1, y1986m10d5h22, y1952, y1973m9d5);"
	     "\n\nResult: x == y1986m10d5h22"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeSmallest::CalcContens() 
{
    BUserCTime* uTime;
    int n = 2;
    contens_ = CTime(Arg(1));
    
    while(uTime = UCTime(Arg(n++)))
	if(contens_.getGranul() > uTime->Contens().getGranul()) 
	    contens_ = uTime->Contens();    
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeBiggest);
DefIntOpr(1, BCTimeBiggest, "Biggest", 2, 0,
	  "(CTime tmi1, CTime tmi2, [CTime tmi3, ...])",
	  I2("Returns the biggest time in the given list. When two or more "
	     "parameters have the same size returns the first one of all "
	     " of them.\n\nExample:\n"
	     "CTime x = Biggest(y1996m1d1, y1986m10d5h22, y1952, y1973m9d5);"
	     "\n\nResult: x == y1952",
	     "Devuelve el instante de tiempo más grande de la lista. "
	     "Cuando dos o más parámetros tienen el mismo tamaño, devuelve "
	     "el primero de ellos.\n\nEjemplo:\n"
	     "CTime x = Biggest(y1996m1d1, y1986m10d5h22, y1952, y1973m9d5);"
	     "\n\nResult: x == y1952"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeBiggest::CalcContens() 
{
    BUserCTime* uTime;
    int n = 2;
    contens_ = CTime(Arg(1));
    
    while(uTime = UCTime(Arg(n++)))
	if(contens_.getGranul() < uTime->Contens().getGranul()) 
	    contens_ = uTime->Contens();    
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeSooner);
DefIntOpr(1, BCTimeSooner, "Sooner", 2, 0,
	  "(CTime tmi1, CTime tmi2, [CTime tmi3, ...])",
	  I2("Returns the sooner ocurrence timestamp of the given list.\n\n"
	     "Example:\n"
	     "CTime x = Sooner(y1996m1d1, y1986m10, y1952, y1973m9d5);\n\n"
	     "Result: x == y1952",
	     "Devuelve el instante de tiempo de la lista que ocurre antes."
	     "\n\nEjemplo:\n"
	     "CTime x = Sooner(y1996m1d1, y1986m10, y1952, y1973m9d5);\n\n"
	     "Result: x == y1952"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeSooner::CalcContens() 
{
    BUserCTime* uTime;
    int n = 2;
    contens_ = CTime(Arg(1));
    
    while(uTime = UCTime(Arg(n++)))
	if(contens_ > uTime->Contens()) 
	    contens_ = uTime->Contens();
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeLater);
DefIntOpr(1, BCTimeLater, "Later", 2, 0,
	  "(CTime tmi1, CTime tmi2, [CTime tmi3, ...])",
	  I2("Returns the later ocurrence timestamp of the given list.\n\n"
	     "Example:\n"
	     "CTime x = Sooner(y1996m1d1, y1986m10, y1952, y1973m9d5);\n\n"
	     "Result: x == y1952",
	     "Devuelve el instante de tiempo de la lista que ocurre más tarde."
	     "\n\nEjemplo:\n"
	     "CTime x = First(y1996m1d1, y1986m10, y1952, y1973m9d5);\n\n"
	     "Result: x == y1952"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeLater::CalcContens() 
{
    BUserCTime* uTime;
    int n = 2;
    contens_ = CTime(Arg(1));
    
    while(uTime = UCTime(Arg(n++)))
	if(contens_ < uTime->Contens()) 
	    contens_ = uTime->Contens();
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeNext);
DefExtOpr(1, BCTimeNext, "NextCTime", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the successor of a given time: CTime tmi.\n\n"
	     "Example:\n"
	     "CTime x = NextCTime(y2004m10);\n\n"
	     "Result:\nx == y2004m11",
	     "Devuelve el instante de tiempo sucesor del instante de "
	     "tiempo dado.\n\nEjemplo:\n"
	     "CTime x = Succ(y2004m10);\n\n"
	     "Resultado:\nx == y2004m11"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeNext::CalcContens() 
{
    BCTime aTmi = CTime(Arg(1));
    contens_ = aTmi;
    contens_.successor(1);
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimePrev);
DefExtOpr(1, BCTimePrev, "PrevCTime", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the predecessor of a given time: CTime tmi.\n\n"
	     "Example:\n"
	     "CTime x = PrevCTime(y2004m10);\n\n"
	     "Result:\nx == y2004m09",
	     "Devuelve el instante de tiempo sucesor del instante de "
	     "tiempo dado.\n\nEjemplo:\n"
	     "CTime x = PrevCTime(y2004m10);\n\n"
	     "Resultado:\nx == y2004m09"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimePrev::CalcContens() 
{
    BCTime aTmi = CTime(Arg(1));
    contens_ = aTmi;
    contens_.successor(-1);
}

//--------------------------------------------------------------------
//DeclareCDateContentClass(BCDateTemporary, BCDateBeginOf);
//DeclareCDateContentClass(BCDateTemporary, BCDateEndOf);
//DeclareCDateContentClass(BCDateTemporary, BCDateFirstNotEqual);

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeAdd);
DefExtOpr(1, BCTimeAdd, "Add", 1, 7, "CTime Real Real Real Real Real Real",
	  I2("(CTime tmi, Real year, [Real month, Real day, Real hour, "
	     "Real minute, Real second])", "(CTime tmi, Real año, "
	     "[Real mes, Real dia, Real hora, Real minuto, Real segundo])"),
	  I2("Returns a Time as a result of adding the given Real "
	     "values to parameter Time tmi. Only the parameters related "
	     "with tmi's granularity will be applied:\n"
	     "Real year -> to a Yearly based tmi\n"
	     "Real year, Real month -> to a Monthly based tmi\n"
	     "Real year, Real month, Real day -> to a Daily based tmi\n"
	     "Real year, Real month, Real day, Real Hour -> "
	     "to a Hourly based tmi\n"
	     "Real year, Real month, Real day, Real Hour, Real minute -> "
	     "to a Minutely based tmi\n"
	     "Real year, Real month, Real day, Real Hour, Real minute, "
	     "Real second -> to a Secondly based tmi\n",
	     "Devuelve un Time como resultado de añadir los parámetros Real "
	     "dados al parámetro Time tmi. Solo los parámetros relacionados "
	     "con la granularidad del parámetro tmi serán aplicados:\n"
	     "Real año -> si tmi tiene base Anual\n"
	     "Real año, Real mes -> si tmi tiene base Mensual\n"
	     "Real año, Real mes, Real dia -> si tmi tiene base Diaria\n"
	     "Real año, Real mes, Real dia, Real Hora -> "
	     "si tmi tiene base Horaria\n"
	     "Real año, Real mes, Real dia, Real Hora, Real minuto -> "
	     "si tmi tiene base Minutal\n"
	     "Real año, Real mes, Real dia, Real Hora, Real minuto, "
	     "Real segundo -> si tmi tiene base Secundal\n"),
	  BOperClassify::TimeAlgebra_);
//--------------------------------------------------------------------
void BCTimeAdd::CalcContens() 
{
    long int year;
    int month, day, hour, minute, second;
    BCTime tmi = CTime(Arg(1));

    year   = static_cast<long int>(Real(Arg(2)));
    month  = Arg((3)) ? static_cast<int>(Real(Arg(3))) : 0;
    day    = Arg((4)) ? static_cast<int>(Real(Arg(4))) : 0;
    hour   = Arg((5)) ? static_cast<int>(Real(Arg(5))) : 0;
    minute = Arg((6)) ? static_cast<int>(Real(Arg(6))) : 0;
    second = Arg((7)) ? static_cast<int>(Real(Arg(7))) : 0;

    tmi.add(year, month, day, hour, minute, second);
    contens_ = tmi;
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeLastUpdate);
DefExtOpr(1, BCTimeLastUpdate, "LastUpdate", 1, 1, "Text",
	  I2("(Text path)","(Text camino)"),
	  I2("Returns last modified date of given file.",
	     "Devuelve el momento de última modificación del fichero dado."),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeLastUpdate::CalcContens() 
//--------------------------------------------------------------------
{
    BDir dir = Text(Arg(1));
    if(dir.Exist()) contens_ = BTimer::lastUpdate(dir.Time());
    else contens_ = BCTime::Unknown();
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeFromText);
DefExtOpr(1, BCTimeFromText, "CTimeFromText", 2, 2, "Text Text",
	  "(Text tmi, Text format)",
	  I2("The CTimeFromText function is the converse function to "
	     "TextFromCTime and converts the character string given as first "
	     "argument, formated with the string given as 2nd. parameter, "
	     "into a CTime value. Here format is a character string that "
	     "consists of field descriptors and text characters. Each field "
	     "descriptor consists of a % character followed by another "
	     "character that specifies the replacement for the field "
	     "descriptor. All other characters in the format string must "
	     "have a matching character in the input string, except for "
	     "whitespace, which matches zero or more whitespace characters "
	     "in the input string. There should be whitespace or other "
	     "alphanumeric characters between any two field descriptors.\n\n"
	     "The  function processes the input string from left to right. "
	     "Each of the three possible input elements (whitespace, "
	     "literal, or format) are handled one after the other. If the "
	     "input cannot be matched to the format string the function "
	     "stops. The remainder of the format and input strings are not "
	     "processed.\n\n"
	     "The supported input field descriptors are listed below. In "
	     "case a text string (such as a weekday or month name) is to be "
	     "matched, the comparison is case insensitive. In case a number "
	     "is to be matched, leading zeros are permitted but not "
	     "required.\n\n"
	     "%%     The % character.\n\n"
	     "%a or %A\n"
	     "       The weekday name according to the current locale, in "
	     "abbreviated form or the  full name.\n\n"
	     "%b or %B or %h\n"
	     "       The month name according to the current locale, in "
	     "abbreviated form or the full name.\n\n"
	     "%c     The date and time representation for the current "
	     "locale.\n\n"
	     "%C     The century number (0-99).\n\n"
	     "%d or %e\n"
	     "       The day of month (1-31).\n\n"
	     "%D     Equivalent to %m/%d/%y. (This is the American style "
	     "date, very confusing to non Americans, especially since "
	     "%d/%m/%y is widely used in Europe. The ISO 8601 standard "
	     "format is %Y-%m-%d.)\n\n"
	     "%H     The hour (0-23).\n\n"
	     "%I     The hour on a 12-hour clock (1-12).\n\n"
	     "%j     The day number in the year (1-366).\n\n"
	     "%m     The month number (1-12).\n\n"
	     "%M     The minute (0-59).\n\n"
	     "%n     Arbitrary whitespace.\n\n"
	     "%p     The locale's equivalent of AM or PM. (Note: there may "
	     "be none.)\n\n"
	     "%r     The 12-hour clock time (using the locale's AM or PM). "
	     "In the POSIX locale equivalent to %I:%M:%S %p. If t_fmt_ampm "
	     "is empty in the LC_TIME part of the current locale then the "
	     "behaviour is undefined.\n\n"
	     "%R     Equivalent to %H:%M.\n\n"
	     "%S     The second (0-60; 60 may occur for leap seconds; "
	     "earlier also 61 was allowed).\n\n"
	     "%t     Arbitrary whitespace.\n\n"
	     "%T     Equivalent to %H:%M:%S.\n\n"
	     "%U     The week number with Sunday the first day of the week "
	     "(0-53). The first Sunday of January is the first day of "
	     "week 1.\n\n"
	     "%w     The weekday number (0-6) with Sunday = 0.\n\n"
	     "%W     The week number with Monday the first day of the week "
	     "(0-53).  The first Monday of January is the first day of "
	     "week 1.\n\n"
	     "%x     The date, using the locale's date format.\n\n"
	     "%X     The time, using the locale's time format.\n\n"
	     "%y     The year within century (0-99). When a century is not "
	     "otherwise specified, values in the range 69-99 refer to years "
	     "in the twentieth century (1969-1999); values in the range "
	     "00-68 refer to years in the twenty-first century "
	     "(2000-2068).\n\n"
	     "%Y     The year, including century (for example, 1991).\n\n"
	     "Some field descriptors can be modified by the E or O modifier "
	     "characters to indicate that an alternative format or "
	     "specification should be used. If the alternative format or "
	     "specification does not exist in the current locale, the "
	     "unmodified field descriptor is used.\n\n"
	     "The E modifier specifies that the input string  may contain "
	     "alternative locale-dependent versions of the date and time "
	     "representation:\n\n"
	     "%Ec    The locale's alternative date and time "
	     "representation.\n\n"
	     "%EC    The name of the base year (period) in the locale's "
	     "alternative representation.\n\n"
	     "%Ex    The locale's alternative date representation.\n\n"
	     "%EX    The locale's alternative time representation.\n\n"
	     "%Ey    The offset from %EC (year only) in the locale's "
	     "alternative representation.\n\n"
	     "%EY    The full alternative year representation.\n\n"
	     "The O modifier specifies that the numerical input may be in "
	     "an alternative locale-dependent format:\n\n"
	     "%Od or %Oe\n"
	     "       The day of the month using the locale's alternative "
	     "numeric symbols; leading zeros are permitted but not "
	     "required.\n\n"
	     "%OH    The hour (24-hour clock) using the locale's alternative "
	     "numeric symbols.\n\n"
	     "%OI    The hour (12-hour clock) using the locale's "
	     "alternative numeric symbols.\n\n"
	     "%Om    The month using the locale's alternative numeric "
	     "symbols.\n\n"
	     "%OM    The minutes using the locale's alternative numeric "
	     "symbols.\n\n"
	     "%OS    The seconds using the locale's alternative numeric "
	     "symbols.\n\n"
	     "%OU    The week number of the year (Sunday as the first day "
	     "of the week) using the locale's alternative numeric symbols.\n\n"
	     "%Ow    The number of the weekday (Sunday=0) using the locale's "
	     "alternative numeric symbols.\n\n"
	     "%OW    The week number of the year (Monday as the first day "
	     "of the week) using the locale's alternative numeric symbols.\n\n"
	     "%Oy    The year (offset from %C) using the locale's alternative "
	     "numeric symbols."
	     ,
	     "La función CTimeFromText es la función inversa de TextFromCTime "
	     "y convierte la cadena de caracteres del parámetro Text tmi en "
	     "un valor CTime, utilizando el formato especificado por el "
	     "parámetro Text format, que es una cadena de caracteres "
	     "consistente en descriptores de campos y caracteres de texto.\n"
	     "Cada  descriptor  de  campo  consiste  en  un  carácter  de "
	     "porcentaje % seguido por otro carácter que especifica el "
	     "reemplazo para el descriptor de campo. Todos los otros "
	     "caracteres de la cadena format deben tener un carácter "
	     "concordante en la cadena de entrada, salvo los espacios en "
	     "blanco que pueden concordar con cero o más espacios en blanco "
	     "de la cadena de entrada. Entre dos descriptores de campo "
	     "cualesquiera debe haber siempre un espacio en blanco o bien "
	     "otros caracteres alfanuméricos.\n\n"
	     "La función procesa la cadena de entrada de izquierda a derecha. "
	     "Cada uno de los tres posibles elementos de entrada (espacio en "
	     "blanco, literal o formato) se tratan uno detrás de otro. Si no "
	     "se puede hacer coincidir la entrada con la cadena de formato, "
	     "la función se detiene. El resto de las cadenas de formato y de "
	     "entrada no se procesa.\n\n"
	     "Los descriptores de campo de entrada se listan a continuación. "
	     "En caso de que la concordancia sea con cadenas de texto (como "
	     "el nombre de un día de la semana o de  un  mes), la "
	     "comparación no tiene en cuenta las mayúsculas. En caso de que "
	     "la concordancia sea con números, se permite poner ceros al "
	     "principio aunque no es obligatorio.\n\n"
	     "%%     El carácter %.\n\n"
	     "%a o %A\n"
	     "       El nombre del día de la semana según la localización "
	     "actual, en forma abreviada o completa.\n\n"
	     "%b o %B o %h\n"
             "       El nombre del mes según la localización actual, en "
	     "forma abreviada o completa.\n\n"
	     "%c     La representación de fecha y hora para la localización "
	     "actual.\n\n"
	     "%C     El número de siglo (0-99).\n\n"
	     "%d o %e\n"
             "       El día del mes (1-31).\n\n"
	     "%D     Equivalente  a  %m/%d/%y.  (Este es el estilo americano "
	     "para las fechas, muy confuso para los no americanos, "
	     "especialmente debido a que el formato %d/%m/%y es el que se "
	     "usa ampliamente en Europa. El formato del estándar ISO 8601 "
	     "es %Y-%m-%d.)\n\n"
	     "%H     La hora (0-23).\n\n"
	     "%I     La hora en formato de 12 horas (1-12).\n\n"
	     "%j     El número de día del año (1-366).\n\n"
	     "%m     El número de mes (1-12).\n\n"
	     "%M     El minuto (0-59).\n\n"
	     "%n     Cadena arbitraria de espacios en blanco.\n\n"
	     "%p     El equivalente de AM o PM en la localización. (Nota: "
	     "puede no existir.)\n\n"
	     "%r     La  hora en formato de 12 horas (usando el equivalente "
	     "de AM o PM en la localización). En la localización POSIX "
	     "equivale a %I:%M:%S %p. Si el campo t_fmt_ampm está vacío en "
	     "la categoría LC_TIME de la localización actual el "
	     "comportamiento es indefinido.\n\n"
	     "%R     Equivalente a %H:%M.\n\n"
	     "%S     El segundo (0-60; 60 puede darse para segundos de salto "
	     "(leap seconds); anteriormente también se permitía 61).\n\n"
	     "%t     Cadena arbitraria de espacios en blanco.\n\n"
	     "%T     Equivalente a %H:%M:%S\n\n"
	     "%U     El número de semana tomando el domingo como primer día "
	     "de la semana (0-53). El primer domingo de enero es el primer "
	     "día de la semana 1.\n\n"
	     "%w     El número de día de la semana (0-6) con domingo = 0.\n\n"
	     "%W     El número de semana tomando el lunes como primer día de "
	     "la semana (0-53). El primer lunes de enero es el primer día "
	     "de la semana 1.\n\n"
	     "%x     La fecha, usando el formato de fecha de la "
	     "localización.\n\n"
	     "%X     La hora, usando el formato de hora de la "
	     "localización.\n\n"
	     "%y     El año dentro del siglo (0-99). Cuando no se especifica "
	     "el siglo, los valores comprendidos en el rango 69-99 se "
	     "refieren al siglo 20 (1969-1999) y los valores comprendidos "
	     "en el rango 00-68 se refieren al siglo 21 (2000-2068).\n\n"
	     "%Y     El año, incluyendo el siglo (por ejemplo, 1991).\n\n"
	     "Algunos descriptores de campo pueden ser modificados por los "
	     "caracteres modificadores E u O para indicar el uso de un "
	     "formato o especificación alternativo. Si el formato o "
	     "especificación alternativo no existe en la localización "
	     "actual, se utiliza el descriptor de campo sin modificar.\n\n"
	     "El modificador E especifica que la cadena de entrada puede "
	     "contener versiones  alternativas de la representación de "
	     "fecha y hora dependientes de la localización:\n\n"
	     "%Ec    La representación alternativa de fecha y hora de la "
	     "localización.\n\n"
	     "%EC    El  nombre del año base (período) en la representación "
	     "alternativa de la localización.\n\n"
	     "%Ex    La representación alternativa de fecha de la "
	     "localización.\n\n"
	     "%EX    La representación alternativa de hora de la "
	     "localización.\n\n"
	     "%Ey    El desplazamiento desde %EC (sólo año) en la "
	     "representación alternativa de la localización.\n\n"
	     "%EY    La representación alternativa completa para el año.\n\n"
	     "El  modificador O especifica que la entrada numérica puede "
	     "estar en un formato alternativo dependiente de la "
	     "localización:\n\n"
	     "%Od o %Oe\n"
             "       El día del mes usando los símbolos numéricos "
	     "alternativos de la localización; los ceros del comienzo están "
	     "permitidos pero no son obligatorios.\n\n"
	     "%OH    La hora (foramto 24 horas) usando los símbolos numéricos "
	     "alternativos de la localización.\n\n"
	     "%OI    La hora (formato 12 horas) usando los símbolos numéricos "
	     "alternativos de la localización.\n\n"
	     "%Om    El mes usando los símbolos numéricos alternativos de la "
	     "localización.\n\n"
	     "%OM    Los minutos usando los símbolos numéricos alternativos "
	     "de la localización.\n\n"
	     "%OS    Los segundos usando los símbolos numéricos alternativos "
	     "de la localización.\n\n"
	     "%OU    El  número de semana del año (tomando el domingo como "
	     "primer día de la semana) usando los símbolos numéricos "
	     "alternativos de la localización.\n\n"
	     "%Ow    El número del día de la semana (domingo=0) usando los "
	     "símbolos numéricos  alternativos de la localización.\n\n"
	     "%OW    El número de semana del año (tomando el lunes como "
	     "primerdía de la semana) usando los símbolos numéricos "
	     "alternativos de la localización.\n\n"
	     "%Oy    El año (desplazamiento desde %C) usando los símbolos "
	     "numéricos alternativos de la localización."
	      ),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeFromText::CalcContens() 
//--------------------------------------------------------------------
{
    BText tmi    = Text(Arg(1));
    BText format = Text(Arg(2));
    contens_ = BCTime::fillNew(tmi.String(), format.String());
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeContainerOf);
DefExtOpr(1, BCTimeContainerOf, "ContainerOf", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns the CTime that contains the given CTime.",
	     "Devuelve el CTime que contiene al CTime dado."),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeContainerOf::CalcContens() 
//--------------------------------------------------------------------
{
    contens_ = CTime(Arg(1));
    contens_.incGranul();
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeSucc);
DefExtOpr(1, BCTimeSucc, "Succ", 2, 3, "CTime CTimeSet Real",
	  "(CTime tmi, CTimeSet dating, [Real n=1])",
	  I2("Returns the umpteenth succeeding instant of Time of the given"
	     "tmi in the given dating. With a negative value for n, returns "
	     "the the umpteenth predecessor.\n\n"
	     "Example:\n"
	     "CTime x = Succ(y1996m1d1, Day(1), 5);\n\n"
	     "Result:\n"
	     "x == y1996m6d1",
	     "Devuelve el enésimo instante de tiempo posterior al dado como "
	     "parámetro, en el conjunto temporal dating. Si n fuera negativo "
	     "devolverá el enésimo predecesor.\n\n"
	     "Ejemplo:\n"
	     "CTime x = Succ(y1996m1d1, Day(1), 5);\n\n"
	     "Resultado:\n"
	     "x == y1996m6d1"),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeSucc::CalcContens() 
//--------------------------------------------------------------------
{
    BCTime         tmi  = CTime(Arg(1));
    BUserCTimeSet *ctms = CTms(Arg(2));
    int            dir  = 1;
    int            nth  = 1;

    if(Arg(3)) {
	nth = static_cast<int>(Real(Arg(3)));
	if(nth) dir = nth/abs(nth);
    }
    
    contens_ = tmi;

    if(dir>0)
	for(int i=0; i<abs(nth); i++)
	{
	    contens_ = ctms->succ(contens_);
	    if(contens_.getGranul() < tmi.getGranul())
	    { contens_ = ctms->maxGranulAdjust(contens_, tmi.getGranul()); }
	}
    else
	for(int i=0; i<abs(nth); i++)
	{
	    contens_ = ctms->pred(contens_);
	    if(contens_.getGranul() < tmi.getGranul())
	    { contens_ = ctms->maxGranulAdjust(contens_, tmi.getGranul()); }
	}
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimePred);
DefExtOpr(1, BCTimePred, "Pred", 2, 3, "CTime CTimeSet Real",
	  "(CTime tmi, CTimeSet dating, [Real n=1])",
	  I2("Returns the umpteenth preceeding instant of Time of the given"
	     "tmi in the given dating. With a negative value for n, returns "
	     "the the umpteenth successor.\n\n"
	     "Example:\n"
	     "CTime x = Pred(y1996m1d6, Day(1), 5);\n\n"
	     "Result:\n"
	     "x == y1996m6d1",
	     "Devuelve el enésimo instante de tiempo anterior al dado como "
	     "parámetro, en el conjunto temporal dating. Si n fuera negativo "
	     "devolverá el enésimo sucesor.\n\n"
	     "Ejemplo:\n"
	     "CTime x = Pred(y1996m1d6, Day(1), 5);\n\n"
	     "Resultado:\n"
	     "x == y1996m6d1"),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimePred::CalcContens() 
//--------------------------------------------------------------------
{
    BCTime         tmi  = CTime(Arg(1));
    BUserCTimeSet *ctms = CTms(Arg(2));
    int            dir  = 1;
    int            nth  = 1;

    if(Arg(3)) {
	nth = static_cast<int>(Real(Arg(3)));
	if(nth) dir = nth/abs(nth);
    }
    
    contens_ = tmi;

    if(dir>0)
	for(int i=0; i<abs(nth); i++)
	{
	    contens_ = ctms->pred(contens_);
	    if(contens_.getGranul() < tmi.getGranul())
	    { contens_ = ctms->maxGranulAdjust(contens_, tmi.getGranul()); }
	}
    else
	for(int i=0; i<abs(nth); i++)
	{
	    contens_ = ctms->succ(contens_);
	    if(contens_.getGranul() < tmi.getGranul())
	    { contens_ = ctms->maxGranulAdjust(contens_, tmi.getGranul()); }
	}
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeMaximal);
DefExtOpr(1, BCTimeMaximal, "Maximal", 2, 2, "CTime CTimeSet",
	  "(CTime tmi, CTimeSet dating)",
	  I2("Returns the maximal instant of Time of the given tmi in the "
	     "given dating.\n\n"
	     "Example:\n"
	     "CTime x = Maximal(y1996m1d3, cMonth(1));\n\n"
	     "Result:\n"
	     "x == y1996m1",
	     "Devuelve el instante de tiempo maximal del CTime dado como "
	     "parámetro, en el conjunto temporal dating.\n\n"
	     "Ejemplo:\n"
	     "CTime x = Maximal(y1996m1d3, cMonth(1));\n\n"
	     "Resultado:\n"
	     "x == y1996m1"),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeMaximal::CalcContens() 
//--------------------------------------------------------------------
{
    BCTime tmi = CTime(Arg(1));
    BUserCTimeSet *ctms = CTms(Arg(2));
    if(!ctms->includes(tmi)) 
	contens_ = BCTime::Unknown();
    else contens_ = ctms->getMaximal(tmi);
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeMaxSucc);
DefExtOpr(1, BCTimeMaxSucc, "MaximalSucc", 2, 3, "CTime CTimeSet Real",
	  "(CTime tmi, CTimeSet dating, [Real n=1])",
	  I2("Returns the umpteenth maximal succeeding instant of Time of "
	     "the given tmi in the given dating. With a negative value for "
	     "n, returns the the umpteenth maximal predecessor.\n\n"
	     "Example:\n"
	     "CTime x = MaximalSucc(y1996m1d1, CTSAllMonths, 2);\n\n"
	     "Result:\n"
	     "x == y1996m3",
	     "Devuelve el enésimo instante de tiempo maximal posterior al "
	     "dado como parámetro, en el conjunto temporal dating. Si n "
	     "es negativo devolverá el enésimo predecesor maximal.\n\n"
	     "Ejemplo:\n"
	     "CTime x = MaximalSucc(y1996m1d1, CTSAllMonths, 2);\n\n"
	     "Resultado:\n"
	     "x == y1996m3"),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeMaxSucc::CalcContens() 
//--------------------------------------------------------------------
{
    BCTime         tmi  = CTime(Arg(1));
    BUserCTimeSet *ctms = CTms(Arg(2));
    int            dir  = 1;
    int            nth  = 1;

    if(Arg(3)) {
	nth = static_cast<int>(Real(Arg(3)));
	if(nth) dir = nth/abs(nth);
    }
    
    contens_ = ctms->getMaximal(tmi);

    if(dir>0) 
	for(int i=0; i<abs(nth); i++) contens_ = ctms->maximalSucc(contens_);
    else for(int i=0; i<abs(nth); i++) contens_ = ctms->maximalPred(contens_);
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeMaxPred);
DefExtOpr(1, BCTimeMaxPred, "MaximalPred", 2, 3, "CTime CTimeSet Real",
	  "(CTime tmi, CTimeSet dating, [Real n=1])",
	  I2("Returns the umpteenth maximal preceeding instant of Time of "
	     "the given tmi in the given dating. With a negative value for "
	     "n, returns the the umpteenth maximal successor.\n\n"
	     "Example:\n"
	     "CTime x = MaximalPred(y1996m1d1, CTSAllMonths, 2);\n\n"
	     "Result:\n"
	     "x == y1996m1",
	     "Devuelve el enésimo instante de tiempo maximal anterior al "
	     "dado como parámetro, en el conjunto temporal dating. Si n "
	     "es negativo devolverá el enésimo sucesor maximal.\n\n"
	     "Ejemplo:\n"
	     "CTime x = MaximalPred(y1996m1d1, CTSAllMonths, 2);\n\n"
	     "Resultado:\n"
	     "x == y1996m1"),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BCTimeMaxPred::CalcContens() 
//--------------------------------------------------------------------
{
    BCTime         tmi  = CTime(Arg(1));
    BUserCTimeSet *ctms = CTms(Arg(2));
    int            dir  = 1;
    int            nth  = 1;

    if(Arg(3)) {
	nth = static_cast<int>(Real(Arg(3)));
	if(nth) dir = nth/abs(nth);
    }
    
    contens_ = tmi;

    if(dir>0) 
	for(int i=0; i<abs(nth); i++) contens_ = ctms->maximalPred(contens_);
    else for(int i=0; i<abs(nth); i++) contens_ = ctms->maximalSucc(contens_);
}

//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeFirst);
DefExtOpr(1, BCTimeFirst, "First", 1, 1, "CSeries",
	  "(CSeries cser)",
	  I2("Returns the first CTime of a series.",
	     "Devuelve el instante CTime inicial de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BCTimeFirst::CalcContens()
{
    BUserCTimeSeries* ctsr = CTsr(Arg(1));
    contens_ = ctsr->FirstCTime();
}


//--------------------------------------------------------------------
DeclareCTimeContentClass(BCTimeTemporary, BCTimeLast);
DefExtOpr(1, BCTimeLast, "Last", 1, 1, "CSeries",
	  "(CSeries cser)",
	  I2("Returns the last CTime of a series.",
	     "Devuelve el instante CTime final de una serie."),
	  BOperClassify::TimeSeriesAlgebra_);
//--------------------------------------------------------------------
void BCTimeLast::CalcContens()
{
    BUserCTimeSeries* ctsr = CTsr(Arg(1));
    contens_ = ctsr->LastCTime();
}

#endif /* __USE_TC__ */
