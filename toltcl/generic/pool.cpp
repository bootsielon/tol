/* pool.cpp: This file implements a simple pool of objects.
             GNU/TOLBase4 Program

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

#include <tt_pool.h>
#include <tcl.h>
#include <string.h>

ToltclPool::ToltclPool( int ini, int grow )
{
  m_last = -1;
  if ( grow <= 0 )
    grow = 5;
  if ( ini <= 0 ) {
    ini = grow;
  }
  m_size = ini;
  m_grow = grow;
  m_pool = (void**)Tcl_Alloc( ini * sizeof(void*) );
}

ToltclPool::ToltclPool( const ToltclPool & pool )
{
  m_grow = pool.m_grow;
  m_size = pool.m_size;
  m_last = pool.m_last;
  m_pool = (void**)Tcl_Alloc( m_size * sizeof(void*) );

  /* only copy the void* used */

  memcpy( m_pool, pool.m_pool, (m_last+1)*sizeof(void*) );
}

ToltclPool::~ToltclPool()
{
  if ( m_pool )
    Tcl_Free( (char*)m_pool );
  m_size = 0;
  m_last = -1;
}

int ToltclPool::AppendObject( void * ptr ) {
  ++m_last;
  if ( m_last == m_size ) {
    m_size += m_grow;
    m_pool = (void**)Tcl_Realloc( (char*)m_pool, sizeof(void*)*m_size );
  }
  m_pool[m_last] = ptr;
  return m_last;
}

void ToltclPool::PrependObject( void * ptr ) {
  ++m_last;
  if ( m_last == m_size ) {
    m_size += m_grow;
    m_pool = (void**)Tcl_Realloc( (char*)m_pool, sizeof(void*)*m_size );
  }
  if ( m_last ) {
    /* make room for first ptr */
    memmove( m_pool+1, m_pool, sizeof(void*) * m_last );
  }
  m_pool[0] = ptr;
}

void * ToltclPool::_RemoveObject( int i )
{
  void * iObj = m_pool[i];
  int n = m_last - i;
  
  if ( n ) {
  }
  memmove( m_pool+i, m_pool+i+1, n * sizeof(m_pool[0]) );
  --m_last;
  return iObj;
}

void * ToltclPool::RemoveLast()
{
  void * lastPtr = NULL;

  if ( m_last >= 0 ) {
    lastPtr = m_pool[m_last];
    --m_last;
  }
  return lastPtr;
}
