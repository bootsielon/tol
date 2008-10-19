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

EXTRA_DIST		= Jamfile

noinst_LTLIBRARIES	= libmatrix.la

pkginclude_HEADERS	= tol_bmatgra.h tol_bmatrix.h tol_matrix.h \
			tol_bindex.h tol_bspamat.h tol_bltrimat.h \
			tol_butrimat.h tol_matrix_previous.h \
			tol_bdiagmat.h tol_bmatimp.h tol_gslmat.h \
			tol_diagmat.hpp tol_index.hpp tol_ltrimat.hpp \
			tol_mat.hpp tol_matopr.hpp tol_spamat.hpp \
			tol_symmat.hpp tol_utrimat.hpp tol_bsymmat.h \
			tol_binnerpoint.h

libmatrix_la_SOURCES	= matgra.cpp gibbssampler.cpp multimin.cpp \
			tol_innerpoint.cpp

libmatrix_la_CPPFLAGS = -I$(top_srcdir)/bmath/gsl_ext

