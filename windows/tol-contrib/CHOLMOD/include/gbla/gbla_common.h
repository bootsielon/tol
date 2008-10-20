/* gbla_types.h
 *
 * Copyright (C) 2005 Jorge Suit Perez Ronda
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __GBLA_TYPES_H__
#define __GBLA_TYPES_H__

#undef __GBLA_BEGIN_DECLS
#undef __GBLA_END_DECLS
#ifdef __cplusplus
# define __GBLA_BEGIN_DECLS extern "C" {
# define __GBLA_END_DECLS }
#else
# define __GBLA_BEGIN_DECLS /* empty */
# define __GBLA_END_DECLS /* empty */
#endif

#if defined(_MSC_VER)
# ifndef WIN32
#  define WIN32
# endif
# if defined(GBLA_SHARED)
#  if defined(GBLA_EXPORTS) 
#   define GBLA_API __declspec(dllexport)
#  else
#   define GBLA_API __declspec(dllimport)
#  endif
# else
#  define GBLA_API
# endif
#else
# define GBLA_API
#endif

#define GBLA_SUCCESS 0
#define GBLA_FAIL -1

#define GBLA_MAX(a,b) ((a) > (b) ? (a) : (b))
#define GBLA_MIN(a,b) ((a) < (b) ? (a) : (b))
#endif
