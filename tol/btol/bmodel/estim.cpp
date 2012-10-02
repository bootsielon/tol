/* estim.cpp: Functions of BEstim.
              GNU/TOL Language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_matrix.h>
#include <tol/tol_bestim.h>
#include <tol/tol_blinalg.h>
#include <tol/tol_blinreg.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bar.h>
#include <tol/tol_bunitroot.h>
#include <tol/tol_blogit.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>

#include <tol/tol_blas.h> 
#include <tol/tol_lapack.h> 

//#include <contrib/fftw/bfft.h>

//#define _USE_GSL_MARQUARDT_

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BEstim).
//--------------------------------------------------------------------
BTraceInit("estim.cpp");

BText& JacobianMethod();

//Static Names and Descriptions to be used in built-in functions
//PREFIX_ must be defined in a different way in each 
// .cpp that uses these macroes to avoid repeated instantiations
// linker errors
#define FNDParam1(NAME)                 PrefixFNDParam1(ESTIM_, NAME);
#define FNDParam(NAME, DESC)            PrefixFNDParam(ESTIM_, NAME, DESC);
#define NewFND(TYPE, NAME, OBJ)         PrefixNewFND(TYPE, ESTIM_, NAME, OBJ)
#define NewSetFND(NAME, ELEM, STR, SUB) PrefixNewSetFND(ESTIM_, NAME, ELEM, STR, SUB)


FNDParam(Alfa,        "P-Value of F-Schnedecor statistic");
FNDParam(COR,         "Correlation Inverse Choleski Matrix");
FNDParam(COV,         "Covarianze Matrix");
FNDParam(CovInf,      "Covarianze Information");
FNDParam(COVL,        "Covarianze Choleski Matrix");
FNDParam(COVLi,       "Covarianze Inverse Choleski Matrix");
FNDParam(DataNumber,  "Length of residuals");
FNDParam(F,           "Regression F-Schnedecor statistic");
FNDParam(FirstDate,   "First date of estimation");
FNDParam(Information, "Statitistics information");
FNDParam(LastDate,    "Last date of estimation");
FNDParam(ParInf,      "Parameters Information");
FNDParam(R2,          "Regression R2 statistic");
FNDParam(RSS,         "Residual Sum of Squares");
FNDParam(Sigma,       "Standard error");
FNDParam(Swartz,      "Swartz information criterium value");
FNDParam(TSS,         "Total Sum of Squares");
FNDParam(v1,          "First freedom degree of F-Schnedecor statistic");
FNDParam(v2,          "Second freedom degree of F-Schnedecor statistic");
FNDParam(VarNumber,   "Number of variables");

FNDParam(z,         "Noise");
FNDParam(w,         "Differenced noise");
FNDParam(ACoviw,    "Inv(ACOV)*w : Is an auxiliar variable but can be usefull");
FNDParam(a,         "Tra(Psi)*Inv(ACOV)*W : Model residuals conditioned by difference noise.");
FNDParam(ACOV,      "Model autocovarianze function");
FNDParam(ACF,       "Model autocorrelation function");
FNDParam(PACF,      "Model partial autocorrelation function");
FNDParam(IACF,      "Model inverse autocorrelation function");
FNDParam(ACOVDetN,  "Det(ACOV)^(1/N): N-th root of autocovarianze matrix determinant");
FNDParam(wtACOViw,  "w'*ACOViw : Estiamted squares sum of differenced noise in autocovariance metrics");
FNDParam(aS2,       "a'*a : Residual's squares sum");
FNDParam(logLH,     "-(N/2)*Log(2*pi*e*ACOVDetN*WAtCOViW/N) : Logarithm of likelihhod function");
FNDParam(w0,        "Initial values of differenced noise");
FNDParam(a0,        "Initial values of residuals");


//--------------------------------------------------------------------
BEstimation::BEstimation(BModel* model, const BDate& f, const BDate& l)

/*! BEstimation constructor
 */
//--------------------------------------------------------------------
: BRnRmFunction(), M_(model), first_(f), last_(l)
{
  if((ok_ = M_->InitData(f,l) && M_->CheckData()))
  {
    M_->GetParameter();
    M_->PutParameter(M_->param_);
    BInt I = M_->interruptionIndex_.Size();
    BInt r = M_->paramAPrioriResiduals_.Size();
//     n_ = M_->numParam_;
//     m_ = M_->N_+r;
    SetDimensions(M_->numParam_, M_->N_+r+M_->lagrangianParam_);
    Std(I2("\n Variables     : ", "\n Variables	      : ")+n_);
    Std(I2("\n Data	         : ", "\n Datos	          : ")+(M_->N_));
    Std(I2("\n Interruptions : ", "\n Interrupciones  : ")+I);
    Std(I2("\n Restrictions  : ", "\n Restricciones   : ")+r);
    Std(I2("\n DIF degree    : ", "\n Grado DIF	      : ")+M_->dif_.Degree());
    Std(I2("\n AR degree     : ", "\n Grado AR	      : ")+M_->ar_.Degree());
    Std(I2("\n MA degree     : ", "\n Grado MA	      : ")+M_->ma_.Degree());
    Std(I2("\n Jacobian	     : ", "\n Jacobiano	      : ")+JacobianMethod());
  }
  M_->initValues_	   = BTRUE;
  M_->recalcInterruptions_ = BTRUE;
  M_->recalculateDifNoise_ = BTRUE;

}


//--------------------------------------------------------------------
BBool BEstimation::Marquardt()

/*! BEstimation constructor
 */
//--------------------------------------------------------------------
{
  M_->iterationNumber_ = 0;
  if(ok_ && M_->numParam_)
  {
    M_->GetParameter();
    BArray<BDat>  x, y;
    BMatrix<BDat> J;
    BDat oldError = M_->initialError_;
    BDat difError;
    if(JacobianMethod()=="Both")
    {
      JacobianMethod() = "Analytical";
      M_->standardError_= LeastSqrMarquardt(M_->param_,
					    M_->residuous_,
					    M_->jacobian_);
      JacobianMethod() = "Numerical";
      M_->standardError_= LeastSqrMarquardt(M_->param_,
					    M_->residuous_,
					    M_->jacobian_);
      JacobianMethod() = "Both";
    }
    else if (JacobianMethod()=="Numerical") {
#ifdef _USE_GSL_MARQUARDT_
      M_->standardError_ = gsl_Marquardt(M_->param_,
#else
      M_->standardError_= LeastSqrMarquardt(M_->param_,
#endif
					    M_->residuous_,
					    M_->jacobian_);
    } else {
      M_->standardError_= LeastSqrMarquardt(M_->param_,
					    M_->residuous_,
					    M_->jacobian_);
   }
    ok_ = M_->standardError_.IsKnown();
  }
  if(ok_)
  {
    if(M_->numParam_)
    {
      M_->GetParameter();
      BInt N1 = M_->residuous_.Size();
      BInt N2 = M_->paramAPrioriResiduals_.Size();
      N1 -= N2;
      M_->residuous_.ReallocBuffer(N1);
      M_->standardError_ = Sqrt(MtMSqr(M_->A_ )(0,0)/M_->A_.Rows());
      M_->informationMatrix_ = M_->informationMatrix_/(M_->standardError_^2);
      M_->IntegrateMissingValues();
    }
    M_->Statistics();
    M_->Diagnostics();
    Std("\n");
  }
  return(ok_);
}


//--------------------------------------------------------------------
void BEstimation::Evaluate(BArray<BDat>&res, const BArray<BDat>& p)
//--------------------------------------------------------------------
{
    M_->Evaluate(res,p);
}


//--------------------------------------------------------------------
void BEstimation::Jacobian (const BArray<BDat>& x,
			    const BArray<BDat>& fx,
			    BMatrix<BDat>& J)
//--------------------------------------------------------------------
{
//BTimer tm(JacobianMethod()+" jacobian method");
  if(M_->aborted_) { return; }
  /*
  if (M_->recalcInterruptions_)
  {Std(BText("\n Entrada de Jacobian: True"));} 
  else {Std(BText("\n Entrada de Jacobian: False"));}
  */
  if(JacobianMethod()=="Analytical") { M_->CalcAnaliticalJacobian(x,fx,J); }
  else				     { M_->CalcNumericalJacobian (x,fx,J); }
//  else				     { BRnRmFunction::Jacobian	 (x,fx,J); }

  M_->iterationNumber_ ++;
  /*
  if (M_->recalcInterruptions_)
  {Std(BText("\n Salida de Jacobian: True"));} 
  else {Std(BText("\n Salida de Jacobian: False"));}
  */
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetEstimate);
DefExtOpr(1, BSetEstimate, "Estimate", 1, 4, "Set Date Date Set",
  I2("(Set modelDef [, Date from, Date until, Set parameterAPriori])",
     "(Set modelDef [, Date desde, Date hasta], Set parametrosAPriori)"),
  I2("Estimates the ARIMA model P(B):Zt = Xt*W + Q(B):At.",
     "Estima el modelo ARIMA P(B):Zt = Xt*W + Q(B):At."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetEstimate::CalcContens()
//--------------------------------------------------------------------
{
  BStruct* strModel  = FindStruct("@ModelDef");
  assert(strModel);
  BInt n=0;
  BSet& set = Set(Arg(1));
  if(!set.Card() || (set.Struct()!=strModel)) 
  { 
    Error(I2("First argument of Estimate must be a Set with structure "
             "@ModelDef",
             "El primer argumento para Estimate debe ser se tipo Set con "
             "estructura @ModelDef"));
    return; 
  }
  BModel  mod(set);
  BDate	 fst;
  BDate	 lst;
  BSet	 pri;
  if(Arg(2)) { fst = Date(Arg(2)); }
  if(Arg(3)) { lst = Date(Arg(3)); }
  if(Arg(4)) { pri = Set (Arg(4)); }
  if(!fst.HasValue()) { fst = mod.output_->FirstDate(); }
  if(!lst.HasValue()) { lst = mod.output_->LastDate (); }

  if(pri.Card())
  {
    mod.paramAPrioriName_  . ReallocBuffer(pri.Card());
    mod.paramAPrioriOrder_ . ReallocBuffer(pri.Card());
    mod.paramAPrioriNu_	   . ReallocBuffer(pri.Card());
    mod.paramAPrioriSigma_ . ReallocBuffer(pri.Card());
    for(n=0; n<pri.Card(); n++)
    {
      BSet param = Set(pri[n+1]);
      mod.paramAPrioriName_  [n] =	  Text (param[1]);
      mod.paramAPrioriOrder_ [n] = (BInt) Real (param[2]);
      mod.paramAPrioriNu_    [n] =	  Dat  (param[3]);
      mod.paramAPrioriSigma_ [n] =	  Dat  (param[4]);
      BBool fixed = mod.paramAPrioriNu_[n].IsKnown() &&
		    mod.paramAPrioriSigma_ [n].IsKnown();
      BText aux = " FREE.";
      if(fixed)
      {
	      aux = BText(" RESTRICTED to Normal(") +mod.paramAPrioriNu_[n]+","+
				      mod.paramAPrioriSigma_ [n]+")";
      }
      Std(BText("\nParameter ")+ mod.paramAPrioriName_	[n] +
		  " with order " + mod.paramAPrioriOrder_ [n] + " is " +aux);
    }
  }
  BGrammar::IncLevel();
  BEstimation estim(&mod,fst,lst);
  if(estim.Marquardt())
  {
    mod.CopyInfToSet(contens_);
  }  
  BGrammar::DecLevel();
  return;
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLinearRegression);
DefExtOpr(1, BSetLinearRegression, "LinearRegression", 2, 2, "Serie Set",
  "(Serie y, Set X)",
  I2("Estimates the linear regression model yt = Xt*A + et.",
     "Estima el modelo de regresión lineal yt = Xt*A + et."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetLinearRegression::CalcContens()
//--------------------------------------------------------------------
{
  BStruct* strParam = FindStruct("@LinRegParamInf");
  if(!strParam)
  {
    Error(I2("@LinRegParamInf structure is not implementeded.",
	     "La estructura @LinRegParamInf no está implementada."));
    return;
  }
  BInt i;
  BUserTimeSerie* y	  = Tsr(Arg(1));
  BSet&		  X	  = Set(Arg(2));
  BUserTimeSet*	  tms	  = y->Dating();  if(!tms) { return; }
  BText		  name	  = y->Identify();
  BText		  resName = BText("Res")+name;

  BSerieGroup	  group;
  group.AddSerie(y);
  group.AddSet  (X);
  group.PrepareOperate();
  if(!group.TestDates()) { return; }
  BDate f = group.MaxFirst();
  BDate l = group.MinLast();
  BSerieTable yTable;
  yTable.AddSerie(y);
  yTable.Fill(f, l);
  BSerieTable XTable;
  XTable.AddSet(X);
  XTable.Fill(f, l);

  BArray<BDat> param, res;
  BMatrix<BDat> Li, L;
  BMatrix<BDat> cov, cor;
  BDat sigma;
  BDat R2;
  BDat F;
  BInt v1;
  BInt v2;
  BDat alfa;
  BDat resSqSum;
  BDat totSqSum;

  BInt N  = yTable.NumDates();
  BInt n  = XTable.NumSeries();
  BTStudentDist T(N-n);

  sigma = LinearRegression(yTable.Data().Data(),
		   XTable.Data().T(),
		   param,
		   res,
		   Li,
		   L,
		   cov,
		   cor,
		   R2,
		   F,
		   v1,
		   v2,
		   alfa,
		   resSqSum,
		   totSqSum);
  if(sigma.IsUnknown()) { return; }
  
  BDat sw = 2*Log(sigma)+n*Log(N)/N;

  BList* lst = NIL;
  BList* aux	= NIL;
  for(i=0; i<n; i++)
  {
    BList* lsta	   = NIL;
    BList* auxa	   = NIL;
    BDat   stDs	   = Sqrt(cov.Get(i,i));
    BDat   student = param[i]/stDs;
    BDat   prob	   = 2*(1-T.Dist(Abs(student)));
    LstFastAppend(lsta,auxa, BContensText::New(X[i+1]->Identify()));
    LstFastAppend(lsta,auxa, BContensDat::New(param[i]));
    LstFastAppend(lsta,auxa, BContensDat::New(stDs));
    LstFastAppend(lsta,auxa, BContensDat::New(student));
    LstFastAppend(lsta,auxa, BContensDat::New(prob));

    BUserSet* s = NewSet(lsta,strParam,BSet::Structured);
    LstFastAppend(lst,aux, s);
  }
  BUserSet* par = NewSetFND(ParInf,lst,NIL,BSet::Table);

  lst = NIL;
  aux = NIL;

  LstFastAppend(lst,aux,NewFND(BDate, FirstDate,  f));
  LstFastAppend(lst,aux,NewFND(BDate, LastDate,   l));
  LstFastAppend(lst,aux,NewFND(BDat,  DataNumber, N));
  LstFastAppend(lst,aux,NewFND(BDat,  VarNumber,  n));
  LstFastAppend(lst,aux,NewFND(BDat,  Sigma,      sigma));
  LstFastAppend(lst,aux,NewFND(BDat,  Swartz,     sw));
  LstFastAppend(lst,aux,NewFND(BDat,  RSS,        resSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  TSS,        totSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  R2,         R2));
  LstFastAppend(lst,aux,NewFND(BDat,  F,          F));
  LstFastAppend(lst,aux,NewFND(BDat,  Alfa,       alfa));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v1));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v2));

  BUserSet* inf = NewSetFND(Information,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;
  LstFastAppend(lst,aux,NewFND(BMat, COVLi, Li));
  LstFastAppend(lst,aux,NewFND(BMat, COVL,  L));
  LstFastAppend(lst,aux,NewFND(BMat, COV,   cov));
  LstFastAppend(lst,aux,NewFND(BMat, COR,   cor));

  BUserSet* covInf = NewSetFND(CovInf,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;

  LstFastAppend(lst,aux,new BTsrPrimary("",resName,tms,f,l,res));
  LstFastAppend(lst,aux,par);
  LstFastAppend(lst,aux,inf);
  LstFastAppend(lst,aux,covInf);

  contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetAutoRegression);
DefExtOpr(1, BSetAutoRegression, "AutoRegression", 1, 3, "Serie Real Set",
  "(Serie y, [ Real p = CountS(y)/4, Set input=Empty])",
  I2("Estimates the auto-regressive model of order p for the given series.",
     "Estima el modelo autoregresivo de orden p para la serie dada."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetAutoRegression::CalcContens()
//--------------------------------------------------------------------
{
  BStruct* strParam = FindStruct("@ParameterInf");
  if(!strParam)
  {
    Error(I2("@LinRegParamInf structure is not implementeded.",
	     "La estructura @LinRegParamInf no está implementa."));
    return;
  }
  BUserTimeSerie* y	  = Tsr(Arg(1));
  BSerieTable yTable; yTable.AddSerie(y);
  BInt N  = yTable.NumDates(), p=N/4, n=p;
  BTStudentDist T(N-n);
  if(Arg(2)) { p = (BInt)Real(Arg(2)); }
  if(p<1) { p=1; }
  BUserTimeSet*	  tms	  = y->Dating();  if(!tms) { return; }
  BDate		  f	  = tms->Next(y->FirstDate(),p);
  BDate		  l	  = y->LastDate();
  BText		  name	  = y->Identify();
  BText		  resName = BText("Res")+name;
  BInt i,j;

  BMatrix<BDat> Y = yTable.Data();
  BArray<BDat> arrY;
  arrY.ReallocBuffer(Y.Columns()-p);
  BMatrix<BDat> X(p, Y.Columns()-p);
  for(j=0; j<Y.Columns(); j++)
  {
    arrY(j) = Y(0,j+p);
    for(i=1; i<=p; i++)
    {
      X(i-1,j) = Y(0,j+p-i);
    }
  }

  BArray<BDat> param, res;
  BMatrix<BDat> Li, L;
  BMatrix<BDat> cov, cor;
  BDat sigma;
  BDat R2;
  BDat F;
  BInt v1;
  BInt v2;
  BDat alfa;
  BDat resSqSum;
  BDat totSqSum;


  sigma = LinearRegression(arrY,
		   X.T(),
		   param,
		   res,
		   Li,
		   L,
		   cov,
		   cor,
		   R2,
		   F,
		   v1,
		   v2,
		   alfa,
		   resSqSum,
		   totSqSum);
  if(sigma.IsUnknown()) { return; }

  BPolyn<BDat> pol = BPol::One();

  BDat sw = 2*Log(sigma)+n*Log(N)/N;

  BList* lst	= NIL;
  BList* aux	= NIL;
  for(i=0; i<n; i++)
  {
    pol -= BMonome<BDat>(param[i], i+1);
    BList* lsta	   = NIL;
    BList* auxa	   = NIL;
    BDat   stDs	   = Sqrt(cov.Get(i,i));
    BDat   student = param[i]/stDs;
    BDat   prob	   = 2*(1-T.Dist(Abs(student)));
    BText  id	   = BText("ARI ")+(i+1);
    LstFastAppend(lsta,auxa, BContensText::New(id));
    LstFastAppend(lsta,auxa, BContensDat::New(1));
    LstFastAppend(lsta,auxa, BContensDat::New((i+1)));
    LstFastAppend(lsta,auxa, BContensDat::New(param[i]));
    LstFastAppend(lsta,auxa, BContensDat::New(stDs));
    LstFastAppend(lsta,auxa, BContensDat::New(student));
    LstFastAppend(lsta,auxa, BContensDat::New(prob));

    BUserSet* s = NewSet("",id,lsta,strParam,BSet::Structured);
    LstFastAppend(lst,aux, s);
  }

  BUserSet* par = NewSetFND(ParInf,lst,NIL,BSet::Table);

  lst = NIL;
  aux = NIL;
  LstFastAppend(lst,aux,NewFND(BDate, FirstDate,  f));
  LstFastAppend(lst,aux,NewFND(BDate, LastDate,   l));
  LstFastAppend(lst,aux,NewFND(BDat,  DataNumber, N));
  LstFastAppend(lst,aux,NewFND(BDat,  VarNumber,  n));
  LstFastAppend(lst,aux,NewFND(BDat,  Sigma,      sigma));
  LstFastAppend(lst,aux,NewFND(BDat,  Swartz,     sw));
  LstFastAppend(lst,aux,NewFND(BDat,  RSS,        resSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  TSS,        totSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  R2,         R2));
  LstFastAppend(lst,aux,NewFND(BDat,  F,          F));
  LstFastAppend(lst,aux,NewFND(BDat,  Alfa,       alfa));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v1));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v2));

  BUserSet* inf = NewSetFND(Information,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;
  LstFastAppend(lst,aux,NewFND(BMat, COVLi, Li));
  LstFastAppend(lst,aux,NewFND(BMat, COVL,  L));
  LstFastAppend(lst,aux,NewFND(BMat, COV,   cov));
  LstFastAppend(lst,aux,NewFND(BMat, COR,   cor));

  BUserSet* covInf = NewSetFND(CovInf,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;

  LstFastAppend(lst,aux,new BTsrPrimary("",resName,tms,f,l,res));
  LstFastAppend(lst,aux,par);
  LstFastAppend(lst,aux,inf);
  LstFastAppend(lst,aux,covInf);
  LstFastAppend(lst,aux,BContensPol::New("",pol, "ARI polyn"));

  contens_.RobElement(lst);
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLinReg);
DefExtOpr(1, BSetLinReg, "LinReg", 2, 2, "Matrix Matrix",
  "(Matrix y, Matrix X)",
  I2("Estimates the linear regression model yt = Xt*A + et.",
     "Estima el modelo de regresión lineal yt = Xt*A + et."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetLinReg::CalcContens()
//--------------------------------------------------------------------
{
  BStruct* strParam = FindStruct("@LinRegParamInf");
  if(!strParam)
  {
    Error(I2("@LinRegParamInf structure is not implementeded.",
	     "La estructura @LinRegParamInf no está implementa."));
    return;
  }
  BMat y = Mat(Arg(1));
  BMat X = Mat(Arg(2));
  BInt i, N=y.Rows(), n=X.Columns();

  BArray<BDat> param, res;
  BMatrix<BDat> Li, L;
  BMatrix<BDat> cov, cor;
  BDat sigma;
  BDat R2;
  BDat F;
  BInt v1;
  BInt v2;
  BDat alfa;
  BDat resSqSum;
  BDat totSqSum;
  BTStudentDist T(N-n);

  sigma = LinearRegression(y.Data(),
		   X,
		   param,
		   res,
		   Li,
		   L,
		   cov,
		   cor,
		   R2,
		   F,
		   v1,
		   v2,
		   alfa,
		   resSqSum,
		   totSqSum);
  if(sigma.IsUnknown()) { return; }

  BList* lst	= NIL;
  BList* aux	= NIL;
  for(i=0; i<param.Size(); i++)
  {
    BList* lsta	   = NIL;
    BList* auxa	   = NIL;
    BDat   stDs	   = Sqrt(cov.Get(i,i));
    BDat   student = param[i]/stDs;
    BDat   prob	   = 2*(1-T.Dist(Abs(student)));
    BText  name	   = BText("Var ")+(i+1);
    LstFastAppend(lsta,auxa, BContensText::New(name));
    LstFastAppend(lsta,auxa, BContensDat::New(param[i]));
    LstFastAppend(lsta,auxa, BContensDat::New(stDs));
    LstFastAppend(lsta,auxa, BContensDat::New(student));
    LstFastAppend(lsta,auxa, BContensDat::New(prob));

    BUserSet* s = NewSet("",name,lsta,strParam,BSet::Structured);
    LstFastAppend(lst,aux, s);
  }
  BMatrix<BDat> residuals(res.Size(),1,res.Buffer());

  BUserSet* par = NewSetFND(ParInf,lst,NIL,BSet::Table);
  lst = NIL;
  aux = NIL;

  BDat sw = 2*Log(sigma)+n*Log(N)/N;
  LstFastAppend(lst,aux,NewFND(BDat,  DataNumber, N));
  LstFastAppend(lst,aux,NewFND(BDat,  VarNumber,  n));
  LstFastAppend(lst,aux,NewFND(BDat,  Sigma,      sigma));
  LstFastAppend(lst,aux,NewFND(BDat,  Swartz,     sw));
  LstFastAppend(lst,aux,NewFND(BDat,  RSS,        resSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  TSS,        totSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  R2,         R2));
  LstFastAppend(lst,aux,NewFND(BDat,  F,          F));
  LstFastAppend(lst,aux,NewFND(BDat,  Alfa,       alfa));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v1));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v2));

  BUserSet* inf = NewSetFND(Information,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;
  LstFastAppend(lst,aux,NewFND(BMat, COVLi, Li));
  LstFastAppend(lst,aux,NewFND(BMat, COVL,  L));
  LstFastAppend(lst,aux,NewFND(BMat, COV,   cov));
  LstFastAppend(lst,aux,NewFND(BMat, COR,   cor));

  BUserSet* covInf = NewSetFND(CovInf,lst,NIL,BSet::Generic);
  lst = NIL;
  aux = NIL;


  LstFastAppend(lst,aux,BContensMat::New("",residuals, "Residuals"));
  LstFastAppend(lst,aux,par);
  LstFastAppend(lst,aux,inf);
  LstFastAppend(lst,aux,covInf);

  contens_.RobElement(lst);
}



//--------------------------------------------------------------------
void BooleanModelEstimation(const BMat& y, 
			    const BMat& X, 
			    BProbDist* dist,
			    BSet& result)
//--------------------------------------------------------------------
{
  BStruct* strParam = FindStruct("@LinRegParamInf");
  if(!strParam)
  {
    Error(I2("@LinRegParamInf structure is not implementeded.",
	     "La estructura @LinRegParamInf no está implementa."));
    return;
  }
  BInt i,j,N=y.Rows(),n=X.Columns();

  BArray<BDat> param, res, yPrev, prob;
  BSymMatrix<BDat> cov;
  BDat sigma;
  BDat R2;
  BDat F;
  BInt v1;
  BInt v2;
  BDat alfa;
  BDat resSqSum;
  BDat totSqSum;
  BTStudentDist T(N-n);

  BooleanModel(y.Data(),
	       X,
	       param,
	       res,
	       cov,
	       prob,
	       yPrev,
	       sigma,
	       R2,
	       F,
	       v1,
	       v2,
	       alfa,
	       resSqSum,
	       totSqSum,
	       BDat::Tolerance(),
	       dist);

  BList* lst	= NIL;
  BList* aux	= NIL;
  for(i=0; i<param.Size(); i++)
  {
    BList* lsta	   = NIL;
    BList* auxa	   = NIL;
    BDat   stDs	   = Sqrt(cov.Get(i,i));
    BDat   student = param[i]/stDs;
    BDat   prob	   = 2*(1-T.Dist(Abs(student)));
    BText  name	   = BText("Var ")+(i+1);
    LstFastAppend(lsta,auxa, BContensText::New(name));
    LstFastAppend(lsta,auxa, BContensDat::New(param[i]));
    LstFastAppend(lsta,auxa, BContensDat::New(stDs));
    LstFastAppend(lsta,auxa, BContensDat::New(student));
    LstFastAppend(lsta,auxa, BContensDat::New(prob));

    BUserSet* s = NewSet("",name,lsta,strParam,BSet::Structured);
    LstFastAppend(lst,aux, s);
  }
  BMatrix<BDat> probability(prob.Size(),1,prob.Buffer());
  BMatrix<BDat> yPrevision(yPrev.Size(),1,yPrev.Buffer());
  BMatrix<BDat> residuals(res.Size(),1,res.Buffer());
  BMatrix<BDat> cor = cov;
  for(i=0; i<param.Size(); i++)
  {
    for(j=0; j<param.Size(); j++)
    {
      cor(i,j) /= Sqrt(cov(i,i)*cov(j,j));
    }
  }
  BDat prop = 0;
  for(i=0; i<y.Rows(); i++)
  {
    if(yPrevision(i,0)!=y(i,0)) { prop += 1; }
  }
  prop /= y.Rows();

  BUserSet* par = NewSetFND(ParInf,lst,NIL,BSet::Table);
  lst = NIL;
  aux = NIL;

  BDat sw = 2*Log(sigma)+n*Log(N)/N;
  LstFastAppend(lst,aux,NewFND(BDat,  DataNumber, N));
  LstFastAppend(lst,aux,NewFND(BDat,  VarNumber,  n));
  LstFastAppend(lst,aux,NewFND(BDat,  Sigma,      sigma));
  LstFastAppend(lst,aux,NewFND(BDat,  Swartz,     sw));
  LstFastAppend(lst,aux,NewFND(BDat,  RSS,        resSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  TSS,        totSqSum));
  LstFastAppend(lst,aux,NewFND(BDat,  R2,         R2));
  LstFastAppend(lst,aux,NewFND(BDat,  F,          F));
  LstFastAppend(lst,aux,NewFND(BDat,  Alfa,       alfa));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v1));
  LstFastAppend(lst,aux,NewFND(BDat,  v1,         v2));

  BUserSet* inf = NewSetFND(Information,lst,NIL,BSet::Generic);

  lst = NIL;
  aux = NIL;
  LstFastAppend(lst,aux,NewFND(BMat, COV,   cov));
  LstFastAppend(lst,aux,NewFND(BMat, COR,   cor));

  BUserSet* covInf = NewSetFND(CovInf,lst,NIL,BSet::Generic);
  lst = NIL;
  aux = NIL;

  LstFastAppend(lst,aux,BContensMat::New("",residuals,   "Residuals"));
  LstFastAppend(lst,aux,BContensMat::New("",probability, "Probability"));
  LstFastAppend(lst,aux,BContensMat::New("",yPrevision,  "Prevision"));
  LstFastAppend(lst,aux,par);
  LstFastAppend(lst,aux,inf);
  LstFastAppend(lst,aux,covInf);

  result.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLogit);
DefExtOpr(1, BSetLogit, "Logit", 2, 2, "Matrix Matrix",
  "(Matrix y, Matrix X)",
  I2("Estimates the logistic model Log(p/(1-p)) = B'*X"
     "Returns a set with these elements:"
     "  1. The vector of parameters"
     "  2. The vector of output probabilitys"
     "  3. The vector of gradient"
     "  4. The matrix of hessian"
     "  5. The vector of logarithm of individuals likelihoods"
     "  6. Logarithm of likelihood"
     "  7. Likelihood",
     "Estima el modelo logístico Log(p/(1-p)) = B'*X."
     "Devuelve un conjunto con estos elementos:"
     "  1. El vector de los  parameters"
     "  2. El vector de las probabilidades de salida"
     "  3. El vector del gradiente"
     "  4. La matrix del hessiano"
     "  5. El vector de los logaritmos de las verosimilitudes individuales"
     "  6. El logaritmo de la verosimilitud"
     "  7. La verosimilitud"),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetLogit::CalcContens()
//--------------------------------------------------------------------
{
  BMat y = Mat(Arg(1));
  BMat X = Mat(Arg(2));
  BMatrix    <BDat> B;
  BMatrix    <BDat> e;
  BMatrix    <BDat> p;
  BMatrix    <BDat> G;
  BSymMatrix <BDat> H;
  BMatrix    <BDat> lnL;
  BDat		    lnLikelyhood;
  BDat		    likelyhood;

  Logit(y,X,B,e,p,G,H,lnL,lnLikelyhood,likelyhood);

  BList* lst=NIL;
  BList* aux=NIL;
  LstFastAppend(lst,aux,BContensMat::New("", B,   "Parameter data"));
  LstFastAppend(lst,aux,BContensMat::New("", e,   "Residuals"));
  LstFastAppend(lst,aux,BContensMat::New("", p,   "Output Probabilitys"));
  LstFastAppend(lst,aux,BContensMat::New("", G,   "Gradient"));
  LstFastAppend(lst,aux,BContensMat::New("", H,   "Hessian"));
  LstFastAppend(lst,aux,BContensMat::New("", lnL, "Log of individuals likelihoods"));
  LstFastAppend(lst,aux,BContensDat::New("", lnLikelyhood, "Log of likelihood"));
  LstFastAppend(lst,aux,BContensDat::New("", likelyhood, "Log of likelihood"));
  contens_.RobElement(lst);

}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetProbit);
DefExtOpr(1, BSetProbit, "Probit", 2, 2, "Matrix Matrix",
  "(Matrix y, Matrix X)",
  I2("Estimates the probit model p = DistNormal(B'*X)",
     "Estima el modelo probit p = DistNormal(B'*X)."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetProbit::CalcContens()
//--------------------------------------------------------------------
{
  BMat y = Mat(Arg(1));
  BMat X = Mat(Arg(2));
  BMatrix    <BDat> B;
  BMatrix    <BDat> e;
  BMatrix    <BDat> p;
  BMatrix    <BDat> G;
  BSymMatrix <BDat> H;
  BMatrix    <BDat> lnL;
  BDat		    lnLikelyhood;
  BDat		    likelyhood;

  Probit(y,X,B,e,p,G,H,lnL,lnLikelyhood,likelyhood);

  BList* lst=NIL;
  BList* aux=NIL;
  LstFastAppend(lst,aux,BContensMat::New("", B,   "Parameter data"));
  LstFastAppend(lst,aux,BContensMat::New("", e,   "Residuals"));
  LstFastAppend(lst,aux,BContensMat::New("", p,   "Output Probabilitys"));
  LstFastAppend(lst,aux,BContensMat::New("", G,   "Gradient"));
  LstFastAppend(lst,aux,BContensMat::New("", H,   "Hessian"));
  LstFastAppend(lst,aux,BContensMat::New("", lnL, "Log of individuals likelihoods"));
  LstFastAppend(lst,aux,BContensDat::New("", lnLikelyhood, "Log of likelihood"));
  LstFastAppend(lst,aux,BContensDat::New("", likelyhood, "Log of likelihood"));
  contens_.RobElement(lst);
}

/*
//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolAutoReg);
DefExtOpr(1, BPolAutoReg, "AutoReg", 1, 4, "Serie Real Real Real",
  I2("(Serie z [, Real period=1, Real minOrder=1, Real maxOrder=-1])",
     "(Serie z [, Real periodo=1, Real minOrden=1, Real maxOrden=-1])"),
  I2("Identifies and estimates the auto-regressive model P(B):z = a.",
     "Identifica y estima el modelo autoregresivo lineal P(B):z = a."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BPolAutoReg::CalcContens()
//--------------------------------------------------------------------
{
  BInt		  period = 1;
  BInt		  minOrd = 1;
  BInt		  maxOrd = -1;
  BUserTimeSerie* z	 = Tsr(Arg(1));
  BSerieTable	zTable; zTable.AddSerie(z);

  if(Arg(2)) { period = (BInt)Real(Arg(2)); }
  if(Arg(3)) { minOrd = (BInt)Real(Arg(3)); }
  if(Arg(4)) { maxOrd = (BInt)Real(Arg(4)); }

  contens_ = IdentifyAutoReg(zTable.Data().Data(),period,minOrd,maxOrd);
}

/*
//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetAutoRegInt);
DefExtOpr(1, BSetAutoRegInt, "AutoRegInt", 1, 2, "Serie Real",
  I2("(Serie z [, Real period=1])",
     "(Serie z [, Real periodo=1])"),
  I2("Identifies and estimates the Auto-Regressive-Integrated model P(B):z = a.",
     "Identifica y estima el modelo autoregresivo integrado P(B):z = a."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetAutoRegInt::CalcContens()
//--------------------------------------------------------------------
{
  BInt		  period = 1;
  BUserTimeSerie* z	 = Tsr(Arg(1));
  BSerieTable	zTable; zTable.AddSerie(z);

  if(Arg(2)) { period = (BInt)Real(Arg(2)); }
  BPol dif, ar;
  IdentifyAutoRegIntegrated(zTable.Data().Data(),dif,ar,period);
  contens_.RobElement
  (
     Cons(BContensPol::New("", dif, BText("DIF(")+z->Identify()+")"),
    NCons(BContensPol::New("", ar,	BText("AR(") +z->Identify()+")")))
  );
}
*/


#if !defined(USE_DELAY_INIT)

//--------------------------------------------------------------------
  static BStruct* BoxCoxStruct_ = NewStructSymbol
//--------------------------------------------------------------------
(
  "@BoxCoxStruct"
  ,
  "Real:Exponent,"
  "Real:Constant"
);

#else

static void * cloneBoxCoxStruct_()
{
  return NewStructSymbol( "@BoxCoxStruct"
			  ,
			  "Real:Exponent,"
			  "Real:Constant" );
}

static BStruct* BoxCoxStruct_ = (BStruct*)(__delay_init((void**)(&BoxCoxStruct_),
							&cloneBoxCoxStruct_));

#endif

// "  [[-1,-1.0/2.0,-1.0/3.0,-1.0/4.0,0,1.0/4.0,1.0/3.0,1.0/2.0,1]], "



//--------------------------------------------------------------------
DeclareContensClass(BDat, BSetTemporary, BSetBoxCoxTrans);
DefExtOpr(1, BSetBoxCoxTrans, "BoxCoxTrans", 1, 3, "Serie Set Real",
  I2("(Serie ser [, Set options={-1;-1/2;-1/3;-1/4;0;1/4;1/3;1/2;1}, "
     "	Real intervalLength=10	])",
     "(Serie ser [, Set opciones={-1;-1/2;-1/3;-1/4;0;1/4;1/3;1/2;1}, "
     "	Real longitudIntervalos=10 ])"),
  I2("Returns the Box-Cox exponential transformation of a time series "
     "by homocedastic analysis. If the series has seasonal periodicity it "
     "must be given as intervalLength parameter, since of other manner is "
     "assumed 10. In the options parameter can be included any set of real "
     "numbers, between those which will be sought the optimum for the "
     "transformation of Box-Cox."
     "\nNOTE: The transformation of Box-Cox width exponent b is defined as :"
     "\n  T(x;b) = x^b (si b<>0);"
     "\n  T(x;0) = Log(x);",
     "Devuelve el exponente de la transformación Box-Cox de una serie "
     "calculado mediante el análisis de su homocedasticidad. Si la serie "
     "tiene periodicidad estacional debe pasarse su valor en el parámetro "
     "longitudIntervalos, pues de otro modo se asume 10. En el parámetro de "
     "opciones se puede incluir cualquier conjunto de números reales entre "
     "los cuales se buscará el óptimo para la transformación de Box-Cox."
     "\nNOTA: La transformación de Box-Cox de exponente b se define como :"
     "\n  T(x;b) = x^b (si b<>0);"
     "\n  T(x;0) = Log(x);"),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetBoxCoxTrans::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSerie* z	 = Tsr(Arg(1));
  if(!z->IsStochastic())
  {
    Error(I2
    (
     "Cannot apply BoxCoxTrans to an infinite time series.",
     "No se puede aplicar BoxCoxTrans a una serie infinita"
    ));
    return;
  }
  BSerieTable	zTable; 
  zTable.AddSerie(z);
  zTable.Fill(z->FirstDate(), z->LastDate());
  BInt period = 10;
  if(Arg(3)) { period = (BInt)Real(Arg(3)); }
  BDat exponent;
  BDat addition;

  if(period<=1)
  {
    period = 10;
    Warning(I2
    (
     "Invalid intervalLength parameter in BoxCoxTrans function. "
     "It is assumed the defect value 10.",
     "Parámetro longitudIntervalos no válido en función BoxCoxTrans. "
     "Se asume el valor por defecto 10"
    ));
  }
  if(zTable.NumDates()<=3*period)
  {
    Error(I2
    (
     "Cannot apply BoxCoxTrans to a time series with length ",
     "No se puede aplicar BoxCoxTrans a una serie temporal de longitud "
    )+
    zTable.NumDates()+" <= 3*intervalLength = "+(3*period));
  }
  if(Arg(2))
  {
    BSet& opt = Set(Arg(2));
    BArray<BDat> options(opt.Card());
    BInt m=0;
    BInt n=1;
    for(; n<=opt.Card(); n++)
    {
      if(opt[n]->Grammar()->Name()=="Real")
      { options[m++] = Dat(opt[n]); }
    }
    options.ReallocBuffer(m);
    if(m==0)
    {
      Warning(I2
      (
       "Empty options parameter in BoxCoxTrans function. "
       "It is assumed the defect value :",
       "Parámetro de opciones vacío en función BoxCoxTrans. "
       "Se asume el valor por defecto :"
      ) +
      "\n  [[-1,-1.0/2.0,-1.0/3.0,-1.0/4.0,0,1.0/4.0,1.0/3.0,1.0/2.0,1]] ])");
    }
    BoxCoxTrans(exponent,addition,zTable.Data().Data(), period, options);
  }
  else
  {
    BoxCoxTrans(exponent,addition,zTable.Data().Data(), period);
  }
  BUserDat* exp = BContensDat::New("",exponent,I2("Exponent","Exponente"));
  BUserDat* add = BContensDat::New("",addition,I2("Addition","Adición"  ));
  BList* lst = Cons(exp,NCons(add));
  contens_.RobStruct(lst,BoxCoxStruct_,BSet::Structured);
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BMatLinLeastSqr);
DefExtOpr(1, BMatLinLeastSqr, "LinearLeastSquares", 2, 4,
  "Matrix Matrix Real Set",
  I2("(Matrix A, Matrix Y [, Real onlySolve=BFALSE, Set names=Empty] )",
     "(Matrix A, Matrix Y [, Real soloResolver=BFALSE, Set nombres=Empty] )"),
  I2("Given any rectangular matrix A and a column vector Y performs "
     "the least squares solution of \n"
     " \n"
     "			 Y = A*X + R \n"
     " \n"
     "and returns the euclidean norm of the residuals R \n"
     " \n"
     "If the parameter onlySolve is false then the function already returns\n"
     " \n"
     "	 i)  the pseudo-inverse of A into P \n"
     " \n"
     "			A*P*A = A; \n"
     "			P*A*P = P; \n"
     " \n"
     "	ii)  the information matrix of A and Y \n"
     " \n"
     "			t(A)*A \n"
     "		       -------- \n"
     "			t(Y)*Y \n"
     " \n"
     " iii)  the covarianze matrix of parameters X that is the inverse "
     "	     of the information matrix \n"
     " \n"
     " \n"
     "	iv)  a base of the kernel of A, \n"
     " \n"
     "		  K(A) = { k | A*k = 0 } \n"
     " \n"
     " \n"
     ,
     "Dada una matriz rectangular A con al menos tantas filas como columnas "
     "y un vector columna resuelve el problema de mínimos cuadrados \n"
     " \n"
     "			 Y = A*X + R \n"
     " \n"
     "y devuelve la media de los cuadrados de los residuos R \n"
     " \n"
     "Si el parámetro soloResolver es false entonces se devuelve también \n"
     " \n"
     "	 i)  la pseudo-inversa de A como la matriz P que cumple \n"
     " \n"
     "			A*P*A = A; \n"
     "			P*A*P = P; \n"
     " \n"
     "	ii)  la matriz de información de A e Y \n"
     " \n"
     "			t(A)*A	\n"
     "		       -------- \n"
     "			t(Y)*Y	\n"
     " \n"
     " iii)  la matriz de covarianzas de los parámetros X, que es la \n"
     "	     inversa de la matriz de información.\n"
     " \n"
     "	iv)  una base del núcleo o kernel de A \n"
     " \n"
     "		  K(A) = { k | A*k = 0 } \n"
     " \n"
     ),
	  BOperClassify::MatrixAlgebra_);
//--------------------------------------------------------------------
void BMatLinLeastSqr::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat> A = Mat(Arg(1));
  BMatrix<BDat> Y = Mat(Arg(2));

  BBool		onlySolve = BFALSE;

  BInt i;
  BInt n = A.Columns();
  BInt N = A.Rows();

  if(N<n)	     { return; }
  if(Y.Columns()!=1) { return; }
  if(Y.Rows   ()!=N) { return; }


  if(Arg(3)) { onlySolve = (BBool)Real(Arg(3)); }

  BArray <BText> name	 (n);
  BArray <BBool> varElim (n);
  for(i=0; i<n; i++)
  {
    name[i] = BText("V[")+(i+1)+"]";
  }

  if(Arg(4))
  {
    BSet nam = Set(Arg(4));

    for(i=0; i<n; i++)
    {
      if(nam[i+1] && (nam[i+1]->Grammar()==GraText()))
      {
	name[i] = Text(nam[i+1]);
      }
//    Std(BText("\nName[")+i+"]='"+name[i]+"'");
    }
  }


  BMatrix<BDat> P, X, R, Ker;
  BSymMatrix<BDat> MI, Cov, Cor;

  BDat S = LinearLeastSquareSolve(A,Y,name,X,R,P,MI,Cov,Ker,varElim,onlySolve);


  BList* result=NIL;
  BList* aux=NIL;

  BUserMat* UX = BContensMat::New("X",X,I2("Parameters of the model","Parámetros del modelo"));
  LstFastAppend(result, aux, UX);

  BUserMat* UR = BContensMat::New("R",R,I2("Residuals of the model","Residuos del modelo"));
  LstFastAppend(result, aux, UR);

  BUserDat* US = BContensDat::New("S",S,I2("Standar error of the residuals","Error estandar de los residuos"));
  LstFastAppend(result, aux, US);

  if(!onlySolve)
  {
    BInt k = Ker.Columns(), m=n-k;
/*  Std(BText("\nn=")+n);
    Std(BText("\nk=")+k);
    Std(BText("\nm=")+m);
    Std(BText("\nN=")+N);
*/  BSymMatrix<BDat> Cor;
    BDiagMatrix<BDat> Var;
    SymmDiagNormalDecomp(Cov,Cor,Var);
    BDiagMatrix<BDat> StDs  = Var^(0.5);
    BDiagMatrix<BDat> StDsi = StDs.I();
    BMatrix<BDat> T;
    T = StDsi*X;

    //	TRZ(Cor); TRZ(Cov); TRZ(Var); TRZ(StDs); TRZ(StDsi); TRZ(X); TRZ(T);
    BMatrix<BDat> prob(n,0);
    prob.SetAllValuesTo(BDat::Unknown());

    if(N>m)
    {
      BProbDist* t = NIL;
      if(N-m<100) { t = new BTStudentDist(N-m); }
      else	  { t = new BNormalDist	 (0,1); }
      prob.Alloc(n,1);
      for(int i=0; i<n; i++) { prob(i,0) = 2*(1-t->Dist(Abs(T(i,0)))); }
    }
//  TRZ(prob);

    BUserMat* UT = BContensMat::New("T",T,I2("Value of the t-student of parameters X",
			  "Valor de las t-student de los parámetros X"));
    LstFastAppend(result, aux, UT);

    BUserMat* UProb = BContensMat::New("RefuseProb",prob,I2("Refuse probability of parameters X",
			  "Probabilidad de rechazo de los parámetros X"));
    LstFastAppend(result, aux, UProb);

    BUserMat* UP = BContensMat::New("P",P,I2("Pseudo-inverse of data matrix",
			  "Pseudo inversa de la matriz de datos"));
    LstFastAppend(result, aux, UP);

    BUserMat* UMI = BContensMat::New("MI",MI,I2("Information matrix of the model",
			  "Matriz de información del modelo"));
    LstFastAppend(result, aux, UMI);

    BUserMat* UCov = BContensMat::New("Cov",Cov,I2("Covarianze matrix of the parameters X",
			    "Matriz de covarianzas de los parámetros X"));
    LstFastAppend(result, aux, UCov);

    BUserMat* UCor = BContensMat::New("Cor",Cor,I2("Correlation matrix of the parameters X",
			    "Matriz de correlaciones de los parámetros X"));
    LstFastAppend(result, aux, UCor);

    BUserMat* UK = BContensMat::New("Ker",Ker,I2("Kernel base matrix of A",
			  "Matrix base del núcleo de A"));
    LstFastAppend(result, aux, UK);

    BSet elim;
    BList* res2 = NIL;
    BList* aux2 = NIL;
    for(i=0; i<n; i++)
    {
      if(varElim[i])
      {
	LstFastAppend(res2, aux2, BContensDat::New(BDat(i+1)));
      }
    }
    elim.RobElement(res2);
    BUserSet* UE = BContensSet::New("Elim",elim,I2("Eliminable variables",
			  "Variables eliminables"));
    LstFastAppend(result, aux, UE);

  }
  contens_.RobElement(result);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatARMATACov);
DefExtOpr(1, BMatARMATACov, "ARMATACov", 3, 3, 
  "Polyn Polyn Real",
  "(Polyn ar, Polyn ma, Real n)",
  I2("Calculates the theorical autocovarianze function of an ARMA model "
     "until order n.",
     "Calcula la función de autocovarianzas teórica de un modelo ARMA "
     "hasta el orden n."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BMatARMATACov::CalcContens()
//--------------------------------------------------------------------
{
  BPol& ar = Pol(Arg(1));
  BPol& ma = Pol(Arg(2));
  BInt  n  = (BInt)Real(Arg(3));
  BArray<BDat> gn;
  ARMAAutoCovarianzeVector(gn,ar,ma,n,1);
  contens_ = BMatrix<BDat>(n,1,gn.Buffer());
}

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolARMATACov);
DefExtOpr(1, BPolARMATACov, "ARMATACov", 3, 3, 
  "Polyn Polyn Real",
  "(Polyn ar, Polyn ma, Real n)",
  I2("Calculates the theorical autocovarianze function of an ARMA model "
     "until order n and return it as the symmetric polynomial gamma(B+F).",
     "Calcula la función de autocovarianzas teórica de un modelo ARMA "
     "hasta el orden n y la devuelve en forma de polinomio simétrico "
     "gamma(B+F)."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BPolARMATACov::CalcContens()
//--------------------------------------------------------------------
{
  BPol& ar = Pol(Arg(1));
  BPol& ma = Pol(Arg(2));
  BInt  n  = (BInt)Real(Arg(3));
  contens_ = ARMAACovGamma(ar,ma,n);
}

static BDat _getACF(const BMat& rho, int k)
{
  int s = rho.Data().Size();
  if(k== 0)  { return(1.0); }
  if(k>= s)  { return(0.0); }
  if(k<=-s)  { return(0.0); }
  if(k<  0)  { return(rho.Data()[-k-1]); }
/* if(k>  0)*/ return(rho.Data()[+k-1]); 
};

//--------------------------------------------------------------------
static BDat _getACF_BartlettFactor(const BMat& rho, int k, int i)
//--------------------------------------------------------------------
{
  return(_getACF(rho,k+i)+_getACF(rho,k-i)-2*_getACF(rho,i)+_getACF(rho,k));
};

//--------------------------------------------------------------------
static BDat _getACF_Bartlett(const BMat& rho, int k, int i, int j)
//--------------------------------------------------------------------
{
  return(_getACF_BartlettFactor(rho,k,i)*_getACF_BartlettFactor(rho,k,j));
};

//--------------------------------------------------------------------
static void _covACF_Bartlett(BMat& cov, const BMat& rho, int n, int T, int s_)
//--------------------------------------------------------------------
{
  cov.Alloc(n,n);
  int i,j,k,s=s_;
  BArray<BDat> bf(n);
  for(s=s_; s>=n+1; s--)
  {
    BDat r = _getACF(rho,s);
    if(Abs(r)>1.E-6) { break; }
  }
  for(i=0; i<n; i++)
  {
    for(j=0; j<=i; j++)
    {
      cov(i,j)=0;
    }
  }
  for(k=1; k<=s; k++)
  {
    for(i=0; i<n; i++)
    {
      bf(i) = _getACF_BartlettFactor(rho,k,i+1);
    //Std(BText("\nTRACE _covACF_Bartlett bf(")+i+")="+bf(i))
    }
    for(i=0; i<n; i++)
    {
      for(j=0; j<=i; j++)
      {
        cov(i,j) += bf(i)*bf(j);
      }
    }
  }
  for(i=0; i<n; i++)
  {
    for(j=0; j<=i; j++)
    {
      cov(i,j) /= T;
      if(i>j) { cov(j,i)  = cov(i,j); }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatARMAACFBartlettCov);
DefExtOpr(1, BMatARMAACFBartlettCov, "ARMA.ACF.Bartlett.Cov", 3, 3, 
  "Matrix Real Real",
  "(Matrix rho, Real n, Real T)",
  I2("Calculates the covariance of asymptotic distribution of sample "
     "autocorrelation of orders from 1 to n of an ARMA noise of size T, "
     "given the theorical ACF rho, that should be calculated until it "
     "decays to zero.",
     "Calcula la matriz de covarianzas de la distribución aintótica "
     "de las autocorrelaciones muestrales de órdenes 1 a n de un ruido "
     "ARMA de tamañlo T, dada la ACF teórica rho, la cual debería ser "
     "calculada hasta que decaiga a cero."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BMatARMAACFBartlettCov::CalcContens()
//--------------------------------------------------------------------
{
  BMat& rho = Mat(Arg(1));
  int n = (int)Real(Arg(2));
  int T = (int)Real(Arg(3));
  int s = rho.Data().Size();
  _covACF_Bartlett(contens_,rho,n,T,s);
}

//--------------------------------------------------------------------
static BDat ARMAACFBartlettLLH_Theorical(
  const BPol& ar,
  const BPol& ma,
  const BMat& acf,
  int T,
  int s)
//--------------------------------------------------------------------
{
  static double _05log2pi = 0.5*log(2*3.14159265358979323846);
  int n = acf.Rows();
  int res;
  BMat rho, rho_, dif, cov, L, eps;
  BDat alpha = 1.0;
  BArray<BDat> gn;
  int i;
  
  ARMAAutoCovarianzeVector(gn,ar,ma,s+1,1);
  rho  = BMatrix<BDat>(s,1,gn.Buffer()+1) / gn[0];
  rho_ = BMatrix<BDat>(n,1,gn.Buffer()+1) / gn[0];
  dif = acf-rho_;
  _covACF_Bartlett(cov,rho,n,T,s);

  res = TolLapack::dpotrf(CblasLower,cov,L);
  if(res) { return(BDat::NegInf()); }
  res = TolBlas::dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,alpha,
                           L,dif,eps);
  if(res) { return(BDat::NegInf()); }
  const BDat* eps_i = eps.Data().Buffer();
  BDat eps_sumSqr = 0;
  BDat log_det = 0;
  for(i=0; i<n; i++, eps_i++)
  {
    log_det += Log(L(i,i));
    eps_sumSqr += *eps_i * *eps_i;
  }
  return(- n*_05log2pi - 0.5*eps_sumSqr - log_det);
}

//--------------------------------------------------------------------
static BDat ARMAACFBartlettLLH_Commuted(
  const BPol& ar,
  const BPol& ma,
  const BMat& acf,
  int T,
  const BMat& L)
//--------------------------------------------------------------------
{
  static double _05log2pi = 0.5*log(2*3.14159265358979323846);
  int n = acf.Rows();
  int res;
  BMat rho, dif, eps;
  BDat alpha = 1.0;
  BArray<BDat> gn;
  int i;
  
  ARMAAutoCovarianzeVector(gn,ar,ma,n+1,1);
  rho = BMatrix<BDat>(n,1,gn.Buffer()+1) / gn[0];
  dif = acf-rho;

  res = TolBlas::dtrsm(CblasLeft,CblasLower,CblasNoTrans,CblasNonUnit,alpha,
                           L,dif,eps);

  const BDat* eps_i = eps.Data().Buffer();
  BDat eps_sumSqr = 0;
  BDat log_det_L = 0;
  for(i=0; i<n; i++, eps_i++)
  {
    log_det_L += Log(L(i,i));
    eps_sumSqr += *eps_i * *eps_i;
  }
  return(- n*_05log2pi - 0.5*eps_sumSqr - log_det_L);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatARMAACFBartlettLLH);
DefExtOpr(1, BDatARMAACFBartlettLLH, "ARMA.ACF.Bartlett.LLH", 4, 5, 
  "Polyn Polyn Matrix Real Real",
  "(Polyn ar, Polyn ma, Matrix acf, Real T [, Real truncating])",
  I2("Calculates the log-likelihood of asymptotic distribution of sample "
     "autocorrelation acf of an ARMA noise z of size T and equations "
     "ar(B)*z= ma(B)*e",
     "Calcula la log-verosimilitud de la distribución aintótica "
     "de las autocorrelaciones muestrales de un ruido "
     "ARMA z de tamaño T y ecuaciones  "
     "ar(B)*z= ma(B)*e"),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BDatARMAACFBartlettLLH::CalcContens()
//--------------------------------------------------------------------
{
  BPol& ar = Pol(Arg(1));
  BPol& ma = Pol(Arg(2));
  BMat& acf = Mat(Arg(3));
  int T = (int)Real(Arg(4));
  int s = T*4;
  if(Arg(5)) { s = (int)Real(Arg(5)); }
  int n = acf.Data().Size();
  contens_ = ARMAACFBartlettLLH_Theorical(ar,ma,acf,T,s);
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatARMAACFBartlettLLHRandStationary);
DefExtOpr(1, BMatARMAACFBartlettLLHRandStationary, 
  "ARMA.ACF.Bartlett.LLH.RandStationary", 6, 7, 
  "Real Real Matrix Real Real Real Real",
  "(Real p, Real q, Matrix acf, Real T, Real N, Real exact [, Real truncating=N*4])",
  I2("Draws N times the log-likelihood of asymptotic distribution of sample "
     "autocorrelation of orders from 1 to n of an ARMA noise of size T."
     "Generates stationary polynomials AR and MA of degrees p and q "
     "with uniform distributed modules of roots.",
     "Simula N veces la log-verosimilitud de la distribución aintótica "
     "de las autocorrelaciones muestrales de órdenes 1 a n de un ruido "
     "ARMA de tamaño T.\n"
     "Genera polinomios AR y MA estacionarios de grados p y q de forma "
     "uniforme en el módulo de las raíces."),
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BMatARMAACFBartlettLLHRandStationary::CalcContens()
//--------------------------------------------------------------------
{
  int p = (int)Real(Arg(1));
  int q = (int)Real(Arg(2));
  BMat& acf = Mat(Arg(3));
  int T = (int)Real(Arg(4));
  int N = (int)Real(Arg(5));
  bool exact = (int)Real(Arg(6));
  int n = acf.Data().Size();
  int s = T*4;
  if(Arg(7)) { s = (int)Real(Arg(7)); }
  contens_.Alloc(N,1);
  contens_.GetData().Replicate(BDat::NegInf(),N);
  int k;
  int res;
  BPol ar, ma;
  BMat cov, L;
  if(exact)
  {
    for(k=0; k<N; k++)
    {
      ar = RandStationary(p,1);
      ma = RandStationary(q,1);
      contens_(k,0) = ARMAACFBartlettLLH_Theorical(ar,ma,acf,T,s);
    }
  }
  else
  {
    _covACF_Bartlett(cov,acf,n,T,s);
    res = TolLapack::dpotrf(CblasLower,cov,L);
    if(res) { return; }
    for(k=0; k<N; k++)
    {
      ar = RandStationary(p,1);
      ma = RandStationary(q,1);
      contens_(k,0) = ARMAACFBartlettLLH_Commuted (ar,ma,acf,T,L);
    }
  };
}

//--------------------------------------------------------------------
void BuildArimaFactor(
  const BSet& s,
  BArray<BARIMAFactor>& factor, 
  int& p, 
  int& q)
//--------------------------------------------------------------------
{
  factor.ReallocBuffer(s.Card());
  int i;
  for(i=p=q=0; i<s.Card(); i++)
  {
    BSet& si = Set(s[i+1]);
    factor(i).s_   = (BInt)Real(si.Field("Periodicity"));
    factor(i).dif_ = Pol(si.Field("DIF"));
    factor(i).ar_  = Pol(si.Field("AR"));
    factor(i).ma_  = Pol(si.Field("MA"));
    p += factor(i).ar_.Degree();
    q += factor(i).ma_.Degree();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARIMALevinsonEval);
DefExtOpr(1, BSetARIMALevinsonEval, "ARIMALevinsonEval", 2, 4, "Set Matrix Real Real",
  "(Set arima, Matrix output [, Real calcInitialValues=false, Real sigma=1.0])",
  I2("Calculates the residuals of the model with initial values of maximum "
     "likelyhood using Levinson algoritm.\n"
     "NOTE: If output matrix is well differenced arima must have difference "
     "polynomial 1 in all seasonalities.\n"
     "If all works fine, returns following information:\n",
     "Calcula los residuos del modelo con valores iniciales máximo "
     "verosímiles usando el algoritmo de Levinson.\n"
     "NOTA: Si la matriz de output ya está diferenciada hay que introducir"
     "en arima el polinomio de diferencias 1 en todas las estacionalidades.\n"
     "Si todo funciona correctamente devuelve lo siguiente:\n")+
  "Matrix z:"+      I2("Noise",
                       "Ruido")+"\n"+
  "Matrix w:"+      I2("Differenced noise",
                      "Ruido diferenciado")+"\n"+
  "Matrix ACoviw:"+ I2("Inv(ACOV)*w : Is an auxiliar variable but can be usefull",
                       "Inv(ACOV)*w : Variable axuliar que puede ser de gran ayuda.")+"\n"+
  "Matrix a:"+      I2("Tra(Psi)*Inv(ACOV)*W : Model residuals conditioned by difference noise.",
                       "Tra(Psi)*Inv(ACOV)*W : Residuos del modelo condicionados al ruido diferenciado.")+"\n"+
  "Matrix ACOV:"+   I2("Model autocovarianze function",
                       "Función de autocoavarianzas del modelo")+"\n"+
  "Matrix ACF:"+    I2("Model autocorrelation function",
                       "Función de Autocorrelaciones del modelo")+"\n"+
  "Matrix PACF:"+   I2("Model partial autocorrelation function",
                       "Función de autocorrelaciones parciales del modelo")+"\n"+
  "Matrix IACF:"+   I2("Model inverse autocorrelation function",
                       "Función de autocorrelaciones inversas del modelo")+"\n"+
  "Real ACOVDetN:"+ I2("Det(ACOV)^(1/N): N-th root of autocovarianze matrix determinant",
                       "Det(ACOV)^(1/N): Raíz N-ésima del determinante de la matriz de autocoavarianzas")+"\n"+
  "Real wtACOViw:"+ I2("w'*ACOViw : Estiamted squares sum of differenced noise in autocovariance metrics",
                       "w'*ACOViw: Suma de cuadrados del ruido diferenciado en la métrica de las autocovarianzas.")+"\n"+
  "Real aS2:"+      I2("a'*a : Residual's squares sum",
                       "a'*a : Suma de cuadrados de los residuos")+"\n"+
  "Real logLH:"+    I2("-(1/2)*(N*Log(2*pi*sigma^2) + N*Log(ACOVDetN) + wtACOViw/sigma^2) : Logarithm of likelihhod function",
                       "-(1/2)*(N*Log(2*pi*sigma^2) + N*Log(ACOVDetN) + wtACOViw/sigma^2) : Logaritmo de la función de verosimilitud")+"\n"+
  "Matrix w0:"+     I2("Max(p,q) initial values of differenced noise. Only returns it if calcInitialValues is true",
                      "Los Max(p,q) valores iniciales del ruido diferenciado. Sólo los devuelve si calcInitialValues es cierto")+"\n"+
  "Matrix a0:"+     I2("Max(p,q) initial values of residuals. Only returns it if calcInitialValues is true",
                      "Los Max(p,q) valores iniciales del ruido diferenciado. Sólo los devuelve si calcInitialValues es cierto")+"\n",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARIMALevinsonEval::CalcContens()
//--------------------------------------------------------------------
{
  BInt i, p, q;
  BSet& s = Set(Arg(1));
  BMat& z = Mat(Arg(2));
  bool  iv= (Arg(3))?(Real(Arg(3))!=0):false;
  BDat  sigma = (Arg(4))?Dat(Arg(4)):1.0;
  BArray<BARIMAFactor> factor;
  BuildArimaFactor(s,factor,p,q);
  if(!(p+q))
  {
  }
  BARIMA model;
  model.PutFactors(factor);
  model.PutOutputData(z);
  model.CalcLikelihood_Levinson(sigma, model.w_.Rows(),BTRUE,iv);
  BMatrix<BDat> ACOV_ (model.aCov_ .Size(),1,model.aCov_ .Buffer());
  BMatrix<BDat> ACF_  (model.aCor_ .Size(),1,model.aCor_ .Buffer());
  BMatrix<BDat> PACF_ (model.paCor_.Size(),1,model.paCor_.Buffer());
  BMatrix<BDat> IACF_ (model.aCorI_.Size(),1,model.aCorI_.Buffer());

  BInt N_     = model.w_.Rows();
  BList* aux = NULL;
  BList* lst = NULL;
  LstFastAppend(lst,aux,NewFND(BMat, z,        model.z_));
  LstFastAppend(lst,aux,NewFND(BMat, w,        model.w_));
  LstFastAppend(lst,aux,NewFND(BMat, ACoviw,   model.aCoviw_));
  LstFastAppend(lst,aux,NewFND(BMat, a,        model.a_));
  LstFastAppend(lst,aux,NewFND(BMat, ACOV,     ACOV_));
  LstFastAppend(lst,aux,NewFND(BMat, ACF,      ACF_));
  LstFastAppend(lst,aux,NewFND(BMat, PACF,     PACF_));
  LstFastAppend(lst,aux,NewFND(BMat, IACF,     IACF_));
  LstFastAppend(lst,aux,NewFND(BDat, ACOVDetN, model.aCovDetN_));
  LstFastAppend(lst,aux,NewFND(BDat, wtACOViw, model.wtCoviw_));
  LstFastAppend(lst,aux,NewFND(BDat, aS2,      model.ata_));
  LstFastAppend(lst,aux,NewFND(BDat, logLH,    model.logLikelihood_));
  if(iv)
  {
    LstFastAppend(lst,aux,NewFND(BMat, w0,     model.w0_));
    LstFastAppend(lst,aux,NewFND(BMat, a0,     model.a0_));
  }
  contens_.RobElement(lst);
  contens_.SetIndexByName();
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARIMAAlmagroEval);
DefExtOpr(1, BSetARIMAAlmagroEval, "ARIMAAlmagroEval", 2, 3, "Set Matrix Real",
  "(Set arima, Matrix output [, Real sigma=1.0])",
  I2("Calculates the residuals of the model with initial values of maximum "
     "likelyhood using Almagro algoritm.\n"
     "NOTE: If output matrix is well differenced arima must have difference "
     "polynomial 1 in all seasonalities.\n"
     "If all works fine, returns following information:\n",
     "Calcula los residuos del modelo con valores iniciales máximo "
     "verosímiles usando el algoritmo de Almagro.\n"
     "NOTA: Si la matriz de output ya está diferenciada hay que introducir"
     "en arima el polinomio de diferencias 1 en todas las estacionalidades.\n"
     "Si todo funciona correctamente devuelve lo siguiente:\n")+
  "Matrix z:"+      I2("Noise",
                       "Ruido")+"\n"+
  "Matrix w:"+      I2("Differenced noise",
                      "Ruido diferenciado")+"\n"+
  "Matrix a:"+      I2("Tra(Psi)*Inv(ACOV)*W : Model residuals conditioned by difference noise.",
                       "Tra(Psi)*Inv(ACOV)*W : Residuos del modelo condicionados al ruido diferenciado.")+"\n"+
  "Matrix ACOV:"+   I2("Model autocovarianze function",
                       "Función de autocoavarianzas del modelo")+"\n"+
  "Matrix ACF:"+    I2("Model autocorrelation function",
                       "Función de Autocorrelaciones del modelo")+"\n"+
  "Real ACOVDetN:"+ I2("Det(ACOV)^(1/N): N-th root of autocovarianze matrix determinant",
                       "Det(ACOV)^(1/N): Raíz N-ésima del determinante de la matriz de autocoavarianzas")+"\n"+
  "Real wtACOViw:"+ I2("w'*ACOViw : Estiamted squares sum of differenced noise in autocovariance metrics",
                       "w'*ACOViw: Suma de cuadrados del ruido diferenciado en la métrica de las autocovarianzas.")+"\n"+
  "Real aS2:"+      I2("a'*a : Residual's squares sum",
                       "a'*a : Suma de cuadrados de los residuos")+"\n"+
  "Real logLH:"+    I2("-(1/2)*(N*Log(2*pi*sigma^2) + N*Log(ACOVDetN) + wtACOViw/sigma^2) : Logarithm of likelihhod function",
                       "-(1/2)*(N*Log(2*pi*sigma^2) + N*Log(ACOVDetN) + wtACOViw/sigma^2) : Logaritmo de la función de verosimilitud")+"\n"+
  "Matrix w0:"+     I2("The p initial values of differenced noise. Only returns it if calcInitialValues is true",
                      "Los p valores iniciales del ruido diferenciado. Sólo los devuelve si calcInitialValues es cierto")+"\n"+
  "Matrix a0:"+     I2("The q initial values of residuals. Only returns it if calcInitialValues is true",
                      "Los q valores iniciales del ruido diferenciado. Sólo los devuelve si calcInitialValues es cierto")+"\n",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARIMAAlmagroEval::CalcContens()
//--------------------------------------------------------------------
{
  BInt i, p, q;
  BSet& s = Set(Arg(1));
  BMat& z = Mat(Arg(2));
  BDat  sigma = (Arg(3))?Dat(Arg(3)):1.0;
  BArray<BARIMAFactor> factor;
  factor.ReallocBuffer(s.Card());

  for(i=p=q=0; i<s.Card(); i++)
  {
    BSet& si = Set(s[i+1]);
    factor(i).s_   = (BInt)Real(si.Field("Periodicity"));
    factor(i).dif_ = Pol(si.Field("DIF"));
    factor(i).ar_  = Pol(si.Field("AR"));
    factor(i).ma_  = Pol(si.Field("MA"));
    p += factor(i).ar_.Degree();
    q += factor(i).ma_.Degree();
  }
  if(!(p+q))
  {
  }
  BARIMA model;
  model.PutFactors(factor);
  model.PutOutputData(z);
  model.CalcLikelihood_Almagro(sigma);
  BMatrix<BDat> ACOV_ (model.aCov_ .Size(),1,model.aCov_ .Buffer());
  BMatrix<BDat> ACF_  (model.aCor_ .Size(),1,model.aCor_ .Buffer());

  BInt N_     = model.w_.Rows();
  BList* aux = NULL;
  BList* lst = NULL;
  LstFastAppend(lst,aux,NewFND(BMat, z,        model.z_));
  LstFastAppend(lst,aux,NewFND(BMat, w,        model.w_));
  LstFastAppend(lst,aux,NewFND(BMat, a,        model.a_));
  LstFastAppend(lst,aux,NewFND(BMat, ACOV,     ACOV_));
  LstFastAppend(lst,aux,NewFND(BMat, ACF,      ACF_));
  LstFastAppend(lst,aux,NewFND(BDat, ACOVDetN, model.aCovDetN_));
  LstFastAppend(lst,aux,NewFND(BDat, wtACOViw, model.wtCoviw_));
  LstFastAppend(lst,aux,NewFND(BDat, aS2,      model.ata_));
  LstFastAppend(lst,aux,NewFND(BDat, logLH,    model.logLikelihood_));
  LstFastAppend(lst,aux,NewFND(BMat, w0,       model.w0_));
  LstFastAppend(lst,aux,NewFND(BMat, a0,       model.a0_));
  contens_.RobElement(lst);
  contens_.SetIndexByName();
}
/*
//--------------------------------------------------------------------
void Taper_TukeyHanning(double rho, BArray<double>& h)
//--------------------------------------------------------------------
{
  static double pi = BDat::Pi().Value();
  int n = h.Size();
  int l = (int)rho*n;
  int t;
  h.AllocBuffer(n);
  double* H = h.GetBuffer();
  double HS2 = 0;
  for(t=0; t<l; t++, H++)
  {
    *H =  0.5*(1-cos(pi*(t+0.5)))/l;
    HS2 += *H * *H;
  }
  for(t; t<n-l; t++, H++)
  {
    *H = 1;
    HS2 += *H * *H;
  }
  for(t; t<n; t++, H++)
  {
    *H =  0.5*(1-cos(pi*(n-t-0.5)))/l;
    HS2 += *H * *H;
  }
  H = h.GetBuffer();
  double HS = sqrt(HS2);
  for(t=0; t<n; t++, H++)
  {
    *H/=HS;
  }
}

/*
//--------------------------------------------------------------------
struct harmonic_power
//--------------------------------------------------------------------
{
  bool   calculated_;
  double exp_2_pi_i_  
}


//--------------------------------------------------------------------
  complex eval_hasrmonic_pol(const BPol& pol, int T, int k)
//--------------------------------------------------------------------
{
  register int n  = pol->Size();
  register int i, deg, nextDeg;
  register double  y = 0;
  register const BMonome<BDat>* aux = pol.Buffer()+(n-1);
  deg = aux.Degree();
  z = exp(2*pi*)
  for(i=n-1; i>=0; i--)
  {
    y += aux.Coef();
    aux--;
    nextDeg = (i>0)?aux.Degree():0
    y *= z^(deg-nextDeg);
    deg = nextDeg;
  }
  return(y);
}

//--------------------------------------------------------------------
  double power_spectrum(const BPol& ar, const BPol& ma, int n, int k)
//--------------------------------------------------------------------
{

}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARIMATaperedWhittelEval);
DefExtOpr(1, BSetARIMATaperedWhittelEval, "ARIMATaperedWhittelEval", 2, 4, "Ratio arma Matrix Real Matrix",
          "(Set arima, Matrix output [, Real sigma=1.0, Matrix taper=TukeyHanningTaper(2/Sqrt(T))])",
  I2("Calculates the tapered Whittle aproximation to ARIMA log-likelihood upon on spectrañ density.\n"
     "See more on http://epub.wu-wien.ac.at/dyn/virlib/wp/mediate/epub-wu-01_9dc.pdf?ID=epub-wu-01_9dc"
     "NOTE: If output matrix is well differenced arima must have difference "
     "polynomial 1 in all seasonalities.\n"
     "If all works fine, returns following information:\n",
     "Calcula el logaritmo de la verosimilitud aproximada de un modelo ARIMA mediante el método "
     "de Wittle disminuido (tapered) basado en la densidad espectral.\n"
     "Ver más detalles en http://epub.wu-wien.ac.at/dyn/virlib/wp/mediate/epub-wu-01_9dc.pdf?ID=epub-wu-01_9dc"
     "NOTA: Si la matriz de output ya está diferenciada hay que introducir"
     "en arima el polinomio de diferencias 1 en todas las estacionalidades.\n"
     "Si todo funciona correctamente devuelve lo siguiente:\n")+
  "Matrix z:"+      I2("Noise",
                       "Ruido")+"\n"+
  "Matrix w:"+      I2("Differenced noise",
                      "Ruido diferenciado")+"\n"+
  "Real logLH:"+    I2("-Sum {Log(S(j)) + (1/(2*PI)*I(j)/S(j)); j=1...T} : Logarithm of spectral density function",
                       "-Sum {Log(S(j)) + (1/(2*PI)*I(j)/S(j)); j=1...T} : Logaritmo de la función de densidad espectral")+"\n"+
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARIMATaperedWhittelEval::CalcContens()
//--------------------------------------------------------------------
{
  BInt i, p, q;
  BRat& arma = Rat(Arg(1));
  BMat& z = Mat(Arg(2));
  BDat  sigma = (Arg(3))?Dat(Arg(3)):1.0;
  BArray<BARIMAFactor> factor;
  factor.ReallocBuffer(s.Card());

  for(i=p=q=0; i<s.Card(); i++)
  {
    BSet& si = Set(s[i+1]);
    factor(i).s_   = (BInt)Real(si.Field("Periodicity"));
    factor(i).dif_ = Pol(si.Field("DIF"));
    factor(i).ar_  = Pol(si.Field("AR"));
    factor(i).ma_  = Pol(si.Field("MA"));
    p += factor(i).ar_.Degree();
    q += factor(i).ma_.Degree();
  }
  if(!(p+q))
  {
  }
  BARIMA model;
  model.PutFactors(factor);
  model.PutOutputData(z);
  model.CalcLikelihood_Almagro(sigma);

  BInt N_     = model.w_.Rows();
  BList* aux = NULL;
  BList* lst = NULL;
  LstFastAppend(lst,aux,NewFND(BMat, z,        model.z_));
  LstFastAppend(lst,aux,NewFND(BMat, w,        model.w_));
  LstFastAppend(lst,aux,NewFND(BDat, ACOVDetN, model.aCovDetN_));
  LstFastAppend(lst,aux,NewFND(BDat, wtACOViw, model.wtCoviw_));
  LstFastAppend(lst,aux,NewFND(BDat, aS2,      model.ata_));
  LstFastAppend(lst,aux,NewFND(BDat, logLH,    model.logLikelihood_));
  contens_.RobElement(lst);
  contens_.SetIndexByName();
}
*/


//--------------------------------------------------------------------
  void ARMAGohbergSemenculInvACovMultVector_Step1
  (
    const BRational <BDat>& arma,
    const BMatrix   <BDat>& w,
          BMatrix   <BDat>& Lxtw, 
          BMatrix   <BDat>& Lytw 
  )
//--------------------------------------------------------------------
{
  int i, j, N = w.Rows(), c=w.Columns();
  BMatrix<BDat> w0(2*N,c);
  for(j=0; j<c; j++)
  {
    for(i=0; i<  N; i++) { w0(i,j) = w(i,j); }
    for(i=N; i<2*N; i++) { w0(i,j) = 0;      }
  }
  MatForwardDifEq (arma, w0, Lxtw);
  MatBackwardDifEq(arma, w0, Lytw);
}

//--------------------------------------------------------------------
  void ARMAGohbergSemenculInvACovMultVector_Step2
  (
    const BRational <BDat>& arma,
    const BMatrix   <BDat>& w,
    const BMatrix   <BDat>& Lxtw, 
    const BMatrix   <BDat>& Lytw, 
          BMatrix   <BDat>& aCoviw 

  )
//--------------------------------------------------------------------
{
  int i, j, N = w.Rows(), c=w.Columns();
  BMatrix<BDat> LxLxtw(2*N,c), LyLytw(2*N,c); 
  aCoviw.Alloc(N,c);
  for(j=0; j<c; j++)
  {
    for(i=0; i<N; i++) { Lytw(i,j) = 0; }
  }
  MatBackwardDifEq(arma, Lxtw, LxLxtw);
  MatForwardDifEq (arma, Lytw, LyLytw);
  for(j=0; j<c; j++)
  {
    for(i=0; i<N; i++) 
    { 
      aCoviw(i,j) = LxLxtw(i,j) - LyLytw(i,j); 
    }
  }
}

//--------------------------------------------------------------------
  void ARMAGohbergSemenculInvACovMultVector
  (
    const BRational <BDat>& arma,
    const BMatrix   <BDat>& w,
          BMatrix   <BDat>& aCoviw 
)
//--------------------------------------------------------------------
{
  BMatrix<BDat> Lxtw, Lytw;
  ARMAGohbergSemenculInvACovMultVector_Step1(arma, w, Lxtw, Lytw);
  ARMAGohbergSemenculInvACovMultVector_Step2(arma, w, Lxtw, Lytw, aCoviw);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARMAGohbergSemenculEval);
DefExtOpr(1, BSetARMAGohbergSemenculEval, "ARMAGohbergSemenculEval", 3, 4, 
  "Polyn Polyn Matrix Real", 
  "(Polyn AR, Polyn MA, Matrix W [, Real calcResiduals=false])",
  I2("Estimates the model varianze as \n"
     "  (1/N)*Tra(W)*Inv(Cov)*W (Maximum Likelihood)\n"
     "using Gohberg-Semencul formulae for inverse of symmetric Toeplitz matrices."
     "This Formulae has been adapted to ARMA autocarianze matrix "
     "trough aproximated difference equations to get the best performance."
     "If optional calcResiduals argument is true, then it also estimates "
     "the model residuals conditioned by difference noise W:\n"
     "  Tra(PSI)*Inv(Cov)*W\n"
     "If W has two or more columns results are stored in correspondent "
     "columns of varianze and residuals, if proceeds."
     "This function is an aproximation and could return erroneous "
     "results in extremal circumstances."
     "Then use ARIMALevinsonEval that is more accurate and slower."
     ,
     "Calcula la varianza del modelo como\n"
     "  (1/N)*Tra(W)*Inv(Cov)*W (Máxima Verosimilitud)\n"
     "mediante la aplicación de la fórmula de Gohberg-Semencul "
     "para el caso de la inversa de una matriz simétrica de Toeplitz "
     "que en el caso ARMA se puede aproximar mediante ecuaciones en "
     "diferencias sobre el ruido ampliado con N ceros por la derecha y "
     "con valores iniciales nulos."
     "Si el argumento opcional calcResiduals es cierto, entonces también "
     "estima los residuos del modelo condicionados al ruido diferenciado W:\n"
     "  Tra(PSI)*Inv(Cov)*W\n"
     "Si W tiene varias columnas los cálculos se reproducen para cada "
     "una de ellas devolviendo los resultados en las correspondientes "
     "columnas de varianzas y residuos, si procede."
     "Esto es especialmente útil para estimación masiva de modelos "
     "idénticos sobre distintas series de ruido"
     "Esta función es una aproximación y puede dar resultados erróneos "
     "en casos extremos. En tal caso es mejor usar ARIMALevinsonEval "
     "que es mucho más precisa y lenta."
    ),BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARMAGohbergSemenculEval::CalcContens()
//--------------------------------------------------------------------
{
  static BDat _log2pie_2_ = 0.5*Log(2*BDat::Pi()*BDat::E());
  BPolyn <BDat> ar = Pol(Arg(1));
  BPolyn <BDat> ma = Pol(Arg(2));
  BMatrix<BDat> w  = Mat(Arg(3));
  BBool         cr = (Arg(4)) ? (BBool) (Real(Arg(4))) : false;
  int           ca = (Arg(5)) ? (int)   (Real(Arg(5))) : 0; //VBR: not used!
  int i, j, N = w.Rows(), c = w.Columns();
  BMatrix<BDat> Lxtw, Lytw, var(1,c);
  BRational <BDat> arma(ar,ma);
  ARMAGohbergSemenculInvACovMultVector_Step1(arma, w, Lxtw, Lytw);
//Std(BText("\nLxtw=\n")+Lxtw.Name());
//Std(BText("\nLytw=\n")+Lytw.Name());
  bool numErr = false;
  for(j=0; j<c; j++)
  {
    var(0,j) = 0; 
    for(i=0; i<  N; i++) { var(0,j) += Lxtw(i,j)*Lxtw(i,j); }
    for(i=N; i<2*N; i++) { var(0,j) -= Lytw(i,j)*Lytw(i,j); }
    if((var(0,j)<0) && !numErr)
    {
      var(0,j) = BDat::PosInf();
/*
      Error(I2("Numerical error in ARMAGohbergSemenculEval: "
               "MaxLHVar aproximation is out of range and take negative values.",
               "Error de tipo numérico en ARMAGohbergSemenculEval: "
               "la aproximación de MaxLHVar está fuera de rango y da valores negativos."));
*/
      numErr = true;
    }  
  }
  var /= double(N);
  BUserMat* Var = BContensMat::New("MaxLHVar",var, I2("Tra(W)*Inv(Cov)*W : Maximum likelihood estimated varianze of the model",
                                          "Tra(W)*Inv(Cov)*W : Estimación máximo verosímil de la varianza del modelo"));
  BList* result=NIL;
  BList* aux=NIL;
  LstFastAppend(result, aux, Var);
  if(cr)
  {
    BMatrix<BDat> aCoviw, a(N,c);
    BRational <BDat> maar(ma,ar);
    ARMAGohbergSemenculInvACovMultVector_Step2(arma, w, Lxtw, Lytw, aCoviw);
    MatForwardDifEq (maar, aCoviw, a);
    BDat rss  = MtMSqr(a)(0,0);
    BDat ren = Sqrt(rss/N);

    BUserMat* aCoviw_	= BContensMat::New("aCoviw",aCoviw, I2("Inv(ACOV)*w : Is an auxiliar variable but can be usefull",
                                                          "Inv(ACOV)*w : Variable axuliar que puede ser de gran ayuda."));
    BUserMat* a_	    = BContensMat::New("a",a,           I2("Tra(Psi)*Inv(ACOV)*W : Model residuals conditioned by difference noise.",
                                                          "Tra(Psi)*Inv(ACOV)*W : Residuos del modelo condicionados al ruido diferenciado."));
    BUserDat* rss_    = BContensDat::New("RSS",rss,       I2("a'*a : Residual's squares sum",
                                                          "a'*a : Suma de cuadrados de los residuos"));
    BUserDat* ren_    = BContensDat::New("REN",ren,       I2("Sqrt(a'*a)/N : Residual's euclidean norm",
                                                          "Sqrt(a'*a)/N : Norma euclídea de los residuos"));
    LstFastAppend(result, aux, aCoviw_);
    LstFastAppend(result, aux, a_);
    LstFastAppend(result, aux, rss_);
    LstFastAppend(result, aux, ren_);
  }
/*
//VBR: This code is a failed try of superfast Log(|ACOV|) calculation
//     but some code may be usefull later
  if(ca>0)
  {
    BInt k, p=ar.Degree(), q=ma.Degree(), s = 3*(p+q+1);
    if(ca<s) { ca = s; }
    BMatrix<BDat> ACOV(ca,1), ACF(ca,1), PACF(ca,1), LD(ca,1);
    if(s==1) //white noise 
    {
      ACOV.SetAllValuesTo(0);
      ACF .SetAllValuesTo(0);
      PACF.SetAllValuesTo(0);
      LD  .SetAllValuesTo(0);
      ACOV(1,0) = ACF(1,0) = PACF(1,0) = 1;
    }
    else
    {
      BArray<BDat> acov(ca), acf(s+1), c(s+1), x, y, pacf;
      if(!ARMAAutoCovarianzeVector(acov, ar, ma, ca, 1)) 
      {
        ACOV.SetAllValuesTo(BDat::Unknown());
        ACF .SetAllValuesTo(BDat::Unknown());
        PACF.SetAllValuesTo(BDat::Unknown());
        LD  .SetAllValuesTo(BDat::Unknown());
      }
      else
      { 
        BDat logDet, acov0=acov(0);
        for(i=0; i<s; i++) { acf(i) = acov(i)/acov0; }
        c.Replicate(1.0,s+1);
        if(!Levinson(acf,c,x,y,pacf,logDet,1))
      //if(!YuleWalkerDurbin(acf, y, pacf, logDet, 1))
        {
          PACF.SetAllValuesTo(BDat::Unknown());
          LD  .SetAllValuesTo(BDat::Unknown());
        }
        else
        {
          BArray<BDat> b(ca),B(ca);
          BDat a, logACov0 = Log(acov0);
          for(i=0; i<s; i++) 
          { 
            ACOV(i,0) = acov(i);
            ACF (i,0) = acf (i);
            PACF(i,0) = a = pacf(i);
          }
          for(; i<ca; i++) 
          { 
            ACOV(i,0) = acov(i);
            ACF (i,0) = ACOV(i,0)/acov0;
            PACF(i,0) = 0;
            for(k=1; k<ma.Size(); k++)
            {
              PACF(i,0) -= PACF(i-ma(k).Degree(),0)*ma(k).Coef();
            }
          }
          for(i=0; i<ca; i++) 
          { 
            a       = PACF(i,0);
            b (i)   = Log(1.0-a)+Log(1.0+a);
            B (i)   =  (i==0)?0:B (i-1)  +b(i);
            LD(i,0) = ((i==0)?0:LD(i-1,0)+B(i))+logACov0;
          }
        }
      }
    }
    BUserMat* ACOV_       = BContensMat::New("ACOV",ACOV,	 I2("Model autocovarianze function",
                                                      "Función de autocoavarianzas del modelo"));
    BUserMat* ACF_        = BContensMat::New("ACF",ACF,	 I2("Model autocorrelation function",
                                                      "Función de Autocorrelaciones del modelo"));
    BUserMat* PACF_       = BContensMat::New("PACF",PACF,  I2("Model partial autocorrelation function",
                                                      "Función de autocorrelaciones parciales del modelo"));
    BUserMat* ACOVLogDet_ = BContensMat::New("ACOVLogDet",LD,    I2("Log(Det(ACOV)): Logarithm of autocovarianze matrix determinant",
                                                      "Log(Det(ACOV)): Logaritmo del determinante de la matriz de autocoavarianzas"));

    LstFastAppend(result, aux, ACOV_);
    LstFastAppend(result, aux, ACF_);
    LstFastAppend(result, aux, PACF_);
    LstFastAppend(result, aux, ACOVLogDet_);
  }
*/
  contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetYuleWalkerDurbin);
DefExtOpr(1, BSetYuleWalkerDurbin, "YuleWalkerDurbin", 1, 1, "Matrix",
  "(Matrix r)",
  "Solves the Yule-Walker equations\n"
  "\n"
  "         T*y = -R\n"
  "\n"
  "where "
  "\n"
  "        r' = (r(0), r(1) ... r(n)) \n"
  "\n"
  "        R' = (r(1) ... r(n)) \n"
  "\n"
  "and T is the nxn symmetric toeplitz matrix generated by r \n"
  "\n"
  "        T(i,j) = r(|i-j|) i,j=1..n \n"
  "\n"
  "This function uses the Durbin algorithm."
  "(Matrix Computation G.H. Golub & C.F. Van Loan, página 196) ",
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetYuleWalkerDurbin::CalcContens()
//--------------------------------------------------------------------
{
/*
  BMatrix<BDat> r = Mat(Arg(1)), y(r.Rows()-1,1), PACF(r.Rows()-1,1);
  BDat logDet;
  YuleWalkerDurbin(r.Data(), y.GetData(), PACF.GetData(), logDet, r.Rows());
  BList* result=NIL;
  BList* aux=NIL;
  LstFastAppend(result, aux,  BContensMat::New("",y,	     "y"));
  LstFastAppend(result, aux,  BContensMat::New("",PACF,	  "PACF"));
  LstFastAppend(result, aux,  BContensDat::New("",logDet,	"Log(Det(T))"));
  contens_.RobElement(result);
*/
//*
  BMatrix<BDat> r = Mat(Arg(1));
  BArray<BDat> y, PACF;
  BDat logDet;
  YuleWalkerDurbin(r.Data(), y, PACF, logDet, r.Rows());
  BList* result=NIL;
  BList* aux=NIL;
  LstFastAppend(result, aux,  BContensMat::New("y",     BMat(y.Size(),1,y.Buffer()),	   "Solution"));
  LstFastAppend(result, aux,  BContensMat::New("PCC",   BMat(y.Size(),1,PACF.Buffer()), "Partial Correlation Coefficients"));
  LstFastAppend(result, aux,  BContensDat::New("LogDet",logDet,	"Log(Det(T))"));
  contens_.RobElement(result);
/* */
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLevinson);
DefExtOpr(1, BSetLevinson, "Levinson", 2, 2, "Matrix Matrix",
  "(Matrix r, Matrix c)",
  "Solves simultaneously the general symmetric Toeplitz system\n"
  "\n"
  "         T(r)*x = -c\n"
  "\n"
  "and the Yule-Walker equations\n"
  "\n"
  "\n"
  "         T(r)*y = -R\n"
  "\n"
  "where "
  "\n"
  "        r' = (r(0), r(1) ... r(n)) \n"
  "\n"
  "        R' = (r(1) ... r(n)) \n"
  "\n"
  "and T(r) is the nxn symmetric toeplitz matrix generated by r \n"
  "\n"
  "        T(r)[i,j] = r(|i-j|) i,j=1..n \n"
  "\n"
  "This function uses the Levinson algorithm."
  "(Matrix Computation G.H. Golub & C.F. Van Loan, página 196) ",
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetLevinson::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat> r = Mat(Arg(1)), 
                c = Mat(Arg(2));
  BArray<BDat>  x, y, PCF;
  BDat logDet;

  Levinson(r.Data(), c.Data(), x, y, PCF, logDet, r.Rows());

  BList* result=NIL;
  BList* aux=NIL;
  BMatrix<BDat> x_  (x  .Size()-1,1,x  .Buffer()+1);
  BMatrix<BDat> y_  (y  .Size()-1,1,y  .Buffer()+1);
  BMatrix<BDat> PCF_(PCF.Size()-1,1,PCF.Buffer()  );
  LstFastAppend(result, aux,  BContensMat::New("x",x_,	    "-Inv(T(r))*c"));
  LstFastAppend(result, aux,  BContensMat::New("y",y_,	    "-Inv(T(r))*R"));
  LstFastAppend(result, aux,  BContensMat::New("PCF",PCF_, "Partial Correlation Coefficients"));
  LstFastAppend(result, aux,  BContensDat::New("LogDet",logDet,  "Log(Det(T(r)))"));
  contens_.RobElement(result);
}

  int LevinsonARMA(const BArray<BDat>& r,
                   const BArray<BDat>& c,
                         BArray<BDat>& x,
                         BArray<BDat>& y,
                         BArray<BDat>& PACF,
                         BDat        & logDet,
                         int           period,
                   const BPolyn<BDat>& ar,
                   const BPolyn<BDat>& ma);

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLevinsonARMA);
DefExtOpr(1, BSetLevinsonARMA, "LevinsonARMA", 4, 4, "Matrix Matrix Polyn Polyn",
  "(Matrix r, Matrix c, Polyn ar, Polyn ma)",
  "Solves simultaneously the system\n"
  "\n"
  "         T(r)*x = -c\n"
  "\n"
  "and the Yule-Walker equations\n"
  "\n"
  "\n"
  "         T(r)*y = -R\n"
  "\n"
  "where "
  "\n"
  "        r' = (r(0), r(1) ... r(n)) \n"
  " is the autocorrelation function of an ARMA process"
  "\n"
  "        R' = (r(1) ... r(n)) \n"
  "\n"
  "and T(r) is the nxn symmetric toeplitz matrix generated by r \n"
  "\n"
  "        T(r)[i,j] = r(|i-j|) i,j=1..n \n"
  "\n"
  "that is the autocorrelation matrix of the ARMA process.\n"
  "This function uses the Levinson algorithm optimized for this case."
  "(Matrix Computation G.H. Golub & C.F. Van Loan, página 196) ",
	  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetLevinsonARMA::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat> r = Mat(Arg(1)), 
                c = Mat(Arg(2));
  BPolyn<BDat> ar = Pol(Arg(3));
  BPolyn<BDat> ma = Pol(Arg(4));
  BArray<BDat> x, y, PCF;
  BDat logDet;

  LevinsonARMA(r.Data(), c.Data(), x, y, PCF, logDet, r.Rows(), ar, ma);

  BList* result=NIL;
  BList* aux=NIL;
  BMatrix<BDat> x_  (x  .Size()-1,1,x  .Buffer()+1);
  BMatrix<BDat> y_  (y  .Size()-1,1,y  .Buffer()+1);
  BMatrix<BDat> PCF_(PCF.Size()-1,1,PCF.Buffer()  );
  LstFastAppend(result, aux,  BContensMat::New("x",x_,	    "-Inv(T(r))*c"));
  LstFastAppend(result, aux,  BContensMat::New("y",y_,	    "-Inv(T(r))*R"));
  LstFastAppend(result, aux,  BContensMat::New("PCF",PCF_, "Partial Correlation Coefficients"));
  LstFastAppend(result, aux,  BContensDat::New("LogDet",logDet,  "Log(Det(T(r)))"));
  contens_.RobElement(result);
}





BPolyn<BDat> ReversePolyn(const BPolyn<BDat>& p);
void VectorToSchur(BPolyn<BDat>& p, const BArray<BDat>& v, int n, int n0);
void SchurToVector(BArray<BDat>& v, const BPolyn<BDat>& p, int n0);

//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolUncVec2SttPol);
DefExtOpr(1, BPolUncVec2SttPol, "UncVec2SttPol", 2, 3, 
  "Matrix Real Real",
  "(Matrix vec, Real period [, Real iniPos] )",
  "Returns the stationary polynomial represented by an unconstrained "
  "vector. The inverse function is SttPol2UncVec.",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BPolUncVec2SttPol::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& V = Mat(Arg(1));
  const BArray<BDat>& vec = V.Data();
  int n = vec.Size();
  if(n==0) { contens_ = BPolyn<BDat>::One(); }
  else
  {
    int period = (int)Real(Arg(2));
    int iniPos = 0;
    if(Arg(3)) { iniPos = (int)Real(Arg(3)); }
    int d;
    BPolyn<BDat> q; 
    VectorToSchur(q, vec, n, iniPos);
    for(d = 0; d<q.Size(); d++) { q(d).PutDegree(q(d).Degree()*period); }
    contens_ = ReversePolyn(q);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatSttPol2UncVec);
DefExtOpr(1, BMatSttPol2UncVec, "SttPol2UncVec", 2, 2, "Polyn Real",
  "(Polyn pol, Real period)",
  "Returns the unconstrained column vector representing an stationary "
  "polynomial. The inverse function is UncVec2SttPol.",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BMatSttPol2UncVec::CalcContens()
//--------------------------------------------------------------------
{
  BPolyn<BDat>& pol = Pol(Arg(1));
  int period =(int)Real(Arg(2));
  int n = pol.Degree()/period;
  if(n<=0)
  {
    contens_.Alloc(0,1);
  }
  else
  {
    contens_.Alloc(n,1);
    BArray<BDat>& vec = contens_.GetData();
    int d;
    BPolyn<BDat> q = pol;
    for(d = 0; d<q.Size(); d++) { q(d).PutDegree(q(d).Degree()/period); }
    q = ReversePolyn(q);
    SchurToVector(vec, q, 0);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BPol, BPolTemporary, BPolVec2Pol);
DefExtOpr(1, BPolVec2Pol, "Vec2Pol", 2, 2, 
  "Matrix Real",
  "(Matrix v, Real p)",
  "Returns the polynomial 'p' represented by a vector 'v' in this way:\n"
  "p(B) = 1 - v[1]*B^p  - v[2]*B^2*p - ... \n"
  "The inverse function is Pol2Vec.",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BPolVec2Pol::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& V = Mat(Arg(1));
  const BArray<BDat>& vec = V.Data();
  int d,n = vec.Size();
  if(n==0) { contens_ = BPolyn<BDat>::One(); }
  else
  {
    int period = (int)Real(Arg(2));
    contens_.AllocBuffer(n+1);
    contens_[0].PutDegree(0);
    contens_[0].PutCoef(1);
    for(d = 1; d<=n; d++) 
    { 
      contens_[d].PutDegree(d*period);
      contens_[d].PutCoef(-vec[d-1]);
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BMat, BMatTemporary, BMatPol2Vec);
DefExtOpr(1, BMatPol2Vec, "Pol2Vec", 2, 2, "Polyn Real",
  "(Polyn p, Real p)",
  "Returns the column vector 'v' representing a polynomial 'p' in this way:\n "
  "  p(B) = 1 - v[1]*B^p  - v[2]*B^2*p - ... \n"
  "The inverse function is Vec2Pol.",
	BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BMatPol2Vec::CalcContens()
//--------------------------------------------------------------------
{
  BPolyn<BDat>& pol = Pol(Arg(1));
  int period =(int)Real(Arg(2));
  int d,n = pol.Degree()/period;
  if(n<=0)
  {
    contens_.Alloc(0,1);
  }
  else
  {
    contens_.Alloc(n,1);
    BArray<BDat>& vec = contens_.GetData();
    for(d = 1; d<=n; d++) { vec[d-1] = -pol.Coef(period*d); }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARMAPreliminaryEstimation);
DefExtOpr(1, BSetARMAPreliminaryEstimation, "ARMAPreliminaryEstimation", 
  4, 6, "Matrix Real Real Real Real Real",
  "(Matrix ACVF, Real p, Real q, Real N [, Real maxIter=100*q, Real eps = 1/Sqrt(N))",
  "Estimates a preliminary AR(p)MA(q) non seassonal model \n"
  "  \n"
  " (1-phi(B)) z[t] = (1-theta(B)) a[t] ; t = 1 ... N \n"
  " \n"
  "  a ~ Normal(0,variance*I) \n"
  " \n"
  "using just the first 1+p+q sampling autocovariances of ARMA noise.\n"
  "Vector ACVF represents the sampling autocovariance of an unknown ARMA "
  "noise z of length N, and must have at least 1+p+q cells, and first one "
  "must be positive.\n"
  "If success, it returns a Set with positive real variance and "
  "polynomials 1-phi(B) and 1-phi(B). Elsewhere it returns unknown variance."
  "The algorithm is given by Box and Jenkins in their book"
  "\n"
  "  Time Series Analysis, forecasting and control \n"
  "  Revised edition, January 1976 \n"
  "  pages 498-500 \n",
  BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARMAPreliminaryEstimation::CalcContens()
//--------------------------------------------------------------------
{
  BMatrix<BDat>& acvf = Mat(Arg(1));
  int p = (int)Abs(Real(Arg(2)));
  int q = (int)Abs(Real(Arg(3)));
  int N = (int)Abs(Real(Arg(4)));
  //Maximum iteration stop criteria
  int maxIter = 100*q;
  //Tolerance stop criteria
  BDat eps = 1.0/Sqrt(N);
  if(Arg(5)) { maxIter = (int)Abs(Real(Arg(5))); }
  if(Arg(6)) { eps = Dat(Arg(6)); }
  BDat variance;
  BArray<BDat> phi;
  BArray<BDat> theta;
  BPolyn<BDat> phiB;
  BPolyn<BDat> thetaB;
  phiB.AllocBuffer(p+1);
  thetaB.AllocBuffer(q+1);
  int i;
  BARIMA::PreliminaryEstimation(acvf.Data(), p, q, N, maxIter, eps, variance, phi, theta);
  phiB[0].PutDegree(0);
  phiB[0].PutCoef(1);
  for(i=1; i<=p; i++) { 
    phiB[i].PutDegree(i);
    phiB[i].PutCoef(-phi(i-1));
  }
  thetaB[0].PutDegree(0);
  thetaB[0].PutCoef(1);
  for(i=1; i<=q; i++) { 
    thetaB[i].PutDegree(i);
    thetaB[i].PutCoef(-theta(i-1));
  }
  BList* result = NULL, *aux = NULL;
  LstFastAppend(result, aux,  BContensDat::New("variance",variance,	""));
  LstFastAppend(result, aux,  BContensPol::New("phi",phiB, ""));
  LstFastAppend(result, aux,  BContensPol::New("theta",thetaB,  ""));
  contens_.RobElement(result);
}

/*
//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetARIMAConditionalEstimate);
DefExtOpr(1, BSetARIMAConditionalEstimate, "ARMAConditionalEstimate", 1, 4, "Set Date Date Set",
  "(Set arima, Matrix z [, Matrix w0, Matrix a0])",
  I2("Estimates by least squares approximation an ARIMA model "
     "conditioned by initial values",
     "Estima por la aproximación de mínimos cuadrados un modelo ARIMA "
     "condicionado por los valores iniciales"),
    BOperClassify::Sthocastic_);
//--------------------------------------------------------------------
void BSetARIMAConditionalEstimate::CalcContens()
//--------------------------------------------------------------------
{
  BInt i, p, q;
  BSet& s = Set(Arg(1));
  BMat& z = Mat(Arg(2));
  BMat w0, a0;
  if(Arg(3)) { w0 = Mat(Arg(3)); }
  if(Arg(4)) { a0 = Mat(Arg(4)); }
  BArray<BARIMAFactor> factor;
  BuildArimaFactor(s,factor,p,q);
  if(!(p+q))
  {
    return;
  }
  BARIMA model;
  model.PutFactors(factor);
  model.PutOutputData(z);
  model.w0_ = w0;
  model.a0_ = a0;

  BARIMAConditionalEstimation est(@model);
  est.Marquardt();
  return;
}

*/