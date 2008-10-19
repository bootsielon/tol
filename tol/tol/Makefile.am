## Makefile.am -- Process this file with automake to produce Makefile.in
## Copyright (C) 2003 Bayes Decisión, SL
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

#hs = tol_bcommon.h config.h
#phs = $(patsubst %.h, %.h.gch, $(hs))

#bbasic_hs = tol_cbbasic.h tol_bcore.h tol_batom.h tol_btext.h tol_bobject.h tol_butil.h tol_bdate.h tol_bdir.h tol_blist.h tol_list.h tol_tree.h tol_bopt.h tol_bout.h tol_bseed.h tol_bsys.h tol_btimer.h tol_barray.h tol_bqfile.h tol_bexc.h tol_bfsmem.h tol_memhandler.h tol_btimer.h
#bbasic_phs = $(patsubst %.h, %.h.gch, $(bbasic_hs))

#bparser_hs = tol_btoken.h tol_bfilter.h tol_bscanner.h tol_bparser.h
#bparser_phs = $(patsubst %.h, %.h.gch, $(bparser_hs))

#btol_hs = tol_bgrammar.h tol_bmethod.h tol_boper.h tol_bspfun.h tol_bstruct.h tol_bsyntax.h tol_btxthash.h tol_bgencon.h tol_bgenobj.h tol_bgentmp.h tol_bext.h tol_bgrastk.h tol_bdb.h tol_bdspool.h tol_bmatgra.h tol_bmatrix.h tol_matrix.h tol_bindex.h tol_bspamat.h tol_bltrimat.h tol_butrimat.h tol_bdiagmat.h tol_bmatimp.h tol_bsymmat.h tol_gslmat.h tol_bratio.h tol_bratgra.h tol_bsparse.h tol_bset.h tol_bsetgra.h tol_bdatgra.h tol_armseval.h tol_arms.h tol_btmset.h tol_btmsgra.h tol_btmsgrav.h tol_bdayinfo.h tol_bmoninfo.h tol_btmsexc.h tol_bdtegra.h tol_btxtgra.h tol_bcode.h tol_bcodgra.h tol_bcmpgra.h tol_bpolgra.h tol_btmser.h tol_btsrgra.h tol_btsrgrav.h tol_btsrgrai.h tol_btsrgrp.h tol_gsl.h tol_bmodel.h tol_baia.h tol_bmaia.h tol_bestim.h
#btol_phs = $(patsubst %.h, %.h.gch, $(btol_hs))

#bmath_hs = tol_bmatfun.h tol_bdat.h tol_bmonome.h tol_bpolyn.h tol_bcomplex.h tol_cdflib.h tol_barith.h tol_bfibonac.h tol_brealfun.h tol_bfunct.h tol_blinalg.h tol_bmfstpro.h tol_bsvdsep.h tol_bprdist.h tol_bmultvar.h tol_bar.h tol_barma.h tol_blinreg.h tol_blogit.h tol_bstat.h
#bmath_phs = $(patsubst %.h, %.h.gch, $(bmath_hs))

#lang_hs = tol_blanguag.h tol_exportfu.h
#lang_phs = $(patsubst %.h, %.h.gch, $(lang_hs))

#ois_hs = tol_oiscreator.h tol_ois.h tol_oisloader.h tol_oisstream.h tol_oisstream_dir.h
#ois_phs = $(patsubst %.h, %.h.gch, $(lang_hs))

header-links: remove-links
	$(LN_S) -f ../config.h config.h; \
	for h in $(top_srcdir)/tol*.h; do \
	  if test -f $$h; then \
	    BASENAME=`basename $$h`; \
	    $(LN_S) -f $$h $$BASENAME; \
	  fi; \
        done; \
	DIRLIST="bbasic bmath bparser btol OIS lang"; \
	for d in $$DIRLIST; do \
	  HEADERLIST="$(top_srcdir)/$$d/tol*.h $(top_srcdir)/$$d/*/tol*.h $(top_srcdir)/$$d/*/tol*.hpp"; \
	  for h in $$HEADERLIST; do \
            if test -f $$h; then \
	      BASENAME=`basename $$h`; \
	      $(LN_S) -f $$h $$BASENAME; \
	    fi; \
	  done; \
	done

#precomp-headers: \
#	$(phs) $(bbasic_phs) $(bparser_phs) \
#	$(btol_phs) $(bmath_phs) $(lang_phs) $(ois_phs)

#clean-precomp:
#	$(shell rm -f *.h.gch)

remove-links: 
	-(rm -f tol*.h tol*.hpp config.h)

all: all-am header-links

clean: clean-am remove-links #clean-precomp
distclean: distclean-am remove-links

#%.h.gch : %.h
#	$(CXX) -c $< -DHAVE_CONFIG_H -I$(top_builddir) -o $@
