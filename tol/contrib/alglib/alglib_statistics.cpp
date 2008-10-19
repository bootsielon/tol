/* alglib_statistics.cpp: Header file for interface between C++ and C for 
                      AlgLib and GNU/TOL Language.

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
  \file   alglib_interp.cpp
  \author vdebuen@tol-project.org
  \date   2007-08-21

  \brief  API of TOL for AlgLib interpolation functions  
*/


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <math.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>

#include "jarquebera.h"
#include "studentttests.h"
#include "stest.h"
#include "wsr.h"
#include "mannwhitneyu.h"
#include "variancetests.h"
#include "correlationtests.h"

#define dMat(arg) ((DMat&)Mat(arg))
#define b2dMat(M) ((DMat&)(M))


BTraceInit("alglib_statistics.cpp");

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAlgLibJarqueBeraTest);
  DefExtOpr(1, BDatAlgLibJarqueBeraTest, "AlgLib.JarqueBeraTest", 1, 1, 
  "Matrix",  "(Matrix x)",
  I2("The Jarque-Bera test is used to check hypothesis about the fact that "
     "a given sample xS  is a sample of normal random variable with unknown "
     "mean and dispersion. As a rule, this test is applied before using methods "
     "of parametric statistics which require distribution normality."
     "\nSee ",
     "La prueba de Jarque-Bera se utiliza para comprobar la hipótesis sobre el "
     "hecho de que una muestra dada x proviene de una variable aleatoria "
     "normal con media y dispersión desconocidas. Por regla general, esta "
     "prueba se aplica antes de utilizar métodos paramétricos que requieren "
     "normalidad."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/jarqueberatest.php",
  BOperClassify::Statistic_);
  void BDatAlgLibJarqueBeraTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  double p;
  int n = X.Data().Size();
  ap::real_1d_array x;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);
  jarqueberatest(x,n,p);
  contens_ = p;
};


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibStudentTtest1);
  DefExtOpr(1, BSetAlgLibStudentTtest1, "AlgLib.StudentTtest1", 2, 2, 
  "Matrix Real",  "(Matrix x, Real m)",
  I2("This test is used to check hypotheses about the fact that the mean of random "
     "variable X equals to given m. Testing sample should be a sample of a normal "
     "random variable."
     "\nSee ",
     "Esta prueba se utiliza para comprobar la hipótesis sobre el hecho de que la "
     "media de la variable aleatoria X es igual a la dada m. Las muestras para la "
     "prueba debe ser una muestra de una variable aleatoria normal."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/studentttest.php",
  BOperClassify::Statistic_);
  void BSetAlgLibStudentTtest1::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  double m = Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  ap::real_1d_array x;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);

  studentttest1(x, n, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "(null hypothesis - the median is equal to the given value)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "(null hypothesis - the median is greater than or equal to the given value)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "(null hypothesis - the median is less than or equal to the given value)"));
  BGrammar::DecLevel();
};


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibStudentTtest2);
  DefExtOpr(1, BSetAlgLibStudentTtest2, "AlgLib.StudentTtest2", 2, 2, 
  "Matrix Matrix",  "(Matrix x, Matrix y)",
  I2("This test checks hypotheses about the fact that the means of two random "
     "variables X and Y which are represented by samples xS  and yS  are equal. "
     "The test works correctly under the following conditions:\n"
     " - both random variables have a normal distribution\n"
     " - dispersions are equal (or slightly different)\n"
     " - samples are independent..\n"
     "\nSee ",
     "Esta prueba chequea la hipótesis de que la media de dos variables "
     "aleatorias X e Y, que son representados por las muestras <x> e <y> son "
     "iguales. La prueba funciona correctamente bajo las siguientes condiciones:\n"
     " - Ambas variables aleatorias tienen una distribución normal\n"
     " - Las dispersiones son iguales (o ligeramente diferente)\n"
     " - Las muestras son independientes.\n"
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/studentttest.php",
  BOperClassify::Statistic_);
  void BSetAlgLibStudentTtest2::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  DMat& Y = dMat(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  int m = Y.Data().Size();
  ap::real_1d_array x;
  ap::real_1d_array y;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);
  y.setcontent(0,m-1,Y.Data().Buffer(), false);

  studentttest2(x, n, y, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test (null hypothesis - the medians are equal)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test (null hypothesis - the median of the first "
  "sample is greater than or equal to the median of the second sample)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test (null hypothesis - the median of the first "
  "sample is less than or equal to the median of the second sample)"));
  BGrammar::DecLevel();
};

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibUnequalVarianceTTest);
  DefExtOpr(1, BSetAlgLibUnequalVarianceTTest, "AlgLib.UnequalVarianceTTest", 2, 2, 
  "Matrix Matrix",  "(Matrix x, Matrix y)",
  I2("This test checks hypotheses about the fact that the means of two random "
     "variables X and Y which are represented by samples xS  and yS  are equal. "
     "The test works correctly under the following conditions:\n"
     " - both random variables have a normal distribution\n"
     " - samples are independent..\n"
     "\nSee ",
     "Esta prueba chequea la hipótesis de que la media de dos variables "
     "aleatorias X e Y, que son representados por las muestras <x> e <y> son "
     "iguales. La prueba funciona correctamente bajo las siguientes condiciones:\n"
     " - Ambas variables aleatorias tienen una distribución normal\n"
     " - Las muestras son independientes.\n"
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/studentttest.php",
  BOperClassify::Statistic_);
  void BSetAlgLibUnequalVarianceTTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  DMat& Y = dMat(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  int m = Y.Data().Size();
  ap::real_1d_array x;
  ap::real_1d_array y;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);
  y.setcontent(0,m-1,Y.Data().Buffer(), false);

  unequalvariancettest(x, n, y, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test (null hypothesis - the medians are equal)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test (null hypothesis - the median of the first "
  "sample is greater than or equal to the median of the second sample)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test (null hypothesis - the median of the first "
  "sample is less than or equal to the median of the second sample)"));
  BGrammar::DecLevel();
};


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibWilcoxonSignedRankTest);
  DefExtOpr(1, BSetAlgLibWilcoxonSignedRankTest, "AlgLib.WilcoxonSignedRankTest", 2, 2, 
  "Matrix Real",  "(Matrix x, Real m)",
  I2("The Wilcoxon signed-rank test is a non-parametric test used to compare "
     "the distribution median with a given value m. This criterion could be "
     "used as an alternative for one-sample Student t-test. Unlike the t-test, "
     "Wilcoxon signed-rank test can work with non-normal distributions."
     "\nSee ",
     "El test de Wilcoxon signed-rank es un test no paramétrico utilizado "
     "para comparar la mediana de la distribución con un determinado valor "
     "m. Este criterio podrí ser utilizado como una alternativa para una "
     "muestra de t de Student. A diferencia de la t de Student, Wilcoxon "
     "puede trabajar con distribuciones no normales."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/wilcoxonsignedrank.php",
  BOperClassify::Statistic_);
  void BSetAlgLibWilcoxonSignedRankTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  double m = Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  ap::real_1d_array x;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);

  wilcoxonsignedranktest(x, n, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "(null hypothesis - the median is equal to the given value)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "(null hypothesis - the median is greater than or equal to the given value)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "(null hypothesis - the median is less than or equal to the given value)"));
  BGrammar::DecLevel();
}

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibOneSampleSignTest);
  DefExtOpr(1, BSetAlgLibOneSampleSignTest, "AlgLib.OneSampleSignTest", 2, 2, 
  "Matrix Real",  "(Matrix x, Real m)",
  I2("The sign test is a non-parametric test used to compare the distribution "
     "median with a given value m. This test could be used as an alternative for "
     "one-sample Student t-test. Unlike the t-test, the sign test can work with "
     "non-normal distributions."
     "\nSee ",
     "La señal de signos es un test no paramétrico utilizado para comparar con "
     "la mediana de la distribución de un determinado valor m. Esta prueba podrí "
     "ser utilizada como una alternativa para una muestra de t de Student. A "
     "diferencia de la t de Student, la prueba de signo puede trabajar con "
     "distribuciones no normales."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/signtest.php",
  BOperClassify::Statistic_);
  void BSetAlgLibOneSampleSignTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  double m = Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  ap::real_1d_array x;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);

  onesamplesigntest(x, n, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "(null hypothesis - the median is equal to the given value)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "(null hypothesis - the median is greater than or equal to the given value)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "(null hypothesis - the median is less than or equal to the given value)"));
  BGrammar::DecLevel();
}


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibMannWhitneyUTest);
  DefExtOpr(1, BSetAlgLibMannWhitneyUTest, "AlgLib.MannWhitneyUtest", 2, 2, 
  "Matrix Matrix",  "(Matrix x, Matrix y)",
  I2("The Mann-Whitney U-test is a non-parametric method which is used as an "
     "alternative to the two-sample Student's t-test. Usually this test is "
     "used to compare medians of non-normal distributions X and Y (the t-test "
     "is not applicable because X and Y are not normal).\n"
     "\nSee ",
     "El test Mann-Whitney-U es un método no paramétrico que se utiliza como "
     "una alternativa a los dos-muestra la prueba t de Student. Por lo "
     "general, esta prueba se utiliza para comparar las medianas de las "
     "distribuciones no normales X e Y (el t-test no es aplicable porque X e Y "
     "no son normales)"
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/mannwhitneyu.php",
  BOperClassify::Statistic_);
  void BSetAlgLibMannWhitneyUTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  DMat& Y = dMat(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  int m = Y.Data().Size();
  ap::real_1d_array x;
  ap::real_1d_array y;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);
  y.setcontent(0,m-1,Y.Data().Buffer(), false);

  mannwhitneyutest(x, n, y, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test (null hypothesis - the medians are equal)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test (null hypothesis - the median of the first "
  "sample is greater than or equal to the median of the second sample)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test (null hypothesis - the median of the first "
  "sample is less than or equal to the median of the second sample)"));
  BGrammar::DecLevel();
}


//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibVarianceChiSquareTest);
  DefExtOpr(1, BSetAlgLibVarianceChiSquareTest, "AlgLib.VarianceChiSquareTest", 2, 2, 
  "Matrix Real",  "(Matrix x, Real s2)",
  I2("This test is used to check hypotheses about the fact that the variance of random "
     "variable X equals to given s2. Testing sample should be a sample of a normal "
     "random variable, or a very large sample of any distribution."
     "\nSee ",
     "Esta prueba se utiliza para comprobar la hipótesis sobre el hecho de que la "
     "varianza de la variable aleatoria X es igual a la dada s2. La muestra para la "
     "prueba debe ser una muestra de una variable aleatoria normal, aunque para "
     "muestras muy grandes funciona bien para cualquer distribución."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/variancetests.php",
  BOperClassify::Statistic_);
  void BSetAlgLibVarianceChiSquareTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  double m = Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  ap::real_1d_array x;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);

  onesamplevariancetest(x, n, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "(null hypothesis - the median is equal to the given value)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "(null hypothesis - the median is greater than or equal to the given value)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "(null hypothesis - the median is less than or equal to the given value)"));
  BGrammar::DecLevel();
};

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibVarianceFTest);
  DefExtOpr(1, BSetAlgLibVarianceFTest, "AlgLib.VarianceFTest", 2, 2, 
  "Matrix Matrix",  "(Matrix x, Matrix y)",
  I2("This test is used to check hypotheses about the fact that the variance of random "
     "variable X equals to variable Y. Testing samples should be samples of normal "
     "random variables, or very large samples of any distribution."
     "\nSee ",
     "Esta prueba se utiliza para comprobar la hipótesis sobre el hecho de que la "
     "varianza de la variable aleatoria X es igual a la de Y. La muestra para la "
     "prueba debe ser una muestra de una variable aleatoria normal, aunque para "
     "muestras muy grandes funciona bien para cualquer distribución."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/variancetests.php",
  BOperClassify::Statistic_);
  void BSetAlgLibVarianceFTest::CalcContens()
//--------------------------------------------------------------------
{
  DMat& X = dMat(Arg(1));
  DMat& Y = dMat(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  int n = X.Data().Size();
  int m = Y.Data().Size();
  ap::real_1d_array x;
  ap::real_1d_array y;
  
  x.setcontent(0,n-1,X.Data().Buffer(), false);
  y.setcontent(0,m-1,Y.Data().Buffer(), false);

  ftest(x, n, y, m, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test (null hypothesis - the variances are equal)"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test (null hypothesis - the variance of the first "
  "sample is greater than or equal to the variance of the second sample)"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test (null hypothesis - the variance of the first "
  "sample is less than or equal to the variance of the second sample)"));
  BGrammar::DecLevel();
};

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibPearsonCorrelationSignificance );
  DefExtOpr(1, BSetAlgLibPearsonCorrelationSignificance, "AlgLib.PearsonCorrelationSignificance", 2, 2, 
  "Real Real",  "(Real r, Real n)",
  I2("This test checks hypotheses about whether X and Y with sample correlation r are "
     "samples of two normal distributions having zero correlation or whether their "
     "correlation is non-zero.."
     "\nSee ",
     "Esta prueba chequea la hipótesis de que dos muestras X e Y con correlación "
     "muestral r son muestras de dos distribuciones normales con correlación cero "
     "o si su correlación es distinto de cero.."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/correlation.php",
  BOperClassify::Statistic_);
  void BSetAlgLibPearsonCorrelationSignificance::CalcContens()
//--------------------------------------------------------------------
{
  double r = Real(Arg(1));
  int    n = (int)Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  pearsoncorrelationsignificance(r, n, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  BGrammar::DecLevel();
};

//--------------------------------------------------------------------
  DeclareContensClass(BSet, BSetTemporary, BSetAlgLibSpearmanRankCorrelationSignificance );
  DefExtOpr(1, BSetAlgLibSpearmanRankCorrelationSignificance, "AlgLib.SpearmanRankCorrelationSignificance", 2, 2, 
  "Real Real",  "(Real r, Real n)",
  I2("This test checks hypotheses about whether X and Y with sample correlation r are "
     "samples of two continuous distributions having zero correlation or whether their "
     "correlation is non-zero.."
     "\nSee ",
     "Esta prueba chequea la hipótesis de que dos muestras X e Y con correlación "
     "muestral r son muestras de dos distribuciones continuous con correlación cero "
     "o si su correlación es distinto de cero.."
	   "\nVer ")+
  "http://www.alglib.net/statistics/hypothesistesting/correlation.php",
  BOperClassify::Statistic_);
  void BSetAlgLibSpearmanRankCorrelationSignificance::CalcContens()
//--------------------------------------------------------------------
{
  double r = Real(Arg(1));
  int    n = (int)Real(Arg(2));
  double bothtails;
  double lefttail;
  double righttail;
  
  spearmanrankcorrelationsignificance(r, n, bothtails, lefttail, righttail);
  BGrammar::IncLevel();
  contens_.PrepareStore(3);
  contens_.AddElement(BContensDat::New("bothtails", bothtails,    
  "p-value for two-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  contens_.AddElement(BContensDat::New("lefttail", lefttail,    
  "p-value for left-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  contens_.AddElement(BContensDat::New("righttail", righttail, 
  "p-value for right-tailed test "
  "If it's less than the given significance level the null hypothesis is rejected"));
  BGrammar::DecLevel();
};

