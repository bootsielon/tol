/* tol_blanguag.h: Initialization of all grammatic operators.
                   GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BLANGUAG_H
#define TOL_BLANGUAG_H 1

#include <tol/tol_bcommon.h>
#include <tol/tol_blist.h>

class BText;
class BDate;
class BSyntaxObject;
class BGrammar;
class BStruct;

//--------------------------------------------------------------------
enum tolType { 
    TT_ANYTHING=1, TT_CSERIES, TT_CTIME,  TT_CTIMESET, TT_CODE,
    TT_COMPLEX,    TT_DATE,    TT_MATRIX, TT_POLYN,    TT_RATIO,
    TT_REAL,       TT_SERIE,   TT_SET,    TT_TEXT,     TT_TIMESET,
    TT_NAMEBLOCK
};
typedef enum tolType TolType;

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

TOL_API BGrammar* GraAnything();
TOL_API BGrammar* GraCode();
TOL_API BGrammar* GraDate();
TOL_API BGrammar* GraPolyn();
TOL_API BGrammar* GraRatio();
TOL_API BGrammar* GraReal();
TOL_API BGrammar* GraComplex();
TOL_API BGrammar* GraMatrix();
TOL_API BGrammar* GraVMatrix();
TOL_API BGrammar* GraSet();
TOL_API BGrammar* GraNameBlock();
TOL_API BGrammar* GraText();
TOL_API BGrammar* GraTimeSet();
TOL_API BGrammar* GraSerie();
TOL_API BGrammar* GraPolMat();

TOL_API BStruct* RefAnything();
TOL_API BStruct* RefCode();
TOL_API BStruct* RefDate();
TOL_API BStruct* RefPolyn();
TOL_API BStruct* RefRatio();
TOL_API BStruct* RefReal();
TOL_API BStruct* RefComplex();
TOL_API BStruct* RefMatrix();
TOL_API BStruct* RefVMatrix();
TOL_API BStruct* RefSet();
TOL_API BStruct* RefNameBlock();
TOL_API BStruct* RefText();
TOL_API BStruct* RefTimeSet();
TOL_API BStruct* RefSerie();

bool InitRefStructs();

TOL_API int  AliveObjects();
TOL_API void TestAliveObjects(int before, int after, const BText& step);
TOL_API void InteractiveTOL();

TOL_API void StopFlagOn();
TOL_API void StopFlagOff();
TOL_API bool StopFlag();

//--------------------------------------------------------------------
class TOL_API BPackage
//--------------------------------------------------------------------
{
private:
  static BText help_;
  static BText localRoot_;
  static BList* required_;
public:
  static BText LocalPath(const BText& package);
  static bool Install(const BText& package);
  static BSyntaxObject* Load(const BText& package, bool retry=false);
  static BList* Required() { return(required_); }
};

BDate TsrFirstDate(BSyntaxObject* obj);
BDate TsrLastDate (BSyntaxObject* obj);

#endif // TOL_BLANGUAG_H
