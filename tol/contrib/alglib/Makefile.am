## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2007, Bayes Decision, SL
## Author: Daniel Rus Morales <danirus@tol-project.org>
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

noinst_LTLIBRARIES	= libalglib.la

pkginclude_HEADERS 	= ap.h polinterpolation.h ratinterpolation.h spline3.h \
			bdsvd.h bidiagonal.h blas.h leastsquares.h \
			levenbergmarquardt.h lq.h qr.h reflections.h \
			rotations.h  stdafx.h  svd.h mannwhitneyu.h normaldistr.h \
			studentttests.h variancetests.h wsr.h binomialdistr.h  \
			chisquaredistr.h correlation.h correlationtests.h fdistr.h  \
			gammaf.h ibetaf.h igammaf.h jarquebera.h nearunityunit.h  \
			stest.h studenttdistr.h 

libalglib_la_SOURCES	= alglib_interp.cpp ap.cpp polinterpolation.cpp \
			ratinterpolation.cpp spline3.cpp bdsvd.cpp \
			bidiagonal.cpp blas.cpp leastsquares.cpp \
			levenbergmarquardt.cpp lq.cpp qr.cpp reflections.cpp  \
			rotations.cpp svd.cpp \
			alglib_statistics.cpp mannwhitneyu.cpp normaldistr.cpp \
			binomialdistr.cpp chisquaredistr.cpp correlation.cpp  \
			correlationtests.cpp fdistr.cpp gammaf.cpp ibetaf.cpp  \
			igammaf.cpp jarquebera.cpp nearunityunit.cpp stest.cpp  \
			studenttdistr.cpp studentttests.cpp variancetests.cpp  \
			wsr.cpp

INCLUDES 		= -I$(top_srcdir)/contrib/alglib
