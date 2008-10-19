## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2008, Bayes Inference, SA
## Written by Daniel Rus Morales <daniel@uponeits.com>
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

EXTRA_DIST		= Jamfile llkarma.h

pkginclude_HEADERS	= tol_bdatgra.h tol_armseval.h tol_arms.h

noinst_LTLIBRARIES	= libreal.la
libreal_la_SOURCES	= arms.cpp armseval.cpp datgra.cpp \
			datgralg.cpp datgrapr.cpp datgrast.cpp \
			datgrav.cpp datgsrst.cpp llkarma.cpp \
			dat_ext_loader.cpp

libreal_la_CPPFLAGS	= -I$(top_srcdir)/bmath/gsl_ext


