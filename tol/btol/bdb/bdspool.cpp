/* bdspool.cpp: Bayes Data Base functions. TOL Language.

   Copyright (C) 2003-2007, Bayes Decision, SL (Spain [EU])

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
#  include <win_tolinc.h>
#endif

#include <tol/tol_bdspool.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bparser.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bstruct.h>

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("bdspool.cpp");
BBool ForzeLinkerBDSpool() { return(BTRUE); }

static const char* TypeDataBaseName[] =
{ "Unknown", "Text", "Date", "Real" };

//--------------------------------------------------------------------
BBool DBOpenQuery(const BText& query, const BText& function)
//--------------------------------------------------------------------
{
  if(!dbOpenQuery(query.String()))
  {
    Error(function + 
	  I2("Cannot open data base query :\n",
	     "No se puede abrir la consulta a la base de datos :\n") +
	  query);
    return BFALSE;
  }

  if(!dbGetFirst()) // Se sitúa el cursor en el primer registro
  {
    dbCloseQuery();
    return BFALSE;
  }
  return BTRUE;
}

//--------------------------------------------------------------------
BInt DBTestTypes(const BText& types,
		 const BText& function,
		       BInt   numFields=-1)
//--------------------------------------------------------------------
{
  BInt ok = BTRUE;
  BArray<BText> tok;
  BInt fields = dbGetFieldsNumber();

  ReadAllTokens(types,tok,' ');

  if((numFields>=0) && (fields!=numFields))
  {
    Error(function+I2("Query returns a wrong number of fields, ",
	       "La consulta devuelve un número equivocado de campos, ") +
	  fields +
	  I2(" instead of "," en lugar de ") +
	  numFields);
    dbCloseQuery();
    return(0);
  }

  if((numFields<0) && (fields<tok.Size()))
  {
    Error(function+I2("Query returns an insuficient number of fields, ",
	       "La consulta devuelve un número insuficiente de campos, ") +
	  fields +
	  I2(" instead of "," en lugar de ") +
	  tok.Size());
    dbCloseQuery();
    return(0);
  }

  for(BInt j=0; j<fields; j++)
  {
    BInt k = Minimum(j,tok.Size()-1);
    BInt tj = dbGetFieldType(j);
    if(tok[k] != BText(TypeDataBaseName[tj]))
    {
      Warning(function+I2("Query must return a ",
		   "La consulta debería devolver un ") + tok[k] +
		I2(" at the field number ",
		   " en el campo ") + (j+1));
      ok = BFALSE;
    }
  }
  if(!ok) { dbCloseQuery(); }

  return(ok?fields:0);
}


//--------------------------------------------------------------------
BDate DBGetDate(BInt field)
//--------------------------------------------------------------------
{
  BDate date;
  unsigned short year, month, day, hour, minute, second, msecond;

  if(dbGetAsDate(field, year, month, day, hour, minute, second, msecond))
  {
    date = BDate(year, month, day, hour, minute, (second+(msecond/1000)));
  }
  return(date);
};

//--------------------------------------------------------------------
BDat DBGetReal(BInt field, const BDat& defaultValue=BDat::Unknown())
//--------------------------------------------------------------------
{
    BDat dat = defaultValue;
    long double x;

    if(dbGetAsReal(field,x)==1) // dbGetAsReal may return:
      {                         //   0 -> Error
	dat = x;                //   1 -> Ok, x bring back a real value
      }                         //   2 -> Ok, a NULL value was red
    return(dat);
};

//--------------------------------------------------------------------
BText DBGetText(BInt field)
//--------------------------------------------------------------------
{
  BText txt("");
  unsigned char *str = NULL;

  int rc = dbGetAsText(field, &str);

  if(rc && str && (strlen((char*) str)!=0)) {
    txt = BText((char*) str);
    dbFreeText(&str);
  }
  
  return txt;
};

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatOpenDataBase);
DefExtOpr(1, BDatOpenDataBase, "DBOpen", 3, 4, "Text Text Text Set",
  I2("(Text aliasName, Text userName, Text password [, Set DBStruct])",
     "(Text alias, Text usuario, Text clave [, Set estructuraBD])"),
  I2("PURPOSE: Opens a database and activates it.\n\n"
     "RETURNS: False on error, True when succesful.\n\n"
     "If the last argument is not specified, then the ODBC driver is used by default.\n"
     "\nDBStruct especification:\n\n"
     "             Struct DBStruct\n"
     "             {\n"
     "               Text driver,\n"
     "               Text database,\n"
     "               Text host\n"
     "             };\n\n"
     "Where:\n\n"
     "  - driver: One of \"odbc\" or \"mysql\".\n"
     "  - database: The name of the database.\n"
     "  - host: The database server.\n"
     "\nDriver-specific notes:\n"
     "    * odbc:\n"
     "      - Use a driver only struct to keep the warning message from bugging you.\n",
     "PROPOSITO: Abre y activa una base de datos.\n\n"
     "DEVUELVE: Falso en caso de error, Verdadero en caso de éxito.\n\n"
     "Si no se especifica el último argumento se usa la interfaz ODBC por defecto.\n"
     "\nEspecificación de estructuraBD:\n\n"
     "             Struct DBStruct\n"
     "             {\n"
     "               Text driver,\n"
     "               Text baseDeDatos,\n"
     "               Text servidor\n"
     "             };\n\n"
     "Donde:\n\n"
     "  - driver: Uno de los siguientes: \"odbc\" o \"mysql\".\n"
     "  - baseDeDatos: El nombre de la base de datos.\n"
     "  - servidor: El servidor de la base de datos que se quiere utilizar.\n"
     "\nNotas específicas de los drivers:\n"
     "    * odbc:\n"
     "      - Utilizando una estructura estructuraBD que contenga únicamente"
     " el driver \"odbc\" se evita el molesto warning.\n"),
    BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatOpenDataBase::CalcContens()
//--------------------------------------------------------------------
{
  BSet dbStruct;
  
  if (Arg(4))
  {
    dbStruct = Set(Arg(4));
    if(!(dbStruct[1] && dbStruct[1]->Grammar()==GraText()))
    {
      Error(I2("Wrong 1st. parameter when trying to open DB Alias ",
	       "1er. parámetro incorrecto al intentar abrir el Alias ")
	    + Text(Arg(1)));
      contens_ = 0;
    } 
    else 
      contens_ = dbOpenDirect(Text(Arg(1)).String(),
			      Text(Arg(2)).String(),
			      Text(Arg(3)).String(),
			      &dbStruct);
  }
  else
    contens_ = dbOpenODBC(Text(Arg(1)).String(),
			  Text(Arg(2)).String(),
			  Text(Arg(3)).String());
}

//////////////////////////////////////////////////////////////////////////////
/*
  DeclareContensClass(BDat, BDatTemporary, BDatSelectDataBase);
  DefExtOpr(1, BDatSelectDataBase, "DBActivate", 1, 1, "Text",
  I2("(Text aliasName)",
     "(Text alias)"),
  I2("Selects a database on the list to be activated. Returns False on error.",
     "Selecciona una base de datos de la lista para su activación."
     "Devuelve Falso en caso de error."),
  BOperClassify::BayesDataBase_);
  void BDatSelectDataBase::CalcContens()
*/


  DeclareContensClass(BDat, BDatTemporary, BDatDBActivate);
  DefExtOpr(1, BDatDBActivate, "DBActivate", 1, 1, "Text",
  I2("(Text aliasName)",
     "(Text alias)"),
  I2("Selects a database on the list to be activated. Returns False on error.",
     "Selecciona una base de datos de la lista para su activación."
     "Devuelve Falso en caso de error."),
  BOperClassify::BayesDataBase_);
  void BDatDBActivate::CalcContens()
//////////////////////////////////////////////////////////////////////////////
{
  contens_ = dbActivate(Text(Arg(1)).String());
}

//--------------------------------------------------------------------

DeclareContensClass(BSet, BSetTemporary, BSetDBGetOpened);
DefExtOpr(1, BSetDBGetOpened, "DBGetOpened", 1, 1,
  "Text",
  I2("(Text dbType)",
     "(Text tipoBD)"),
  I2("Returns a set of all opened databases of the \"dbtype\" type.\n"
     "If an empty string is given, then all databases are returned. "
     "Along with each database, it's alias, type and status are returned.\n"
     "All possible values for 'dbType' are: \"ODBC\", \"MySQL\" y "
     "\"PostgreSQL\".\n",
     "Devuelve un conjunto de las bases de datos abiertas cuyo tipo "
     "coincide con el proporcionado(\"tipoBD\").\n"
     "Si se proporciona una cadena de texto vacía, entonces todas "
     "las bases de datos abiertas son incluídas.\n"
     "Para cada base de datos se devuelve su alias, tipo y estado.\n"
     "Los posibles valores para 'tipoDB' son: \"ODBC\", \"MySQL\", "
     "\"PostgreSQL\"\n"),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetDBGetOpened::CalcContens()
//--------------------------------------------------------------------
{
  BText B = "(Funcion DBGetOpened)";
  const char *type = Text(Arg(1)).String();
  BList* lst = NIL;
  
  lst = dbGetOpened(type);
  
  contens_.RobElement(lst);
  contens_.PutSubType(BSet::Table);
}

//--------------------------------------------------------------------

DeclareContensClass(BDat, BDatTemporary, BDatCloseDataBase);
DefExtOpr(1, BDatCloseDataBase, "DBClose", 1, 1, "Text", "(Text alias)",
	  I2("Releases a database connection.\n"
	     "-The given 'alias' tells which connection must be closed.\n"
	     "-If the given 'alias' is an empty string, the current active "
	     "connection will be closed.\n"
	     "-If the connection to be closed is the active connection, "
	     "the first connection of DBConns will be activated.\n\n"
	     "Returns:\n"
	     "1 whether the connection was successfully closed.\n"
	     "0 whether the 'alias' does not exist, or 'DBConns' is empty.\n"
	     ,
	     "Cierra la conexión con una base de datos.\n"
	     "-El parámetro 'alias' indica que conexión debe ser cerrada.\n"
	     "-Si el parámetro 'alias' es la cadena vacía, cierra la "
	     "conexión con la base de datos activa.\n"
	     "-Si la conexión que se cierra es la conexión activa, se "
	     "activará la primera conexión de 'DBConns'.\n\n"
	     "Devuelve:\n "
	     "1 si se ha cerrado la conexión.\n"
	     "0 si no existe 'alias', o 'DBConns' esta vacío.\n"),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatCloseDataBase::CalcContens()
//--------------------------------------------------------------------
{
  const char* alias = NULL;

  if(Arg(1)) 
    alias = Text(Arg(1)).String();
  else alias = "";

  contens_ = dbClose(alias);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatDBExecQuery);
DefExtOpr(1, BDatDBExecQuery, "DBExecQuery", 1, 1, "Text",
	  I2("(Text query)", "(Text consulta)"),
	  I2(".",
	     "Ejecuta una orden SQL de tipo inserción, borrado o "
	     "actualización y devuelve el número de registros afectados."
	     "Devuelve -1 en caso de error."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatDBExecQuery::CalcContens()
{
    const BChar* query = Text(Arg(1)).String();
    contens_ = dbExecQuery(query);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDBSeries);
DefExtOpr(1, BSetDBSeries, "DBSeries", 3, 5, "Text TimeSet Set Set Real",
	  I2("(Text query, TimeSet dating,"
	     " Set names [, Set descriptions, Real defaultValue=0])",
	     "(Text consulta , TimeSet fechado,"
	     " Set nombres [, Set descripciones, Real valorDefecto=0])"),
	  I2(".",
	     "Devuelve un conjunto de series con los nombres dados y el "
	     "fechado indicado (común para todas ellas) cuyos datos "
	     "vienen dados por una consulta a la base de datos abierta.\n"
	     "Dicha consulta debe devolver como primer campo una fecha y a "
	     "continuación un campo numérico para cada serie que se quiere "
	     "construir. Además debe estar ordenada por el campo fecha."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetDBSeries::CalcContens()
//--------------------------------------------------------------------
{
    BText B = "(Funcion DBSeries) ";

    BText query = Text(Arg(1));
    BUserTimeSet* dating = Tms(Arg(2));
    BSet names = Set (Arg(3));
    BSet descriptions;
    BDat defaultValue = 0;
    BText datingId = dating->Identify();
    int n = names.Card();
    int i=0,j;
    
    if(Arg(4)) { descriptions = Set(Arg(4)); }
    if(Arg(5)) { defaultValue = Dat(Arg(5)); }
    
    if(!DBOpenQuery(query,B)) { return; }

    int fields = DBTestTypes("Date Real",B,n+1);

    if(!fields) { return; }
    
    BArray<BData> data;
    data.ReallocBuffer(100);
    BDate date, first, last, prev = BDate::Begin();
    int reg = 0;
    
    do
    {
	reg++;
	date = DBGetDate(0);
	if(!date.HasValue())
	{
	    Warning(B+I2("Wrong date at register ",
			 "Fecha no válida en el registro ")+ reg);
	}
	else if(!dating->Contain(date))
	{
	    Warning(B + date.Name() 
		    + I2(" is not a date of dating ",
			 " no es una fecha del fechado ") 
		    + datingId + I2(" at register "," en el registro ")
		    + reg);
	}
	else
	{
	    if(i==0)
	    {
		first = date;
		prev  = dating->Predecessor(date);
	    }
	    if(date < prev)
	    {
		Warning(B + I2("Unorder date at register ",
			       "Fecha desordenada en el registro ")
			+ reg + date + " < " + prev);
	    }
	    else if(date == prev)
	    {
		Warning(B+I2("Repeated date at register ",
			     "Fecha repetida en el registro ")+ reg +" "+
			date+" == "+prev);
	    }
	    else
	    {
		prev = dating->Successor(prev);
		for(; prev<date; prev = dating->Successor(prev))
		{
		    if(i>=data.Size()) { data.ReallocBuffer(2*i); }
		    data[i].ReallocBuffer(n);
		    for(j=1; j<=n; j++) { data[i][j-1] = defaultValue; }
		    i++;
		}
		if(i>=data.Size()) { data.ReallocBuffer(2*i); }
		data[i].ReallocBuffer(n);
		for(j=1; j<=n; j++) { 
		    data[i][j-1] = DBGetReal(j,defaultValue); 
		}
		i++;
	    }
	}
    }
    while(dbGetNext());
    
    last = date;
    data.ReallocBuffer(i);
    
    BArray<BUserTimeSerie*> serie;
    serie.ReallocBuffer(n);
    int saveLevel = BGrammar::Level();
    
    if(!BTimeSerie::GlobalizeSeries().Value()) 
	BGrammar::IncLevel();
    else BGrammar::PutLevel(0);

    bool badName = false;
    BList *aux = 0;
    BList *seriesInstances = 0;
    int stackPos = BGrammar::StackSize();
    
    for(j=1; j<=n; j++)
    {
	BText nameAux = "";
	BText name = "";
	
	if(names[j] && names[j]->Grammar()==GraText())
	{
	    nameAux = Text(names[j]);
	    name=nameAux.RightCompact();

	    if(!(BParser::DefaultParser()->Filter()->IsIdentifier(name)))
	    {
	      /* Activar en TOL_1_1_4
	       * --------------------
		Error(B+I2("Wrong identifier: ",
			   "Identificador erróneo: ") + name);
		badName = true;
	      */
	      Deprecated(BText("'")+name+"'" + I2(" will be soon not accepted as an "
				   "identifier.", " no será aceptado"
				   "próximamente como identificador."));
	    }
	}

	if(!badName)
	{
	    BText desc = name;
	    BSyntaxObject *obj = 0;
	
	    // Looks for an object with the same name in the same grammar
	    if(BTimeSerie::GlobalizeSeries().Value()) {
		obj = GraSerie()->FindVariable(name);
		if(obj) {
		    Warning(B+I2("Serie ","La serie ") + "'"+ name +"'"+
			    I2(" can't be created because already exists."
			       "\nThe existing serie will be changed with "
			       "the new one\n"," no puede ser creada porque "
			       "ya existe.\nLa serie existente va a ser "
			       "reemplaza por la nueva.\n"));
		    GraSerie()->DelObject(obj);
		}
	    }
	
	    serie[j-1] = new BTsrPrimary("", "");
	    obj = new BTsrRenamed(name, serie[j-1]);
	
	    if(Arg(4) && descriptions[j] && 
	       descriptions[j]->Grammar()==GraText())
	    {
		desc = Text(descriptions[j]);
	    }
	    serie[j-1]->PutDescription(desc);
	    serie[j-1]->PutDating(dating);
	    serie[j-1]->Realloc(data.Size());
	    for(i=0;i<data.Size();i++)
	    { serie[j-1]->PutBufDat(i,data[i][j-1]); }
	    serie[j-1]->PutFirstDate(first);
	    serie[j-1]->PutLastDate(last);
	    serie[j-1]->PutLength(data.Size());
	    LstFastAppend(seriesInstances, aux, obj);
	}
    }

    contens_.RobElement(seriesInstances);

    if(!BTimeSerie::GlobalizeSeries().Value())
    {
	BGrammar::DecLevel();
	BGrammar::DestroyStackUntil(stackPos, 0);
    } 
    else
    {
	BGrammar::PutLevel(saveLevel);
	if(saveLevel>0) 
	    BGrammar::DestroyStackUntil(stackPos, 0);
    }
    
    dbCloseQuery();
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDBSeriesColumn);
DefExtOpr(1, BSetDBSeriesColumn, "DBSeriesColumn", 2, 3,
	  "Text TimeSet Real",
	  I2("(Text query, TimeSet dating [, Real defaultValue=0])",
	     "(Text consulta , TimeSet fechado [, Real valorDefecto=0])"),
	  I2(".",
	     "Devuelve un conjunto de series con el fechado indicado (común "
	     "para todas ellas) cuyos datos vienen dados por una consulta a "
	     "la base de datos abierta. Dicha consulta debe devolver tres "
	     "campos que contengan un nombre de serie, una fecha y un número "
	     "real. Además debe estar ordenada por el campo serie y el campo "
	     "fecha."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetDBSeriesColumn::CalcContens()
//--------------------------------------------------------------------
{
    BText B = "(Funcion DBSeriesColumn) ";

    BText         query        = Text(Arg(1));
    BUserTimeSet* dating       = Tms (Arg(2));
    BDat          defaultValue = 0;
    BText         datingId     = dating->Identify();

    if(Arg(3)) { defaultValue = Dat(Arg(3)); }

    if(!DBOpenQuery(query,B)) { return; }

    BInt fields = DBTestTypes("Text Date Real",B,3);
    
    if(!fields) { return; }
    
    BDat dat;
    BData data;
    BDate date, first, prev = BDate::Begin(), last;
    
    BText lastName, name, nameAux;
    BInt i   = 0;
    BInt j   = 0;
    BInt reg = 0;
    bool badName = false;
    
    int saveLevel = BGrammar::Level();

    if(!BTimeSerie::GlobalizeSeries().Value()) 
	BGrammar::IncLevel();
    else BGrammar::PutLevel(0);

    BList *aux = 0;
    BList *seriesInstances = 0;
    int stackPos = BGrammar::StackSize();

    int next = BTRUE, end = BFALSE, newSerie;
    data.ReallocBuffer(1000);

    do
    {
	end = !next;
	last = date;

	if(!end)
	  {
	    reg++;
	    nameAux = DBGetText(0);
	    name = nameAux.RightCompact();
	       
	    date = DBGetDate(1);
	    dat  = DBGetReal(2,defaultValue);
	    newSerie = lastName.HasName() && (name!=lastName);

	    if((name!=lastName) && 
	       !(BParser::DefaultParser()->Filter()->IsIdentifier(name)))
	      {
		/* Activar en TOL_1_1_4
		 * --------------------
		 Error(B+I2("Wrong identifier: ",
		 "Identificador erróneo: ") + name);
		 badName = true;
		*/
		Deprecated(BText("'")+name+"'" + I2(" will be soon not accepted as an "
				     "identifier.", " no será aceptado "
				     "próximamente como identificador."));
	      }
	  }

	if(!badName)
	{
	    if(end || newSerie)
	    {
		j++;
		data.ReallocBuffer(i);
		i=0;
		
		BUserTimeSerie* serie = 0;
		BSyntaxObject* obj = 0;
		
		// Looks for an object with the same name in the same grammar
		if(BTimeSerie::GlobalizeSeries().Value()) {
		    obj = GraSerie()->FindVariable(lastName);
		    if(obj) {
			Warning(B+I2("Serie ","La serie ") + "'"+lastName +"'"+
				I2(" can't be created because already exists."
				   "\nThe existing serie will be changed with "
				   "the new one\n", " no puede ser creada "
				   "porque ya existe.\nLa serie existente "
				   "va a ser reemplaza por la nueva.\n"));
			GraSerie()->DelObject(obj);
		    }
		}
		
		serie = new BTsrPrimary("", "");
		obj = new BTsrRenamed(lastName, serie);
		
		serie->PutDating(dating);
		serie->Realloc(data.Size());
		
		for(BInt k=0; k<data.Size(); k++) 
		    serie->PutBufDat(k,data[k]);
		
		serie->PutFirstDate(first);
		serie->PutLastDate(last);
		serie->PutLength(data.Size());
		serie->PutDescription(lastName);
		LstFastAppend(seriesInstances, aux, obj);
	    }
	    
	    if(!end)
	    {
		if(!date.HasValue())
		{
		    Warning(B+I2("Wrong date at register ",
				 "Fecha no válida en el registro ")+ reg);
		}
		else if(!dating->Contain(date))
		{
		    Warning(B+date.Name() + I2(" is not a date of dating ",
					       " no es una fecha del "
					       "fechado ") + datingId 
			    + I2(" at register "," en el registro ") + reg);
		}
		else
		{
		    if(i==0)
		    {
			first = date;
			prev = dating->Predecessor(date);
		    }
		    if(date < prev)
		    {
			Warning(B+I2("Unorder date at register ", "Fecha "
				     "desordenada en el registro ")+ reg +
				date+" < "+prev);
		    }
		    else if(date == prev)
		    {
			Warning(B+I2("Repeated date at register ",
				     "Fecha repetida en el registro ")+ reg +" "+
				date+" == "+prev);
		    }
		    else
		    {
			prev = dating->Successor(prev);
			for(; prev<date; prev = dating->Successor(prev))
			{
			    if(i>=data.Size()) { data.ReallocBuffer(2*i); }
			    data[i] = defaultValue;
			    i++;
			}
			if(i>=data.Size()) { data.ReallocBuffer(2*i); }
			data[i] = dat;
			i++;
		    }
		    lastName = name;
		}
	    }
	}
	next = dbGetNext();
    }
    while(!end);

    contens_.RobElement(seriesInstances);

    if(!BTimeSerie::GlobalizeSeries().Value())
    {
	BGrammar::DecLevel();
	BGrammar::DestroyStackUntil(stackPos, 0);
    } 
    else
    {
	BGrammar::PutLevel(saveLevel);
	if(saveLevel>0) 
	    BGrammar::DestroyStackUntil(stackPos, 0);
    }

    dbCloseQuery();
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDBSeriesTable);
DefExtOpr(1, BSetDBSeriesTable, "DBSeriesTable", 3, 5,
  "Text TimeSet Set Set Real",
  I2("(Text query, TimeSet dating,"
     " Set names [, Set descriptions, Real defaultValue=0])",
     "(Text consulta , TimeSet fechado,"
     " Set nombres [, Set descripciones, Real valorDefecto=0])"),
  I2(".",
     "Devuelve un conjunto bidimensional de series con el fechado "
     "indicado (común para todas ellas) cuyos datos vienen dados por una "
     "consulta a la base de datos abierta. "
     "Dicha consulta debe devolver al menos tres campos que contengan un "
     "prefijo para el nombre de la serie, una fecha y un número real. Además "
     "debe estar ordenada por el campo prefijo y el campo fecha."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetDBSeriesTable::CalcContens()
//--------------------------------------------------------------------
{
    BText B = "(Funcion DBSeriesTable) ";

    BText	  query        = Text(Arg(1));
    BUserTimeSet* dating       = Tms (Arg(2));
    BDat	  defaultValue = 0;
    BText	  datingId     = dating->Identify();
    
    BSet names = Set (Arg(3));
    BSet descriptions;
    int n = names.Card();
    
    if(Arg(4)) { descriptions = Set(Arg(4)); }
    if(Arg(5)) { defaultValue = Dat(Arg(5)); }
    
    if(!DBOpenQuery(query,B)) { return; }
    
    BInt fields = DBTestTypes("Text Date Real",B,n+2);
    
    if(!fields) { return; }
    
    BArray<BUserTimeSerie*> serie(n);
    BArray<BDat> dat(n);
    BArray<BData> data(n);
    BDate date, first, prev = BDate::Begin(), last;
    
    BText lastName, name, serName, nameAux;
    BInt i   = 0;
    BInt k   = 0;
    BInt reg = 0;
    BList* lst = NIL;
    BList* aux = NIL;
    
    bool badName = true;
    int saveLevel = BGrammar::Level();
    
    if(!BTimeSerie::GlobalizeSeries().Value()) 
	BGrammar::IncLevel();
    else BGrammar::PutLevel(0);
    
    int next = BTRUE, end = BFALSE, newSerie;
    for(k=0; k<n; k++) { data(k).ReallocBuffer(1000); }

    do
    {
	end = !next;
	last = date;

	if(!end)
	{
	    reg++;
	    nameAux = DBGetText(0);
	    name = nameAux.RightCompact();
	    date = DBGetDate(1);
	    for(k=0; k<n; k++) dat(k) = DBGetReal(2+k,defaultValue);
	    newSerie = lastName.HasName() && (name!=lastName);
	}

	if(end || newSerie)
	  {
	    BList* lst2=NIL;
	    BList* aux2=NIL;
	    
	    for(k=0; k<n; k++)
	      {
		data(k).ReallocBuffer(i);
		serName = lastName+Text(names[k+1]);
		
		// Check the name for the each Serie
		if(!(BParser::DefaultParser()->Filter()->IsIdentifier(serName)))
		  {
		    /* Activar en TOL_1_1_4
		     * --------------------
		     Error(B+I2("Wrong identifier: ",
		     "Identificador erróneo: ") + serName);
		    */
		    Deprecated(BText("'")+serName+"'" + I2(" will be soon not accepted as an "
					    "identifier.", " no será aceptado "
					    "próximamente como identificador."));
		  } 
		/*
		  else
		  {
		*/
		if(BTimeSerie::GlobalizeSeries().Value())
		  serie(k) = (BUserTimeSerie*)
		    GraSerie()->FindVariable(serName);
		
		if(!serie(k) && BTimeSerie::GlobalizeSeries().Value()) 
		  {
		    serie(k) = new BTsrPrimary(serName, "");
		  } 
		else {
		  serie(k) = new BTsrPrimary("", "");
		  serie(k)->PutName(serName);
		  
		  if(BTimeSerie::GlobalizeSeries().Value())
		    Warning(B+I2("Serie ","La serie ")+ serName +
			    I2(" can't be created as global object "
			       "because already exists"," no puede "
			       "ser creada como global porque ya "
			       "existe"));
		}
		
		serie(k)->PutDating(dating);
		serie(k)->Realloc(i);
		for(BInt t=0; t<i; t++)
		  { serie(k)->PutBufDat(t,data(k)(t)); }
		serie(k)->PutFirstDate(first);
		serie(k)->PutLastDate(last);
		serie(k)->PutLength(i);
		LstFastAppend(lst2, aux2, serie(k));
		GraSerie()->AddObject(serie(k));
		/*
		  }
		*/
	      }
	
	    // Check the name for the Set variable
	    if(!(BParser::DefaultParser()->Filter()->IsIdentifier(lastName)))
	      {
	      /* Activar en TOL_1_1_4
	       * --------------------
		Error(B+I2("Wrong identifier: ",
			   "Identificador erróneo: ") + lastName);
	      */
		Deprecated(BText("'")+ name+"'"+ I2(" will be soon not accepted as an "
				     "identifier.", " no será aceptado "
				     "próximamente como identificador."));
	      } 
	    /*
	    else 
	      {
	    */
		BUserSet* set = new BContensSet();
		set->PutName(lastName);
		set->PutDescription(lastName);
		set->Contens().RobElement(lst2);
		LstFastAppend(lst, aux, set);
            /*
	      }
            */
	    
	    i=0;
	  }
	
	if(!end)
	  {
	    if(!date.HasValue())
	      {
		Warning(B+I2("Wrong date at register ",
			     "Fecha no válida en el registro ")+ reg);
	      }
	    else if(!dating->Contain(date))
	      {
		Warning(B+date.Name()+I2(" is not a date of dating ",
					 " no es una fecha del fechado ") 
			+ datingId + I2(" at register "," en el registro ") 
			+ reg);
	      }
	    else
	      {
		if(i==0)
		  {
		    first = date;
		    prev  = dating->Predecessor(date);
		  }
		
		if(date < prev)
		  {
		    Warning(B+I2("Unorder date at register ",
				 "Fecha desordenada en el registro ") 
			    + reg + date + " < " + prev);
		  }
		else if(date == prev)
		  {
		    Warning(B+I2("Repeated date at register ",
				 "Fecha repetida en el registro ")
			    + reg + " "+ date + " == " + prev);
		  }
		else
		  {
		    prev = dating->Successor(prev);
		    for(; prev<date; prev = dating->Successor(prev))
		      {
			if(i>=data(0).Size())
			  {
			    for(k=0; k<n; k++) { data(k).ReallocBuffer(2*i); }
			  }
			
			for(k=0; k<n; k++) { data(k)(i) = defaultValue; }
			
			i++;
		      }
		    
		    if(i>=data(0).Size())
		      {
			for(k=0; k<n; k++) { data(k).ReallocBuffer(2*i); }
		      }
		    
		    for(k=0; k<n; k++) { data(k)(i) = dat(k); }
		    i++;
		  }
		
		lastName = name;	
		next = dbGetNext();
	      }
	  }
    }
    while(!end);
    
    if(!BTimeSerie::GlobalizeSeries().Value())
      BGrammar::DecLevel();
    else BGrammar::PutLevel(saveLevel);
    
    contens_.RobElement(lst);
    dbCloseQuery();
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatDBMatrix);
DefExtOpr(1, BMatDBMatrix, "DBMatrix", 1, 2, "Text Real",
	  I2("(Text query [, Real defaultValue=0])",
	     "(Text consulta [, Real valorDefecto=0])"),
	  I2(".",
	     "Devuelve una matriz cuyos datos vienen dados por una "
	     "consulta a la base de datos abierta. "
	     "Dicha consulta debe devolver sólo campos numéricos."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BMatDBMatrix::CalcContens()
//--------------------------------------------------------------------
{
    BText B = "(Funcion DBMatrix) ";
    BInt i,j;
    BText query  = Text(Arg(1));
    BDat defaultValue = 0;
    if(Arg(2)) { defaultValue = Dat(Arg(2)); }
    
    if(!DBOpenQuery(query,B)) { return; }
    BInt fields = DBTestTypes("Real",B);
    if(!fields) { return; }
    BArray< BArray<BDat> > data;
    data.ReallocBuffer(100);
    i = 0;
    do
    {
	if(i>=data.Size()) { data.ReallocBuffer(2*i); }
	data[i].ReallocBuffer(fields);
	for(j=0; j<fields; j++) { data[i][j] = DBGetReal(j,defaultValue); }
	i++;
    }
    while(dbGetNext());
    
    contens_ = BMat(i,fields,data);
    dbCloseQuery();
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDBTable);
DefExtOpr(1, BSetDBTable, "DBTable", 1, 2, "Text Text",
	  I2("(Text query [, Text structName])",
	     "(Text consulta [, Text nombreEstructura])"),
	  I2(".",
	     "Devuelve un conjunto con estructura de tabla cuyos datos "
	     "vienen dados por una consulta a la base de datos abierta. "
	     "Dicha consulta debe devolver sólo campos numéricos, de "
	     "texto o fechas."),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BSetDBTable::CalcContens()
//--------------------------------------------------------------------
{
    BText B = "(Funcion DBTable) ";
    BText query  = Text(Arg(1));
    BStruct* str = NIL;
    BInt i,j;
    char **colName;
    
    if(Arg(2))
    {
	BText strName = Text(Arg(2));
	str = FindStruct(strName);
	if(!str)
	{
	    Warning(BText("DBTable : ") + strName +
		    I2(" is not a valid structure",
		       " no es una estructura válida"));
	}
    }
    if(!DBOpenQuery(query,B)) { return; }
    BInt fields = dbGetFieldsNumber();
    if(fields<=0) { 
      Error(I2("invalid query\n","consulta no válida\n"));
      return; 
    }

    BArray<BInt> types;
    colName = (char **)malloc( fields * sizeof(char*));
    types.ReallocBuffer(fields);

    for(j=0; j<fields; j++)
    {
	types[j] = dbGetFieldType(j);

	if(types[j]==TypeDBUnknown)
	{
	    Warning(B+I2("Query must return a text, a date or a number at "
			 "the field number", "La consulta debería devolver "
			 "un texto, una fecha o un número en el campo ") 
		    + (j+1));
//    return;
	}

	colName[j]=(char *)malloc(128*sizeof(char));
	strcpy(colName[j], "unnamed");
	dbGetFieldName(j,colName[j]);
    }
    
    BList* lst = NIL;
    BList* aux = NIL;
    i = 0;
    BText txtRow = "row ";

    do
    {
	BList* lst2 = NIL;
	BList* aux2 = NIL;

	for(j=0; j<fields; j++)
	{
	    BSyntaxObject* elem = NIL;
	    switch(types[j])
	    {
		case TypeDBUnknown :
		    elem = BContensText::New(DBGetText(j));
		    break;

		case TypeDBText :
		    elem = BContensText::New(DBGetText(j));
		    break;

		case TypeDBDate :
		    elem = BContensDate::New(DBGetDate(j));
		    break;

		case TypeDBReal :
		    elem = BContensDat::New(DBGetReal(j));
		    break;
	    }
	    
	    elem->PutName(colName[j]);
	    LstFastAppend(lst2,aux2,elem);
	}

	BSet set;
	set.RobStruct(lst2,str,BSet::Structured);
	BUserSet * row = BContensSet::New(set);
//row->PutDescription( txtRow + i );
	LstFastAppend(lst,aux,row);
	i++;
    }
    while(dbGetNext());

    for(j=0; j<fields; j++) free(colName[j]);
    free(colName);

    contens_.RobElement(lst);
    contens_.PutSubType( BSet::Table );
    dbCloseQuery();
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatDBCreateSeriesTable);
DefExtOpr(1, BDatDBCreateSeriesTable, "DBCreateSeriesTable", 2, 6,
	  "Text Set Text Text Text Set",
	  "(Text tableName, Set seriesSet, "
	  "[Text dateColName , Text dateColType,"
	  " Text dateFormat, Set serNames])",
	  I2("Creates a table, in the active database, with all series "
	     "in seriesSet. Deletes table tableName if exist, before creates "
	     "the new one.\n\n"
	     "Text tableName   -> Name of table to create\n"
	     "Set  seriesSet   -> Set of Serie to insert in tableName\n"
	     "Text dateColName -> Name of column with dates\n"
	     "Text dateColType -> Type of column with dates. Default value is "
	     "'DateTime'. Could be any supported.\n"
	     "Text dateFormat  -> Date column format used to insert each "
	     "date. Default value is '%Y/%m/%d %h:%i:%s'\n"
	     "Set  serNames    -> Set of Text with the column names for "
	     "each Serie. Default ist the Serie name.\n\n"
	     "1st Example:\n"
	     "  Serie ser1 = SubSer(Gaussian(0,1,Daily), y2005m1d1, "
	     "y2005m12d31);\n"
	     "  Serie ser2 = SubSer(Gaussian(0,1,Daily), y2005m1d1, "
	     "y2005m12d31);\n"
	     "  Real DBCreateSeriesTable(\"seriesA\", SetOfSerie(ser1, "
	     "ser2));\n"
	     "Creates table 'seriesA' with 3 columns and 365 rows. "
	     "First column represents day values between 1/1/2005 and "
	     "12/31/2005. Second and third columns have ser1 and ser2 "
	     "values.\n\n"
	     "2nd Example:\n"
	     "  Serie ser3 = SubSer(Gaussian(0,1,Daily), y2005m1d1, "
	     "y2005m12d31); // define 2005 values\n"
	     "  Serie ser4 = SubSer(Gaussian(0,1,Daily), y2006m1d1, "
	     "y2006m12d31); // define 2006 values\n"
	     "  Real DBCreateSeriesTable(\"seriesB\", SetOfSerie(ser1, "
	     "ser2));\n"
	     "Creates table 'seriesB' with 3 columns and 730 rows. "
	     "First column represents day values between 1/1/2005 and "
	     "12/31/2006. Second and third columns have ser3 y ser4 values,"
	     "being NULL for each date out of their own time interval"
	     ", where each of these series were defined.\n",
	     "Crea una tabla, en la base de datos activa, con las series "
	     "contenidas en el conjunto seriesSet. Si tableName existe, "
	     "la borra y vuelve a crearla con el contenido de seriesSet.\n\n"
	     "Text tableName   -> Nombre de la tabla\n"
	     "Set  seriesSet   -> Conjunto de series a insertar en la tabla\n"
	     "Text dateColName -> Nombre de la columna con los instantes de "
	     "tiempo\n"
	     "Text dateColType -> Tipo del campo con los instantes de tiempo. "
	     "Por defecto es 'DateTime'. Puede ser cualquiera soportado por "
	     "la Base de Datos.\n"
	     "Text dateFormat  -> Formato del campo fecha, por defecto es "
	     "'%Y/%m/%d %h:%i:%s'\n"
	     "Set  serNames    -> Conjunto de variables Text con los nombres "
	     " de las columnas de cada Serie\n\n"
	     "Ejemplo 1:\n"
	     "  Serie ser1 = SubSer(Gaussian(0,1,Diario), y2005m1d1, "
	     "y2005m12d31);\n"
	     "  Serie ser2 = SubSer(Gaussian(0,1,Diario), y2005m1d1, "
	     "y2005m12d31);\n"
	     "  Real DBCreateSeriesTable(\"seriesA\", SetOfSerie(ser1, "
	     "ser2));\n"
	     "Crea la tabla 'seriesA' con 3 columnas y 365 filas. La primera "
	     "columna contiene los dias desde el 1-1-2005 al 31-12-2005. La "
	     "segunda y tercera columnas almacenan los valores de las "
	     "variables ser1 y ser2 respectivamente.\n\n"
	     "Ejemplo 2:\n"
	     "  Serie ser3 = SubSer(Gaussian(0,1,Diario), y2005m1d1, "
	     "y2005m12d31); // definida en 2005\n"
	     "  Serie ser4 = SubSer(Gaussian(0,1,Diario), y2006m1d1, "
	     "y2006m12d31); // definida en 2006\n"
	     "  Real DBCreateSeriesTable(\"seriesB\", SetOfSerie(ser1, "
	     "ser2));\n"
	     "Crea la tabla 'seriesB' con 3 columnas y 730 filas. La primera "
	     "columna contiene los dias desde el 1-1-2005 al 31-12-2006. La "
	     "segunda y tercera columnas almacenan los valores de las "
	     "variables ser3 y ser4 respectivamente.\n"
	     "La columna ser3 y ser4 toman valores NULL desde el 1-1-2006 "
	     "hasta el 31-12-2006, y desde el 1-1-2005 hasta el 31-12-2005 "
	     "respectivamente.\n"),
	  BOperClassify::BayesDataBase_);
//--------------------------------------------------------------------
void BDatDBCreateSeriesTable::CalcContens()
//--------------------------------------------------------------------
{
    BText B          = "(Funcion DBCreateSeriesTable) ";
    BText name	     = Text(Arg(1));
    BSet& series     = Set(Arg(2));
    BText dateName   = "Date";
    BText dateType   = "Timestamp";
    BText dateFormat = "%Y/%m/%d %h:%i:%s";

    BSet setNames;
    BArray<BText> serName;
    BInt i=0, j, n=series.Card();
    
    contens_ = 0;

    if(Arg(3)) { dateName   = Text(Arg(3)); }
    if(Arg(4)) { dateType   = Text(Arg(4)); }
    if(Arg(5)) { dateFormat = Text(Arg(5)); }
    if(Arg(6))
    {
      setNames = Set (Arg(6));
      if(setNames.Card()!=n)
      {
	Error(B+I2("Wrong number of elements in 'serNames' argument.",
		   "Número equivocado de elementos en el argumento "
		   "'nombres'."));
	return;
      }
    }

    serName.ReallocBuffer(n);
    BText fields	   = dateName;
    BText dropQuery	   = BText("drop table ") + name;
    BText createTableQuery = BText("create table ") + name + "\n(\n  " 
	+ dateName + " " + dateType;

    for(j=0; j<n; j++)
    {
	if(!series[j+1] || (series[j+1]->Grammar()!=GraSerie()))
	{
	    Error(B+I2("Not Serie element found in argument 'series' at "
		       "position ", "Elemento de tipo distinto de Serie "
		       "encontrado en el argumento 'series' en la "
		       "posición ") + (j+1));
	    return;
	}

	if(Arg(6))
	{
	    if(!setNames[j+1] || (setNames[j+1]->Grammar()!=GraText()))
	    {
		Error(B+I2("Not Text element found in argument 'serNames' at "
			   "position ", "Elemento de tipo distinto de Text "
			   "encontrado en el argumento 'nombres' en la "
			   "posición ") + (j+1));
		return;
	    }

	    serName[j] = Text(setNames[j+1]);
	}
	else serName[j] = series[j+1]->Name();

	if(!serName[j].HasName())
	{
	    Error(B+I2("Unresolved field name at position ", "No se ha "
		       "resulto el nombre de campo para la posición ") 
		  + (j+1));
	    return;
	}

	createTableQuery += BText(",\n  ") + serName[j] + " Real";
	fields += BText(", ") + serName[j];
    }

    createTableQuery += BText(",\n  primary key(") + dateName + ")\n)\n";

    BSerieTable table;
    table.AddSet(series);
    table.Fill();
    if(table.NumSeries()!=n) { return; };

    int drop = dbExecQuery(dropQuery.String());
    int	created = dbExecQuery(createTableQuery.String());
    BDateFormat format(dateFormat);

    if(created == -1) //== The table cannot be created
    {
      Error(I2("The table '" + name + "' cannot be created.\n",
	       "La tabla '" + name + "' no ha podido crearse.\n"));
    }
    else         //== The table was created
    {
      for(i=0; i<table.NumDates(); i++)
      {
	BText validFields = dateName;
	BText validValues = BText("'")+table.Date(i+1).GetText(format)+"'";
	BInt  validNumber = 0;
	
	for(j=0; j<n; j++)
	{
	  BDat x = table.Dat(j+1,i+1);
	      
	  if(x.IsKnown())
	  {
	    validNumber ++;
	    validFields += BText(", ")+serName[j];
	    validValues += BText(", ")+x;
	  }
	}
	
	if(validNumber)
	{
	  BText insertQuery = 
	    BText("insert into ") + name 
	    + "\n(" + validFields + ")" + "\nvalues(" + validValues +")";
	  BInt  insert = dbExecQuery(insertQuery.String());
	  if(!insert) break;
	}
      }
    }

    contens_ = i;
}

//////////////////////////////////////////////////////////////////////////////
DeclareContensClass(BSet, BSetTemporary, BSetDBTableColumn);
DefExtOpr(1, BSetDBTableColumn, "DBTableColumn", 3, 6,
  "Text Real Real Real Text Real",
  I2("(Text query, Real indexNumber , Real prefixNumber "
     "[, Real invertName, Text pad=\"\", Real defaultValue=?])",
     "(Text consulta, Real numeroIndices, Real numeroPrefijos "
     "[, Real invertirNombre=0, Text separador=\"\", Real valorDefecto=?])"),
  I2("\n  Returns a table-structured set which data comes from an"
     " SQL query made to the active DDBB.\n\n"
     "  The query must return the index fields first, followed by the"
     " prefixes and numeric series, and must be ordered by index"
     " and prefixes.\n\n"
     "  The index and prefix fields can be of any type,"
     " but series should have numeric values.\n\n"
     "  The optional arguments invertName and pad can be"
     " used respectively to invert the column's names and to insert"
     " a string between prefixes and serie names.\n\n"
     "  The defectValue argument is a Real value which will be"
     " used every time an empty serie is found instead of the Unknown value.",
     "\n  Devuelve un conjunto con estructura de tabla cuyos datos"
     " vienen dados por una consulta a la base de datos abierta.\n\n"
     "  Dicha consulta debe devolver en primer lugar los campos índice"
     " seguidos por los campos prefijo y terminando por los campos"
     " numéricos correspondientes a las series y debe estar ordenada por"
     " los campos indice y prefijo.\n\n"
     " Además, mientras que los campos índice y prefijo pueden ser de"
     " cualquier tipo, el resto tiene que ser de tipo numérico.\n\n"
     " Los argumentos opcionales invertirNombre y separador"
     " sirven para invertir el nombre de las columnas serie e"
     " insertar una cadena de texto entre ellos respectivamente.\n\n"
     "  El argumento valorDefecto debe ser un real cuyo valor adquirirán"
     " todas las celdas sin valor. Por defecto toma el valor Desconocido."),
	  BOperClassify::BayesDataBase_);
//----------------------------------------------------------------------------
void BSetDBTableColumn::CalcContens()
//----------------------------------------------------------------------------
{
//TODO: Use vector methods when inserting elements into prefixNames.
//TODO: Use any data type in serie fields.
//TODO: Use any data type in prefix fields.
//NOTA: NO puede usarse map para llevar la relación datos-prefijos, porque se
// pretende permitir datos de todo tipo, tanto en series (lo cual no sería un
// problema) como en prefijos, lo cual si incompatibiliza el uso de map.

  BText B = "(Funcion DBTableColumn) ";
  BText query  = Text(Arg(1));
  BInt indexNumber = (BInt)Real(Arg(2));
  BInt prefixNumber = (BInt)Real(Arg(3));
  BInt invertName = 0;
  BText padString;
  BDat defaultValue;

  if(Arg(4)) { invertName = (BInt)Real(Arg(4)); }
  if(Arg(5)) { padString = Text(Arg(5)); }
  if(Arg(6))
  {
    defaultValue = (BDat)Real(Arg(6));
  }

  BStruct* str = NIL;
  unsigned long int i, k, numRow;
  int j;

  if ((prefixNumber < 1) || (indexNumber < 1))
  {
    Warning(B+I2(" Wrong index and/or prefix number.",
    "Número de prefijos o índices menor a la unidad.") );
    return;
  }

  if(!DBOpenQuery(query,B)) { return; }

  BInt fields = dbGetFieldsNumber();

  if(fields<=0) { 
    Error(I2("invalid query\n","consulta no válida\n"));
    return; 
  }
  if(fields <= (indexNumber + prefixNumber))
  {
    Warning(B+I2(" Wrong index and/or prefix number. "+fields+
    " columns were returned while "+indexNumber+" indexes and "+
    prefixNumber+" prefixes were specified.",
    "La consulta ha devuelto "+fields+" campos, y se han especificado "+
    indexNumber +" indices y "+ prefixNumber +" prefijos.") );
    return;
  }
  BInt serieNumber = fields-indexNumber-prefixNumber;

  std::vector<char> types(fields);

  //TO STL (Not yet, type mismatch in some operations):
  //unsigned char fieldNames[indexNumber][64];
  
  //Using string type would add a relatively big speed decrease. 64 chars seems ok.
  std::vector<std::string> fieldNames(indexNumber);

  //To STL (Not yet, type mismatch in some operations):
/*  BArray <> serieNames;
  serieNames.ReallocBuffer(serieNumber);
  for(i=0;i<serieNumber;i++)
    serieNames[i]
*/
  std::vector<std::string> serieNames(serieNumber);
  //Using a string type would add a relatively big speed decrease. 64 chars seems ok.

  std::vector <BAny> currentIndex(indexNumber);
  std::vector <BAny> lastIndex(indexNumber);

  std::vector <BText> prefixNames;//same here: Will grow when necessary.

  //First run: check fields and types.
  //Indexes
  char txt[128];
  for(j=0;j<indexNumber; j++)
  {
    types[j] = dbGetFieldType(j);
    switch(types[j])
    {
    case TypeDBUnknown :
      Warning(B+I2("Query must return a text, a date or a number at the field "
        "number",
        "La consulta debería devolver un texto, una fecha o un "
        "número en el campo ") + (j+1));
    case TypeDBText :
    {
        lastIndex[j] = new BText();
        currentIndex[j] = new BText();
        break;
    }
    case TypeDBDate :
      {
        lastIndex[j] = new BDate();
        currentIndex[j] = new BDate();
        break;
      }
    case TypeDBReal :
      {
        lastIndex[j] = new BDat();
        currentIndex[j] = new BDat();
        break;
      }
    }
    dbGetFieldName(j, txt); 
    fieldNames[j] = (char*)txt;
  }

  //Prefix:
  for(;j<(prefixNumber + indexNumber); j++)
  {
    types[j] = dbGetFieldType(j);
    if((types[j]!=TypeDBText) && (types[j]!=TypeDBDate) && (types[j]!=TypeDBReal))
    {
      Warning(B+I2("Query must return a text, date or real at the prefix field "
        "number ",
        "La consulta debe devolver un texto, fecha o real "
        "en el campo prefijo ") + (j+1));
      //CLEAN UP:
      for(i=0;i<indexNumber;i++)
      {
        switch(types[i])
        {
        case TypeDBUnknown :
        case TypeDBText :
        {
          delete ((BText*)lastIndex[i]);
          delete ((BText*)currentIndex[i]);
          break;
        }
        case TypeDBDate :
         {
           delete ((BDate*)lastIndex[i]);
           delete ((BDate*)currentIndex[i]);
           break;
         }
        case TypeDBReal :
        {
          delete ((BDat*)lastIndex[i]);
          delete ((BDat*)currentIndex[i]);
          break;
        }
        }
      }
      return;
    }
  }

  //Series:
  for(;j<fields; j++)
  {
    types[j] = dbGetFieldType(j);
    if(types[j] != TypeDBReal)
    {
      Warning(B+I2("Query must return a number at the field "
        "number ",
        "La consulta debe devolver un número en el campo ") + (j+1));
      //CLEAN UP:
      for(i=0;i<indexNumber;i++)
      {
        switch(types[i])
        {
        case TypeDBUnknown :
        case TypeDBText :
        {
          delete ((BText*)lastIndex[i]);
          delete ((BText*)currentIndex[i]);
          break;
        }
        case TypeDBDate :
         {
           delete ((BDate*)lastIndex[i]);
           delete ((BDate*)currentIndex[i]);
           break;
         }
        case TypeDBReal :
        {
          delete ((BDat*)lastIndex[i]);
          delete ((BDat*)currentIndex[i]);
          break;
        }
        }
      }
      return;
    }

    dbGetFieldName(j, txt);
    (serieNames[(j-indexNumber-prefixNumber)]) = (char *)txt;
  }

  //Fixed field name length por a faster access:
  //unsigned char fieldName[64];

  BList* lst = NIL;
  BList* aux = NIL;
  i = 0, numRow = 0;

  BText txtRow = "row ";
  BList* lst2 = NIL;
  BList* aux2 = NIL;
  BSyntaxObject* data;

  BText *tmpPrefix = new BText();
  int lastPrefix=-1;

  //For each row:
  do
  {
    //INDEX FIELDS:
    //////////////////////////////////////////////////////////////////////////
    int newRow = 0;
    for(j=0; j<indexNumber; j++)
    {
      switch(types[j])
      {
      case TypeDBUnknown :
      case TypeDBText :
        {
          ((BText*)(currentIndex[j]))->Copy(BText(DBGetText(j)));
          if (strcmp(((BText*)(currentIndex[j]))->String(),
                      ((BText*)(lastIndex[j]))->String()) != 0)
          {
            ((BText*)(lastIndex[j]))->Copy(((BText*)(currentIndex[j]))->String());
            newRow=1;
          }
          break;
        }
      case TypeDBDate :
        {
          *((BDate*)(currentIndex[j])) = DBGetDate(j);
          if (((BDate*)(currentIndex[j]))->SecondIndex() != ((BDate*)(lastIndex[j]))->SecondIndex())
          {
            *((BDate*)lastIndex[j])=*((BDate*)currentIndex[j]);
            newRow=1;
          }
          break;
        }
      case TypeDBReal :
        {
          *((BDat*)(currentIndex[j])) = DBGetReal(j);
          if  (((BDat*)(currentIndex[j]))->Value() != ((BDat*)(lastIndex[j]))->Value())
          {
            ((BDat*)(lastIndex[j]))->PutValue(((BDat*)currentIndex[j])->Value());
            newRow=1;
          }
          break;
        }
      }
    }

    //Check if the indexes define a new row (skip the first one):
    if ((i!=0) && (newRow==1))//New row in result table.
    {
      //Fill-up default remaining values
      for(k=(lastPrefix+1);k<prefixNames.size();k++)
      {
        for(int ser=0;ser<serieNumber;ser++)
        {
          BSyntaxObject* elemTmp = NIL;
          elemTmp = (numRow==0)?
              BContensDat::New((char *)(fieldNames[k]).c_str(),defaultValue,(char *)(fieldNames[k]).c_str()):
              BContensDat::New(defaultValue);
          LstFastAppend(lst2,aux2,elemTmp);
        }
      }
      BSet set;
      set.RobStruct(lst2,str,BSet::Structured);
      BUserSet * row = BContensSet::New(set);
    //row->PutDescription( txtRow + (int)numRow++ );
      LstFastAppend(lst,aux,row);
      lst2 = NIL;
      aux2 = NIL;
      lastPrefix=-1;
    }
    if((newRow==1) || (i==0))
    {
      for (k=0;k<indexNumber;k++)
      {
        switch(types[k])
        {
        case TypeDBUnknown :
        case TypeDBText :
          {
            data = (numRow==0)?
              BContensText::New((char *)(fieldNames[k]).c_str(),*((BText*)currentIndex[k]),(char *)(fieldNames[k]).c_str()):
              BContensText::New(*((BText*)currentIndex[k]));
            break;
          }
        case TypeDBDate :
          {
            data = (numRow==0)?
              BContensDate::New((char *)(fieldNames[k]).c_str(),*((BDate*)currentIndex[k]),(char *)(fieldNames[k]).c_str()):
              BContensDate::New(*((BDate*)currentIndex[k]));
            break;
          }
        case TypeDBReal :
          {
            data = (numRow==0)?
              BContensDat::New((char *)(fieldNames[k]).c_str(),*((BDat*)(currentIndex[k])),(char *)(fieldNames[k]).c_str()):
              BContensDat::New(*((BDat*)(currentIndex[k])));

            break;
          }
        }
        LstFastAppend(lst2,aux2, data);
      }
    }

    //PREFIX FIELDS:
    //////////////////////////////////////////////////////////////////////////

    *tmpPrefix=NULL;

    for(; j<(indexNumber+prefixNumber); j++)
    {
      switch(types[j])
      {
      case TypeDBUnknown:
        {
          tmpPrefix->Concat(DBGetText(j).String());
          break;
        }
      case TypeDBText:
        {
          tmpPrefix->Concat(DBGetText(j).String());
          break;
        }
      case TypeDBDate:
        {
          tmpPrefix->Concat(DBGetDate(j).Dump());
          break;
        }
      case TypeDBReal:
        {
          tmpPrefix->Concat(DBGetReal(j).Dump());
          break;
        }
      }
    }
    //Check if it's a new prefix and re-arrange the Set and the control array:
    k=lastPrefix+1;
    while( (k<prefixNames.size()) &&(((BText)prefixNames[k]) < *tmpPrefix))
    {
      for(int ser=0;ser<serieNumber;ser++)
      {
        BSyntaxObject* elemTmp = NIL;
        elemTmp = BContensDat::New(defaultValue);
        //DESC(Will we ever enter here?):
        if(numRow==0)
        {
          if (invertName==0)
          {
             BText tmp=prefixNames[k];
             if (padString) tmp.Concat(padString);
             tmp.Concat((char *)(serieNames[ser]).c_str());
             elemTmp->PutDescription((char *)tmp.String());
          }
          else
          {
             BText tmp=(char *)(serieNames[ser]).c_str();
             if (padString) tmp.Concat(padString);
             tmp.Concat(prefixNames[k]);
             elemTmp->PutDescription((char *)tmp.String());
          }
        }
        LstFastAppend(lst2,aux2,elemTmp);
      }
      k++;
      lastPrefix++;
    }

    if ((k==prefixNames.size()) || (prefixNames[k] != *tmpPrefix))//Add column at the "k" pos:
    {
      prefixNames.resize((prefixNames.size() + 1));
      //vector<BText>::iterator iter = prefixNames.c_str();
      //for(int m = 0;m<k;m++) iter++;
      for(int l=((int)prefixNames.size()-1);l>k;l--){
        prefixNames[l] = prefixNames[l-1];
      }
      prefixNames[k]=tmpPrefix->String();

      //Re-arrange Table inserting a default value in "k" position for all lines:
      int ins = ((k*serieNumber)+indexNumber);
      //ins+=serieNumber;
      ins++;
      for(int x=1;x<=numRow;x++)//Re-arrange lines:
      {
        //BSet & lstAux =((BUserSet*)((LstNthCdr(lst,x))->Car()))->Contens();
        BSet & lstAux =Set(LstNthCdr(lst,x)->Car());
        BList* lstAux2 = lstAux.ToList();
        for(int ser=0;ser<serieNumber;ser++)
        {
          BSyntaxObject* elemTmp = NIL;
          elemTmp = BContensDat::New(defaultValue);

          if(x==1)
          {
            if (invertName == 0)
            {
              BText tmp=prefixNames[k];
              if (padString) tmp.Concat(padString);
              tmp.Concat((char *)(serieNames[ser]).c_str());
              elemTmp->PutDescription((char *)tmp.String());
            }
            else
            {
              BText tmp=(char *)(serieNames[ser]).c_str();
              if (padString) tmp.Concat(padString);
              tmp.Concat(prefixNames[k]);
              elemTmp->PutDescription((char *)tmp.String());
            }
          }

          lstAux2 = LstInsert(lstAux2, (BCore*)elemTmp, (ins+ser));
        }
        lstAux.RobElement(lstAux2);
      }
    }

    //DATA(SERIE) FIELDS:
    //////////////////////////////////////////////////////////////////////////

    //Insert the new values:
    for(int ser=0;ser<serieNumber;ser++)
    {
      BSyntaxObject* elemTmp = NIL;
      elemTmp = BContensDat::New(DBGetReal(j+ser));

      if(numRow==0)
      {
        if (invertName==0)
        {
          BText tmp = *tmpPrefix;
          if (padString) tmp.Concat(padString);
          tmp.Concat((char *)(serieNames[ser]).c_str());
          elemTmp->PutDescription((char *)(tmp.String()));
        }
        else
        {
          BText tmp=(char *)(serieNames[ser]).c_str();
          if (padString) tmp.Concat(padString);
          tmp.Concat(prefixNames[k]);
          elemTmp->PutDescription((char *)tmp.String());
        }
      }

      LstFastAppend(lst2,aux2,elemTmp);
    }
    lastPrefix++;
    i++;
  }
  while(dbGetNext());

  //Last row:
  for(k=(lastPrefix+1);k<prefixNames.size();k++)
  {
    for(int ser=0;ser<serieNumber;ser++)
    {
      BSyntaxObject* elemTmp = NIL;
      elemTmp = BContensDat::New(defaultValue);
      LstFastAppend(lst2,aux2,elemTmp);
    }
  }
  BSet set;
  set.RobStruct(lst2,str,BSet::Structured);
  BUserSet * row = BContensSet::New(set);
//row->PutDescription( txtRow + (int)numRow );
  LstFastAppend(lst,aux,row);

  //CLEAN UP:
  delete (tmpPrefix);
  for(i=0;i<indexNumber;i++)
  {
    switch(types[i])
    {
    case TypeDBUnknown :
    case TypeDBText :
    {
      delete ((BText*)lastIndex[i]);
      delete ((BText*)currentIndex[i]);
      break;
    }
    case TypeDBDate :
     {
       delete ((BDate*)lastIndex[i]);
       delete ((BDate*)currentIndex[i]);
       break;
     }
    case TypeDBReal :
    {
      delete ((BDat*)lastIndex[i]);
      delete ((BDat*)currentIndex[i]);
      break;
    }
    }
  }

  contens_.RobElement(lst);
  contens_.PutSubType( BSet::Table );
  /* put names to the elements of the first row */
  BSyntaxObject * so;
  BSet * row1;
  
  so = contens_[1];
  if (so->Grammar() == GraSet()) {
    //row1 = &(((BUserSet*)so)->Contens());
    row1 = &Set(so);
    for (k = 0; k < prefixNames.size(); ++k) {
      if (k+indexNumber+1 <= row1->Card()) {
        (*row1)[k+indexNumber+1]->PutName(prefixNames[k]);
      }
    }
    
  } else {
    printf("Firts element of TableColumn must be a set");
  }
  dbCloseQuery();
}

