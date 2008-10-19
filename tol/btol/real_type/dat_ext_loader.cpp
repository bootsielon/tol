/* dat_ext_loader.cpp: Real Built-In Functions for Extensions Loading

   Copyright (C) 2006 - Bayes Decision, SL (Spain [EU])
   author: Daniel Rus <danirus@tol-project.org>

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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>

#ifdef __USE_EXT_LOADER__
//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDat_Lib_Load);
DefExtOpr(1, BDat_Lib_Load, "ExtLoad", 1, 1, "Text",
	  "(Text tolExt)",
	  I2("Try to load the Tol Extension given as argument and returns 1 "
	     "whether it was successful.\n"
	     "Parameter \"tolExt\" must be the name of the extension.\n"
	     "Tol looks for Extensions in each directory listed in "
	     "TOL_EXT_PATH environment variable.\n",
	     "Intenta cargar la Extensión de Tol indicada como parámetro, "
	     "devolviendo 1 en caso de tener éxito.\n"
	     "El parámetro \"tolExt\" debe ser el nombre de la extensión.\n"
	     "Tol busca las Extensiones en cada uno de los directorios "
	     "listados en la variable de entorno TOL_EXT_PATH.\n"),
	  BOperClassify::Extension_);
//--------------------------------------------------
void BDat_Lib_Load::CalcContens()
{
    BText extName = Text(Arg(1));
    contens_ = BGrammar::Ext_Load(extName);
}

//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDat_Lib_Unload);
DefExtOpr(1, BDat_Lib_Unload, "ExtUnload", 1, 1, "Text",
	  "(Text tolExt)",
	  I2("Try to unload the Tol Extension given as argument and returns 1 "
	     "whether it was successful.\n"
	     "Parameter \"tolExt\" must be the name of the extension.\n",
	     "Intenta descargar la Extensión de Tol indicada como parámetro, "
	     "devolviendo 1 en caso de tener éxito.\n"
	     "El parámetro \"tolExt\" debe ser el nombre de la extensión.\n"),
	  BOperClassify::Extension_);
//--------------------------------------------------
void BDat_Lib_Unload::CalcContens()
{
    BText extName = Text(Arg(1));
    contens_ = BGrammar::Ext_Unload(extName); 
}

#endif
