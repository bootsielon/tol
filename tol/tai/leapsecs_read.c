/* leapsecs_read.c: TAI Library

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

#include <config.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
extern int errno;
#include <tai/tai.h>
#include <tai/leapsecs.h>

#ifdef WIN32
# include <io.h>
# include <stdlib.h>
#endif

struct tai *leapsecs = 0;
int leapsecs_num = 0;

#define DATFILE  LEAPSECS_DATFILE_DIR##/leapsecs.dat
#define PATHFILE "DATFILE"

#ifdef WIN32 

int leapsecs_read()
{
    int fd;
    struct _stat st;
    struct tai *t;
    int n;
    int i;
    struct tai u;
    
    fd = _open(PATHFILE, _O_RDONLY);
    if (fd == -1) {
	if (errno != ENOENT) return -1;
	if (leapsecs) free(leapsecs);
	leapsecs = 0;
	leapsecs_num = 0;
	return 0;
    }

    if (_fstat(fd,&st) == -1) { _close(fd); return -1; }
    
    t = (struct tai *) malloc(st.st_size);
    if (!t) { _close(fd); return -1; }
    
    n = _read(fd,(char *) t,st.st_size);
    _close(fd);
    if (n != st.st_size) { free(t); return -1; }
    
    n /= sizeof(struct tai);
    
    for (i = 0;i < n;++i) {
	tai_unpack((char *) &t[i],&u);
	t[i] = u;
    }
    
    if (leapsecs) free(leapsecs);
    
    leapsecs = t;
    leapsecs_num = n;
}

#else 

int leapsecs_read()
{
    int fd;
    struct stat st;
    struct tai *t;
    int n;
    int i;
    struct tai u;
    
//  fd = open("/etc/leapsecs.dat",O_RDONLY | O_NDELAY);
//  fd = open("LEAPSECS_DATFILE_DIR/leapsecs.dat",O_RDONLY | O_NDELAY);
    fd = open(PATHFILE, O_RDONLY | O_NDELAY);
    if (fd == -1) {
	if (errno != ENOENT) return -1;
	if (leapsecs) free(leapsecs);
	leapsecs = 0;
	leapsecs_num = 0;
	return 0;
    }
    
    if (fstat(fd,&st) == -1) { close(fd); return -1; }
    
    t = (struct tai *) malloc(st.st_size);
    if (!t) { close(fd); return -1; }
    
    n = read(fd,(char *) t,st.st_size);
    close(fd);
    if (n != st.st_size) { free(t); return -1; }
    
    n /= sizeof(struct tai);
    
    for (i = 0;i < n;++i) {
	tai_unpack((char *) &t[i],&u);
	t[i] = u;
    }
    
    if (leapsecs) free(leapsecs);
    
    leapsecs = t;
    leapsecs_num = n;
}

#endif
