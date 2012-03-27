/* tol_bpolgra.h: Classes and functions for polynomial grammars
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

#ifndef TOL_BPOLGRA_H
#define TOL_BPOLGRA_H 1

#include <tol/tol_bpolyn.h>

//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
#define BPol	        BPolyn	         < BDat >
#define BUserPol      BGraContensBase  < BPol >
#define BContensPol   BGraContens      < BPol >
#define BSystemPol    BGraConstant     < BPol >
#define BPolTemporary BTmpContens      < BPol >


template class TOL_API BArray          < BMonome < BDat > >;
template class TOL_API BPolyn          < BDat >;
template class TOL_API BGraContensBase < BPol >;
template class TOL_API BGraContens     < BPol >;

TOL_API BPol EvalPolyn(const BText& expr, const BPol&  defVal); 
TOL_API BPol RandStationary(BInt degree, BInt period);


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
CastingsDeclaration(Pol);

#endif // TOL_BPOLGRA_H

