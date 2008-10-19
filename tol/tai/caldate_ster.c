/* caldate_ster.c: TAI Library

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

void caldate_easter(struct caldate *cd)
{
    long c;
    long t;
    long j;
    long n;
    long y;
    
    y = cd->year;
    
    c = (y / 100) + 1;
    t = 210 - (((c * 3) / 4) % 210);
    j = y % 19;
    n = 57 - ((14 + j * 11 + (c * 8 + 5) / 25 + t) % 30);
    if ((n == 56) && (j > 10)) --n;
    if (n == 57) --n;
    n -= ((((y % 28) * 5) / 4 + t + n + 2) % 7);
    
    if (n < 32) { cd->month = 3; cd->day = n; }
    else { cd->month = 4; cd->day = n - 31; }
}
