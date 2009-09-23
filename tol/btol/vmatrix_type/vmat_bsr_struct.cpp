/* vmat_bsr_struct.cpp: BVMat Bayesian Sparse Regression 
                              parsing methods
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

//#define BOOST_SPIRIT_DEBUG

#include <tol/tol_bvmat_bsr.h>
#include <tol/tol_bvmat_impl.h>
#include <tol/tol_blanguag.h>

namespace BysSparseReg {

////////////////////////////////////////////////////////////////////////////////
const BText& BSR()
////////////////////////////////////////////////////////////////////////////////
{
  static BText BSR_ = "[BSR.Parse] ";
  return(BSR_);
}      

//--------------------------------------------------------------------
BStruct* ModelDefStr()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol(
    "BSR.ModelDef"
    ,
    "Text:ModuleType,"
    "Set:DocInfo,"
    "Set:LinearBlock,"
    "Set:InputMissingBlock,"
    "Set:OutputMissingBlock,"
    "Set:NoiseDistrib,"
    "VMatrix:Y,"
    "VMatrix:X,"
    "VMatrix:a,"
    "VMatrix:A");
    aux->PutDescription("Contains all information needed to make bayesian "
    "estimation using MonteCarlo Markov Chain methods over a Restricted "
    "Linear Regression model:\n"
    "  Y = X*B + E \n"
    "  A*B <= a\n"
    "Field LinearBlock contains information about variables vector b and "
    "must have structure of BSR.LinearBlock, that includes initial "
    "values B0 matching constraining inequations A*B0<=a.\n"
    "Field NoiseDistrib contains information about noise vector "
    "E and must have structure of BSR.NoiseDistrib");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* DocInfoStr()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol(
    "BSR.DocInfo"
    ,
    "Text:Model.Name,"
    "Text:Model.Description,"
    "Text:Session.Name,"
    "Text:Session.Description,"
    "Text:Session.Authors,"
    "Date:Session.Creation,"
    "Text:Path");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* LinearBlockStr()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol(
    "BSR.LinearBlock"
    ,
    "Text:Name,"
    "Real:InitialValue");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* MissingBlockStr()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol(
    "BSR.MissingBlock"
    ,
    "Text:Name,"
    "Real:Index,"
    "Real:Row,"
    "Real:Col,"
    "Text:Prior,"
    "Real:Nu,"
    "Real:Sigma2,"
    "Real:MinBound,"
    "Real:MaxBound");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* NoiseTimeInfo()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol( 
    "BSR.NoiseTimeInfo"
    ,
    "TimeSet:Dating,"
    "Date:FirstDate,"
    "Date:LastDate");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* NoiseDistribStr()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol( 
    "BSR.NoiseDistrib"
    ,
    "Text:Name,"
    "Real:Nu,"
    "Text:SigmaName,"
    "Real:SigmaIndex,"
    "Set:SigmaPrior,"
    "Set:Arima,"
    "Set:ArimaAuxInfo,"
    "VMatrix:Cov,"
    "VMatrix:L,"
    "VMatrix:Li,"
    "Set:EquIdx,"
    "Set:TimeInfo,"
    "Set:NonLinFilters"
  );
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* MasterSubModule()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol( 
    "BSR.MasterSubModule"
    ,
    "Text:ModuleType,"
    "Text:FilePath");
  }
  return(aux);
};

//--------------------------------------------------------------------
BStruct* MasterInfo()
//--------------------------------------------------------------------
{
  static BStruct* aux = NULL;
  if(!aux)
  {
    aux = NewStructSymbol( 
    "BSR.MasterInfo"
    ,
    "Text:ModuleType,"
    "Set:DocInfo,"
    "Text:Sampler,"
    "Set:SubModule");
  }
  return(aux);
};

///////////////////////////////////////////////////////////////////////////////
  int Initialize()
///////////////////////////////////////////////////////////////////////////////
{
  static int initialized_ = false;
  if(!initialized_)
  {
    initialized_ = true;
    BStruct* aux1 = ModelDefStr();
    BStruct* aux2 = DocInfoStr();
    BStruct* aux3 = LinearBlockStr();
    BStruct* aux4 = MissingBlockStr();
    BStruct* aux5 = NoiseDistribStr();
    BStruct* aux6 = NoiseTimeInfo();
    
    return(true);
  }
  return(false);
};

};