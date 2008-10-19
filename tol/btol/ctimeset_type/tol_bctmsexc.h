/* tol_bctmsexc.h - TOL Language. Class for BCTimeSet Exception Handling
 *
 *   Copyright (C) 2004 - Bayes Decision, SL
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *   USA.
 */

#ifndef TOL_BCTMSEXC_H
#define TOL_BCTMSEXC_H 1

#include <tol/tol_cbtol.h>

#if defined(__USE_TC__)

//--------------------------------------------------------------------
/*! Class to represent a bound exception in operations with BCTime
 * objects. Its only attribute is a string indicating the cause of
 * the error.
 */
class TOL_API BEmptyCTimeSetException : public BException {
 public:
    // Initializing the error with a messages
    BEmptyCTimeSetException(const std::string& enMsg, 
			    const std::string& esMsg)
	: BException(enMsg, esMsg)
	{}
};

#endif /* __USE_TC__ */
#endif /* TOL_BCDTEEXC_H */
