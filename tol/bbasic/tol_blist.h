/* tol_blist.h: List implementation for internal use in
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

/*! \brief Defining BList basic class, its attributes and methods prototypes
//  \brief funcionality has been implemented in lst.cpp, lsttext.cpp and
//  \brief lstalgeb.cpp
*/

#ifndef TOL_BLIST_H
#define TOL_BLIST_H 1

#include <tol/tol_batom.h>
#include <tol/tol_barray.h>

//! Indicates if the parameters \a c is a list
inline BBool IsList(const BCore* c)
{ return(c && (c->IsListClass())); }


//--------------------------------------------------------------------
class TOL_API BList: public BCore

/*! Element for lists construction
 */
//--------------------------------------------------------------------
{
private:
  BCore* car_; //!< Left pointer, usually a contents
  BList* cdr_; //!< Right pointer, usually the rest of the list

public:
  //  Constructors and destructors: lst.cpp
  //! Create a new List  
  BList();
  //! Create a new List with indicated values
  BList(BCore*, BList*);
  //! Destroy a List
 ~BList();

  //! Create a new List with indicated values and reserved memory
  friend BList* Cons(BCore*, BList*);
  //
  friend BList* DestroyFirstCons(BList*);

public:
  // Access and manipulation: inline
  //! Destroys all the elements of a list 
  BBool	 Destroy();
  //! Indicates that it is a list, always gives back certain
  BBool	 IsListClass()const { return(BTRUE); }
  //! return \a car_ attribute
  BCore* Car()	 const { return(car_); }
  //! return \a cdr_ attribute
  BList* Cdr()	 const { return(cdr_); }

  //! Establishes or changes the value of the attribute \a car_
  void PutCar(BCore* c)
  {
    if(car_ && !IsList(car_)) { ((BAtom*)car_)->DecNRefs(); }
    car_=c;
    if(c && !IsList(c)) { ((BAtom*)c)->IncNRefs(); }
  }

  //! Establishes or changes the value of the attribute \a cdr_ 
  void PutCdr(BList* c) { cdr_=c; }

  DeclareClassNewDelete(BList);
};


//--------------------------------------------------------------------
// EXTERNAL FUNCTIONS
//--------------------------------------------------------------------
//! Destroy a element of the list
TOL_API BList* LstDestroyFirstCons(BList*);

//! Destroy all the elements of a list
TOL_API BList* LstDestroyAllCons(BList*);
//! Destroy all the elements of a list
TOL_API BList* LstDestroyAll(BList*);
//! Destoys all the consecutive elements of a list
TOL_API BList* LstDestroyUntil(BList*, BList*);
//! Remove all the atom aocurrences in the list
TOL_API BList* LstRemoveAtom(BList*, const BCore*);

//! Append the atom at the tail of the list 
TOL_API BList* LstAppend(BList*, BCore*);
//! Append a new atom to the list if it does not exit
TOL_API BList* LstAppendUnique(BList*, BCore*);
//! Makes a compactation of a list removing duplicated atoms
TOL_API BList* LstCompact(BList*);
//! Concat all atoms at the end of the list 
TOL_API BList* LstConcat(BList*, BList*);
//! Concat atoms at the end of the list, that its are not in the list 
TOL_API BList* LstConcatUnique(BList*, BList*);
//! Duplicates one list
TOL_API BList* LstDuplicate(const BList*);
//! Looks for an atom in the list
TOL_API BList* LstFind(BList*, const BCore*);
//! Inserts an atom in a list ina a position
TOL_API BList* LstInsert(BList*, BCore*, BInt);
//! Intersect the elements ot two list
TOL_API BList* LstIntersect(BList*, BList*);
//! Obtain the last \a cdr_ of the list
TOL_API BList* LstLastCdr(BList*);
//! Obtain length of a list
TOL_API BInt LstLength(const BList*);
//! Obtain one sublist from the n-th position of a list
BList* LstNthCdr(BList*, BInt);
//! Looks for an atom in the list with Name() equal than \a name
TOL_API BCore* LstFindByName(const BList*, const BText&);
//! Add a atom in a list and indicate if another one with the same name exists
TOL_API BList* LstAdd(BList*, const BAtom*, BBool& duplicate);
//! Put atoms of a list in a array
TOL_API void LstToArr(const BList*, BArray<BAtom*>&);
//! Put atoms of a array in a list
TOL_API BList* ArrToLst(BArray<BAtom*>&);
//! Orders atoms of a list
TOL_API BList* LstSort(const BList* lst,
			       BOrderCriterium order=AtmCmp);
//! Build a text with the names of atoms of a list
TOL_API BText LstText(const BList* lst,
                          const BText& openSign="(",
                          const BText& closeSign=")",
                          const BText& separatorSign=",");
//! Joint two lists, eliminating the duplicated atoms
TOL_API BList* LstJoint(const BList*, const BList*);
//! Adds alements at the end of a list
TOL_API BList* LstStack(BList* lst, ...);
//! Reverse a list
TOL_API BList* LstReverse(BList* lst);
//! Obtain a sublist of a list
TOL_API BList* LstSublist(BList* lst, BInt from=0, BInt to=0);
//! Obtain a sublist that its atom names make match with the text \a pattern
TOL_API BList* LstMatch(BList* lst, const BText& pattern);
//! Obtain the first atom that makes match with the text \a pattern
TOL_API BAtom* LstFirstMatch(BList* lst,
                                const BText& pattern,
                                BInt* position);
//! Obtain the difference between two lists
TOL_API BList* LstDifference(BList*, BList* );
//! Obtain the difference between two lits using the object names
TOL_API BList* LstDifferenceByName(BList*, BList* );


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------

//--------------------------------------------------------------------
/*! Fast list append. Only, \a lst be used when the new list is
 *  created from its begining.
 *
 *  EXAMPLES: Note that lstMatch and the auxiliar con are NIL initializes
 *            BList* lstMatch=NIL; BList* con=NIL; BAtom* atm;
 *  <pre>
 *    ForAll(BAtom, atm, lstMatch)
 *     { if(atm->Name().Match(match)) { LstFastAppend(lstMatch, con, atm); } }
 *    EndFor;
 *  </pre>  
 * \param lst
 * \param con
 * \param atm
 * \return
 */
//--------------------------------------------------------------------
#define LstFastAppend(lst,con,atm)				 \
{ BList* _auxCon=NIL;						 \
  if(!lst) { lst=con=Cons(NIL,NIL); }				 \
  else	   { con->PutCdr(_auxCon=Cons(NIL,NIL)); con=_auxCon; }	 \
  con->PutCar(atm); }


//--------------------------------------------------------------------
/*! Lists traversal macros. One crosses all the elements of a list
 *  and its remained with each atom of each list. A condition of
 *  stop can be put
 *
 *  EXAMPLES:
 *  <pre>
 *    ForAll(type,atom,list)
 *      { atom manipulation; ...; }
 *    EndFor
 *
 *    ForAll(type,atom,list)
 *      Until(condition)
 *	    { atom manipulation; ...; }
 *    EndFor </pre>
 *  \param type
 *  \param atm
 *  \param lst
 *  \return
 */
//--------------------------------------------------------------------
#define ForAll(type,atm,lst)\
{								     \
  const BList* _lst; BBool _ctrl=BTRUE; type* _atm;		     \
  for(_lst=lst, atm=_atm=(type*)Car(_lst); _lst && _ctrl;	     \
    atm=_atm=(_ctrl ? (type*)Car(Cdr(_lst)) : _atm), _lst=Cdr(_lst)) \
  {
#define EndFor }}

#ifndef Until
#  define Until(condition) if (condition) {_ctrl=BFALSE;} else
#endif // Until


//--------------------------------------------------------------------
// INLINE FUCTIONS
//--------------------------------------------------------------------
//! Create a new List reserving memory with indicates values 
inline BList* Cons (BCore* A, BList* B) { return(new BList(A,B)); }
//! Create a new list whit \a cdr_ attribute is NIL
inline BList* NCons(BCore* A) { return(Cons(A,NIL));	    }

//! Return a \a _car attribute of list \a l
inline BCore* Car(const BList* l)
  { return((l) ? ((BList*)l)->Car() : NIL) ; }
//! Return a \a _cdr attribute of list \a l
inline BList* Cdr(const BList* l)
  { return((l) ? (BList*) ((BList*)l)->Cdr() : NIL); }

//! Return a \a _car attibute from the position \a n of a list \a l 
inline BCore* LstNthCar(BList* l, BInt n)
  { return(Car(LstNthCdr(l,n))); }
//! Return the last \a _car atttibute of a list \a l
inline BCore* LstLastCar(BList* l)
  { return(Car(LstLastCdr(l))); }
//! Return the next \a _cdr attribute to atom \a c in list \a l 
inline BList* LstNextCdr(BList* l, BCore* c)
  { return(Cdr(LstFind(l,c))); }
//! Return the next \a _car attribute to atom \a c in list \a l 
inline BCore* LstNextCar(BList* l, BCore* c)
  { return(Car(LstNextCdr(l,c))); }
//! Return the following \a _cdr attribute to atom \a c in list \a l
inline BList* LstFollowingCdr(BList* lst, BCore* c)
  { return(Cdr(LstFind(lst,c))); }
//! Return the following \a _car attribute to atom \a c in list \a l  
inline BCore* LstFollowingCar(BList* lst, BCore* c)
  { return(Car(LstFollowingCdr(lst,c))); }

#endif // BLIST
