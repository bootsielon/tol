/* lst.cpp: BList constructors and destructor.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_blist.h>
#include <tol/tol_bcore.h>
#include <tol/tol_batom.h>
#include <tol/tol_bobject.h>
#include <tol/tol_bout.h>
#include <tol/tol_blist.h>

//static BInt cdrCount = 0;
BTraceInit("lst.cpp");

//--------------------------------------------------------------------
BList::BList()

/*! PURPOSE: Lists elements constructor. Inizializa the attributes to nil
*/
//--------------------------------------------------------------------
    : BCore(), car_(NIL), cdr_(NIL)
{
//  cdrCount++;
//  if(cdrCount%100==0) { printf("\nCdrCount = %ld\n", cdrCount); }
//printf("\nNULL list node created %ld", BCore::NCore());
}


//--------------------------------------------------------------------
BList::BList(BCore* a, BList* b): BCore(), car_(a), cdr_(b)

/*! Lists elements constructor. Inizialices the attributes with the
 *  values that go like parameters
 *           
 *  ALGORITHM: If a is an atom: Incremente a's references.
 * \param a value to assign \a car_
 * \param b value to assign \a cdr_
 */   
//--------------------------------------------------------------------
{
//  cdrCount++;
//  if(cdrCount%100==0) { printf("\nCdrCount = %ld\n", cdrCount);  }
    if(a && !IsList(a))  { ((BAtom*)a)->IncNRefs(); }
//printf("\nNew list node created %ld", BCore::NCore());
}


//--------------------------------------------------------------------
BList::~BList()

/*! Cons destructor
 * 
 *  ALGORITHM: If the car of the cons is an atom:
 *               Decremente the atom references.
 */   
//--------------------------------------------------------------------
{
//  cdrCount--;
//  if(cdrCount%100==0) { printf("\nCdrCount = %ld\n", cdrCount);   }
    if((car_) && !IsList(car_))  { ((BAtom*)car_)->DecNRefs(); }
    car_=NIL;cdr_=NIL;
}


//--------------------------------------------------------------------
BList* LstRemoveAtom(BList* lst, const BCore* atm)

/*! Remove all the atom ocurrences in the list \a lst.
 *  <pre>
 *  ALGORITHM:
 *    If \a car_ of the list is atom
 *      Destroy the first element
 *      Eliminate the atom of the rest of the list
 *    Else:
 *      Put like rest of the list the turn out to erase \a atm in the rest
 *      of the list </pre>
 * \param lst list of which element is removied
 * \param atm atom to eliminate
 * \return Return the list modified
 */     
//--------------------------------------------------------------------
{
    if(lst && IsList(lst) && atm) {
	if(Car(lst)==atm) { 
	    return(LstRemoveAtom(LstDestroyFirstCons(lst),atm)); 
	} else {
	    lst->PutCdr(LstRemoveAtom(Cdr(lst),atm));
	    return(lst);
	}
    } else { 
	return(lst); 
    }
}


//--------------------------------------------------------------------
BList* LstDestroyFirstCons(BList* lst)

/*! Destroys the first element of a list
 *  <pre> 
 *  ALGORITHM:
 *   If \ car_ of list it is an atom:
 *     Destroy the first element.
 *     To eliminate the atom of the rest of the list.
 *   Else:
 *     To put like rest of the list the out to erase atom in the rest of the
 *     list </pre>
 * \param lst list of which element is destroyed
 * \return The modified list
 */     
//--------------------------------------------------------------------
{
    if(lst)
    {
	BList* firstCons = lst;
	lst = Cdr(lst);
	delete firstCons;
    }
    return(lst);
}


//--------------------------------------------------------------------
BList* LstDestroyAllCons(BList* lst)

/*! Destroys all the elements of a list
 *  <pre> 
 *  ALGORITHM:
 *    While it is not the end of the list
 *      Destroy the first element.
 *      Next element </pre>
 *  \param lst list of all elements are destroyed  
 *  \return Pointer a NIL
 */     
//--------------------------------------------------------------------
{
    while(lst) { lst=LstDestroyFirstCons(lst); }
    return(NIL);
}


//--------------------------------------------------------------------
BList* LstDestroyUntil(BList* lst, BList* last)
       
/*! Destroys all the consecutive elements of a list until it finds
 *  the elements \a last. This last one is not destroyed.
 *  It is assumed that one is a linear list.
 * \param lst list of which elements are destroyed
 * \param last the end element that is dretroyed
 * \return The modified list
 */
//--------------------------------------------------------------------
{
    while(lst && (lst!=last))
    {
	BCore* car = Car(lst);
	lst=LstDestroyFirstCons(lst);
	if(car) { DESTROY(car); }
    }
    return(lst);
}


//--------------------------------------------------------------------
BBool BList::Destroy()

/*! Destroy one list
 *  <pre> 
 *  ALGORITHM:
 *    Destroy the first element of the list
 *    If \a car_ of first element is a list: Destroy list
 *    Else: Destroy atom
 *    If \a cdr_ of first element is a list: Destroy list
 *    Else: Destroy atom </pre>
 *  \return True destroy satisfactorily, False isn't 
 */   
//--------------------------------------------------------------------
{
    BBool destroyied = BTRUE;
    BCore* first = car_;
    BList* rest  = cdr_;
    
    delete this;
    if(first) { destroyied &= first->Destroy(); }
    while(rest)  
    { 
      first = rest->car_;
      BList* rest_  = rest->cdr_;
      delete rest; 
      destroyied &= first->Destroy();
      rest = rest_;
    }
    
    return(destroyied);
}


//--------------------------------------------------------------------
BList* LstDestroyAll(BList* lst)

/*! Destroys all the elements of a list
 * \param lst list of which elements are destroyed
 * \return NIL 
 */   
//--------------------------------------------------------------------
{
//Std(BText("\nDestroying list ") + LstText(lst));
    if(lst!=NIL) { lst->Destroy(); }
    return(NIL);
}
