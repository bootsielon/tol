## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2008 Bayes Inference, SA
## Written Daniel Rus Morales <daniel@uponeits.com>
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

noinst_LTLIBRARIES	= libblinalg.la
pkginclude_HEADERS	= tol_blinalg.h tol_bmfstpro.h tol_bsvdsep.h \
			tol_blas.h tol_lapack.h
libblinalg_la_SOURCES	= divconq.cpp linalg.cpp matimp.cpp \
			mfastpro.cpp svdsep.cpp tol_blas.cpp tol_lapack.cpp
