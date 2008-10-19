/* tt_crash.h - GNU/tolTcl Library

   Copyright (C) 2003 - Bayes Decisión, SL

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

#ifndef TT_CRASH_H
#define TT_CRASH_H 1

#include <tt_common.h>
#include <setjmp.h>

BEGIN_C_DECLS

extern jmp_buf jmp_mark;

#define try_crash if ( !setjmp(jmp_mark) )

#define catch_crash else

int InitCrashHandler();

END_C_DECLS

#endif /* TT_CRASH_H */
