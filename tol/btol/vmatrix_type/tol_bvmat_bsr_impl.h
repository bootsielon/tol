/* vmat_bsr_impl_h: BVMat Bayesian Sparse Regression 
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
#include <tol/tol_bmatgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btmsgra.h>
#include <tol/tol_bcodgra.h>
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

/*
///////////////////////////////////////////////////////////////////////////////
struct bsr_modeldef
///////////////////////////////////////////////////////////////////////////////
{
  doc_info&               docInfo,
  vector<variable_info>&  linearInfo,
  vector<missing_info>&   inputMissingInfo,
  vector<missing_info>&   outputMissingInfo,
  vector<noise_info>&     noiseInfo,
  BVMat&                  Y, 
  BVMat&                  X,
  BVMat&                  a, 
  BVMat&                  A
};
*/

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
template<typename T> class add_symbol
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<T>* sh_;
  add_symbol(symbol_handler<T>& sh) 
  : sh_(&sh) 
  {}
  void action() const
  {
    symbol_handler<T>* sh = sh_;
  //Std(BText("\nadd_symbol(")+sh_->info.name.c_str()+")");
  //sh_->info.show();
    sh->table.add(sh->info.name.c_str(), sh->vec.size());
    sh->vec.push_back(sh_->info);
    sh->count++;
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void  operator()(IteratorT first, IteratorT last) const { action(); }
};


///////////////////////////////////////////////////////////////////////////////
class assign_declared_var_name
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<variable_info>*  var_;
  assign_declared_var_name(symbol_handler<variable_info>& var) 
  : var_(&var)
  {}
  void action(const std::string& str) const
  {
    var_->info.index = var_->vec.size()+1;
    var_->info.name = str;
    var_->info.isExtern = false;
    var_->info.initValue = BDat::Nan();
    var_->info.used = false;
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_extern_var_name
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<variable_info>*  var_;
  assign_extern_var_name(symbol_handler<variable_info>& var) 
  : var_(&var)
  {}
  void action(const std::string& str) const
  {
    var_->info.index = var_->vec.size()+1;
    var_->info.name = str;
    var_->info.isExtern = true;
    var_->info.initValue = BDat::Nan();
    var_->info.used = true;
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_missing_min
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<missing_info>*  mis_;
  
  assign_missing_min(symbol_handler<missing_info>& mis)
  : mis_(&mis)
  {}
  void action(const double d) const
  {
    symbol_handler<missing_info>* mis = mis_;
    mis->info.minBound  = d;
  }
  void action(const std::string& str) const
  {
    if(str=="?")
    {
      action(BDat::NegInf());
    }
    else
    {
      action(atof(str.c_str()));
    }
  }
  void operator()(const double d) const
  {
    action(d);
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }
  void operator()(const char* str) const
  {
    action(str);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};


///////////////////////////////////////////////////////////////////////////////
class assign_missing_max
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<missing_info>*  mis_;
  
  assign_missing_max(symbol_handler<missing_info>& mis)
  : mis_(&mis)
  {}
  void action(const double d) const
  {
    symbol_handler<missing_info>* mis = mis_;
    mis_->info.maxBound  = d;
  }
  void action(const std::string& str) const
  {
    if(str=="?")
    {
      action(BDat::PosInf());
    }
    else
    {
      action(atof(str.c_str()));
    }
  }
  void operator()(const double d) const
  {
    action(d);
  }
  void operator()(const char* str) const
  {
    action(str);
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_missing_row
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<missing_info>*  mis_;
  
  assign_missing_row(symbol_handler<missing_info>& mis)
  : mis_(&mis)
  {}
  void action(const int d) const
  {
    symbol_handler<missing_info>* mis = mis_;
    mis_->info.row  = d;
  }
  void operator()(const int d) const
  {
    action(d);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(atoi(str.c_str()));
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_missing_output
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<missing_info>*  mis_;
  
  assign_missing_output(symbol_handler<missing_info>& mis)
  : mis_(&mis)
  {}
  void action() const
  {
    symbol_handler<missing_info>* mis = mis_;
    mis_->info.col  = 0;
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void  operator()(IteratorT first, IteratorT last) const { action(); }
};

///////////////////////////////////////////////////////////////////////////////
class assign_missing_input
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<missing_info>*  mis_;
  vector<variable_info>* var_vec_; 
  
  assign_missing_input(
    symbol_handler<missing_info>& mis,
    vector<variable_info>& var_vec)
  : mis_(&mis),
    var_vec_(&var_vec)
  {}
  void action(const int d) const
  {
    assign_missing_input* t = (assign_missing_input*)this;
    t->mis_->info.col  = d;
  }
  void operator()(const int d) const
  {
    action(d);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(atoi(str.c_str()));
  }
};


///////////////////////////////////////////////////////////////////////////////
class assign_noise_size
///////////////////////////////////////////////////////////////////////////////
{
public:
  noise_info* noise_info_; 
  int*        var_count_; 
  int*        numEqu_; 
  assign_noise_size(
    noise_info& noise_info, 
    int&        var_count,
    int&        numEqu)
  : noise_info_(&noise_info), 
    var_count_ (&var_count),
    numEqu_    (&numEqu)
  {}

  void action(const int size) const
  { 
    assign_noise_size* t = (assign_noise_size*)this;
    t->noise_info_->size = size;
    t->noise_info_->dating = "";
    t->noise_info_->firstDate = "";
    if(!(*numEqu_))
    {
      Std(BSR()+" Parsed "+*(t->var_count_)+" linear block variables \n");
    }
    *(t->numEqu_) += size;
  }
  void operator()(const int size) const
  {
    action(size);
  }
  void action(const std::string& str) const
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



///////////////////////////////////////////////////////////////////////////////
class assign_sigma_to_noise
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbols<int>*       sig_table_;
  vector<sigma_info>* sig_vec_;
  noise_info*         noise_info_; 
  int*                numEqu_; 

  assign_sigma_to_noise(
    symbols<int>&       sig_table,
    vector<sigma_info>& sig_vec,
    noise_info&         noise_inf,
    int&                numEqu)
  : sig_table_   (&sig_table),
    sig_vec_     (&sig_vec),
    noise_info_  (&noise_inf), 
    numEqu_      (&numEqu)
  {}

  void action(sigma_info& sigInf) const
  {
    assign_sigma_to_noise* t = (assign_sigma_to_noise*)this;
  //Std(BText("\nassign_sigma_to_noise(")+str.c_str()+")");
  //Std(BText("sigmaIdx=")+sigInf->index);
    t->noise_info_->sigmaName  = sigInf.name;
    t->noise_info_->sigmaIdx   = sigInf.index;
    t->noise_info_->sigmaCte   = BDat::Nan();
    t->noise_info_->covExpr    = "";
    t->noise_info_->cov.Eye(noise_info_->size,noise_info_->size,0,1);
    t->noise_info_->covIsDiag=true;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((*sig_vec_)[n]);
  }

  void action(const std::string& str) const
  {
  //Std(BText("\nassign_sigma_to_noise(")+str.c_str()+")");
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

///////////////////////////////////////////////////////////////////////////////
class assign_sig_pri
///////////////////////////////////////////////////////////////////////////////
{
public:
  noise_info* noise_info_; 
  assign_sig_pri(noise_info& noise_inf) 
  : noise_info_(&noise_inf)
  {}
  void action(const std::string& str) const
  {
    noise_info* noise_inf = noise_info_;
  //Std(BText("\nassign_sigma_to_noise(")+str.c_str()+")");
  //Std(BText("sigmaIdx=")+sigInf->index);
    noise_inf->sigPriExpr  = str;
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }

  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }

};

///////////////////////////////////////////////////////////////////////////////
class assign_reg_matrix
///////////////////////////////////////////////////////////////////////////////
{
public:
  BText& desc_;
  BText& expr_;
  BVMat& vmat_; 

  assign_reg_matrix(BText& desc, BText& expr, BVMat& vmat) 
  : desc_(desc),
    expr_(expr),
    vmat_(vmat)
  {}
  void action(const std::string& str) const
  {
    assign_reg_matrix& t = *((assign_reg_matrix*)this);
    t.expr_.Copy(str.c_str()+2, str.size()-4);
  //Std(BSR()+" assign_reg_matrix\n  desc_ = "+desc_+"\n  expr_="+t.expr_);
    BSyntaxObject* obj = GraAnything()->EvaluateExpr(t.expr_);
    if(!obj)
    {
      Error(BText("Cannot create ")+desc_+" from TOL expression \n"+expr_);
    }
    else if(obj->Grammar()==GraVMatrix())
    {
      t.vmat_ = VMat(obj);
    }
    else if(obj->Grammar()==GraMatrix())
    {
      BMat& M = Mat(obj);
      t.vmat_.DMat2VMat((BMatrix<double>&)M, false, .50, 0.0);
    } 
    else
    {
      Error(BText("Cannot create ")+desc_+" from TOL expression \n"+expr_+"\n"
            " This expression is returning a "+obj->Grammar()->Name()+
            " instead of a Matrix or VMatrix");
    }
  }
  void operator()(const std::string& str) const
  {
    action(str);
  }

  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }

};


///////////////////////////////////////////////////////////////////////////////
class assign_const_sigma_to_res
///////////////////////////////////////////////////////////////////////////////
{
public:
  noise_info* noise_info_; 
  assign_const_sigma_to_res( noise_info& noise_inf) 
  : noise_info_(&noise_inf)
  {}
  void action(const double d) const
  {
    noise_info* noise_inf = noise_info_;
  //Std(BText("\nassign_const_sigma_to_res(")+d+")");
    noise_inf->sigmaName  = "";
    noise_inf->sigmaIdx   = -1;
    noise_inf->sigmaCte   = d;
    noise_inf->covExpr    = "";
    noise_inf->cov.Eye(noise_inf->size,noise_inf->size,0,noise_inf->sigmaCte);
    noise_inf->covIsDiag=true;
  //Std(BText(" sigmaIdx=-1"));
  //Std(BText(" sigmaCte=")+noise_info_->sigmaCte);
  }
  void operator()(const double d) const
  {
    action(d);
  }
  void action(const std::string& str) const
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


///////////////////////////////////////////////////////////////////////////////
class assign_covariance_to_res
///////////////////////////////////////////////////////////////////////////////
{
public:
  noise_info* noise_info_; 
  assign_covariance_to_res(noise_info& noise_inf)
  : noise_info_(&noise_inf)
  {}
  void action(const std::string& str) const
  {
    noise_info* noise_inf = noise_info_; 
  //Std(BText("\nassign_covariance_to_res(")+str.c_str()+")");
    noise_inf->sigmaName  = "";
    noise_inf->sigmaIdx   = -2;
    noise_inf->sigmaCte   = 1;
    noise_inf->covExpr    = str;
    BUserVMat* u = (BUserVMat*)GraVMatrix()->EvaluateExpr(str.c_str());
    int err = 0;
    if(!u)
    {
      err = -999999;
    }
    else
    {
      noise_inf->cov = u->Contens();
      noise_inf->covIsDiag=noise_inf->cov.IsDiagonal();
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

///////////////////////////////////////////////////////////////////////////////
class assign_non_lin_flt_to_noise
///////////////////////////////////////////////////////////////////////////////
{
public:
  noise_info* noise_info_; 
  assign_non_lin_flt_to_noise( noise_info& noise_inf) 
  : noise_info_(&noise_inf)
  {}
  void action(const std::string& str) const
  {
    noise_info* noise_inf = noise_info_;
  //Std(BText("\nassign_const_sigma_to_res(")+d+")");
    noise_inf->nonLinFlt  = str;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    action(str);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_pos_sign_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  var_term* var_term_info_; 
  assign_pos_sign_to_equ_term(var_term& var_term_info) 
  : var_term_info_(&var_term_info)
  {}
  void action() const 
  { 
    var_term* var_term_info = var_term_info_; 
    var_term_info->sign = 1.0; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};


///////////////////////////////////////////////////////////////////////////////
class assign_neg_sign_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  var_term* var_term_info_; 
  assign_neg_sign_to_equ_term(var_term& var_term_info) 
  : var_term_info_(&var_term_info)
  {}
  void action() const
  {
    var_term* var_term_info = var_term_info_; 
    var_term_info->sign = -1.0; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};

///////////////////////////////////////////////////////////////////////////////
class assign_pos_sign_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  var_term* var_term_info_; 
  assign_pos_sign_to_ine_term(var_term& var_term_info) 
  : var_term_info_(&var_term_info)
  {}
  void action() const 
  { 
    var_term* var_term_info = var_term_info_; 
    var_term_info->sign = 1.0; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};


///////////////////////////////////////////////////////////////////////////////
class assign_neg_sign_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  var_term* var_term_info_; 
  assign_neg_sign_to_ine_term(var_term& var_term_info) 
  : var_term_info_(&var_term_info)
  {}
  void action() const 
  {
    var_term* var_term_info = var_term_info_; 
    var_term_info->sign = -1.0; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};

///////////////////////////////////////////////////////////////////////////////
class assign_IsGE_to_ine
///////////////////////////////////////////////////////////////////////////////
{
public:
  lin_noise_inequation* ine_; 
  assign_IsGE_to_ine(lin_noise_inequation& ine) 
  : ine_(&ine)
  {}
  void action() const
  {
    lin_noise_inequation* ine = (lin_noise_inequation*)ine_; 
    ine->isGE = true; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};

///////////////////////////////////////////////////////////////////////////////
class assign_IsLE_to_ine
///////////////////////////////////////////////////////////////////////////////
{
public:
  lin_noise_inequation* ine_; 
  assign_IsLE_to_ine(lin_noise_inequation& ine) 
  : ine_(&ine)
  {}
  void action() const
  {
    lin_noise_inequation* ine = (lin_noise_inequation*)ine_; 
    ine_->isGE = false; 
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};


///////////////////////////////////////////////////////////////////////////////
class assign_var_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  symbol_handler<variable_info>* var_; 
  var_term*              var_term_info_; 
  int*                   nzmax_;

  assign_var_to_equ_term(
    symbol_handler<variable_info>& var_,
    var_term& var_term_info, 
    int& nzmax) 
  : 
    var_          (&var_),
    var_term_info_(&var_term_info), 
    nzmax_        (&nzmax)
  {}
  void action(variable_info& varInfo) const
  {
    assign_var_to_equ_term* t = (assign_var_to_equ_term*)this;
    varInfo.used = true;
    t->var_term_info_->varIndex = varInfo.index;
    t->var_term_info_->misIndex = -1;
    (*(t->nzmax_))++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    int n = pos.get(); 
    action((var_->vec)[n]);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    int* pos = find(var_->table,str.c_str());
    action((var_->vec)[*pos]);
  }

};


///////////////////////////////////////////////////////////////////////////////
class assign_mis_to_equ_out
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<missing_info>*  mis_vec_; 
  lin_reg_equation*      equ_info_;

  assign_mis_to_equ_out(
    vector<missing_info>&  mis_vec, 
    lin_reg_equation&      equ_info) 
  : mis_vec_ (&mis_vec),
    equ_info_(&equ_info)
  {}

  void action(missing_info& misInfo) const
  {
    assign_mis_to_equ_out* t = (assign_mis_to_equ_out*)this;
    if(misInfo.used)
    {
      Error(BText("Missing variable ")+misInfo.name.c_str()+" is already "
            "in use at equation "+misInfo.row);
    }
    else
    {
      misInfo.used = true;
      t->equ_info_->misOutputIndex = misInfo.index;
      misInfo.row = t->equ_info_->index+1;
      misInfo.col = 0;
    }
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    assign_mis_to_equ_out* t = (assign_mis_to_equ_out*)this;
    int n = pos.get(); 
    action((*t->mis_vec_)[n]);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_mis_to_var_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<missing_info>* mis_vec_; 
  var_term*             var_term_info_; 

  assign_mis_to_var_term(
    vector<missing_info>& mis_vec,
    var_term&             var_term_info)
  : mis_vec_       (&mis_vec),
    var_term_info_ (&var_term_info) 
  {}
  void action(missing_info& misInfo) const
  {
    var_term* var_term_info = var_term_info_; 
    var_term_info->misIndex = misInfo.index;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    vector<missing_info>* mis_vec = mis_vec_; 
    int n = pos.get(); 
    action((*mis_vec)[n]);
  }
};

///////////////////////////////////////////////////////////////////////////////
class assign_mis_to_equ_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<missing_info>*  mis_vec_; 
  symbol_handler<variable_info>* var_; 
  var_term* var_term_info_; 
  lin_reg_equation* equ_info_;
  int* nzmax_;

  assign_mis_to_equ_term(
    vector<missing_info>&  mis_vec, 
    symbol_handler<variable_info>& var_,
    var_term& var_term_info,
    lin_reg_equation& equ_info,
    int& nzmax) 
  :
    mis_vec_       (&mis_vec),
    var_           (&var_),
    var_term_info_ (&var_term_info),
    equ_info_      (&equ_info),
    nzmax_         (&nzmax)
  {}
  void action(variable_info& varInfo) const
  {
    assign_mis_to_equ_term* t = (assign_mis_to_equ_term*)this;
    t->var_term_info_->varIndex = varInfo.index;
    missing_info& mis_info = (*t->mis_vec_)[t->var_term_info_->misIndex-1];
    mis_info.row = t->equ_info_->index;
    mis_info.col = varInfo.index;
    (*t->nzmax_)++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    assign_mis_to_equ_term* t = (assign_mis_to_equ_term*)this;
    int n = pos.get(); 
    action((var_->vec)[n]);
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    std::string str;
    str.assign(first, last);
    int* pos = find(var_->table,str.c_str());
    action((var_->vec)[*pos]);
  }

};
      

///////////////////////////////////////////////////////////////////////////////
class assign_var_to_ine_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<variable_info>* var_vec_; 
  var_term*              var_term_info_; 
  int*                   nzmax_;

  assign_var_to_ine_term(
    vector<variable_info>& var_vec, 
    var_term&              var_term_info,
    int&                   nzmax)
  :
  var_vec_(&var_vec),
  var_term_info_(&var_term_info),
  nzmax_(&nzmax)
  {}
  void action(variable_info& varInfo) const
  {
    assign_var_to_ine_term* t = (assign_var_to_ine_term*) this;
    t->var_term_info_->varIndex = varInfo.index;
    (*t->nzmax_)++;
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    assign_var_to_ine_term* t = (assign_var_to_ine_term*) this;
    int n = pos.get(); 
    action((*t->var_vec_)[n]);
  }
};


///////////////////////////////////////////////////////////////////////////////
class assign_noise_to_term
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<noise_info>*   noise_vec_;
  lin_reg_equation*     equ_; 
  vector<sigma_info>*   sig_vec_;
  assign_noise_to_term(
    vector<noise_info>&   noise_vec,
    vector<sigma_info>&   sig_vec,
    lin_reg_equation*     equ = NULL) 
  :
    noise_vec_(&noise_vec),
    sig_vec_(&sig_vec),
    equ_(equ)
  {}
  void action(noise_info& resInf) const
  {
    assign_noise_to_term* t = (assign_noise_to_term*)this;
    if(t->equ_)
    {
      t->equ_->resIndex = resInf.index;
      resInf.equIdx.push_back(t->equ_->index);
    }
    if(resInf.sigmaIdx>=0)
    {
      sigma_info& sig = (*t->sig_vec_)[resInf.sigmaIdx-1];
      sig.nzmax++;
    }
  }
  void operator()(const boost::reference_wrapper<int>& pos) const
  {
    assign_noise_to_term* t = (assign_noise_to_term*)this;
    int n = pos.get(); 
    action((*t->noise_vec_)[n]);
  }

};

///////////////////////////////////////////////////////////////////////////////
class add_term_to_equ
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<duplet>* X_;
  var_term* var_term_info_; 
  add_term_to_equ(
    vector<duplet>& X,
    var_term& var_term_info)
  :
    X_(&X),
    var_term_info_(&var_term_info)
  {}
  void action() const
  {
    add_term_to_equ* t = (add_term_to_equ*)this;
    int n = t->X_->size();
    if(t->X_->capacity()<n+1) { t->X_->reserve(int((n+1)*1.20)); }
    t->X_->resize(n+1);
    (*t->X_)[n].varIndex = t->var_term_info_->varIndex;
    (*t->X_)[n].x        = t->var_term_info_->sign * t->var_term_info_->coef;
    t->var_term_info_->varIndex = -1;
    t->var_term_info_->sign = 1;
    t->var_term_info_->coef = 1;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }
};


///////////////////////////////////////////////////////////////////////////////
class add_term_to_ine
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<duplet>* X_;
  var_term* var_term_info_; 
  add_term_to_ine(
    vector<duplet>& X,
    var_term& var_term_info)
  :
    X_(&X),
    var_term_info_(&var_term_info)
  {}
  void action() const
  {
    add_term_to_ine* t = (add_term_to_ine*)this;
    int n = t->X_->size();
    if(t->X_->capacity()<n+1) { t->X_->reserve(int((n+1)*1.20)); }
    t->X_->resize(n+1);
    (*t->X_)[n].varIndex = t->var_term_info_->varIndex;
    (*t->X_)[n].x        = t->var_term_info_->sign * t->var_term_info_->coef;
    t->var_term_info_->varIndex = -1;
    t->var_term_info_->sign = 1;
    t->var_term_info_->coef = 1;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }
};


///////////////////////////////////////////////////////////////////////////////
class add_equ
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<lin_reg_equation>* equ_vec_;
  lin_reg_equation*         equ_;
  int*                      numEqu_; 
  int*                      mis_count_; 
  int*                      sig_count_; 
  int*                      noise_count_; 
  double                    iniTime_ ;
  int                       oldRatio_;

  add_equ(
    vector<lin_reg_equation>& equ_vec,
    lin_reg_equation&         equ,
    int&                      numEqu, 
    int&                      mis_count, 
    int&                      sig_count,
    int&                      noise_count)
  : equ_vec_(&equ_vec),
    equ_(&equ),
    numEqu_(&numEqu),
    mis_count_(&mis_count),
    sig_count_(&sig_count),
    noise_count_(&noise_count), 
    iniTime_ (0.0),
    oldRatio_(0)
  {}
  void action() const
  {
    add_equ* t = (add_equ*)this;
    t->force_action();
  }
  void force_action() 
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


///////////////////////////////////////////////////////////////////////////////
class add_ine
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<lin_noise_inequation>* ine_vec_;
  lin_noise_inequation*         ine_;
  add_ine(
    vector<lin_noise_inequation>& ine_vec,
    lin_noise_inequation&         ine) 
  :
    ine_vec_(&ine_vec),
    ine_(&ine)
  {}
  void action() const
  {
    add_ine* t = (add_ine*)this;
    t->force_action();
  }
  void force_action() 
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

///////////////////////////////////////////////////////////////////////////////
class add_submodule
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<moduleDef>* subMod_;
  moduleDef* currentSubMod_;
  add_submodule(
    vector<moduleDef>& subMod,
    moduleDef& currentSubMod) 
  :
    subMod_(&subMod),
    currentSubMod_(&currentSubMod)
  {}
  void action() const
  {
    add_submodule* t = (add_submodule*)this;
    t->force_action();
  }
  void force_action() 
  {
    subMod_->push_back(*currentSubMod_);
  }
  void  operator()(const char& str) const { action();  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const { action(); }
};


///////////////////////////////////////////////////////////////////////////////
class assign_explicit_end
///////////////////////////////////////////////////////////////////////////////
{
public:
  bool* endFound_; 
  assign_explicit_end(bool& endFound)
  : endFound_(&endFound)
  {}
  void action() const
  { 
    bool* endFound = endFound_; 
    (*endFound) = true;
  }
  template<typename IteratorT>
  void operator()(IteratorT first, IteratorT last) const
  {
    action();
  }

};

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
struct error_report_parser 
///////////////////////////////////////////////////////////////////////////////
{
  std::string msg_;
  error_report_parser(std::string msg) : msg_(msg) {}
  typedef nil_t result_t;
  static void show_parser_error(
    char const* msg, 
    file_position const& lc, 
    bool end);
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
class bys_sparse_reg 
///////////////////////////////////////////////////////////////////////////////
{
public:
  string    moduleType;
  string    fileName;
  ifstream* file;
  size_t    fileSize;

  doc_info docInfo;

  symbol_handler<variable_info>  var;
  symbol_handler<missing_info>   mis;
  symbol_handler<sigma_info>     sig;
  symbol_handler<noise_info>     noise;

  double   realValue;
  int Xnzmax_;
  int Anzmax_;
  int numEqu_;

  bool endFound_;

  bys_sparse_reg();

  virtual int getData(vector<variable_info>&  linearInfo_,
              vector<missing_info>&   inputMissingInfo_,
              vector<missing_info>&   outputMissingInfo_,
              vector<noise_info>&     noiseInfo_,
              BVMat&                  Y, 
              BVMat&                  X,
              BVMat&                  A, 
              BVMat&                  a)
  {
    return(0);
  };
  
protected:     
  int expand2AllEqu(noise_info& resInfo, 
                    const BVMat& A, BVMat& A_);

  int expand2AllEqu_covAndFactors(noise_info& resInfo);

  int checkDimensions(int m);

  int getMissing(BVMat& Y,
                 BVMat& X, 
                 vector<missing_info>&   inputMissingInfo_,
                 vector<missing_info>&   outputMissingInfo_);

};

};

