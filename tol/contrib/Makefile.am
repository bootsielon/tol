## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Bayes Decision, SL
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

EXTRA_DIST 		= Jamfile
SUBDIRS			= clusterlib kmlocal alglib optimal_bw

noinst_LTLIBRARIES	= libcontrib.la
libcontrib_la_SOURCES 	=
libcontrib_la_LIBADD 	= clusterlib/libcluster.la kmlocal/libkmlocal.la \
			alglib/libalglib.la optimal_bw/liboptimal_bw.la
