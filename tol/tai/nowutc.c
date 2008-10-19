/* nowutc.c: TAI Library

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

#include <stdio.h>
#include <tai/leapsecs.h>
#include <tai/tai.h>
#include <tai/taia.h>
#include <tai/caltime.h>

struct taia now;
struct tai sec;
struct caltime ct;

char x[TAIA_FMTFRAC];

main()
{
    if (leapsecs_init() == -1) {
	fprintf(stderr,"utcnow: fatal: unable to init leapsecs\n");
	exit(111);
    }
    
    taia_now(&now);
    x[taia_fmtfrac(x,&now)] = 0;
    
    taia_tai(&now,&sec);
    caltime_utc(&ct,&sec,(int *) 0,(int *) 0);
    
    printf("%d-%02d-%02d %02d:%02d:%02d.%s\n"
	   ,ct.date.year
	   ,ct.date.month
	   ,ct.date.day
	   ,ct.hour
	   ,ct.minute
	   ,ct.second
	   ,x
	);
    
    exit(0);
}
