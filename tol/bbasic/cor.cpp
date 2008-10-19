/* BCore: Global variables, constructors, destructors and functions
          for GNU/TOL Language.

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

#include <tol/tol_bcore.h>

BTraceInit("cor.cpp");

/*! Static global class variable for BCore list of instances.
 */
BInt BCore::nCore_ = 0;


/*! Constructor. Increments the core counter.
*/
BCore::BCore()
{
    nCore_++;
}


/*! Destructor. Decrements the core counter.
 */
BCore::~BCore()
{
    nCore_--;
  //printf("BCore::~BCore() nCore_=%ld",nCore_);
}


/*! Returns the actual number of core objects.
*/
BInt BCore::NCore()
{
    return(nCore_);
}
