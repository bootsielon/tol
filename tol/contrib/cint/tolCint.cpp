/* tolCint.cpp: API for CINT, a interpreter to C and C++.

   http://root.cern.ch/drupal/content/cint
   http://root.cern.ch/viewvc/trunk/cint/doc/ref.txt

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
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>

//#pragma pack(16)
//#include <Api.h>
#include <G__ci.h>
//#pragma pack(4)

/*

G__scratch_all()

*/

//--------------------------------------------------------------------
 BText cint_url_link()
//--------------------------------------------------------------------
{
  return(I2(  
  "\nSee more about the C/C++ interpreter CINT at ",
  "\nMás detalles acerca del intérprete CINT para C/C++ en ")+
  "\n  http://root.cern.ch/drupal/content/cint\n"
  "\n  http://root.cern.ch/viewvc/trunk/cint/doc/ref.txt\n");
}


//--------------------------------------------------------------------
void Cint_errmsgcallback(char *msg)
//--------------------------------------------------------------------
{
  BText filename = G__lasterror_filename();
  BText linenum = G__lasterror_linenum();
  BText locInfo = "";
  if(filename.HasName()) 
  { 
    locInfo = BText("\nIn file ")+filename+" line "+linenum;
  }
  Error(BText("\n[Cint] ")+locInfo+msg);
}


//--------------------------------------------------------------------
int Cint_initialize()
//--------------------------------------------------------------------
{
  int G__init_cint_ = G__init_cint("cint");
  G__set_errmsgcallback(Cint_errmsgcallback);
  G__setautoconsole(1);
  return(G__init_cint_);
}

//--------------------------------------------------------------------
static int G__init_cint__ = Cint_initialize();
//--------------------------------------------------------------------


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_loadfile);
  DefExtOpr(1, BDatCINT_loadfile, "Cint.loadfile", 1, 1, "Text",
  "(Text filePath)",
  I2("Loads a C/C++ file to global scope in order to be used in TOL " 
     "by mean of CINT API functions.",
     "Carga un fichero C/C++ en el ámbito global para ser usado en "
     "TOL mediante las funciones de la API de CINT.")+
     cint_url_link(),
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
     cint_url_link(),
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
  DefExtOpr(1, BDatCINT_exec_tempfile, "Cint.exec_text", 1, 1, "Text",
  "(Text expression)",
    I2("Executes any C/C++ expression or set of expressions.\n"
     "Functions loaded with Cint.loadfile can be callable.\n"
     "Unlike Cint.calc API, Cint.exec_text can evaluate declaration and "
     "conditional statement. However, because Cint.exec_text uses temporary "
     "file, execution can be slower than Cint.calc\n",
     "Ejecuta una expresión C/C++ o grupo de expresiones.\n"
     "Se pueden usar las funciones previamente cargadas con Cint.loadfile\n"
     "Al contrario que con Cint.calc sí se pueden declarar variables y "
     "funciones y usar ciclos o sentencias condicionales, pero debido a "
     "que internamente se usa un fichero temporal en disco puede resultar "
     "más lento en ejecución.")+
     cint_url_link(),
     BOperClassify::Conversion_);
  void BDatCINT_exec_tempfile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.exec_text")) { return; }
  BText& expression = Text(Arg(1));
  G__exec_text(expression);
  contens_ = true;
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
     cint_url_link(),
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



//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_import_from_tol);
  DefExtOpr(1, BDatCINT_import_from_tol, 
  "Cint.import_from_tol", 1, 0, "Anything Anything",
  "(Anything var1 [, Anything var2, ...])",
  I2("Imports TOL variables, global or local, into CINT global scope.\n"
     "Returns the number of imported variables\n"
     "TOL variables must have a valid name in TOL and C languages.\n"
     "At this moment these are the available TOL types :\n",    
     "Importa variables TOL, globales o locales, al ámbito global de CINT\n"
     "Devuelve el número total de variables correctamen te importadas.\n" 
     "Las variables TOL deben tener un nombre válido en los lenguajes "
     "TOL y C\n"
     "Los tipos TOL admitidos para la importación son los siguientes:\n")+
     "  TOL       CINT\n"
     "  Real      double\n"
     "  Matrix    class Matrix\n"
     "  Text      class Text\n"+
  I2("You can view the declaration of corresponding C/C++ types at ",    
     "Puede ver la declaración de los correspondientes tipos C/C++ en ")+
     "\nhttps://www.tol-project.org/browser/tolp/trunk/tol/"
     "stdlib/general/TolCint/tol_cint_casting.cpp"+
  I2("Imported variables can be modified by CINT routines but memory "
     "allocation and the internal structure cannot be altered. "
     "For example, you can change the elements of a Matrix but cannot "
     "change the number of cells, rows nor columns.",
     "Las variables importadas pueden ser modificadas por rutinas CINT "
     "pero el alojo de memoria no puede ser alterado. "
     "Por ejemplo, es posible cambiar los elementos de una matriz pero "
     "no el número de celdas, filas o columnas")+
    cint_url_link(),
  BOperClassify::Conversion_);
  void BDatCINT_import_from_tol::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.import_from_tol")) { return; }
  
  BSyntaxObject* var = NULL;
  int numArg; 
  contens_ = 0;
  
  for(numArg = 1; var=Arg(numArg); numArg++)
  {
    BGrammar* gra = var->Grammar();
    if(gra==GraReal())
    {
      BDat& dat = Dat(var);
      double* ptr = (double*)(&dat);
      int ptrNum = (int)ptr;
      BText expression = BText("double& ")+var->Name()+" = *((double *)("<<ptrNum<<"))";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      G__exec_text(expression);
      contens_ += 1;
    }
    else if(gra==GraMatrix())
    {
      BMat& mat = Mat(var);
      double* ptr = (double*)(mat.GetData().GetBuffer());
      int ptrNum = (int)ptr;
      BText expression = BText("Matrix ")+var->Name()+" ( "<<
        mat.Rows()<<","<<
        mat.Columns()<<","<<
        " ((double *)("<<ptrNum<<")) ); 0;";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      G__exec_text(expression);
      contens_ += 1;
    }
    else if(gra==GraText())
    {
      BText& txt = Text(var);
      char* ptr = (char*)(txt.Buffer());
      int ptrNum = (int)ptr;
      BText expression = BText("Text ")+var->Name()+" ( "<<
        txt.Length()<<","<<
        " ((char *)("<<ptrNum<<")) ); 0;";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      G__exec_text(expression);
      contens_ += 1;
    }
    else 
    {
      Warning(BText("[Cint.import_from_tol] ")+
        "Cannot import TOL variables of type "<<gra->Name());
    }
  }
}

