/* opttext.cpp: text functions
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
BText BOpt::Help()

/*! Returns a text explaining all the options.
 */
//--------------------------------------------------------------------
{
  BText txt(BText("\n")+Name()+", "+I2("arguments description",
				       "descripcion de los argumentos")+":\n");

  for(BInt i=0; i<NumOptions(); i++) { txt+=optArg_[i].Help(margin_); }

  if(FreeLet())
  {
    txt=txt+"  "+I2("Free arguments are permited",
		    "Se permiten argumentos libres")+"\n";
  }

  if(samples_.HasName())
  {
    txt=txt+I2("Use samples:",
	       "Ejemplos de uso:")+"\n"+samples_;
  }
  return(txt+copyright_+"\n");
}


//--------------------------------------------------------------------
BText BOpt::State()

/*! Returns a text with the final values of the options.
 */
//--------------------------------------------------------------------
{
  BText txt(Name()+":\n");

  for(BInt i=0; i<NumOptions(); i++) { txt+=optArg_[i].State(margin_); }

  if(FreeLet() && (FreeNum()>0))
  {
    txt=txt+"  ["+freeArg_[0].Name();
    for(BInt i=1; i<FreeNum(); i++) { txt=txt+", "+freeArg_[i].Name(); }
    txt=txt+"]\n";
  }
  return(txt);
}

