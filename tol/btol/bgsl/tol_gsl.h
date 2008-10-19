/* tol_gsl.h: Header file for interface between C++ and C for gsl
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
  \file   tolgsl.h
  \author Jorge Suit Perez Ronda
  \date   Fri Apr  4 08:16:06 2003
  
  \brief  Header file for interface between C++ and C for gsl
*/

#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>


bool GslInitialize();


//--------------------------------------------------------------------
  class RealGSLFunction 
//--------------------------------------------------------------------
{
 public:
  static double StaticEvalReal(double x, void * instance);
  RealGSLFunction( BCode * code, BUserSet * uset );
  virtual ~RealGSLFunction();
  virtual double Eval( void * arg );
 protected:
  virtual void PutExternalArg( void * arg ) = 0;
  virtual BList * BuildArgs() = 0;
  BCode * UserCode() {
    return m_ptrUCode;
  }
  BUserSet * UserData() {
    return m_ptrUSet;
  }
  int & UserError() {
    return m_UError;
  }
  double & LastResult() {
    return m_LastResult;
  }
  int m_UError;
  double m_LastResult;
  BCode * m_ptrUCode;
  BUserSet * m_ptrUSet;  
};

//--------------------------------------------------------------------
  class RealGSLFunctionR1 : public RealGSLFunction
//--------------------------------------------------------------------
{
 public:
  RealGSLFunctionR1( BCode * code, BUserSet * uset );
  ~RealGSLFunctionR1();
 protected:
  virtual void PutExternalArg( void * arg );
  virtual BList * BuildArgs();
  BContensDat m_UDat;
  BDat * m_ptrDat;
};
