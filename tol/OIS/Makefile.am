## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2008 Bayes Inference, SA
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

EXTRA_DIST	= Jamfile

if OIS
  noinst_LTLIBRARIES	= libois.la
  pkginclude_HEADERS	= tol_oiscreator.h tol_ois.h tol_oisloader.h \
			tol_oisstream.h tol_oisstream_dir.h \
			tol_oisstream_zip.h
  libois_la_SOURCES	= oisapitol.cpp ois.cpp oisindex.cpp \
			oisloader.cpp oiscompress.cpp oiscreator.cpp  \
			oisinteractive.cpp oisxml.cpp oisstream.cpp \
			oisstream_dir.cpp oisstream_zip.cpp
endif

