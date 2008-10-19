/* mc-integrator.cpp : Monte Carlo Integration.
                       GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
   Author: Jorge Suit Perez Ronda <jsperez@bayesinf.com>

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

/*!
  \file   mc-integrator.cpp
  \author Jorge Suit Perez Ronda
  \date   Tue Feb 18 18:05:25 2003
  
  \brief  Monte Carlo Integration
  
  This file is implemented the TOL interface with the Monte Carlos
  intergrator methods. The methods are: plain, miser and vegas.  The
  interface is implemented in the Set grammar.
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcodgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>

BTraceInit("mc-integrator.cpp");

/*
Real Q(Matrix X) { Real 2 * MatDat(X, 1, 1) }
Set Integral = MonteCarloMiser(Q,1,Col(0), Col(2), 10);
*/
static int __InitGSLRGN__()
{
  gsl_rng_env_setup ();
  return 1;
}

static int  __rgn_setup__ = __InitGSLRGN__();

//! BMonteCarlo class
/*!  This is a base class for the three classes implementing the Monte
  Carlos integration methods: Plain (\a BMonteCarloPlain), MISER (\a
  BMonteCarloMiser) and VEGAS.
*/

class BMonteCarlo :  public BSetTemporary {
public:
  BMonteCarlo(const char * MClass, BList * args) : BSetTemporary(args) 
  {
    Initialize();
    flags_.calculated_ = BFALSE;
    m_funcalls = 100000;
    if (!mc_UX->NRefs())
      mc_UX->IncNRefs();
  //PutName(MClass);
    CheckArguments();
  }

  static bool Initialize()
  {
    /* Alloc the matrix for the evaluations */
    if (!mc_UX) 
    {
      mc_UX = BContensMat::New("", BMatrix<BDat>(),"UX");
      return(true);
    }
    else
    {
      return(false);
    }
  }

  virtual void CalcContens();

  //! Main evaluator called by Monte Carlo integrator
  /*! This is a static member passed to the gsl_monte_plain_integrate
    routine.
    
  \param x Array with the current point in the domain to be evaluated
  \param dim Dimension of the domain, it is the size of x
  \param params Not used.
  
  \return The result of the evaluation.
  */
  static double FunctionEval(double * x, size_t dim, void * params);
  
  static BCode * CurrentFunction; /*!< Pointer to the user code to evaluate in FunctionEval  */
  static gsl_rng * rgn;         /*!< Pointer to random number generator */
  static BUserMat * mc_UX;      /*!<  \a BSyntaxObject used to evaluate \a CurrentFunction */
  static int nfuncall;

protected:
  //! Check the arguments passed by the user
  /*! Verify for the validity of every argument: Arg(1) = function
  //  must be a user code returning a Real value and with one matrix
  //  arguments, Arg(2) = dim must be a positive number specifying the
  //  dimension of the integration domain, Arg(3) = xlower and Arg(4)
  //  = xupper must be (dim,1) Matrix arguments. If the arguments are
  //  well specified the member \a m_ArgsOK is set to BTRUE, in other
  //  case is set to BFALSE.
    
  \return The value of \a m_ArgsOK.
  */
  BBool CheckArguments(); 
  virtual int DoMonteCarlo(gsl_monte_function * G, double * hiper[], int dim, double * result, double * sigma) = 0;
  virtual BList * AddExtraParameters(BList *);
  BBool m_ArgsOk;               /*!< Status of the arguments */
  int m_funcalls;               /*!< Number of functions calls to sample */
};

BCode *       BMonteCarlo::CurrentFunction = NULL;
gsl_rng *     BMonteCarlo::rgn = gsl_rng_alloc (gsl_rng_default);
BUserMat *    BMonteCarlo::mc_UX = NULL; 
int           BMonteCarlo::nfuncall = 0;

void BMonteCarlo::CalcContens()
{
  BList * lst = NIL, * aux = NIL;
  BUserDat * integral, * sigma;
  if ( flags_.calculated_ )
    return;
  if (m_ArgsOk) {
    BText english("Starting integration: ");
    BText spanish("Comenzando integración: ");
    english += Name();
    english += "\n";
    spanish += Name();
    spanish += "\n";
    Std(I2(english,spanish));
    /* do integration */
    unsigned int i, r;
    
    size_t dim = (size_t)((((BUserDat*)Arg(2))->Contens()).Value());
    double res, err;
    double * hiper[2], * x;
    BMatrix<BDat> * X;
    BDat * datarr;

    CurrentFunction = &Code(Arg(1));
    /* copy the hipercube */
    for (i=0; i < 2; i++) {
      X = &(((BUserMat*)Arg(i+3))->Contens());
      datarr = X->GetData().GetBuffer();
      hiper[i] = x = new double[dim];
      for (r = 0; r < dim; r++) {
        if (datarr[r].Known())
          x[r] = datarr[r].Value();
        else
          /* what should we do here ?*/
          x[r] = 0.0;
      } 
    }
    mc_UX->Contens().Alloc(dim,1);
    gsl_monte_function G = { &BMonteCarlo::FunctionEval, dim, NULL };

    /* here we can check for the returned value from DoMonteCarlo */

    DoMonteCarlo(&G, hiper, dim, &res, &err);  
    
    for (i=0; i < 2; i++)
      delete [](hiper[i]);
    integral = BContensDat::New("value", BDat(res),
			    I2("Integral result","Resultado de la integral"));
    sigma = BContensDat::New("sigma", BDat(err),
                         I2("Error estimate","Error estimado"));
  } else {
    /* errors in arguments */
    /* build user data for result and error */
    integral = BContensDat::New("value", BDat(0.0),
			    I2("Integral result","Resultado de la integral"));
    sigma = BContensDat::New("sigma", BDat(0.0),
			 I2("Error estimate","Error estimado"));
  }
  lst = Cons(integral,Cons(sigma,NIL));
  lst = AddExtraParameters(lst);
  
  contens_.RobElement(lst);
  /* now I'm calculated */
  flags_.calculated_ = BTRUE;
  Std(I2("Done integration\n","Integración finalizada\n"));
}

BList * BMonteCarlo::AddExtraParameters(BList * lst)
{
  return lst;
}

double BMonteCarlo::FunctionEval(double * x, size_t dim, void * params)
{
  unsigned int r;
  BList * args;
  BSyntaxObject * eval_res;
  BDat * dat_res;
  static double last_res = 0.0;
  static BBool bUserError = BFALSE;
  double dbl_res;
  BDat * mc_XBuffer = mc_UX->Contens().GetData().GetBuffer();

  /* if there was an error in the user function evaluation
     return always the las result.
   */

  if (bUserError)
    return last_res;

  int nref = mc_UX->NRefs();
  ++nfuncall;
  if (!(nfuncall % 50000)) {
    Std(I2("Don't desperate, still integrating ...\n",
	   "No desespere, integrando todavía ...\n"));
  }
  for (r = 0; r < dim; r++) {
    mc_XBuffer[r].PutValue(x[r]);
  }
  eval_res = CurrentFunction->Evaluator(args=NCons(mc_UX));
  /* verify if eval just failed */
  if (eval_res) {
    dat_res = &(((BUserDat*)eval_res)->Contens());
    if (dat_res->Known()) {
      dbl_res = dat_res->Value();
    } else {
      BText msg_en("In MonteCarlo, unknown value in evaluation of ");
      BText msg_sp("En MonteCarlo, valor desconocido en la evaluación de ");
      msg_en.Concat(CurrentFunction->Name());
      msg_sp.Concat(CurrentFunction->Name());
      Warning(I2(msg_en, msg_sp));
    }
    DESTROY(eval_res);
  } else {
    /* there was an error evaluating the user function,
     assume the last evaluation */
    bUserError = BFALSE;
    dbl_res = last_res;
  }
  return last_res = dbl_res;
}

BBool BMonteCarlo::CheckArguments()
{
  BText english("In ");
  BText spanish("En ");

  english += Name();
  spanish += Name();
  english += ", ";
  spanish += ", ";

  /* verify arguments */

  m_ArgsOk = BFALSE;

  BCode * cod = &Code(Arg(1));
  if (!cod->Operator()) {
    english += "invalid code argument";
    spanish += "argumento código inválido";
    Error(I2(english,spanish));
  } else if (cod->Grammar() != GraReal()) {
    english += "invalid type ";
    english += cod->Grammar()->Name();
    english += " for code argument: should be Real";
    spanish += " tipo inválido ";
    spanish += cod->Grammar()->Name();
    spanish += " para argumento código: debe ser Real";
    Error(I2(english,spanish));
  } else if (cod->Operator()->MinArg()!=1 && 
             cod->Operator()->MaxArg()!=1) {
    english += "invalid argument number for code argument: should be 1";
    spanish += "número de argumentos inválidos para argumento código: debe ser 1";
    Error(I2(english,spanish));  
  } else if (cod->Operator()->GrammarForArg(1) != GraMatrix()) {
    english += "invalid type ";
    english += cod->Operator()->GrammarForArg(1)->Name();
    english += " for first argument of code argument: should be Matrix";
    spanish += "tipo inválido ";
    spanish += cod->Operator()->GrammarForArg(1)->Name();
    spanish += " en el primer argumento del argumento código: debe ser Matrix";
    Error(I2(english,spanish));
  } else {
    BDat * dim = &(((BUserDat*)Arg(2))->Contens()); 
    if (!dim->Known() || dim->Value() < 0) {
      english += "invalid dimension argument: should be known and positive";
      spanish += "argumento dimensión inválido: debe ser conocido y positivo";
      Error(I2(english,spanish));      
    } else {
      BMatrix<BDat> * X;
      BText arg_names[] = {BText("xlower"),
                           BText("ylower")};
      int i;
      for (i = 0; i < 2; i++) { 
        X = &(((BUserMat*)Arg(i+3))->Contens());
        if (X->Rows() != dim->Value() || X->Columns() != 1) {
          /* english message */
          english += "invalid size for matrix ";
          english += arg_names[i];
          english += ": must be only one column of size equal to dim argument";
          /* spanish message */                  
          spanish += "tamaño inválido en la matriz ";
          spanish += arg_names[i];
          spanish += ": debe tener solo una columna de tamaño igual al argumento dim";
          Error(I2(english, spanish));
          break;
        }
      }
      if (i == 2) {
        if (NumArgs() > 4) {
          int fcalls;
          BDat * dat_calls = &(((BUserDat*)Arg(5))->Contens());
          if (dat_calls->Known() && (fcalls = int(dat_calls->Value()))>1) {
            m_funcalls = fcalls;
          }
          else {
            english += "invalid number of function calls: must be known and greater than 1. Defaults to ";
            english += m_funcalls;
            spanish += "número de llamadas a función inválido: debe ser conocido y mayor que 1. Se asume ";
            spanish += m_funcalls;
            Warning(I2(english,spanish));
          }
        }
        m_ArgsOk = BTRUE;
      }
    }
  }
  return m_ArgsOk;
}


//! BMonteCarloPlain class: Plain method.
/*! BMonteCarloPlain class implement the interface for computing a
  Monte Carlo integral following the clasic method.
*/

class BMonteCarloPlain : public BMonteCarlo
{
public:
    BMonteCarloPlain(BList * args)
	: BMonteCarlo("MonteCarloPlain", args) 
	{
	}
protected:
    virtual int DoMonteCarlo(gsl_monte_function * G, 
			     double * hiper[], 
			     int dim, 
			     double * result, 
			     double * sigma);
};

DeclareEvaluator(BMonteCarloPlain);
DefExtOpr(1, BMonteCarloPlain, "MonteCarloPlain", 4, 5, "Code Real Matrix Matrix Real",
          "(Code function, Real dim, Matrix xlower, Matrix xupper [, Real calls])", 
          I2("Monte Carlo Integration: plain method. Integrate by uniform random sampling.\n\n"
	     "Arguments:\n\n"
	     "    function --> user function to integrate. Should be declared as\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim      --> domain dimension.\n"
	     "    xlower   --> \n"
	     "    xupper   --> both are column matrix of size dim and define the integration volumen\n"
	     "    calls    --> number of function calls to evaluate. It defaults to 100000.\n\n"
	     "Example:\n\n"
	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloPlain(UserFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 100000);"
	     ,
             "Integración de Monte Carlo: método clásico. Integra mediante muestreo aleatorio uniforme\n\n"
	     "Argumentos:\n\n"
	     "    function --> función de usuario a integrar. Debe declararse como\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim      --> dimensión del dominio.\n"
	     "    xlower   --> \n"
	     "    xupper   --> ambos son matrices columnas de tamaño dim y definen el volumen the integración\n"
	     "    calls    --> número de evaluaciones de la función a ejecutar. Por omisión se asume 100000.\n\n"
	     "Ejemplo:\n\n"
	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloPlain(UserFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 100000);"
	     ),
          BOperClassify::NumericalAnalysis_);


int BMonteCarloPlain::DoMonteCarlo(gsl_monte_function * G, 
				   double * hiper[], 
				   int dim, 
				   double * result, 
				   double * sigma)
{
    int status;
    
    nfuncall = 0;
    gsl_monte_plain_state *s = gsl_monte_plain_alloc(dim);
    status = gsl_monte_plain_integrate (G, hiper[0], hiper[1], dim, 
					m_funcalls, rgn, s, result, sigma);
    gsl_monte_plain_free (s);
    return status;
}

//! BMonteCarloMiser class: MISER method
/*! BMonteCarloMiser class implement the interface for computing a
  Monte Carlo integral following the MISER method.
*/

class BMonteCarloMiser : public BMonteCarlo
{
public:
  BMonteCarloMiser(BList * args)
    : BMonteCarlo("MonteCarloMiser", args) {
  }

protected:
  virtual int DoMonteCarlo(gsl_monte_function * G, double * hiper[], int dim, double * result, double * sigma);

private:
};

DeclareEvaluator(BMonteCarloMiser);
DefExtOpr(1, BMonteCarloMiser, "MonteCarloMiser", 4, 5, "Code Real Matrix Matrix Real",
          "(Code function, Real dim, Matrix xlower, Matrix xupper [, Real calls])", 
          I2("Monte Carlo Integration: MISER method. Integrate by recursive stratified sampling.\n\n"
	     "Arguments:\n\n"
	     "    function --> user function to integrate. Should be declared as\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim      --> domain dimension.\n"
	     "    xlower   --> \n"
	     "    xupper   --> both are column matrix of size dim and define the integration volumen\n"
	     "    calls    --> number of function calls to evaluate. It defaults to 100000.\n\n"
	     "Example:\n\n"
	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloMiser(UserFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 100000);"
	     ,
             "Integración de Monte Carlo: método MISER. Integra mediante muestreo recursivo estratificado.\n\n"
	     "Argumentos:\n\n"
	     "    function --> función de usuario a integrar. Debe declararse como\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim      --> dimensión del dominio.\n"
	     "    xlower   --> \n"
	     "    xupper   --> ambos son matrices columnas de tamaño dim y definen el volumen the integración\n"
	     "    calls    --> número de evaluaciones de la función a ejecutar. Por omisión se asume 100000.\n\n"
	     "Ejemplo:\n\n"
	     	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloMiser(UserFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 100000);"),
          BOperClassify::NumericalAnalysis_);

int BMonteCarloMiser::DoMonteCarlo(gsl_monte_function * G, double * hiper[], int dim, double * result, double * sigma)
{
  int status;

  nfuncall = 0;
  gsl_monte_miser_state *s = gsl_monte_miser_alloc(dim);
  status = gsl_monte_miser_integrate (G, hiper[0], hiper[1], dim, m_funcalls, rgn, s, result, sigma);
  gsl_monte_miser_free (s);
  return status;
}

//! BMonteCarloVegas class: VEGAS method.
/*! BMonteCarloVegas class implement the interface for computing a
  Monte Carlo integral following the VEGAS method.
*/

class BMonteCarloVegas : public BMonteCarlo
{
public:
  BMonteCarloVegas(BList * args)
    : BMonteCarlo("MonteCarloVegas", args) {
    /* provide default values */
    m_hotcalls = m_funcalls / 5;
    if (m_hotcalls < 1)
      m_hotcalls = 10;
    m_chisqsig = 1.0;
    /* now verify my extra arguments */
    if (m_ArgsOk)
      CheckExtraArguments();
  }

protected:
  virtual int DoMonteCarlo(gsl_monte_function * G, double * hiper[], int dim, double * result, double * sigma);
  //! CheckExtraArguments verify the arguments specific to VEGAS method.
  /*! \sa  BMonteCarlo::CheckArguments
  
  \return 
  */
  virtual BList * AddExtraParameters(BList *);
  BBool CheckExtraArguments();
private:
  int m_hotcalls;
  float m_chisqsig;
  double m_chisqres;
};

DeclareEvaluator(BMonteCarloVegas);
DefExtOpr(1, BMonteCarloVegas, "MonteCarloVegas", 4, 7, "Code Real Matrix Matrix Real Real Real",
          "(Code function, Real dim, Matrix xlower, Matrix xupper [, Real warmcalls, Real hotcalls, Real signif])", 
          I2("Monte Carlo Integration: VEGAS method. Integrate by a combination of importance and"
	     " recursive stratified sampling.\n\n"
	     "Arguments:\n\n"
	     "    function  --> user function to integrate. Should be declared as\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim       --> domain dimension.\n"
	     "    xlower    --> \n"
	     "    xupper    --> both are column matrix of size dim and define the integration volumen\n"
	     "    warmcalls --> number of function calls to preparate the histogram. It defaults to 100000.\n"
	     "    hotcalls  --> number of function calls to estimate an independent integral\n"
	     "    signif    --> result's significance, should be close to 0\n\n"
	     "Example:\n\n"
	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloVegas(TestFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 10000, 100000);"
	     ,
             "Integración de Monte Carlo: método VEGAS. Integra mediante una combinación the muestreo"
	     " con importancia\n"
	     "y muestreo recursivo estratificado.\n\n"
	     "Argumentos:\n\n"
	     "    function  --> función de usuario a integrar. Debe declararse como\n"
	     "                 Real (Matrix) {...}\n"
	     "    dim       --> dimensión del dominio.\n"
	     "    xlower    --> \n"
	     "    xupper    --> ambos son matrices columnas de tamaño dim y definen el volumen the integración\n"
	     "    warmcalls --> número de evaluaciones de la función para preparar el histograma.\n"
	     "                  Por omisión se asume 100000.\n"
	     "    hotcalls  --> número de evaluaciones de la función usadas en el estimado de una integral independiente\n"
	     "    signif    --> significancia de los resultados, debe ser cercano a 0.\n\n"
	     "Ejemplo:\n\n"
	     "\tReal UserFunc ( Matrix X )\n"
	     "\t{\n"
	     "\t    Real dim = Rows(X);\n"
	     "\t    Real A = 1 / Pi ^ dim;\n"
	     "\t    A / (1.0 - MatProd(Cos(X)))\n"
	     "\t};\n"
	     "\tSet IntMonte = MonteCarloVegas(UserFunc, 3, Col(0,0,0), Col(Pi,Pi,Pi), 10000, 100000);"),
          BOperClassify::NumericalAnalysis_);

int BMonteCarloVegas::DoMonteCarlo(gsl_monte_function * G, double * hiper[], int dim, double * result, double * sigma)
{
  int status;
  char english[512], spanish[512];

  sprintf(english, "VEGAS warm-up  with %d calls ...\n", m_funcalls);
  sprintf(spanish, "Iniciando VEGAS con %d llamadas ...\n", m_funcalls);
  Std(I2(english,spanish));

  nfuncall = 0;
  gsl_monte_vegas_state *s = gsl_monte_vegas_alloc(dim);
  /* vegas warm-up */
  status = gsl_monte_vegas_integrate (G, hiper[0], hiper[1], dim, m_funcalls, rgn, s, result, sigma);
  Std(I2("VEGAS warm-up done\n", "VEGAS iniciado\n"));
  sprintf(english, "Converging VEGAS with %d calls ...\n", m_hotcalls);
  sprintf(spanish, "Convergiendo VEGAS con %d llamadas ...\n", m_hotcalls);
  Std(I2(english,spanish));
  nfuncall = 0;
  // iterations and stage could be configurables.
  //s->iterations = 1;
  do {
    //s->stage = 2;
    gsl_monte_vegas_integrate (G, hiper[0], hiper[1], dim, m_hotcalls, rgn, s, result, sigma);
    sprintf(english, "Partial iteration done, significance = %g\n", s->chisq);
    sprintf(spanish, "Iteración parcial ejecutada, significancia = %g\n", s->chisq);
    Std(I2(english,spanish));
  } while (fabs(s->chisq - 1.0) > m_chisqsig);
  m_chisqres = s->chisq;
  gsl_monte_vegas_free (s);
  return status;
}

BList * BMonteCarloVegas::AddExtraParameters(BList * lst)
{
  BUserDat * chisq;

  chisq = BContensDat::New("chi-squared", BDat(m_chisqres), I2("chi-squared per degree of freedom "
                                                  "for the weighted estimate of the integral",
                                                  "chi-squared por grados de libertad "
                                                  "para el estimado ponderado de la integral"));
  return LstAppend(lst,chisq);
}

BBool BMonteCarloVegas::CheckExtraArguments()
{
  BText english("In ");
  BText spanish("En ");
  int argc = NumArgs();

  english += Name();
  spanish += Name();
  english += ", ";
  spanish += ", ";
  if (argc > 5) {
    /* at least hotcall provided */
    BDat * dat_hot = &(((BUserDat*)Arg(6))->Contens());
    int hcalls;
    if (dat_hot->Known() && (hcalls = int(dat_hot->Value()))>0) {
      m_hotcalls = hcalls;
    }
    else {
      english += "invalid number of function hot-calls: must be known and positive. Defaults to ";
      english += m_hotcalls;
      spanish += "número de llamadas a función inválido: debe ser conocido y positivo. Se asume ";
      spanish += m_hotcalls;
      Warning(I2(english,spanish));
    }    
    if (argc == 7) {
      /* chi-squared provided */
      BDat * dat_chisq = &(((BUserDat*)Arg(7))->Contens());
      float chisq;
      if (dat_chisq->Known() && (chisq = float(dat_chisq->Value()))>0.0) {
        m_chisqsig = chisq;
      }
      else {
        english += "invalid chi-squared threshold: must be known and positive. Defaults to ";
        english += m_chisqsig;
        spanish += "umbral de chi-squared inválido: debe ser conocido y positivo. Se asume ";
        spanish += m_chisqsig;
        Warning(I2(english,spanish));
      }    
    }
  }
  return BTRUE;
}
