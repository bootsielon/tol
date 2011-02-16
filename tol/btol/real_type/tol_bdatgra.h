/* tol_bdatgra.h: Classes and functions for real numbers grammar
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

#ifndef TOL_BDATGRA_H
#define TOL_BDATGRA_H 1

#include <tol/tol_bdate.h>
#include <tol/tol_bgencon.h>

#ifdef __USE_TC__
#  include <tol/tol_bctime.h>
#endif

class BDat;

//#define __USE_SHORT_LOGIC__

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserDat     BGraContensBase < BDat >
#define BContensDat  BGraContens     < BDat >
#define BSystemDat   BGraConstant    < BDat >
#define BParamDat    BGraParameter   < BDat >


template class TOL_API BGraContensBase< BDat >;
template class TOL_API BGraContens    < BDat >;
//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Dat);
inline BReal Real(BSyntaxObject* arg) { return(Dat(arg).Value()); }
inline BReal Real(BCore*	 arg) { return(Dat(arg).Value()); }
inline BReal Real(const BText&	 arg) { return(Dat(arg).Value()); }


//--------------------------------------------------------------------
class BDatTemporary: public BTmpContens<BDat>

/*! Base class to generate temporary objects of class BDat.
 */
//--------------------------------------------------------------------
{
 public:
    // Constructors and destructors: inline
    BDatTemporary(BList* arg) : BTmpContens<BDat>(arg) { }
    ~BDatTemporary() { }
 protected:
#ifndef __USE_SHORT_LOGIC__
//BDat LogicShort(BBool(*comp)(const BDat&,const BDat&))const;
  BDat LogicShort(BDat (*comp)(const BDat&,const BDat&))const;
#endif
};


//--------------------------------------------------------------------
class BDatStatistic: public BDatTemporary

/*! Base class to generate temporary objects of class BDat.
 */
//--------------------------------------------------------------------
{
 protected:
    static BList* instances_;
    BSyntaxObject* ser_;
    BDate	   ini_;
    BDate	   fin_;
    BData    vec_;
 public:
   // Constructors and destructors: inline
   BDatStatistic(BList* arg);
   void	PutArgs(BSyntaxObject* ser, const BDate& ini, const BDate& fin);
};

//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

#define DeclareStatisticClass(CLASS)	      \
class CLASS : public  BDatStatistic	      \
{					      \
public:					      \
  CLASS(BList* arg) : BDatStatistic(arg) { }  \
 ~CLASS() { }				      \
  void CalcContens();			      \
};					      \
DeclareEvaluator(CLASS)

DeclareStatisticClass(BDatStatFirst);
DeclareStatisticClass(BDatStatLast);
DeclareStatisticClass(BDatStatCount);
DeclareStatisticClass(BDatStatMax);
DeclareStatisticClass(BDatStatMin);
DeclareStatisticClass(BDatStatMedian);
DeclareStatisticClass(BDatStatQuantile);
DeclareStatisticClass(BDatStatSum);
DeclareStatisticClass(BDatStatAverage);
DeclareStatisticClass(BDatStatVarianze);
DeclareStatisticClass(BDatStatStDs);
DeclareStatisticClass(BDatStatSubSampleAverage);
DeclareStatisticClass(BDatStatSubSampleVarianze);
DeclareStatisticClass(BDatStatSubSampleStDs);
DeclareStatisticClass(BDatStatAsimetry);
DeclareStatisticClass(BDatStatKurtosis);
DeclareStatisticClass(BDatStatMoment);
DeclareStatisticClass(BDatStatCenterMoment);


#ifdef __USE_TC__
//--------------------------------------------------------------------
/*! Base class to generate temporary objects of class BDat.
 */
class BDatCStatistic: public BDatTemporary
{
 protected:
    static BList* instances_;
    BSyntaxObject* cser_;
    BCTime	   ini_;
    BCTime	   fin_;
    BData      vec_;
 public:
    // Constructors and destructors: inline
    BDatCStatistic(BList* arg);
    void PutArgs(BSyntaxObject* cser, const BCTime& ini, const BCTime& fin);
};

#define DeclareCStatisticClass(CLASS)         \
class CLASS : public  BDatCStatistic	      \
{					      \
public:					      \
  CLASS(BList* arg) : BDatCStatistic(arg) { } \
 ~CLASS() { }				      \
  void CalcContens();			      \
};					      \
DeclareEvaluator(CLASS)

DeclareCStatisticClass(BDatCStatFirst);
DeclareCStatisticClass(BDatCStatLast);
DeclareCStatisticClass(BDatCStatCount);
DeclareCStatisticClass(BDatCStatMax);
DeclareCStatisticClass(BDatCStatMin);
DeclareCStatisticClass(BDatCStatMedian);
DeclareCStatisticClass(BDatCStatQuantile);
DeclareCStatisticClass(BDatCStatSum);
DeclareCStatisticClass(BDatCStatAverage);
DeclareCStatisticClass(BDatCStatVarianze);
DeclareCStatisticClass(BDatCStatStDs);
DeclareCStatisticClass(BDatCStatSubSampleAverage);
DeclareCStatisticClass(BDatCStatSubSampleVarianze);
DeclareCStatisticClass(BDatCStatSubSampleStDs);
DeclareCStatisticClass(BDatCStatAsimetry);
DeclareCStatisticClass(BDatCStatKurtosis);
DeclareCStatisticClass(BDatCStatMoment);
DeclareCStatisticClass(BDatCStatCenterMoment);
//--------------------------------------------------------------------
#endif // __USE_TC__


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

TOL_API BDat StatFirst (BSyntaxObject* ser);
TOL_API BDat StatLast (BSyntaxObject* ser);
TOL_API BDat StatCount (BSyntaxObject* ser);
TOL_API BDat StatMax (BSyntaxObject* ser);
TOL_API BDat StatMin (BSyntaxObject* ser);
TOL_API BDat StatSum (BSyntaxObject* ser);
TOL_API BDat StatAverage (BSyntaxObject* ser);
TOL_API BDat StatStDs (BSyntaxObject* ser);
TOL_API BDat StatVarianze (BSyntaxObject* ser);
TOL_API BDat StatAsimetry (BSyntaxObject* ser);
TOL_API BDat StatKurtosis (BSyntaxObject* ser);
TOL_API BDat StatMedian (BSyntaxObject* ser);
TOL_API BDat StatMoment (BSyntaxObject* ser, BInt order);
TOL_API BDat StatCenterMoment (BSyntaxObject* ser, BInt order);
TOL_API BDat StatQuantile (BSyntaxObject* ser, BDat q);

#ifdef __USE_TC__
TOL_API BDat CStatFirst (BSyntaxObject* cser);
TOL_API BDat CStatLast (BSyntaxObject* cser);
TOL_API BDat CStatCount (BSyntaxObject* cser);
TOL_API BDat CStatMax (BSyntaxObject* cser);
TOL_API BDat CStatMin (BSyntaxObject* cser);
TOL_API BDat CStatSum (BSyntaxObject* cser);
TOL_API BDat CStatAverage (BSyntaxObject* cser);
TOL_API BDat CStatStDs (BSyntaxObject* cser);
TOL_API BDat CStatVarianze (BSyntaxObject* cser);
TOL_API BDat CStatAsimetry (BSyntaxObject* cser);
TOL_API BDat CStatKurtosis (BSyntaxObject* cser);
TOL_API BDat CStatMedian (BSyntaxObject* cser);
TOL_API BDat CStatMoment (BSyntaxObject* cser, int order);
TOL_API BDat CStatCenterMoment (BSyntaxObject* cser, int order);
TOL_API BDat CStatQuantile (BSyntaxObject* cser, BDat q);
#endif // __USE_TC__

#endif // TOL_BDATGRA_H

