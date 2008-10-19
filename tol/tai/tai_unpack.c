/* tai_unpack.c: TAI Library

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

#include <tai/tai.h>

void tai_unpack(char *s, struct tai *t)
{
    uint64 x;
    
    x = (unsigned char) s[0];
    x <<= 8; x += (unsigned char) s[1];
    x <<= 8; x += (unsigned char) s[2];
    x <<= 8; x += (unsigned char) s[3];
    x <<= 8; x += (unsigned char) s[4];
    x <<= 8; x += (unsigned char) s[5];
    x <<= 8; x += (unsigned char) s[6];
    x <<= 8; x += (unsigned char) s[7];
    t->x = x;
}
