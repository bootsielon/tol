/* caltime_scan.c: TAI Library

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

#include <tai/caltime.h>

unsigned int caltime_scan(s,ct)
char *s;
struct caltime *ct;
{
    char *t = s;
    unsigned long z;
    unsigned long c;
    int sign;
    
    t += caldate_scan(t,&ct->date);
    
    while ((*t == ' ') || (*t == '\t') || (*t == 'T')) ++t;

    z = 0; 
    while ((c = (unsigned char) (*t - '0')) <= 9) 
    { z = z * 10 + c; ++t; }

    ct->hour = z;
    
    if (*t++ != ':') return 0;

    z = 0; 
    while ((c = (unsigned char) (*t - '0')) <= 9) 
    { z = z * 10 + c; ++t; }

    ct->minute = z;
    
    if (*t != ':')
	ct->second = 0;
    else {
	++t;

	z = 0; 
	while ((c = (unsigned char) (*t - '0')) <= 9) 
	{ z = z * 10 + c; ++t; }

	ct->second = z;
    }
    
    while ((*t == ' ') || (*t == '\t')) ++t;
    if (*t == '+') sign = 1; else if (*t == '-') sign = -1; else return 0;
    ++t;
    c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = c;
    c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 10 + c;
    c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 6 + c;
    c = (unsigned char) (*t++ - '0'); if (c > 9) return 0; z = z * 10 + c;
    ct->offset = z * sign;
    
    return t - s;
}
