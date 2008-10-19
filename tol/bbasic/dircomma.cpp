/* dircomma.cpp: BDir (dir)
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

#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>

BTraceInit("dircomma.cpp");

//--------------------------------------------------------------------
BInt BDir::Command(      BBool trace,
			 BBool recursive,
		   const BText& command,
		   const BText& pattern)

/*! Applies a system command to all files within directory object,
 *  applies to those file than make math with the pattern. The power
 *  of this math is responsability of the function BText::Match().
 *  This command can work recursively.
 * \param trace show the operation to make
 * \param recursive to apply to the descending directories
 * \param command instruction to execute
 * \param pattern comparison text
 * \return returns the number of procesed files
 */
//--------------------------------------------------------------------
{
  BInt	counter=0;
  BText txt(Name());

  if(!Exist()) { Std(Name()+I2(": Don't exist",": No existe")+"\n"); }
  else
  {
    if(Name().Match(pattern))
    {
      BText order(command+" "+Name());
      if(trace) { Std(order+"\n"); }
      BSys::System(order);
      counter++;
    }
    else
    {
      if(trace) { Std(Name()+I2(": Does not match",": No equipara")+"\n"); }
    }
    if(IsDir())
    {
      if(Empty()) { Std(Name()+I2(": Is empty",": Esta vacio")+"\n"); }
      else
      {
    	if(trace) { Std(Name()+"\n"); }
    	if(NumFiles()>0)
        {
    	  for(BInt f=0; f<NumFiles(); f++)
    	  {
    	    BText name=Name()+TSLASH+FileName(f);
    	    if(FileName(f).Match(pattern))
     	    {
    	      BText order(command+" "+name);
    	      if(trace) { Std(order+"\n"); }
	          BSys::System(order);
			  counter++;
			}
    	    else
            {
 		      if(trace)
		      { Std(name+I2(": Does not match",": No equipara")+"\n"); }
		    }
		  }
		}
		if(NumDirs()>0)
		{
		  if(recursive)
		  {
		    for(BInt d=0; d<NumDirs(); d++)
		    {
		      BDir subDir(Name()+TSLASH+DirName(d));
		      counter += subDir.Command(trace,recursive,command,pattern);
		    }
		  }
		}
      }
    }
  }
  return(counter);
}
