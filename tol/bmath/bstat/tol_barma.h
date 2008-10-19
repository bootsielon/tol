/* tol_barma.h: Auto-Regressive identification and estimation.
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

#ifndef TOL_BARMA_H
#define TOL_BARMA_H 1

#include <tol/tol_bratio.h>
#include <tol/tol_bsparse.h>
#include <tol/tol_bdiagmat.h>
#include <tol/tol_bsymmat.h>

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

void MatBackwardDifEq  (const BRational <BDat>& R,
			const BMatrix   <BDat>& X,
			BMatrix   <BDat>& Y);

void MatBackwardDifEq  (const BRational <BDat>& R,
			const BMatrix   <BDat>& X0,
			const BMatrix   <BDat>& X,
			const BMatrix   <BDat>& Y0,
			BMatrix   <BDat>& Y);

void MatForwardDifEq   (const BRational <BDat>& R,
			const BMatrix   <BDat>& X,
			BMatrix   <BDat>& Y);

void MatForwardDifEq   (const BRational <BDat>& R,
			const BMatrix   <BDat>& X0,
			const BMatrix   <BDat>& X,
			const BMatrix   <BDat>& Y0,
			BMatrix   <BDat>& Y);

void ForecastingToNull(const BRational <BDat>& R,
		       const BMatrix   <BDat>& Z,
		       BMatrix   <BDat>& A,
		       BMatrix   <BDat>& ZF);

void ARMABackastingHist(const BRational <BDat>& R,
			BMatrix   <BDat>& A0A,
			BMatrix   <BDat>& A0,
			BMatrix   <BDat>& A,
			BMatrix   <BDat>& Z0,
			const BMatrix   <BDat>& Z,
			BInt              refination);

void ARMABackasting    (const BRational <BDat>& R,
			BMatrix   <BDat>& X0,
			BMatrix   <BDat>& X,
			BMatrix   <BDat>& Y0,
			const BMatrix   <BDat>& Y);


BMatrix<BDat> Pol2Mat(BInt r, BInt c, const BPolyn<BDat> pol);

bool ARMAAutoCovarianzeVector(BArray<BDat>& gn,
                              const BPolyn<BDat>& ar,
                              const BPolyn<BDat>& ma,
                              int n0, BDat sigma);

BPolyn<BDat> ARMAACovGamma(const BPolyn<BDat>& ar,
                           const BPolyn<BDat>& ma,
                           int n);

BSymMatrix<BDat> ARMAAutoCovarianze(const BPolyn<BDat> ar,
				    const BPolyn<BDat> ma,
				    BInt n, 
				    BDat sigma);

BBool YuleWalkerDurbin(const BArray<BDat>& r,
                       BArray<BDat>& y,
                       BArray<BDat>& PACF,
                       BDat & logDet,
                       int period);

int Levinson(const BArray<BDat>& r,
             const BArray<BDat>& c,
             BArray<BDat>& x,
             BArray<BDat>& y,
             BArray<BDat>& PACF,
             BDat        & logDet,
             int           period);

//--------------------------------------------------------------------
class BARIMAFactor
//--------------------------------------------------------------------
{
public:
  BInt	       s_;
  BPolyn<BDat> dif_;
  BPolyn<BDat> ar_;
  BPolyn<BDat> ma_;

  BARIMAFactor() { SetOne(); }
 ~BARIMAFactor() {}

  void SetOne();
  BARIMAFactor& operator =(const BARIMAFactor& a);
  BARIMAFactor& operator*=(const BARIMAFactor& a);
  BText Name();
  BBool HasValue();
};

/*
#ifdef _MSC_VER
_INSTANTIATE_BARRAY_BARIMAFACTOR
#undef _INSTANTIATE_BARRAY_BARIMAFACTOR
#define _INSTANTIATE_BARRAY_BARIMAFACTOR

_INSTANTIATE_BMATRIX_BDAT
#undef _INSTANTIATE_BMATRIX_BDAT
#define _INSTANTIATE_BMATRIX_BDAT

_INSTANTIATE_BARRAY_BINTPAIR
#undef _INSTANTIATE_BARRAY_BINTPAIR
#define _INSTANTIATE_BARRAY_BINTPAIR
#endif
*/

//--------------------------------------------------------------------
class BARIMA
//--------------------------------------------------------------------
{
public:
  BArray<BARIMAFactor>	factor_;
  BArray<BARIMAFactor>	grFact_;
  BArray<BARIMAFactor>	acumFact_;
  BARIMAFactor		prod_;
  BInt			d_;
  BInt			p_;
  BInt			q_;
  BMatrix<BDat>		z_;
  BMatrix<BDat>		w_;
  BMatrix<BDat>		r_;
  BMatrix<BDat>		a_;
  BMatrix<BDat>		aCoviw_;
  BMatrix<BDat>		a0_;
  BMatrix<BDat>		w0_;
  BArray<BDat> aCov_;
  BArray<BDat> aCor_;
  BArray<BDat> aCorI_;
  BArray<BDat> paCor_;
  BDat aCovDetN_;
  BDat ata_;
  BDat wtCoviw_;
  BDat logLikelihood_;
  BDat likelihoodCoef_;
  BDat waCoef_;

  BARIMA() {};
 ~BARIMA() {};

  void PutFactors   (const BArray<BARIMAFactor>& factor);
  void PutOutputData(const BMatrix<BDat>& z);
  void OutputDataUpdated();
  void SetError();
  bool CheckStationary();
  bool CalcAutoCovarianze(BInt N);
  bool CalcAutoCorrelation();
  bool CalcLikelihood_Levinson(BDat sigma=1.0, BInt n=-1, BBool calcDet=BTRUE, BBool calcInitValues=BTRUE);
  bool CalcLikelihood_Almagro(BDat sigma=1.0);
  bool CalcLikelihood(BDat sigma=1.0);
};


#endif	//BARMA
