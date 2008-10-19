/* tol_butil.h: Internal Utilities.
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

#ifndef TOL_BUTIL_H
#define TOL_BUTIL_H 1

#include <tol/tol_bcommon.h>

//--------------------------------------------------------------------
// INLINE FUCTIONS
//--------------------------------------------------------------------


//--------------------------------------------------------------------
inline BBool CharToBool(BChar c)

/*! Return true if c is 's', 'S', 'y' or 'Y' (si/Si/yes/Yes).
 */
//--------------------------------------------------------------------
{ 
    return(((c=='s') || (c=='S') || (c=='y') || (c=='Y')) ? BTRUE : BFALSE); 
}


//--------------------------------------------------------------------
inline BBool IsMultiple(BInt a, BInt b)

/*! Return true if a is multiple of b, for example 4 of 2.
 */
//--------------------------------------------------------------------
{
    return(!(a%b)); 
}


//--------------------------------------------------------------------
template<class Any> Any Minimum(const Any& a, const Any& b)

/*! Return the minimum value between a and b.
 */
//--------------------------------------------------------------------
{ 
    return((a<=b) ? a : b); 
}


//--------------------------------------------------------------------
template<class Any> Any Maximum(const Any& a, const Any& b)

/*! Return the maximum value between a and b.
 */
//--------------------------------------------------------------------
{
    return((a>=b) ? a : b);
}


//--------------------------------------------------------------------
inline BInt BCharOrderCriterium(const BAny any1, const BAny any2)

/*! For ordering alphabetically BChar*.
 */
//--------------------------------------------------------------------
{ 
    return(strcmp((BChar*)any1, (BChar*)any2)); 
}


//--------------------------------------------------------------------
inline BInt BIntOrderCriterium(const BAny any1, const BAny any2)

/*! For ordering alphabetically BInt.
 */
//--------------------------------------------------------------------
{
    BInt a1 = *(BInt*)(any1);
    BInt a2 = *(BInt*)(any2);
    
    return((a1 < a2) ? -1 : ((a1 > a2) ? 1 : 0));
}


//--------------------------------------------------------------------
inline BInt BRealOrderCriterium(const BAny any1, const BAny any2)

/*! For ordering alphabetically BInt.
 */
//--------------------------------------------------------------------
{
    BReal a1 = *(BReal*)(any1);
    BReal a2 = *(BReal*)(any2);
    
    return((a1 < a2) ? -1 : ((a1 > a2) ? 1 : 0));
}


struct BLoHiWord    { short int loWord_; short int hiWord_; };
union  BMixLoHiWord { long longWord_;  BLoHiWord loHiWord_; };

//--------------------------------------------------------------------
inline long MixLoHi(short int lo, short int hi)

/*! Interchange a low word with a high word
 */
//--------------------------------------------------------------------
{
    BMixLoHiWord mix;
    mix.loHiWord_.loWord_=lo;
    mix.loHiWord_.hiWord_=hi;
    return(mix.longWord_);
}


//--------------------------------------------------------------------
inline long Div(long a, long b)

/*! Returns the floor of integer division
 */
//--------------------------------------------------------------------
{
    if(((a>=0)&&(b>=0))||((a<=0)&&(b<=0))) { return(a/b);   }
    else 				   { return(a/b-1); }
}

//--------------------------------------------------------------------
inline long Mod(long a, long b)

/*! Returns the floor of integer division
 */
//--------------------------------------------------------------------
{
    return(a-b*Div(a,b));
}




#endif // TOL_BUTIL_H

