/* leapsecs_add.c: TAI Library

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

#include <tai/leapsecs.h>
#include <tai/tai.h>

/* XXX: breaks tai encapsulation */

extern struct tai *leapsecs;
extern int leapsecs_num;

void leapsecs_add(t,hit)
struct tai *t;
int hit;
{
    int i;
    uint64 u;
    
    if (leapsecs_init() == -1) return;
    
    u = t->x;
    
    for (i = 0;i < leapsecs_num;++i) {
	if (u < leapsecs[i].x) break;
	if (!hit || (u > leapsecs[i].x)) ++u;
    }
    
    t->x = u;
}
