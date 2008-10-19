/* easter.c: TAI Library

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
#include <tai/caldate.h>

char *dayname[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" } ;

char out[101];

main(argc,argv)
int argc;
char **argv;
{
    struct caldate cd;
    long day;
    int weekday;
    int yearday;
    int i;
    
    while (*++argv) {
	cd.year = atoi(*argv);
	if (cd.year > 0) {
	    caldate_easter(&cd);
	    day = caldate_mjd(&cd);
	    caldate_frommjd(&cd,day,&weekday,&yearday);
	    if (caldate_fmt((char *) 0,&cd) + 1 >= sizeof out) exit(1);
	    out[caldate_fmt(out,&cd)] = 0;
	    printf("%s %s  yearday %d  mjd %d\n",
		   dayname[weekday], out, yearday, day);
	}
    }
    exit(0);
}
