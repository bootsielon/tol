/* vmat_bsr_master.cpp: BVMat Bayesian Sparse Regression 
                              parsing methods for master modules
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
class bys_sparse_reg_master : public grammar<bys_sparse_reg_master>,
                              public bys_sparse_reg
///////////////////////////////////////////////////////////////////////////////
{
public:
  string sampler;
  vector<moduleDef> subMod;
  moduleDef currentSubMod;

  bys_sparse_reg_master() 
  : bys_sparse_reg(),
    sampler(""), 
    subMod(),
    currentSubMod()
  {
  }
    
  template <typename ScannerT>
  struct definition
  {
    bys_sparse_reg_master& s;
    rule<ScannerT>  
      argumentSeparator, endOfSentence, 
      openParenthesys, closeParenthesys, 
      module_type, model_nameDef, model_descriptionDef, 
      session_nameDef, session_descriptionDef, session_authorDef, 
      samplerOpt,moduleOpt,modulePath,
      sampler, subModule,
      explicit_begin, explicit_end, problem;

    definition(const bys_sparse_reg_master& s_)
    : s ((bys_sparse_reg_master&)s_)
    {
      add_submodule add_submodule_(s.subMod, s.currentSubMod); 
      assign_explicit_end assign_explicit_end_(s.endFound_);

      #include "tol_bvmat_bsr_err.h"
      
      argumentSeparator = (ch_p(',') | error_comma1Expected)
        ;
      endOfSentence = (ch_p(';') | error_comma2Expected)
        ;
      openParenthesys = (ch_p('(') | error_openParenthesysExpected)
        ;
      closeParenthesys = (str_p(")") | error_closeParenthesysExpected)
        ;
      module_type =
      (
        (
          str_p("Module.Type") >> ch_p('=') >> 
          (str_p("master")[assign_a(s.moduleType)]) >> 
          endOfSentence
        ) |
        error_ModuleTypeMasterDefExpected
      );
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
      explicit_begin = str_p("$BEGIN") | error_beginExpected; 
      explicit_end   = str_p("$END")[assign_explicit_end_] | error_endExpected; 

      samplerOpt = 
        (str_p("monophasic") | str_p("sequential") | str_p("parallel")) |
        error_SamplerOptExpected;

      moduleOpt = 
        (str_p("primary") | str_p("joint") | str_p("master")) |
        error_ModuleOptExpected;

      sampler = 
        str_p("Sampler") >> ch_p('=') >>
        samplerOpt[assign_a(s.sampler)];

      modulePath = 
        confix_p("\"", *(anychar_p - '\"'), "\"")[assign_a(s.currentSubMod.filePath)]
        |error_ModulePathExpected;

      subModule = (
        str_p("Include") >>
        (
          moduleOpt[assign_a(s.currentSubMod.moduleType)] >>
          (str_p("module")|error_ModuleExpected) >>
          modulePath
        )
      )[add_submodule_]
      >>endOfSentence;
 
      problem = 
        explicit_begin >>
        //header
        module_type >>
        model_nameDef >>
        model_descriptionDef >>
        session_nameDef >>
        session_descriptionDef >>
        session_authorDef >>
        //body
        sampler >> 
        (*(subModule))  >>
        (explicit_end | error_declarationExpected )
        ;
/*
        BOOST_SPIRIT_DEBUG_NODE(argumentSeparator );
        BOOST_SPIRIT_DEBUG_NODE(endOfSentence );
        BOOST_SPIRIT_DEBUG_NODE(openParenthesys );
        BOOST_SPIRIT_DEBUG_NODE(closeParenthesys );
        BOOST_SPIRIT_DEBUG_NODE(openBracket );
        BOOST_SPIRIT_DEBUG_NODE(closeBracket );
        BOOST_SPIRIT_DEBUG_NODE(openKey );
        BOOST_SPIRIT_DEBUG_NODE(closeKey );
        BOOST_SPIRIT_DEBUG_NODE(initValue );
        BOOST_SPIRIT_DEBUG_NODE(like );
        BOOST_SPIRIT_DEBUG_NODE(power2 );
        BOOST_SPIRIT_DEBUG_NODE(normalDist );
        BOOST_SPIRIT_DEBUG_NODE(posSign );
        BOOST_SPIRIT_DEBUG_NODE(negSign );
        BOOST_SPIRIT_DEBUG_NODE(unknown );
        BOOST_SPIRIT_DEBUG_NODE(product );
        BOOST_SPIRIT_DEBUG_NODE(member );
        BOOST_SPIRIT_DEBUG_NODE(eq);
        BOOST_SPIRIT_DEBUG_NODE(le);
        BOOST_SPIRIT_DEBUG_NODE(ge);
        BOOST_SPIRIT_DEBUG_NODE(identifier_simple );
        BOOST_SPIRIT_DEBUG_NODE(identifier );
        BOOST_SPIRIT_DEBUG_NODE(newIdentifier );
        BOOST_SPIRIT_DEBUG_NODE(variantSigma );
        BOOST_SPIRIT_DEBUG_NODE(constantSigma );
        BOOST_SPIRIT_DEBUG_NODE(covariance );
        BOOST_SPIRIT_DEBUG_NODE(arima );
        BOOST_SPIRIT_DEBUG_NODE(resSigmaDef );
        BOOST_SPIRIT_DEBUG_NODE(noiseName);
        BOOST_SPIRIT_DEBUG_NODE(noisePosition );
        BOOST_SPIRIT_DEBUG_NODE(errorTerm );
        BOOST_SPIRIT_DEBUG_NODE(signEquVarTerm );
        BOOST_SPIRIT_DEBUG_NODE(signIneVarTerm );
        BOOST_SPIRIT_DEBUG_NODE(knownReal );
        BOOST_SPIRIT_DEBUG_NODE(unkOrReal );
        BOOST_SPIRIT_DEBUG_NODE(knownURealEquTerm );
        BOOST_SPIRIT_DEBUG_NODE(knownURealIneTerm );
        BOOST_SPIRIT_DEBUG_NODE(unkOrRealInitValue );
        BOOST_SPIRIT_DEBUG_NODE(variable );
        BOOST_SPIRIT_DEBUG_NODE(missing );
        BOOST_SPIRIT_DEBUG_NODE(noiseSize );
        BOOST_SPIRIT_DEBUG_NODE(normalNu );
        BOOST_SPIRIT_DEBUG_NODE(noise );
        BOOST_SPIRIT_DEBUG_NODE(output);
        BOOST_SPIRIT_DEBUG_NODE(input);
        BOOST_SPIRIT_DEBUG_NODE(model_nameDef );
        BOOST_SPIRIT_DEBUG_NODE(model_descriptionDef );
        BOOST_SPIRIT_DEBUG_NODE(session_nameDef );
        BOOST_SPIRIT_DEBUG_NODE(session_descriptionDef );
        BOOST_SPIRIT_DEBUG_NODE(session_authorDef );
        BOOST_SPIRIT_DEBUG_NODE(problem );
*/
    }

    rule<ScannerT> const&  start() const { return problem; }
  };
  

  /////////////////////////////////////////////////////////////////////////////
  int getData(vector<variable_info>&  linearInfo_,
              vector<missing_info>&   inputMissingInfo_,
              vector<missing_info>&   outputMissingInfo_,
              vector<noise_info>&     noiseInfo_,
              BVMat&                  Y, 
              BVMat&                  X,
              BVMat&                  a, 
              BVMat&                  A)
  /////////////////////////////////////////////////////////////////////////////
  {
    return(-1);
  };
};

////////////////////////////////////////////////////////////////////////////////
int Parse_Module_Master(
  const std::string &     fileName,
  doc_info&               docInfo,
  std::string &           sampler,
  vector<moduleDef>&      subModules)
////////////////////////////////////////////////////////////////////////////////
{
  bys_sparse_reg_master bsr;
  int errCode = 0;
  #include "tol_bvmat_bsr_run.h"
  if(!errCode)
  {
    docInfo.model_name          =  bsr.docInfo.model_name;
    docInfo.model_description   =  bsr.docInfo.model_description;
    docInfo.session_name        =  bsr.docInfo.session_name;
    docInfo.session_description =  bsr.docInfo.session_description;
    docInfo.session_authors     =  bsr.docInfo.session_authors;
    sampler                     =  bsr.sampler; 
    subModules                  =  bsr.subMod;
  }
  return(errCode);
};

};

