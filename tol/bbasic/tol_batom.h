/* tol_batom.h: BAtom basic class, attributes and methods.
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

/*! \class BAtom
//  \brief Defining BAtom basic class, its attributes and methods prototypes
//  \brief functionality has been implemented in atm.cpp
*/ 

#ifndef TOL_BATOM_H
#define TOL_BATOM_H 1

#include <tol/tol_bcore.h>

class BText; /* Forward reference */

//--------------------------------------------------------------------
class TOL_API BAtom: public BCore

/*! Minimun element that will be inserted inside lists. BAtom is an
 *  abstract class (can't be instantiated).
 */
//--------------------------------------------------------------------
{
 private:
    BInt nRefs_; //!< Number of atom references

 public:
    // Constructors and destructors: inline
    //! Create a new BAtom 
    BAtom();
    //! Destroy a BAtom
    ~BAtom();
    
    // Access and manipulation: inline

    //! Indicates that not is a list 
    BBool IsListClass () const
	  {
	    return(BFALSE);
	  }
    virtual const BText& Name() const = 0;

    //! Indicate the name of the atom
    virtual	BText  Dump() const;
    //! Indicates the name of it self
    virtual	BBool  HasName() const;	 // atm.cpp
    //! Indicates the name of it self
    virtual bool IsDeadObj () const { return(false); }
    virtual	bool System    () const { return(true); }
    //! return \a Refs_ attribute
    BInt NRefs() const   { return(nRefs_); }
    //! \a nRefs increase attribute 
    virtual BInt IncNRefs() { return(nRefs_++); }
    //! \a nRefs decrease attribute
    virtual BInt DecNRefs() { return(nRefs_--); }
    
    // Implementation: atm.cpp
    //! Destroy it self 
    BBool Destroy();
    
    //! Indicates if the two atoms have the same name 
    static BBool IsEqualThan(const BAtom& atm1, const BAtom& atm2);
    //! Indicates if the name of an atom is minor who the one of another one
    static BBool IsMinorThan(const BAtom& atm1, const BAtom& atm2);
    //! Indicates if the name of an atom is greater who the one of  another one
    static BBool IsMayorThan(const BAtom& atm1, const BAtom& atm2);
    
};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

//! Concats a the text with the name of the atom 
BText& operator << (BText&,  const BAtom*);

//! Compare the name of two atoms
BInt AtmCmp (const void*, const void*);

//! Compare the name of two atoms
BInt AtmRCmp(const void*, const void*);



#endif // TOL_BATOM_H
