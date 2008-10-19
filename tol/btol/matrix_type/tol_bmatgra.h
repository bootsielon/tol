/* tol_bmatgra.h: Classes and functions for matrix grammar.
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BMATGRA_H
#define TOL_BMATGRA_H 1

#include <tol/tol_bdate.h>
#include <tol/tol_bmatimp.h>
#include <tol/tol_bgencon.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BUserMat      BGraContensBase < BMat >
#define BContensMat   BGraContens     < BMat >
#define BSystemMat    BGraConstant    < BMat >
#define BMatTemporary BTmpContens     < BMat >


template class TOL_API BMatrix         < BDat >;
template class TOL_API BGraContensBase < BMat >;
template class TOL_API BGraContens     < BMat >;

CastingsDeclaration(Mat);

//--------------------------------------------------------------------
// extern functions
//--------------------------------------------------------------------
extern	void SerieFrequency  (BSyntaxObject* ser, int parts,
			      const BText& fName);
extern	void SerieAutoCor    (BSyntaxObject* ser, int lag,
			      const BText& fName);
extern	void SeriePartAutoCor(BSyntaxObject* ser, int lag,
			      const BText& fName);
extern	void SerieInverseAutoCor(BSyntaxObject* ser, int lag,
				 const BText& fName);
extern	void SerieInvPartAutoCor(BSyntaxObject* ser, int lag,
				 const BText& fName);

#endif // TOL_BMATGRA_H
