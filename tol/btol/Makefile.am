## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003-2005 Bayes Decision, SL
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

EXTRA_DIST = Jamfile

SUBDIRS = bdb bgrammar bgsl bmodel bmonte code_type complex_type date_type matrix_type vmatrix_type polynomial_type ratio_type real_type serie_type set_type text_type timeset_type

SUBLIBS	= bdb/libbdb.la bgrammar/libbgrammar.la bgsl/libbgsl.la bmodel/libbmodel.la bmonte/libbmonte.la code_type/libcode.la complex_type/libcomplex.la date_type/libdate.la vmatrix_type/libvmatrix.la matrix_type/libmatrix.la polynomial_type/libpolynomial.la ratio_type/libratio.la real_type/libreal.la serie_type/libserie.la set_type/libset.la text_type/libtext.la timeset_type/libtimeset.la

noinst_LTLIBRARIES	= libbtol.la
pkginclude_HEADERS	= tol_delay_init.h tol_bsparse.h
libbtol_la_SOURCES	= delay_init.cpp
libbtol_la_LIBADD	= $(SUBLIBS)
