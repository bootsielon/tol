/* init.cpp: Kernel initialization.
             GNU/TOL language.

   Copyright (C) 2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_version_def.h> 

#include <tol/tol_init.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bparser.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bmoninfo.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bvmat_bsr.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#ifdef __USE_TC__
#  include <tol/tol_bctmigra.h>
#  include <tol/tol_bctmsgra.h>
#  include <tol/tol_bctsrgra.h>
#endif
#include <tol/tol_baia.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_btmsgrav.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bmatimp.h>
#include <tol/tol_delay_init.h>
#include <tol/tol_bmethodhook.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_ois.h>
#include <tol/tol_gsl.h>
#include <malloc.h>
#include <locale.h>
#include <signal.h>

#include <tol/tol_bnameblock.h>

#ifdef _DEBUG
static int igtStep_=0;
#define TrcIG(msg)   Trace(BText("\n -Step ")+(++igtStep_)+": "+(msg));
#define TrcIGS(msg)  Trace(BText("[")+(msg)+"]");
#define TrcIGSL(msg) Trace(BText("\n   -")+(msg));
#else
#define TrcIG(msg)
#define TrcIGS(msg)
#define TrcIGSL(msg)
#endif

#if defined(_MSC_VER)
static errno_t reset_errno_ = _set_errno(0);
#else
static int reset_errno_ = (errno=0);
#endif

//--------------------------------------------------------------------
// static variables and initializations
//--------------------------------------------------------------------
BTraceInit("init.cpp");

//Initializes TOL sesion timer
static double initSession_ = BSys::SessionTime();

#if defined(_MSC_VER) && (_MSC_VER>=1300)
//Using special Small Block Heap (SBH) for allocations lower than this value
//or 0 to avoi SBH. It can affect efficience in some programs compiled
//with Visual C++ .NET
#define defaultSBH_ 0
#endif

//------------------------------
// DCDLIB API:
extern "C" int dcdflib_init();
static int dcdflib_inited = dcdflib_init();

//------------------------------
// GBLA API:
#if defined(__USE_GBLA__)
#  include <gbla/blapi.h>
#endif

//------------------------------
static bool           TOLHasBeenInitialized_      = false;
static bool           TOLHasBeenEnded_            = false;
static BSyntaxObject* initTol_                    = NULL;
static BText         _tolVersion_                 = "";
static BText         _tolCredits_                 = ""; 
static BText         _tolContribAcknowledgements_ = "";
static BText         _tolSessionPath_             = "";
static BSystemText*   tolSessionPath_             = NULL;



//--------------------------------------------------------------------
  TOL_API const char* TOLCppRoot() 
//Returns absolute root path of TOL C++ project
//--------------------------------------------------------------------
{
  static BText path_ = "";
  if(path_=="")
  {
    path_ = GetFilePath(__FILE__)+"../";
    path_ = GetAbsolutePath(path_);
    path_ = Replace(path_,'\\','/');
  };
  return(path_.String());
}


//--------------------------------------------------------------------
  TOL_API int TOLHasBeenInitialized() 
//--------------------------------------------------------------------
{
  return(TOLHasBeenInitialized_);
}

//--------------------------------------------------------------------
  TOL_API int TOLHasBeenEnded() 
//--------------------------------------------------------------------
{
  return(TOLHasBeenEnded_);
}

//--------------------------------------------------------------------
  TOL_API void SetTOLEnd() 
//--------------------------------------------------------------------
{
  if(!TOLHasBeenEnded_)
  {
    TOLHasBeenEnded_ = true;
    int BTolOprProfiler_Dump(); 
    BTolOprProfiler_Dump();
  }
}

//--------------------------------------------------------------------
  static void Error_UnexpectedTolEnd(void)
//--------------------------------------------------------------------
{
  Error(I2("An unexpected error has made TOL unstable and session must be closed",
           "Un error inesperado ha hecho TOL inestable y la sesión debe ser cerrada"));
}

//--------------------------------------------------------------------
  TOL_API void UnexpectedTolEnd(void)
//--------------------------------------------------------------------
{
  Error_UnexpectedTolEnd();
  SetTOLEnd();
}

//--------------------------------------------------------------------
static void signal_error_SIGABRT(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: Abnormal termination, such as instigated by the abort function. (Abort.)");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGFPE(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: Erroneous arithmetic operation, such as divide by 0 or overflow. (Floating point exception.)");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGILL(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: An ‘invalid object program’ has been detected. This usually means that there is an illegal instruction in the program. (Illegal instruction.)");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGINT(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: Interactive attention signal; on interactive systems this is usually generated by typing some ‘break-in’ key at the terminal. (Interrupt.)");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGSEGV(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: Invalid storage access; most frequently caused by attempting to store some value in an object pointed to by a bad pointer. (Segment violation.)");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGTERM(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: Termination request made to the program. (Terminate.) ");
  Error_UnexpectedTolEnd();
  exit(sig);
};
//--------------------------------------------------------------------
static void signal_error_SIGBREAK(int sig) 
//--------------------------------------------------------------------
{
  Error("SIGNAL: /* Ctrl-Break sequence */ ");
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_error_SIGUNK(int sig) 
//--------------------------------------------------------------------
{
  Error(BText("SIGNAL: Unknown signal description for code ")+sig);
  Error_UnexpectedTolEnd();
  exit(sig);
};

//--------------------------------------------------------------------
static void signal_assign()
//--------------------------------------------------------------------
{
//signal(       0, signal_error_SIGUNK);
  signal(       1, signal_error_SIGUNK);
  signal(SIGINT,   signal_error_SIGINT);
  signal(       3, signal_error_SIGUNK);
  signal(SIGILL,   signal_error_SIGILL);
//signal(       5, signal_error_SIGUNK);
  signal(       6, signal_error_SIGUNK);
//signal(       7, signal_error_SIGUNK);
  signal(SIGFPE,   signal_error_SIGFPE);
//signal(       9, signal_error_SIGUNK);
//signal(      10, signal_error_SIGUNK);
  signal(SIGSEGV,  signal_error_SIGSEGV);
//signal(      12, signal_error_SIGUNK);
  signal(      13, signal_error_SIGUNK);
//signal(      14, signal_error_SIGUNK);
  signal(SIGTERM,  signal_error_SIGTERM);
  signal(      16, signal_error_SIGUNK);
  signal(      17, signal_error_SIGUNK);
//signal(      18, signal_error_SIGUNK);
//signal(      19, signal_error_SIGUNK);
//signal(      20, signal_error_SIGUNK);
  signal(SIGBREAK, signal_error_SIGBREAK);
  signal(SIGABRT,  signal_error_SIGABRT);
}

//--------------------------------------------------------------------
  const char* TOLSessionPath() 
//--------------------------------------------------------------------
{
  return(_tolSessionPath_.String());
}

//--------------------------------------------------------------------
void InitCommonInstances(BGrammar* gra)
//--------------------------------------------------------------------
{
  assert(gra);
  BText name = gra->Name();
  BText typname = name;
  if(typname=="Ratio") typname+="n";
  new BEqualOperator(gra);
  new BUserFunctionCreator(gra);
  new BStructCreator("Struct", gra);
  if(gra==GraAnything()) { new BMethodView(gra); }
  new BExternalOperator ExtOprConstructor
  (
    BSetOfType, BText("SetOf")+typname, 1, 0, name+" "+name,
    BText("(") +name+ " var1 [, " + name + " var2, ...])",
    I2("Returns an enumerated set of objects of type ",
       "Devuelve un conjunto enumerado de objetos de tipo ") + name,
    BOperClassify::SetAlgebra_
  );
}


//--------------------------------------------------------------------
BBool BuildMethodFunctions()
    
/*! For each method in each grammar, it builds a function
 *  for apply this method to a set.
 */
//--------------------------------------------------------------------
{
//    Std(BText("\nBuilding method functions "));
    BList* lstGra = BGrammar::Instances();
    while(lstGra)
    {
  BGrammar* gra = (BGrammar*)Car(lstGra);
  BList* lstMet = gra->Methods();
  while(lstMet)
  {
      ((BMethod*)Car(lstMet))->BuildFunction();
      lstMet = Cdr(lstMet);
  }
  lstGra = Cdr(lstGra);
    }
    return(BTRUE);
}

//--------------------------------------------------------------------
#if defined(__USE_GBLA__)
BBool BLAPI_Init()
{
    char buffer[256];
    
    gbla_blapi_env_setup();
    int gbla_status = gbla_blapi_init();
    if(gbla_status==-1)
    {
  snprintf(buffer, 255, "error: could not initialize BLAS API with %s",
     gbla_blas_lib);
  Std(buffer);
  return BFALSE;
    } 
    else if(gbla_status==-2) 
    {
  snprintf(buffer, 255, "error: could not initialize LAPACK API with %s",
     gbla_lapack_lib);
  Std(buffer);
  return BFALSE;
    }

    return BTRUE;
}
#endif

//--------------------------------------------------------------------
  bool InitializeOrderedClasses()
//--------------------------------------------------------------------
{
  static bool _isAlreadyInitialized_ = false;
  if(!_isAlreadyInitialized_)
  {
    _isAlreadyInitialized_ = true;

    #if(__USE_POOL__==__POOL_BFSMEM__)
    # ifndef NDEBUG
      printf("\nInitializing Bayes Fixed Sise Memory Pool ");
    # endif
    BFixedSizeMemoryBase** BFSMEM_BuildInstances();
    BFSMEM_BuildInstances();
    #endif

    # ifndef NDEBUG
      printf("\nInitializing static member of basic classes ");
    # endif
    BText    ::InitializeClass();
    BOut     ::InitializeClass();
    BDate    ::InitializeClass();
  //BDat     ::InitializeClass();
    BProbDist::InitializeClass();

    return(false);
  }
  else
  {
    return(true);
  }
};


//--------------------------------------------------------------------
BBool InitGrammars()
//--------------------------------------------------------------------
{
  static BBool initGrammars_ = BFALSE;

  if(initGrammars_)  { return(false); }

  signal_assign();

  initGrammars_=BTRUE;

  Std(BText("\nInitializing TOL ")+_tolVersion_);

  TrcIG("Setting system defaults: ");
  TrcIGSL(BText("Output logging ")+BOut::DumpFile());
  TrcIGSL("LC_NUMERIC=C");
  setlocale( LC_NUMERIC, "C");

  TrcIG("System tools: ");
# if defined(_MSC_VER) && (_MSC_VER>=1300)
  int sbh = defaultSBH_;
  TrcIGS(BText("Small Block Heap=")<<sbh);
  _set_sbh_threshold(sbh);
# endif
# if defined(__USE_GBLA__)
  //------------------------------
  // GBLA Initialization:
  TrcIGS("GBLA");
  if(!BLAPI_Init()) return false;
  //------------------------------
# endif
  TrcIGS("GSL");
  GslInitialize();
  TrcIGS("Parser");
  BParser::InitDefaultParser();
  TrcIGS("Global Hashed Table");
  BGrammar::Initialize();
  TrcIGS("Local Stack Manager");
  BStackManager::Initialize();

  TrcIG("System types: ");
  //--------------------------------------------------
    TrcIGS("Anything");
  //--------------------------------------------------
  BAnyGrammar::InitGrammar();

  //--------------------------------------------------
    TrcIGS("Real");
  //--------------------------------------------------
  BUserDat::InitGrammar(1, "Real",
        I2("The real type is implemented as a number of double precision. "
     "Also it is included the unknown value (?) for omitted, infinite, "
     "imaginary or indeterminate values."
     "The separating decimal is the point.\n\n"
     "Example :\nReal x = 192.3\n",

     "El tipo real se implementa como un número de doble precisión. "
     "También se incluye el valor desconocido (?) para valores omitidos, "
     "infinitos, imaginarios o indeterminados."
     "El separador decimal es el punto.\n\n"
     "Ejemplo :\nReal x = 192.3\n"),
            BCopyContens<BDat>::New,
            BFunArgContens<BDat>::NewLocal);

  //--------------------------------------------------
    TrcIGS("Text");
  //--------------------------------------------------
  BUserText::InitGrammar(2, "Text",
         I2("An object of type text can contain any chain of ASCII characters."
      "The form of creating a variable of type text is to put the text "
      "between quotation marks.\n\n"
      "Example :\nText txt = \"Hello world\"\n",

      "Un objeto de tipo texto puede contener cualquier cadena de "
      "caracteres ASCII. La forma de crear una variable de tipo texto es "
      "poner el texto entre comillas.\n\n"
      "Ejemplo :\nText txt = \"Hola mundo\"\n\n"),
           BCopyContens<BText>::New,
           BFunArgContens<BText>::NewLocal);

  //--------------------------------------------------
    TrcIGS("Date");
  //--------------------------------------------------
  BUserDate::InitGrammar(3, "Date",
         I2("The type date represents a minimal time unit in the system that "
      "in the present version is referred to day. A date comes defined "
      "by a year, a month and a day of mondth.\n\n"
      "Example :\nDate dte =y1995m2d11\n",
      
      "El tipo fecha representa una unidad de tiempo mínima en el "
      "sistema que en la presente versión se refiere al día. Una fecha "
      "viene definida por un año, un mes y un día del mes.\n\n"
      "Ejemplo :\nDate dte =y1995m2d11\n"),
            BCopyContens<BDate>::New,
            BFunArgContens<BDate>::NewLocal);

  //--------------------------------------------------
    TrcIGS("Complex");
  //--------------------------------------------------
  BUserCmp::InitGrammar(4, "Complex",
        I2("The complex type is implemented as a real and a imaginary "
     "coordinates.\n\n"
     "Example :\nComplex z = 3+i*4\n",
     
     "El tipo complejo se implementa como el par de coordenadas real y "
     "compleja.\n\n"
     "Ejemplo :\nComplex z = 3+i*4\n"),
            BCopyContens<BCmp>::New,
            BFunArgContens<BCmp>::NewLocal);
  BUserCmp::OwnGrammar()->AddCasting(BUserDat::OwnGrammar());
  
  //--------------------------------------------------
    TrcIGS("Set");
  //--------------------------------------------------
  BUserSet::InitGrammar(5, "Set",
        I2("A set is a collection of objects of any type including other sets, "
     "something which permits to build structures as vectors, tables, "
     "trees, etc. The sets are the structural base of the language "
     "since the primary time series contents in data files constitute "
     "sets of objects, the same as the sentences of a file TOL that "
     "they can at the same time to include other files or directory.\n\n"
     "Example :\nSet s = SetOfSet(SetOfText(\"Hello\",\"Good-bye\"),"
     "SetOfReal(1,2,3,4,5,6))\n",
     
     "Un conjunto es una colección de objetos de cualquier tipo "
     "incluyendo otros conjuntos, lo cual permite construir estructuras "
     "como vectores, tablas, árboles, etc."
     "Los conjuntos son la base estructural del lenguaje pues las series "
     "temporales primarias contenidas en ficheros de datos constituyen "
     "conjuntos de objetos, al igual que las sentencias de un fichero "
     "TOL que pueden a su vez incluir a otros ficheros o directorios.\n\n"
     "Ejemplo :\nSet s = SetOfSet(SetOfText(\"Hola\",\"Adiós\"),"
     "SetOfReal(1,2,3,4,5,6))\n"),
           BRenContens<BSet>::New,
           BFunArgContens<BSet>::NewLocal);
  
  //--------------------------------------------------
    TrcIGS("Matrix");
  //--------------------------------------------------
  MatrixInitialize();
  BUserMat::InitGrammar(6, "Matrix",
        I2("Type matrix of real numbers. "
     "It is available a set of functions of linear systems resolution, "
     "matrixs investment and other functions of linear algebra "
     "and statistics.\n\n"
     "Example :\nMatrix x = ConcatRows(Row(1,3),Row(2,-1))\n",
     
     "Tipo matriz de números reales. "
     "Está disponible un conjunto de funciones de resolución de sistemas "
     "lineales, inversión de matrices y otras funciones de álgebra "
     "lineal y estadística.\n\n"
     "Ejemplo :\nMatrix x = ConcatRows(Row(1,3),Row(2,-1))\n"),
            BCopyContens<BMat>::New,
            BFunArgContens<BMat>::NewLocal);

  
  //--------------------------------------------------
    TrcIGS("TimeSet");
  //--------------------------------------------------
  BUserTimeSet::InitGrammar(7, "TimeSet",
      I2("A time set is any subset, finite or infinite, "
         "of the joint of all the dates from the beginning until the end "
         "of the time. An infinite time set would be, for example,"
         "all on Mondays that they fall in first of month,"
         "all on Fridays and thirteen, all on Sundays of Easter, ..."
         "They are included functions to build practically any "
         "imaginable time set, something which is very useful to handle "
         "time series, above all when it is considered daily series, "
         "since the complexity of the social time suffers often serious "
         "problems for the data analyst.\n\n"
         "Example :\nTimeSet Sunday = WD(7)\n",
         
         "Un conjunto temporal es cualquier subconjunto, finito o "
         "infinito, del conjunto de todas las fechas desde el comienzo "
         "hasta el final del tiempo. Un conjunto temporal infinito "
         "podría ser, por ejemplo, todos los lunes que caen en primero "
         "de mes, todos los viernes y trece, todos los domingos de "
         "Pascua, ... "
         "Se incluyen funciones para construir prácticamente cualquier "
         "conjunto temporal imaginable, lo cual es muy útil para manejar "
         "series temporales, sobre todo cuando se trata de series "
         "diarias, pues la complejidad del tiempo social conlleva a "
         "menudo graves problemas para el analista de datos.\n\n"
         "Ejemplo :\nTimeSet Domingo = WD(7)\n"),
          BTmsRenamed::New,
          BTmsFunArg::NewLocal);
  BUserTimeSet::OwnGrammar()->AddCasting(BUserDate::OwnGrammar());
  
  //--------------------------------------------------
    TrcIGS("Serie");
  //--------------------------------------------------
  BUserTimeSerie::InitGrammar(8, "Serie",
        I2("A time series is an application of an infinite time set "
           "(dating) in the real numbers, that is to say, a series "
           "returns, for each date of the dating, a real number, the "
           "one which can be done not know. If all the values of a "
           "series are unknown before a given date is said that this "
           "is the initial date of such series. In other case the "
           "series lacks initial date. In the same way if all the "
           "values of the series are unknown as of a given date, "
           "this will be the final date of the series. "
           "If this did not occur for no date we will say that the "
           "series does not has final date.\n\n"
           "Example :\nSerie SundayIndicator = CalInd(WD(7), Daily)\n",

           "Una serie temporal es una aplicación de un conjunto temporal "
           "infinito (fechado) en los números reales, es decir,  una "
           "serie devuelve, para cada fecha del fechado, un número "
           "real, el cual puede ser desconocido. Si todos los valores "
           "de una serie son desconocidos antes de una fecha dada se "
           "dice que ésta es la fecha inicial de dicha serie. En otro "
           "caso la serie carece de fecha inicial. Del mismo modo si "
           "todos los valores de la serie son desconocidos a partir de "
           "una fecha dada, ésta será la fecha final de la serie. "
           "Si esto no ocurriera para ninguna fecha diremos que la "
           "serie no tiene fecha final.\n\nEjemplo :\n\nSerie "
           "IndicadorDomingo = CalInd(WD(7), Daily)\n"),
            BTsrRenamed ::New,
            BTsrFunArg::NewLocal);
  BUserTimeSerie::OwnGrammar()->AddCasting(BUserDat::OwnGrammar());

  
  //--------------------------------------------------
    TrcIGS("Polyn");
  //--------------------------------------------------
  BUserPol::InitGrammar(9, "Polyn",
        I2("A delays polynomial is a polynomial in the backward and forward "
     "operators (B and F) that applied to a series return for each date "
     "the value of the series for the previous or subsequent date "
     "respectively. These objects are very useful to build ARIMA models "
     "of time series.\n\n"
       "Example :\nPolyn pol = F^2 + 1 - B^2\n",
     
     "Un polinomio de retardos es un polinomio en los operadores de "
     "retardo y adelanto (B y F) que aplicados a una serie devuelven "
     "para cada fecha el valor de la serie para la fecha anterior o "
     "posterior respectivamente. Estos objetos son muy útiles para "
     "construir modelos ARIMA de series temporales.\n\n"
     "Ejemplo :\nPolyn pol = F^2 +1 -B^2\n"),
           BCopyContens<BPol>::New,
           BFunArgContens<BPol>::NewLocal);
  BUserPol::OwnGrammar()->AddCasting(BUserDat::OwnGrammar());
  
  //--------------------------------------------------
    TrcIGS("Ratio");
  //--------------------------------------------------
  BUserRat::InitGrammar(10, "Ratio",
        I2("A rational delays function is defined as a quotient of delay "
     "polynomials. Its principal usefulness is to solve equations in "
     "differences of the type P(B)Zt = Q(B)At.\n\n"
     "Example :\nRatio rat = (F^2 +1 -B^2) / (1 - B)\n",
     
     "Una función racional de retardos se define como un cociente de "
     "polinomios de retardo. Su principal utilidad es resolver "
     "ecuaciones en diferencias del tipo P(B)Zt = Q(B)At.\n\n"
     "Ejemplo :\n\nRatio rat = (F^2 +1 -B^2) / (1 - B)\n"),
           BCopyContens<BRat>::New,
           BFunArgContens<BRat>::NewLocal);
  BUserRat::OwnGrammar()->AddCasting(BUserPol::OwnGrammar());


  //--------------------------------------------------
    TrcIGS("Code");
  //--------------------------------------------------
  BUserCode::InitGrammar(11, "Code",
         I2("Any function can be used as an object of type Code "
      "to happen it as argument of other function. "
      "The user can define new functions.\n\n"
      "Example :\n"
      "Real HalfSumm(Real x1, Real x2)\n"
      "{\n"
      "  summ = x1+x2;\n"
      "  summ/2\n"
      "}\n\n",
      
      "Cualquier función puede ser usada como un objeto de tipo Code "
      "para pasarla como argumento de otra función. El usuario puede "
      "definir nuevas funciones.\n\n"
      "Ejemplo :\n"
      "Real SemiSumm(Real x1, Real x2)\n"
      "{\n"
      "  suma = x1+x2;\n"
      "  suma/2\n"
      "}\n\n"),
           BCopyContens<BCode>::New,
           BFunArgContens<BCode>::NewLocal);

#ifdef __USE_TC__

  //--------------------------------------------------
    TrcIGS("CTime");
  //--------------------------------------------------
  BUserCTime::InitGrammar("CTime",
             I2("Example :\nCTime tmi =y1995m2d11h22\n",
          "Ejemplo :\nCTime tmi =y1995m2d11h22\n"),
             BCopyContens<BCTime>::New,
             BFunArgContens<BCTime>::NewLocal);

  //--------------------------------------------------
    TrcIGS("CTimeSet");
  //--------------------------------------------------
  BUserCTimeSet::InitGrammar("CTimeSet",
			     I2("Example :\nCTimeSet Sunday = WD(7)\n",
				"Ejemplo :\nCTimeSet Domingo = WD(7)\n"),
			     BCTmsRenamed::New,
			     BCTmsFunArg::NewLocal);
  BUserCTimeSet::OwnGrammar()->AddCasting(BUserCTime::OwnGrammar());
  
  //--------------------------------------------------
    TrcIGS("CSerie");
  //--------------------------------------------------
  BUserCTimeSeries::InitGrammar("CSeries",
		I2("Example: \nCSeries csr = CalInd(WD(7), Daily)\n",
		   "Ejemplo: \nCSeries csr = CalInd(WD(7), Daily)\n"),
				BCTsrRenamed ::New,
				BCTsrFunArg::NewLocal);
  BUserCTimeSeries::OwnGrammar()->AddCasting(BUserDat::OwnGrammar());
  
#endif /* __USE_TC__ */

  //--------------------------------------------------
    TrcIGS("NameBlock");
  //--------------------------------------------------
  BUserNameBlock::InitGrammar(12, "NameBlock",
  I2("A NameBlock is a special data type that is created from a set to "
     "which we will call base of the NameBlock, and that implies a "
     "scope of location of objects.\n"
     "We will call members to  its elements and can as much be "
     "variables as functions or structures.\n"
     "There will be them remain hidden outside the scope and we will call "
     "private members to them.\n"
     "The rest, called members public, could be accessible from the "
     "outside, in spite of being always all premises by construction, "
     "with no need to specify the keys  {...}.\n",
     "Un espacio de nombres o NameBlock es un tipo de datos especial que se "
     "crea partiendo de un conjunto al que llamaremos base del NameBlock, y "
     "que implica un ámbito de localización de objetos.\n"
     "A sus elementos les llamaremos miembros y pueden ser tanto variables "
     "como funciones o estructuras.\n"
     "Los habrá que permanezcan ocultos fuera del ámbito y les llamaremos "
     "miembros privados.\n"
     "El resto, llamados miembros públicos, podrán ser accesibles desde el "
     "exterior, a pesar de ser siempre todos locales por construcción, sin "
     "necesidad de explicitar las llaves {...}. ")+
  I2("Read more at ","Leer mas en ")+"http://www.tol-project.org/es/node/40",
  BRenContens<BNameBlock>::New,
  BFunArgContens<BNameBlock>::NewLocal);

  //--------------------------------------------------
    TrcIGS("VMatrix");
  //--------------------------------------------------
  BVMat::InitializeClass();
  BUserVMat::InitGrammar(13, "VMatrix", I2(
  "Type virtual matrix is usefull to handle with special matrices :\n",
  "El tipo matriz virtual es útil para manejar matrices especiales:\n")+
  " - Cholmod Real Sparse : \n"
  " - Cholmod Real Factor \n"
  " - Cholmod Real Triplet \n"
  " - Blas Real Dense \n"+
  I2("Read more at ","Leer mas en ")+"http://www.tol-project.org/es/node/90",
  BCopyContens<BVMat>::New,
  BFunArgContens<BVMat>::NewLocal);
//BUserVMat::OwnGrammar()->AddCasting(BUserMat::OwnGrammar());

    TrcIG ("Syntax elements: ");
    TrcIGS("Code Classify");
    BOperClassify::InitInstances();
    TrcIGS("Special functions");
    BSpecialFunction::Initialize();

    TrcIG("Built-in objects: ");
    TrcIGS("Code");
    InitCommonInstances(GraCode());
    TrcIGS("Date");
    InitCommonInstances(GraDate());
    TrcIGS("Polyn");
    InitCommonInstances(GraPolyn());
    TrcIGS("Ratio");
    InitCommonInstances(GraRatio());
    TrcIGS("Real");
    InitCommonInstances(GraReal());
    TrcIGS("Complex");
    InitCommonInstances(GraComplex());
    TrcIGS("Matrix");
    InitCommonInstances(GraMatrix());
    TrcIGS("VMatrix");
    InitCommonInstances(GraVMatrix());
    TrcIGS("Set");
    InitCommonInstances(GraSet());
    TrcIGS("NameBlock");
    InitCommonInstances(GraNameBlock());
    TrcIGS("Text");
    InitCommonInstances(GraText());
    TrcIGS("TimeSet");
    InitCommonInstances(GraTimeSet());
    TrcIGS("Serie");
    InitCommonInstances(GraSerie());
    TrcIGS("Anything");
    InitCommonInstances(GraAnything());
#ifdef __USE_TC__
    TrcIGS("CTime");
    InitCommonInstances(GraCTime());
    TrcIGS("CTime");
    InitCommonInstances(GraCTimeSet());
    TrcIGS("CSeries");
    InitCommonInstances(GraCSeries());
#endif /* __USE_TC__ */
    TrcIG("Sub-systems:");
    TrcIGSL("AIA: Automatic Intervention Analisis");
  	BAia::StaticInit();	
    TrcIGSL("OIS: Objects Indexed Serialization");
    BOis::Initialize();
    BGrammar::PutLast(GraSet());
#   if defined(USE_DELAY_INIT)
    TrcIG("Delayed static objects");
      __delay_init(NULL,NULL);
#   endif

    TrcIGS("Methods");
    BuildMethodFunctions();

  //initializing tools 
    BysSparseReg::Initialize();
    int BTolOprProfiler_Init();
    BTolOprProfiler_Init();
    TOLHasBeenInitialized_ = true;
    return(initGrammars_);
}


//--------------------------------------------------------------------
const char * GetCompilationDate()
//--------------------------------------------------------------------
{
  static char aux_[64];
  static bool init_ = false;
  if(!init_)
  {
    snprintf(aux_, 64, "%s",__DATE__);
    init_ = true;
  }
  return aux_;
}

//--------------------------------------------------------------------
const char * GetCompilationTime()
//--------------------------------------------------------------------
{
  static char aux_[64];
  static bool init_ = false;
  if(!init_)
  {
    snprintf(aux_, 64, "%s",__TIME__);
    init_ = true;
  }
  return aux_;
}

//--------------------------------------------------------------------
const char * GetCompilationDateTime()
//--------------------------------------------------------------------
{
  static char aux_[64];
  static bool init_ = false;
  if(!init_)
  {
    snprintf(aux_, 64, "%s %s",__DATE__,__TIME__);
    init_ = true;
  }
  return aux_;
}

//--------------------------------------------------------------------
const char * TOLVersionBuild()
//--------------------------------------------------------------------
{
  static char aux_  [128];
  static bool init_ = false;
  if(!init_)
  {
   #ifdef __TOL_BUILD_DATE__
    const char* tolBuildDate = __TOL_BUILD_DATE__;
   #else
    char month_[4];
    char tolBuildDate[64];
    int year, month, day;
    sscanf(__DATE__,"%s %d %d",month_,&day,&year);
    BText m = ToLower(month_);
    if((m=="ene")||(m=="jan")) { month= 1; } else
    if((m=="feb")            ) { month= 2; } else
    if((m=="mar")            ) { month= 3; } else
    if((m=="abr")||(m=="apr")) { month= 4; } else
    if((m=="may")            ) { month= 5; } else
    if((m=="jun")            ) { month= 6; } else
    if((m=="jul")            ) { month= 7; } else
    if((m=="ago")||(m=="aug")) { month= 8; } else
    if((m=="sep")            ) { month= 9; } else
    if((m=="oct")            ) { month=10; } else
    if((m=="nov")            ) { month=11; } else
    if((m=="dic")||(m=="dec")) { month=12; }
    snprintf(tolBuildDate, 64, "%04d-%02d-%02d",
            year,
            month,
            day);
   #endif
   #ifdef __TOL_BUILD_TIME__
    const char* tolBuildTime = __TOL_BUILD_TIME__;
   #else
    const char* tolBuildTime = __TIME__;
   #endif
   #ifdef __TOL_BUILD_PLATFORM__
    const char* tolBuildPlatform = __TOL_BUILD_PLATFORM__;
   #elif defined WIN32
    const char* tolBuildPlatform = "i686-win";
   #elif defined UNIX
    const char* tolBuildPlatform = "i686-linux-gnu";
   #endif
    snprintf(aux_, sizeof(aux_), "b.%s %s %s %s",
            VERSION_BUILD,
            tolBuildDate,
            tolBuildTime,
            tolBuildPlatform);
    init_ = true;
  }
  return aux_;
}

//--------------------------------------------------------------------
const char * TOLVersion()
//--------------------------------------------------------------------
{
  static char aux_  [64];
  static bool init_ = false;
  if(!init_)
  {
    snprintf
    (
     aux_, 64,
      "v%d.%d.%d %s"
# ifndef NDEBUG
      " {DEBUG}"
# endif
     ,VERSION_MAJOR,
      VERSION_MINOR,
      VERSION_RELEASE,
      TOLVersionBuild()
    );
    init_ = true;
  }
  return aux_;
}

//--------------------------------------------------------------------
const char * TOLCredits()
//--------------------------------------------------------------------
{
  if(!_tolCredits_.Length())
  {
    _tolCredits_ = 
    I2("TOL is an open source project developed by Bayes (http://www.bayesforecast.com/) "
       "and distributed under public GNU license from http://www.tol-project.org",
       "TOL es un proyecto de código abierto desarrollado por la empresa Bayes "
       "(http://www.bayesforecast.com/) y distribuido bajo licencia pública GNU "
       "en http://www.tol-project.org")+"\n"
    "TOL (Time Oriented Language) - "
    "Copyright © 2003-"+DteToday().Year()+ " by Bayes Decision, SA";
  }
  return(_tolCredits_);
}

//--------------------------------------------------------------------
const char * TOLContribAcknowledgements()
//--------------------------------------------------------------------
{
  if(!_tolContribAcknowledgements_.Length())
  {
    _tolContribAcknowledgements_ = 
    I2("Acknowledgements.\n"
      "TOL was not possible without the contribution of next open source projects:\n",
      "Agradecimientos.\n"
      "TOL no sería posible sin la contribución de los siguientes proyectos de código abierto:")+"\n\n"+
    " * ALGLIB: \n"
    "     Efficient multilingual scientific software library \n"
    "     (http://www.alglib.net)\n\n"
    " * BLAS: \n"
    "     Basic Linear Algebra Subprograms \n"
    "     (http://www.netlib.org/blas/index.html)\n\n"
    " * Boost Spirit: \n"
    "     Is a Boost Template Library that implements sn object oriented recursive descent parser framework \n"
    "     (http://spirit.sourceforge.net/; http://www.boost.org/)\n\n"
    " * BZip2: \n"
    "     High-quality data compressor \n"
    "     (http://www.bzip.org/)\n\n"
    " * CLUSTERLIB: \n"
    "     The C clustering library for cDNA microarray data \n"
    "     (http://bonsai.ims.u-tokyo.ac.jp/~mdehoon/software/cluster/software.htm#source)\n\n"
    " * DCDFLIB: \n"
    "     Evaluating cumulative probability density functions \n"
    "     (http://people.scs.fsu.edu/~burkardt/cpp_src/dcdflib/dcdflib.html)\n\n"
    " * Google Sparse Hash: \n"
    "     An extremely memory-efficient hash_map implementation \n"
    "     (http://code.google.com/p/google-sparsehash/)\n\n"
    " * GSL: \n"
    "     GNU Scientific Library \n"
    "     (http://www.gnu.org/software/gsl/)\n\n"
    " * KMLOCAL:\n"
    "     Efficient Algorithms for K-Means Clustering \n"
    "     (http://www.cs.umd.edu/users/mount/Projects/KMeans/)\n\n"
    " * LAPACK: \n"
    "     Linear Algebra PACKage \n"
    "     (http://www.netlib.org/lapack/index.html)\n\n"
    " * Optimal_bw: \n"
    "     Fast O(N+M) Univariate Kernel Density Estimation with Optimal Bandwith \n"
    "     (http://www.umiacs.umd.edu/~vikas/Software/optimal_bw/optimal_bw_code.htm)\n\n"
    " * SuiteSparse CHOLMOD: \n"
    "     Sparse Cholesky factorization and update/downdate \n"
    "     (http://www.cise.ufl.edu/research/sparse/cholmod/)\n\n"
    " * ZipArchive: \n"
    "     Adds ZIP compression functionality to your software \n"
    "     (http://www.artpol-software.com/)\n";
  }
  return(_tolContribAcknowledgements_.String());
}
//--------------------------------------------------------------------
  void DestroyInitTOLFile()
//--------------------------------------------------------------------
{
  if(initTol_)
  {
    initTol_->DecNRefs();
    DESTROY(initTol_);
  }
}

//--------------------------------------------------------------------
  bool InitTOLFile(const char* str)
//--------------------------------------------------------------------
{
  BText txt = str;
#ifndef UNIX
  txt.Replace( '\\', '/' );
#endif
  if(initTol_) { return(true); }
  if(txt.HasName()) 
  {
    Std(I2("\nLoading standard TOL library ",
           "\nCargando la libreria estandar de TOL ") + txt +"\n");
    initTol_ = GraSet()->EvaluateExpr(Out()+"Set InitTol = Include(\""+txt+"\")");
    if(initTol_) 
    {
      initTol_->IncNRefs();
    }
  }
  return(true);
};

//--------------------------------------------------------------------
void LoadInitLibrary(char* calledProgram)
//--------------------------------------------------------------------
{
  static bool done_ = false;
  static BSystemText* version_ = NULL;
  if(done_) { return; }
  else      { done_ = true; }
  _tolVersion_ = TOLVersion();
  TOLHasBeenInitialized_ = false;
  version_=new BSystemText("Version", _tolVersion_,
  I2("Time Oriented Language Version Identifier.",
     "Identificador de la versión de Time Oriented Language."));
  _tolSessionPath_ = Replace(TolFindExecutable(calledProgram),"\\","/");
  tolSessionPath_ = new BSystemText("TOLSessionPath", _tolSessionPath_,
    I2("The path of the TOL program that is being executed in this session."
       "It's usefull, for example, to call TOL using operative system.",
	     "El camino del programa TOL que esta siendo ejecutado en este momento."
       "Es util, por ejemplo, para llamar a TOL usando el sistema operativo."));
  TOLHasBeenInitialized_ = true;

  BText initpath = GetFilePath(_tolSessionPath_)+  "stdlib";
  BDir initTolPath = initpath + "/_init_stdlib.tol";
  if(!initTolPath.Exist()) 
  {
    initpath = "./stdlib";
    initTolPath = initpath + "/_init_stdlib.tol";
    if(!initTolPath.Exist()) 
    {
      initpath = BSys::GetEnv( "TOL_LIBRARY" );
      if(initpath != "") 
      {
        initTolPath = initpath + "/_init_stdlib.tol";
      }
#ifdef TOLINIT_LIB_DIR
      if (!initTolPath.Exist()) 
      {
        initpath = TOLINIT_LIB_DIR;
        initTolPath =  initpath + "/_init_stdlib.tol";
      }
#endif
    }
  }
  if (initTolPath.Exist()) 
  {
    InitTOLFile(initTolPath.Name());
  }
}

//--------------------------------------------------------------------
static BText HelpVerboseMode() 
//--------------------------------------------------------------------
{
  BText helpEng = BText("")+
  "It's possible to specify what kind of message TOL must show or not "+
  "using the verbose/mute argument which syntax can be expressed as:\n"+
  "    {v/m}[X...X] -> X={A,E,W,S,U,T} \n"+
  "Lower case flag 'v' means enabling messages and 'm' disabling ones.\n"+
  "Upper case flags X={E,W,S,U,T} means the type of message that must be modified:\n"+
  "  A: All messages\n"+
  "  E: Error messages\n"+
  "  W: Warning messages\n"+
  "  S: Normal system messages like 'Included file ...'\n"+
  "  U: User messages, it's to say, messages launched with functions Write/WriteLn/View \n"+
  "  T: Trace system messages for debugging\n\n"+
  "Observe that user can send all types of messages adding a second argument to "+
  "Write/WriteLn functions, but system can't send user messages.\n\n"
  "There are some examples of use: \n"+
  "  -vA     : Enables all messages\n"+
  "  -vE     : Enables just error messages and disables the rest\n"+
  "  -vW     : Enables just warning messages and disables the rest\n"+
  "  -vS     : Enables just normal system messages and disables the rest\n"+
  "  -vU     : Enables just user messages and disables the rest\n"+
  "  -vT     : Enables just tracing messages and disables the rest\n"+
  "  -vEW    : Enables just error and warning messages and disables the rest. Its the same as mUST\n"+
  "  -mA     : Disables all messages \n"+
  "  -mE     : Disables just error messages and enables the rest\n"+
  "  -mW     : Disables just warning messages and enables the rest\n"+
  "  -mS     : Disables just normal system messages and enables the rest\n"+
  "  -mU     : Disables just user messages and enables the rest\n"+
  "  -mT     : Disables just tracing messages and enables the rest\n"+
  "  -mST    : Disables normal system and tracing messgaes and enables the rest. Its the same as vEWU\n"+
  "Note that any combination can be written in 3 character or less. For example vEWU=mST.\n"
  "For backward compatibility reasons it will allowed next simplified expressions:\n"
  "  -v      : Enables all messages but tracing ones. Its the same as mT\n"+
  "  -m      : Disables all messages but errors. Its the same as vE\n\n"+
  "If no verbose/mode argument is passed to TOL, then default values will be used:"+
  "  -vEU    : Enables just error and user messages and disables the rest\n";
  BText helpSpa = BText("")+
  "Es posible especificar a TOL qué tipos de mensajes se quieren mostrar y cuáles no "+
  "mediante el argumento de verbosidad cuya sintaxis se puede expresar así:\n"+
  "    {v/m}[X...X] -> X={A,E,W,S,U,T} \n"+
  "El indicador en minúsculas 'v' implica habilitación de mensajes y 'm' deshabilitación.\n"+
  "El indicador en mayúsculas X={E,W,S,U,T} especifica el tipo de mensaje al que modifica el indicador anterior:\n"+
  "  A: Todos los mensajes (All)\n"+
  "  E: Mensajes de Error\n"+
  "  W: Mensajes de aviso (Warning)\n"+
  "  S: Mensajes normales del Sistema como 'Ha sido incluido el fichero ...'\n"+
  "  U: Mensajes de Usuario, es decir, los lanzados por las funciones Write/WriteLn/View \n"+
  "  T: Mensajes de Trazado para la depuración de problemas\n\n"+
  "Obsérvese que el usuario puede enviar mensajes de todo tipo añadiendo un segundo arguemnto a las funciones "+
  "Write/WriteLn, pero el sistema nunca puede mandar mensajes de usuario.\n\n"+
  "He aquí algunos ejemplos de uso: \n"+
  "  -vA     : Habilita todos los mensajes\n"+
  "  -vE     : Habilita sólo los mensajes de error y deshabilita el resto\n"+
  "  -vW     : Habilita sólo los mensajes de aviso y deshabilita el resto\n"+
  "  -vS     : Habilita sólo los mensajes normales del sistema y deshabilita el resto\n"+
  "  -vU     : Habilita sólo los mensajes de usuario y deshabilita el resto\n"+
  "  -vT     : Habilita sólo los mensajes de trazado y deshabilita el resto\n"+
  "  -vEW    : Habilita sólo los mensajes de error y aviso y deshabilita el resto. Es lo mismo que mUST\n"+
  "  -mA     : Deshabilita todos los mensajes \n"+
  "  -mE     : Deshabilita sólo los mensajes de error y habilita el resto\n"+
  "  -mW     : Deshabilita sólo los mensajes de aviso y habilita el resto\n"+
  "  -mS     : Deshabilita sólo los mensajes normales del sistema y habilita el resto\n"+
  "  -mU     : Deshabilita sólo los mensajes de usuario y habilita el resto\n"+
  "  -mT     : Deshabilita sólo los mensajes de trazado y habilita el resto\n"+
  "  -mST    : Deshabilita sólo los mensajes normales del sistema y de trazado y habilita el resto. Es lo mismo que vEWU\n"+
  "Nótese que cualquier combinación se puede expresar con tres caracteres o menos. Por ejemplo vEWU=mST.\n"+
  "Por razones de compatibilidad hacia atrás se permiten las siguientes formas simplificadas:"+
  "  -v      : Habilita todos los mensajes salvo los de trazado. Es lo mismo que mT\n"+
  "  -m      : Deshabilita todos los mensajes salvo los errores. Es lo mismo que vE\n\n"+
  "Si no se pasa ningún argumento de verbosidad a TOL se usarán los valores por defecto:\n"
  "  -vEU    : Habilita sólo los mensajes de error y de usuario y deshabilita el resto\n"
  "\n"
  ;
  return(I2(helpEng,helpSpa));
}


//--------------------------------------------------------------------
static void InitVerboseMode() 
//--------------------------------------------------------------------
{
//If no verbose/mode is used then it will be used next initial 
//default values: Only user and error/warning messages will be 
//displayed.
  BOut::PutAllTerm  (BTRUE);
  BOut::PutAllHci   (BTRUE);
  BOut::PutErrorHci (BTRUE);
  BOut::PutErrorTerm(BTRUE);
  BOut::PutInfoTerm (BTRUE);
  BOut::PutInfoHci  (BTRUE);
}  

//--------------------------------------------------------------------
static void ChangeVerboseMode(const char* vmode) 
//--------------------------------------------------------------------
{
  bool ok = true;
  unsigned int i=0;
  if(!vmode) { return; }
  if((vmode[0]!='v')&&(vmode[0]!='m'))
  {
    ok = false;
  }
  else if((vmode[0]=='v') && (vmode[1]=='\0'))
  {
  //Backward compatibility of non explicit verbose mode vmode=="v"
  //is equivalent to explicit verbose mode "vAmT", is to say, all
  //vervose but traces
    BOut::PutAllHci   (true);
    BOut::PutAllTerm  (true);
    BOut::PutTraceHci (false);
    BOut::PutTraceTerm(false);
  }
  else if((vmode[0]=='m') && (vmode[1]=='\0'))
  {
  //Backward compatibility of non explicit verbose mode vmode=="m"
  //is equivalent to explicit verbose mode "mAvE", is to say, all
  //mute but errors
    BOut::PutAllHci   (false);
    BOut::PutAllTerm  (false);
    BOut::PutErrorHci (true);
    BOut::PutErrorTerm(true);
  }
  else
  {
    bool mode;
  //If argument begis 
    if(vmode[0]=='v')      
    { 
      BOut::PutAllHci (false);
      BOut::PutAllTerm(false);
      mode = true; 
    }
    else if(vmode[0]=='m') 
    { 
      BOut::PutAllHci (true);
      BOut::PutAllTerm(true);
      mode = false; 
    }
    else                   
    { 
      ok = false; 
    }
    for(i=1; ok && vmode[i]; i++) 
    {
      switch (vmode[i]) 
      {
        case 'A':
          BOut::PutAllHci (mode);
          BOut::PutAllTerm(mode);
          break;
        case 'E':
          BOut::PutErrorHci (mode);
          BOut::PutErrorTerm(mode);
          break;
        case 'W':
          BOut::PutWarningHci (mode);
          BOut::PutWarningTerm(mode);
          break;
        case 'S':
          BOut::PutStdHci (mode);
          BOut::PutStdTerm(mode);
          break;
        case 'U':
          BOut::PutInfoHci (mode);
          BOut::PutInfoTerm(mode);
          break;
        case 'T':
          BOut::PutTraceHci (mode);
          BOut::PutTraceTerm(mode);
          break;
        default :
          /* invalid character */
          ok = false;
          break;
      }
      if(!ok) { break; }
    }
  }
  if(!ok)
  {
    InitVerboseMode();
    Error(I2
    (
      "Wrong formatted verbose/mute argument\n"
      "Default values will be used.\n"
      "Unexpected character '",
      "Formato inválido del argumento verbose/mute\n"
      "Se usarán los valores por defecto.\n" 
      "Caracter inesperado '"
    )+vmode[i]+"' "+I2
    (
      "at position",
      "en la posición"
    )+" "+int(i+1)+I2
    (
      " of argument ",
      " del argumento "
    )+vmode+"\n\n\n"+HelpVerboseMode());
  }
}

//--------------------------------------------------------------------
void InitTolKernel( int lang, const char* vmode ) 
//--------------------------------------------------------------------
{
  BText::PutLanguage(lang);
  InitVerboseMode();
  /* Aqui hay que particionar vmode e iterar invocando
     ChangeVerboseMode. Las diferentes opciones vienen separadas por
     espacios */
  if (vmode && *vmode) {
    char *mode = strdup(vmode);
    char *cur_mode = mode, *next_mode;
    while (*cur_mode) {
      next_mode = cur_mode;
      while (*next_mode && *next_mode != ' ') {
        ++next_mode;
      }
      if (*next_mode==' ') {
        *next_mode = '\0';
        ++next_mode;
      }
      ChangeVerboseMode(cur_mode);
      cur_mode = next_mode;
    }
    free(mode);
  }
  InitGrammars();
}


//--------------------------------------------------------------------
void Dialog()
//--------------------------------------------------------------------
{
  BGrammar::PutLast(GraReal());
  InteractiveTOL();
//BGrammar::End();
  Trace("END");
}


//--------------------------------------------------------------------
void InitializeFromMainArgs(int argc, char *argv[], char *env[])
//--------------------------------------------------------------------
{
  int aliveObjects_1  = 0, aliveObjects_2  = 0;
  int aliveObjects_1_ = 0, aliveObjects_2_ = 0;  
  InitVerboseMode();
  BText::PutFormatBReal("%lf");
  BText::PutFormatBInt ("%ld");
  BGrammar::PutLast(GraSet());
  bool initTOL=true, dialogMode=BFALSE, helpMode=(argc==1);

  BList* stack = NIL;

  if(!helpMode) {
    BInt i = 1;
    TrcIG("Parsing TOL executable arguments");
    do 
    {
      BText arg = Compact(argv[i]);
      TrcIGSL(BText("(")+i+")["+arg+"]");
      if(arg.Get(0) == '-')
      {
        BChar com = arg.Get(1);
        BText par = arg.SubString(2,arg.Length()-1);
             if  (com == 'v') { ChangeVerboseMode(arg.String()+1); }
        else if  (com == 'm') { ChangeVerboseMode(arg.String()+1); }
        else if  (com == 'f') { BOut::PutDumpFile(); }
        else if  (com == 'i') { initTOL = BFALSE;  }
        else if  (com == 'e') { BText::PutLanguage(BENGLISH); }
        else if  (com == 's') { BText::PutLanguage(BSPANISH); }
        else if  (com == 'd') { dialogMode=BTRUE; } 
        else if  (com == 'h') { helpMode = BTRUE; }
        else if  (com == 'c')
        {
          InitGrammars();
          if(initTOL) { LoadInitLibrary(argv[0]); }
          Trace(I2("\nCompiling command line argument -c :\n",
                   "\nCompilando argumento -c de la línea de comandos :\n  ") + par+"\n");
          BList* result = MultyEvaluate(par);
          if(result)
          {
            BSet set; set.RobStruct(result,NIL,BSet::Generic);
            BUserSet* uSet = new BContensSet("",set,"");
            stack = Cons(uSet, stack);
          }
        }
#if defined(_MSC_VER) && (_MSC_VER>=1300)
        else if  (com == 'S')
        {
          int sbh = defaultSBH_;
          sscanf(par.String(),"%ld", &sbh);
          _set_sbh_threshold(sbh);
          Std(BText("Setting Small Block Heap For MaxBytes : ")<<sbh);
        }
#endif
        else
        {
          Error(I2("Unknown command ",
                   "Comando desconocido ")+"'"+arg+"'");
          helpMode = true;
        }
      }
      else
      {
        InitGrammars();
        if(initTOL) { LoadInitLibrary(argv[0]); }
        if(arg.HasName()) {
          BText fName = argv[i];
          fName = Replace(fName,"\n","/n");
          fName = Replace(fName,"\t","/t");
          fName = Replace(fName,"\\","/");
          BText command = BText("\nSet Include(\"")+fName+"\")";
          BSyntaxObject* result = GraSet()->EvaluateExpr(command);
          if(result)
          {
            stack = Cons(result, stack); 
            result->Do();
          }
        }
      }
      i++;
    } while(i<argc);
  }
  
  if(helpMode) {
    BText exeName = GetFileName(argv[0]);
    BText helpText =  BText("\n") + TOLVersion()+ I2
      (
      "This program is a Time Oriented Language analizer in command"
      " \n line mode.\n\nYou can compile any number of TOL files"
      " writing simply : \n \n"
      "   "+exeName+" file1 file2 file3 ... \n"
      "\n"
      "NOTE : You nust use the '/' character for paths like UNIX"
      " even if \n you are using other operating systems.\n"
      "\n"
      "If a file with relative path 'initlibrary/_inittol.tol' is"
      " found\n this programa will compile before than another one"
      " file if you don't\n disables it with option -i\n"
      "\n"
      "You can select the idiom with next options :\n"
      "   -e : english \n"
      "   -s : spanish \n"
      "\n"
      "If you want to see the messages generated along the TOL files"
      " \ncompiling you must use the verbose/mute option. For example : \n"
      "\n"
      "   To show all kind of messages for all files, type  \n"
      "     "+exeName+" -vA file1 file2 file3  \n"
      "\n"
      "   To show messages only for file2 and file3, type  \n"
      "   "+exeName+" -mA file1 -vA file2 file3  \n"
      "\n"+
      HelpVerboseMode()+
      "In order to compile a single expression you can use the -c"
      " option :\n"
      "     "+exeName+" -v -c\"Real n=1\"  \n"
      "Also you can use the single expression analizer using the"
      " dialog \n(-d) option.\n\n"
      ,
      "Este programa es un analizador de Time Oriented Language \n"
      "\n(Lenguaje Orientado al Tiempo) en modo de linea de"
      " comandos.\n\n"
      "Para compilar cualquier numero de ficheros TOL basta escribir"
      " : \n\n"
      "   "+exeName+" fichero1 fichero2 fichero3 ... \n"
      "\n"
      "NOTA: Usese el caracter '/' para los caminos como en UNIX"
      " aunque se \neste usando otro sistema operativo.\n"
      "\n"
      "Si existe el fichero con camino relativo"
      " 'initlibrary/_inittol.tol' \n"
      "el programa lo compilara antes que cualquier otro a no ser"
      " que se le\nimpida con la opcion -i\n"
      "\n"
      "Se puede seleccionar el idioma con las opciones \n"
      "   -e : english \n"
      "   -s : spanish \n"
      "Si se desea observar los mensajes hay que usar el argumento "
      "de verbosidad. Por ejemplo: \n"
      "\n"
      "   Para mostrar los mensajes de todos los ficheros, escribir  \n"
      "     "+exeName+" -vA fichero1 fichero2 fichero3  \n"
      "\n"
      "   Para mostrar solo los mensajes de los ficheros 2 y 3"
      " , escribir \n"
      "   "+exeName+" -mA fichero1 -v fichero2 fichero3  \n"
      "\n"+
      HelpVerboseMode()+
      "\n"
      "In order to compile a single expression you can use the -c"
      " option :\n"
      "     "+exeName+" -v -c\"Real n=1\"  \n"
      "Tambien puede usarse el analizador de expresiones simples"
      " mediante \n"
      "la opción de diálogo -d. En este caso el programa pedirá"
      " expresiones\n"
      "por el estándar input y mostrará el resultado por el"
      " estándar output\n"
      "hasta que la expresion sea la orden de salida $END.\n\n"
      )+"\n\n"+
      TOLContribAcknowledgements()+"\n\n"+
      TOLCredits()+"\n\n";
    printf("%s", helpText.String());
  }
  if(dialogMode)
  {
    InitGrammars();
    aliveObjects_1 = AliveObjects();
    if(initTOL) { LoadInitLibrary(argv[0]); }
    aliveObjects_2 = AliveObjects();
    BOut::PutAllTerm(BTRUE);
    Dialog();
    aliveObjects_2_ = AliveObjects();
    TestAliveObjects(aliveObjects_2,aliveObjects_2_,"TOL-dialog");
  }
/*
  DESTROY(stack);
  DestroyInitTOLFile();
  aliveObjects_1_ = AliveObjects();
  TestAliveObjects(aliveObjects_1,aliveObjects_1_,"TOL-dialog");
*/
 TOLHasBeenEnded_ = true;

/*
 BDat& EnableTraceMemory();
 EnableTraceMemory()=1;
*/
}

//--------------------------------------------------------------------
/* To force including gsl error routines
 */
const char * 
Tol_gsl_strerror (const int gsl_errno)
{
    return gsl_strerror(gsl_errno);
}

//--------------------------------------------------------------------
tgsl_error_handler * 
Tol_gsl_set_error_handler (tgsl_error_handler * new_handler)
{
    return gsl_set_error_handler(new_handler);
}

