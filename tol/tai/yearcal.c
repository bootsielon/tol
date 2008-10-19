/* yearcal.c: TAI Library

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

char *montab[] = 
{
    "January", "February", "March",      "April",   "May",      "June"
    ,"July"  , "August"  , "September" , "October", "November", "December"
};

int main(int argc,char *argv[])
{
    int year;
    long daystart;
    long dayend;
    long day;
    int weekday;
    struct caldate cd;
    
    while (*++argv) {
	year = atoi(*argv);
	
	cd.year = year;
	cd.month = 1;
	cd.day = 1;
	daystart = caldate_mjd(&cd);
	cd.year = year + 1;
	dayend = caldate_mjd(&cd);
	
	while ((daystart + 3) % 7) --daystart;
	while ((dayend + 3) % 7) ++dayend;
	
	for (day = daystart;day < dayend;++day) {
	    caldate_frommjd(&cd,day,&weekday,(int *) 0);
	    if (cd.year != year)
		printf("   ");
	    else {
		if (cd.month & 1)
		    if (cd.day < 10)
			printf(" %d%c%d ",cd.day % 10,8,cd.day % 10);
		    else
			printf("%d%c%d%d%c%d ",cd.day / 10,8,cd.day / 10,cd.day % 10,8,cd.day % 10);
		else
		    printf("%2d ",cd.day);
		if (weekday == 6) {
		    if ((cd.day >= 15) && (cd.day < 22))
			printf(" %s %d\n",montab[cd.month - 1],year);
		    else
			printf("\n");
		}
	    }
	}
	printf("\n");
    }
    
    exit(0);
}
