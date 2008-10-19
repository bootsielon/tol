/* taia_fmtfrac.c: TAI Library

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

unsigned int taia_fmtfrac(s,t)
char *s;
struct taia *t;
{
    unsigned long x;
    
    if (s) {
	x = t->atto;
	s[17] = '0' + (x % 10); x /= 10;
	s[16] = '0' + (x % 10); x /= 10;
	s[15] = '0' + (x % 10); x /= 10;
	s[14] = '0' + (x % 10); x /= 10;
	s[13] = '0' + (x % 10); x /= 10;
	s[12] = '0' + (x % 10); x /= 10;
	s[11] = '0' + (x % 10); x /= 10;
	s[10] = '0' + (x % 10); x /= 10;
	s[9] = '0' + (x % 10);
	x = t->nano;
	s[8] = '0' + (x % 10); x /= 10;
	s[7] = '0' + (x % 10); x /= 10;
	s[6] = '0' + (x % 10); x /= 10;
	s[5] = '0' + (x % 10); x /= 10;
	s[4] = '0' + (x % 10); x /= 10;
	s[3] = '0' + (x % 10); x /= 10;
	s[2] = '0' + (x % 10); x /= 10;
	s[1] = '0' + (x % 10); x /= 10;
	s[0] = '0' + (x % 10);
    }
    
    return 18;
}
