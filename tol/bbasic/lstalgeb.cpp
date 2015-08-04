/* lstalgeb.cpp: BList algebraic functions.
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

//#include <config.h>
#include <tol/tol_bcommon.h>
#include <tol/tol_btext.h>
#include <tol/tol_blist.h>
#include <stdarg.h>

BTraceInit("lstalgeb.cpp");

//--------------------------------------------------------------------
BList* LstAppend(BList* lst, BCore* atm)

/*! Append the atom at the tail of the list.
 * \param lst list in which it adds the atom
 * \param atm atom that we added
 * \return Return \a lst modified
 */
//--------------------------------------------------------------------
{
    if(lst) { LstLastCdr(lst)->PutCdr(NCons(atm)); }
    else    { lst = NCons(atm);                    }
    
    return(lst);
}


//--------------------------------------------------------------------
BList* LstAppendUnique(BList* lst, BCore* atm)

/*! Append the atom at the tail of the list, unless the atom was
 *  previously in the list.
 * \param lst list in which it adds the atom
 * \param atm atom that we added
 * \return Return \a lst modified
 */
//--------------------------------------------------------------------
{
    if(! LstFind(lst,atm)) { lst=LstAppend(lst, atm); }
    
    return(lst);
}


//--------------------------------------------------------------------
BList* LstCompact(BList* lst)

/*! Makes a compactation of a list removing duplicated atoms.
 * \param lst list to compact
 * \return Return list compacted
 */
//--------------------------------------------------------------------
{
    BList* auxLst=lst;
    
    while(auxLst)
    {
	auxLst->PutCdr(LstRemoveAtom(Cdr(auxLst),Car(auxLst)));
	auxLst=Cdr(auxLst);
    }
    return(lst);
}


//--------------------------------------------------------------------
BList* LstConcat(BList* lstA, BList* lstB)

/*! Concat all atoms in lstB at the end of lstA
 * \param lstA list in which the elements are added
 * \param lstB list to add
 * \return Return \a lstA modified
 */
//--------------------------------------------------------------------
{
    BList* lastCons=LstLastCdr(lstA);
    
    if(lastCons)
    {
	if(lstA != lstB) { lastCons->PutCdr(lstB); }
	else             { lastCons->PutCdr(LstDuplicate(lstA)); }
    }
    else { lstA=lstB; }
    
    return(lstA);
}


//--------------------------------------------------------------------
BList* LstConcatUnique(BList* lstA, BList* lstB)

/*! Concat all the atoms of \a lstB, that do not appear in \a lstA,
 *  at the end \a lstA 
 * \param lstA list in which the elements are added
 * \param lstB list to add
 * \return Return \a lstA modified
 */
//--------------------------------------------------------------------
{
    BAtom* atm;
    
    ForAll(BAtom, atm, lstB) { lstA=LstAppendUnique(lstA,atm); }
    EndFor;
    
    return(lstA);
}


//--------------------------------------------------------------------
BList* LstDuplicate(const BList* lst)

/*! Makes a copy of the list duplicating the pointers guide
 * \param lst list to duplicate
 * \return A new list
 */
//--------------------------------------------------------------------
{
/*  if(lst && IsList(lst))
  {
    return(Cons(LstDuplicate((BList*) Car(lst)),
		LstDuplicate((BList*) Cdr(lst))));
  }
  else { return(NIL); }
*/
    if(!lst) { return(NIL); }
    BCore* dupCar = lst->Car();
    if(IsList(dupCar))  { dupCar = LstDuplicate((BList*)dupCar); }
    return(Cons(dupCar, LstDuplicate(lst->Cdr())));
}


//--------------------------------------------------------------------
BList* LstFind(BList* lst, const BCore* atm)

/*! Looks for an atom in the list.
 * \param lst list in which we looked for
 * \param atm atom to look for
 * \return Returns the sublist that its car just point to the atom.
 * \return If the atom is not in the list, returns NIL.
 */
//--------------------------------------------------------------------
{
    while(lst && (Car(lst)!=atm)) { lst = Cdr(lst); }
    return(lst);
}


//--------------------------------------------------------------------
BCore* LstFindByName(const BList* lst, const BText& name)

/*! Looks for an atom in the list with Name() equal than \a name.
 * \param lst list in which one looks for
 * \param name text to look for
 * \return Return the atom whose Name() is \a name.
 * \return If the atom is not in the list, returns NIL.
 */
//--------------------------------------------------------------------
{
    if(!lst) { return(NIL); }
    BBool found = BFALSE;
    BAtom* atm;
    do
    {
	if( (atm= (BAtom*)Car(lst)) && (atm->Name()!=name)) { 
	    lst = Cdr(lst); 
	} else { 
	    found = BTRUE; 
	}
    }
    while(lst && !found);
    return(Car(lst));
}


//--------------------------------------------------------------------
BList* LstAdd(BList* lst, const BAtom* atm, BBool& duplicate)

/*! Adds \a atm to the alfabetically ordered list \a lst if not
 *  exist any atom in the list with Name() equal than atm->Name().
 * \param lst list in which the element is added
 * \param atm atom to add
 * \param duplicate value by reference, gives back TRUE if \a amt this
 *        repeated, FALSE if no
 * \return Return \a lst modified
 */
//--------------------------------------------------------------------
{
    BList* last  = lst;
    BList* penul = NIL;
    BText	 carName;
    duplicate = BFALSE;
    
    if(!lst) { lst = NCons((BCore*)atm); }
    else
    {
	BText name = atm->Name();
	while(last && ((carName=((BAtom*)Car(last))->Name()) < name))
	{ penul = last; last=Cdr(last); }
	
	if(carName==name) { duplicate = BTRUE; }
	else
	{
	    if(penul)
	    {
		if(!last) { last = NCons((BCore*)atm); }
		else { last = Cons((BCore*)atm, last); }
		penul->PutCdr(last);
	    }
	    else { lst = Cons((BCore*)atm, lst); }
	}
    }
    return(lst);
}


//--------------------------------------------------------------------
void LstToArr(const BList* lst, BArray<BAtom*>& arr)

/*! Puts the \a car_ members of \a lst in \a arr. The list \a lst
 *  must be a simple list, is to say, its cars must be not lists
 * \param lst list that is introduced in a array
 * \param arr value by reference, return atom of a list
 * \sa BArray::AllocBufer
 */
//--------------------------------------------------------------------
{
    BInt n = LstLength(lst);
    
    arr.AllocBuffer(n);
    for(BInt i=0; i<n; i++)
    {
	arr[i] = (BAtom*)Car(lst);
	lst	   = Cdr(lst);
    }
}


//--------------------------------------------------------------------
BList* ArrToLst(BArray<BAtom*>& arr)

/*! Makes a new list with the items of \a arr, preserving the order
 *  in the array \a arr, and returns the list
 * \param arr array with atoms
 * \return Return a new list
 * \sa BArray::Size
 */
//--------------------------------------------------------------------
{
    BList* lst = NIL;
    for(BInt i=(arr.Size()-1); i>=0; i--)
    {
	lst = Cons(arr[i], lst);
    }
    return(lst);
}


//--------------------------------------------------------------------
BList* LstSort(const BList* lst, BOrderCriterium order)

/*! Sorts the list \a lst with order criterium and returns the new
 *  sorted list. The list \a lst must have only BAtom or derived
 *  objects in their \a car_ members
 *  <pre>         
 *  ALGORITHM:                                                            
 *    Make a BArray<BAtom*> with their \a car_ members                   
 *    Sort it with order criterium                                      
 *    Make a new list with the items of the array.                     
 *    Return the new list </pre>
 * \param lst list to sort
 * \param order order criterion
 * \return Return the new list
 * \sa BArray::Sort, BOrderCriterium
 */
//--------------------------------------------------------------------
{
    BArray<BAtom*> arr;
    LstToArr(lst, arr);
    arr.Sort(order);
    BList* newList = ArrToLst(arr);
    return(newList);
}


//--------------------------------------------------------------------
BList* LstIntersect(BList* lstA, BList* lstB)

/*! Builds a new list with commons atoms to both list. Looks for an
 *  atom in the list. Returns the sublist that its car just point to
 *  the atom. If the atom is not in the list, returns NIL
 * \param lstA list to intersect
 * \param lstB list to intersect
 * \return Return a new list with the commons elements
 */
//--------------------------------------------------------------------
{
    if(lstA && lstB)
    {
	BList* auxLst=NIL;
	BAtom* auxAtm;
	
	ForAll(BAtom, auxAtm, lstA)
	    if (LstFind(lstB,auxAtm)) { auxLst=LstAppend(auxLst,auxAtm); }
	EndFor
	    
	    return(auxLst);
    }
    else { return(NIL); }
}


//--------------------------------------------------------------------
BList* LstInsert(BList* lst, BCore* c, BInt nth)

/*! Inserts \a c at the \a nth atom. If the \a nth atom does not
 *  exist, appends \a c to the tail of the list
 * \param lst list in which it is inserted
 * \param c atom to insert
 * \param nth position in which it is inserted
 * \return A new list, if the \a lst not exist, returns a new list with
 * \return the atom \a c        
 */
//--------------------------------------------------------------------
{
    if (!lst) { lst = new BList(c, NIL); }
    else
    {
	if (nth<1)	{ lst = Cons(c, lst); }
	else
	{
	    BList* previous = LstNthCdr(lst, nth-1);
	    if (previous) { previous->PutCdr(Cons(c, (Cdr(previous))));}
	    else	  { lst = LstAppend(lst, c); }
	}
    }
    BList * l = lst;
    return(l);
}


//--------------------------------------------------------------------
BList* LstLastCdr(BList* lst)

/*! Obtain the las \a cdr_ of the list.
 * \param lst list of which we looked for the last element
 * \return Returns the last \a cdr_ of the list
 */
//--------------------------------------------------------------------
{
    if(lst) {	while(Cdr(lst)) { lst = Cdr(lst); } }
    return(lst);
}


//--------------------------------------------------------------------
BInt LstLength(const BList* lst)

/*! Obtain length of a list
 * \param lst list in which number of elements calculates
 * \return  Returns the length of the list.
 */
//--------------------------------------------------------------------
{
    BInt length;
    
    for(length=0; lst; length++, lst=Cdr(lst)) { ; }
    
    return(length);
}


//--------------------------------------------------------------------
BList* LstNthCdr(BList* lst, BInt nth)

/*! Obtain one sublist from the \a nth position of a list \a lst
 * \param lst list in which one look for 
 * \param nth position to look for
 * \return Returns the sublist that its car points to the \a n-th atom.
 *         If the \a nth atom does not exist, returns NIL.
 */
//--------------------------------------------------------------------
{
    for(; lst && ((nth-1)>0); nth--, lst=Cdr(lst)) { ; }
    return(lst);
}


//--------------------------------------------------------------------
BList* LstJoint(const BList* lstA, const BList* lstB)

/*! Concatenate two list eliminating the duplicated atoms
 * \param lstA first list to concatene
 * \param lstB second list to concatene
 * \return Return a new list
 */
//--------------------------------------------------------------------
{
    BList* auxLst=NIL;
    BAtom* auxAtm;
    
    ForAll(BAtom, auxAtm, lstA)
	{ auxLst=LstAppendUnique(auxLst,auxAtm); }
    EndFor
	
    ForAll(BAtom, auxAtm, lstB)
	{ auxLst=LstAppendUnique(auxLst,auxAtm); }
    EndFor
	
    return(auxLst);
}


//--------------------------------------------------------------------
BList* LstStack(BList* lst, ...)

/*! Constructs a list, adding elements by the end
 * \param lst list in which the atoms are added
 * \param ... number of variable parameters of BCore type or descendants,
 *            the last one has to be NULL
 * \return Return a \a lst modified
 */
//--------------------------------------------------------------------
{
  va_list ap;
  BCore* arg;
  BList* newList=lst;

  if(newList) { while(Cdr(newList)) { newList = Cdr(newList); } }
  va_start(ap, lst);
  do
  {
    if((arg = va_arg(ap,BCore*)))
    {
      if(!newList)
      {
        newList = new BList(arg, NIL);
        lst = newList;
      }
      else
      {
        newList->PutCdr(new BList(arg, NIL));
        newList = Cdr(newList);
      }
    }
  }
  while (arg);
  va_end(ap);

  return(lst);
}


//--------------------------------------------------------------------
BList* LstReverse(BList* lst)

/*! Reverses the order of atoms of the \a lst
 * \param lst list to reverse
 * \return Return \a lst reversed
 */
//--------------------------------------------------------------------
{
    BList* rev = NIL;
    while(lst)
    {
	rev = Cons(Car(lst), rev);
	lst = Cdr(lst);
    }
    return(rev);
}


//--------------------------------------------------------------------
BList* LstSublist(BList* lst, BInt from, BInt to)

/*! Returns the sublist from position \a from to position \a to
 * \param lst list from which the sublist is obtain
 * \param from position from, default 0
 * \param to position to, defalut 0 
 *  \return Return a new lits with the values of the sublist
 */
//--------------------------------------------------------------------
{
    BList* lstRes=NIL;
    
    if(from>=0 && to>=0 && from<=to)
    {
	BList* resto=LstNthCdr(lst, from);
	BInt i=(from) ? from : 1;
	
	while((i<=to || to==0) && resto)
	{
	    lstRes = LstAppend(lstRes, Car(resto));
	    resto = Cdr(resto);
	    i++;
	}
    }
    return(lstRes);
}


//--------------------------------------------------------------------
BList* LstMatch(BList* lst, const BText& pattern)

/*! Obtain a new sublist that its atom names make match with the
 *  text \a pattern, like *Beta makes match with AlfaBeta
 * \param lst list from which the sublist is obtain
 * \param pattern Text to match which text
 * \return Return a new sublist
 * \sa BText::Match
 */
//--------------------------------------------------------------------
{
    BList* lstMatch=NIL;
    BList* con=NIL;
    BAtom* atm;
    
  ForAll(BAtom, atm, lst)
    { if(atm->Name().Match(pattern)) { LstFastAppend(lstMatch, con, atm); } }
  EndFor;

  return(lstMatch);
}


//--------------------------------------------------------------------
BAtom* LstFirstMatch(BList* lst, const BText& pattern, BInt* position)

/*! Obtain the first atom that makes match with the pattern. Also
 *  gives the \a position of this atom, like *Beta makes with
 *  AlfaBeta
 * \param lst list from which the atom is obtain
 * \param pattern Text to match which text
 * \param position Indicates the position in which is the atom
 * \return Return the first atom that makes match with the pattern,
 * \return NIL and position 0 if any makes match
 * \sa BText::Match
 */
//--------------------------------------------------------------------
{
  BAtom* atm=NIL;
  BBool	 found=BFALSE;

  *position=0;
  if(pattern.HasName())
  {
    BBool  exact=(pattern.Find('*')<0); // There aren't *
    BInt   p=0;

    while(lst && !found)
    {
      atm=(BAtom*)Car(lst);
      if(( exact && atm->Name()==pattern) ||
	 (!exact && atm->Name().Match(pattern))) { *position=p; found=BTRUE; }
      lst=Cdr(lst);
      p++;
    }
  }
  return(found ? atm : NIL);
}


//--------------------------------------------------------------------
BList* LstDifference(BList* lst1, BList* lst2)
 
/*! Obtain the difference between \a lst1 and \a lst2.
 *  Elements of \a lst1 that not appears in \a lst2.
 * \param lst1 List on which it is reduced
 * \param lst2 List that is reduced
 * \return Return a new list that the difference between \a lst1 and \a lst2
 */
//--------------------------------------------------------------------
{
    BList* lstRes=NIL;
    BCore* elem;
    
    ForAll(BCore, elem, lst1)
	if(!LstFind(lst2, elem)) { lstRes = LstAppend(lstRes, elem); }
    EndFor
	
    return(lstRes);
}


//--------------------------------------------------------------------
BList* LstDifferenceByName(BList* lst1, BList* lst2)

/*! Obtains the difference between \a lst1 and \a lst2 using the
 *  object names (not the pointer as LstDifference()). Elements of
 * \a lst1 that its name not appears in \a lst2.
 * \param lst1 List on which it is reduced
 * \param lst2 List that is reduced
 * \return Returns the difference between \a lst1 and \lst2
 */
//--------------------------------------------------------------------
{
    BList* lstRes=NIL;
    BAtom* atm;

  ForAll(BAtom, atm, lst1)
    if(!LstFindByName(lst2,atm->Name())) { lstRes=LstAppend(lstRes,atm); }
  EndFor

    return(lstRes);
}
