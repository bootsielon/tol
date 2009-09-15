/* vmat_bsr_primary.cpp: BVMat Bayesian Sparse Regression 
                              parsing methods for primary modules
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

#include <tol/tol_bvmat_bsr_impl.h>

namespace BysSparseReg {

////////////////////////////////////////////////////////////////////////////////
int Parse_Module(const BText& filePath, 
                 const BText& moduleType,
                 BSet& contens_)
////////////////////////////////////////////////////////////////////////////////
{
  string fileName = filePath.String();
  int k,h;
  BVMat Y,X,a,A;
  BArray<BVMat> VSArr;
  BysSparseReg::doc_info                     docInfo;    
  std::vector<BysSparseReg::variable_info>   linearInfo;
  std::vector<BysSparseReg::missing_info>    inputMissingInfo;
  std::vector<BysSparseReg::missing_info>    outputMissingInfo;
  std::vector<BysSparseReg::noise_info>      noiseInfo;
  if(moduleType=="primary")
  {
    BysSparseReg::Parse_Module_Primary
    (
      fileName, docInfo, 
      linearInfo,inputMissingInfo,outputMissingInfo,noiseInfo,
      Y, X, a, A
    );
  }
  else if(moduleType=="joint")
  {
    BysSparseReg::Parse_Module_Joint
    (
      fileName, docInfo, 
      linearInfo,inputMissingInfo,outputMissingInfo,noiseInfo,
      Y, X, a, A
    );
  }
  BSet HD, LI, DI, MI, MO;
  BGrammar::IncLevel();
  LI.PrepareStore(linearInfo.size());
  for(k=0; k<linearInfo.size(); k++)
  {
    BText name = linearInfo[k].name.c_str();
    BSet aux;
    aux.PrepareStore(2);
    aux.AddElement(BContensText::New("",name, ""));
    aux.AddElement(BContensDat ::New("",linearInfo[k].initValue,""));
    aux.PutStruct(BysSparseReg::LinearBlockStr());
    LI.AddElement(BContensSet::New(name,aux,""));
  }
  if(LI.Card()) { LI.SetIndexByName(); }
  MI.PrepareStore(inputMissingInfo.size());
  for(k=0; k<inputMissingInfo.size(); k++)
  {
    BText name = inputMissingInfo[k].name.c_str();
    BSet aux;
    aux.PrepareStore(9);
    aux.AddElement(BContensText::New("",name,                              ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].index,         ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].row,           ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].col,           ""));
    aux.AddElement(BContensText::New("",inputMissingInfo[k].prior.c_str(), ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].nu,            ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].sigma2,        ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].minBound,      ""));
    aux.AddElement(BContensDat ::New("",inputMissingInfo[k].maxBound,      ""));
    aux.PutStruct(BysSparseReg::MissingBlockStr());
    MI.AddElement(BContensSet::New(name,aux,""));
  }
  if(MI.Card()) { MI.SetIndexByName(); }
  MO.PrepareStore(outputMissingInfo.size());
  for(k=0; k<outputMissingInfo.size(); k++)
  {
    BText name = outputMissingInfo[k].name.c_str();
    BSet aux;
    aux.PrepareStore(9);
    aux.AddElement(BContensText::New("",name,                               ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].index,         ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].row,           ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].col,           ""));
    aux.AddElement(BContensText::New("",outputMissingInfo[k].prior.c_str(), ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].nu,            ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].sigma2,        ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].minBound,      ""));
    aux.AddElement(BContensDat ::New("",outputMissingInfo[k].maxBound,      ""));
    aux.PutStruct(BysSparseReg::MissingBlockStr());
    MO.AddElement(BContensSet::New(name,aux,""));
  }
  if(MO.Card()) { MO.SetIndexByName(); }
  DI.PrepareStore(noiseInfo.size());
  for(k=0; k<noiseInfo.size(); k++)
  {
    BText name = noiseInfo[k].name.c_str();
    BGrammar::IncLevel();
    BSet aux;
    BSet aux2;
    std::vector<int>& equIdx=noiseInfo[k].equIdx;
    aux2.PrepareStore(equIdx.size());
    for(h=0; h<equIdx.size(); h++)
    {
      aux2.AddElement(BContensDat::New("",equIdx[h],""));
    }
    aux.PrepareStore(10);
    aux.AddElement(BContensText::New
    (
      "Name",     
      name,      
      ""
    ));
    aux.AddElement(BContensDat::New
    (
      "Nu",             
      noiseInfo[k].nu,                
      ""
    ));
    aux.AddElement(BContensText::New
    (
      "SigmaName",      
      noiseInfo[k].sigmaName.c_str(), 
      ""
    ));
    aux.AddElement(BContensDat::New
    (
      "SigmaIndex",
      noiseInfo[k].sigmaIdx,
      ""
    ));
    BSyntaxObject* sigPri = NULL;
    BText expr = noiseInfo[k].sigPriExpr.c_str();
    if(expr=="")
    {
      sigPri = GraSet()->EvaluateExpr("Copy(Empty)");
    }
    else
    {
      sigPri = GraSet()->EvaluateExpr(expr);
      if(!sigPri) 
      { 
        Error(I2("[BSR] Cannot evaluate SigmaPrior expression for ",
                 "[BSR] No se puede evaluar la expresión del SigmaPrior para ")+
              noiseInfo[k].name.c_str()+"=\n"+expr+"\n");
        sigPri = GraSet()->EvaluateExpr("Copy(Empty)"); 
      }
    }
    sigPri->PutName("SigmaPrior");
    aux.AddElement(sigPri);
    BSyntaxObject* arima = NULL;
    expr = noiseInfo[k].arimaExpr.c_str();
    if(!expr.HasName())
    {
      arima = GraSet()->EvaluateExpr("Copy(Empty)"); 
    }
    else
    {
  //Std(BText("  ARIMA expression for ")+noiseInfo[k].name.c_str()+"=\n"+expr+"\n");
      arima = GraSet()->EvaluateExpr(expr);
      if(!arima) 
      { 
        Error(I2("[BSR] Cannot evaluate ARIMA expression for ",
                 "[BSR] No se puede evaluar la expresión ARIMA para ")+
              noiseInfo[k].name.c_str()+"=\n"+expr+"\n");
        arima = GraSet()->EvaluateExpr("Copy(Empty)"); 
      }
    }
    arima->PutName("Arima");
    aux.AddElement(arima);
    BSyntaxObject* arimaAuxInfo = GraSet()->EvaluateExpr("Copy(Empty)");
    arimaAuxInfo->PutName("ArimaAuxInfo");
    aux.AddElement(arimaAuxInfo);
    aux.AddElement(BContensVMat::New
    (
      "COV",
      noiseInfo[k].cov,
      ""
    ));
    assert(noiseInfo[k].cov.Check());
    aux.AddElement(BContensVMat::New
    (
      "L",  
      noiseInfo[k].L,               
      "COV=L'L"
    ));
    assert(noiseInfo[k].L.Check());
    aux.AddElement(BContensVMat::New
    (
      "Li", 
      noiseInfo[k].Li, 
      "L*Li=I"
    ));
    assert(noiseInfo[k].Li.Check());
    aux.AddElement(BContensSet::New
    (
      "EquationIndexes", 
      aux2,                               
      ""
    ));
    BSyntaxObject* timeInfo  = NULL;
    if(noiseInfo[k].dating != "")
    {
      BUserTimeSet* dating = (BUserTimeSet*)
        GraTimeSet()->EvaluateExpr(noiseInfo[k].dating.c_str());
      if(!dating)
      {
        Error(I2("[BSR] Cannot evaluate dating expression for ",
                 "[BSR] No se puede evaluar la expresión de fechado para ")+
              noiseInfo[k].dating.c_str()+"=\n"+expr+"\n");
      }
      else
      {
        BUserDate* firstDate = (BUserDate*)
          GraDate()->EvaluateExpr(noiseInfo[k].firstDate.c_str());
        if(!firstDate)
        {
          Error(I2("[BSR] Cannot evaluate first date expression for ",
                   "[BSR] No se puede evaluar la expresión de fecha inicial para ")+
                noiseInfo[k].firstDate.c_str()+"\n");
          DESTROY(dating);
        }
        else if(!dating->Includes(firstDate->Contens()))
        {
          Error(I2("[BSR] First date ", "[BSR] La fecha inicial ")+
                noiseInfo[k].firstDate.c_str()+
                I2("doesn't match dating ", " no es del fechado ")+
                noiseInfo[k].dating.c_str()+"\n");
          DESTROY(dating);
          DESTROY(firstDate);
        }
        else
        {
          BUserDate* lastDate  = (BUserDate*)
            GraDate()->EvaluateExpr(noiseInfo[k].lastDate.c_str());
          if(!lastDate)
          {
            Error(I2("[BSR] Cannot evaluate last date expression for ",
                     "[BSR] No se puede evaluar la expresión de fecha final para ")+
                  noiseInfo[k].firstDate.c_str()+"\n");
            DESTROY(dating);
            DESTROY(firstDate);
          }
          else
          {
            int length = 
              1+dating->Difference(firstDate->Contens(),lastDate->Contens());
            if(length != equIdx.size())
            {
              Error(I2("[BSR] Interval between first and last dates ", 
                       "[BSR] El intervalo entre las fechas inicial y final ")+
                    "["+noiseInfo[k].firstDate.c_str()+";"+
                        noiseInfo[k].lastDate.c_str()+"]"+
                    I2(" along dating ", " a lo largo del fechado ")+
                      noiseInfo[k].dating.c_str()+
                    I2(" has length "," tiene longitud ")+length+
                    I2(" but noise ", " pero el ruido ")+name+
                    I2(" was declared with length "," fue declarado de longitud ")+
                    (int)equIdx.size()+"\n");
              DESTROY(dating);
              DESTROY(firstDate);
              DESTROY(lastDate);  
            }
            else
            {
              BSet auxTimeInfo;
              auxTimeInfo.PrepareStore(3);
              auxTimeInfo.AddElement(dating);
              auxTimeInfo.AddElement(firstDate);
              auxTimeInfo.AddElement(lastDate);
              auxTimeInfo.PutStruct(BysSparseReg::NoiseTimeInfo());
              timeInfo = BContensSet::New("",auxTimeInfo,"TimeInfo");
            }
          }
        }
      }
    }
    if(!timeInfo)
    {
      timeInfo = GraSet()->EvaluateExpr("Copy(Empty)");
    }
    timeInfo->PutName("TimeInfo");
    aux.AddElement(timeInfo);

    aux.PutStruct(BysSparseReg::NoiseDistribStr());
    DI.AddElement(BContensSet::New(name,aux,""));
    BGrammar::DecLevel();
  }
  if(DI.Card()) { DI.SetIndexByName(); }
  HD.PrepareStore(6);
  HD.AddElement(BContensText::New
  (
    "Model.Name", 
    docInfo.model_name.c_str(),
    "Model identifier"
  ));
  HD.AddElement(BContensText::New
  (
    "Model.Description",
    docInfo.model_description.c_str(),
    "Notes about the model"
  ));
  HD.AddElement(BContensText::New
  (
    "Session.Name", 
    docInfo.session_name.c_str(),
    "Session identifier"
  ));
  HD.AddElement(BContensText::New
  (
    "Session.Description",
    docInfo.session_description.c_str(),
    "Notes about the session"
  ));
  HD.AddElement(BContensText::New
  (
    "Session.Authors",     
    docInfo.session_authors.c_str(),
    "Name, e-mail, url or any identifier of session authors separated by commas"
  ));
  HD.AddElement(BContensDate::New
  (
    "Session.Creation",     
    DteNow(),
    "Session creation date and time"
  ));
  HD.AddElement(BContensText::New
  (
    "Path",       
    GetFilePath(filePath),
    "The root path of directory used to store all information related to this model is the same of BSR ASCII file"
  ));
  HD.PutStruct(BysSparseReg::DocInfoStr());

  contens_.PrepareStore(9);
  contens_.AddElement(BContensSet::New
  (
    "DocInfo", 
    HD,
    "Header documentary information"
  ));
  contens_.AddElement(BContensSet ::New
  (
    "LinearBlock",
    LI,  
    "Linear block information is a table with structure "
    "BSR.LinearBlockStr"
  ));
  contens_.AddElement(BContensSet ::New
  (
    "MissingInputBlock",
    MI,  
    "Missing Input block information is a table with structure "
    "BSR.MissingBlockStr"
  ));
  contens_.AddElement(BContensSet ::New
  (
    "MissingOutputBlock",
    MO,  
    "Missing Output block information is a table with structure "
    "BSR.MissingBlockStr"
  ));
  contens_.AddElement(BContensSet ::New
  (
    "NoiseDistrib", 
    DI,  
    "Noise distribution information is a table with structure "
    "BSR.SigmaBlockStr"
  ));
  contens_.AddElement(BContensVMat::New
  (
    "Y",          
    Y,   
    "Regression output"
  ));
  contens_.AddElement(BContensVMat::New
  (
    "X",          
    X,   
    "Regression input"
  ));
  contens_.AddElement(BContensVMat::New
  (
    "a",          
    a,   
    "Restrictions border"
  ));
  contens_.AddElement(BContensVMat::New
  (
    "A",          
    A,   
    "Restrictions coefficients"
  ));  
  contens_.PutStruct(BysSparseReg::ModelDefStr());
  BGrammar::DecLevel();
  assert(Y.Check());
  assert(X.Check());
  assert(a.Check());
  assert(A.Check());
};

};

