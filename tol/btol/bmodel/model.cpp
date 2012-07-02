/* model.cpp: Functions of BModel
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
#include <tol/tol_bmodel.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bstat.h>
#include <tol/tol_bprdist.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_btsrgrai.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bstruct.h>


//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BModel).
//--------------------------------------------------------------------
BTraceInit("modset.cpp");

BDat& DoDiagnostics ();

BText RegStaParts(BInt i)
{
  BText txt;
  if(!i) { txt = "Regular"; }
  else	 { txt = I2("Seasonal","Estacional")+ " ("+i+")"; }
  return(txt);
};


BArray<BText> BModel::qualifTitle_;
BArray<BText> BModel::testTitle_;

//Static Names and Descriptions to be used in built-in functions
//PREFIX_ must be defined in a different way in each 
// .cpp that uses these macroes to avoid repeated instantiations
// linker errors
#define FNDParam1(NAME)                 PrefixFNDParam1(MODEL_, NAME);
#define FNDParam(NAME, DESC)            PrefixFNDParam(MODEL_, NAME, DESC);
#define NewFND(TYPE, NAME, OBJ)         PrefixNewFND(TYPE, MODEL_, NAME, OBJ)
#define NewSetFND(NAME, ELEM, STR, SUB) PrefixNewSetFND(MODEL_, NAME, ELEM, STR, SUB)

FNDParam(ACOR, "Residuous autocorrelation");
FNDParam(Alfa, "P-Value of F-Schnedecor statistic");
FNDParam(AR, "Auto-Regressive polynomials");
FNDParam(ArithmeticQualification, "Diagnostics qualification value");
FNDParam(Asymmetry, "Asymmetry of residuals");
FNDParam(Average, "Average of residuals");
FNDParam(Constant, "Constant of the model");
FNDParam(COR, "Correlation Inverse Choleski Matrix");
FNDParam(Correlations, "Analisys of correlations of parameters and residuals");
FNDParam(COV, "Covarianze Matrix");
FNDParam(CovInf, "Covarianze Information");
FNDParam(COVL, "Covarianze Choleski Matrix");
FNDParam(COVLi, "Covarianze Inverse Choleski Matrix");
FNDParam(D, "Singular values of Jacobian SVD");
FNDParam(DataNumber, "Length of residuals");
FNDParam(Dif, "Difference polynomial");
FNDParam(Effects, "The effects of the linear inputs with omega-transfer function");
FNDParam(F, "Regression F-Schnedecor statistic");
FNDParam(FirstDate, "First date of estimation");
FNDParam(FstTransfor, "First coefficient (exponential) of Box-Cox transformation");
FNDParam(Information, "Statitistics information");
FNDParam(InformationMatrix, "Information matrix (J'J)/varianze");
FNDParam(Input, "Set of linear inputs with omega-transfer function");
FNDParam(Jacobian, "Jacobian matrix of residuals");
FNDParam(Kernel, "Jacobian kernel");
FNDParam(Kurtosis, "Kurtosis of residuals");
FNDParam(LastDate, "Last date of estimation");
FNDParam(MA, "Moving-Average polynomials");
FNDParam(MaxLHSigma, "Maximum Likelihood estimation of standard error of residuals");
FNDParam(NonLinInput, "User defined non linear inputs");
FNDParam(normMaxLH, "Modified norm of residuals that is minimized to maximize model likelihood");
FNDParam(MarquardtIterNumber, "Number of Marquardt iterations which have been run");
FNDParam(Omega, "Omega-transfer function");
FNDParam(PACOR, "Residuous partial autocorrelation");
FNDParam(ParInf, "Parameters Information");
FNDParam(Pearson, "Pearson Coefficient");
FNDParam(Period, "Periodicity of the model polynomials");
FNDParam(Qualification, "Diagnostics qualification cathegory");
FNDParam(R2, "Regression R2 statistic");
FNDParam(RSS, "Residual Sum of Squares");
FNDParam(Series, "Model estimated time series");
FNDParam(Sigma, "Standard error of residuals");
FNDParam(SndTransfor, "Second coefficient (additive) of Box-Cox transformation");
FNDParam(Swartz, "Swartz information criterium value");
FNDParam(TSS, "Total Sum of Squares");
FNDParam(U, "Left singular vectors of Jacobian SVD");
FNDParam(V, "Left singular vectors of Jacobian SVD");
FNDParam(v1, "First freedom degree of F-Schnedecor statistic");
FNDParam(v2, "Second freedom degree of F-Schnedecor statistic");
FNDParam(Variance, "Variance of residuals");
FNDParam(VarNumber, "Number of variables");
FNDParam(ParameterInfo, "Parameters Information");
FNDParam(Diagnostics, "Diagnostics Information");


//--------------------------------------------------------------------
static void ModelInitialize()
//--------------------------------------------------------------------
{
  static BBool isInitialized = BFALSE;
  if(!isInitialized)
  {
    BModel::qualifTitle_.ReallocBuffer(4);
    BModel::qualifTitle_[0] = I2("UNNECESSARY", "INNECESARIO");
    BModel::qualifTitle_[1] = I2("GOOD",	"BUENO");
    BModel::qualifTitle_[2] = I2("ACCEPTABLE",	"ACEPTABLE");
    BModel::qualifTitle_[3] = I2("REFUSED",	"RECHAZADO");

    BModel::testTitle_.ReallocBuffer(11);
    BModel::testTitle_[ 0] = I2("First Regular AutoCorrelation",
				"Primera Autocorrelacion Regular");
    BModel::testTitle_[ 1] = I2("First Seasonal AutoCorrelation",
				"Primera Autocorrelación Estacional");
    BModel::testTitle_[ 2] = I2("Second Regular AutoCorrelation",
				"Segunda Autocorrelación Regular");
    BModel::testTitle_[ 3] = I2("Second Seasonal AutoCorrelation",
				"Segunda Autocorrelación Estacional");
    BModel::testTitle_[ 4] = I2("Regular Box-Pierce-Ljung's statistic",
				"Estadístico de Box-Pierce-Ljung Regular");
    BModel::testTitle_[ 5] = I2("Seasonal Box-Pierce-Ljung's statistic",
				"Estadístico de Box-Pierce-Ljung Estacional");
    BModel::testTitle_[ 6] = I2("Fisher's Distance to Normal",
				  "Distancia a la normal de Fisher");
    BModel::testTitle_[ 7] = I2("Minimum Signification of Parameters",
				"Mínima Significación de los Parámetros");
    BModel::testTitle_[ 8] = I2("Maximum Correlation of Parameters",
				"Máxima Correlación de los Parámetros");
    BModel::testTitle_[ 9] = I2("Mixed Signification and Correlation",
				"Combinacion de Significación y Correlación");
    BModel::testTitle_[10] = I2("Probabilty of Unary Roots",
				"Probabilidad de Raíces Unitarias");
    isInitialized = BTRUE;
  }
}


//--------------------------------------------------------------------
BModel::BModel(const BText& outName, const BText& foreFile, BInt period)

/*! BModel constructor
 */
//--------------------------------------------------------------------
    :aborted_(BFALSE),
     constant_(BDat::Unknown()),
     nonLinFun_(NIL), 
     nonLinPar_(),
     DI_(NULL),
     aCorNum_(0)
{
    ModelInitialize();
    outName_	 = outName;
    resName_	 = outName+"Res";
    dif_	 = BPol::One();
    periodicity_ = period;
    
    ReadForecast(foreFile);
    ConstructARMAFromRoots();
    res_=NIL;
    diagValue_  .ReallocBuffer(testTitle_.Size());
    diagPunct_  .ReallocBuffer(testTitle_.Size());
    diagQualify_.ReallocBuffer(testTitle_.Size());
    testAccept_ .ReallocBuffer(testTitle_.Size());
    testRefuse_ .ReallocBuffer(testTitle_.Size());
    
}


//--------------------------------------------------------------------
BModel::BModel(	      BUserTimeSerie* output,
		      BList*		inputSeries,
		const BPol&		dif,
		const BArray<BPol>&	arFactors,
		const BArray<BPol>&	maFactors,
		const BDate&		first,
		const BDate&		last,
		      BReal		firstTrans,
		      BReal		secondTrans,
		      BReal		constant      )
    
/*! BModel constructor
 */
//--------------------------------------------------------------------
: aborted_(BFALSE),
  constant_(BDat::Unknown()),
  DI_(NULL),
  aCorNum_(0)
{
  inData_ .AddList  (inputSeries);
  outData_.AddSerie(output),
 
    ModelInitialize();
    output_		= output;
    outName_		= output_->Identify();
    resName_		= outName_+"Res";
    arFactors_		= arFactors;
    maFactors_		= maFactors;
    dif_			= dif;
    ConstructARMAFromRoots();
    firstDate_		= first;
    lastDate_		= last;
    firstTransformation_	= firstTrans;
    secondTransformation_ = secondTrans;
    if(constant==1) { constant_ = 1.0; }
    inpName_.ReallocBuffer(inData_.NumSeries());
    for(int n=0; n<inpName_.Size(); n++)
    {
	    inpName_[n] = inData_[n+1]->Identify();
    }
    res_=NIL;
    diagValue_  .ReallocBuffer(testTitle_.Size());
    diagPunct_  .ReallocBuffer(testTitle_.Size());
    diagQualify_.ReallocBuffer(testTitle_.Size());
    testAccept_ .ReallocBuffer(testTitle_.Size());
    testRefuse_ .ReallocBuffer(testTitle_.Size());
}


//--------------------------------------------------------------------
/*! BModel constructor
 */
BModel::BModel(BSet& set)
//--------------------------------------------------------------------
    : aborted_(BFALSE), constant_(BDat::Unknown()), DI_(NULL),
     aCorNum_(0)
{
    ModelInitialize();
    BStruct* strInput  = FindStruct("@InputDef");
//Std(set.Name());
    BInt n,k;
    output_	     	        = Tsr (set.Field("Output"));
    firstTransformation_	= Real(set.Field("FstTransfor"));
    secondTransformation_       = Real(set.Field("SndTransfor"));
    periodicity_		= (BInt)Real(set.Field("Period"));
    
//VBR: Inexplicablemente a veces llegan valores no nulos con lo 
//     que se entiende que el modelo tiene constante
    constant_ = BDat::Unknown(); 
//constant_		        = Dat (set.Field("Constant"));
//if (constant_.Value()== 0.0) {	constant_ = BDat::Unknown(); }
    
    dif_			= Pol (set.Field("Dif"));
    BSet&	 arFactors	= Set (set.Field("AR"));
    BSet&	 maFactors	= Set (set.Field("MA"));
    BSet&	 input		= Set (set.Field("Input"));
    BSet&	 nonLinInput	= Set (set.Field("NonLinInput"));
    BList* aux = NIL;
    
    transfer_.ReallocBuffer(input.Card());
    inpName_.ReallocBuffer(input.Card());

    for(n=0;n<transfer_.Size();n++)
    {
	    BSet& inp = Set(input[n+1]);
      if(inp.Struct()!=strInput)
      {
    		Error(I2("One or more inputs has no @InputDef structure",
		          	 "Uno o más inputs no tienen estructura @InputDef"));
		    aborted_ = BTRUE;
		    return;
      }
	    transfer_[n] = Pol(inp[1]);
      if( transfer_[n][0].Degree()<0)
      {
    		Error(I2("Transfer function cannot have negative degree terms (F)",
		          	 "la función de transferencia no puede tener términos de grado negativo (F)"));
		    aborted_ = BTRUE;
		    return;
      }
	    inpName_ [n] = inp[2]->Identify();
	    inpName_ [n].Replace('\n',' ');
	    inpName_ [n].Compact();
      inData_.AddSerie(Tsr(inp[2]));
    }

    int pq = arFactors.Card() + maFactors.Card();

    if(arFactors.Card() != maFactors.Card())
    {
	Error(I2("Distinct number of factors in AR and MA structures",
		 "No hay el mismo nmero de factores en las estructuras "
		 "AR y MA"));
	pq=0;
    }

    armaStationarity_.ReallocBuffer(pq);
    armaLagrangeMult_.ReallocBuffer(pq);
    armaFactors_     .ReallocBuffer(pq);
    armaIndex_	     .ReallocBuffer(pq);
    armaPeriod_	     .ReallocBuffer(pq);
    arFactors_	     .ReallocBuffer((!pq)?0:arFactors.Card());
    maFactors_       .ReallocBuffer((!pq)?0:maFactors.Card());

    for(k=n=0; n<arFactors_.Size(); n++, k++)
    {
	if(arFactors[n+1]->Grammar()!=GraPolyn())
	    if(arFactors[n+1]->Grammar()==GraReal())
		arFactors.ChangeElement(n+1,
					GraPolyn()->Casting(arFactors[n+1]));
	    else {
		Error(I2("AR have not Polyn members",
			 "AR no contienen miembros Polyn"));
		aborted_ = BTRUE;
		return;
	    }
	
	arFactors_[n] = Pol(arFactors[n+1]);
	armaFactors_[k] = &(arFactors_[n]);
	armaPeriod_[k] = 0;
	armaIndex_[k] = 0;

	if(arFactors_[n].Size()>1)
	{
	    armaPeriod_[k] = (*armaFactors_[k])[1].Degree();

	    if(armaFactors_[k]->Degree() != 
	       armaPeriod_[k]*(armaFactors_[k]->Size()-1))
	    {
		Warning(I2("Non full AR polynomial ",
			   "No está lleno el polinomio AR ") << 
			"\n  " << armaFactors_[k]->Name() << "\n" <<
			I2("Schur variable change cannot be applied to avoid "
			   "non stationarity.\nIt's highly recomended to add "
			   "a new AR factor with periodicity like ",
			   "No se podrá aplicar el cambio de variable de "
			   "Schur para evitar la no estacionariedad.\n"
			   "Es muy recomendable añadir un nuevo factor AR "
			   "de periodicidad aproximada a ") <<
			armaFactors_[k]->Degree());
	    }

	    BPolStationaryFunction S(arFactors_[n], armaPeriod_(k));
	    BDat s = S.GetValue();
	    armaStationarity_(k) = s;
	    armaLagrangeMult_(k) = 0;//(s>0)?Sqrt(s):BDat(0);
	}
    }

    for(n=0; n<maFactors_.Size(); n++, k++)
    {
	if(maFactors[n+1]->Grammar()!=GraPolyn())
	    if(maFactors[n+1]->Grammar()==GraReal())
		maFactors.ChangeElement(n+1,
					GraPolyn()->Casting(maFactors[n+1]));
	    else {
		Error(I2("MA have not Polyn members",
			 "MA no contienen miembros Polyn"));
		aborted_ = BTRUE;
		return;
	    }

	maFactors_[n] = Pol(maFactors[n+1]);
	armaFactors_[k] = &(maFactors_[n]);
	armaPeriod_[k] = 0;
	armaIndex_[k] = 1;

	if(maFactors_[n].Size()>1)
	{
	    armaPeriod_[k] = (*armaFactors_[k])[1].Degree();

	    if(armaFactors_[k]->Degree()!=
	       armaPeriod_[k]*(armaFactors_[k]->Size()-1))
	    {
		Warning(I2("Non full MA ploynomial ",
			   "No está lleno el polinomio MA ") << 
			"\n  " << armaFactors_[k]->Name() << "\n" <<
			I2("Schur variable change cannot be applied to avoid "
			   "non stationarity.\nIt's highly recomended to add "
			   "a new MA factor with periodicity like ",
			   "No se podrá aplicar el cambio de variable de "
			   "Schur para evitar la no estacionariedad.\n"
			   "Es muy recomendable añadir un nuevo factor MA "
			   "de periodicidad aproximada a ") <<
			armaFactors_[k]->Degree());
	    }

	    BPolStationaryFunction S(maFactors_[n], armaPeriod_(k));
	    BDat s = S.GetValue();
	    armaStationarity_(k) = s;
	    armaLagrangeMult_(k) = 0;//(s>0)?Sqrt(s):BDat(0);
	}
    }
    
    BText nonLinFunName = "NullSerie";
    if(nonLinInput.Card())
    {
      nonLinFunName = Text(nonLinInput.Field("Function"));
    }
    nonLinFun_  = UCode(nonLinFunName);

    if((nonLinFunName!="NullSerie") && nonLinFun_)
    {
//  Trace(nonLinFunName);
	BSet& nLPar = Set(nonLinInput.Field("Parameter"));
	BList* result = aux = NIL;

	for(n=1;n<=nLPar.Card();n++)
  {
	  LstFastAppend(result, aux, BContensDat::New(Dat(nLPar[n])));
  }
	
	nonLinPar_.RobElement(result);
    }

    outData_.AddSerie(output_);
    outName_     = output_->Name();
    resName_     = outName_+"Res";
    ConstructARMAFromRoots();
    res_=NIL;
    diagValue_  .ReallocBuffer(testTitle_.Size());
    diagPunct_  .ReallocBuffer(testTitle_.Size());
    diagQualify_.ReallocBuffer(testTitle_.Size());
    testAccept_ .ReallocBuffer(testTitle_.Size());
    testRefuse_ .ReallocBuffer(testTitle_.Size());
}


//--------------------------------------------------------------------
BList* BModel::GetDefinitionList()

/*! BModel constructor
 */
//--------------------------------------------------------------------
{
    BStruct* strInput  = FindStruct("@InputDef");
    BStruct* strModel  = FindStruct("@ModelDef");
    BStruct* strNonLin = FindStruct("@NonLinearInputDef");
    
    if(!strInput)
    {
	Error(I2("@InputDef structure is not implementeded.",
		 "La estructura @InputDef no está implementada."));
	aborted_ = BTRUE;
	return(NIL);
    }
    if(!strModel)
    {
	Error(I2("@ModelDef structure is not implementeded.",
		 "La estructura @ModelDef no está implementada."));
	aborted_ = BTRUE;
	return(NIL);
    }
    if(!strNonLin)
    {
	Error(I2("@NonLinearInputDef structure is not implementeded.",
		 "La estructura @NonLinearInputDef no está implementada."));
	aborted_ = BTRUE;
	return(NIL);
    }
    BInt i,k,r;
    BList* lsta	= NIL;
    BList* auxa	= NIL;
  //BList* lstb	= NIL;
  //BList* auxb	= NIL;
    BList* lst	= NIL;
    BList* aux	= NIL;
    BSyntaxObject * obj;
    
    LstFastAppend(lst,aux,output_);
    LstFastAppend(lst,aux,
		  obj = NewFND(BDat,FstTransfor,firstTransformation_));
    LstFastAppend(lst,aux,
		  obj = NewFND(BDat,SndTransfor,secondTransformation_));
    LstFastAppend(lst,aux,obj = NewFND(BDat,Period,periodicity_));
    LstFastAppend(lst,aux,obj = NewFND(BDat,Constant,constant_));
    LstFastAppend(lst,aux, obj = NewFND(BPol,Dif,dif_));
    for((i=0),(lsta=auxa=NIL);i<arFactors_.Size();i++) 
    {
  	  LstFastAppend(lsta,auxa, obj = BContensPol::New(RegStaParts(i),arFactors_[i]));
    }
    // Generic pues no se su tipo. 
    LstFastAppend(lst,aux, obj = NewSetFND(AR,lsta,NIL,BSet::Generic));
    for((i=0),(lsta=auxa=NIL);i<maFactors_.Size();i++) 
    {
  	LstFastAppend(lsta,auxa, 
		      obj = BContensPol::New(RegStaParts(i),maFactors_[i]));
    }
    // Generic pues no se su tipo. 
    LstFastAppend(lst,aux, obj = NewSetFND(MA,lsta,NIL,BSet::Generic));
    for((r=i=0),(lsta=auxa=NIL);i<transfer_.Size();i++)
    {
	BPol pol = transfer_(i);
	for(k=0; k<pol.Size(); k++, r++)
	{
	    if(eliminated_(r)) { pol(k).PutCoef(0); }
	}
	pol.Aggregate();
	BList* l=Cons(NewFND(BPol,Omega,pol),NCons(inData_[i+1]));
  BText uName = BText("Inp")+inpName_[i];
  LstFastAppend(lsta,auxa, obj = strInput->Create(l,uName));
  obj->PutName(uName);
    }
    // Generic pues no se su tipo. 
    LstFastAppend(lst,aux, obj = NewSetFND(Input,lsta,NIL,BSet::Generic));
    lsta=auxa=NIL;
    LstFastAppend(lsta,auxa, obj = BContensSet::New(nonLinPar_));
    BText nonLinFunName = "";
    if(nonLinFun_) { nonLinFunName = nonLinFun_->Name(); }
    LstFastAppend(lsta,auxa, obj = BContensText::New(nonLinFunName));
    LstFastAppend(lst,aux, obj = strNonLin->Create(lsta,"NonLinInput"));
    return(lst);
}

//--------------------------------------------------------------------
BSyntaxObject* BModel::GetDefinition()

/*! BModel constructor
 */
//--------------------------------------------------------------------
{
    BStruct* strModel  = FindStruct("@ModelDef");
    BSyntaxObject * def = strModel->Create(GetDefinitionList(),"Definition");
    def->PutName("Definition");
    return def;
}

//--------------------------------------------------------------------
BSyntaxObject* BModel::OneParamInfo(BText name,
				    BDat  value,
				    BReal factor,
				    BReal order,
				    BInt  n)
//--------------------------------------------------------------------
{
  BSyntaxObject * obj;
  
    BProbDist* T = NIL;
    if((N_<100)&&(N_>numParam_))
    {
	T = new BTStudentDist(N_-numParam_);
    }
    else
    {
	T = new BNormalDist(0,1);
    }
    BStruct* strParam = FindStruct("@ParameterInf");
    if(!strParam)
    {
	Error(I2("ParamaterInf structure is not implementeded.",
		 "La estructura ParamaterInf no está implementada."));
	aborted_ = BTRUE;
  delete T;
	return(NIL);
    }
    
    BList* lst	= NIL;
    BList* aux	= NIL;
    infoParam_[n] = BText("P[")+n+"]="+name + "[" + factor + "," + order+"]";
/*
  for(i=0;i<n;i++)
  {
  BDat cor = paramCor_.Get(n,i);
  if(Abs(cor)>testRefuse_[8])
  {
  Std(BText("\n  ")+cor+" : "+infoParam_[n]+" , "+infoParam_[i]);
  }
  }
*/

/*BMatrix<BDat> AUX;
AUX.Alloc(1,2);
AUX(0,0) = param_[n];
AUX(0,1) = paramSD_[n];
TRZ(AUX);*/
  //BDat	 student = param_[n]/paramSD_[n];
    BDat	 student = value/paramSD_[n];
  //BDat stdOri = value/student;
    BDat	 prob	 = 2*(1-T->Dist(Abs(student)));
    LstFastAppend(lst,aux, obj = BContensText::New(name));
    LstFastAppend(lst,aux, obj = BContensDat::New(factor));
    LstFastAppend(lst,aux, obj = BContensDat::New(order));
    LstFastAppend(lst,aux, obj = BContensDat::New(value));
    LstFastAppend(lst,aux, obj = BContensDat::New(paramSD_[n]));
    LstFastAppend(lst,aux, obj = BContensDat::New(student));
    LstFastAppend(lst,aux, obj = BContensDat::New(prob));
    delete T;
    obj = strParam->Create(lst,name);
    obj->PutName(name);
    return obj;
}

//--------------------------------------------------------------------
BSyntaxObject* BModel::ParamInfo()
//--------------------------------------------------------------------
{
  BInt i=0, j, k, r, deg;
  BList* lst	= NIL;
  BList* aux	= NIL;
  BDat value;
  BText name;
  PutParameter(param_);
    
/*
  Std(BText("\n")+I2("Parameters correlations great than ",
  "Correlaciones de los parámetros mayores que ")+
  testRefuse_[8].Name() + " :");
*/
  name = "Constant";
  if(constant_.IsKnown())
  {
    LstFastAppend(lst,aux, OneParamInfo(name, param_[i], 1, 0, i));
    i++;
  }
  for(r=j=0; j<transfer_.Size(); j++)
  {
    name = inpName_[j];
    for(k=0;k<transfer_[j].Size();k++,r++)
    {
      if(!eliminated_(r))
      {
        deg = transfer_[j][k].Degree();
        LstFastAppend(lst,aux, OneParamInfo(name, param_[i], 1, deg,  i));
        i++;
      }
    }
  }
  BUserFunction* uFun = (BUserFunction*)(Code(nonLinFun_).Operator());
  for(j=1; j<=nonLinPar_.Card(); j++)
  {
    name = uFun->ArgName(j-1);
    LstFastAppend(lst,aux, OneParamInfo(name, param_[i], 1, j, i));
    i++;
  }
  for(j=0; j<arFactors_.Size(); j++)
  {
    name = RegStaParts(j)+"AR";
    for(j=0; j<arFactors_.Size(); j++)
    {
      name = RegStaParts(j)+"AR";
      for(k=1; k<arFactors_[j].Size(); k++, i++)
      {
        deg = arFactors_[j][k].Degree();
        value = -arFactors_[j][k].Coef();
        LstFastAppend(lst,aux, OneParamInfo(name, value, 1+j, deg,  i));
      }
    }
  }
  for(j=0; j<maFactors_.Size(); j++)
  {
    name = RegStaParts(j)+"MA";
    for(k=1; k<maFactors_[j].Size(); k++, i++)
    {
      deg   = maFactors_[j][k].Degree();
      value = -maFactors_[j][k].Coef();
      LstFastAppend(lst,aux, OneParamInfo(name, value, 1+j, deg,  i));
    }
  }
/*
  Std(BText("\n")+I2("End of parameters correlations inform.",
  "Fin del informe de correlaciones de los parámetros."));
*/
  BSyntaxObject * obj = NewSetFND(ParameterInfo,lst,NIL,BSet::Generic);
  return obj;
}


//--------------------------------------------------------------------
BSyntaxObject* BModel::OneParamDiag(BInt n)
//--------------------------------------------------------------------
{
  BStruct* strTestResult = FindStruct("@TestResult");
  if(!strTestResult)
  {
    Error(I2("@TestResult structure is not implementeded.",
	     "La estructura @TestResult no está implementada"));
    aborted_ = BTRUE;
    return(NIL);
  }

  BList* lst	= NIL;
  BList* aux	= NIL;
  BSyntaxObject * tmp;

  if(DoDiagnostics().Value())
  {
    LstFastAppend(lst,aux, tmp = BContensText::New(testTitle_ [n]));
    LstFastAppend(lst,aux, tmp = BContensDat::New(diagValue_ [n]));
    LstFastAppend(lst,aux, tmp = BContensDat::New(diagPunct_ [n]));
    LstFastAppend(lst,aux, tmp = BContensDat::New(testAccept_[n]));
    LstFastAppend(lst,aux, tmp = BContensDat::New(testRefuse_[n]));
    LstFastAppend(lst,aux, tmp = BContensText::New(qualifTitle_[diagQualify_[n]]));
  }
  tmp = strTestResult->Create(lst,testTitle_ [n]);
  tmp->PutName(testTitle_[n]);
  return tmp;
}


//--------------------------------------------------------------------
BSyntaxObject* BModel::ParamDiag()
//--------------------------------------------------------------------
{
    BInt n;
    BList* lst	= NIL;
    BList* aux	= NIL;
    BSyntaxObject * tmp;
    
    if (DoDiagnostics().Value()) {
	for(n=0; n<testTitle_.Size(); n++) {
	    LstFastAppend(lst,aux, OneParamDiag(n));
	}
    }
    tmp = NewSetFND(Diagnostics,lst,NIL,BSet::Generic);
return tmp;
}

//--------------------------------------------------------------------
void BModel::CopyInfToSet(BSet& set)
//--------------------------------------------------------------------
{
//BTimer tm("\nCopyInfToSet");
  BInt i,j;
  BList* lst	= NIL;
  BList* aux	= NIL;
  BSet	 inf;
  BSet	 def;
  BSet	 par;
  BSet	 diag;
  
  BMatrix<BDat>  Y     = outDifData_.T();
  BMatrix<BDat>  Yprev = Y - A_;
  BDat rss       = MtMSqr(A_)(0,0);
  BDat var       = rss / N_;
  BDat stdErr    = Sqrt(var);
  BDat sigma     = Sqrt(arma_.wtCoviw_/(N_-arma_.d_));
  BDat normMaxLH = stdErr*arma_.likelihoodCoef_;
  BDat r2        = R2Coefficient(Y.Data(),Yprev.Data());
  BDat pearson   = Sqrt(r2);

  BList* lsta = NIL;
  BList* auxa = NIL;
  BSyntaxObject* paramInfo = ParamInfo();
  BSyntaxObject* tmp;
  LstFastAppend(lsta,auxa,tmp=NewFND(BDate,FirstDate,firstDate_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDate,LastDate,lastDate_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,DataNumber,N_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,VarNumber,param_.Size()));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Average,Average(A_.Data())));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,RSS,rss));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Variance,var));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Sigma,standardError_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Asymmetry,AsymmetryCoefficient(A_.Data())));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Kurtosis,Kurtosis(A_.Data())));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Pearson,pearson));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,R2,r2));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,Swartz,SwartzInfo_));
  if(DoDiagnostics().Value())
  {
    LstFastAppend(lsta,auxa,tmp=NewFND(BText,Qualification,qualifTitle_[qualification_]));
    LstFastAppend(lsta,auxa,tmp=NewFND(BDat,ArithmeticQualification,arithmeticQualification_));
  }
  else
  {
    LstFastAppend(lsta,auxa,tmp=NewFND(BText,Qualification,"?"));
    LstFastAppend(lsta,auxa,tmp=NewFND(BDat,ArithmeticQualification,BDat::Unknown()));
  }
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,MaxLHSigma,sigma));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,normMaxLH,normMaxLH));
  LstFastAppend(lsta,auxa,tmp=NewFND(BDat,MarquardtIterNumber,iterationNumber_));

  inf.RobElement(lsta);
//LstFastAppend(lst,aux,tmp=BContensSet::New("Information",inf,""));
  LstFastAppend(lst,aux,tmp=NewFND(BSet,Information,inf));
  
  LstFastAppend(lst,aux,GetDefinition());

  BUserTimeSet*	   tms	= output_->Dating();
  BText		   nam	= output_->Identify();
  BText bcStrText = BText("BoxCoxStruct(") +
		    firstTransformation_+", "+secondTransformation_+")";

  BList* effects = NIL;
  BList* effectsAux = NIL;


  CalcInputFilter(N_);
  BMatrix<BDat> eff;
  for(i=0;i<transfer_.Size();i++)
  {
    BText name = BText("Effect_")+ToName(inpName_[i]);
    eff = effectData_.SubRow(i);
//  Std(BText("\nEfecto ")+i+" : "+name);
//  TRZ(eff);
    BTsrPrimary* serInp = new BTsrPrimary
    (
      "",
      I2("Effect ", "Efecto ")+ name,
      tms,
      tms->Prev(lastDate_,eff.Data().Size()-1),
      eff.Data()
    );
    serInp->PutName(name);
    LstFastAppend(effects, effectsAux, serInp);
  }
  // Generic pues no se su tipo. 
  BUserSet* effectsSet =  NewSetFND(Effects,effects,NIL,BSet::Generic);
  BArray<BDat> noiseData = fullData_;
  if(inputFilter_.Size())
  {
    BInt filterOffset = noiseData.Size() - inputFilter_.Size();
    for(int k=Maximum(0,filterOffset); k<noiseData.Size(); k++)
    {
      noiseData[k] -= inputFilter_[k-filterOffset];
    }
  }
  else
  {
    inputFilter_      .Replicate(0,transOutData_.Size());
    systemInputFilter_.Replicate(0,transOutData_.Size());
  }

  BTsrPrimary* filter = new BTsrPrimary
  (
    "",
    I2("Sum of input effects of ", "Suma de los efectos de los inputs de ")+
    nam,
    tms,
    tms->Prev(lastDate_,inputFilter_.Size()-1),
    inputFilter_
  );
  //filter->PutName(nam+"Filter");
  filter->PutName("Filter");
  
  BTsrPrimary* noise = new BTsrPrimary
  (
    "",
    I2("ARIMA Noise of ", "Ruido ARIMA de ")+
    nam,
    tms,
    tms->Prev(lastDate_,noiseData.Size()-1),
    noiseData
  );
  //noise->PutName(nam+"Noise");
  noise->PutName("Noise");

  //--------------------------------------------------
  // Residual Series
  //--------------------------------------------------
  {
//  BTimer tm("Residual Series");
    DESTROY(res_);
    BMat A0 = A0_.Sub(A0_.Rows()-q_,0,q_,1);
    BMat A =  A0_ << A_;
    res_ = new BTsrPrimary
    (
      "",
      I2("Model residuals of ", "Residuos del modelo de ")+
         output_->Identify(),
      tms,
      tms->Prev(lastDate_,A.Data().Size()-1),
      A.Data()
    );
    //res_->PutName(output_->Identify()+"Res");
    res_->PutName("Residuals");
  }

  BInt t0 =  systemInputFilter_.Size()-interruptionValue_.Size();
  for(j=0; j<interruptionValue_.Size();j++)
  {
    if(systemInputFilter_(j+t0)!=0)
    {
      fullData_(j) = interruptionValue_(j) = - systemInputFilter_(j+t0);
    }
  }
  for(i=0; i<interruptionIndex_.Size();i++)
  {
    j = interruptionIndex_[i];
    interruptionValue_[j] = fullData_[j];
  }
  BTsrPrimary* transformed = new BTsrPrimary
  (
    "",
    I2("Box-Cox Transformed ", "Transformada de Box-Cox ")+
    bcStrText+
    I2("of ","de ") +nam,
    tms,
    tms->Prev(lastDate_,transOutData_.Size()-1),
    transOutData_
  );
  //transformed->PutName(nam+"Transformed");
  transformed->PutName("Transformed");
  
  for(BInt n=0; n<interruptionValue_.Size(); n++)
  {
    if(interruptionValue_(n).IsUnknown())
    {
      interruptionValue_(n) = 0;
    }
  }
  BTsrPrimary* interruptions = new BTsrPrimary
  (
    "",
    I2("Estimated values for interruptions in transformed of",
       "Valores estimados para la interrupciones en la transformada de ")+nam,
    tms,
    tms->Prev(lastDate_,interruptionValue_.Size()-1),
    interruptionValue_
  );
  //interruptions->PutName(nam+"Interruptions");
  interruptions->PutName("Interruptions");
  
  BTsrPrimary* full = new BTsrPrimary
  (
    "",
    I2("Not interrupted transformation of ",
       "Transformación no interrupida de ")+nam,
    tms,
    tms->Prev(lastDate_,fullData_.Size()-1),
    fullData_
  );
  //full->PutName(nam+"FullTransformed");
  full->PutName("FullTransformed");
  
  BMat Z = Z0_.Sub(Z0_.Rows()-p_,0,p_,1) << Z_;

  BTsrPrimary* difNoise = new BTsrPrimary
  (
    "",
    I2("Differenced noise of ", "Ruido diferenciado de ")+nam,
    tms,
    tms->Prev(lastDate_,Z.Rows()-1),
    Z.Data()
  );
  //difNoise->PutName(nam+"DifNoise");
  difNoise->PutName("DifNoise");
  
  BUserTimeSerie* prevHistTrans = new BTsrDifference(Cons(full,NCons(res_)));
  //prevHistTrans->PutName(nam+"PrevHistTrans");
  prevHistTrans->PutName("PrevHistTrans");
  prevHistTrans->PutDescription(
    I2("Historic forecasting for transformed of ",
       "Previsión histórica para la transformada de ")+nam);

  lsta = auxa = NIL;
  LstFastAppend(lsta,auxa,res_);
  LstFastAppend(lsta,auxa,difNoise);
  LstFastAppend(lsta,auxa,noise);
  LstFastAppend(lsta,auxa,interruptions);
  LstFastAppend(lsta,auxa,full);
  LstFastAppend(lsta,auxa,transformed);
  LstFastAppend(lsta,auxa,prevHistTrans);
  LstFastAppend(lsta,auxa,filter);
  LstFastAppend(lsta,auxa,effectsSet);
  BUserSet* series = NewSet("","",lsta, NIL, BSet::Generic);
  series->PutName("Series");
  series->PutDescription("Model estimated time series");
  LstFastAppend(lst,aux,series);
  LstFastAppend(lst,aux,paramInfo);

  lsta = auxa = NIL;

  BText& JacobianMethod();
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,Jacobian,jacobian_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,U,U_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,D,D_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,V,V_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,Kernel,kernel_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,InformationMatrix,informationMatrix_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,COV,paramCov_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,COR,paramCor_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,ACOR,resACor_));
  LstFastAppend(lsta,auxa,tmp=NewFND(BMat,PACOR,resPACor_));
  LstFastAppend(lst,aux,  tmp=NewSetFND(Correlations,lsta,NIL,BSet::Generic));

  LstFastAppend(lst,aux,ParamDiag());
  set.RobElement(lst);
}
