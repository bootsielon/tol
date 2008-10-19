/* caldate_mjd.c: TAI Library

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

static unsigned long times365[4] = { 0, 365, 730, 1095 } ;
static unsigned long times36524[4] = { 0, 36524UL, 73048UL, 109572UL } ;
static unsigned long montab[12] =
{ 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337 } ;
/* month length after february is (306 * m + 5) / 10 */

long caldate_mjd(struct caldate *cd)
{
    long y;
    long m;
    long d;
    
    d = cd->day - 678882L;
    m = cd->month - 1;
    y = cd->year;
    
    d += 146097L * (y / 400);
    y %= 400;
    
    if (m >= 2) m -= 2; else { m += 10; --y; }
    
    y += (m / 12);
    m %= 12;
    if (m < 0) { m += 12; --y; }
    
    d += montab[m];
    
    d += 146097L * (y / 400);
    y %= 400;
    if (y < 0) { y += 400; d -= 146097L; }
    
    d += times365[y & 3];
    y >>= 2;
    
    d += 1461L * (y % 25);
    y /= 25;
    
    d += times36524[y & 3];
    
    return d;
}
