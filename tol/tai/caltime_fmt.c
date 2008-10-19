/* caltime_fmt.c: TAI Library

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
#include <tai/caltime.h>

unsigned int caltime_fmt(char *s, struct caltime *ct)
{
    unsigned int result;
    long x;
    
    result = caldate_fmt(s,&ct->date);
    
    if (s) {
	s += result;
	
	x = ct->hour;
	s[0] = ' ';
	s[2] = '0' + (x % 10); x /= 10;
	s[1] = '0' + (x % 10);
	s += 3;
	
	x = ct->minute;
	s[0] = ':';
	s[2] = '0' + (x % 10); x /= 10;
	s[1] = '0' + (x % 10);
	s += 3;
	
	x = ct->second;
	s[0] = ':';
	s[2] = '0' + (x % 10); x /= 10;
	s[1] = '0' + (x % 10);
	s += 3;
	
	s[0] = ' ';
	x = ct->offset;
	if (x < 0) { s[1] = '-'; x = -x; } else s[1] = '+';
	
	s[5] = '0' + (x % 10); x /= 10;
	s[4] = '0' + (x % 6); x /= 6;
	s[3] = '0' + (x % 10); x /= 10;
	s[2] = '0' + (x % 10);
    }
    
    return result + 15;
}
