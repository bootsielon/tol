## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Bayes Decisión, SL
## Author: Daniel Rus <daniel@uponeits.com>
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

EXTRA_DIST		= Jamfile

noinst_LTLIBRARIES	= libbstat.la
pkginclude_HEADERS	= tol_bar.h tol_barma.h tol_blinreg.h \
			tol_blogit.h tol_bstat.h \
			tol_btest.h tol_bunitroot.h tol_nortest.h
libbstat_la_SOURCES	= ar.cpp arma.cpp logit.cpp stat.cpp \
			test.cpp unitroot.cpp linreg.cpp
