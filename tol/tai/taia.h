/* taia.h: TAI Library

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

#ifndef TAIA_H
#define TAIA_H

#include <tai/tai_common.h>
#include <tai/tai.h>

BEGIN_DECLS

struct taia {
    struct tai sec;
    unsigned long nano; /* 0...999999999 */
    unsigned long atto; /* 0...999999999 */
} ;

TAI_API extern void taia_tai();

TAI_API extern void taia_now();

TAI_API extern double taia_approx();
TAI_API extern double taia_frac();

TAI_API extern void taia_add();
TAI_API extern void taia_sub();
TAI_API extern void taia_half();
TAI_API extern int taia_less();

#define TAIA_PACK 16
TAI_API extern void taia_pack();
TAI_API extern void taia_unpack();

#define TAIA_FMTFRAC 19
TAI_API extern unsigned int taia_fmtfrac();

END_DECLS

#endif
