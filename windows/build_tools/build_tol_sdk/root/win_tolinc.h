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
