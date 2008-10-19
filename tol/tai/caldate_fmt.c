/* caldate_fmt.c: TAI Library

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

#include <tai/caldate.h>

unsigned int caldate_fmt(s,cd)
char *s;
struct caldate *cd;
{
    long x;
    int i = 0;

    x = cd->year; if (x < 0) x = -x; do { ++i; x /= 10; } while(x);
    
    if (s) {
	x = cd->year;
	if (x < 0) { x = -x; *s++ = '-'; }
	s += i; do { *--s = '0' + (x % 10); x /= 10; } while(x); s += i;
	
	x = cd->month;
	s[0] = '-'; s[2] = '0' + (x % 10); x /= 10; s[1] = '0' + (x % 10);
	
	x = cd->day;
	s[3] = '-'; s[5] = '0' + (x % 10); x /= 10; s[4] = '0' + (x % 10);
    }
    
    return (cd->year < 0) + i + 6;
}
