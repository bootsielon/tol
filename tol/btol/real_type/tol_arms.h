/* tol_arms.h: header file for arms function.
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


int arms_simple (int ninit, double *xl, double *xr,
	         double (*myfunc)(double x, void *mydata), void *mydata,
                 int dometrop, double *xprev, double *xsamp);

int arms (double *xinit, int ninit, double *xl, double *xr,
	 double (*myfunc)(double x, void *mydata), void *mydata,
         double *convex, int npoint, int dometrop, double *xprev, double *xsamp,
         int nsamp, double *qcent, double *xcent, int ncent,
         int *neval);

double expshift(double y, double y0);

#define YCEIL 50.                /* maximum y avoiding overflow in exp(y) */

