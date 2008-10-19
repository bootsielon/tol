/* tol_bexc.h - TOL Language. Class for Basic Exception Handling
 *
 *   Copyright (C) 2005 - Bayes Decision, SL
 *   Author: Daniel Rus <danirus@tol-project.org>
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

#ifndef TOL_BEXC_H
#define TOL_BEXC_H 1

#include <tol/tol_bcommon.h>


//--------------------------------------------------------------------
/*! Class to represent a bound exception in operations with BTime
 * objects. Its only attribute is a string indicating the cause of
 * the error.
 */
class BException 
{
 public:
    std::string enReason_;   // What caused the error, in english
    std::string esReason_;   // What caused the error, in spanish

    //! Initializing the error with a messages
    BException(const std::string& enMsg, const std::string& esMsg)
	:enReason_(enMsg), esReason_(esMsg)
	{}

    //! Initialize the bound error with a message
    std::string enReason() const { return enReason_; }
    std::string esReason() const { return esReason_; }

    // Assignment operator default
    // BException(BTimeBoundException) -> def copy const
    // ~BException -> default destructor
};

#endif /* TOL_BEXC_H */
