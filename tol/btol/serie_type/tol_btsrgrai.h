/* tol_btsrgrai.h: Internal operators for time serie grammar.
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

#ifndef TOL_BTSRGRAI_H
#define TOL_BTSRGRAI_H 1 

#include <tol/tol_btsrgra.h>

//--------------------------------------------------------------------
class BTsrInternal: public BTsrTemporary

/*! Base class for each operator which result and arguments
 *  are all time series.
 */
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BTsrInternal(BList* arg);
 ~BTsrInternal() {}
};


#define DeclareInternalBTsrClass(BTSRCLASS) \
class BTSRCLASS: public BTsrInternal {      \
  public:				    \
    BTSRCLASS(BList* arg)		    \
    : BTsrInternal(arg)  { }		    \
    ~BTSRCLASS() {}			    \
    BDat GetDat(const BDate& dte);	    \
  DeclareClassNewDelete(BTSRCLASS); \
};					    \
DeclareEvaluator(BTSRCLASS)

DeclareInternalBTsrClass(BTsrIdentity);
DeclareInternalBTsrClass(BTsrDifference);


#endif // TOL_BTSRGRAI_H
