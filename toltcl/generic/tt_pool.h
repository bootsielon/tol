/* tt_pool.h - This file implements a simple pool of objects.
               GNU/tolTcl Library

   Copyright (C) 2001, 2002, 2003 - Bayes Decisión, SL

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

#ifndef TT_POOL_H
#define TT_POOL_H 1

#include <tt_common.h>

class ToltclPool {
public:
  ToltclPool( int ini = 10, int grow = 5 );
  ToltclPool( const ToltclPool & pool );
  ~ToltclPool();
  int AppendObject( void * ptr );
  void PrependObject( void * ptr );
  void ** GetObject( int i ) {
    if ( CheckIndex( i ) )
      return _GetObject(i);
    return NULL;
  }
  void ** _GetObject( int i ) {
    return m_pool+i;
  }
  void * RemoveObject( int i ) {
    if ( CheckIndex( i ) )
      return _RemoveObject(i);
    return NULL;
  }
  void * _RemoveObject( int i );
  void * RemoveLast();
  int GetSize() {
    return m_last+1;
  }
  int GetLast() {
    return m_last;
  }
private:
  int CheckIndex( int i ) {
    return i >= 0 && i <= m_size;
  }
  int m_last, m_grow, m_size;
  void ** m_pool;
};

#endif /* TT_POOL_H */
