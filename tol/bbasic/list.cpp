/* list.cpp: Implements internal List behavior of GNU/TOL language.

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

#include <tol/tol_list.h>

BTraceInit("list.cpp");

//--------------------------------------------------------------------
List::List ()

/*! Lists elements constructor. Initialize the attributes to nil
 */
//--------------------------------------------------------------------
    : BCore(), car_(NIL), cdr_(NIL) 
{}


//--------------------------------------------------------------------
List::List (BCore* a, List* b): BCore(), car_(a), cdr_(b) 

/*! Lists elements constructor. Inizialices the attributes with the
 *  values that go like parameters
 *           
 *  ALGORITHM: If a is an atom: Incremente a's references.
 * \param a value to assign \a car_
 * \param b value to assign \a cdr_
 */   
//--------------------------------------------------------------------
{
    if(a && !a->IsListClass()) {
      ((BAtom*)a)->IncNRefs();
    }
}


//--------------------------------------------------------------------
List::~List() 

/*! Cons destructor
 * 
 *  ALGORITHM: If the car of the cons is an atom: 
 *             Decremente the atom references.
 */   
//--------------------------------------------------------------------
{
    if((car_) && !car_->IsListClass()) {
	((BAtom*)car_)->DecNRefs();
    }
    car_=NIL; cdr_=NIL;
}


//--------------------------------------------------------------------
List* List::destroyFirstCons () 

/*! Destroys the first element of a list
 */     
//--------------------------------------------------------------------
{
    List* lAux = this;
    if(lAux) {
	List* firstCons = lAux;
	lAux = lAux->cdr();
	delete firstCons;
    }
    return lAux;
}


//--------------------------------------------------------------------
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
BBool List::Destroy() {
    BBool destroyied = BTRUE;
    BCore* first = car_;
    List*  rest  = cdr_;

    delete this;
    if(first) { destroyied = first->Destroy(); }
    if(rest)  { destroyied = rest ->Destroy(); }

    return(destroyied);
}


//------------------------------------------------------------
BBool List::empty() const {
    if (!cdr_) return BTRUE;
    else return BFALSE;
}

//- static method --------------------------------------------
List* List::cons (BCore* A, List* B) {
    return (new List(A,B));
}

//- static method --------------------------------------------
List* List::nCons (BCore* A) {
    return (cons(A,NIL));
}

//------------------------------------------------------------
BBool List::IsListClass() const {
    return(BTRUE);
}

//------------------------------------------------------------
BCore* List::car () const { 
    if (car_) return car_;
    else return NIL;
}

//------------------------------------------------------------
List* List::cdr() const {
    if (cdr_) return cdr_;
    else return NIL;
}

//------------------------------------------------------------
void List::setCar (BCore* c) {
    if(car_ && !(car_->IsListClass())) {
	((BAtom*)car_)->DecNRefs();
    }
    car_=c;
    if(c && !c->IsListClass()) {
	((BAtom*)c)->IncNRefs();
    }
}

//------------------------------------------------------------
void List::setCdr (List* c) {
    cdr_=c;
}

//------------------------------------------------------------
/*! Return cdr_ attribute of the list at level specified with
 * \a n. If \a n is 1 return original List*. If \a n is 2
 * return first cdr_ of this List*
 * \param n cdr_ will be returned (1 == this List, 2 == first cdr_)
 */
List* List::getNthCdr (BInt n) { 
    List* lAux = this;
    if (n==1) return this;
    else for(; lAux && ((n-1)>0); n--, lAux=lAux->cdr()) {}
    return(lAux);
}

//------------------------------------------------------------
BCore* List::getNthCar (BInt n) {
    List* aList = getNthCdr(n);
    if(aList) 
	return aList->car();
    else return NIL;
}


//--------------------------------------------------------------------
BBool List::insert (BCore* c, BInt nth) 

/*! Inserts \a c at the \a nth atom. If the \a nth atom does not
 *  exist, appends \a c to the tail of the list
 * \param c atom to insert
 * \param nth position in which it is inserted
 * \return BTRUE if \a c is added correctly
 */
//--------------------------------------------------------------------
{ 
    // n es 1 para this, 2 para el this->cdr_... etc.
    BBool ok = BFALSE;
    if (nth<=1) {
	cdr_ = List::cons(car(), cdr());
	car_ = c;
	ok = BTRUE;
    } else {
	List* llevel = getNthCdr(nth);
	if (llevel) {
	    llevel->setCdr(List::cons(llevel->car(), llevel->cdr()));
	    llevel->setCar(c);
	    ok = BTRUE;
	} else {
	    ok = append(c);
	}
    }

    return ok;
}


//--------------------------------------------------------------------
BBool List::append (BCore* atm) 

/*! PURPOSE:
 *     Append the atom at the tail of the list.
 *  \param lst list in which it adds the atom
 *  \param atm atom that we added
 *  \return Return \a lst modified
 */
//--------------------------------------------------------------------
{
    getLastCdr()->setCdr(List::nCons(atm));
    return BTRUE;
}


//--------------------------------------------------------------------
List* List::getLastCdr () 

/*! PURPOSE:
 *     Obtain the las \a cdr_ of the list.
 *  \param lst list of which we looked for the last element
 *  \return Returns the last \a cdr_ of the list
 */
//--------------------------------------------------------------------
{
    List* lAux = this;
    while(lAux->cdr()) {
	lAux = lAux->cdr();
    }
    return(lAux);
}


//--------------------------------------------------------------------
BInt List::length () const 

/*! Calculates how many Cdr's has this List
 */
//--------------------------------------------------------------------
{
  BInt length;
  List* lst = cdr_;
  for(length=1; lst; length++, lst=lst->cdr());
  return(length);
}


//--------------------------------------------------------------------
List* List::duplicate () const 

/*! Makes a copy of the list duplicating the pointers guide
 * \param lst list to duplicate
 * \return A new list
 */
//--------------------------------------------------------------------
{
    BCore* dupCar;
    List*  dupCdr;

    if (car_) {
	dupCar = car();
	if (dupCar->IsListClass()) {
	    dupCar = ((List*) dupCar)->duplicate();
	}
    } else dupCar = NIL;

    if (cdr_) {
	dupCdr = cdr()->duplicate();
    } else dupCdr = NIL;

    return List::cons(dupCar, dupCdr);
}

//--------------------------------------------------------------------
BList* List::toBList () 
//--------------------------------------------------------------------
{
    BCore* dupCar;
    BList*  dupCdr;

    if (car_) {
	dupCar = car();
	if (dupCar->IsListClass()) {
	    dupCar = ((List*) dupCar)->toBList();
	}
    } else dupCar = NIL;

    if (cdr_) {
	dupCdr = cdr()->toBList();
    } else dupCdr = NIL;

    return Cons(dupCar, dupCdr);
}


//--------------------------------------------------------------------
BCore* List::findByName (const BText& name) 
//--------------------------------------------------------------------
{
    if(cdr_ == NIL) { return(NIL); }
    BBool found = BFALSE;
    BAtom* atm;
    List* lst = this;
    do {
	if( (atm = (BAtom*) lst->car()) && (atm->Name()!=name)) { 
	    lst = lst->cdr();
	} else { found = BTRUE; }
    } while(lst && !found);
    if (found) return(lst->car());
    else return NIL;
}
