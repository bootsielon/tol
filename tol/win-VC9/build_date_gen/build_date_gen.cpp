// build_date_gen.cpp: define el punto de entrada de la aplicación de consola.
//

#include <stdio.h>
#include <string.h>
#include <time.h>


int main()
{
/*
struct tm {
   int tm_sec;
   int tm_min;
   int tm_hour;
   int tm_mday;
   int tm_mon;
   int tm_year;
   int tm_wday;
   int tm_yday;
   int tm_isdst;
*/
  time_t t = time(0);
  struct tm* timeStruct = gmtime(&t);
  FILE* f = fopen("tol_build_date.h","w");
  if(f)
  {
    fprintf(f,
"/* build_date.h: Defines date and time of Tol generation. \n"
"          GNU/TOL Language. \n"
" \n"
"   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU]) \n"
" \n"
"   This program is free software; you can redistribute it and/or modify \n"
"   it under the terms of the GNU General Public License as published by \n"
"   the Free Software Foundation; either version 2, or (at your option) \n"
"   any later version. \n"
" \n"
"   This program is distributed in the hope that it will be useful, \n"
"   but WITHOUT ANY WARRANTY; without even the implied warranty of \n"
"   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n"
"   GNU General Public License for more details. \n"
" \n"
"   You should have received a copy of the GNU General Public License \n"
"   along with this program; if not, write to the Free Software \n"
"   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, \n"
"   USA. \n"
"*/"
" \n"
"////////////////////////////////////////////////////////////////////////////////////\n"
"// DEFINES\n"
"////////////////////////////////////////////////////////////////////////////////////\n");
    fprintf(f,"#define __TOL_BUILD_DATE__ \"%04ld-%02ld-%02ld\"\n",
            timeStruct->tm_year+1900,
            timeStruct->tm_mon+1,
            timeStruct->tm_mday);
    fprintf(f,"#define __TOL_BUILD_TIME__ \"%02ld:%02ld:%02ld\"\n",
            timeStruct->tm_hour,
            timeStruct->tm_min,
            timeStruct->tm_sec);
  }
}

