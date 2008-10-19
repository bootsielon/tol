/* caldate_fmjd.c: TAI Library

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

void caldate_frommjd(cd,day,pwday,pyday)
struct caldate *cd;
long day;
int *pwday;
int *pyday;
{
    long year;
    long month;
    int yday;
    
    year = day / 146097L;
    day %= 146097L;
    day += 678881L;
    while (day >= 146097L) { day -= 146097L; ++year; }
    
    /* year * 146097 + day - 678881 is MJD; 0 <= day < 146097 */
    /* 2000-03-01, MJD 51604, is year 5, day 0 */
    
    if (pwday) *pwday = (day + 3) % 7;
    
    year *= 4;
    if (day == 146096L) { year += 3; day = 36524L; }
    else { year += day / 36524L; day %= 36524L; }
    year *= 25;
    year += day / 1461;
    day %= 1461;
    year *= 4;
    
    yday = (day < 306);
    if (day == 1460) { year += 3; day = 365; }
    else { year += day / 365; day %= 365; }
    yday += day;
    
    day *= 10;
    month = (day + 5) / 306;
    day = (day + 5) % 306;
    day /= 10;
    if (month >= 10) { yday -= 306; ++year; month -= 10; }
    else { yday += 59; month += 2; }
    
    cd->year = year;
    cd->month = month + 1;
    cd->day = day + 1;
    
    if (pyday) *pyday = yday;
}
