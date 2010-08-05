/* tolgsl.cpp: Header file for interface between C++ and C for gsl
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

/*!
  \file   tolgsl.cpp
  \author Jorge Suit Perez Ronda
  \date   Fri Apr  4 08:10:04 2003
  
  \brief  Implementation file for interface between C++ and C for gsl
  
  
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_gsl.h>
#include <gsl/gsl_errno.h>

BTraceInit("tolgsl.cpp");


//--------------------------------------------------------------------
void GslErrorHandler (const char * reason,
                      const char * file,
                      int line,
                      int gsl_errno)
{
  Warning(BText("[GSL Error ")+gsl_errno+"] "+reason+"\n"
          "At line "+line+" of file "+file); 
}


//--------------------------------------------------------------------
bool GslInitialize()
/*! 
 */
//--------------------------------------------------------------------
{
  gsl_set_error_handler(&GslErrorHandler);
  return(true);
}


double RealGSLFunction::StaticEvalReal( double x, void * instance )
{
  RealGSLFunction * obj = (RealGSLFunction*)instance;

  return obj->Eval(&x);
}

RealGSLFunction::RealGSLFunction( BCode * code, BUserSet * uset )
{
  m_UError = 0;
  m_LastResult = 0.0;
  m_ptrUCode = code;
  m_ptrUSet = uset;
}

RealGSLFunction::~RealGSLFunction()
{
  // nothing
}

double RealGSLFunction::Eval( void * arg )
{
  // verify if previous user code error
  if (!m_UError) {
  
    // do the evaluation
    
    BSyntaxObject * eval_res;
    BDat * dat_res;

    PutExternalArg(arg);
    eval_res = m_ptrUCode->Evaluator(BuildArgs());
    if (eval_res) {
      dat_res = &(((BUserDat*)eval_res)->Contens());
      if (dat_res->Known()) {
	// save last evaluation
	m_LastResult = dat_res->Value();    
      } else {
	BText msg_en("In RealGSLFunction::Eval, unknown value in evaluation of ");
	BText msg_sp("En RealGSLFunction::Eval, valor desconocido en la evaluación de ");
	msg_en.Concat(m_ptrUCode->Name());
	msg_sp.Concat(m_ptrUCode->Name());
	Warning(I2(msg_en, msg_sp));
      }
      DESTROY(eval_res);
    } else {
      /* there was an error evaluating the user function,
	 assume the last evaluation */
      m_UError = 1;
    }
  }
  return m_LastResult;
}

RealGSLFunctionR1::RealGSLFunctionR1( BCode * code, BUserSet * uset )
  : RealGSLFunction(code, uset)
{
  m_UDat.IncNRefs();
  m_ptrDat = &(m_UDat.Contens());
  m_ptrDat->PutValue(0.0);
}

RealGSLFunctionR1::~RealGSLFunctionR1()
{
  m_UDat.DecNRefs();
  // nothing
}


void RealGSLFunctionR1::PutExternalArg( void * arg )
{
  m_ptrDat->PutValue(*((double*)arg));
}

BList * RealGSLFunctionR1::BuildArgs()
{
  if (m_ptrUSet)
    return Cons(&m_UDat,Cons(m_ptrUSet,NIL));
  return Cons(&m_UDat,NIL);
}
