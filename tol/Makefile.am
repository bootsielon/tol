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
## along with this program; if not, write to the Free Software Foundation, 
## Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

auxdir			= @ac_aux_dir@
AUX_DIST		= $(auxdir)/config.guess \
			$(auxdir)/config.sub \
			$(auxdir)/install-sh \
			$(auxdir)/ltmain.sh \
			$(auxdir)/missing \
			$(auxdir)/mkinstalldirs

EXTRA_DIST		= bootstrap common-build replace stdlib \
			ChangeLog win-VC8 Jamroot config.jam

if OIS
  OIS_SUBDIR = OIS
  OIS_SUBLIB = OIS/libois.la
endif

SUBDIRS			= tol bbasic bparser btol bmath $(OIS_SUBDIR) \
			stdlib contrib lang

SUBLIBS			= bbasic/libbasic.la \
			bparser/libbparser.la \
			btol/libbtol.la \
			bmath/libbmath.la \
			$(OIS_SUBLIB) \
			lang/liblang.la \
			contrib/libcontrib.la

pkginclude_HEADERS	= config.h tol_bcommon.h tol_bmethodhook.h tol_init.h \
			tol_version_def.h

lib_LTLIBRARIES		= libtol1.1.7.la
libtol1_1_7_la_SOURCES	= init.cpp
libtol1_1_7_la_LIBADD	= $(SUBLIBS)
libtol1_1_7_la_CPPFLAGS =
libtol1_1_7_la_LDFLAGS	= -version-info $(TOL_LT_VERSION) $(dl_LDFLAGS)

## GSL and GSL-CBlas Flags
if HAVE_gsl_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(gsl_LDFLAGS)
endif
if HAVE_gslcblas_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(gslcblas_LDFLAGS)
endif

## BLAS Flags
if HAVE_blas_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(blas_LDFLAGS)
endif

## LAPACK Flags
if HAVE_lapack_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(lapack_LDFLAGS)
endif

## CHOLMOD Flags
if HAVE_cholmod_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(cholmod_LDFLAGS)
endif

## BZip2 Flags
if HAVE_bz2_LDFLAGS
  libtol1_1_7_la_LDFLAGS += $(bz2_LDFLAGS)
endif

## ZipArchive Flags
if HAVE_ziparch_CPPFLAGS
  libtol1_1_7_la_LDFLAGS += $(ziparch_LDFLAGS)
endif

bin_PROGRAMS		= tol117
tol117_SOURCES		= tol.cpp
tol117_LDFLAGS		= libtol1.1.7.la 
