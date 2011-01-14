/* datgra.cpp: Real Grammar's arithmetic functions
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

/*
 * OJO!!! este valor no debe redefinirse en ninguno de los .h que se incluyan
 * a continuacion.
 */
#define DEFAULT_INIT_PRIORITY 30

#include <tol/tol_bout.h>
#include <tol/tol_bqfile.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatfun.h>
#include <tol/tol_bvmat.h>


//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraLogic();
BBool ForzeLinkerDatGraStatistic();
BBool ForzeLinkerDatGraProbability();
BBool ForzeLinkerDatGraVarious();
BBool ForzeLinkerDatGraSeriesStatistic();

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("datgra.cpp");

static BBool forzeLinkers =
    ForzeLinkerDatGraLogic()	       &&
    ForzeLinkerDatGraSeriesStatistic() &&
    ForzeLinkerDatGraStatistic()       &&
    ForzeLinkerDatGraProbability()     &&
    ForzeLinkerDatGraVarious();

template<>
BGrammar* BGraContensBase<BDat>::ownGrammar_ = NIL;


DefineContensCommonOperators(BDat, "Real");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BDat>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}



//--------------------------------------------------------------------
class BDatTime: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatTime() : BSystemDat
  (
    "Time",0,
    I2("Returns the time elapsed in seconds from session started.",
       "Devuelve el tiempo transcurrido en segundos desde que se inició la sesión.")
  )
  {}
  BDat& Contens()
  {
		return(contens_=BSys::SessionTime());
  }
};

//--------------------------------------------------------------------
class BDatCpuClock: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatCpuClock() : BSystemDat
  (
    "CpuClock",0,
    I2("Returns the CPU usage elapsed in seconds from session started.",
       "Devuelve la CPU usada en segundos desde que se inició la sesión.")
  )
  {}
  BDat& Contens()
  {
		return(contens_=BTimer::ClockToMiliSecond(clock())/1000.0);
  }
};


//--------------------------------------------------------------------
class BDatNCore: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatNCore() : BSystemDat
  (
    "NCore",0,
    I2("The number of alives cores at this moment.",
       "El número de nucleos vivos en este momento.")
  )
  {}
  BDat& Contens() { return(contens_=BCore::NCore()); }
};


//--------------------------------------------------------------------
class BDatNObject: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatNObject() : BSystemDat
  (
    "NObject",0,
    I2("The number of alives objects at this moment.",
       "El número de objetos vivos en este momento.")
  )
  {}
  BDat& Contens()     
  { 
    return(contens_=AliveObjects()); 
  }
};

//--------------------------------------------------------------------
class BDatNError: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatNError() : BSystemDat
  (
    "NError",0,
    I2("The number of errors emited in this session."
       "You can use it to monitorize processes calculating the "
       "difference between the begin and end values of this variable",
       "El número de errores emitidos en la sesión."
       "Puede usarse para monitorizar procesos calculando la "
       "diferencia entre los valores de esta variable "
       "antes y después del proceso.")
  )
  {}
  BDat& Contens() { return(contens_=TOLErrorNumber()); }
};

//--------------------------------------------------------------------
class BDatNWarning: public BSystemDat
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BDatNWarning() : BSystemDat
  (
    "NWarning",0, 
    I2("The number of warnings emited in this session."
       "You can use it to monitorize processes calculating the "
       "difference between the begin and end values of this variable",
       "El número de avisos emitidos en la sesión."
       "Puede usarse para monitorizar procesos calculando la "
       "diferencia entre los valores de esta variable "
       "antes y después del proceso.")
  )
  {}
  BDat& Contens() { return(contens_=TOLWarningNumber()); }
};


//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BDat>::InitInstances()
{
  BTraceInit("BGraContens<BDat>::InitInstances");

  BDatTime*     timeDat_     = new BDatTime;
  BDatCpuClock* cpuClockDat_ = new BDatCpuClock;
  BDatNCore*    numCore_	   = new BDatNCore;
  BDatNObject*  numObject_   = new BDatNObject;
  BDatNError*   numError_    = new BDatNError;
  BDatNWarning* numWarning_  = new BDatNWarning;

  BSystemDat* unknown_ = new BSystemDat("?",	 BDat::Unknown(),
			     I2("Unknown value",
				"Valor desconocido"));
  BSystemDat* true1_  = new BSystemDat ("TRUE",	 BTRUE,
			     I2("Logic true value (1)",
				"Valor lógico de verdad (1)"));
  BSystemDat* true2_   = new BSystemDat("True",	 BTRUE,
			     I2("Logic true value (1)",
				"Valor lógico de verdad (1)"));
  BSystemDat* true3_   = new BSystemDat("true",	 BTRUE,
			     I2("Logic true value (1)",
				"Valor lógico de verdad (1)"));
  BSystemDat* false1_  = new BSystemDat("FALSE", BFALSE,
			     I2("Logic false value (0)",
				"Valor lógico de falsedad (0)"));
  BSystemDat* false2_  = new BSystemDat("False", BFALSE,
			     I2("Logic false value (0)",
				"Valor lógico de falsedad (0)"));
  BSystemDat* false3_  = new BSystemDat("false", BFALSE,
			     I2("Logic false value (0)",
				"Valor lógico de falsedad (0)"));
  BSystemDat* pi1_     = new BSystemDat("PI",	 BDat::Pi(),
			     I2("Pi number (3.141592654...)",
				"Número Pi (3.141592654...)"));
  BSystemDat* pi2_     = new BSystemDat("Pi",	 BDat::Pi(),
			     I2("Pi number (3.141592654...)",
				"Número Pi (3.141592654...)"));
  BSystemDat* pi3_     = new BSystemDat("pi",	 BDat::Pi(),
			     I2("Pi number (3.141592654...)",
				"Número Pi (3.141592654...)"));
  BSystemDat* e1_      = new BSystemDat("E",	 exp(1.0),
			     I2("Euler number (2.718281828...)",
				"Número e de Euler (2.718281828...)"));
  BSystemDat* e2_      = new BSystemDat("e",	 exp(1.0),
			     I2("Euler number (2.718281828...)",
				"Número e de Euler (2.718281828...)"));
  BSystemDat* inf_  = new BSystemDat ("Inf",	 BDat::PosInf(),
			     I2("Positive infinte",
				      "Infinito positivo"));

  BSystemDat* epsilon_  = new BSystemDat ("MachineEpsilon",	LDBL_EPSILON,
			     I2("Real double precission machine epsilon number. "
              "If you add a number smaller than this to 1.0, the result will be 1.0.",
				      "Número epsilon de la máquina para los reales de doble precisión. "
              "Si se añade a 1.0 un número máspequeño que éste, el resultado será 1.0."));

  BSystemDat* houseHolder_   = new BSystemDat
  (
    "HouseHolder",
    HOUSEHOLDER,
    I2("HouseHolder method identificator in Marquardt's least square method",
       "Identificador de método de Housholder en el método de minimizacion "
       "cuadrática de Marquardt") + "(MarqLinMet)"
  );
  BSystemDat* givens_ = new BSystemDat
  (
    "Givens",
    GIVENS,
    I2("Givens method identificator in Marquardt's least square method",
       "Identificador de método de Givens en el método de minimización "
       "cuadrática de Marquardt.")
  );
  BSystemDat* choleski_ = new BSystemDat
  (
    "Choleski",
    CHOLESKI,
    I2("Choleski method identificator in Marquardt's least square method",
       "Identificador de método de Choleski en el método de minimización "
       "cuadrática de Marquardt.")
  );

#ifdef __USE_DBTIME_MONITOR__
  BDat& DBTotalCPUTime();
  BParamDat* dbTotalCPUTime_ = new BParamDat
  (
   "DBTotalCPUTime",
   DBTotalCPUTime(),
   I2("Total cumulative CPU time used in all database accesses.",
      "Tiempo total de CPU acumulado en todos los accesos a bases de datos.")
  );
#endif

#ifdef TRACE_MEMORY
  BDat& EnableTraceMemory();
  BParamDat* EnableTraceMemory_  = new BParamDat
  (
    "EnableTraceMemory",
    EnableTraceMemory(),
    "Enables/Disables debuging memory traces"
  );
#endif



  BDat mxlen(BQFile::MaxFieldLength());
  BParamDat* bdbMaxFieldLength_	 = new BParamDat
  (
    "BDBMaxFieldLength",
    mxlen,
    I2("The maximum length of fields for BDB files",
       "Longitud máxima de los campos en ficheros BDB")
  );

  BDat& StrassenCutoff();
  BParamDat* StrassenCutoff_  = new BParamDat
  (
    "StrassenCutoff",
    StrassenCutoff(),
    I2("The minimum size to use the Strassen's algorithm for fast matrix "
       "multiplication.",
       "Tamaño mínimo para utilizar el algoritmo de Strassen para la"
       "múltiplicación rápida de matrices.")
  );
  BParamDat* marquardtLinearMethod_  = new BParamDat
  (
    "MarqLinMet",
    MarquardtLinearMethod(),
    I2("The linear method used in each iteration of Marquardt method"
       "(Householder or Givens)",
       "Método lineal usado en cada iteración del método de Marquardt"
       "(Householder o Givens)")
  );
  BParamDat* marquardtFactor_  = new BParamDat
  (
    "MarqFactor",
    MarquardtFactor(),
    I2("Lambda parameter factor in Marquardt's least square method",
       "Parámetro de factor lambda en el método de minimización "
       "cuadrática de Marquardt")
  );
  BParamDat* relTolerance_ = new BParamDat
  (
    "RelativeTolerance",
    BDat::RelTolerance(),
    I2("Relative tolerance for iterative numeric methods",
       "Tolerancia relativa para métodos numéricos iterativos")
  );
  BParamDat* tolerance_ = new BParamDat
  (
    "Tolerance",
    BDat::Tolerance(),
    I2("Tolerance for iterative numeric methods",
       "Tolerancia para métodos numéricos iterativos")
  );
  BParamDat* maxIter_ = new BParamDat
  (
    "MaxIter",
    BDat::MaxIter(),
    I2("Maxim of iterations for iterative numeric methods",
       "Máximo de iteraciones para métodos numéricos iterativos")
  );
  BParamDat* distance_ = new BParamDat
  (
    "DiffDist",
    Distance(),
    I2("Distance of points for numeric differential aproximations",
       "Distancia de puntos para aproximaciones diferenciales numericas")
  );
  BDat& CGMaxIter();
  BParamDat* CGMaxIter_ = new BParamDat
  (
    "CGMaxIter",
    CGMaxIter(),
    I2("Maximum number of Conjugate Gradient method to run in each non linear "
       "least squares pass when the Marquardt iteration doesn't get any advance "
       "over the tolerance. To avoid CG method use 0.",
       "Número máximo de iteraciones del método del Gradiente Conjugado que "
       "se realizan en cada paso del método no lineal de mínimos cuadrados "
       "cuando las iteraciones de Marquardt no producen un avance mayor que "
       "la tolerancia. "
       "Si no se quiere utilizar el método CG hay que poner esta variable a 0.")
  );
  BParamDat* formatBDT_ = new BParamDat
  (
    "FormatBDT",
    BSerieTable::FormatBDT(),
    I2("Switch between formating and unformating BDT files",
       "Conmuta entre ficheros BDT formateados o no")
  );
  BParamDat* BDTFillValue_ = new BParamDat
  (
    "BDTFillValue",
    BSetIncludeBDT::FillValue(),
    I2("Real value to fill the data omitted in BDT files.",
       "Valor real para rellenar los datos omitidos en ficheros BDT.")
  );

  BDat& NullInitResiduals ();
  BParamDat* nullInitResiduals_ = new BParamDat
  (
    "NullInitResiduals",
    NullInitResiduals(),
    I2("ARIMA Model Estimate function will take null init residuals if this "
       "variable is TRUE.",
       "La función Estimate de modelos ARIMA tomará residuos iniciales nulos "
       "si esta variable es TRUE.")
  );

  BDat& MinOutlierLikelyhood ();
  BParamDat* minOutlierLikelyhood_ = new BParamDat
  (
    "MinOutlierLikelyhood",
    MinOutlierLikelyhood(),
    I2("The minimal agreeable value for oulier's likelihood reason ",
       "El mínimo valor admisible para la razon de verosimilitud de "
       "'outliers'") + "(t-Student)"
  );

  BDat& DoDiagnostics ();
  BParamDat* doDiagnostics_ = new BParamDat
  (
    "DoDiagnostics",
    DoDiagnostics(),
    I2("ARIMA Model Estimate function will do model diagnostics if this "
       "variable is TRUE.",
       "La función Estimate de modelos ARIMA realizará los diagnósticos de "
       " los modelos si esta variable es TRUE.")
  );

  BDat& DoStatistics ();
  BParamDat* doSatistics_ = new BParamDat
  (
    "DoStatistics",
    DoStatistics(),
    I2("ARIMA Model Estimate function will do model statistics if this "
       "variable is TRUE.",
       "La función Estimate de modelos ARIMA realizará las estadísticas de "
       " los modelos si esta variable es TRUE.")
  );

  BDat& DoKernel ();
  BParamDat* doKernel_ = new BParamDat
  (
    "DoKernel",
    DoKernel(),
    I2("ARIMA Model Estimate function will do model kernel analysis if this "
       "variable is TRUE and DoStatistics is TRUE.",
       "La función Estimate de modelos ARIMA realizará el análisis del kernel"
       " de los modelos si esta variable es TRUE y DoStatistics también.")
  );

  BParamDat* doUnparseNodes_ = new BParamDat
  (
    "DoUnparseNodes",
    BGrammar::DoUnparseNodes(),
    I2("Description Function returns Tol sentence applyied in creation time"
       " of this object, only if this variable is not zero. If it is 1 only full "
       "expressions will be set. If is is 2 or more subexpressions will be "
       "unparsed recursively to all nodes of evaluation tree. This feature "
       "is usefull to debugging",
       "La función Description aplicada a un objeto devuelve la sentencia "
       " Tol asignada en su creación solo si esta variable es no es cero."
       "Si el valor es 1 sólo las expresiones completas se expandirán y si es "
       "2 ó más también lo harán las subexpresiones para todos los nodos del "
       "árbol de evaluación. Esta opción es útil para depuración.")
  );

  BParamDat* globalizeSeries_ = new BParamDat
  (
    "GlobalizeSeries",
    BTimeSerie::GlobalizeSeries(),
    I2("By default 1, exports all Series created by DBSeriesXXX functions, "
       "and IncludeBDT, to Global Scope.",
       "Por defecto toma valor 1, exportando todas las Series creadas por "
       "las funciones DBSeriesXXX, e IncludeBDT,  al ámbito global.")
  );

  BDat& DBRTrim();
  BParamDat* DBRTrim_ = new BParamDat
  (
    "DBRTrim",
    DBRTrim(),
    I2("All Text fields are right-trimed on all DDBB operations only if"
       " this variable is TRUE.",
       "Los espacios a la derecha de todos los campos de texto leidos"
       " de bases de datos serán eliminados si esta variable es TRUE")
  );

  BParamDat* VMatAutoConvert_	 = new BParamDat
  (
    "VMatAutoConvert",
    BVMat::AutoConvert(),
    I2("If true VMatrix will try to convert between operands subtypes "
       "in order to perform not allowed operations with current subtypes",
       "Si es cierto, los operadores de VMatrix intentarán convertir "
       "los subtipos de lso operandos para poder ejecutar operaciones "
       "que no sean posibles con los actuales subipos.")
  );

  OwnGrammar()->PutDefect(unknown_);

}



//--------------------------------------------------------------------
/*! Returns a valid constant BDat for name.
 */
template<>
BSyntaxObject* BGraContensBase<BDat>::FindConstant(const BText& name)
{
    const char* expr = name.String();
    while(isspace(expr[0])) { expr++; }
    BDat dat;
    dat.PutValue(expr);
    BUserDat* userDat = NIL;
    
    if(dat.IsKnown()||(expr==BText("?"))) { userDat = new BContensDat(dat); }
  //else { Error(expr+" "+I2("is not a valid constant number","no es un número constante válido")); }
    return((BSyntaxObject*)userDat);
}


//--------------------------------------------------------------------
/*! Returns a valid real for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BDat>::Casting(BSyntaxObject* obj)
{
    if(!obj) 			     { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BDat>::New(FILE* fil)
{
    return 0;
}


//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIdentity);
  DefIntOpr(1, BDatIdentity, " + ", 1, 1,
  "(Real x)",
  I2("Returns the same real number.",
     "Devuelve el mismo número real."),
  BOperClassify::RealArythmetic_);
  void BDatIdentity::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMinus);
  DefIntOpr(1, BDatMinus, " - ", 1, 1,
  "(Real x)",
  I2("Returns the real number width the oposite sign.",
     "Devuelve el argumento con el signo contrario."),
  BOperClassify::RealArythmetic_);
  void BDatMinus::CalcContens()
//--------------------------------------------------------------------
{ contens_ = -Dat(Arg(1)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSign);
  DefIntOpr(1, BDatSign, "Sign", 1, 1,
  "(Real x)",
  I2("Returns - 1, 0, or 1 according to will be x less than zero, "
     "just as zero, or greater than zero.",
     "Devuelve -1, 0, o 1 según sea x menor que cero, igual que cero, "
     "o mayor que cero."),
  BOperClassify::RealArythmetic_);
  void BDatSign::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Sign(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAbs);
  DefIntOpr(1, BDatAbs, "Abs", 1, 1,
  "(Real x)",
  I2("Returns the absolute value of x.",
     "Devuelve el valor absoluto de x."),
  BOperClassify::RealArythmetic_);
  void BDatAbs::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Abs(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRound);
  DefIntOpr(1, BDatRound, "Round", 1, 2,
  "(Real x [, Real ndecs])",
  I2("Returns the closest integer to x with (optional) ndecs digits in "
     "decimal part."
     "If the decimal part is exactly .5 is rounded by down.",
     "Devuelve el número entero más cercano a x con (opcionalmente) ndecs "
     "número de digitos en la parte decimal."
     "Si la parte decimal es exactamente .5 se redondea por abajo"),
  BOperClassify::RealArythmetic_);
  void BDatRound::CalcContens()
//--------------------------------------------------------------------
{ 
    if(Arg(2)) {
	contens_ = RoundD(Dat(Arg(1)), Dat(Arg(2)));
    } else {
	contens_ = Round(Dat(Arg(1))); 
    }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFloor);
  DefIntOpr(1, BDatFloor, "Floor", 1, 1,
  "(Real x)",
  I2("Returns the greater integer not great than x.",
     "Devuelve el mayor entero menor o igual que x."),
  BOperClassify::RealArythmetic_);
  void BDatFloor::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Floor(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSqrt);
  DefIntOpr(1, BDatSqrt, "SqRt", 1, 1,
  "(Real x)",
  I2("Returns the square root of x.",
     "Devuelve la raíz cuadrada de x."),
  BOperClassify::RealArythmetic_);
  void BDatSqrt::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Sqrt(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLog10);
  DefIntOpr(1, BDatLog10, "Log10", 1, 1,
  "(Real x)",
  I2("Returns the decimal logarithm of x.",
     "Devuelve el logaritmo decimal de x."),
  BOperClassify::RealArythmetic_);
  void BDatLog10::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Log10(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLog);
  DefIntOpr(1, BDatLog, "Log", 1, 1,
  "(Real x)",
  I2("Returns the natural logarithm of x.",
     "Devuelve el logaritmo natural de x."),
  BOperClassify::RealArythmetic_);
  void BDatLog::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Log(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLogBase);
  DefIntOpr(1, BDatLogBase, "LogBase", 2, 2,
  "(Real x, Real b)",
  I2("Returns the b-based logarithm of x.",
     "Devuelve el logaritmo en base b de x."),
  BOperClassify::RealArythmetic_);
  void BDatLogBase::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Log(Dat(Arg(1)))/Log(Dat(Arg(2))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatExp);
  DefIntOpr(1, BDatExp, "Exp", 1, 1,
  "(Real x)",
  I2("Returns the exponential of x.",
     "Devuelve la exponencial de x."),
  BOperClassify::RealArythmetic_);
  void BDatExp::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Exp(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatRadToDeg);
  DefIntOpr(1, BDatRadToDeg, "RadToDeg", 1, 1,
  "(Real x)",
  I2("Converts radians to degrees.",
     "Convierte radianes en grados."),
  BOperClassify::RealArythmetic_);
  void BDatRadToDeg::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) * BDat(180) / BDat::Pi(); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDegToRad);
  DefIntOpr(1, BDatDegToRad, "DegToRad", 1, 1,
  "(Real x)",
  I2("Converts degrees to radians.",
     "Convierte grados en radianes."),
  BOperClassify::RealArythmetic_);
  void BDatDegToRad::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) * BDat::Pi() / BDat(180); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSin);
  DefIntOpr(1, BDatSin, "Sin", 1, 1,
  "(Real x)",
  I2("Returns the trigonometric sine of x,which must be given in radians.",
     "Devuelve el seno trigonométrico de un ángulo x medido en radianes."),
  BOperClassify::RealArythmetic_);
  void BDatSin::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Sin(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCos);
  DefIntOpr(1, BDatCos, "Cos", 1, 1,
  "(Real x)",
  I2("Returns the trigonometric cosine of x,which must be given in radians.",
     "Devuelve el coseno trigonométrico de un ángulo x medido en radianes."),
  BOperClassify::RealArythmetic_);
  void BDatCos::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Cos(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTan);
  DefIntOpr(1, BDatTan, "Tan", 1, 1,
  "(Real x)",
  I2("Returns the trigonometric tangent of x,which must be given in radians.",
     "Devuelve la tangente trigonométrica de un ángulo x medido en radianes."),
  BOperClassify::RealArythmetic_);
  void BDatTan::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Tan(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatASin);
  DefIntOpr(1, BDatASin, "ASin", 1, 1,
  "(Real x)",
  I2("Returns, in radians, the angle wich trigonometric sine is x.",
     "Devuelve, en radianes, el ángulo cuyo seno trigonométrico es x."),
  BOperClassify::RealArythmetic_);
  void BDatASin::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ASin(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatACos);
  DefIntOpr(1, BDatACos, "ACos", 1, 1,
  "(Real x)",
  I2("Returns, in radians, the angle wich trigonometric cosine is x.",
     "Devuelve, en radianes, el ángulo cuyo coseno trigonométrico es x."),
  BOperClassify::RealArythmetic_);
  void BDatACos::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ACos(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatATan);
  DefIntOpr(1, BDatATan, "ATan", 1, 1,
  "(Real x)",
  I2("Returns, in radians, the angle wich trigonometric tangent is x.",
     "Devuelve, en radianes, el ángulo cuya tangente trigonométrica es x."),
  BOperClassify::RealArythmetic_);
  void BDatATan::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ATan(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatASinH);
  DefIntOpr(1, BDatASinH, "ASinH", 1, 1,
  "(Real x)",
  I2("Returns the number wich hyperbolic sine is x.",
     "Devuelve el número cuyo seno hiperbólico es x."),
  BOperClassify::RealArythmetic_);
  void BDatASinH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ASinH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatACosH);
  DefIntOpr(1, BDatACosH, "ACosH", 1, 1,
  "(Real x)",
  I2("Returns the number wich hyperbolic cosine is x.",
     "Devuelve el número cuyo coseno hiperbólico es x."),
  BOperClassify::RealArythmetic_);
  void BDatACosH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ACosH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatATanH);
  DefIntOpr(1, BDatATanH, "ATanH", 1, 1,
  "(Real x)",
  I2("Returns the number wich hyperbolic tangent is x.",
     "Devuelve el número cuya tangente hiperbólica es x."),
  BOperClassify::RealArythmetic_);
  void BDatATanH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = ATanH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSinH);
  DefIntOpr(1, BDatSinH, "SinH", 1, 1,
  "(Real x)",
  I2("Returns the hyperbolic sine of x.",
     "Devuelve el seno hiperbólico de x."),
  BOperClassify::RealArythmetic_);
  void BDatSinH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = SinH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCosH);
  DefIntOpr(1, BDatCosH, "CosH", 1, 1,
  "(Real x)",
  I2("Returns the hyperbolic cosine of x.",
     "Devuelve el coseno hiperbólico de x."),
  BOperClassify::RealArythmetic_);
  void BDatCosH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = CosH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatTanH);
  DefIntOpr(1, BDatTanH, "TanH", 1, 1,
  "(Real x)",
  I2("Returns the hyperbolic tangent of x.",
     "Devuelve la tangente hiperbólica de x."),
  BOperClassify::RealArythmetic_);
  void BDatTanH::CalcContens()
//--------------------------------------------------------------------
{ contens_ = TanH(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLogGamma);
  DefIntOpr(1, BDatLogGamma, "LogGamma", 1, 1,
  "(Real p)",
  I2("Returns the logarythm of the Euler's Gamma function of p>0.",
     "Devuelve el logaritmo de la función Gamma de Euler de p>0."),
  BOperClassify::RealArythmetic_);
  void BDatLogGamma::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogGamma(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLogBeta);
  DefIntOpr(1, BDatLogBeta, "LogBeta", 2, 2,
  "(Real p, Real q)",
  I2("Returns the logarythm of the Euler's Beta function of p,q>0.",
     "Devuelve el logaritmo de la función Beta de Euler de p,q>0."),
  BOperClassify::RealArythmetic_);
  void BDatLogBeta::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogBeta(Dat(Arg(1)),Dat(Arg(2))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGamma);
  DefIntOpr(1, BDatGamma, "Gamma", 1, 1,
  "(Real p)",
  I2("Returns the Euler's Gamma function of p>0.",
     "Devuelve la función Gamma de Euler de p>0."),
  BOperClassify::RealArythmetic_);
  void BDatGamma::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Gamma(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBeta);
  DefIntOpr(1, BDatBeta, "Beta", 2, 2,
  "(Real p, Real q)",
  I2("Returns the Euler's Beta function of p,q>0.",
     "Devuelve la función Beta de Euler de p,q>0."),
  BOperClassify::RealArythmetic_);
  void BDatBeta::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Beta(Dat(Arg(1)),Dat(Arg(2))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatFactorial);
  DefIntOpr(1, BDatFactorial, "Factorial", 1, 1,
  "(Real n)",
  I2("Returns the factorial of the natural number n. ",
     "Devuelve el factorial del número natural n. ")+
     "n! = n*(n-1)*(n-2)*...*1",
  BOperClassify::RealArythmetic_);
  void BDatFactorial::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Factorial((BInt)Real(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatInvFactorial);
  DefIntOpr(1, BDatInvFactorial, "InvFactorial", 1, 1,
  "(Real n)",
  I2("Returns the factorial of the natural number n. ",
     "Devuelve el factorial del número natural n. ")+
     "n! = n*(n-1)*(n-2)*...*1",
  BOperClassify::RealArythmetic_);
  void BDatInvFactorial::CalcContens()
//--------------------------------------------------------------------
{ contens_ = InvFactorial(Dat(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatBiFactorial);
  DefIntOpr(1, BDatBiFactorial, "BiFactorial", 1, 1,
  "(Real n)",
  I2("Returns the bifactorial of the natural number n ",
     "Devuelve el bifactorial del número natural n ")+
     "n!! = n*(n-2)*(n-4)*...*1",
  BOperClassify::RealArythmetic_);
  void BDatBiFactorial::CalcContens()
//--------------------------------------------------------------------
{ contens_ = BiFactorial((BInt)Real(Arg(1))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCombinat);
  DefIntOpr(1, BDatCombinat, "Comb", 2, 2,
  "(Real n, Real m)",
  I2("Returns the m by m combinations of n elements.",
     "Devuelve las combinaciones de n elementos tomados de m en m."),
  BOperClassify::RealArythmetic_);
  void BDatCombinat::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Combinat((BInt)Real(Arg(1)),(BInt)Real(Arg(2))); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatSum2);
  DefIntOpr(1, BDatSum2, "+", 2, 2,
  "x1 + x2 {Real x1, Real x2}",
  I2("Returns the summe of both real numbers.",
     "Devuelve la suma de ambos números reales."),
  BOperClassify::RealArythmetic_);
  void BDatSum2::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) + Dat(Arg(2)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatDifference);
  DefIntOpr(1, BDatDifference, "-", 2, 2,
  "x1 - x2 {Real x1, Real x2}",
  I2("Returns the difference between both real numbers.",
     "Devuelve la diferencia entre ambos números reales."),
  BOperClassify::RealArythmetic_);
  void BDatDifference::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) - Dat(Arg(2)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatProduct2);
  DefIntOpr(1, BDatProduct2, "*", 2, 2,
  "x1 * x2 {Real x1, Real x2}",
  I2("Returns the product of both real numbers.",
     "Devuelve el producto de ambos números reales."),
  BOperClassify::RealArythmetic_);
  void BDatProduct2::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) * Dat(Arg(2)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatQuotient);
  DefIntOpr(1, BDatQuotient, "/", 2, 2,
  "x1 / x2 {Real x1, Real x2}",
  I2("Returns the quotient of both real numbers.",
     "Devuelve el cociente entre ambos números reales."),
  BOperClassify::RealArythmetic_);
  void BDatQuotient::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) / Dat(Arg(2)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatPow);
  DefIntOpr(2, BDatPow, "^", 2, 2,
  "x1 ^ x2 {Real x1, Real x2}",
  I2("Returns the power of x1 increased to x2.",
     "Devuelve la potencia de x1 elevado a x2.")+
  I2("Also it can be used the operator ** or the function Pow.",
     "También se puede utilizar el operador ** o la función Pow."),
  BOperClassify::RealArythmetic_);
  void BDatPow::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)) ^ Dat(Arg(2)); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatMod);
  DefIntOpr(1, BDatMod, "%", 2, 2,
  "x1 % x2 {Real x1, Real x2}",
  I2("Returns x1 modulo x2.",
     "Devuelve x1 modulo x2."),
  BOperClassify::RealArythmetic_);
  void BDatMod::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Mod(Dat(Arg(1)),Dat(Arg(2))); }



