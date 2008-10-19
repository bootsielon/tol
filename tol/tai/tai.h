/* tai.h: TAI Library

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

#ifndef TAI_H
#define TAI_H

#include <tai/tai_common.h>
#include <tai/uint64.h>

BEGIN_DECLS

struct tai {
  uint64 x;
} ;

TAI_API extern void tai_now(struct tai *t);

#define tai_approx(t) ((double) ((t)->x))

TAI_API extern void tai_add(struct tai *t,struct tai *u,struct tai *v);
TAI_API extern void tai_sub(struct tai *t,struct tai *u,struct tai *v);
#define tai_less(t,u) ((t)->x < (u)->x)

#define TAI_PACK 8
TAI_API extern void tai_pack(char* s, struct tai *t);
TAI_API extern void tai_unpack(char* s, struct tai *t);

END_DECLS

#endif
