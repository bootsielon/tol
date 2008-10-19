## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2007, Bayes Decision, SL
## Author: Jorge <josp@tol-project.org>
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

INCLUDES 		= -I$(top_srcdir)/contrib/kmlocal
EXTRA_DIST		= Jamfile

noinst_LTLIBRARIES	= libkmlocal.la
pkginclude_HEADERS 	= KCtree.h KM_ANN.h KMdata.h \
			KMfilterCenters.h KMrand.h KCutil.h \
			KMcenters.h KMeans.h KMlocal.h KMterm.h
libkmlocal_la_SOURCES	= \
KCtree.cpp  KM_ANN.cpp     KMdata.cpp  KMfilterCenters.cpp  KMrand.cpp  \
KCutil.cpp  KMcenters.cpp  KMeans.cpp  KMlocal.cpp          KMterm.cpp
