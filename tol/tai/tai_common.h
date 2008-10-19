/* tai_common.h: Common MACRO descriptions to Tai Library sources

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])
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

#ifndef TAI_COMMON_H
#define TAI_COMMON_H 1

#undef BEGIN_DECLS
#undef END_DECLS
#ifdef __cplusplus
#  define BEGIN_DECLS extern "C" {
#  define END_DECLS   }
#else
#  define BEGIN_DECLS /* empty */
#  define END_DECLS   /* empty */
#endif /* __cplusplus */

#ifdef STATIC_BUILD
#   define DLLIMPORT
#   define DLLEXPORT
#else
#   if (defined(WIN32) && (defined(_MSC_VER) || (__BORLANDC__ >= 0x0550) || (defined(__GNUC__) && defined(__declspec)))) || (defined(MAC_TCL) && FUNCTION_DECLSPEC)
#	define DLLIMPORT __declspec(dllimport)
#	define DLLEXPORT __declspec(dllexport)
//no suitable definition provided for explicit template instantiation request
#pragma warning( disable : 4661)

//disable warnings on extern before template instantiation
#pragma warning (disable : 4231)
// identifier was truncated to '255' characters in the browser information
#pragma warning (disable : 4786)
#   else
#	define DLLIMPORT
#	define DLLEXPORT
#   endif
#endif

#ifdef TAI_API
#   undef TAI_API
#endif /* TAI_API */

#ifdef LIBTAI_EXPORTS
#    define TAI_API DLLEXPORT
#else
#    define TAI_API DLLIMPORT
#endif

#endif
