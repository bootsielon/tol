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
  BVMat Y_;
  BVMat X_;

  bys_sparse_reg_primary() : bys_sparse_reg() {}
    
  template <typename ScannerT>
  struct definition
  {
    bys_sparse_reg_primary& s;
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
      variable, missing, arima, noise, equation, inequation,
      model_nameDef, model_descriptionDef, 
      session_nameDef, session_descriptionDef, session_authorDef, 
      explicit_begin, explicit_end, problem;

    definition(const bys_sparse_reg_primary& s_)
    : s ((bys_sparse_reg_primary&)s_)
    {
      add_symbol<variable_info> add_var(s.var);
      add_symbol<missing_info>  add_mis(s.mis);
      add_symbol<sigma_info>    add_sig(s.sig);
      add_symbol<noise_info>    add_res(s.noise);
      assign_missing_min assign_missing_min_(s.mis); 
      assign_missing_max assign_missing_max_(s.mis); 
      assign_noise_size assign_noise_size_(s.noise.info, s.var.count, s.numEqu_);
      assign_sigma_to_noise assign_sigma_to_noise_(s.sig.table, s.sig.vec, s.noise.info, s.numEqu_);
      assign_sig_pri assign_sig_pri_(s.noise.info);
      assign_const_sigma_to_res assign_const_sigma_to_res_(s.noise.info);
      assign_covariance_to_res  assign_covariance_to_res_ (s.noise.info);
      assign_noise_to_term assign_noise_to_term_(s.noise.vec,s.sig.vec);
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
          [assign_a(s.sig.info.name)])[add_sig][assign_sigma_to_noise_] >>
        (
          ch_p('~') >>  
          (
            str_p("Bsr.Sigma.Prior.InverseGamma") >> confix_p("(", (*(anychar_p)), ")")
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
        [assign_covariance_to_res_]
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
      errorTerm = noiseName >> noisePosition
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
                ((str_p("?")|real_p)[assign_missing_min_] | error_badNumber) >>
                argumentSeparator >>
                ((str_p("?")|real_p)[assign_missing_max_] | error_badNumber) >>
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
           noise ) >>
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
    int i,j,k;
    int n = var.vec.size();
    int b = noise.vec.size();
    int m = Y_.Rows();
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
    linearInfo_  = var.vec;
    noiseInfo_   = noise.vec;
    for(i=0; i<noiseInfo_.size(); i++)
    {
      Std(BSR()+" Building noise "+noiseInfo_[i].name.c_str()+"\n");
      if(expand2AllEqu_covAndFactors(noiseInfo_[i]))
      {
        return(-6);
      }
    }
    Y = Y_;
    X = X_;
    a.BlasRDense(0,1);
    A.ChlmRTriplet(0,0,0);

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
    Std(BSR()+"Succesfully build\n");
    return(0);
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

