/* tol_bmodel.h: Class to handle with ARIMA model 
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

#ifndef TOL_BMODEL_H
#define TOL_BMODEL_H 1



#include <tol/tol_bpolgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_barma.h>

//#define POL_ORIGINAL_PARAM 1
#define POL_NATURAL_ORDER 1

//--------------------------------------------------------------------
// constants
//--------------------------------------------------------------------
#define BDIAGUNNECESSARY  0
#define BDIAGGOOD	  1
#define BDIAGACCEPT	  2
#define BDIAGREFUSE	  3


//--------------------------------------------------------------------
class BPolStationaryFunction : public BRnRFunction
//--------------------------------------------------------------------
{
private:
  BPolyn<BDat> p_;
  BDat f_;

public:

  BPolStationaryFunction(BPolyn<BDat>& pol, BInt period);
 ~BPolStationaryFunction() {}

  BDat GetValue();
  void Evaluate(BDat& y, const BArray<BDat>& x);
  void GetGradient (BArray<BDat>& G);

};


//--------------------------------------------------------------------
class BModel

/*! ARIMAX stochastic models implementation
 */
//--------------------------------------------------------------------
{
public: //member data
  static BArray<BText> testName_;
  static BArray<BText> testTitle_;
  static BArray<BText> qualifTitle_;
  BInt		  iterationNumber_;
  BDat		  initialError_;
  BBool		  aborted_;
  BBool		  initValues_;
  BBool		  recalcInterruptions_;
  BBool		  recalculateDifNoise_;
  BArray<BDat>	  testAccept_;
  BArray<BDat>	  testRefuse_;
  BArray<BDat>	  diagValue_;
  BArray<BDat>	  diagPunct_;
  BArray<BInt>	  diagQualify_;
  BInt		  qualification_;
  BDat		  diagnosysLogPrior_;
  BReal		  firstTransformation_;	 // additive BOX-COX transformation
  BReal		  secondTransformation_; // exponential BOX-COX transformation
  BInt		  regularDifference_;	 // regular difference exponent
  BInt		  seasonalDifference_;	 // seasonal difference exponent
  BDat		  standardError_;	 // standard desviation of residuous
  BText		  resName_;		 // residuous serie's name of model
  BArray<BText>	  inpName_;		 // input serie's names of model
  BArray<BPol>	  transfer_;		 // input's transfer functions.
  BArray<BInt>	  eliminated_;		 // eliminated variables
  BARIMA arma_;

  BArray<BPol>	  arFactors_;		 // AutoRegressive polinomials
  BArray<BPol>	  maFactors_;		 // Moving-Average polinomials
  BArray<BPol*>	  armaFactors_;		 // ARMA polinomials directions
  BArray<BInt>	  armaIndex_;		 // ARMA polinomials periodicitys
  BArray<BInt>	  armaPeriod_;		 // ARMA polinomials periodicitys
  BArray<BDat>	  armaLagrangeMult_;
  BArray<BDat>	  armaStationarity_;
  BPol		  dif_;			 // difference polinomial
  BPol		  ar_;			 // auto-regressive polynomial
  BPol		  ari_;			 // auto-regressive integrated
  BPol		  ma_;			 // moving-average polynomial
  BPol		  piw_;			 // PI-Weights
  BPol		  psiw_;		 // PSI-Weights
  BDat		  constant_;		 // constant of model
  BDat		  teta1_;
  BDat		  fi1_;

  BUserTimeSerie* output_;		 // output serie of model
  BArray <BInt>	  interruptionIndex_;	 // interruption location
  BArray <BDat>	  interruptionValue_;	 // interruption values
  BMatrix<BDat>	  PIW_;
  BMatrix<BDat>	  MPIW_;
  BMatrix<BDat>	  MPSIW_;
  BMatrix<BDat>	  ILtLiLt_;

  BTsrPrimary*	  res_;			 // output serie of model
  BArray <BDat>	  residuous_;
  BArray <BText>  paramAPrioriName_;
  BArray <BInt>	  paramAPrioriOrder_;
  BArray <BDat>	  paramAPrioriNu_;
  BArray <BDat>	  paramAPrioriSigma_;
  BArray <BDat>	  paramAPrioriResiduals_;
  BArray <BDat>	  paramSD_;
  BMatrix<BDat>	  paramLi_;
  BMatrix<BDat>	  paramL_;

  BBool        	   calculatingJacobian_;
  BMatrix<BDat>	   jacobian_;
  BSymMatrix<BDat> informationMatrix_;
  BDiagMatrix<BDat> D_;
  BMatrix<BDat> U_,V_,kernel_;
  BSymMatrix<BDat> paramCov_;
  BSymMatrix<BDat> paramCor_;

  BMatrix<BDat>	  resACor_;
  BMatrix<BDat>	  resPACor_;

  BText		  outName_;		 // output serie's name of model
  BDate		  firstDate_;		 // firstDate of model
  BDate		  lastDate_;		 // firstDate of model
  BDate		  firstForInput_;
  BDate		  lastForInput_;
  BInt		  periodicity_;		 // periodicity of dating

  BInt		  numData_;		 // periodicity of dating
  BArray<BDat>	  autoCov_;
  BArray<BDat>	  inputFilter_;
  BArray<BDat>	  systemInputFilter_;
  BArray<BDat>	  inputDifFilter_;
  BArray<BDat>	  noise_;
  BArray<BDat>	  difNoise_;
  BSerieTable	    inData_;
  BSerieTable	    outData_;
  BMatrix<BDat>	  effectData_;
  BMatrix<BDat>	  inDifData_;
  BMatrix<BDat>	  outDifData_;
  BArray<BDat>	  transOutData_;
  BArray<BDat>   fullData_;
  BMatrix<BDat>	 A0_;
  BMatrix<BDat>	 Z0_;
  BMatrix<BDat>	 A_;
  BMatrix<BDat>	 Z_;

  BInt		  arParam_;
  BInt		  maParam_;
  BInt		  lagrangianParam_;
  BInt		  systemInputParam_;
  BInt		  inputParam_;
  BInt		  numParam_;
  BArray<BText>	  infoParam_;
  BArray<BDat>	  param_;

  BArray<BDat>	  Fi_;
  BArray<BDat>	  DFi_;
  BArray<BDat>	  Teta_;
  BInt		  maxTransferDegree_;
  BInt		  difDeg_;
  BInt		  N_;
  BInt		  p_;
  BInt		  q_;
  BInt		  aCorNum_;
  BInt		  maxpq_;
  BDat		  varianze_;
  BDat		  SwartzInfo_;
  BDat		  RegBoxPierceLjung_;
  BDat		  SeaBoxPierceLjung_;
  BDat		  RegBoxPierceMod_;
  BDat		  SeaBoxPierceMod_;
  
  BDat    maxLikelihoodCoef_;

  BUserCode*	  nonLinFun_;
  BSet		  nonLinPar_;
  BArray<BDat>	  inDifNonLin_;

public:
   BModel(BUserTimeSerie*       output,
          BList*                inputSeries,
          const BPol&           dif,
          const BArray<BPol>&   arRoots,
          const BArray<BPol>&   maRoots,
          const BDate&          first       = BDate::Begin(),
          const BDate&          last        = BDate::End(),
          BReal                 firstTrans  = 0,
          BReal                 secondTrans = 0,
          BReal                 constant    = 0                );

//BModel(BUserSet* constructParam, const BText& foreFile);
  BModel(const BText& outName, const BText& foreFile, BInt period);

  BModel(BSet& set);

 ~BModel()
  {
    if(DI_)
    delete []DI_;
  }

  BList*	 GetDefinitionList();
  BSyntaxObject* GetDefinition();
  BSyntaxObject* OneParamInfo(BText name,
			      BDat  value,
			      BReal factor,
			      BReal order,
			      BInt  n);
  BSyntaxObject* ParamInfo();
  BSyntaxObject* OneParamDiag(BInt n);
  BSyntaxObject* ParamDiag();
  void	CopyInfToSet(BSet& set);
  void	ConstructARMAFromRoots();
  BBool ReadForecast(const BText& forecastFile);

  BDat Transfor	  (BDat x) const;
  BDat InvTransfor(BDat x) const;

  void	AddRestriction(BInt& s, BInt& r, BInt i);
  void	GetParameter();
  void	PutParameter(const BArray<BDat>& p);
  void	CalcInputFilter(BInt N);
  void	CalcInputDifFilter(BInt N);
  void	CalcNonLinnearInputDifFilter(BInt N);
  void	CalcDifNoise(BInt N);
  BBool InitData(const BDate& f, const BDate& l, BInt numPrev=0);
  void	GetInfoParam();
  BBool CheckData();

//BDat	CalcRestrictionPenalization();
  void	CalcResiduous		   (BArray<BDat>&  res, int method=1);
  void	CleanInterruptions	   (BMatrix<BDat>& A, BBool recalculate);
  void	CleanInterruptions();
  void	Evaluate(BArray<BDat>&res, const BArray<BDat>& p);

  void	CalcNumPartDer	    (const BArray<BDat>&  x,
			     const BArray<BDat>&  y,
			     BArray<BDat>&	  Jn,
			     BInt r,
			     BInt n,
			     BBool initValues,
			     BBool recalcInterruptions);
  void	CalcNumericalJacobian(const BArray<BDat>&  x,
			      const BArray<BDat>&  y,
			      BMatrix<BDat>&	   j);

  void	CalcAnaliticalJacobian(const BArray<BDat>&  x,
			       const BArray<BDat>&  y,
			       BMatrix<BDat>&	    j);

  void  IntegrateMissingValues();
  void	Statistics	     ();
  void	Diagnostics	     ();

  BTsrPrimary* TransPrev(const BDate&	first,
			 const BDate&	last,
			 BInt		num,
			 BMatrix<BDat>& aCov);

  void PrevisionSeries(const BText&	      serieName,
		       const BUserTimeSerie*  transPrev,
		       const BMatrix<BDat>&   aCov,
			     BUserTimeSerie** prev,
			     BUserTimeSerie** transError,
			     BUserTimeSerie** error,
			     BUserTimeSerie** upperBand,
			     BUserTimeSerie** lowerBand);

private: //Battery of tests

  BInt Qualify(BInt n, BDat x, BBool necessary);
  BInt FstRegAutCorTest		   (BInt n);
  BInt FstSeaAutCorTest		   (BInt n);
  BInt SndRegAutCorTest		   (BInt n);
  BInt SndSeaAutCorTest		   (BInt n);
  BInt RegBoxPierceLjungTest (BInt n);
  BInt SeaBoxPierceLjungTest (BInt n);
  BInt RegBoxPierceModTest   (BInt n);
  BInt SeaBoxPierceModTest   (BInt n);
  BInt KullbackLeiblerDistanceTest (BInt n);
  BInt PearsonNormalityTest  (BInt n);
  BInt MinSignificationTest	 (BInt n);
  BInt MaxCorrelationTest	   (BInt n);
  BInt MixedSignCorrTest	   (BInt n);
  BInt UnitRootsProbTest	   (BInt n);
  BInt RunTest			   (BInt n);

private: //file's reader functions
 
  BBool ReadBlocks	   (const BText& forecastFile);
  BBool ReadOutName	   ();
  BBool ReadVariables	   ();
  BBool ReadTransformation ();
  BBool ReadDifference	   ();
  BBool ReadPolynomials	   ();
  BBool ReadStandardError  ();

private: // private data members

  double * DI_;
};


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define FORZELINKERMODEL \
BBool ForzeLinkerModel(); \
static BBool forzeLinkerModel_ = ForzeLinkerModel();

#endif // TOL_BMODEL_H
