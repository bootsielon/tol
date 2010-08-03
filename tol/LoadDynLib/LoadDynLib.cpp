/* LoadDynLib.cpp: A protocol for defining C++ built-in TOL entities over 
dynamic link libraries (.so, .dll) and an automatic cross-platform loading 
system which will be called from TOL.

This file implements the global built-in function used to load libraries.

            GNU/TOL Language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bnameblock.h>
#include <tol/tol_btxtgra.h>


//--------------------------------------------------------------------
DeclareContensClass(BNameBlock, BNameBlockTemporary, BLoadDynLib);
DefExtOpr(1, BLoadDynLib, "LoadDynLib", 1, 1, "Set",
 "(Text libraryPath)",
 I2("Returns a NameBlock that contains methods and members written "
    "in C + + in a precompiled library for dynamic linking. Once "
    "loaded Nameblock is nothing particular and can be used like "
    "any other object created with language TOL. See more details on ",
    "Devuelve un NameBlock que contiene métodos y miembros escritos "
    "en C++ en una biblioteca previamente compilada para su enlace "
    "dinámico. Una vez cargado el Nameblock no tiene nada de "
    "particular y puede ser usado como cualquier otro objeto creado "
    "con el lenguaje TOL. Vea más detalles en ")+"\n"
 "https://www.tol-project.org/wiki/LoadDynLib",
 BOperClassify::System_);
//--------------------------------------------------------------------
void BLoadDynLib::CalcContens()
//--------------------------------------------------------------------
{
  BText& libraryPath = Text(Arg(1));
  
}