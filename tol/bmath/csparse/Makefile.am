## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Bayes Decision, SL
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

noinst_LTLIBRARIES	= libcspack.la

libcspack_la_SOURCES	= \
cs_add.c        cs_dupl.c    cs_lsolve.c    cs_post.c     cs_tdfs.c      \
cs_amd.c        cs_entry.c   cs_ltsolve.c   cs_print.c    cs_transpose.c \
cs_chol.c       cs_etree.c   cs_lu.c        cs_pvec.c     cs_triplet.c   \
cs_cholsol.c    cs_fkeep.c   cs_lusol.c     cs_qr.c       cs_updown.c    \
cs_counts.c     cs_gaxpy.c   cs_malloc.c    cs_qrsol.c    cs_usolve.c    \
cs_cumsum.c     cs.h         cs_maxtrans.c  cs_scatter.c  cs_util.c      \
cs_dfs.c        cs_happly.c  cs_multiply.c  cs_scc.c      cs_utsolve.c   \
cs_dmperm.c     cs_house.c   cs_norm.c      cs_schol.c                   \
cs_droptol.c    cs_ipvec.c   cs_permute.c   cs_sqr.c                     \
cs_dropzeros.c  cs_load.c    cs_pinv.c      cs_symperm.c

INCLUDES = -I$(top_srcdir)/bmath/csparse
