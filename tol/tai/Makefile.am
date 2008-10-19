## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Bayes Decision, SL
## Makefile.am v0.1 courtesy of Daniel Rus <danirus@tol-project.org>
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

header-links: 
	for h in $(srcdir)/*.h; do \
	  if test -f $$h; then \
	    BASENAME=`basename $$h`; \
            if ! test -f $$BASENAME; then \
	      $(LN_S) -f $$h $$BASENAME; \
            fi; \
	  fi; \
        done;

all: all-am header-links

lib_LTLIBRARIES	= libtai.la
pkgincludedir	= $(includedir)/tai

pkginclude_HEADERS	= tai_common.h tai.h taia.h caldate.h  \
			  caltime.h uint64.h leapsecs.h

libtai_la_SOURCES	= tai_add.c tai_now.c tai_pack.c tai_sub.c      \
			  tai_unpack.c taia_add.c taia_approx.c         \
			  taia_fmtfrac.c taia_frac.c taia_half.c        \
			  taia_less.c taia_now.c taia_pack.c taia_sub.c \
			  taia_tai.o taia_unpack.c caldate_fmt.c        \
			  caldate_scan.c caldate_fmjd.c caldate_mjd.c   \
			  caldate_norm.c caldate_ster.c leapsecs_read.c \
			  leapsecs_init.c leapsecs_add.c leapsecs_sub.c \
			  caltime_fmt.c caltime_scan.c caltime_tai.c    \
			  caltime_utc.c

man_MANS		= tai.3 tai_now.3 tai_pack.3 taia.3 taia_now.3  \
			  taia_pack.3 caldate.3 caldate_mjd.3 caltime.3 \
			  caltime_tai.3 leapsecs.3
EXTRA_DIST		= $(man_MANS)

INCLUDES          =  @INCLUDES@ -I$(top_srcdir)