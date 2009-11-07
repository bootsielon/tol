/* tol_ois_macros.h: Object's Indexed Serialization.
                       GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_OIS_MACROS_H
#define TOL_OIS_MACROS_H 1


//#define TRACE_OIS_HIERARCHY
#ifdef TRACE_OIS_HIERARCHY
static FILE* logHrchyWrite_ = 
  fopen((BSys::TolAppData()+"syslog/OisHierarchyLogWrite.log").String(),"w");
#endif

//#define TRACE_OIS_STREAM_ENABLED
#ifdef TRACE_OIS_STREAM_ENABLED
#define TRACE_OIS_STREAM(IO,STREAM,TYPE,DESC) \
  Std(BText("TRACE OIS\t")+(IO)+ \
      "\t"+(STREAM->Name())+"\t"+ \
      "\t"+(STREAM->Offset())+"\t"+ \
      (TYPE)+"\t"+(DESC)+"\n");
#else
#define TRACE_OIS_STREAM(IO,STREAM,TYPE,DESC)
#endif


#endif // TOL_OIS_MACROS_H


