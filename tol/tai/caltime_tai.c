/* caltime_tai.c: TAI Library

   Copyright (C) 2005 - Free Software Foundation
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

#include <tai/tai.h>
#include <tai/leapsecs.h>
#include <tai/caldate.h>
#include <tai/caltime.h>

/* XXX: breaks tai encapsulation */

void caltime_tai(struct caltime *ct, struct tai *t)
{
    long day;
    long s;
    
    /* XXX: check for overflow? */
    
    day = caldate_mjd(&ct->date);
    
    s = ct->hour * 60 + ct->minute;
    s = (s - ct->offset) * 60 + ct->second;

# ifdef WIN32    
    t->x = day * ((uint64) 86400) + ((uint64) 4611686014920671114) + (uint64) s;
# else
    t->x = day * 86400ULL + 4611686014920671114ULL + (long long) s;
# endif
    
    leapsecs_add(t,ct->second == 60);
}
