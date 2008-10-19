/* taia_half.c: TAI Library

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
   Author: D. J. Bernstein <djb@cr.yp.to>

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

#include <tai/taia.h>

/* XXX: breaks tai encapsulation */

void taia_half(t,u)
struct taia *t;
struct taia *u;
{
    t->atto = u->atto >> 1;
    if (u->nano & 1) t->atto += 500000000UL;
    t->nano = u->nano >> 1;
    if (u->sec.x & 1) t->nano += 500000000UL;
    t->sec.x = u->sec.x >> 1;
}
