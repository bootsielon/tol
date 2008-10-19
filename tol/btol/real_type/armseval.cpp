/* armseval.cpp: Wraper for evaluating an UserCode as a C pointer.
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
  \file   armseval.cpp
  \author Jorge Suit Perez Ronda
  \date   Mon Mar 31 11:44:41 2003
  
  \brief  Wraper for evaluating an UserCode as a C pointer.
  
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_arms.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_gsl.h>

BTraceInit("armseval.cpp");

DeclareContensClass(BDat, BDatTemporary, BDatARMS);
DefExtOpr(1, BDatARMS, "ARMS", 3, 5, "Code Real Real Real Set",
	  I2("(Code logf, Real lower, Real upper [, Real xprev, Set data])",
	     "(Code logf, Real inf, Real sup [, Real xprev, Set datos])"),
	  I2("Perform adaptive rejection metropolis sampling in one "
	     "dimension.\n\n"
	     "Arguments:\n\n"
	     "    logf         --> A function to evaluate the log-density. "
	     "This function "
	     "have one Real argument at which the log-density is to be "
	     "evaluated. Optionally "
	     "it can receive a second Set argument to be used in the "
	     "computation of the log-density\n"
	     "    lower, upper --> Left and right bounds. Bounds must be "
	     "supplied even "
	     "if the support is truly unbounded. This should present no "
	     "problem if the "
	     "bounds are set far enough out.\n"
	     "    xprev        --> Previous value from a Markov chain, for "
	     "instance when "
	     "using ARMS withing the Gibbs sampler.\n"
	     "    data         --> Parameters to for the function logf"
	     "Example:\n\n"
	     "\tReal LogNorm( Real x )\n"
	     "\t{\n"
	     "\t    Real -0.5 * Pow((x-10.0)/5.0,2.0)\n"
	     "\t};\n\n"
	     "\tReal xsamp = ARMS(LogNorm, -100, 100);\n",
	     "Realiza muestreo ARMS en una dimensión.\n\n"
	     "Argumentos:\n\n"
	     "    logf     --> Función que evalua el logaritmo de la "
	     "densidad. Esta función "
	     "recibe un argumento Real que es donde se evaluará. "
	     "Opcionalmente puede recibir "
	     "un segundo argumento Set que se usará en la evaluación de la "
	     "densidad.\n"
	     "    inf, sup --> Intervalo de evaluación. Las cotas deben "
	     "darse siempre incluso cuando se sepa que el dominio es no "
	     "acotado, en tal caso las cotas pueden separarse "
	     "suficientemente.\n"
	     "    xprev    --> Valor previo del parámetro en una cadena de "
	     "Markov, por ejemplo cuando se use en el Gibbs sampler.\n\n"
	     "Ejemplo:\n\n"
	     "\tReal LogNorm( Real x )\n"
	     "\t{\n"
	     "\t    Real -0.5 * Pow((x-10.0)/5.0,2.0)\n"
	     "\t};\n\n"
	     "\tReal xsamp = ARMS(LogNorm, -100, 100);\n"),
	  BOperClassify::Statistic_);

void BDatARMS::CalcContens()
{
  BDat * inf, * sup, * xprev_dat = NULL;
    
  BText english("In ARMS, ");
  BText spanish("En ARMS, ");

  BUserSet * uset = NULL;

  BCode * cod = &Code(Arg(1));
  inf = &Dat(Arg(2));
  sup = &Dat(Arg(3));

  /* check inf and sup */

  if (!inf->IsKnown() || !sup->IsKnown()) {
    english += "lower and upper values must be known";
    spanish += "los valores inf y sup deben ser conocidos";
    Error(I2(english,spanish));
    return;
  }
  
  /* check valid interval */

  if (inf->Value()>=sup->Value()) {
    english += "invalid interval: upper must be greater than lower";
    spanish += "intervalo inválido: sup debe ser mayor que inf";
    Error(I2(english,spanish));
    return;
  }

  /* check for previous sample */

  if (NumArgs() > 3) {
    xprev_dat = &Dat(Arg(4));
    if (!xprev_dat->IsKnown() || xprev_dat->Value()<inf->Value() ||
	xprev_dat->Value()>sup->Value()) {
      english += "invalid xprev argument value: must be known and included in [lower,upper]";
      spanish += "valor de argumento xprev inválido: debe ser conocido e incluido en [inf,sup]";
      Error(I2(english,spanish));
      return;
    }
  }

  /* check user function */

  if (!cod->Operator()) {
    english += "invalid code argument";
    spanish += "argumento código inválido";
    Error(I2(english,spanish));
    return;
  }

  /* check user function's grammar */

  if (cod->Grammar() != GraReal()) {
    english += "invalid type ";
    english += cod->Grammar()->Name();
    english += " for logf argument: should be Real";
    spanish += " tipo inválido ";
    spanish += cod->Grammar()->Name();
    spanish += " para argumento logf: debe ser Real";
    Error(I2(english,spanish));
    return;
  } 

  /* check grammar of user function's first argument */

  if (cod->Operator()->GrammarForArg(1) != GraReal()) {
    english += "invalid type ";
    english += cod->Operator()->GrammarForArg(1)->Name();
    english += " for first argument of logf argument: should be Real";
    spanish += "tipo inválido ";
    spanish += cod->Operator()->GrammarForArg(1)->Name();
    spanish += " en el primer argumento del argumento logf: debe ser Real";
    Error(I2(english,spanish));
    return;
  }

  if (NumArgs() == 5) {
    // function data provided
    uset = (BUserSet*)(Arg(5));
    /* check user function's number of arguments */
    if (cod->Operator()->MinArg()!=2 && 
	cod->Operator()->MaxArg()!=2) {
      english += "invalid argument number for logf argument: should be 2";
      spanish += "número de argumentos inválidos para argumento logf: debe ser 2";
      Error(I2(english,spanish));  
      return;
    }
    /* check grammar of user function's second argument */
    if (cod->Operator()->GrammarForArg(2) != GraSet()) {
      english += "invalid type ";
      english += cod->Operator()->GrammarForArg(2)->Name();
      english += " for second argument of logf argument: should be Set";
      spanish += "tipo inválido ";
      spanish += cod->Operator()->GrammarForArg(2)->Name();
      spanish += " en el segundo argumento del argumento logf: debe ser Set";
      Error(I2(english,spanish));
      return;
    }
  } else {
    // no function data provided
    uset = NULL;
    /* check user function's number of arguments */
    if (cod->Operator()->MinArg()!=1 && 
	cod->Operator()->MaxArg()!=1) {
      english += "invalid argument number for logf argument: should be 1";
      spanish += "número de argumentos inválidos para argumento logf: debe ser 1";
      Error(I2(english,spanish));  
      return;
    }  
  }
  
  // do arms
  int    ninit = 4;
  double xl = inf->Value(), xr = sup->Value();
  int    domet = 1;
  double xprev;
  double xsamp;
  
  // xprev default to center
  xprev = (xl+xr)/2;

  /*
  int npoint = 100, nsamp = 1, ncent = 4, neval;
  double xinit[10] = {0.0,3.0,17.0,20.0};
  double xsamp[100], xcent[10], qcent[10] = {5.0, 30.0, 70.0, 95.0};
  double convex = 1.0;
  */

  int arms_status;

  
  RealGSLFunctionR1 funeval(cod, uset);

  if (xprev_dat)
    xprev = xprev_dat->Value();
  /*
  arms_status = arms(xinit, ninit, &xl, &xr, ArmsCodeEval,
 		     arms_data, &convex, npoint, domet, &xprev, xsamp,
 		     nsamp, qcent, xcent, ncent, &neval);
  */
  arms_status = arms_simple(ninit, &xl, &xr, RealGSLFunction::StaticEvalReal, &funeval, domet, &xprev, &xsamp);

  if (arms_status > 0) {
    char buffer[128];

    sprintf(buffer, "error code = %d", arms_status);
    english += buffer;
    sprintf(buffer, "código de error = %d", arms_status);
    spanish += buffer;
    Error(I2(english,spanish));
  } else {
    contens_.PutValue(xsamp);
  }
}
