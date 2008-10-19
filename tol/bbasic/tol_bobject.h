/* tol_bobject.h: Declarations of BObject Class attributes and methods
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

#ifndef TOL_BOBJECT_H
#define TOL_BOBJECT_H 1

#include <tol/tol_batom.h>
#include <tol/tol_btext.h>

//--------------------------------------------------------------------
class TOL_API BObject: public BAtom

/*! Minimum element of hierarchicaly classes organization with name.
 */
//--------------------------------------------------------------------
{
protected:
  BText name_; //!< specify name of this hierarchicaly minimum object

public:
  // Constructors and destructors: inline
  //! Create a new BObject with \a name_ attribute uninitialized
  BObject();
  //! Create a new BObject with \a name_ attribute equals to \a name argument
  BObject(const BChar* name);
  //! Create a new BObject with \a name_ attribute equals to \a name argument
  BObject(const BText& name);
  //! Destroy a BObject
  ~BObject();

  // Access and manipulation: inline
  //! Return name of this BObject like a BChar 
  const BChar* String() const;
  //! Return READ ONLY name of this BObject like a BText
  const BText& Name() const;
  //! Modify value of \a name_ attribute with BChar argument
  void PutName(const BChar* name);
  //! Modify value of \a name_ attribute with BText argument
  void PutName(const BText& name);

};

//--------------------------------------------------------------------
class TOL_API BObjectCRef: public BAtom

/*! False objects usefull for searching in sorted arrays.
 */
//--------------------------------------------------------------------
{
protected:
  const BText& name_; //!< specify name of this hierarchicaly minimum object

public:
  // Constructors and destructors: inline
  //! Create a new BObjectCREf with \a name_ attribute equals to \a name argument
  BObjectCRef(const BText& name) : name_(name) {}
  //! Destroy a BObjectCRef
  ~BObjectCRef() {}

  // Access and manipulation: inline
  //! Return name of this BObject like a BChar 
  const BChar* String() const { return(name_.String()); }
  //! Return READ ONLY name of this BObject like a BText
  const BText& Name() const { return(name_); }
  //! Modify value of \a name_ attribute with BChar argument

};

#endif // BOBJECT
