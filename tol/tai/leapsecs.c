/* leapsecs.c: TAI Library

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

#include <stdio.h>
#include <tai/tai.h>
#include <tai/leapsecs.h>
#include <tai/caldate.h>

/* XXX: breaks tai encapsulation */

/* XXX: output here has to be binary; DOS redirection uses ASCII */

char line[100];

main()
{
    struct caldate cd;
    struct tai t;
    char x[TAI_PACK];
    long leaps = 0;
    
    while (fgets(line,sizeof line,stdin))
	if (line[0] == '+')
	    if (caldate_scan(line + 1,&cd)) {
		t.x = (caldate_mjd(&cd) + 1) * 86400ULL + 4611686014920671114ULL + leaps++;
		tai_pack(x,&t);
		fwrite(x,TAI_PACK,1,stdout);
	    }
    
    exit(0);
}
