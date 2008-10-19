/* tol_armseval.h: definition of arms evaluation of
                   GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
   Author: Jorge Suit Perez Ronda <jsperez@bayesinf.com>

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
  \file   armseval.h
  \author Jorge Suit Perez Ronda
  \date   Mon Mar 31 13:36:46 2003
  
  \brief  
*/

#include <tol/tol_bdatgra.h>
#include <tol/tol_bcode.h>

typedef struct {
  int uerror;
  double last_res;
  BCode * ucode;
  BUserDat udat;
  BDat * dat;
  BGraContens<BSet> * uset;
} ARMS_DATA;

/*!
 *  Alloc the data needed to do the arms evaluations. Receive the user
 *  code implementing the log evaluation.
 */
ARMS_DATA * ArmsAlloc(BCode * cod, BGraContens<BSet> * uset);


/*!
 *  Release a previously (ARMS)allocated data.
 */
void ArmsRelease(ARMS_DATA * data);


/*!
 *  Do the evaluation of the user code specified in data.
 */
double ArmsCodeEval(double x, void * data);
