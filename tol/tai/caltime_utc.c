/* caltime_utc.c: TAI Library

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

void caltime_utc(struct caltime *ct,
		 struct tai *t,
		 int *pwday,
		 int *pyday)
{
    struct tai t2 = *t;
    uint64 u;
    int leap;
    long s;
    
    /* XXX: check for overfow? */
    
    leap = leapsecs_sub(&t2);
    u = t2.x;
    
    u += 58486;
# ifdef WIN32
    s = u % ((uint64) 86400);
#  else
    s = u % 86400ULL;
#  endif
    
    ct->second = (s % 60) + leap; s /= 60;
    ct->minute = s % 60; s /= 60;
    ct->hour = s;

#ifdef WIN32    
    u /= ((uint64) 86400);
    caldate_frommjd(&ct->date,/*XXX*/(long) (u - ((uint64) 53375995543064)),pwday,pyday);
#else
    u /= 86400ULL;
    caldate_frommjd(&ct->date,/*XXX*/(long) (u - 53375995543064ULL),pwday,pyday);
#endif
    
    ct->offset = 0;
}
