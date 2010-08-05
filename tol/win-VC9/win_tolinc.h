/* tol_bcommon.h: Common MACRO descriptions to all sources of 
                  GNU/TOL Language

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
 */

#ifndef __TOLINC_H__
#define __TOLINC_H__

#pragma inline_recursion(on)


#pragma include_alias(<ext/hash_map>, <hash_map>)
#pragma include_alias(<tol/tol_hash_map.h>, <tol_hash_map.h>)

#pragma include_alias(<tol/config.h>, <config.h>)
#pragma include_alias(<tol/tol_version_def.h>, <tol_version_def.h>)
#pragma include_alias(<tol/tol_bcommon.h>, <tol_bcommon.h>)
#pragma include_alias(<tol/tol_blanguag.h>, <tol_blanguag.h>)
#pragma include_alias(<tol/tol_bmethodhook.h>, <tol_bmethodhook.h>)
#pragma include_alias(<tol/tol_exportfu.h>, <tol_exportfu.h>)
#pragma include_alias(<tol/tol_init.h>, <tol_init.h>)
#pragma include_alias(<tol/tol_bexc.h>, <tol_bexc.h>)
#pragma include_alias(<tol/tol_bfsmem.h>, <tol_bfsmem.h>)
#pragma include_alias(<tol/tol_memhandler.h>, <tol_memhandler.h>)
#pragma include_alias(<tol/tol_barray.h>, <tol_barray.h>)
#pragma include_alias(<tol/tol_batom.h>, <tol_batom.h>)
#pragma include_alias(<tol/tol_bcore.h>, <tol_bcore.h>)
#pragma include_alias(<tol/tol_bdate.h>, <tol_bdate.h>)
#pragma include_alias(<tol/tol_bdir.h>, <tol_bdir.h>)
#pragma include_alias(<tol/tol_blist.h>, <tol_blist.h>)
#pragma include_alias(<tol/tol_bobject.h>, <tol_bobject.h>)
#pragma include_alias(<tol/tol_bopt.h>, <tol_bopt.h>)
#pragma include_alias(<tol/tol_bout.h>, <tol_bout.h>)
#pragma include_alias(<tol/tol_bqfile.h>, <tol_bqfile.h>)
#pragma include_alias(<tol/tol_bseed.h>, <tol_bseed.h>)
#pragma include_alias(<tol/tol_bsys.h>, <tol_bsys.h>)
#pragma include_alias(<tol/tol_btext.h>, <tol_btext.h>)
#pragma include_alias(<tol/tol_btimer.h>, <tol_btimer.h>)
#pragma include_alias(<tol/tol_butil.h>, <tol_butil.h>)
#pragma include_alias(<tol/tol_list.h>, <tol_list.h>)
#pragma include_alias(<tol/tol_tree.h>, <tol_tree.h>)
#pragma include_alias(<tol/tol_classify.h>, <tol_classify.h>)
#pragma include_alias(<tol/tol_bmatfun.h>, <tol_bmatfun.h>)
#pragma include_alias(<tol/tol_cbmath.h>, <tol_cbmath.h>)
#pragma include_alias(<tol/tol_barith.h>, <tol_barith.h>)
#pragma include_alias(<tol/tol_bfibonac.h>, <tol_bfibonac.h>)
#pragma include_alias(<tol/tol_bfunct.h>, <tol_bfunct.h>)
#pragma include_alias(<tol/tol_brealfun.h>, <tol_brealfun.h>)
#pragma include_alias(<tol/tol_blas.h>, <tol_blas.h>)
#pragma include_alias(<tol/tol_lapack.h>, <tol_lapack.h>)
#pragma include_alias(<tol/tol_bvmat.h>, <tol_bvmat.h>)
#pragma include_alias(<tol/tol_bvmat_impl.h>, <tol_bvmat_impl.h>)
#pragma include_alias(<tol/tol_bvmat_bsr.h>, <tol_bvmat_bsr.h>)
#pragma include_alias(<tol/tol_bvmat_bsr_impl.h>, <tol_bvmat_bsr_impl.h>)
#pragma include_alias(<tol/tol_bvmatgra.h>, <tol_bvmatgra.h>)
#pragma include_alias(<tol/tol_blinalg.h>, <tol_blinalg.h>)
#pragma include_alias(<tol/tol_bmfstpro.h>, <tol_bmfstpro.h>)
#pragma include_alias(<tol/tol_bsvdsep.h>, <tol_bsvdsep.h>)
#pragma include_alias(<tol/tol_bmultvar.h>, <tol_bmultvar.h>)
#pragma include_alias(<tol/tol_bprdist.h>, <tol_bprdist.h>)
#pragma include_alias(<tol/tol_bprdist.h>, <tol_bprdist_internal.h>)
#pragma include_alias(<tol/tol_bar.h>, <tol_bar.h>)
#pragma include_alias(<tol/tol_barma.h>, <tol_barma.h>)
#pragma include_alias(<tol/tol_blinreg.h>, <tol_blinreg.h>)
#pragma include_alias(<tol/tol_blogit.h>, <tol_blogit.h>)
#pragma include_alias(<tol/tol_bltrimat.h>, <tol_bltrimat.h>)
#pragma include_alias(<tol/tol_bstat.h>, <tol_bstat.h>)
#pragma include_alias(<tol/tol_btest.h>, <tol_btest.h>)
#pragma include_alias(<tol/tol_bunitroot.h>, <tol_bunitroot.h>)
#pragma include_alias(<tol/tol_nortest.h>, <tol_nortest.h>)
#pragma include_alias(<tol/tol_cdflib.h>, <tol_cdflib.h>)
#pragma include_alias(<tol/tol_bcomplex.h>, <tol_bcomplex.h>)
#pragma include_alias(<tol/tol_bdat.h>, <tol_bdat.h>)
#pragma include_alias(<tol/tol_ieee.h>, <tol_ieee.h>)
#pragma include_alias(<tol/tol_bmonome.h>, <tol_bmonome.h>)
#pragma include_alias(<tol/tol_bpolyn.h>, <tol_bpolyn.h>)
#pragma include_alias(<tol/tol_bfilter.h>, <tol_bfilter.h>)
#pragma include_alias(<tol/tol_bparser.h>, <tol_bparser.h>)
#pragma include_alias(<tol/tol_bscanner.h>, <tol_bscanner.h>)
#pragma include_alias(<tol/tol_btoken.h>, <tol_btoken.h>)
#pragma include_alias(<tol/tol_cbparser.h>, <tol_cbparser.h>)
#pragma include_alias(<tol/tol_bsparse.h>, <tol_bsparse.h>)
#pragma include_alias(<tol/tol_cbtol.h>, <tol_cbtol.h>)
#pragma include_alias(<tol/tol_delay_init.h>, <tol_delay_init.h>)
#pragma include_alias(<tol/tol_bdb.h>, <tol_bdb.h>)
#pragma include_alias(<tol/tol_bdspool.h>, <tol_bdspool.h>)
#pragma include_alias(<tol/tol_filehandle.h>, <tol_filehandle.h>)
#pragma include_alias(<tol/tol_bgencon.h>, <tol_bgencon.h>)
#pragma include_alias(<tol/tol_bgenobj.h>, <tol_bgenobj.h>)
#pragma include_alias(<tol/tol_bgentmp.h>, <tol_bgentmp.h>)
#pragma include_alias(<tol/tol_bgrammar.h>, <tol_bgrammar.h>)
#pragma include_alias(<tol/tol_bgrastk.h>, <tol_bgrastk.h>)
#pragma include_alias(<tol/tol_bmethod.h>, <tol_bmethod.h>)
#pragma include_alias(<tol/tol_boper.h>, <tol_boper.h>)
#pragma include_alias(<tol/tol_bspfun.h>, <tol_bspfun.h>)
#pragma include_alias(<tol/tol_bnameblock.h>, <tol_bnameblock.h>)
#pragma include_alias(<tol/tol_bstruct.h>, <tol_bstruct.h>)
#pragma include_alias(<tol/tol_bclass.h>, <tol_bclass.h>)
#pragma include_alias(<tol/tol_bsyntax.h>, <tol_bsyntax.h>)
#pragma include_alias(<tol/tol_bsymboltable.h>, <tol_bsymboltable.h>)
#pragma include_alias(<tol/tol_gsl.h>, <tol_gsl.h>)
#pragma include_alias(<tol/tol_baia.h>, <tol_baia.h>)
#pragma include_alias(<tol/tol_bmaia.h>, <tol_bmaia.h>)
#pragma include_alias(<tol/tol_bestim.h>, <tol_bestim.h>)
#pragma include_alias(<tol/tol_bmodel.h>, <tol_bmodel.h>)
#pragma include_alias(<tol/tol_bsymmat.h>, <tol_bsymmat.h>)
#pragma include_alias(<tol/tol_bcode.h>, <tol_bcode.h>)
#pragma include_alias(<tol/tol_bcodgra.h>, <tol_bcodgra.h>)
#pragma include_alias(<tol/tol_bcmpgra.h>, <tol_bcmpgra.h>)
#pragma include_alias(<tol/tol_bdtegra.h>, <tol_bdtegra.h>)
#pragma include_alias(<tol/tol_bdiagmat.h>, <tol_bdiagmat.h>)
#pragma include_alias(<tol/tol_bindex.h>, <tol_bindex.h>)
#pragma include_alias(<tol/tol_gslmat.h>, <tol_gslmat.h>)
#pragma include_alias(<tol/tol_bmatgra.h>, <tol_bmatgra.h>)
#pragma include_alias(<tol/tol_bmatimp.h>, <tol_bmatimp.h>)
#pragma include_alias(<tol/tol_bmatrix.h>, <tol_bmatrix.h>)
#pragma include_alias(<tol/tol_bspamat.h>, <tol_bspamat.h>)
#pragma include_alias(<tol/tol_butrimat.h>, <tol_butrimat.h>)
#pragma include_alias(<tol/tol_matrix_previous.h>, <tol_matrix_previous.h>)
#pragma include_alias(<tol/tol_matrix.h>, <tol_matrix.h>)
#pragma include_alias(<tol/tol_bpolgra.h>, <tol_bpolgra.h>)
#pragma include_alias(<tol/tol_bratgra.h>, <tol_bratgra.h>)
#pragma include_alias(<tol/tol_bratio.h>, <tol_bratio.h>)
#pragma include_alias(<tol/tol_arms.h>, <tol_arms.h>)
#pragma include_alias(<tol/tol_armseval.h>, <tol_armseval.h>)
#pragma include_alias(<tol/tol_bdatgra.h>, <tol_bdatgra.h>)
#pragma include_alias(<tol/tol_btmser.h>, <tol_btmser.h>)
#pragma include_alias(<tol/tol_btsrgra.h>, <tol_btsrgra.h>)
#pragma include_alias(<tol/tol_btsrgrai.h>, <tol_btsrgrai.h>)
#pragma include_alias(<tol/tol_btsrgrav.h>, <tol_btsrgrav.h>)
#pragma include_alias(<tol/tol_btsrgrp.h>, <tol_btsrgrp.h>)
#pragma include_alias(<tol/tol_bset.h>, <tol_bset.h>)
#pragma include_alias(<tol/tol_bsetgra.h>, <tol_bsetgra.h>)
#pragma include_alias(<tol/tol_btxtgra.h>, <tol_btxtgra.h>)
#pragma include_alias(<tol/tol_bdayinfo.h>, <tol_bdayinfo.h>)
#pragma include_alias(<tol/tol_bmoninfo.h>, <tol_bmoninfo.h>)
#pragma include_alias(<tol/tol_btmset.h>, <tol_btmset.h>)
#pragma include_alias(<tol/tol_btmsexc.h>, <tol_btmsexc.h>)
#pragma include_alias(<tol/tol_btmsgra.h>, <tol_btmsgra.h>)
#pragma include_alias(<tol/tol_btmsgrav.h>, <tol_btmsgrav.h>)
#pragma include_alias(<tol/tol_diagmat.hpp>, <tol_diagmat.hpp>)
#pragma include_alias(<tol/tol_index.hpp>, <tol_index.hpp>)
#pragma include_alias(<tol/tol_ltrimat.hpp>, <tol_ltrimat.hpp>)
#pragma include_alias(<tol/tol_mat.hpp>, <tol_mat.hpp>)
#pragma include_alias(<tol/tol_matopr.hpp>, <tol_matopr.hpp>)
#pragma include_alias(<tol/tol_spamat.hpp>, <tol_spamat.hpp>)
#pragma include_alias(<tol/tol_symmat.hpp>, <tol_symmat.hpp>)
#pragma include_alias(<tol/tol_utrimat.hpp>, <tol_utrimat.hpp>)

//------------------------------------------
// Includes for TC: Continuous Time Project
#pragma include_alias(<tol/tol_bctime.h>, <tol_bctime.h>)
#pragma include_alias(<tol/tol_bctmigra.h>, <tol_bctmigra.h>)

#pragma include_alias(<tol/tol_bctimeset.h>, <tol_bctimeset.h>)
#pragma include_alias(<tol/tol_bctmsgra.h>, <tol_bctmsgra.h>)
#pragma include_alias(<tol/tol_bctmsbas.h>, <tol_bctmsbas.h>)
#pragma include_alias(<tol/tol_bctmsfnit.h>, <tol_bctmsfnit.h>)
#pragma include_alias(<tol/tol_bctmsbool.h>, <tol_bctmsbool.h>)
#pragma include_alias(<tol/tol_bctmsspec.h>, <tol_bctmsspec.h>)
#pragma include_alias(<tol/tol_bctmsexc.h>, <tol_bctmsexc.h>)

#pragma include_alias(<tol/tol_bcseries.h>, <tol_bcseries.h>)
#pragma include_alias(<tol/tol_bctsrgra.h>, <tol_bctsrgra.h>)
#pragma include_alias(<tol/tol_bctsrgrav.h>, <tol_bctsrgrav.h>)
//------------------------------------------

#pragma include_alias(<tol/tol_ois.h>, <tol_ois.h>)
#pragma include_alias(<tol/tol_oiscreator.h>, <tol_oiscreator.h>)
#pragma include_alias(<tol/tol_oisloader.h>, <tol_oisloader.h>)
#pragma include_alias(<tol/tol_oisstream.h>, <tol_oisstream.h>)
#pragma include_alias(<tol/tol_oisstream_dir.h>, <tol_oisstream_dir.h>)
#pragma include_alias(<tol/tol_oisstream_zip.h>, <tol_oisstream_zip.h>)

//inconsistent DLL linkage. dllexport assumed.
#pragma warning(disable : 4273)
// C++ exception specification ignored 
#pragma warning(disable : 4290)
//function should return a value; void return type assumed
#pragma warning(disable : 4541)
// template-class specialization is already instantiated.
#pragma warning(disable : 4660)
// not all control paths return a value
#pragma warning(disable : 4715)
 // symbol truncated to 255 characters
#pragma warning(disable : 4786)
//class ??? necesita tener una interfaz DLL para que la utilicen los clientes de class ???
#pragma warning(disable : 4251)
// '???': el símbolo se ha declarado como deprecated
//      c:\archivos de programa\microsoft visual studio 8\vc\include\string.h(205) : vea la declaración de '???'
//      Mensaje: 'The POSIX name for this item is deprecated. Instead, use the ISO C++ conformant name: _???. See online help for details.'
#pragma warning(disable : 4996)

//unsafe conversion from 'type of expression' to 'type required' 
//#pragma warning(error : 4191)


#define _CRT_SECURE_NO_DEPRECATE

#endif
