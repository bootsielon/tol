/* tol_bmethodhook.h: Hooks of methods like Chart or Table
   GNU/TOL Language.

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

#ifndef TOL_BMETHODHOOK_H 
#define TOL_BMETHODHOOK_H 1

#include <tol/tol_bcommon.h>
#include <tol/tol_blist.h>

typedef void (*BMethodHookPtr)(BList * lst, const BText& FileName );

// Serie methods
TOL_API BMethodHookPtr InstallSerieChartHook(BMethodHookPtr Handler);
TOL_API BMethodHookPtr InstallSerieTableHook(BMethodHookPtr Handler);
TOL_API BMethodHookPtr InstallSerStatHook(BMethodHookPtr Handler);

// TimeSet methods
BMethodHookPtr InstallTMSChartHook(BMethodHookPtr Handler);

// Matrix methods
BMethodHookPtr InstallMatrixChartHook(BMethodHookPtr Handler);
BMethodHookPtr InstallMatrixTableHook(BMethodHookPtr Handler);

// Set methods
BMethodHookPtr InstallSetChartHook(BMethodHookPtr Handler);
BMethodHookPtr InstallSetTableHook(BMethodHookPtr Handler);

#endif /* TOL_BMETHODHOOK_H */
