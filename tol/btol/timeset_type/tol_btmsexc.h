/* tol_btmsexc.h: Classes and functions for time set grammar.
                  GNU/TOL Language.

   Copyright (C) 2004 - Bayes Decision, SL (Spain [EU])
   Author: Tol Development Team

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

#ifndef TOL_BTMSEXC_H
#define TOL_BTMSEXC_H 1


#include <iostream>
//#include <cstdlib>

class TimeSetException {
 public:
    const std::string enReason_; // What caused the error, in english
    const std::string esReason_; // What caused the error, in spanish

    // Initializing the error with a messages
    TimeSetException(const std::string& enMsg, const std::string& esMsg)
	:enReason_(enMsg), esReason_(esMsg) {};
};

#endif /* TOL_BTMSEXC_H */
