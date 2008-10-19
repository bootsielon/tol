/* tol_list.h: Declaration of internal List of GNU/TOL language.

   Copyright (C) 2003 
                 European Free Software Foundation, Inc.

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

/*! \brief Defining List basic class, its attributes and methods prototypes
 *  \brief funcionality has been implemented in list.cpp
 */

#ifndef TOL_LIST_H
#define TOL_LIST_H

#include <tol/tol_blist.h>


//--------------------------------------------------------------------
class TOL_API List: public BCore 
//--------------------------------------------------------------------
{
  private:
    BCore* car_; //!< Left pointer, usually a contents
    List*  cdr_; //!< Right pointer, usually the rest of the list
 
  public:
    //  Constructors and destructors: lst.cpp
    List(); //!< Create a new List  
    List(BCore*, List*); //!< Create a new List with indicated values
   ~List(); //!< Destroy a List

    BBool empty() const;
    static List* cons (BCore* A, List* B);
    static List* nCons (BCore* A);
    List* destroyFirstCons ();

    //! Destroys all the elements of a list. Inherited from BCore
    BBool Destroy(); 

    //! Indicates that it is a list, always gives back certain
    BBool IsListClass() const;
    //! Indicates if the parameters \a c is a list
    inline BBool IsList () const 
    {
      return(this && (this->IsListClass()));
    }

    BCore* car () const; //!< return \a car_ attribute
    List* cdr() const; //!< return \a cdr_ attribute
    
    //! Establishes or changes the value of the attribute \a car_
    void setCar (BCore* c);
    //! Establishes or changes the value of the attribute \a cdr_ 
    void setCdr (List* c);

    List* getNthCdr (BInt n);
    BCore* getNthCar (BInt n); 
    BBool insert (BCore*, BInt);
    BBool append (BCore*);
    List* getLastCdr ();
    BInt length () const;
    List* duplicate () const;
    BCore* findByName (const BText&);
    BList* toBList (); // para mantener la compatibilidad con el ant BList

  DeclareClassNewDelete(List);
}; // end of List class declaration

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------

/*! PURPOSE: Fast list append. Only, \a lst be used when the new list is
 *           created from its begining.
 *
 *  EXAMPLES: Note that lstMatch and the auxiliar con are NIL initializes
 *            BList* lstMatch=NIL; BList* con=NIL; BAtom* atm;
 *  <pre>
 *    ForAll(BAtom, atm, lstMatch)
 *     { if(atm->Name().Match(match)) { LstFastAppend(lstMatch, con, atm); } }
 *    EndFor;
 *  </pre>  
 *  \param lst
 *  \param con
 *  \param atm
 *  \return
 */
#define LST_FAST_APPEND(lst,con,atm)				        \
{ List* _auxCon=NIL;						        \
  if(!lst) { lst=con=List::cons(NIL,NIL); }				\
  else	   { con->setCdr(_auxCon=List::cons(NIL,NIL)); con=_auxCon; }	\
  con->setCar(atm); }


#endif // BLIST
