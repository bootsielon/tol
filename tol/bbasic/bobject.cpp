/* bobject.cpp: Declarations of BObject Class attributes and methods
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

/*! \brief Declarations of BObject Class attributes and methods
*/

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bobject.h>

BTraceInit("bobject.cpp");

/*! Create a new BObject with \a name_ attribute uninitialized
 */
BObject::BObject()
    : BAtom(), name_()
{
};

/*! Create a new BObject with \a name_ attribute 
 * equals to \a name argument
 */
BObject::BObject(const BChar* name)
    : BAtom(), name_(name)
{
};

/*! Create a new BObject with \a name_ attribute 
 * equals to \a name argument
 */
BObject::BObject(const BText& name)
    : BAtom(), name_(name)
{
};

/*! Destroy a BObject
 */
BObject::~BObject()
{
};

/*! Return name of this BObject like a BChar 
 */
const BChar*
BObject::String() const
{
    return (name_.String());
};

/*! Return name of this BObject like a BText
 */
const BText&
BObject::Name() const
{
    return(name_);
};

/*! Modify value of \a name_ attribute with BChar argument
 */
void
BObject::PutName(const BChar* name)
{
    name_=name;
};

/*! Modify value of \a name_ attribute with BText argument
 */
void
BObject::PutName(const BText& name)
{
    name_=name;
};
