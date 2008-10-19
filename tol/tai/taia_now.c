/* taia_now.c: TAI Library

   Copyright (C) 2005 - Free Software Foundation
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

#include <sys/types.h>
#ifndef WIN32
#  include <config.h>
#endif

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
#  include <winsock2.h>
# endif
#endif

#include <tai/taia.h>

#if defined(_MSC_VER) || defined(__MINGW32__)
int gettimeofday(struct timeval* tp, void* tzp) {
    DWORD t;
    t = timeGetTime();
    tp->tv_sec = t / 1000;
    tp->tv_usec = t % 1000;
    /* 0 indicates that the call succeeded. */
    return 0;
}
#endif

/* XXX: breaks tai encapsulation */

void taia_now(t)
struct taia *t;
{
    struct timeval now;
    gettimeofday(&now,(struct timezone *) 0);
# ifdef WIN32
    t->sec.x = ((uint64) 4611686018427387914) + (uint64) now.tv_sec;
# else
    t->sec.x = 4611686018427387914ULL + (uint64) now.tv_sec;
# endif
    t->nano = 1000 * now.tv_usec + 500;
    t->atto = 0;
}
