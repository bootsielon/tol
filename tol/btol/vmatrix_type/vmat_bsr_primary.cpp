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
class bys_sparse_reg_primary : public grammar<bys_sparse_reg_primary>,
                               public bys_sparse_reg
///////////////////////////////////////////////////////////////////////////////
{
public:
  vector<lin_noise_inequation> ine_vec;
  lin_noise_inequation         ine_info;
  var_term ine_var_term_info;
  BText descY_;
  BText descX_;
  BText exprY_;
  BText exprX_;
  BVMat Y_;
  BVMat X_;

  bys_sparse_reg_primary() : bys_sparse_reg() 
  {
    ine_info.index = 0;
    descY_ = "Output regression matrix";
    descX_ = "Input regression matrix";
    var.info.used = true;
    moduleType = "primary";
  }
    
  template <typename ScannerT>
  struct definition
  {
    bys_sparse_reg_primary& s;
    rule<ScannerT>  
      variantSigma, constantSigma, covariance, 
      noiseName, noisePosition,
      posSign, negSign, unknown, product,
      initValue, like, power2, member, nonLinearFilters,
      noiseSize, normalDist, normalNu, eq, le, ge, tolExpression,
      argumentSeparator, endOfSentence, 
      openParenthesys, closeParenthesys,
      openBracket, closeBracket,
      openKey, closeKey,
      identifier_simple, identifier, nonExistentIdentifier, newIdentifier, 
      resSigmaDef, errorTerm, 
      signEquVarTerm, signIneVarTerm, 
      knownReal, unkOrReal, 
      unkOrRealInitValue, 
      knownURealEquTerm, 
      knownURealIneTerm,
      variable, variableExistentOrExtern, missing, arima, noise,
      matOrVMatExpr, output, input,
      ineVarTerm, inequation,
      module_type, model_nameDef, model_descriptionDef, 
      session_nameDef, session_descriptionDef, session_authorDef, 
      explicit_begin, explicit_end, problem;

    definition(const bys_sparse_reg_primary& s_)
    : s ((bys_sparse_reg_primary&)s_)
    {
      add_symbol<variable_info> add_var(s.var);
      add_symbol<missing_info>  add_mis(s.mis);
      add_symbol<sigma_info>    add_sig(s.sig);
      add_symbol<noise_info>    add_res(s.noise);
      assign_moduleType assign_moduleType_(s.moduleType);
      assign_declared_var_name assign_declared_var_name_(s.var);
      assign_extern_var_name assign_extern_var_name_(s.var);
      assign_missing_min_neginf assign_missing_min_neginf_(s.mis); 
      assign_missing_max_posinf assign_missing_max_posinf_(s.mis); 
      assign_missing_sigma2_posinf assign_missing_sigma2_posinf_(s.mis); 
      assign_missing_min assign_missing_min_(s.mis); 
      assign_missing_max assign_missing_max_(s.mis); 
      assign_noise_size assign_noise_size_(s.noise.info, s.var.count, s.numEqu_);
      assign_sigma_to_noise assign_sigma_to_noise_(s.sig.table, s.sig.vec, s.noise.info, s.numEqu_);
      assign_sig_pri assign_sig_pri_(s.noise.info);
      assign_const_sigma_to_res assign_const_sigma_to_res_(s.noise.info);
      assign_cov_to_res  assign_cov_to_res_ (s.noise.info);
      assign_covinv_to_res  assign_covinv_to_res_ (s.noise.info);
      assign_covl_to_res  assign_covl_to_res_ (s.noise.info);
      assign_covlinv_to_res  assign_covlinv_to_res_ (s.noise.info);
      assign_non_lin_flt_to_noise assign_non_lin_flt_to_noise_(s.noise.info);
      assign_reg_matrix assign_output_(s.descY_, s.exprY_, s.Y_);
      assign_reg_matrix assign_input_ (s.descX_, s.exprX_, s.X_);
      assign_missing_row assign_missing_row_(s.mis); 
      assign_missing_output assign_missing_output_(s.mis); 
      assign_missing_input assign_missing_input_(s.mis,s.var.vec); 

      assign_pos_sign_to_ine_term assign_pos_sign_to_ine_term_(s.ine_var_term_info);
      assign_neg_sign_to_ine_term assign_neg_sign_to_ine_term_(s.ine_var_term_info);
      assign_IsGE_to_ine assign_IsGE_to_ine_(s.ine_info);
      assign_IsLE_to_ine assign_IsLE_to_ine_(s.ine_info);
      assign_var_to_ine_term assign_var_to_ine_term_(s.var,s.ine_var_term_info,s.Anzmax_);
      add_term_to_ine add_term_to_ine_(s.ine_info.A,s.ine_var_term_info);
      add_ine add_ine_(s.ine_vec, s.ine_info);

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
      tolExpression = confix_p(str_p("{$"), (*(anychar_p)), "$}")
        ;
  // TOL identifiers
      identifier_simple =
        lexeme_d[((alpha_p | '_' ) >> *(alnum_p | '_' | '.'))]
        ;
  // TOL or NameBlock identifiers
      identifier =
        (*(identifier_simple >> member)) >> identifier_simple;
        ;
      nonExistentIdentifier = 
         (identifier - (s.var.table | s.sig.table | s.mis.table | s.noise.table));
      newIdentifier = 
        (
          nonExistentIdentifier |
          (identifier >> error_alreadyInUse >> nothing_p >> end_p)
        )
        ;
      variantSigma = 
        (newIdentifier[increment_a(s.sig.info.index)]
          [assign_a(s.sig.info.name)])[add_sig][assign_sigma_to_noise_] >>
        (
          ch_p('~') >>  
          (
            str_p("@Bsr.Sigma.Prior.InverseGamma") >> confix_p("(", (*(anychar_p)), ")")
            //| error_expectedSigmaPriorInverseGamma
          )[assign_sig_pri_] 
          | 
          eps_p
        )
        ;
      constantSigma = 
        ureal_p[assign_const_sigma_to_res_]
        ;
      covariance =          
        (str_p("Cov")       >>str_p("=")>>tolExpression[assign_cov_to_res_]    ) | 
        (str_p("CovInv")    >>str_p("=")>>tolExpression[assign_covinv_to_res_] ) | 
        (str_p("CovChol")   >>str_p("=")>>tolExpression[assign_covl_to_res_]   ) | 
        (str_p("CovInvChol")>>str_p("=")>>tolExpression[assign_covlinv_to_res_]) 
        ;
      arima = 
        (
          ( 
            str_p("ARIMA.COV") | 
            (str_p("Cov") >> str_p("=") >> str_p("ArimaCovariance"))
          ) >> 
          confix_p("([[", (*(anychar_p)), "]])")[assign_a(s.noise.info.arimaExpr)]
        )
        ;
      resSigmaDef = 
        (
          constantSigma | 
          variantSigma | 
          error_sigmaExpected
        ) >>
        (
          (str_p("*") >> (arima | covariance) ) 
          | eps_p
        )
        ;
      knownReal =
        real_p[assign_a(s.realValue)]
        ;
      unkOrReal =
        unknown[assign_a(s.realValue,BDat::Nan())]
        |
        knownReal
        ;
      unkOrRealInitValue =
        unknown[assign_a(s.var.info.initValue,BDat::Nan())]
        |
        real_p[assign_a(s.var.info.initValue)]
        ;
      variable =
        newIdentifier[assign_declared_var_name_] >> 
        str_p("<-") >> 
        (
          unkOrRealInitValue | error_linearVariableDeclareExpected
        )>>
        endOfSentence[add_var]
        ;
      variableExistentOrExtern =
        s.var.table |
        (
          nonExistentIdentifier[assign_extern_var_name_]
                               [add_var]
        );
      missing =
        newIdentifier[increment_a(s.mis.info.index)]
                            [assign_a(s.mis.info.name)] >> 
        ch_p('?') >> (
        (
          ((str_p("at") >> str_p("row") >> int_p[assign_missing_row_]) | 
           error_missingAtRowExpected) >> 
          (str_p("for") | error_missingForExpected) >>  
          (
            (
              str_p("output")[assign_missing_output_] | (
              str_p("input" ) >> int_p[assign_missing_input_] )
            ) 
          ) 
          >>
          (
            (
              str_p("<-")[assign_a(s.mis.info.prior,"None")]>>
              (real_p[assign_a(s.mis.info.nu)] | error_badNumber) >>
              (
                (
                  ch_p('~') >>
                  (
                    (
                      str_p("Uniform")[assign_a(s.mis.info.prior,"Uniform")] >> 
                      openParenthesys >>
                      ((str_p("?")|real_p)[assign_missing_min_] | error_badNumber) >>
                      argumentSeparator >>
                      ((str_p("?")|real_p)[assign_missing_max_] | error_badNumber) >>
                      closeParenthesys
                    ) 
                  )
                )
                | eps_p[assign_missing_min_neginf_]
                       [assign_missing_max_posinf_]
              )[assign_missing_sigma2_posinf_]
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
                  ((str_p("?")|real_p)[assign_missing_min_] | error_badNumber) >>
                  argumentSeparator >>
                  ((str_p("?")|real_p)[assign_missing_max_] | error_badNumber) >>
                  closeParenthesys
                )
              ) 
            )
          ) 
        )    
        | error_missingVariableDeclareExpected)
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

      nonLinearFilters = (
        str_p("with") >> str_p("non") >> str_p("linear") >> str_p("filters") >>
        tolExpression[assign_non_lin_flt_to_noise_]
      ) | (
         eps_p[assign_non_lin_flt_to_noise_]
      );

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
        nonLinearFilters >>
        endOfSentence[add_res]
        ;
      output = str_p("Output") >> ch_p('=') >>  tolExpression[assign_output_]
        >> endOfSentence;
      input  = str_p("Input")  >> ch_p('=') >>  tolExpression [assign_input_]
        >> endOfSentence;

      posSign = str_p("+")
        ;
      negSign = str_p("-")
        ;
      signIneVarTerm = 
        posSign[assign_pos_sign_to_ine_term_] 
        | 
        negSign[assign_neg_sign_to_ine_term_]
        ;
      ineVarTerm = 
        signIneVarTerm >>
        (
          variableExistentOrExtern[assign_var_to_ine_term_]
          |
          (
            knownURealIneTerm >>
            (product                                           | error_prodExpected     ) >> 
            (variableExistentOrExtern[assign_var_to_ine_term_] | error_linBlkVarExpected)
          )
        )
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
      module_type =
      (
        (
          str_p("Module.Type") >> ch_p('=') >> 
          str_p("primary")[assign_moduleType_] >> 
          endOfSentence
        ) |
        error_ModuleTypePrimaryDefExpected
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
      problem = 
        explicit_begin >>
        //header
        module_type >>
        model_nameDef >>
        model_descriptionDef >>
        session_nameDef >>
        session_descriptionDef >>
        session_authorDef 
        >> (
        //body
          *(variable) >> 
          noise  >> 
          output >> 
          (input | eps_p) >> 
          *(missing)  >>
          *(inequation)
        ) >>
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
  int checkDimensions(int m)
  /////////////////////////////////////////////////////////////////////////////
  {
    int b = noise.vec.size();
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
    int i,j;
    int n = var.vec.size();
    int b = noise.count;
    int m = numEqu_;
    int r = ine_vec.size();
    if(!n && !X_.Rows()) { X_.BlasRDense(m,0); }
    Std(BSR()+" Building model definition of primary module\n");
    int result = checkDimensions(m);
    if(result) { return(result); }
    linearInfo_  = var.vec;
    noiseInfo_   = noise.vec;
    Std(BSR()+" Building noise "+noiseInfo_[0].name.c_str()+"\n");
    for(i=1; i<=m; i++)
    {
      noiseInfo_[0].equIdx.push_back(i);
    }
    if(expand2AllEqu_covAndFactors(noiseInfo_[0]))
    {
      return(-6);
    }
    else if(result = Y_.Rows()!=m)
    {
      Error(BSR()+" Number of rows of output matrix ("+Y_.Rows()+
            ") is not equal to declared noise length "+m);
    }   
    else if(result = X_.Rows()!=m)
    {
      Error(BSR()+" Number of rows of input matrix ("+X_.Rows()+
            ") is not equal to declared noise length "+m);
    }   
    else if(result = X_.Columns()!=n)
    {
      Error(BSR()+" Number of columns of input matrix ("+X_.Columns()+
            ") is not equal to number of declared variables "+n);
    }   
    else
    {
      Y = Y_;
      X = X_;
      BVMat A_;
      Std(BSR()+" Allocating dense inequation border with "+r+" cells\n");
      a_.BlasRDense(r,1);
      Std(BSR()+" Allocating triplet sparse inequation coefficeints with "+Anzmax_+" cells\n");
      A_.ChlmRTriplet(r,n,Anzmax_);
      size_t& An = A_.s_.chlmRtriplet_->nnz;
      double* ax = (double*)a_.s_.blasRdense_->x;
      double* Ax = (double*)A_.s_.chlmRtriplet_->x;
      int*    Ai = (int*)A_.s_.chlmRtriplet_->i;
      int*    Aj = (int*)A_.s_.chlmRtriplet_->j;
      int oldRatio = 0;
      An = 0;
      Std(BSR()+"Building "+r+"constrain inequations with "+n+" variables\n");
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
      result = getMissing(Y,X,inputMissingInfo_, outputMissingInfo_);
    }
    if(!result) { Std(BSR()+"Succesfully build\n"); }
    return(result);
  };
};

////////////////////////////////////////////////////////////////////////////////
int Parse_Module_Primary(
  const string &          fileName,
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
  bys_sparse_reg_primary bsr;
  int errCode = 0;
  bool verbose = true;
  #include "tol_bvmat_bsr_run.h"
  if(!errCode)
  {
    docInfo.model_name          =  bsr.docInfo.model_name;
    docInfo.model_description   =  bsr.docInfo.model_description;
    docInfo.session_name        =  bsr.docInfo.session_name;
    docInfo.session_description =  bsr.docInfo.session_description;
    docInfo.session_authors     =  bsr.docInfo.session_authors;
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

