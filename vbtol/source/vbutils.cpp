/* tclutils.cpp: This file define a set of utils functions.
                 GNU/tolTcl Library

   Copyright (C) 2001, 2002, 2003 - Bayes Decisión, SL

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

#include "VButils.h"

/*
  Convert a Character Array to a BSTR String
*/

BSTR ChartoBSTR(char* str)
{
  int i, Len = strlen(str);
  unsigned short* FinalStr = new unsigned short[Len + 1];

  for (i=0; i<Len; i++)
  {
    unsigned char Part = str[i];
    FinalStr[i] = Part;
    FinalStr[i+1] = '\0';
  }
  return((BSTR)FinalStr);
}

FN_PTR VB_Writer = NULL;

void VBTol_gsl_error_handler(const char * reason, const char * file, int line,
	 						 int gsl_errno)
{
  char * buf = new char[strlen(reason)+25];

  sprintf(buf, "GSL Error # %-5d: %s", gsl_errno, reason);
  if (VB_Writer) {
    BSTR temp = ChartoBSTR(buf);
    BSTR ret = SysAllocString(temp);
    delete[] temp;
    VB_Writer(ret);
  }
  else
    ::MessageBox(NULL, buf, "C++ DLL", 0);
  delete[] buf;
}

void VBTol_HciWriter(const BText & str)
{
  if (VB_Writer) {
    BSTR temp = ChartoBSTR(((BText&)str).Buffer());
    BSTR ret = SysAllocString(temp);
    delete[] temp;
    VB_Writer(ret);
  }
  else
    ::MessageBox(NULL, ((BText&)str).Buffer(), "C++ DLL", 0);
}
