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

///////////////////////////////////////////////////////////////////////////////
void error_report_parser::show_parser_error (
  char const* msg, 
  file_position const& lc, 
  bool end)
///////////////////////////////////////////////////////////////////////////////
{
  BText full_msg = BSR() + BText(msg) + "\n" +
      " File:'" + lc.file.c_str()  + "'" +
      " Line:" + (int)lc.line +
      " Col:" + (int)lc.column;
  if(end)
  {
    full_msg += BText("Unexpected end of file.");
  };
  full_msg += BText("\n")+url_parse_bsr();
  Error(full_msg);
}

/////////////////////////////////////////////////////////////////////////////
bys_sparse_reg::bys_sparse_reg()
/////////////////////////////////////////////////////////////////////////////
: Xnzmax_  (0),
  Anzmax_  (0),
  numEqu_  (0),
  endFound_ (false),
  moduleType("joint")
{
  var.count = 0;
  mis.count = 0;
  sig.count = 0;
  noise.count = 0;
  var.info.index = 0;
  mis.info.index = 0;
  sig.info.index = 0;
  noise.info.index = 0;
  noise.info.sigmaIdx=-1;
}
   
/////////////////////////////////////////////////////////////////////////////
int bys_sparse_reg::expand2AllEqu(
  noise_info& resInfo, 
  const BVMat& A, 
  BVMat& A_) 
/////////////////////////////////////////////////////////////////////////////
{
//Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu 1"));
  int s = resInfo.equIdx.size();
  int n = A.Rows();
  if(s!=n)
  { 
    Error(BSR()+"Size of noise "+
      resInfo.name.c_str()+" has been declared as "+
    s + " but there are "+n+" equations for it.");
    return(-1); 
  }
  BVMat A1, A2;
  int k, nnz;
  A1.Convert(A,BVMat::ESC_chlmRtriplet);
  nnz = A1.s_.chlmRtriplet_->nnz;
  A2.ChlmRTriplet(numEqu_,numEqu_,nnz);
  int*    r1_ = (int*)   A1.s_.chlmRtriplet_->i;
  int*    c1_ = (int*)   A1.s_.chlmRtriplet_->j;
  double* x1_ = (double*)A1.s_.chlmRtriplet_->x;
  int*    r2_ = (int*)   A2.s_.chlmRtriplet_->i;
  int*    c2_ = (int*)   A2.s_.chlmRtriplet_->j;
  double* x2_ = (double*)A2.s_.chlmRtriplet_->x;
  for(k=0; k<nnz; k++)
  {
    if(r1_[k]>=s)
    { 
      Error(BSR()+"Size of noise "+
        resInfo.name.c_str()+" should be at least "+
      (r1_[k]+1) + " but is set to "+s);
      return(-1); 
    }
    if(resInfo.equIdx[r1_[k]]>numEqu_)
    { 
      Error(BSR()+"Number of equations "+
        resInfo.name.c_str()+" should be at least "+
      (resInfo.equIdx[r1_[k]]) + " but is set to "+numEqu_);
      return(-2); 
    }
    if(x1_[k]!=0.0)
    {
      r2_[k]=resInfo.equIdx[r1_[k]]-1;
      c2_[k]=resInfo.equIdx[c1_[k]]-1;
      x2_[k]=x1_[k];
      A2.s_.chlmRtriplet_->nnz++;
    }
  }
  A_.Convert(A2, BVMat::ESC_chlmRsparse);
//Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu END"));
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int bys_sparse_reg::expand2AllEqu_covAndFactors(noise_info& resInfo) 
/////////////////////////////////////////////////////////////////////////////
{
//Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu_covAndFactors 1"));
  int k, n, err;
  BVMat cov, L, Ls, Li, D;
  cov = resInfo.cov;
  n = cov.Rows();
  if(resInfo.covIsDiag)
  {
  //Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu_covAndFactors 2"));
    L = cov;
    Li = cov;
    double* xCov, *xL, * xLi;
    int nzmax;
    cov.StoredData(xCov, nzmax);
    L  .StoredData(xL,   nzmax);
    Li .StoredData(xLi,  nzmax);
    for(k=0; k<nzmax; k++)
    {
      xL [k] = sqrt(xCov[k]);
      xLi[k] = 1.0/xL[k];
    }
    if(err = expand2AllEqu(resInfo, cov, resInfo.cov)) { return(err); }
    if(err = expand2AllEqu(resInfo, L,   resInfo.L  )) { return(err); }
    if(err = expand2AllEqu(resInfo, Li,  resInfo.Li )) { return(err); }
  }
  else
  {
  //Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu_covAndFactors 3"));
    err = BVMat::CholeskiFactor(cov,L,BVMat::ECFO_XtX,true,true,true);
    if(err) 
    { 
      Error(BSR()+"Non symmetric definite positive covariance matrix for noise "+
        resInfo.name.c_str());
      return(err); 
    }
    D.Eye(n);
    err = BVMat::CholeskiSolve(L, D, Li, BVMat::ECSS_L);
    if(err) 
    { 
      Error(BSR()+"Cannot inverse Choleski Factor of covariance matrix for noise "+
        resInfo.name.c_str());
      return(err); 
    }
    Ls.Convert(L,BVMat::ESC_chlmRsparse);
    if(err = expand2AllEqu(resInfo, cov, resInfo.cov)) { return(err); }
    if(err = expand2AllEqu(resInfo, Ls,  resInfo.L  )) { return(err); }
    if(err = expand2AllEqu(resInfo, Li,  resInfo.Li )) { return(err); }
  }
//Std(BText("\nTRACE bys_sparse_reg::expand2AllEqu_covAndFactors END"));
  return(err);
};

/////////////////////////////////////////////////////////////////////////////
int bys_sparse_reg::checkDimensions(int m)
/////////////////////////////////////////////////////////////////////////////
{
  int n = var.vec.size();
  int b = noise.vec.size();
  if(!n)
  {
    Error(BSR()+"At least a linear regression variable must be defined");
    return(-3);
  }
  if(!b)
  {
    Error(BSR()+"At least a vector of noise with independent normal distribution must be defined");
    return(-4);
  }
  if(!m)
  {
    Error(BSR()+"At least a linear equation must be defined");
    return(-5);
  }
  return(0);
};

/////////////////////////////////////////////////////////////////////////////
int bys_sparse_reg::getMissing(
  BVMat& Y,
  BVMat& X, 
  vector<missing_info>&   inputMissingInfo_,
  vector<missing_info>&   outputMissingInfo_)
/////////////////////////////////////////////////////////////////////////////
{
  int i,j,k;
  Std(BSR()+"Setting input and output missing block\n");
  int inputMisSize = 0;
  int outputMisSize = 0;
  for(i=0; i<mis.vec.size(); i++)
  {
    if(mis.vec[i].col==0) { outputMisSize++; }
    else                  { inputMisSize ++; }
  }
  inputMissingInfo_ .resize(inputMisSize );
  outputMissingInfo_.resize(outputMisSize);
  for(i=j=k=0; i<mis.vec.size(); i++)
  {
    if(mis.vec[i].col==0) { outputMissingInfo_[j++]=mis.vec[i]; }
    else                  { inputMissingInfo_ [k++]=mis.vec[i]; }
  }
  for(i=0; i<inputMissingInfo_.size(); i++)
  {
    int r = inputMissingInfo_[i].row;
    int c = inputMissingInfo_[i].col;
    if((r<=0)||(r>X.Rows())||(c<=0)||(c>X.Columns()))
    {
      Error(BSR()+"Wrong missing input "+inputMissingInfo_[i].name.c_str());
      return(-7);
    }
    else
    {
      X.PutCell(inputMissingInfo_[i].row-1, inputMissingInfo_[i].col-1, 0.0);
      inputMissingInfo_[i].index = i+1;
      if(inputMissingInfo_[i].prior == "None")
      {
        inputMissingInfo_[i].sigma2   = BDat::Nan();
        inputMissingInfo_[i].minBound = BDat::Nan();
        inputMissingInfo_[i].maxBound = BDat::Nan();
      }
      else if(inputMissingInfo_[i].prior == "Normal")
      {
        inputMissingInfo_[i].minBound = BDat::NegInf();
        inputMissingInfo_[i].maxBound = BDat::PosInf();
      }
    }
  }
  for(i=0; i<outputMissingInfo_.size(); i++)
  {
    outputMissingInfo_[i].col = 1;
    int r = outputMissingInfo_[i].row;
    if((r<=0)||(r>Y.Rows()))
    {
      Error(BSR()+"Wrong missing output "+outputMissingInfo_[i].name.c_str());
      return(-8);
    }
    else
    {
      Y.PutCell(r-1, 0, 0.0);

      outputMissingInfo_[i].index = i+1;
      if(outputMissingInfo_[i].prior == "None")
      {
        outputMissingInfo_[i].sigma2   = BDat::Nan();
        outputMissingInfo_[i].minBound = BDat::Nan();
        outputMissingInfo_[i].maxBound = BDat::Nan();
      }
      else if(outputMissingInfo_[i].prior == "Normal")
      {
        outputMissingInfo_[i].minBound = BDat::NegInf();
        outputMissingInfo_[i].maxBound = BDat::PosInf();
      }
    }
  }

  return(0);
};

#define AddAndPutName(_set_,_type_,_name_,_obj_,_desc_)   \
{                                                                \
  BText _name_aux_ = _name_;                                     \
  BSyntaxObject* _aux_ = _type_::New("",_obj_,_desc_);           \
  if(_name_aux_.HasName()) { _aux_->PutName(_name_aux_); }       \
  _set_.AddElement(_aux_);                                       \
}


///////////////////////////////////////////////////////////////////////////////
class bys_sparse_reg_moduletype : public grammar<bys_sparse_reg_moduletype>,
                               public bys_sparse_reg
///////////////////////////////////////////////////////////////////////////////
{
public:
  bys_sparse_reg_moduletype() : bys_sparse_reg() 
  {
    moduleType = "joint";
  }
    
  template <typename ScannerT>
  struct definition
  {
    bys_sparse_reg_moduletype& s;
    rule<ScannerT>  
      endOfSentence,
      module_type, 
      problem;

    definition(const bys_sparse_reg_moduletype& s_)
    : s ((bys_sparse_reg_moduletype&)s_)
    {
      #include "tol_bvmat_bsr_err.h"     
      endOfSentence = (ch_p(';') | error_comma2Expected);
      module_type =
      (
        str_p("Module.Type") >> ch_p('=') >> 
        (str_p("primary")|str_p("joint")|str_p("master"))[assign_a(s.moduleType)] >> 
        endOfSentence
      );
      problem = 
        (str_p("$BEGIN") | eps_p) >>
        (module_type | eps_p) >>
        confix_p(eps_p, (*(anychar_p - end_p)), end_p)
        ;
    }

    rule<ScannerT> const&  start() const { return problem; }
  };
};

////////////////////////////////////////////////////////////////////////////////
static int Parse_ModuleType(const BText& fileName_, 
                                  BText& moduleType)
////////////////////////////////////////////////////////////////////////////////
{
  string fileName = fileName_.String();
  bys_sparse_reg_moduletype bsr;
  int errCode = 0;
  #include "tol_bvmat_bsr_run.h"
  if(!errCode)
  {
    moduleType.Copy(bsr.moduleType.c_str(),bsr.moduleType.size());
  }
  return(errCode);
};


////////////////////////////////////////////////////////////////////////////////
int Parse_NoMasterModule(const BText& filePath, 
                         const BText& moduleType,
                         BSet& contens_)
////////////////////////////////////////////////////////////////////////////////
{
  int k,h;
  BVMat Y,X,a,A;
  BArray<BVMat> VSArr;
  BysSparseReg::doc_info                     docInfo;    
  std::vector<BysSparseReg::variable_info>   linearInfo;
  std::vector<BysSparseReg::missing_info>    inputMissingInfo;
  std::vector<BysSparseReg::missing_info>    outputMissingInfo;
  std::vector<BysSparseReg::noise_info>      noiseInfo;
  BText oldPath = BDir::GetCurrent();
  BText dirPath = GetAbsolutePath(GetFilePath(filePath));
  BText fName = GetFileName(filePath);
  BText fullPath = dirPath+fName;
  BDir::ChangeCurrent(dirPath);
  BGrammar::IncLevel();
  if(moduleType=="primary")
  {
    BysSparseReg::Parse_Module_Primary
    (
      (std::string)fName.String(), docInfo, 
      linearInfo,inputMissingInfo,outputMissingInfo,noiseInfo,
      Y, X, a, A
    );
  }
  else if(moduleType=="joint")
  {
    BysSparseReg::Parse_Module_Joint
    (
      (std::string)fName.String(), docInfo, 
      linearInfo,inputMissingInfo,outputMissingInfo,noiseInfo,
      Y, X, a, A
    );
  }
  BSet HD, LI, DI, MI, MO;
  LI.PrepareStore(linearInfo.size());
  for(k=0; k<linearInfo.size(); k++)
  {
    BText name = linearInfo[k].name.c_str();
    BSet aux;
    BDat isExtern = linearInfo[k].isExtern?1:0;
    BDat used = linearInfo[k].used?1:0;
    aux.PrepareStore(4);
    AddAndPutName(aux,BContensText,"",name,"");
    AddAndPutName(aux,BContensDat,"",linearInfo[k].initValue,"");
    AddAndPutName(aux,BContensDat,"",used,"");
    AddAndPutName(aux,BContensDat,"",isExtern,"");
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
    AddAndPutName(aux,BContensText,"",name,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].index,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].row,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].col,"");
    AddAndPutName(aux,BContensText,"",inputMissingInfo[k].prior.c_str(),"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].nu,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].sigma2,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].minBound,"");
    AddAndPutName(aux,BContensDat,"",inputMissingInfo[k].maxBound,"");
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
    AddAndPutName(aux,BContensText,"",name,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].index,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].row,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].col,"");
    AddAndPutName(aux,BContensText,"",outputMissingInfo[k].prior.c_str(),"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].nu,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].sigma2,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].minBound,"");
    AddAndPutName(aux,BContensDat,"",outputMissingInfo[k].maxBound,"");
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
    AddAndPutName(aux,BContensText,"Name",name,"");
    AddAndPutName(aux,BContensDat,"Nu",noiseInfo[k].nu,"");
    AddAndPutName(aux,BContensText,"SigmaName",noiseInfo[k].sigmaName.c_str(),"");
    AddAndPutName(aux,BContensDat,"SigmaIndex",noiseInfo[k].sigmaIdx,"");
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
                 I2("regression segment ","el segmento de regresión ")+name+
                 "\n"+expr+"\n");
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
                 I2("regression segment ","el segmento de regresión ")+name+
                 "\n"+expr+"\n");
        arima = GraSet()->EvaluateExpr("Copy(Empty)"); 
      }
    }
    arima->PutName("Arima");
    aux.AddElement(arima);
    BSyntaxObject* arimaAuxInfo = GraSet()->EvaluateExpr("Copy(Empty)");
    arimaAuxInfo->PutName("ArimaAuxInfo");
    aux.AddElement(arimaAuxInfo);
    AddAndPutName(aux,BContensVMat,"COV",noiseInfo[k].cov,"");
    assert(noiseInfo[k].cov.Check());
    AddAndPutName(aux,BContensVMat,"L",noiseInfo[k].L,"COV=L'L");
    assert(noiseInfo[k].L.Check());
    AddAndPutName(aux,BContensVMat,"Li",noiseInfo[k].Li,"L*Li=I");
    assert(noiseInfo[k].Li.Check());
    AddAndPutName(aux,BContensSet,"EquationIndexes",aux2,"");
    BSyntaxObject* timeInfo  = NULL;
    if(noiseInfo[k].dating != "")
    {
      BUserTimeSet* dating = (BUserTimeSet*)
        GraTimeSet()->EvaluateExpr(noiseInfo[k].dating.c_str());
      if(!dating)
      {
        Error(I2("[BSR] Cannot evaluate dating expression for ",
                 "[BSR] No se puede evaluar la expresión de fechado para ")+
              I2("regression segment ","el segmento de regresión ")+name+
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
                I2("regression segment ","el segmento de regresión ")+name+
                noiseInfo[k].firstDate.c_str()+"\n");
          DESTROY(dating);
        }
        else if(!dating->Includes(firstDate->Contens()))
        {
          Error(I2("[BSR] First date ", "[BSR] La fecha inicial ")+
                noiseInfo[k].firstDate.c_str()+" "+
                I2("of regression segment ","del segmento de regresión ")+name+
                I2("doesn't match dating ", " no es del fechado ")+
                noiseInfo[k].dating.c_str()+"\n"
             )  ;
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
                  I2("regression segment ","el segmento de regresión ")+name+
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
                    I2(" but regression segment ", " pero el segmento de regresión ")+name+
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
    BSyntaxObject* nonLinFilters = NULL;
    if(noiseInfo[k].nonLinFlt != "")
    {
      BText expr;
      expr.Copy(noiseInfo[k].nonLinFlt.c_str(), 2, noiseInfo[k].nonLinFlt.size()-4);
      nonLinFilters = GraSet()->EvaluateExpr(expr);
      if(!nonLinFilters)
      {
        Error(I2("[BSR] Cannot evaluate non linear filter expression for ",
                 "[BSR] No se puede evaluar la expresión de filtro no lineal para ")+
                 I2("regression segment ","el segmento de regresión ")+name+"\n"+
                 noiseInfo[k].nonLinFlt.c_str()+"=\n"+expr+"\n");
      }

    }
    if(!nonLinFilters)
    {
      nonLinFilters = GraSet()->EvaluateExpr("Copy(Empty)");
    }
    nonLinFilters->PutName("NonLinFilters");
    aux.AddElement(nonLinFilters);

    aux.PutStruct(BysSparseReg::NoiseDistribStr());
    DI.AddElement(BContensSet::New(name,aux,""));
    BGrammar::DecLevel();
  }
  if(DI.Card()) { DI.SetIndexByName(); }
  HD.PrepareStore(6);
  AddAndPutName(HD,BContensText,"Model.Name",docInfo.model_name.c_str(),"Model identifier");
  AddAndPutName(HD,BContensText,"Model.Description",docInfo.model_description.c_str(),"Notes about the model");
  AddAndPutName(HD,BContensText,"Session.Name",docInfo.session_name.c_str(),"Session identifier");
  AddAndPutName(HD,BContensText,"Session.Description",docInfo.session_description.c_str(),"Notes about the session");
  AddAndPutName(HD,BContensText,"Session.Authors",docInfo.session_authors.c_str(),"Name, e-mail, url or any identifier of session authors separated by commas");
  AddAndPutName(HD,BContensDate,"Session.Creation",DteNow(),"Session creation date and time");
  AddAndPutName(HD,BContensText,"Path",fullPath,"The root path of directory used to store all information related to this model is the same of BSR ASCII file");
  HD.PutStruct(BysSparseReg::DocInfoStr());

  contens_.PrepareStore(10);
  AddAndPutName(contens_,BContensText,
    "ModuleType", 
    moduleType,
    "Type of BSR-ASCII module: primary, joint or master"
  );
  AddAndPutName(contens_,BContensSet,
    "DocInfo", 
    HD,
    "Header documentary information"
  );
  AddAndPutName(contens_,BContensSet,
    "LinearBlock",
    LI,  
    "Linear block information is a table with structure "
    "@BSR.LinearBlockStr"
  );
  AddAndPutName(contens_,BContensSet,
    "MissingInputBlock",
    MI,  
    "Missing Input block information is a table with structure "
    "@BSR.MissingBlockStr"
  );
  AddAndPutName(contens_,BContensSet,
    "MissingOutputBlock",
    MO,  
    "Missing Output block information is a table with structure "
    "@BSR.MissingBlockStr"
  );
  AddAndPutName(contens_,BContensSet,
    "NoiseDistrib", 
    DI,  
    "Noise distribution information is a table with structure "
    "@BSR.SigmaBlockStr"
  );
  AddAndPutName(contens_,BContensVMat,
    "Y",          
    Y,   
    "Regression output"
  );
  AddAndPutName(contens_,BContensVMat,
    "X",          
    X,   
    "Regression input"
  );
  AddAndPutName(contens_,BContensVMat,
    "a",          
    a,   
    "Restrictions border"
  );
  AddAndPutName(contens_,BContensVMat,
    "A",          
    A,   
    "Restrictions coefficients"
  );  
  contens_.PutStruct(BysSparseReg::ModelDefStr());
  BGrammar::DecLevel();
  BDir::ChangeCurrent(oldPath);
  assert(Y.Check());
  assert(X.Check());
  assert(a.Check());
  assert(A.Check());
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
int Parse_MasterModule(const BText& filePath, 
                       BSet& contens_)
////////////////////////////////////////////////////////////////////////////////
{
  int k;
  BysSparseReg::doc_info docInfo;    
  std::string sampler;
  std::vector<moduleDef> subMod;
  BSet SM, HD;
  BText oldPath = BDir::GetCurrent();
  BText dirPath = GetAbsolutePath(GetFilePath(filePath));
  BText fName = GetFileName(filePath);
  BText fullPath = dirPath+fName;
  BDir::ChangeCurrent(dirPath);
  BGrammar::IncLevel();
  Parse_Module_Master((std::string)fName.String(), docInfo, sampler, subMod);
  SM.PrepareStore(subMod.size());
  BGrammar::IncLevel();
  for(k=0; k<subMod.size(); k++)
  {
    BText moduleType(subMod[k].moduleType.c_str(),subMod[k].moduleType.size());
    BText modulePath(subMod[k].filePath.c_str(),1,subMod[k].filePath.size()-2);
    BSet module;
    Parse_Module(modulePath, moduleType, module);
    BUserSet* uSet = BContensSet::New("", module, "");
    uSet->PutName(modulePath);
    SM.AddElement(uSet);
  }
  BGrammar::DecLevel();
  HD.PrepareStore(6);
  AddAndPutName(HD,BContensText,
    "Model.Name", 
    docInfo.model_name.c_str(),
    "Model identifier"
  );
  AddAndPutName(HD,BContensText,
    "Model.Description",
    docInfo.model_description.c_str(),
    "Notes about the model"
  );
  AddAndPutName(HD,BContensText,
    "Session.Name", 
    docInfo.session_name.c_str(),
    "Session identifier"
  );
  AddAndPutName(HD,BContensText,
    "Session.Description",
    docInfo.session_description.c_str(),
    "Notes about the session"
  );
  AddAndPutName(HD,BContensText,
    "Session.Authors",     
    docInfo.session_authors.c_str(),
    "Name, e-mail, url or any identifier of session authors separated by commas"
  );
  AddAndPutName(HD,BContensDate,
    "Session.Creation",     
    DteNow(),
    "Session creation date and time"
  );
  AddAndPutName(HD,BContensText,
    "Path",       
    fullPath,
    "The root path of directory used to store all information related to this model is the same of BSR ASCII file"
  );
  HD.PutStruct(BysSparseReg::DocInfoStr());

  contens_.PrepareStore(4);
  AddAndPutName(contens_,BContensText,
    "ModuleType", 
    "master",
    "Type of BSR-ASCII module: primary, joint or master"
  );
  AddAndPutName(contens_,BContensSet,
    "DocInfo", 
    HD,
    "Header documentary information"
  );
  AddAndPutName(contens_,BContensText,
    "Modular.Schema",
    sampler.c_str(),  
    "Sampling strategy is one of these: monophasic, sequential or parallel"
  );
  AddAndPutName(contens_,BContensSet,
    "SubModule",
    SM,  
    "Set of sub modules of the master with structure "
    "@BSR.MasterSubModule"
  );
  contens_.PutStruct(BysSparseReg::MasterInfo());
  BGrammar::DecLevel();
  BDir::ChangeCurrent(oldPath);
  return(0);
};

////////////////////////////////////////////////////////////////////////////////
int Parse_Module(const BText& filePath, 
                       BText& moduleType,
                 BSet& contens_)
////////////////////////////////////////////////////////////////////////////////
{
  if(moduleType=="")
  {
    Parse_ModuleType(filePath, moduleType);
  }
  if(moduleType=="master")
  {
    return(Parse_MasterModule(filePath,contens_));
  }
  else
  {
    return(Parse_NoMasterModule(filePath,moduleType,contens_));
  }
};
/* */
};

