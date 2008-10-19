/* lsttext.cpp: BList text functions
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
#include <tol/tol_blist.h>
#include <tol/tol_bout.h>

BTraceInit("lstext.cpp");

//--------------------------------------------------------------------
BText LstText(const BList* lst,
	      const BText& openSign,
	      const BText& closeSign,
	      const BText& separatorSign)
    
/*! Transform in text any kind of list. This function work with plane
 *  lists and with recursive ones.
 * \param lst list from which the text is obtained
 * \param openSign prefix that is added to the text, default "("
 * \param closeSign suffix taht is added to the text, default ")"
 * \param separatorSign separator that is added between each atom, defalut ","
 * \return Return a text formed by all the names ot atoms, separated by
 * \return \a separatorSign, with the prefix \a openSign and the suffix
 * \return \a closeSign
 */
//--------------------------------------------------------------------
{
  BBool	   first=BTRUE;
  BText	   txt(openSign);

  while(lst)
  {
    if(first) { first = BFALSE;	      }
    else      { txt += separatorSign; }

    BAtom* car = (BAtom*)Car(lst);
    if(car)
    {
      if(IsList(car))
      { txt += LstText((BList*)car, openSign, closeSign, separatorSign); }
      else
      { txt += car->Name(); }
    }
    else
    { txt += "NIL"; }


    lst = Cdr(lst);
  }
  txt += closeSign;

  return(txt);
}
