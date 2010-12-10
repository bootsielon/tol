/* tolCint.cpp: API for CINT, a interpreter to C and C++.

   http://root.cern.ch/drupal/content/cint

   Copyright (C) 2010 - Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>

//#pragma pack(16)
//#include <Api.h>
#include <G__ci.h>
//#pragma pack(4)

/*

extern G__EXPORT G__value G__calc G__P((G__CONST char *expr));
extern G__EXPORT int  G__loadfile G__P((G__CONST char* filename));
extern G__EXPORT int  G__unloadfile G__P((G__CONST char* filename));
extern G__EXPORT G__value G__exec_tempfile G__P((G__CONST char *file));

*/

static int G__init_cint__ = G__init_cint("cint");

static BText cint_url_link = I2( 
  "\nSee more about the C/C++ interpreter CINT at ",
  "\nMás detalles acerca del intérprete CINT para C/C++ en ")+
  "\n  http://root.cern.ch/drupal/content/cint\n";

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_loadfile);
  DefExtOpr(1, BDatCINT_loadfile, "Cint.loadfile", 1, 1, "Text",
  "(Text filePath)",
  I2("Loads a C/C++ file to global scope in order to be used in TOL " 
     "by mean of CINT API functions.",
     "Carga un fichero C/C++ en el ámbito global para ser usado en "
     "TOL mediante las funciones de la API de CINT.")+
     cint_url_link,
     BOperClassify::Conversion_);
  void BDatCINT_loadfile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.loadfile")) { return; }
  BText& filePath = Text(Arg(1));
  contens_ = G__loadfile(filePath.String());
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_unloadfile);
  DefExtOpr(1, BDatCINT_unloadfile, "Cint.unloadfile", 1, 1, "Text",
  "(Text filePath)",
  I2("Unloads a C/C++ file previously loaded by Cint.loadfile.",
     "Descarga un fichero C/C++ previamente cargado con Cint.loadfile")+
     cint_url_link,
     BOperClassify::Conversion_);
  void BDatCINT_unloadfile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.unloadfile")) { return; }
  BText& filePath = Text(Arg(1));
  contens_ = G__unloadfile(filePath.String());
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_exec_tempfile);
  DefExtOpr(1, BDatCINT_exec_tempfile, "Cint.exec_tempfile", 1, 1, "Text",
  "(Text filePath)",
  I2(".",
     "."),
     BOperClassify::Conversion_);
  void BDatCINT_exec_tempfile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.exec_tempfile")) { return; }
  BText& filePath = Text(Arg(1));
  const char* str = filePath.String();
  G__value g = G__exec_tempfile(str);
  switch (g.type) 
  {
    case 100: contens_ = g.obj.d; break;
    case 105: contens_ = g.obj.i; break;
    default: Error(I2("Unknown CINT type ",
                      "Tipo CINT desconocido ")<<g.type);
  }
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_calc);
  DefExtOpr(1, BDatCINT_calc, "Cint.calc", 1, 1, "Text",
  "(Text expression)",
  I2("Executes a simple C/C++ expression returning an integer or double "
     "value.\n"
     "Functions loaded with Cint.loadfile can be callable.\n"
     "Declaration, loop  and  conditional statement can not be used in "
     "the expression.\n",
     "Ejecuta una expresión simple C/C++ que devuelve un resultado de tipo "
     "integer o double.\n"
     "Se pueden usar las funciones previamente cargadas con Cint.loadfile\n"
     "No se pueden declarar variables ni funciones ni hacer ciclos ni "
     "sentencias condicionales.")+
     cint_url_link,
     BOperClassify::Conversion_);
  void BDatCINT_calc::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.calc")) { return; }
  BText& expression = Text(Arg(1));
  G__value g = G__calc(expression.String());
  switch (g.type) 
  {
    case 100: contens_ = g.obj.d; break;
    case 105: contens_ = g.obj.i; break;
    default: Error(I2("Unknown CINT type ",
                      "Tipo CINT desconocido ")<<g.type);
  }
}

