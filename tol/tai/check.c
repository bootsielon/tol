/* check.c: TAI Library

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
#include <time.h>
#include <tai/tai.h>
#include <tai/leapsecs.h>
#include <tai/caltime.h>

char line[100];

char *dayname[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" } ;

char out[101];
char x[TAI_PACK];

main()
{
    struct tai t;
    struct tai t2;
    struct caltime ct;
    struct caltime ct2;
    int weekday;
    int yearday;
    int i;
    double packerr;
    
    if (leapsecs_init() == -1)
	printf("unable to init leapsecs\n");
    
    while (fgets(line,sizeof line,stdin))
	if (!caltime_scan(line,&ct))
	    printf("unable to parse\n");
	else {
	    caltime_tai(&ct,&t);
	    caltime_utc(&ct2,&t,&weekday,&yearday);
	    tai_pack(x,&t);
	    tai_unpack(x,&t2);
	    tai_sub(&t2,&t2,&t);
	    packerr = tai_approx(&t2);
	    for (i = 0;i < TAI_PACK;++i)
		printf("%2.2x",(unsigned long) (unsigned char) x[i]);
	    if (packerr)
		printf(" packerr=%f",packerr);
	    printf(" %03d  %s",yearday,dayname[weekday]);
	    if (caltime_fmt((char *) 0,&ct2) + 1 < sizeof out) {
		out[caltime_fmt(out,&ct2)] = 0;
		printf(" %s",out);
	    }
	    printf("\n");
	}
    exit(0);
}
