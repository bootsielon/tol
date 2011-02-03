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

#include <contrib/cint/tolCint.h>

#include <tol/tol_bdir.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bsetgra.h>
#include <ltdl.h>

static int  lt_dlinit_ =  -1;

typedef void      (*G__scratch_all_t)        (void);
typedef char*     (*G__lasterror_filename_t) (void);
typedef int       (*G__lasterror_linenum_t)  (void);
typedef int       (*G__init_cint_t)          (G__CONST char* command);
typedef void      (*G__set_errmsgcallback_t) (void* p);
typedef void      (*G__setautoconsole_t)     (int autoconsole);
typedef void      (*G__SetCINTSYSDIR_t)      (char* cintsysdir);
typedef int       (*G__loadfile_t)           (G__CONST char* filename);
typedef int       (*G__unloadfile_t)         (G__CONST char* filename);
typedef G__value  (*G__exec_tempfile_t)      (G__CONST char *file);
typedef G__value  (*G__exec_text_t)          (G__CONST char *unnamedmacro);
typedef long      (*G__int_t)                (G__value buf);
typedef double    (*G__double_t)             (G__value buf);
typedef G__value  (*G__calc_t)               (G__CONST char *expr);
typedef int       (*G__deletevariable_t)     (G__CONST char* varname);

static G__scratch_all_t         G__scratch_all_ptr          = NULL;
static G__lasterror_filename_t  G__lasterror_filename_ptr   = NULL;
static G__lasterror_linenum_t   G__lasterror_linenum_ptr    = NULL;
static G__init_cint_t           G__init_cint_ptr            = NULL;
static G__set_errmsgcallback_t  G__set_errmsgcallback_ptr   = NULL;
static G__setautoconsole_t      G__setautoconsole_ptr       = NULL;
static G__SetCINTSYSDIR_t       G__SetCINTSYSDIR_ptr        = NULL;
static G__loadfile_t            G__loadfile_ptr             = NULL;
static G__unloadfile_t          G__unloadfile_ptr           = NULL;
static G__exec_tempfile_t       G__exec_tempfile_ptr        = NULL;
static G__exec_text_t           G__exec_text_ptr            = NULL;
static G__int_t                 G__int_ptr                  = NULL;
static G__double_t              G__double_ptr               = NULL;
static G__calc_t                G__calc_ptr                 = NULL;
static G__deletevariable_t      G__deletevariable_ptr       = NULL;

//--------------------------------------------------------------------
int Cint_dynamic_linkage (void)
//--------------------------------------------------------------------
{
  if(lt_dlinit_) 
  { lt_dlinit_ = lt_dlinit(); }
  BText libraryName = "libcint";  
  BText libraryPath = "";
  #ifdef UNIX
    libraryPath = libraryName+".so";
  #else
    libraryPath = libraryName+".dll";
  #endif 

  lt_dlhandle handleLib=0;

  // abro la DynLib
  handleLib = lt_dlopen( libraryPath );
  BText reason( lt_dlerror( ) );
  if ( !handleLib ) 
  {
  //Std(BText("\nTRACE BLoadDynLib::Evaluator 3 lt_dlerror='")+lt_dlerror()+"'");
    Warning(BText("[Cint_dynamic_linkage(\"")+libraryPath+"\")]"+" \n"
    "  lt_dlopen error:'"+reason+"'");
    return(false);
  } 
  G__init_cint_ptr = reinterpret_cast<G__init_cint_t>( lt_dlsym( handleLib, "G__init_cint" ) );
  if(!G__init_cint_ptr)
  {
    Warning(BText("[Cint_dynamic_linkage(\"")+libraryPath+"\")]"+" \n"
    "  lt_dlopen error:'"+reason+"'");
    return(false);     
  }
  G__scratch_all_ptr = reinterpret_cast<G__scratch_all_t>( lt_dlsym( handleLib, "G__scratch_all" ) );
  G__lasterror_filename_ptr = reinterpret_cast<G__lasterror_filename_t>( lt_dlsym( handleLib, "G__lasterror_filename" ) );
  G__lasterror_linenum_ptr = reinterpret_cast<G__lasterror_linenum_t>( lt_dlsym( handleLib, "G__lasterror_linenum" ) );
  G__set_errmsgcallback_ptr = reinterpret_cast<G__set_errmsgcallback_t>( lt_dlsym( handleLib, "G__set_errmsgcallback" ) );
  G__setautoconsole_ptr = reinterpret_cast<G__setautoconsole_t>( lt_dlsym( handleLib, "G__setautoconsole" ) );
  G__SetCINTSYSDIR_ptr = reinterpret_cast<G__SetCINTSYSDIR_t>( lt_dlsym( handleLib, "G__SetCINTSYSDIR" ) );
  G__loadfile_ptr = reinterpret_cast<G__loadfile_t>( lt_dlsym( handleLib, "G__loadfile" ) );
  G__unloadfile_ptr = reinterpret_cast<G__unloadfile_t>( lt_dlsym( handleLib, "G__unloadfile" ) );
  G__exec_tempfile_ptr = reinterpret_cast<G__exec_tempfile_t>( lt_dlsym( handleLib, "G__exec_tempfile" ) );
  G__exec_text_ptr = reinterpret_cast<G__exec_text_t>( lt_dlsym( handleLib, "G__exec_text" ) );
  G__int_ptr = reinterpret_cast<G__int_t>( lt_dlsym( handleLib, "G__int" ) );
  G__double_ptr = reinterpret_cast<G__double_t>( lt_dlsym( handleLib, "G__double" ) );
  G__calc_ptr = reinterpret_cast<G__calc_t>( lt_dlsym( handleLib, "G__calc" ) );
  G__deletevariable_ptr = reinterpret_cast<G__deletevariable_t>( lt_dlsym( handleLib, "G__deletevariable" ) );
  return(true);
}

//--------------------------------------------------------------------
void Cint_not_linked_message (const char* function)
//--------------------------------------------------------------------
{
  Error(BText("Cannot use CINT function ")+function+
  " due to libcint.dll is not linked.");
}

//--------------------------------------------------------------------
void Cint_scratch_all (void)
//--------------------------------------------------------------------
{
  if(!G__scratch_all_ptr) 
  { 
    Cint_not_linked_message("Cint_scratch_all"); 
    return; 
  } 
  (*G__scratch_all_ptr)();
}

//--------------------------------------------------------------------
char* Cint_lasterror_filename (void)
//--------------------------------------------------------------------
{
  if(!G__lasterror_filename_ptr) 
  { 
    Cint_not_linked_message("Cint_lasterror_filename"); 
    return(NULL); 
  } 
  return((*G__lasterror_filename_ptr)());
}

//--------------------------------------------------------------------
int Cint_lasterror_linenum (void)
//--------------------------------------------------------------------
{
  if(!G__lasterror_linenum_ptr) 
  { 
    Cint_not_linked_message("Cint_lasterror_linenum"); 
    return(0); 
  } 
  return((*G__lasterror_linenum_ptr)());
}

//--------------------------------------------------------------------
int Cint_init_cint (G__CONST char* command)
//--------------------------------------------------------------------
{
  if(!G__init_cint_ptr) 
  { 
    Cint_not_linked_message("Cint_init_cint"); 
    return(-1); 
  } 
  return((*G__init_cint_ptr)(command));
}

//--------------------------------------------------------------------
void Cint_set_errmsgcallback (void* p)
//--------------------------------------------------------------------
{
  if(!G__set_errmsgcallback_ptr) 
  { 
    Cint_not_linked_message("Cint_set_errmsgcallback"); 
    return; 
  } 
  (*G__set_errmsgcallback_ptr) (p);
}

//--------------------------------------------------------------------
void Cint_setautoconsole (int autoconsole)
//--------------------------------------------------------------------
{
  if(!G__setautoconsole_ptr)
  { 
    Cint_not_linked_message("Cint_setautoconsole"); 
    return; 
  } 
  (*G__setautoconsole_ptr) (autoconsole);
}

//--------------------------------------------------------------------
void Cint_SetCINTSYSDIR (char* cintsysdir)
//--------------------------------------------------------------------
{
  if(!G__SetCINTSYSDIR_ptr)
  { 
    Cint_not_linked_message("Cint_SetCINTSYSDIR"); 
    return; 
  } 
  (*G__SetCINTSYSDIR_ptr) (cintsysdir);
}

//--------------------------------------------------------------------
int Cint_loadfile (G__CONST char* filename)
//--------------------------------------------------------------------
{
  if(!G__loadfile_ptr)
  { 
    Cint_not_linked_message("Cint_loadfile"); 
    return(0); 
  } 
  return((*G__loadfile_ptr) (filename));
}

//--------------------------------------------------------------------
int Cint_unloadfile (G__CONST char* filename)
//--------------------------------------------------------------------
{
  if(!G__unloadfile_ptr)
  { 
    Cint_not_linked_message("Cint_unloadfile"); 
    return(0); 
  } 
  return((*G__unloadfile_ptr) (filename));
}

//--------------------------------------------------------------------
G__value Cint_exec_tempfile (G__CONST char *file)
//--------------------------------------------------------------------
{
  static G__value gv;
  if(!G__exec_tempfile_ptr)
  { 
    Cint_not_linked_message("Cint_exec_tempfile"); 
    return(gv); 
  } 
  return((*G__exec_tempfile_ptr) (file));
}

//--------------------------------------------------------------------
G__value Cint_exec_text (G__CONST char *unnamedmacro)
//--------------------------------------------------------------------
{
  static G__value gv;
  if(!G__exec_text_ptr)
  { 
    Cint_not_linked_message("Cint_exec_text"); 
    return(gv); 
  } 
  return((*G__exec_text_ptr) (unnamedmacro));
}

//--------------------------------------------------------------------
long Cint_int (G__value buf)
//--------------------------------------------------------------------
{
  if(!G__int_ptr)
  { 
    Cint_not_linked_message("Cint_int"); 
    return(0); 
  } 
  return((*G__int_ptr) (buf));
}

//--------------------------------------------------------------------
double Cint_double (G__value buf)
//--------------------------------------------------------------------
{
  if(!G__double_ptr)
  { 
    Cint_not_linked_message("Cint_double"); 
    return(BDat::Unknown().Value()); 
  } 
  return((*G__double_ptr) (buf));
}

//--------------------------------------------------------------------
G__value Cint_calc (G__CONST char *expr)
//--------------------------------------------------------------------
{
  static G__value gv;
  if(!G__calc_ptr)
  { 
    Cint_not_linked_message("Cint_calc"); 
    return(gv ); 
  } 
  return((*G__calc_ptr) (expr));
}

//--------------------------------------------------------------------
int Cint_deletevariable (G__CONST char* varname)
//--------------------------------------------------------------------
{
  if(!G__deletevariable_ptr)
  { 
    Cint_not_linked_message("Cint_deletevariable"); 
    return(0); 
  } 
  return((*G__deletevariable_ptr) (varname));
}

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
  BText filename = Cint_lasterror_filename();
  BText linenum = Cint_lasterror_linenum();
  BText locInfo = "";
  if(filename.HasName()) 
  { 
    locInfo = BText("\nIn file ")+filename+" line "+linenum;
  }
  Error(BText("\n[Cint] ")+locInfo+msg);
}

//--------------------------------------------------------------------
int& Cint_is_initialized()
//--------------------------------------------------------------------
{
  static int _Cint__initialized_ = false;
  return(_Cint__initialized_);
}

//--------------------------------------------------------------------
int Cint_initialize(const BText& filePath)
//--------------------------------------------------------------------
{
  if(Cint_is_initialized())
  {
    TRACE_CINT_DECIMAL;
    return(G__INIT_CINT_SUCCESS);
  }
  else
  {
    Cint_is_initialized() = true; 
    int linked = Cint_dynamic_linkage();
    if(!linked) { return(false); }
    BText order = "cint";
    if(filePath.HasName()) { order += BText(" ")+filePath; }
    int G__init_cint_ = Cint_init_cint(order);
    switch(G__init_cint_) {
    case G__INIT_CINT_SUCCESS_MAIN:
      Error("Cint called out of main should never happen G__INIT_CINT_SUCCESS_MAIN.\n");
      /* Should never happen */
      break;
    case G__INIT_CINT_SUCCESS:
    //Std("\nCint has been succesfully initialized.\n");
      break;
    case G__INIT_CINT_FAILURE:
    default:
      Error("Cint initialization failed G__INIT_CINT_FAILURE.\n");
    }
    Cint_set_errmsgcallback(reinterpret_cast<void*>(Cint_errmsgcallback));
    Cint_setautoconsole(1);
    TRACE_CINT_DECIMAL;
    return(G__init_cint_);
  }
}

//--------------------------------------------------------------------
int Cint_scratch_all_safe()
//--------------------------------------------------------------------
{
  if(Cint_is_initialized())
  {
    return(false);
  }
  else
  {
    Cint_is_initialized() = false;
    Cint_scratch_all();
    return(true);
  }
}

//--------------------------------------------------------------------
int Cint_initialize_tol_variables()
//--------------------------------------------------------------------
{
  int linked = Cint_dynamic_linkage();
  BSystemDat* linked_ = new BSystemDat("Cint_IsLinked", linked,
	  I2("Is true if optional libcint library has been linked succesfully.",
	     "Es cierto si la librería opcional libcint ha sido enlazada correctamente"));
  return(linked);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCint_SetCINTSYSDIR);
  DefExtOpr(1, BDatCint_SetCINTSYSDIR, "Cint.SetCINTSYSDIR", 1, 1, "Text",
  "(Text CINTSYSDIR)",
  I2("Set the value of the internal variable CINTSYSDIR.",
     "Inicializa el valor de la variable interna CINTSYSDIR.")+
     cint_url_link(),
     BOperClassify::Conversion_);
  void BDatCint_SetCINTSYSDIR::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.SetCINTSYSDIR")) { return; }
  BText& dirPath = Text(Arg(1));
  bool ok = G__SetCINTSYSDIR_ptr!=NULL;

  if(!BDir::CheckIsDir(dirPath))
  {
    Error(BText("[Cint.SetCINTSYSDIR]")+" Not found path "+dirPath);
    ok = false;
  }
  contens_ = ok;
  if(ok)
  {
    Cint_SetCINTSYSDIR(dirPath);
  }
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCint_initialize);
  DefExtOpr(1, BDatCint_initialize, "Cint.initialize", 1, 1, "Text",
  "(Text filePath)",
  I2("Initializes the CINT environment with an optional C/C++ file.",
     "Inicilaliza el entorno de CINT con un fichero C/C++ opcional.")+
     cint_url_link(),
     BOperClassify::Conversion_);
  void BDatCint_initialize::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.initialize")) { return; }
  BText& filePath = Text(Arg(1));
  contens_ = Cint_initialize(filePath);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCint_scratch_all);
  DefExtOpr(1, BDatCint_scratch_all, "Cint.scratch_all", 1, 1, "Real",
  "(Real unused)",
  I2("Terminates the interpreter and frees heap memory used by it."
     "The interpreter should not be used after call this function."
     "To resume the interpreter, user needs to call function Cint.initialize.",
     "Termina el intérprete y libera memoria heap usada por él."
     "El intérprete no debe utilizarse después de llamar a esta función."
     "Para continuar con el intérprete, el usuario debe llamar a la "
     "función Cint.initialize.")+
     cint_url_link(),
     BOperClassify::Conversion_);
  void BDatCint_scratch_all::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.scratch_all")) { return; }
  contens_ = Cint_scratch_all_safe();
}


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
  contens_ = Cint_loadfile(filePath.String());
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
  contens_ = Cint_unloadfile(filePath.String());
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_exec_tempfile);
  DefExtOpr(1, BDatCINT_exec_tempfile, "Cint.exec_tempfile", 1, 1, "Text",
  "(Text filePath)",
  I2("Opens temporary source file and execute it in the current context. "
     "The tempfile can contain variable declaration, struct, union, "
     "enum definitions, conditional expressions, loop and control "
     "statements but cannot contain function nor class definitions, "
     "wich can be loaded just calling Cint.loadfile",
     "Abre un archivo temporal de código fuente, que se ejecuta en el "
     "contexto actual. El archivo temporal puede contener declaración "
     "de variables, definiones de struct, union, enum, expresiones "
     "condicionales, órdenes de control y bucles, pero no puede "
     "contener definiciones de función ni de clase, las cuales "
     "sólo pueden cargarse con Cint.loadfile"),
     BOperClassify::Conversion_);
  void BDatCINT_exec_tempfile::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.exec_tempfile")) { return; }
  BText& filePath = Text(Arg(1));
  Cint_exec_tempfile(filePath.String());
  contens_ = true;
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_exec_text);
  DefExtOpr(1, BDatCINT_exec_text, "Cint.exec_text", 1, 1, "Text",
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
  void BDatCINT_exec_text::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.exec_text")) { return; }
  BText& expression = Text(Arg(1));
  Cint_exec_text(expression);
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
  contens_ = Cint_double(Cint_calc(expression));
/*
  G__value g = Cint_calc(expression);
  switch (g.type) 
  {
    case 100: contens_ = g.obj.d; break;
    case 105: contens_ = g.obj.i; break;
    default: Error(BText("[Cint.calc] ")+
     I2("Unknown CINT numerical type ",
                      "Tipo CINT numérico desconocido ")<<g.type);
  }
*/
}

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetCINT_import_from_tol);
  DefExtOpr(1, BSetCINT_import_from_tol, 
  "Cint.import_from_tol", 1, 4, "Set Text Text Text",
  "(Set vars [, Text prefix=\"\", Text suffix=\"\", Text namespace=\"\"])",
  I2("Imports TOL variables, global or local, into CINT global scope.\n"
     "Returns the names of imported CINT variables.\n"
     "TOL variables must have a valid name in TOL and C languages.\n"
     "At this moment these are the available TOL types :\n",    
     "Importa variables TOL, globales o locales, al ámbito global de CINT\n"
     "Devuelve los nombres de las variables CINT correctamente importadas.\n" 
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
     "stdlib/general/TolCint/tol_cint_casting.cpp\n"+
  I2("Imported variables can be modified by CINT routines but memory "
     "allocation and the internal structure cannot be altered. "
     "For example, you can change the elements of a Matrix but cannot "
     "change the number of cells, rows nor columns.\n",
     "Las variables importadas pueden ser modificadas por rutinas CINT "
     "pero el alojo de memoria no puede ser alterado. "
     "Por ejemplo, es posible cambiar los elementos de una matriz pero "
     "no el número de celdas, filas o columnas.\n")+
  I2("If argument prefix is not empty then it will added before each TOL name to "
     "create transformed CINT names.\n",
     "Si el argumento prefix no es vacío se añadirá por delante de cada nombre TOL "
     "para crear los nombres CINT transformados.\n")+
  I2("If argument suffix is not empty then it will added after each TOL name to "
     "create transformed CINT names.\n",
     "Si el argumento suffix no es vacío se añadirá por detrás de cada nombre TOL "
     "para crear los nombres CINT transformados.\n") +
  I2("If argument namespace is not empty then CINT variables will be created in "
     "in the speciefied and pre-existant namespace instead of global scope.\n",
     "Si el argumento namespace no es vacío se crearán las variables dentro "
     "de dicho namespace preexistente, en lugar de en el ámbito global.\n")+
    cint_url_link(),
  BOperClassify::Conversion_);
  void BSetCINT_import_from_tol::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.import_from_tol")) { return; }
  
  BSyntaxObject* var = NULL;
  BGrammar* gra = NULL;
  int numArg; 
  BSet& arg = Set(Arg(1));
  BText prefix_ = "";
  BText sufix_ = "";
  BText namespace_ = "";
  if(Arg(2)) { prefix_ = Text(Arg(2)); }
  if(Arg(3)) { sufix_ = Text(Arg(3)); }
  if(Arg(4)) { namespace_ = Text(Arg(4)); }
  BText expression = "";
  if(namespace_.HasName())
  {
    expression = BText("namespace ") + namespace_ + " {\n";
  };
  int c = arg.Card();
  contens_.PrepareStore(c);
  for(numArg = 1; numArg<=c; numArg++)
  {
    var = arg[numArg];
    if(!var) { continue; }
    gra = var->Grammar();
    BText pns = prefix_+var->Name()+sufix_;
    if(gra==GraReal())
    {
      BDat& dat = Dat(var);
      double* ptr = (double*)(&dat);
      int ptrNum = (int)ptr;
      expression += BText("double& ")+pns+" = *((double *)("<<ptrNum<<"));\n";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      contens_.AddElement(new BContensText("",pns,""));
    }
    else if(gra==GraMatrix())
    {
      BMat& mat = Mat(var);
      double* ptr = (double*)(mat.GetData().GetBuffer());
      int ptrNum = (int)ptr;
      expression += BText("Matrix ")+pns+" ( "<<
        mat.Rows()<<","<<
        mat.Columns()<<","<<
        " ((double *)("<<ptrNum<<")) );\n";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      contens_.AddElement(new BContensText("",pns,""));
    }
    else if(gra==GraText())
    {
      BText& txt = Text(var);
      char* ptr = (char*)(txt.Buffer());
      int ptrNum = (int)ptr;
      expression += BText("Text ")+pns+" ( "<<
        txt.Length()<<","<<
        " ((char *)("<<ptrNum<<")) );\n";
    //Std(BText("TRACE [Cint.import_from_tol] ")+expression);
      contens_.AddElement(new BContensText("",pns,""));
    }
    else 
    {
      Warning(BText("[Cint.import_from_tol] ")+
        I2("Cannot import TOL variable ",
           "No se puede importar la variable TOL ")<<
           gra->Name()+" "+var->Name());
    }
  }
  if(namespace_.HasName())
  {
    expression += "\n};";
  };
//Std(BText("\nTRACE [Cint.import_from_tol] expression=[\n")+expression+"\n]\n");
  Cint_exec_text(expression);
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCINT_delete_variables);
  DefExtOpr(1, BDatCINT_delete_variables, 
  "Cint.delete_variables", 1, 3, 
  "Set Text Text",
  "(Set varNames [, Text prefix=\"\", Text suffix=\"\"] )",
  I2("Deletes  from CINT global scope the variables which names are in "
     "specified set varNames.\n"
     "Returns the number of deleted variables.\n",    
     "Borra del ámbito global de CINT las variables cuyos nombres se "
     "especifican en el conjunto varNames\n"
     "Devuelve el número total de variables realmente borradas.\n")+
  I2("If argument prefix is not empty then it will added before each TOL name to "
     "create transformed CINT names.\n",
     "Si el argumento prefix no es vacío se añadirá por delante de cada nombre TOL "
     "para crear los nombres CINT transformados.\n")+
  I2("If argument suffix is not empty then it will added after each TOL name to "
     "create transformed CINT names.\n",
     "Si el argumento suffix no es vacío se añadirá por detrás de cada nombre TOL "
     "para crear los nombres CINT transformados.\n") +
     cint_url_link(),
  BOperClassify::Conversion_);
  void BDatCINT_delete_variables::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Cint.delete_variables")) { return; }
  
  BSyntaxObject* var = NULL;
  BGrammar* gra = NULL;
  int numArg; 
  contens_ = 0;
  BSet& arg = Set(Arg(1));
  BText prefix_ = "";
  BText sufix_ = "";
  if(Arg(2)) { prefix_ = Text(Arg(2)); }
  if(Arg(3)) { sufix_ = Text(Arg(3)); }

  for(numArg = 1; numArg<=arg.Card(); numArg++)
  {
    var = arg[numArg];
    if(!var) { continue; }
    gra = var->Grammar();
    if(gra==GraText())
    {
      BText pns = prefix_+var->Name()+sufix_;
      contens_ += Cint_deletevariable(pns);
    }
  }
}

