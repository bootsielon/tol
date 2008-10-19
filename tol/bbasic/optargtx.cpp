/* optargtx.cpp: text functions
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bopt.h>

//--------------------------------------------------------------------
BText BArg::Range()

/*! Returns a text explaining range of a numeric option (range: 1 4).
 *  Returns a empty text if the option isn't numeric.
 */
//--------------------------------------------------------------------
{
  BText range("");

  if(numeric_)
  { range=BText(" (")+I2("range ","rango ")+minimum_+" : "+maximum_+")"; }

  return(range);
}


//--------------------------------------------------------------------
BText BArg::Help(const BText& prefix)

/*! Returns a text explaining the option. Samples:
 *   [-uid <./uid/base.uid>]: User interface description file
 *   [-app <./app-defaults/fornews.uid>]: Application colors and resources
 *   [-tol <arg>]: Tol lenguage code source file
 *   [-lan <english>]: Language english or spanish
 *   [-bdi]: Let data base access
 *   -c <3>: Number of columns (range: 1 4)
 */
//--------------------------------------------------------------------
{
  BText txt(prefix);
  BText aux(Name());

  if(parametric_)
  {
    if(default_.HasName()) { aux=aux+" <"+default_+">"; }
    else		   { aux=aux+" <arg>"; }
  }

  if(!mandatory_) { aux=BText("[")+aux+"]: "; }
  else		  { aux=aux+": "; }

  txt+=aux+description_;

  return(txt+Range()+"\n");
}


//--------------------------------------------------------------------
BText BArg::State(const BText& prefix)

/*! Returns a text with the final sate of one option. Samples:
 *      [-uid <./uid/base.uid>]
 *      [-app <./app-defaults/fornews.uid>]
 *      [-tol <arg>]
 *      [-lan <english>]
 *      [-bdi <no>]
 *      [-motif <yes>]
 *      [-c <4>]
 *      [./tol/input1.tol, ./tol/input2.tol/]
 */
//--------------------------------------------------------------------
{
  BText txt(prefix);
  BText aux(Name());


  if(parametric_) { aux=aux+" <"+parameter_+">"; }
  else
  {
    if(appear_) { aux=aux+" <"+I2("yes","si")+">"; }
    else	{ aux=aux+" <"+I2("no" ,"no")+">"; }
  }
  aux=BText("[")+aux+"]";

  return(txt+aux+"\n");
}

