/* vmat_bsr.cpp: BVMat Bayesian Sparse Regression 
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
#include <tol/tol_bvmatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdir.h>

#include <iostream>

#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>
#include <boost/spirit/actor.hpp>
#include <boost/spirit/error_handling/exceptions.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include <boost/spirit/iterator/position_iterator.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/dynamic/stored_rule.hpp>

namespace BysSparseReg {

////////////////////////////////////////////////////////////////////////////////
static const BText& BSR()
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
    "Text:Session.Creation,"
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
    "Real:Prior,"
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
    "Set:Arima,"
    "Set:ArimaAuxInfo,"
    "VMatrix:Cov,"
    "VMatrix:L,"
    "VMatrix:Li,"
    "Set:EquIdx,"
    "Set:TimeInfo"
  );
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

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Information items
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
struct duplet
///////////////////////////////////////////////////////////////////////////////
{
  int    varIndex;
  double x;
  duplet()
  : varIndex(-1),
    x       (BDat::Nan())
  {}
  duplet(const duplet& aux)
  : varIndex(-1),
    x       (BDat::Nan())
  {
    copy(aux);
  }
  const duplet& operator=(const duplet& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const duplet& aux)
  {
    varIndex = aux.varIndex;
    x        = aux.x;
  }
};

///////////////////////////////////////////////////////////////////////////////
struct var_term
///////////////////////////////////////////////////////////////////////////////
{
  int    varIndex;
  int    misIndex;
  double sign;
  double coef;

  var_term()
  : varIndex(-1),
    misIndex(-1),
    sign    (1),
    coef    (1)
  {}
  var_term(const var_term& aux)
  : varIndex(-1),
    misIndex(-1),
    sign    (BDat::Nan()),
    coef    (BDat::Nan())
  {
    copy(aux);
  }
  const var_term& operator=(const var_term& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const var_term& aux)
  {
    varIndex = aux.varIndex;
    misIndex = aux.misIndex;
    sign     = aux.sign;
    coef     = aux.coef;
  }
};

///////////////////////////////////////////////////////////////////////////////
struct lin_reg_equation
///////////////////////////////////////////////////////////////////////////////
{
  int              index;
  int              resIndex;
  int              misOutputIndex;
  int              resPos;
  double           Y;
  vector<duplet>   X;

  lin_reg_equation()
  : index         (-1),
    resIndex      (-1), 
    misOutputIndex(-1),
    resPos        (-1), 
    Y             (BDat::Nan()),
    X             ()
  {}
  lin_reg_equation(const lin_reg_equation& aux)
  : index         (-1),
    resIndex      (-1), 
    misOutputIndex(-1),
    resPos        (-1), 
    Y             (BDat::Nan()),
    X             ()
  {
    copy(aux);
  }
  const lin_reg_equation& operator=(const lin_reg_equation& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const lin_reg_equation& aux)
  {
    index          = aux.index;
    resIndex       = aux.resIndex;
    misOutputIndex = aux.misOutputIndex;
    resPos         = aux.resPos;
    Y              = aux.Y;
    X              = aux.X;
  }
};

///////////////////////////////////////////////////////////////////////////////
struct lin_noise_inequation
///////////////////////////////////////////////////////////////////////////////
{
  int              index;
  double           a;
  vector<duplet>   A;
  bool             isGE;
  lin_noise_inequation()
  : index(-1),
    a    (0.0), 
    A    (),
    isGE (false)
  {}
  lin_noise_inequation(const lin_noise_inequation& aux)
  : index(-1),
    a    (0.0), 
    A    (),
    isGE (false)
  {
    copy(aux);
  }
  const lin_noise_inequation& operator=(const lin_noise_inequation& aux)
  {
    copy(aux);
    return(*this);
  }
  void copy(const lin_noise_inequation& aux)
  {
    index = aux.index;
    a     = aux.a;
    A     = aux.A;
    isGE  = aux.isGE;
  }

};

///////////////////////////////////////////////////////////////////////////////
//Actions
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
template<typename T> struct symbol_handler
///////////////////////////////////////////////////////////////////////////////
{
  vector<T>        vec;
  symbols<int>     table;
  T                info;
  int              count;
};

///////////////////////////////////////////////////////////////////////////////
template<typename T> struct add_symbol
///////////////////////////////////////////////////////////////////////////////
{
  static symbol_handler<T>* sh_;
  add_symbol() {}
  static void action() 
  {
  //Std(BText("\nadd_symbol(")+info_->name.c_str()+")");
  //info_->show();
    sh_->table.add(sh_->info.name.c_str(), sh_->vec.size());
    sh_->vec.push_back(sh_->info);
    sh_->count++;
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void  operator()(IteratorT first, IteratorT last) const { action(); }
};

template<> symbol_handler<variable_info>*  add_symbol<variable_info>  ::sh_   = NULL;
template<> symbol_handler<missing_info>*   add_symbol<missing_info>   ::sh_   = NULL;
template<> symbol_handler<sigma_info>*     add_symbol<sigma_info>     ::sh_   = NULL;
template<> symbol_handler<noise_info>* add_symbol<noise_info> ::sh_   = NULL;


///////////////////////////////////////////////////////////////////////////////
struct assign_noise_size
///////////////////////////////////////////////////////////////////////////////
{
  static noise_info* noise_info_; 
  static int*            var_count_; 
  static int*            numEqu_; 
  
  static void action(const int size)
  { 
    noise_info_->size = size;
    noise_info_->dating = "";
    noise_info_->firstDate = "";
    if(!(*numEqu_))
    {
      Std(BSR()+" Parsed "+(*var_count_)+" linear block variables \n");
    }
    (*numEqu_) += size;
  }
  void operator()(const int size) const
  {
    action(size);
  }
  static void action(const std::string& str)
  {
    action(atoi(str.c_str()));
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};
noise_info* assign_noise_size::noise_info_  = NULL; 
int*            assign_noise_size::numEqu_    = NULL; 
int*            assign_noise_size::var_count_ = NULL; 


///////////////////////////////////////////////////////////////////////////////
struct assign_sigma_to_res
///////////////////////////////////////////////////////////////////////////////
{
  static symbols<int>*       sig_table_;
  static vector<sigma_info>* sig_vec_;
  static noise_info*     noise_info_; 
  static int*                numEqu_; 

  assign_sigma_to_res() {}

  static void action(sigma_info& sigInf)
  {
  //Std(BText("\nassign_sigma_to_res(")+str.c_str()+")");
  //Std(BText("sigmaIdx=")+sigInf->index);
    noise_info_->sigmaName  = sigInf.name;
    noise_info_->sigmaIdx   = sigInf.index;
    noise_info_->sigmaCte   = BDat::Nan();
    noise_info_->covExpr    = "";
    noise_info_->cov.Eye(noise_info_->size,noise_info_->size,0,1);
    noise_info_->covIsDiag=true;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*sig_vec_)[n]);
  }

  static void action(const std::string& str)
  {
  //Std(BText("\nassign_sigma_to_res(")+str.c_str()+")");
    int* pos = find(*sig_table_,str.c_str());
    assert(pos);
    if(pos)
    {
      sigma_info& sigInf = (*sig_vec_)[*pos];
      action(sigInf);
    }
    else
    {
      Error(BText("Cannot find Sigma ")+str.c_str());
      noise_info_->sigmaIdx = -1;
    }   
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }

};

symbols<int>*       assign_sigma_to_res::sig_table_ = NULL;
vector<sigma_info>* assign_sigma_to_res::sig_vec_   = NULL;
noise_info*     assign_sigma_to_res::noise_info_  = NULL; 
int*                assign_sigma_to_res::numEqu_    = NULL; 

///////////////////////////////////////////////////////////////////////////////
struct assign_const_sigma_to_res
///////////////////////////////////////////////////////////////////////////////
{
  static noise_info* noise_info_; 
  static int*            numEqu_; 
  assign_const_sigma_to_res() {}
  static void action(const double d)
  {
  //Std(BText("\nassign_const_sigma_to_res(")+d+")");
    noise_info_->sigmaName  = "";
    noise_info_->sigmaIdx   = -1;
    noise_info_->sigmaCte   = d;
    noise_info_->covExpr    = "";
    noise_info_->cov.Eye(noise_info_->size,noise_info_->size,0,noise_info_->sigmaCte);
    noise_info_->covIsDiag=true;
  //Std(BText(" sigmaIdx=-1"));
  //Std(BText(" sigmaCte=")+noise_info_->sigmaCte);
  }
  void operator()(const double d) const
  {
    action(d);
  }
  static void action(const std::string& str)
  {
    action(atof(str.c_str()));
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

noise_info* assign_const_sigma_to_res::noise_info_  = NULL; 
int*            assign_const_sigma_to_res::numEqu_    = NULL; 

///////////////////////////////////////////////////////////////////////////////
struct assign_covariance_to_res
///////////////////////////////////////////////////////////////////////////////
{
  static noise_info* noise_info_; 
  static int*            numEqu_; 
  assign_covariance_to_res() {}
  static void action(const std::string& str)
  {
  //Std(BText("\nassign_const_sigma_to_res(")+str.c_str()+")");
    noise_info_->sigmaName  = "";
    noise_info_->sigmaIdx   = -2;
    noise_info_->sigmaCte   = 1;
    noise_info_->covExpr    = str;
    BUserVMat* u = (BUserVMat*)GraVMatrix()->EvaluateExpr(str.c_str());
    int err = 0;
    if(!u)
    {
      err = -999999;
    }
    else
    {
      noise_info_->cov = u->Contens();
      noise_info_->covIsDiag=noise_info_->cov.IsDiagonal();
    }
    if(err)
    {
      Error(BText("Not valid covariance definition ")+str.c_str());
    }
  //Std(BText(" sigmaIdx=-1"));
  //Std(BText(" sigmaCte=")+noise_info_->sigmaCte);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

noise_info* assign_covariance_to_res::noise_info_  = NULL; 
int*            assign_covariance_to_res::numEqu_    = NULL; 

///////////////////////////////////////////////////////////////////////////////
struct assign_pos_sign_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
  static var_term* var_term_info_; 
  assign_pos_sign_to_equ_term() {}
  static void action() { var_term_info_->sign = 1.0; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
var_term* assign_pos_sign_to_equ_term::var_term_info_ = NULL;
///////////////////////////////////////////////////////////////////////////////
struct assign_neg_sign_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
  static var_term* var_term_info_; 
  assign_neg_sign_to_equ_term() {}
  static void action() { var_term_info_->sign = -1.0; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
var_term* assign_neg_sign_to_equ_term::var_term_info_ = NULL;
///////////////////////////////////////////////////////////////////////////////
struct assign_pos_sign_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
  static var_term* var_term_info_; 
  assign_pos_sign_to_ine_term() {}
  static void action() { var_term_info_->sign = 1.0; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
var_term* assign_pos_sign_to_ine_term::var_term_info_ = NULL;
///////////////////////////////////////////////////////////////////////////////
struct assign_neg_sign_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
  static var_term* var_term_info_; 
  assign_neg_sign_to_ine_term() {}
  static void action() { var_term_info_->sign = -1.0; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
var_term* assign_neg_sign_to_ine_term::var_term_info_ = NULL;
///////////////////////////////////////////////////////////////////////////////
struct assign_IsGE_to_ine
///////////////////////////////////////////////////////////////////////////////
{
  static lin_noise_inequation* ine_; 
  assign_IsGE_to_ine() {}
  static void action() { ine_->isGE = true; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
lin_noise_inequation* assign_IsGE_to_ine::ine_ = NULL;
///////////////////////////////////////////////////////////////////////////////
struct assign_IsLE_to_ine
///////////////////////////////////////////////////////////////////////////////
{
  static lin_noise_inequation* ine_; 
  assign_IsLE_to_ine() {}
  static void action() { ine_->isGE = false; }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};
lin_noise_inequation* assign_IsLE_to_ine::ine_ = NULL;


///////////////////////////////////////////////////////////////////////////////
struct assign_var_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
  static vector<variable_info>* var_vec_; 
  static var_term*              var_term_info_; 
  static int*                   nzmax_;

  assign_var_to_equ_term() {}
  static void action(variable_info& varInfo)
  {
    varInfo.used = true;
    var_term_info_->varIndex = varInfo.index;
    var_term_info_->misIndex = -1;
    (*nzmax_)++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*var_vec_)[n]);
  }
};

vector<variable_info>* assign_var_to_equ_term::var_vec_       = NULL;
var_term*              assign_var_to_equ_term::var_term_info_ = NULL;
int*                   assign_var_to_equ_term::nzmax_         = NULL;


///////////////////////////////////////////////////////////////////////////////
struct assign_mis_to_equ_out
///////////////////////////////////////////////////////////////////////////////
{
  static vector<missing_info>*  mis_vec_; 
  static lin_reg_equation*      equ_info_;

  assign_mis_to_equ_out() {}

  static void action(missing_info& misInfo)
  {
    if(misInfo.used)
    {
      Error(BText("Missing variable ")+misInfo.name.c_str()+" is already "
            "in use at equation "+misInfo.row);
    }
    else
    {
      misInfo.used = true;
      equ_info_->misOutputIndex = misInfo.index;
      misInfo.row = equ_info_->index+1;
      misInfo.col = 0;
    }
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*mis_vec_)[n]);
  }
};
      
vector<missing_info>*  assign_mis_to_equ_out::mis_vec_       = NULL; 
lin_reg_equation*      assign_mis_to_equ_out::equ_info_      = NULL;

///////////////////////////////////////////////////////////////////////////////
struct assign_mis_to_var_term
///////////////////////////////////////////////////////////////////////////////
{
  static vector<missing_info>* mis_vec_; 
  static var_term*             var_term_info_; 

  assign_mis_to_var_term() {}
  static void action(missing_info& misInfo)
  {
    var_term_info_->misIndex = misInfo.index;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*mis_vec_)[n]);
  }
};

vector<missing_info>*  assign_mis_to_var_term::mis_vec_       = NULL; 
var_term*              assign_mis_to_var_term::var_term_info_ = NULL; 

///////////////////////////////////////////////////////////////////////////////
struct assign_mis_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
  static vector<missing_info>*  mis_vec_; 
  static vector<variable_info>* var_vec_; 
  static var_term*              var_term_info_; 
  static lin_reg_equation*      equ_info_;
  static int*                   nzmax_;

  assign_mis_to_equ_term() {}
  static void action(variable_info& varInfo)
  {
    var_term_info_->varIndex = varInfo.index;
    missing_info& mis_info = (*mis_vec_)[var_term_info_->misIndex-1];
    mis_info.row = equ_info_->index;
    mis_info.col = varInfo.index;
    (*nzmax_)++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*var_vec_)[n]);
  }

};
      
vector<missing_info>*  assign_mis_to_equ_term::mis_vec_       = NULL; 
vector<variable_info>* assign_mis_to_equ_term::var_vec_       = NULL; 
var_term*              assign_mis_to_equ_term::var_term_info_ = NULL; 
lin_reg_equation*      assign_mis_to_equ_term::equ_info_      = NULL;
int*                   assign_mis_to_equ_term::nzmax_         = NULL;

///////////////////////////////////////////////////////////////////////////////
struct assign_var_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
  static vector<variable_info>* var_vec_; 
  static var_term*              var_term_info_; 
  static int*                   nzmax_;

  assign_var_to_ine_term() {}
  static void action(variable_info& varInfo)
  {
    var_term_info_->varIndex = varInfo.index;
    (*nzmax_)++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*var_vec_)[n]);
  }
};

vector<variable_info>* assign_var_to_ine_term::var_vec_       = NULL;
var_term*              assign_var_to_ine_term::var_term_info_ = NULL;
int*                   assign_var_to_ine_term::nzmax_         = NULL;

///////////////////////////////////////////////////////////////////////////////
struct assign_noise_to_term
///////////////////////////////////////////////////////////////////////////////
{
  static vector<noise_info>* noise_vec_;
  static symbols<int>*         noise_table_;
  static lin_reg_equation*     equ_; 
  static vector<sigma_info>*   sig_vec_;
  assign_noise_to_term() {}
  static void action(noise_info& resInf)
  {
    equ_->resIndex = resInf.index;
    resInf.equIdx.push_back(equ_->index);
    if(resInf.sigmaIdx>=0)
    {
      sigma_info& sig = (*sig_vec_)[resInf.sigmaIdx-1];
      sig.nzmax++;
    }
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*noise_vec_)[n]);
  }
/*
  static void action(const std::string& str)
  {
  //Std(BText("\nassign_noise_to_term(")+str.c_str()+")");
    int* pos = find(*noise_table_,str.c_str());
    assert(pos);
    if(pos)
    {
    //Std(BText("\tsigmaIndex=")+resInf->sigmaIdx);
    //Std(BText("\tindex=")+resInf->index);
      noise_info& resInf = (*noise_vec_)[*pos];
      equ_->resIndex = resInf.index;
      resInf.equIdx.push_back(equ_->index);
      if(resInf.sigmaIdx>=0)
      {
        sigma_info& sig = (*sig_vec_)[resInf.sigmaIdx-1];
        sig.nzmax++;
      }
    }
    else
    {
      Error(BText("Cannot find Noise ")+str.c_str());
      //Error: Not exists
      equ_->resIndex = -1;
    }
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
*/
};

vector<noise_info>*  assign_noise_to_term::noise_vec_   = NULL;
symbols<int>*        assign_noise_to_term::noise_table_ = NULL;
lin_reg_equation*    assign_noise_to_term::equ_         = NULL;
vector<sigma_info>*  assign_noise_to_term::sig_vec_     = NULL;

///////////////////////////////////////////////////////////////////////////////
struct add_term_to_equ
///////////////////////////////////////////////////////////////////////////////
{
  static vector<duplet>* X_;
  static var_term* var_term_info_; 
  add_term_to_equ() {}
  static void action()
  {
    int n = X_->size();
    if(X_->capacity()<n+1) { X_->reserve(int((n+1)*1.20)); }
    X_->resize(n+1);
    (*X_)[n].varIndex = var_term_info_->varIndex;
    (*X_)[n].x        = var_term_info_->sign * var_term_info_->coef;
    var_term_info_->varIndex = -1;
    var_term_info_->sign = 1;
    var_term_info_->coef = 1;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }
};

vector<duplet>* add_term_to_equ::X_             = NULL;
var_term*       add_term_to_equ::var_term_info_ = NULL; 

///////////////////////////////////////////////////////////////////////////////
struct add_term_to_ine
///////////////////////////////////////////////////////////////////////////////
{
  static vector<duplet>* X_;
  static var_term* var_term_info_; 
  add_term_to_ine() {}
  static void action()
  {
    int n = X_->size();
    if(X_->capacity()<n+1) { X_->reserve(int((n+1)*1.20)); }
    X_->resize(n+1);
    (*X_)[n].varIndex = var_term_info_->varIndex;
    (*X_)[n].x        = var_term_info_->sign * var_term_info_->coef;
    var_term_info_->varIndex = -1;
    var_term_info_->sign = 1;
    var_term_info_->coef = 1;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }
};

vector<duplet>* add_term_to_ine::X_             = NULL;
var_term*       add_term_to_ine::var_term_info_ = NULL; 


///////////////////////////////////////////////////////////////////////////////
struct add_equ
///////////////////////////////////////////////////////////////////////////////
{
  static vector<lin_reg_equation>* equ_vec_;
  static lin_reg_equation*         equ_;
  static int*                      numEqu_; 
  static int*                      mis_count_; 
  static int*                      sig_count_; 
  static int*                      noise_count_; 
  static double                    iniTime_ ;
  static int                       oldRatio_;

  add_equ() {}
  static void action() 
  {
    int n = equ_vec_->size();
    if(!n) 
    { 
      iniTime_ = (double)time(0); 
      Std(BSR()+" Parsed "+(*mis_count_)+" missing variables \n");
      Std(BSR()+" Parsed "+(*sig_count_)+" sigma variables \n");
      Std(BSR()+" Parsed "+(*noise_count_)+" vectors of noise  \n");
    }
    if(equ_vec_->capacity()<n+1) { equ_vec_->reserve(int((n+1)*1.20)); }
    equ_vec_->resize(n+1);
    (*equ_vec_)[n].index    = equ_->index;
    (*equ_vec_)[n].resIndex = equ_->resIndex;
    (*equ_vec_)[n].resPos   = equ_->resPos;
    (*equ_vec_)[n].Y        = equ_->Y;
    (*equ_vec_)[n].X        = equ_->X;

    equ_->resIndex = -1;
    equ_->Y = BDat::Nan();
    equ_->X.clear();
    int ratio = (100*n) / (*numEqu_);
    if(n && (ratio!=oldRatio_) && !(ratio%10))
    {
      oldRatio_ = ratio;
      double elapsed = time(0) - iniTime_;
      double remain  = elapsed*double((*numEqu_)-n)/double(n);
      Std(BSR()+" Parsed "+n+" equations of "+(*numEqu_)+" ["+ratio+"%] "
          "in "+elapsed+" seconds (remaining "+remain+" s.)\n");
    }
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};

vector<lin_reg_equation>* add_equ::equ_vec_   = NULL;
lin_reg_equation*         add_equ::equ_       = NULL;
int*                      add_equ::numEqu_    = NULL;
int*                      add_equ::mis_count_ = NULL;
int*                      add_equ::sig_count_ = NULL;
int*                      add_equ::noise_count_ = NULL;
double                    add_equ::iniTime_   = 0.0;
int                       add_equ::oldRatio_  = 0;

///////////////////////////////////////////////////////////////////////////////
struct add_ine
///////////////////////////////////////////////////////////////////////////////
{
  static vector<lin_noise_inequation>* ine_vec_;
  static lin_noise_inequation*         ine_;
  add_ine() {}
  static void action() 
  {
    int n = ine_vec_->size();
    if(ine_vec_->capacity()<n+1) { ine_vec_->reserve(int((n+1)*1.20)); }
    ine_vec_->resize(n+1);
    (*ine_vec_)[n].index = ine_->index;
    (*ine_vec_)[n].isGE  = ine_->isGE;
    (*ine_vec_)[n].a     = ine_->a;
    (*ine_vec_)[n].A     = ine_->A;
    ine_->a = BDat::Nan();
    ine_->A.clear();
    if(n && !(n%1000))
    {
      Std(BSR()+" Parsed "+n+" inequations \n");
    }
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};

vector<lin_noise_inequation>* add_ine::ine_vec_ = NULL;
lin_noise_inequation*         add_ine::ine_     = NULL;


///////////////////////////////////////////////////////////////////////////////
struct assign_explicit_end
///////////////////////////////////////////////////////////////////////////////
{
  static bool* endFound_; 
  
  static void action() 
  { 
    (*endFound_) = true;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }

};

bool* assign_explicit_end::endFound_ = NULL; 

///////////////////////////////////////////////////////////////////////////////
//  Here's our comment rule
struct skip_grammar : public grammar<skip_grammar>
///////////////////////////////////////////////////////////////////////////////
{
  template <typename ScannerT>
  struct definition
  {
    definition(skip_grammar const& /*self*/)
    {
      skip
          =   space_p
          |   comment_p("//")  // C++ comment
          |   comment_p("/*", "*/")  // C comment
          //VBR: Skipping NodeName:: to allow reading BLR2HLM format 
//          |  (lexeme_d[((alpha_p | '_' ) >> *(alnum_p | '_' | '.'))] >> str_p("::"))
          ;
    }
    rule<ScannerT> skip;
    rule<ScannerT> const&
    start() const { return skip; }
  };
};



///////////////////////////////////////////////////////////////////////////////
void show_parser_error(char const* msg, file_position const& lc, bool end)
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

///////////////////////////////////////////////////////////////////////////////
struct error_report_parser 
///////////////////////////////////////////////////////////////////////////////
{
  std::string msg_;
  error_report_parser(std::string msg) : msg_(msg) {}
  typedef nil_t result_t;
  template <typename ScannerT>

  int operator()(ScannerT const& scan, result_t& /*result*/) const
  {
    file_position fpos = scan.first.get_position();
    show_parser_error(msg_.c_str(), fpos, scan.at_end());
    return -1; // Fail.
  }
};

typedef functor_parser<error_report_parser> error_report_p;


///////////////////////////////////////////////////////////////////////////////
struct bys_sparse_reg : public grammar<bys_sparse_reg>
///////////////////////////////////////////////////////////////////////////////
{
  ifstream* file;
  string    fileName;
  size_t    fileSize;

  doc_info docInfo;

  symbol_handler<variable_info>  var;
  symbol_handler<missing_info>   mis;
  symbol_handler<sigma_info>     sig;
  symbol_handler<noise_info> noise;

  vector<lin_reg_equation>   equ_vec;
  vector<lin_noise_inequation> ine_vec;
  lin_reg_equation           equ_info;
  lin_noise_inequation         ine_info;

  var_term equ_var_term_info;
  var_term ine_var_term_info;
  double   realValue;
  int Xnzmax_;
  int Anzmax_;
  int numEqu_;

  bool endFound_;

  bys_sparse_reg()
  : Xnzmax_  (0),
    Anzmax_  (0),
    numEqu_  (0),
    endFound_ (false)
  {
    var.count = 0;
    mis.count = 0;
    sig.count = 0;
    noise.count = 0;
    var.info.index = 0;
    mis.info.index = 0;
    sig.info.index = 0;
    noise.info.index = 0;
    equ_info.index = 0;
    ine_info.index = 0;
    noise.info.sigmaIdx=-1;
  }
    
  template <typename ScannerT>
  struct definition
  {
    bys_sparse_reg& s;
    rule<ScannerT>  
      variantSigma, constantSigma, covariance, 
      noiseName, noisePosition,
      posSign, negSign, unknown, product,
      initValue, like, power2, member,
      noiseSize, normalDist, normalNu, eq, le, ge,
      argumentSeparator, endOfSentence, 
      openParenthesys, closeParenthesys,
      openBracket, closeBracket,
      openKey, closeKey,
      identifier_simple, identifier,  newIdentifier, 
      resSigmaDef, errorTerm, 
      signEquVarTerm, signIneVarTerm, 
      knownReal, unkOrReal, 
      unkOrRealInitValue, 
      knownURealEquTerm, 
      knownURealIneTerm,
      equVarTerm, ineVarTerm, 
      variable, missing, arima, noise, equation, inequation,
      model_nameDef, model_descriptionDef, 
      session_nameDef, session_descriptionDef, session_authorDef, 
      explicit_begin, explicit_end, problem;

    definition(const bys_sparse_reg& s_)
    : s ((bys_sparse_reg&)s_)
    {
      add_symbol<variable_info>::sh_ = &s.var;
      add_symbol<variable_info> add_var;

      add_symbol<missing_info>::sh_ = &s.mis;
      add_symbol<missing_info> add_mis;

      add_symbol<sigma_info>::sh_ = &s.sig;
      add_symbol<sigma_info> add_sig;

      add_symbol<noise_info>::sh_ = &s.noise;
      add_symbol<noise_info> add_res;

      assign_noise_size::noise_info_  = &s.noise.info; 
      assign_noise_size::numEqu_    = &s.numEqu_;
      assign_noise_size::var_count_ = &s.var.count;
      assign_noise_size assign_noise_size_;

      assign_sigma_to_res::sig_vec_   = &s.sig.vec;
      assign_sigma_to_res::sig_table_ = &s.sig.table;
      assign_sigma_to_res::noise_info_  = &s.noise.info; 
      assign_sigma_to_res::numEqu_    = &s.numEqu_;
      assign_sigma_to_res assign_sigma_to_noise_;

      assign_const_sigma_to_res::noise_info_ = &s.noise.info;
      assign_const_sigma_to_res::numEqu_   = &s.numEqu_;
      assign_const_sigma_to_res assign_const_sigma_to_noise_;
    
      assign_covariance_to_res::noise_info_ = &s.noise.info;
      assign_covariance_to_res::numEqu_   = &s.numEqu_;
      assign_covariance_to_res assign_covariance_to_noise_;

      assign_pos_sign_to_equ_term::var_term_info_ = &s.equ_var_term_info; 
      assign_pos_sign_to_equ_term assign_pos_sign_to_equ_term_;
      assign_neg_sign_to_equ_term::var_term_info_ = &s.equ_var_term_info; 
      assign_neg_sign_to_equ_term assign_neg_sign_to_equ_term_;

      assign_var_to_equ_term::var_vec_       = &s.var.vec;
      assign_var_to_equ_term::var_term_info_ = &s.equ_var_term_info; 
      assign_var_to_equ_term::nzmax_         = &s.Xnzmax_;
      assign_var_to_equ_term assign_var_to_equ_term_;

      assign_mis_to_equ_out::mis_vec_   = &s.mis.vec; 
      assign_mis_to_equ_out::equ_info_  = &s.equ_info;
      assign_mis_to_equ_out assign_mis_to_equ_out_;

      assign_mis_to_var_term::mis_vec_       = &s.mis.vec;
      assign_mis_to_var_term::var_term_info_ = &s.equ_var_term_info; 
      assign_mis_to_var_term assign_mis_to_var_term_;

      assign_mis_to_equ_term::mis_vec_       = &s.mis.vec;
      assign_mis_to_equ_term::var_vec_       = &s.var.vec;
      assign_mis_to_equ_term::var_term_info_ = &s.equ_var_term_info;
      assign_mis_to_equ_term::equ_info_      = &s.equ_info;
      assign_mis_to_equ_term::nzmax_         = &s.Xnzmax_;
      assign_mis_to_equ_term assign_mis_to_equ_term_;

      assign_pos_sign_to_ine_term::var_term_info_ = &s.ine_var_term_info; 
      assign_pos_sign_to_ine_term assign_pos_sign_to_ine_term_;
      assign_neg_sign_to_ine_term::var_term_info_ = &s.ine_var_term_info; 
      assign_neg_sign_to_ine_term assign_neg_sign_to_ine_term_;

      assign_IsGE_to_ine::ine_ = &s.ine_info;
      assign_IsGE_to_ine assign_IsGE_to_ine_;
              
      assign_IsLE_to_ine::ine_ = &s.ine_info;
      assign_IsLE_to_ine assign_IsLE_to_ine_;
              
      assign_var_to_ine_term::var_vec_       = &s.var.vec;
      assign_var_to_ine_term::var_term_info_ = &s.ine_var_term_info; 
      assign_var_to_ine_term::nzmax_         = &s.Anzmax_;
      assign_var_to_ine_term assign_var_to_ine_term_;

      assign_noise_to_term::noise_vec_   = &s.noise.vec;
      assign_noise_to_term::noise_table_ = &s.noise.table;
      assign_noise_to_term::equ_       = &s.equ_info;
      assign_noise_to_term::sig_vec_   = &s.sig.vec;
      assign_noise_to_term assign_noise_to_term_;
      
      add_term_to_equ::X_             = &s.equ_info.X;
      add_term_to_equ::var_term_info_ = &s.equ_var_term_info; 
      add_term_to_equ add_term_to_equ_;
      
      add_term_to_ine::X_             = &s.ine_info.A;
      add_term_to_ine::var_term_info_ = &s.ine_var_term_info; 
      add_term_to_ine add_term_to_ine_;

      add_equ::equ_vec_   = &s.equ_vec;
      add_equ::equ_       = &s.equ_info;
      add_equ::numEqu_    = &s.numEqu_;
      add_equ::mis_count_ = &s.mis.count;
      add_equ::sig_count_ = &s.sig.count;
      add_equ::noise_count_ = &s.noise.count;
      
      add_equ add_equ_;

      add_ine::ine_vec_ = &s.ine_vec;
      add_ine::ine_     = &s.ine_info;
      add_ine add_ine_;
       
      assign_explicit_end::endFound_ = &s.endFound_;
      assign_explicit_end assign_explicit_end_;

      error_report_p error_badNumber = error_report_parser(
        "Expecting a number, but found something else" );
      error_report_p error_badNumberOrUnk = error_report_parser(
        "Expecting a number or unknown symbol '?', but found something else" );
      error_report_p error_badIdentifier = error_report_parser(
        "Expecting a valid identifier, but found something else" );
      error_report_p error_alreadyInUse = error_report_parser(
        "Identifier is already in use" );
      error_report_p error_intExpected = error_report_parser(
        "Expected integer number" );

      error_report_p error_ModelNameDefExpected = error_report_parser(
        "Expected model name definition 'Model.Name = \"...\"' " );
      error_report_p error_ModelDescriptionDefExpected = error_report_parser(
        "Expected model description definition 'Model.Description = \"...\"' " );
      error_report_p error_SessionNameDefExpected = error_report_parser(
        "Expected session name definition 'Session.Name = \"...\"' " );
      error_report_p error_SessionDescriptionDefExpected = error_report_parser(
        "Expected session description definition 'Session.Description = \"...\"' " );
      error_report_p error_SessionAuthorDefExpected = error_report_parser(
        "Expected session authors definition 'Session.Authors = \"...\"' " );

      error_report_p error_AssignExpected = error_report_parser(
        "Expected symbol '=' " );
      error_report_p error_cmpEquExpected = error_report_parser(
        "Expected symbol '==' " );
      error_report_p error_cmpIneExpected = error_report_parser(
        "Expected symbol '<=' or '>=' " );
      error_report_p error_signExpected = error_report_parser(
        "Expected symbol '+' or '-' " );
      error_report_p error_prodExpected = error_report_parser(
        "Expected symbol '*' " );
      error_report_p error_power2Expected = error_report_parser(
        "Expected symbols '^2' " );
      error_report_p error_comma1Expected = error_report_parser(
        "Expected symbol ',' " );
      error_report_p error_comma2Expected = error_report_parser(
        "Expected symbol ';' " );
      error_report_p error_likeExpected = error_report_parser(
        "Expected symbol '~' " );
      error_report_p error_initValExpected = error_report_parser(
        "Expected symbol '<-' " );
      error_report_p error_openParenthesysExpected = error_report_parser(
        "Expected symbol '(' " );
      error_report_p error_closeParenthesysExpected = error_report_parser(
        "Expected symbol ')' " );
      error_report_p error_openBracketExpected = error_report_parser(
        "Expected symbol '[' " );
      error_report_p error_closeBracketExpected = error_report_parser(
        "Expected symbol ']' " );
      error_report_p error_openKeyExpected = error_report_parser(
        "Expected symbol '{' " );
      error_report_p error_closeKeyExpected = error_report_parser(
        "Expected symbol '}' " );
      error_report_p error_noiseNameExpected = error_report_parser(
        "Expected noise name: 'Normal' " );
      error_report_p error_fromExpected = error_report_parser(
        "Expected from clause after dating declaration" );
      error_report_p error_untilExpected = error_report_parser(
        "Expected until clause after dating and from declarations" );
      error_report_p error_linBlkVarExpected = error_report_parser(
        "Expected LinearBlock variable name " );
      error_report_p error_noiseExpected = error_report_parser(
        "Expected Noise name " );
      error_report_p error_sigmaExpected = error_report_parser(
        "Expected Sigma name or constant positive real number " );
      error_report_p error_equTermExpected = error_report_parser(
        "Expected equation term, invalid syntax or non declared variable:\n"
        "  {{number|missing_block_variable}*}lineal_block_variable " );
      error_report_p error_ineTermExpected = error_report_parser(
        "Expected inequation term, invalid syntax or non declared variable:\n"
        "  {number*}lineal_block_variable " );

      std::string linearVariableDeclare_syntax = 
        "Linear block variable declaration with initial values:\n"
        "  my_linear_block_name <- number; \n";
      error_report_p error_linearVariableDeclareExpected = error_report_parser(
        string("Expected syntax was: \n")+linearVariableDeclare_syntax);

      std::string missingVariableDeclare_syntax = 
        "Missing block variable declaration with initial values:\n"
        "  my_linear_block_name ? <- number; \n"
        "Missing block variable declaration with prior info:\n"
        "  my_linear_block_name ? ~ {Normal(nu,sigma^2)|TruncatedNormal(nu,sigma^2,minBound,maxBound)}; \n";
      error_report_p error_missingVariableDeclareExpected = error_report_parser(
        string("Expected syntax was: \n")+missingVariableDeclare_syntax);

      std::string noiseDistribDeclare_syntax = 
        "White noise distribution declaration with known homocedastic diagonal covariance:\n"
        "  my_noise_block_name[length] ~ Normal(0,sigma^2);\n"
        "White noise distribution declaration with known heterocedastic diagonal covariance:\n"
        "  my_noise_block_name[length] ~ Normal(0,SetDiag([[sigma_1^2, sigma_2^2, ...]]));\n"
        "White noise distribution declaration with unknown variance:\n"
        "  my_noise_block_name[length] ~ Normal(0,my_sigma_block_name);\n"
        "ARMA noise distribution declaration with unknown variance:\n"
        "  my_noise_block_name[length] ~ Normal(0,my_sigma_block_name * ARMA.COV([[ARIMAStruct(period,ar,ma,dif),...]]));\n";
      error_report_p error_noiseDistribDeclareExpected = error_report_parser(
        string("Expected syntax was: \n")+noiseDistribDeclare_syntax);

      std::string linearRegressionEquationDeclare_syntax = 
        "Linear regressing equation:\n"
        "  {number|?} == my_noise_block_name[index] "
        "{+|-}{{number|missing_block_name}*}my_linear_block_name "
        "{+|-}{{number|missing_block_name}*}my_linear_block_name  ... ;\n";
      error_report_p error_linearRegressionEquationDeclareExpected = error_report_parser(
        string("Expected regression equation, invalid syntax or non declared variable or noise\n"
               "Expected syntax was: \n")+linearRegressionEquationDeclare_syntax);

      std::string linearConstrainInequationDeclare_syntax = 
        "Linear constraining inequation\n"
        "  number {<=|>=} {+|-}{{number}*}my_linear_block_name "
                         "{+|-}{{number}*}my_linear_block_name ... ;\n";
      error_report_p error_linearConstrainInequationDeclareExpected = error_report_parser(
        string("Expected constrain inequation, invalid syntax or non declared variable\n"
               "Expected syntax was: \n")+linearConstrainInequationDeclare_syntax);

        error_report_p error_declarationExpected = error_report_parser(string("")+
        "Unexpected symbol not matching none of BSR (Bayesian Sparse Regression) "
        "declaration syntax options: \n"+
        linearVariableDeclare_syntax+
        missingVariableDeclare_syntax+
        noiseDistribDeclare_syntax+
        linearRegressionEquationDeclare_syntax+
        linearConstrainInequationDeclare_syntax);
      
      argumentSeparator = (ch_p(',') | error_comma1Expected)
        ;
      endOfSentence = (ch_p(';') | error_comma2Expected)
        ;
      openParenthesys = (ch_p('(') | error_openParenthesysExpected)
        ;
      closeParenthesys = (str_p(")") | error_closeParenthesysExpected)
        ;
      openBracket = (str_p("[") | error_openBracketExpected)
        ;
      closeBracket = (str_p("]") | error_closeBracketExpected)
        ;
      openKey = (str_p("{") | error_openKeyExpected)
        ;
      closeKey = (str_p("}") | error_closeKeyExpected)
        ;
      initValue = (str_p("<-") | error_initValExpected)
        ;
      like = (str_p("~") | error_likeExpected )
        ;
      power2 = ((str_p("^")>>str_p("2")) | error_power2Expected )
        ;
      normalDist = (str_p("Normal") | error_noiseNameExpected )
        ;
      posSign = str_p("+")
        ;
      negSign = str_p("-")
        ;
      unknown = str_p("?")
        ;
      product = str_p("*")
        ;
      member = str_p("::")
        ;
      eq = str_p("==")
        ;
      le = str_p("<=")
        ;
      ge = str_p(">=")
        ;
  // TOL identifiers
      identifier_simple =
        lexeme_d[((alpha_p | '_' ) >> *(alnum_p | '_' | '.'))]
        ;
  // TOL or NameBlock identifiers
      identifier =
        (*(identifier_simple >> member)) >> identifier_simple;
        ;
      newIdentifier = 
        (
          (identifier - (s.var.table | s.sig.table | s.mis.table | s.noise.table)) |
          (identifier >> error_alreadyInUse >> nothing_p >> end_p)
        )
        ;
      variantSigma = 
        (newIdentifier[increment_a(s.sig.info.index)]
          [assign_a(s.sig.info.name)])[add_sig][assign_sigma_to_noise_]
        ;
      constantSigma = 
        ureal_p[assign_const_sigma_to_noise_]
        ;
      covariance = 
        (
          (
            str_p("SetDiag") >> 
            confix_p("([[", (*(anychar_p)), "]])")
          )
          |
          (
            str_p("Diag") >> 
            confix_p("(", (*(anychar_p)), ")")
          ) 
        )
        [assign_covariance_to_noise_]
        ;
      arima = 
        (
          (
            str_p("*") >>
            str_p("ARIMA.COV") >> 
            confix_p("([[", (*(anychar_p)), "]])")[assign_a(s.noise.info.arimaExpr)]
          )
          |
          eps_p[assign_a(s.noise.info.arimaExpr,"")]
        )
        ;
      resSigmaDef = 
        (
          covariance | 
          constantSigma | 
          variantSigma | 
          error_sigmaExpected
        ) >>
        arima
        ;
      noiseName=
        (s.noise.table[assign_noise_to_term_]|error_noiseExpected)
        ;
      noisePosition =
        openBracket >>
          (int_p[assign_a(s.equ_info.resPos)] | error_intExpected) >> 
        closeBracket
        ;
      errorTerm = noiseName >> noisePosition
        ;
      signEquVarTerm = 
        posSign[assign_pos_sign_to_equ_term_] 
        | 
        negSign[assign_neg_sign_to_equ_term_]
        ;
      signIneVarTerm = 
        posSign[assign_pos_sign_to_ine_term_] 
        | 
        negSign[assign_neg_sign_to_ine_term_]
        ;
      knownReal =
        real_p[assign_a(s.realValue)]
        ;
      unkOrReal =
        unknown[assign_a(s.realValue,BDat::Nan())]
        |
        knownReal
        ;
      knownURealEquTerm =
        ureal_p[assign_a(s.equ_var_term_info.coef)]
        ;
      knownURealIneTerm =
        ureal_p[assign_a(s.ine_var_term_info.coef)]
        ;
      unkOrRealInitValue =
        unknown[assign_a(s.var.info.initValue,BDat::Nan())]
        |
        real_p[assign_a(s.var.info.initValue)]
        ;
      equVarTerm = 
        signEquVarTerm >>
        (
          s.var.table[assign_var_to_equ_term_]
          |
          (
            knownURealEquTerm >>
            (product                              | error_prodExpected     ) >> 
            (s.var.table[assign_var_to_equ_term_] | error_linBlkVarExpected)
          )
          |
          (
            s.mis.table[assign_mis_to_var_term_] >>
            (product                              | error_prodExpected     ) >> 
            (s.var.table[assign_mis_to_equ_term_] | error_linBlkVarExpected)
          )
        )
        ;
      ineVarTerm = 
        signIneVarTerm >>
        (
          s.var.table[assign_var_to_ine_term_]
          |
          (
            knownURealIneTerm >>
            (product                              | error_prodExpected     ) >> 
            (s.var.table[assign_var_to_ine_term_] | error_linBlkVarExpected)
          )
        )
        ;
      variable =
        newIdentifier[increment_a(s.var.info.index)]
                            [assign_a(s.var.info.name)] >> 
        str_p("<-") >> 
        (
         unkOrRealInitValue | error_linearVariableDeclareExpected
        )>>
        endOfSentence[add_var]
        ;
      missing =
        newIdentifier[increment_a(s.mis.info.index)]
                            [assign_a(s.mis.info.name)] >> 
        ch_p('?') >> 
        ((
          (
            str_p("<-")[assign_a(s.mis.info.prior,"None")]>>
            (real_p[assign_a(s.mis.info.nu)] | error_badNumber)
          )
          |
          (
            ch_p('~') >>
            (
              (
                str_p("Normal")[assign_a(s.mis.info.prior,"Normal")] >> 
                openParenthesys >>
                (real_p[assign_a(s.mis.info.nu)] | error_badNumber) >> 
                argumentSeparator >>
                (real_p[assign_a(s.mis.info.sigma2)] | error_badNumber) >>
                closeParenthesys
              )
              |
              (
                str_p("TruncatedNormal")[assign_a(s.mis.info.prior,"TruncatedNormal")] >> 
                openParenthesys >>
                (real_p[assign_a(s.mis.info.nu)] | error_badNumber) >>
                argumentSeparator >>
                (real_p[assign_a(s.mis.info.sigma2)] | error_badNumber)>>
                argumentSeparator >>
                (real_p[assign_a(s.mis.info.minBound)] | error_badNumber)>>
                argumentSeparator >>
                (real_p[assign_a(s.mis.info.maxBound)] | error_badNumber) >>
                closeParenthesys
              )
            ) 
          )
        )| error_missingVariableDeclareExpected)
        >> endOfSentence[add_mis]
        ;
      noiseSize =
        (int_p[assign_noise_size_] | error_intExpected) >>
        (
          (
            str_p("dating") >> 
            identifier_simple[assign_a(s.noise.info.dating)] >>
            (str_p("from")|error_fromExpected) >> 
            identifier_simple[assign_a(s.noise.info.firstDate)] >>
            (str_p("until")|error_untilExpected) >> 
            identifier_simple[assign_a(s.noise.info.lastDate)]
          ) 
          | eps_p
        )
        ;
      normalNu =
        (real_p[assign_a(s.noise.info.nu)] | error_badNumber)
        ;
      noise = 
        (newIdentifier[increment_a(s.noise.info.index)]
                             [assign_a(s.noise.info.name)]
                             [assign_a(s.noise.info.sigmaIdx,-1)])>> 
        ch_p('[') >> 
        noiseSize >> 
        closeBracket >>
        like >>
        (( 
          normalDist >>
          openParenthesys >> 
          normalNu >> 
          argumentSeparator >>
          resSigmaDef >> 
        //power2 >>
          closeParenthesys
        ) | error_noiseDistribDeclareExpected) >>
        endOfSentence[add_res]
        ;
      equation =
        (
          knownReal 
          |
          s.mis.table[assign_mis_to_equ_out_]
          |
          ( ch_p('+') >> s.mis.table[assign_mis_to_equ_out_] )
        )
        >>
        eq[increment_a(s.equ_info.index)][assign_a(s.equ_info.Y,s.realValue)] >>
        ((
          errorTerm >>
          (*(equVarTerm[add_term_to_equ_]))
        ) | error_linearRegressionEquationDeclareExpected) >>
        endOfSentence[add_equ_]
        ;
      inequation =
        real_p[assign_a(s.realValue)] >>
        (
          ge[assign_IsGE_to_ine_]
          |
          le[assign_IsLE_to_ine_]
        )[increment_a(s.ine_info.index)][assign_a(s.ine_info.a,s.realValue)] >>
        ((
          (*(ineVarTerm[add_term_to_ine_][increment_a(s.Anzmax_)]))
        )| error_linearConstrainInequationDeclareExpected) >>
        endOfSentence[add_ine_]
        ;
      model_nameDef =
        (
          str_p("Model.Name") >> ch_p('=') >>
          *confix_p("\"", (*(anychar_p - eol_p))[assign_a(s.docInfo.model_name)], "\"") >>
          endOfSentence
        ) 
        |
        error_ModelNameDefExpected
        ;
      model_descriptionDef =
        (
          str_p("Model.Description") >> ch_p('=') >>
          *confix_p("\"", (*(anychar_p - eol_p))[assign_a(s.docInfo.model_description)], "\"") >>
          endOfSentence
        ) 
        |
        error_ModelDescriptionDefExpected
        ;
      session_nameDef =
        (
          str_p("Session.Name") >> ch_p('=') >>
          *confix_p("\"", (*(anychar_p - eol_p))[assign_a(s.docInfo.session_name)], "\"") >>
          endOfSentence
        ) 
        |
        error_SessionNameDefExpected
        ;
      session_descriptionDef =
        (
          str_p("Session.Description") >> ch_p('=') >>
          *confix_p("\"", (*(anychar_p - eol_p))[assign_a(s.docInfo.session_description)], "\"") >>
          endOfSentence
        ) 
        |
        error_SessionDescriptionDefExpected
        ;
      session_authorDef =
        (
          str_p("Session.Authors") >> ch_p('=') >>
          *confix_p("\"", (*(anychar_p - eol_p))[assign_a(s.docInfo.session_authors)], "\"") >>
          endOfSentence
        ) 
        |
        error_SessionAuthorDefExpected
        ;
      explicit_begin = str_p("$BEGIN"); 
      explicit_end   = str_p("$END")[assign_explicit_end_]; 
      problem = 
        (explicit_begin | eps_p) >>
        //header
        (
          model_nameDef >>
          model_descriptionDef >>
          session_nameDef >>
          session_descriptionDef >>
          session_authorDef 
         ) >>
        //body
        *( 
           variable | 
           missing | 
           noise | 
           equation | 
           inequation ) >>
        //end
        (explicit_end | end_p | error_declarationExpected )
        ;
      //BOOST_SPIRIT_DEBUG_NODE(argumentSeparator );
      //BOOST_SPIRIT_DEBUG_NODE(endOfSentence );
      //BOOST_SPIRIT_DEBUG_NODE(openParenthesys );
      //BOOST_SPIRIT_DEBUG_NODE(closeParenthesys );
      //BOOST_SPIRIT_DEBUG_NODE(openBracket );
      //BOOST_SPIRIT_DEBUG_NODE(closeBracket );
      //BOOST_SPIRIT_DEBUG_NODE(openKey );
      //BOOST_SPIRIT_DEBUG_NODE(closeKey );
      //BOOST_SPIRIT_DEBUG_NODE(initValue );
      //BOOST_SPIRIT_DEBUG_NODE(like );
      //BOOST_SPIRIT_DEBUG_NODE(power2 );
      //BOOST_SPIRIT_DEBUG_NODE(normalDist );
      //BOOST_SPIRIT_DEBUG_NODE(posSign );
      //BOOST_SPIRIT_DEBUG_NODE(negSign );
      //BOOST_SPIRIT_DEBUG_NODE(unknown );
      //BOOST_SPIRIT_DEBUG_NODE(product );
      //BOOST_SPIRIT_DEBUG_NODE(member );
      //BOOST_SPIRIT_DEBUG_NODE(eq);
      //BOOST_SPIRIT_DEBUG_NODE(le);
      //BOOST_SPIRIT_DEBUG_NODE(ge);
      //BOOST_SPIRIT_DEBUG_NODE(identifier_simple );
      //BOOST_SPIRIT_DEBUG_NODE(identifier );
      //BOOST_SPIRIT_DEBUG_NODE(newIdentifier );
      //BOOST_SPIRIT_DEBUG_NODE(variantSigma );
      //BOOST_SPIRIT_DEBUG_NODE(constantSigma );
      //BOOST_SPIRIT_DEBUG_NODE(covariance );
      //BOOST_SPIRIT_DEBUG_NODE(arima );
      //BOOST_SPIRIT_DEBUG_NODE(resSigmaDef );
      //BOOST_SPIRIT_DEBUG_NODE(noiseName);
      //BOOST_SPIRIT_DEBUG_NODE(noisePosition );
      //BOOST_SPIRIT_DEBUG_NODE(errorTerm );
      //BOOST_SPIRIT_DEBUG_NODE(signEquVarTerm );
      //BOOST_SPIRIT_DEBUG_NODE(signIneVarTerm );
      //BOOST_SPIRIT_DEBUG_NODE(knownReal );
      //BOOST_SPIRIT_DEBUG_NODE(unkOrReal );
      //BOOST_SPIRIT_DEBUG_NODE(knownURealEquTerm );
      //BOOST_SPIRIT_DEBUG_NODE(knownURealIneTerm );
      //BOOST_SPIRIT_DEBUG_NODE(unkOrRealInitValue );
      //BOOST_SPIRIT_DEBUG_NODE(equVarTerm );
      //BOOST_SPIRIT_DEBUG_NODE(ineVarTerm );
      //BOOST_SPIRIT_DEBUG_NODE(variable );
      //BOOST_SPIRIT_DEBUG_NODE(missing );
      //BOOST_SPIRIT_DEBUG_NODE(noiseSize );
      //BOOST_SPIRIT_DEBUG_NODE(normalNu );
      //BOOST_SPIRIT_DEBUG_NODE(noise );
      //BOOST_SPIRIT_DEBUG_NODE(equation );
      //BOOST_SPIRIT_DEBUG_NODE(inequation );
      //BOOST_SPIRIT_DEBUG_NODE(problem );

    }

    rule<ScannerT> const&  start() const { return problem; }
  };
  
  /////////////////////////////////////////////////////////////////////////////
  int expand2AllEqu(noise_info& resInfo, 
                    const BVMat& A, BVMat& A_) 
  /////////////////////////////////////////////////////////////////////////////
  {
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
    return(0);
  };

  /////////////////////////////////////////////////////////////////////////////
  int expand2AllEqu_covAndFactors(noise_info& resInfo) 
  /////////////////////////////////////////////////////////////////////////////
  {
    int k, n, err;
    BVMat cov, L, Ls, Li, D;
    cov = resInfo.cov;
    n = cov.Rows();
    if(resInfo.covIsDiag)
    {
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
        if(err = expand2AllEqu(resInfo, cov, resInfo.cov)) { return(err); }
        if(err = expand2AllEqu(resInfo, L,   resInfo.L  )) { return(err); }
        if(err = expand2AllEqu(resInfo, Li,  resInfo.Li )) { return(err); }
      }
    }
    else
    {
      err = BVMat::CholeskiFactor(cov,L,BVMat::ECFO_XtX,true);
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
    return(err);
  };

  /////////////////////////////////////////////////////////////////////////////
  int getData(vector<variable_info>&  linearInfo_,
              vector<missing_info>&   inputMissingInfo_,
              vector<missing_info>&   outputMissingInfo_,
              vector<noise_info>&     noiseInfo_,
              BVMat&                  Y, 
              BVMat&                  X,
              BVMat&                  a_, 
              BVMat&                  A)
  /////////////////////////////////////////////////////////////////////////////
  {
    int i,j,k;
    int n = var.vec.size();
    int b = noise.vec.size();
    int m = equ_vec.size();
    int r = ine_vec.size();
    Std(BSR()+" Parsed "+r+" inequations \n");
    Std(BSR()+" Building model definition \n");
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
    linearInfo_         = var.vec;
    noiseInfo_      = noise.vec;
    for(i=0; i<noiseInfo_.size(); i++)
    {
      Std(BSR()+" Building noise "+noiseInfo_[i].name.c_str()+"\n");
      if(expand2AllEqu_covAndFactors(noiseInfo_[i]))
      {
        return(-6);
      }
    }
    BVMat X_, A_;
    Std(BSR()+" Allocating dense output with "+m+" cells\n");
    Y.BlasRDense(m,1);
    Std(BSR()+" Allocating triplet sparse input with "+Xnzmax_+" non null cells\n");
    X_.ChlmRTriplet(m,n,Xnzmax_);
    Std(BSR()+" Allocating dense inequation border with "+r+" cells\n");
    a_.BlasRDense(r,1);
    Std(BSR()+" Allocating triplet sparse inequation coefficeints with "+Anzmax_+" cells\n");
    A_.ChlmRTriplet(r,n,Anzmax_);
    size_t& Xn = X_.s_.chlmRtriplet_->nnz;
    size_t& An = A_.s_.chlmRtriplet_->nnz;
    double* Yx = (double*)Y.s_.blasRdense_->x;
    double* Xx = (double*)X_.s_.chlmRtriplet_->x;
    double* ax = (double*)a_.s_.blasRdense_->x;
    double* Ax = (double*)A_.s_.chlmRtriplet_->x;
    int*    Xi = (int*)X_.s_.chlmRtriplet_->i;
    int*    Xj = (int*)X_.s_.chlmRtriplet_->j;
    int*    Ai = (int*)A_.s_.chlmRtriplet_->i;
    int*    Aj = (int*)A_.s_.chlmRtriplet_->j;
    int oldRatio = 0;
    Std(BSR()+"Building regression equations\n");
    for(i=0; i<m; i++)
    {
      int ratio = i/m;
      if((ratio!=oldRatio) && !(ratio%5))
      {
        oldRatio = ratio;
        Std(".");
      }
      noise_info& noise_inf = noise.vec[equ_vec[i].resIndex-1];
      Yx[i] = equ_vec[i].Y - noise_inf.nu;
      for(j=0; j<equ_vec[i].X.size(); j++)
      {
        Xi[Xn] = i;
        Xj[Xn] = equ_vec[i].X[j].varIndex-1;
        Xx[Xn] = equ_vec[i].X[j].x;
        assert((Xj[Xn]>=0)&&(Xj[Xn]<n));
        Xn++;
      }
    }
    Std("\n");
    Std(BSR()+"Converting regression equations from triplet to sparse\n");
    X.Convert(X_,BVMat::ESC_chlmRsparse);
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
    oldRatio = 0;
    Std(BSR()+"Building constrain inequations\n");
    for(i=0; i<r; i++)
    {
      int ratio = i/r;
      if((ratio!=oldRatio) && !(ratio%5))
      {
        oldRatio = ratio;
        Std(".");
      }
      double sign = (ine_vec[i].isGE)?1.0:-1.0;
      ax[i] = ine_vec[i].a*sign;
      for(j=0; j<ine_vec[i].A.size(); j++)
      {
        Ai[An] = i;
        Aj[An] = ine_vec[i].A[j].varIndex-1;
        Ax[An] = ine_vec[i].A[j].x*sign;
        assert((Aj[An]>=0)&&(Aj[An]<n));
        An++;
      }
    }
    Std("\n");
    Std(BSR()+"Converting constrain inequations from triplet to sparse\n");
    A.Convert(A_,BVMat::ESC_chlmRsparse);
    Std(BSR()+"Succesfully build\n");
    return(0);
  };
};

////////////////////////////////////////////////////////////////////////////////
int Parse(const string &          fileName,
          doc_info&               docInfo,
          vector<variable_info>&  linearInfo,
          vector<missing_info>&   inputMissingInfo,
          vector<missing_info>&   outputMissingInfo,
          vector<noise_info>&     noiseInfo,
          BVMat&                  Y, 
          BVMat&                  X,
          BVMat&                  a, 
          BVMat&                  A)
////////////////////////////////////////////////////////////////////////////////
{
  bys_sparse_reg bsr;
  const char* fName = fileName.c_str();
  
//BOOST_SPIRIT_DEBUG_NODE(bsr);
  int errCode=0;
  ifstream in(fName);
  if(!in)
  {
    Error(BSR() + BText("Could not open input file: ") + 
          fName + "\n");
    return(-1);
  }
  bsr.fileName = fileName;
  bsr.fileSize = GetFileSize(fName);
  bsr.file = &in;

  in.unsetf(ios::skipws); //  Turn of white space skipping on the stream
  skip_grammar skip;
  typedef position_iterator< file_iterator<char> > iterator_t; 
  file_iterator<char> fiter(fileName); 
  iterator_t begin(fiter, fiter.make_end(), fName);
  iterator_t end;  
  Std(BSR()+"Parsing BSR file "+fName+" with "+(int)bsr.fileSize+" bytes\n");
  parse_info<iterator_t> result = parse(begin, end, bsr, skip);
  if(!result.full && !bsr.endFound_)
  {
    BText msg = BSR() + BText(fName) + " Fails Parsing\n"+url_parse_bsr();
    BText desc;
    for (int i = 0; i < 1000; i++)
    {
      if (result.stop == end) break;
      desc += *result.stop++;
    }
    if(!desc.HasName()) { desc = "Unexpectend end of file"; }
    Error(msg+"\nProblem description:'"+desc+"'");
    return(-2);
  }
  if(!errCode)
  {
    docInfo.model_name          =  bsr.docInfo.model_name;
    docInfo.model_description   =  bsr.docInfo.model_description;
    docInfo.session_name        =  bsr.docInfo.session_name;
    docInfo.session_description =  bsr.docInfo.session_description;
    docInfo.session_authors      =  bsr.docInfo.session_authors;
    errCode = bsr.getData
    (
      linearInfo,
      inputMissingInfo,
      outputMissingInfo,
      noiseInfo,
      Y,
      X,
      a,
      A
    );
  }
  return(errCode);
};

};

