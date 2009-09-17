/* vmat_bsr_err_h: BVMat Bayesian Sparse Regression 
                              parsing errors
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


error_report_p error_beginExpected = error_report_parser(
  "Expected $BEGIN" );
error_report_p error_endExpected = error_report_parser(
  "Expected $END" );

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

error_report_p error_ModuleTypePrimaryDefExpected = error_report_parser(
  "Expected primary module type definition 'Module.Type = primary' " );
error_report_p error_ModuleTypeJointDefExpected = error_report_parser(
  "Expected joint module type definition 'Module.Type = joint' " );
error_report_p error_ModuleTypeMasterDefExpected = error_report_parser(
  "Expected master module type definition 'Module.Type = master' " );
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
