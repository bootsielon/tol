/*************************************************************************
Minpack Copyright Notice (1999) University of Chicago.  All rights reserved

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the
following conditions are met:

1. Redistributions of source code must retain the above
copyright notice, this list of conditions and the following
disclaimer.

2. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials
provided with the distribution.

3. The end-user documentation included with the
redistribution, if any, must include the following
acknowledgment:

   "This product includes software developed by the
   University of Chicago, as Operator of Argonne National
   Laboratory.

Alternately, this acknowledgment may appear in the software
itself, if and wherever such third-party acknowledgments
normally appear.

4. WARRANTY DISCLAIMER. THE SOFTWARE IS SUPPLIED "AS IS"
WITHOUT WARRANTY OF ANY KIND. THE COPYRIGHT HOLDER, THE
UNITED STATES, THE UNITED STATES DEPARTMENT OF ENERGY, AND
THEIR EMPLOYEES: (1) DISCLAIM ANY WARRANTIES, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE
OR NON-INFRINGEMENT, (2) DO NOT ASSUME ANY LEGAL LIABILITY
OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR
USEFULNESS OF THE SOFTWARE, (3) DO NOT REPRESENT THAT USE OF
THE SOFTWARE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS, (4)
DO NOT WARRANT THAT THE SOFTWARE WILL FUNCTION
UNINTERRUPTED, THAT IT IS ERROR-FREE OR THAT ANY ERRORS WILL
BE CORRECTED.

5. LIMITATION OF LIABILITY. IN NO EVENT WILL THE COPYRIGHT
HOLDER, THE UNITED STATES, THE UNITED STATES DEPARTMENT OF
ENERGY, OR THEIR EMPLOYEES: BE LIABLE FOR ANY INDIRECT,
INCIDENTAL, CONSEQUENTIAL, SPECIAL OR PUNITIVE DAMAGES OF
ANY KIND OR NATURE, INCLUDING BUT NOT LIMITED TO LOSS OF
PROFITS OR LOSS OF DATA, FOR ANY REASON WHATSOEVER, WHETHER
SUCH LIABILITY IS ASSERTED ON THE BASIS OF CONTRACT, TORT
(INCLUDING NEGLIGENCE OR STRICT LIABILITY), OR OTHERWISE,
EVEN IF ANY OF SAID PARTIES HAS BEEN WARNED OF THE
POSSIBILITY OF SUCH LOSS OR DAMAGES.
*************************************************************************/

#ifndef _levenbergmarquardt_h
#define _levenbergmarquardt_h

#include "ap.h"

/*-----------------------------------------------
This routines must be defined by you:

void funcvecjac(const ap::real_1d_array& x,
     ap::real_1d_array& fvec,
     ap::real_2d_array& fjac,
     int& iflag);
-----------------------------------------------*/

/*************************************************************************
The subroutine minimizes the sum of squares  of  M nonlinear finctions  of
N  arguments  with  Levenberg-Marquardt  algorithm  using  Jacobian    and
information about function values.

Programmer  should  redefine  FuncVecJac  subroutine  which  takes array X
(argument)  whose  index  ranges  from  1 to N as an input and if variable
IFlag is equal to:
    * 1, returns vector of function values in array FVec (in elements from
      1 to M), not changing FJac.
    * 2,  returns  Jacobian  in  array FJac (in elements [1..M,1..N]), not
      changing FVec.
The subroutine can change the IFlag parameter by setting it into a negative
number. It will terminate program.

Programmer  can  also  redefine  LevenbergMarquardtNewIteration subroutine
which is called on each new step.   Current  point  X  is  passed into the
subroutine.   It  is  reasonable  to  redefine  the  subroutine for better
debugging, for example, to visualize the solution process.

The AdditionalLevenbergMarquardtStoppingCriterion could  be  redefined  to
modify stopping conditions.

Input parameters:
    N       –   number of unknowns, N>0.
    M       –   number of summable functions, M>=N.
    X       –   initial solution approximation.
                Array whose index ranges from 1 to N.
    EpsG    –   stopping criterion. Iterations are stopped, if  cosine  of
                the angle between vector of function values  and  each  of
                the  Jacobian  columns  if  less or equal EpsG by absolute
                value. In fact this value defines stopping condition which
                is based on the function gradient smallness.
    EpsF    –   stopping criterion. Iterations are  stopped,  if  relative
                decreasing of sum of function values squares (real and
                predicted on the base of extrapolation)  is  less or equal
                EpsF.
    EpsX    –   stopping criterion. Iterations are  stopped,  if  relative
                change of solution is less or equal EpsX.
    MaxIts  –   stopping  criterion.  Iterations  are  stopped,  if  their
                number exceeds MaxIts.

Output parameters:
    X       –   solution
                Array whose index ranges from 1 to N.
    Info    –   a reason of a program completion:
                    * -1 wrong parameters were specified,
                    * 0 interrupted by user,
                    * 1 relative decrease of sum of function values
                        squares (real and predicted on the base  of
                        extrapolation) is less or equal EpsF.
                    * 2 relative change of solution is less or equal
                        EpsX.
                    * 3 conditions (1) and (2) are fulfilled.
                    * 4 cosine of the angle between vector of function
                        values and each of the Jacobian columns is less
                        or equal EpsG by absolute value.
                    * 5 number of iterations exceeds MaxIts.
                    * 6 EpsF is too small.
                        It is impossible to get a better result.
                    * 7 EpsX is too small.
                        It is impossible to get a better result.
                    * 8 EpsG is too small. Vector of functions is
                        orthogonal to Jacobian columns with near-machine
                        precision.
argonne national laboratory. minpack project. march 1980.
burton s. garbow, kenneth e. hillstrom, jorge j. more

Contributors:
    * Sergey Bochkanov (ALGLIB project). Translation from FORTRAN to
      pseudocode.
*************************************************************************/
void levenbergmarquardtminimize(const int& n,
     const int& m,
     ap::real_1d_array& x,
     const double& epsg,
     const double& epsf,
     const double& epsx,
     const int& maxits,
     int& info);


#endif
