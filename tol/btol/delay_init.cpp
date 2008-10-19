/* delay_init.cpp: Support for variable number of parameters in functions.
                   GNU/TOL Language

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

#include <tol/tol_delay_init.h>
#include <stdlib.h>

struct sdelay {
  void ** obj;	     // the object to init.
  void * (*clone)(); // function clonning the object.
  int key;           // priority in initialization
};

static int CmpKey( const void * obj1, const void * obj2 )
{
  int key1 = ((sdelay*)obj1)->key;
  int key2 = ((sdelay*)obj2)->key;
  return key1 < key2 ? -1 : key1 > key2;
}

void * __delay_init( void ** obj, void * (*clone)(), int key )
{
  static sdelay * __list_to_delay;
  static int __size = 0;
  static int __last = -1;

  if ( obj == NULL ) {
    if ( __size == 0 ) return NULL;
    /* sort first, regarding the key field */
    qsort((void*)__list_to_delay,__last+1,sizeof(sdelay),&CmpKey);
    /* perform the initialization */
    for ( int i = 0; i <= __last; i++ ) {
      *(__list_to_delay[i].obj) = (*(__list_to_delay[i].clone))();
    }
    free( __list_to_delay );
    __list_to_delay = NULL;
    __size = 0;
    __last = -1;
  } else {
    /* register to init later */
    if ( __size == 0 ) {
      __size = DELAY_LIST_INCR;
      __list_to_delay = (sdelay*)(malloc( __size * sizeof(__list_to_delay[0]) ));
      __last = 0;
    } else {
      ++__last;
      if ( __last == __size )
      __size += DELAY_LIST_INCR;
      __list_to_delay = (sdelay*)(realloc( __list_to_delay, __size * sizeof(__list_to_delay[0]) ));
    }
    __list_to_delay[__last].obj = obj;
    __list_to_delay[__last].clone = clone;
    __list_to_delay[__last].key = key;
  }
  return NULL;
}
