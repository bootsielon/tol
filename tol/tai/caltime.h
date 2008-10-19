/* caltime.h: TAI Library

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

#ifndef CALTIME_H
#define CALTIME_H

#include <tai/tai_common.h>
#include <tai/caldate.h>
#include <tai/tai.h>

BEGIN_DECLS

struct caltime {
    struct caldate date;
    int hour;
    int minute;
    int second;
    long offset;
} ;

TAI_API void caltime_tai(struct caltime *ct, struct tai *t);
TAI_API void caltime_utc(struct caltime *ct, struct tai *t, int *pwday, int *pyday);

TAI_API unsigned int caltime_fmt(char*, struct caltime*);
TAI_API unsigned int caltime_scan();

END_DECLS

#endif
