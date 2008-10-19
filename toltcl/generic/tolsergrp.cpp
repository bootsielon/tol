/* tolsergrp.cpp: This file implements a the command to manipulate a
                  group of series.
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

#include <toltclInt.h>
#include <tt_crash.h>

#ifdef USE_BLT_VECTOR
#include <bltVector.h>
#endif

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bstat.h>

struct Tol_SerieCursor;
struct Tol_SerieData;
struct Tol_SerieGroup;

static Tol_SerieCursor * Tol_AllocSerieCursor();
static void Tol_FreeSerieCursor(Tol_SerieCursor * tsc);
static int Tol_InitSerieCursor(Tol_SerieCursor * tsc,
			       Tol_SerieData * tsd,
			       BUserTimeSerie * uts,
			       BDate & begin,
			       BDate & end,
			       int RangeIsDefault);

static int Tol_GetIntIndex(Tcl_Interp * interp,
			   Tcl_Obj * obj,
			   const char * keys,
			   int max, int * idx);

static int Tol_SerieGrpObjCmd(ClientData clientData,
			      Tcl_Interp *interp,
			      int objc, Tcl_Obj *CONST objv[]);

static void Tol_SerieGrpDelCmd(ClientData clientData);

static int Tol_CreateSerieGrpData(Tcl_Interp * interp,
				  int objc,
				  Tcl_Obj *CONST series[],
				  Tol_SerieGroup ** tsgPtr,
				  BDate & begin,
				  BDate & end,
				  int RangeIsDefault);

static int Tol_CreateSerieGrpData(Tcl_Interp * interp,
				  BList * series,
				  Tol_SerieGroup ** tsgPtr,
				  BDate & begin,
				  BDate & end,
				  int RangeIsDefault);

#define TICK_END -1
#define TICK_TCL  1
#define TICK_TOL  2
#define GROW_TICK_DATA 10

struct Tol_TicksData {
  /* set of ticks marker at integer indexes */

  int length;
  int size;
  int * ticks;

  /* name of the set of ticks, could be a TimeSet or a constant set of
     dates */

  char * name;

  /* 1 means TimeSet, 2 means constant set of dates */

  char type;

  /* if invalid we can reuse this slot */
  char  valid;

  int ReturnTicks( Tcl_Interp * interp );

  void SetInfo( char * n, int t ) {
    if ( !name ) {
      int lenstr = strlen(n);
      name = Tcl_Alloc( lenstr + 1 );
      strcpy( name, n );
      type = t;
      valid = 1;
    }
  }
  int new_tick( int t ) {
    int _new;
    if ( length == size ) {
      size += GROW_TICK_DATA;
      ticks = (int *)Tcl_Realloc( (char*)ticks, size * sizeof(ticks[0]) );
    }
    _new = length;
    ticks[_new] = t;
    ++length;
    return _new;
  }
};

int Tol_TicksData::ReturnTicks( Tcl_Interp * interp )
{
  Tcl_Obj * obj_result = Tcl_GetObjResult( interp );
  Tcl_Obj ** items;
  
  if ( length ) {
    items = (Tcl_Obj **)Tcl_Alloc( length * sizeof(items[0]) );
    for ( int i = 0; i < length; ++i )
      items[i] = Tcl_NewIntObj( ticks[i] );
    Tcl_SetListObj( obj_result, length, items );
    Tcl_Free( (char*)items );
  } else
    Tcl_SetListObj( obj_result, 0, NULL );
  
  return TCL_OK;
}

static void Tol_InitTicksData _ANSI_ARGS_((Tol_TicksData &ttd));
static void Tol_FreeTicksData _ANSI_ARGS_((Tol_TicksData &ttd));
static void Tol_ReleaseTicksData _ANSI_ARGS_((Tol_TicksData &ttd));

#define GROW_SET_TICKS 10

class CSetOfTicksData {
public:
  CSetOfTicksData() {
    Init();
  }
  ~CSetOfTicksData() {
    Destroy();
  }
  void Init() {
    length = size = 0;
    data = NULL;
  }
  void Destroy ();
  Tol_TicksData * Get( int i ) {
    return data + i;
  }
  int New();
  int Length() { return length; }
  int Find( char * name );
  void Release( int i );
private:
  void Compact( int i );
  int length;
  int size;
  Tol_TicksData * data;
};

void CSetOfTicksData::Destroy ()
{
  if ( data ) {
    for ( int i = 0; i < length; i++ )
      Tol_FreeTicksData( data[i] );
    Tcl_Free( (char*)data );
    data = NULL;
    length = size = 0;
  }
}

int CSetOfTicksData::New()
{
  int _new;
  if ( length == size ) {
    size += GROW_SET_TICKS;
    data = (Tol_TicksData *)Tcl_Realloc( (char*)data, size * sizeof(data[0]) );
  }
  Tol_InitTicksData( data[length] );
  _new = length;
  ++length;
  return _new;
}
 
int CSetOfTicksData::Find( char * name )
{
  int i;

  for ( i = 0; i < length; i++ )
    if ( !strcmp( name, data[i].name ) )
      break;
  return i < length ? i : -1;
}

void CSetOfTicksData::Release( int i )
{
  Tol_TicksData & ttd = data[i];

  Tol_ReleaseTicksData( ttd );
  if ( ttd.type == TICK_TCL )
    Compact( i );
}

void CSetOfTicksData::Compact( int i )
{
  int n;

  n = length - 1 - i;
  if ( n ) {
    Tol_TicksData tmp = data[i];
    memmove( data+i, data+i+1, n * sizeof(data[0]) );
    data[length-1] = tmp;
  }
  --length;
}

static 
int CmpDate( const void * date1, const void * date2 )
{
  BDate & dte1 = *((BDate*)date1);
  BDate & dte2 = *((BDate*)date2);

  return dte1 > dte2 ? 1 : (dte1 < dte2 ? -1 : 0);
}

class CTickCursor {
public:
  CTickCursor( BDate * Index, int size ) {
    m_index = Index;
    m_size = size;
    m_current = -1;
  }
  virtual ~CTickCursor() {}
  int Start();
  int Next();
  int Current() {
    return m_current;
  }
protected:
  virtual int _first( BDate & f ) = 0;
  virtual int _next() = 0;

  BDate   m_curdate;
  BDate * m_index;
  int     m_size;
  int     m_current;
};

int CTickCursor::Start() 
{
  BDate & last = m_index[m_size-1];

  m_current = 0;

  do {
    if ( !_first( m_index[m_current] ) )
      return 0;

    if ( m_curdate > last ) {
      m_current = -1;
      return 1;
    }
    /* este ciclo lo puedes mejorar 
       con una busqueda binaria */

    while ( m_current < m_size ) {
      if ( m_index[m_current] == m_curdate )
        return 1;
      if ( m_index[m_current] > m_curdate ) 
        break;
      ++m_current;
    }
    if ( m_current >= m_size )
      m_current = -1;
  } while ( m_current != -1 );
  return 1;
}

int CTickCursor::Next() {
  if ( m_current != -1 ) {
    if ( !_next() )
      return 0;
    if ( m_curdate <= m_index[m_size-1] ) {
      BDate * item = (BDate *)bsearch( &m_curdate,
                                       m_index + m_current,
                                       m_size - m_current,
                                       sizeof(BDate),
                                       &CmpDate );
      /*
       * Si yo implementara mi bsearch quizas pudiera optimizar
       * esta busqueda, ya que aprovecharia el hecho de que
       * despues de una bsearch m_current ya nunca se alcanzara siempre
       * que el "mas cercano por debajo" a m_curdate es mayor que m_current.
       */

      if ( item )
        m_current = item - m_index;
      else
        return Next();
    } else
      m_current = -1;
  }
  return 1;
}

class CTickCursor_TOL : public CTickCursor {
public:
  CTickCursor_TOL( BDate * Index, int size )
    : CTickCursor( Index, size ) { }
  ~CTickCursor_TOL() {}
  void SetData( BUserTimeSet * uts ) {
    m_uts = uts;
  }
protected:
  virtual int _first( BDate & f );
  virtual int _next();
private:
  BUserTimeSet * m_uts;
};


int CTickCursor_TOL::_first( BDate & f ) {
  m_curdate = m_uts->FirstNoLess( f );
  return 1;
}

int CTickCursor_TOL::_next() {
  m_curdate = m_uts->Next( m_curdate );
  return 1;
}

class CTickCursor_TCL: public CTickCursor {
public:
  CTickCursor_TCL( BDate * Index, int size )
    : CTickCursor( Index, size ) {
    m_objv = 0;
  }
  ~CTickCursor_TCL() {
    if ( m_objv )
      Tcl_Free( (char*)m_objv );
    m_objc = m_curobj = 0;
  }
  int SetData( Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[] );
protected:
  virtual int _first( BDate & f );
  virtual int _next();
private:
  int m_curobj;
  int m_objc;
  BDate * m_objv;
};

int CTickCursor_TCL::SetData( Tcl_Interp * interp, int objc, Tcl_Obj * CONST objv[] )
{
  int tcl_code;

  m_curobj = 0;
  m_objc = objc;
  m_objv = (BDate*)Tcl_Alloc( objc * sizeof(BDate) );
  for ( int i = 0; i < objc; i++ ) {
    tcl_code = Tol_ListToDate( interp, objv[i], m_objv[i],
                               Tcl_GetObjResult(interp) );
    if ( tcl_code != TCL_OK )
      return 0;
  }
  return 1;
}

int CTickCursor_TCL::_first( BDate & )
{
  if ( m_objc > 0 ) {
    m_curobj = 0;
    m_curdate = m_objv[0];
  }
  return 1;
}

int CTickCursor_TCL::_next()
{
  ++m_curobj;
  if ( m_curobj < m_objc )
    m_curdate = m_objv[m_curobj];
  else {
    m_curdate = m_index[m_size-1];
    m_curdate.IncDay();
  }
  return 1;
}

struct Tol_SerieData {
  int size;  /* == #?+1 */
#ifdef USE_BLT_VECTOR
  Blt_Vector ** X, ** Y;
#else
    Tcl_Obj ** X, ** Y;
#endif
  char * name;
  /* name of the syntax object, may be equal to name */
  char * obj_name;
  /* pointer to user object, valid only during construction */
  BUserTimeSerie * tms_obj;
  int add_segment(int * x, double * y, int n,
                  Tcl_Interp * interp,
                  Tol_SerieCursor * tsc);
};

static Tol_SerieData * Tol_AllocSerieData _ANSI_ARGS_((void));
static void Tol_FreeSerieData _ANSI_ARGS_((Tol_SerieData *tsd));

#define GROW_DATING_BUFFER 10

struct Tol_SerieGroup {
  Tcl_Interp * interp;
  
  /* basic dating, common to every serie in the group */
  Tcl_DString format;
  BDate * dating;
  char * dating_name;
  int     dating_length;
  int   dating_size;
  
  /* list of serie data */
  int size;
  Tol_SerieData ** series_data;

  /* list of ticks markers */

  CSetOfTicksData ticks_markers;
  
  /* token used to remove the command */
  
  Tcl_Command cmd_token;

  int AddDate( BDate & date );
  int AddTicks( int objc, Tcl_Obj * CONST objv[] );
  int RemoveTicks( int objc, Tcl_Obj * CONST objv[] );
  int SetFormat( char * fmt );
  int GetFormat();
  int ReturnDate( int idx, char * str_idx );
  int ReturnDatingName();
};

static Tol_SerieGroup * Tol_AllocSerieGroup _ANSI_ARGS_((void));

static void Tol_FreeSerieGroup _ANSI_ARGS_((Tol_SerieGroup * tsg));

static int Tol_GroupFindSerie _ANSI_ARGS_((char * name,
                                           Tol_SerieGroup * sgrp));

static int Tol_GroupFindSerie _ANSI_ARGS_((BUserTimeSerie * obj,
                                           Tol_SerieGroup * sgrp));

int Tol_SerieGroup::AddDate( BDate & date ) {
  if ( dating_length == dating_size ) {
    dating_size += GROW_DATING_BUFFER;
    dating = (BDate*)Tcl_Realloc( (char*)dating,
                                  dating_size*sizeof(BDate) );
  }
  dating[dating_length] = date;
  ++dating_length;
  return TCL_OK;
}

int Tol_SerieGroup::AddTicks( int objc, Tcl_Obj * CONST objv[] )
{
  char * tsn;
  int idx;
  Tol_TicksData * ttd;
  Tcl_Obj ** items;
  int length;
  int tcl_code;
  Tcl_DString uniBuffer;

  BGrammar * gra = GraTimeSet();
  BUserTimeSet * uts;

  CTickCursor * tickCursor;
  CTickCursor_TOL tolCursor( dating, dating_length  );
  CTickCursor_TCL tclCursor( dating, dating_length );
  BDate current;
  int ctick;

  for ( int i = 0; i < objc; i++ ) {
    tcl_code = Tcl_ListObjGetElements( interp, objv[i], &length, &items );
    if ( tcl_code != TCL_OK )
      return tcl_code;
    if ( !length ) {
      Tcl_AppendResult( interp, "empty tick set name", NULL );
      return TCL_ERROR;
    }
    tsn = Tcl_GetString( items[0] );
    idx = ticks_markers.Find( tsn );
    if ( idx != -1 ) {
      ttd = ticks_markers.Get( idx ); 
      if ( ttd->valid ) {
        Tcl_AppendResult( interp, "tick set '", tsn,
                          "' already exists", NULL );
        return TCL_ERROR;
      } else {
        /* should be a previous TimeSet, so use it */ 
        ttd->valid = 1;
        continue;
      }
    } else {
      if ( length == 1 ) {
        Tcl_DStringInit( &uniBuffer );
        Tcl_UtfToExternalDString(NULL, tsn, -1,&uniBuffer);
        uts = (BUserTimeSet *)(gra->FINDVARIABLE(uniBuffer.string));
        Tcl_DStringFree(&uniBuffer);
        if ( !uts ) {
          Tcl_AppendResult( interp, "object timeset '",
          tsn, "' not found", NULL );
          return TCL_ERROR;
        }
	/* CACHE: try to fill cache for uts */
        tolCursor.SetData( uts );
        tickCursor = &tolCursor;
        idx = ticks_markers.New();
        ttd = ticks_markers.Get( idx );
        ttd->SetInfo( tsn, TICK_TOL ); 
      } else {
        /* verificar las fechas y construir un array de BDate */
        if ( !tclCursor.SetData( interp, length-1, items+1 ) ) {
          Tcl_AppendResult( interp, "\nError in cursor", NULL );
          ticks_markers.Release( idx );
          return TCL_ERROR;
        }
        tickCursor = &tclCursor;
        idx = ticks_markers.New();
        ttd = ticks_markers.Get( idx );
        ttd->SetInfo( tsn, TICK_TCL ); 
      }
      /* recorrer tickCursor para rellenar ttd */
      if ( !tickCursor->Start() ) {
        Tcl_AppendResult( interp, "\nError in cursor", NULL );
        ticks_markers.Release( idx );
        return TCL_ERROR;
      }
      while ( (ctick = tickCursor->Current()) != TICK_END ) {
        // add tick;
        ttd->new_tick( ctick );
        if ( !tickCursor->Next() ) {
          Tcl_AppendResult( interp, "\nError in cursor", NULL );
          ticks_markers.Release( idx );
          return TCL_ERROR;
        }
      }
    }
  }
  return TCL_OK;
}

int Tol_SerieGroup::RemoveTicks( int objc, Tcl_Obj * CONST objv[] )
{
  char * tms;
  int idx;

  for ( int i = 0; i < objc; i++ ) {
    tms = Tcl_GetString( objv[i] );
    idx = ticks_markers.Find( tms );
    if ( idx == -1 || !ticks_markers.Get(idx)->valid ) {
      Tcl_AppendResult( interp, "time set '", tms,
                        "' not found", NULL );
      return TCL_ERROR;
    }
    ticks_markers.Release( idx );
  }
  return TCL_OK;
}

int Tol_SerieGroup::SetFormat( char * fmt )
{
  Tcl_DStringSetLength( &format, strlen(fmt) );
  strcpy( format.string, fmt );
  return TCL_OK;
}

int Tol_SerieGroup::GetFormat()
{
  char * fmt;

  if ( Tcl_DStringLength( &format ) )
    fmt = Tcl_DStringValue( &format );
  else
    fmt = ((BText&)(BDateFormat::Defect().Format())).Buffer();
  Tcl_SetResult( interp, fmt, TCL_VOLATILE );
  return TCL_OK;
}

int Tol_SerieGroup::ReturnDate( int idx, char * str_idx )
{
  BDateFormat bformat( BDateFormat::Defect() );
  Tcl_DString dstr;

  if ( Tcl_DStringLength( &format ) ) 
    bformat = BDateFormat( Tcl_DStringValue( &format ) );

  BText btxt( bformat.DateToText( dating[idx] ) );
 
  Tcl_DStringInit( &dstr );
  Tcl_ExternalToUtfDString( NULL , btxt.Buffer(), -1, &dstr );
  Tcl_DStringResult( interp, &dstr );
  Tcl_DStringFree( &dstr );

  return TCL_OK;
}

int Tol_SerieGroup::ReturnDatingName()
{
  if (dating_name)
    Tcl_SetResult( interp, dating_name, TCL_VOLATILE );
  else
    Tcl_SetResult( interp, "", TCL_VOLATILE );
  return TCL_OK;
}

/*
 * Tol_SerieCursor : used to build a group of series
 */

struct Tol_SerieCursor {
  /* bounds of the group of series */
  static BDate lowerD, upperD;
  static void InitBounds() {
    lowerD = BDate::Begin();
    upperD = BDate::End();
  }
  /* current date in tms, start in first */
  BDate cur_date;
  /* last date to see */
  BDate until_date;
  /* is the serie unbounded */
  unsigned char lowerU, upperU;
  /* serie with data */
  BUserTimeSerie * serie;
  /* dating of serie */
  BUserTimeSet * tms;
  /* all indexes, to light the memory manager */
  int * _x_;
  /* all data, to light the memory manager */
  double * _y_;
  
  /* 
   * indexes of the current segment, respect to _x_ & _y_
   * represent the boundaries of current segment
   */
  
  int idx0, idx1;
  Tol_SerieData * tsd;
  /* tsd->size <= size */
#define SEG_GROW 10
  int size;
  void append(int x, double y) {
    ++idx1;
    _x_[idx1] = x;
    _y_[idx1] = y;
  }
  void new_segment(Tcl_Interp * interp) {
    if (idx0<=idx1) {
      tsd->add_segment(_x_+idx0,_y_+idx0,idx1-idx0+1,interp,this);
      idx0 = idx1+1;
    }
  }
  /* useful <==> cur_date <= until_date */
  int useful;
  int operator<= (Tol_SerieCursor & c) {
    return !useful || cur_date <= c.cur_date;
  }
  int operator== (Tol_SerieCursor & c) {
    return cur_date == c.cur_date;
  }
  int next_date() {
    cur_date += tms;
    if (cur_date>until_date)
      useful = 0;
    return useful;
  }
};

BDate Tol_SerieCursor::lowerD = BDate::Begin();
BDate Tol_SerieCursor::upperD = BDate::End();

int Tol_SerieData::add_segment(int * x, double * y, int n,
                               Tcl_Interp * interp,
                               Tol_SerieCursor * tsc)
{
  if (size == tsc->size) {
    tsc->size += SEG_GROW;
#ifdef USE_BLT_VECTOR
    X = (Blt_Vector**)Tcl_Realloc((char*)X,sizeof(Blt_Vector*)*tsc->size);
#else
    X = (Tcl_Obj**)Tcl_Realloc((char*)X,sizeof(Tcl_Obj*)*tsc->size);
#endif
    if (!X)
      return TCL_ERROR;
#ifdef USE_BLT_VECTOR
    Y = (Blt_Vector**)Tcl_Realloc((char*)Y,sizeof(Blt_Vector*)*tsc->size);
#else
    Y = (Tcl_Obj**)Tcl_Realloc((char*)Y,sizeof(Tcl_Obj*)*tsc->size);
#endif
    if (!Y)
      return TCL_ERROR;
  }
#ifdef USE_BLT_VECTOR
  tcl_code = Blt_CreateVector(interp,"#auto",n,X+size);
  if (tcl_code!=TCL_OK)
    return TCL_ERROR;
  Blt_Vector * xv = X[size];
  tcl_code = Blt_CreateVector(interp,"#auto",n,Y+size);
  if (tcl_code!=TCL_OK)
    return TCL_ERROR;
  Blt_Vector * yv = Y[size];
  for (int i=0; i<n; ++i) {
    xv->valueArr[i] = x[i];
    yv->valueArr[i] = y[i];
  }
#else
  Tcl_Obj ** itemsX = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*)*n);
  Tcl_Obj ** itemsY = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*)*n);
  for (int i=0; i<n; ++i) {
    itemsX[i] = Tcl_NewDoubleObj(x[i]);
    itemsY[i] = Tcl_NewDoubleObj(y[i]);
  }
  Tcl_IncrRefCount(X[size]=Tcl_NewListObj(n, itemsX));
  Tcl_IncrRefCount(Y[size]=Tcl_NewListObj(n, itemsY));
#endif
  ++size;
  return TCL_OK;
}

/*
 *  Tol_CreateSerieGrp --
 *
 */

int Tol_CreateSerieGrp(Tcl_Interp * interp,
                       int objc, Tcl_Obj *CONST series[], Tcl_Obj * obj_result)
{
  int tcl_code;
  Tol_SerieGroup * sgrp;
  Tcl_CmdInfo cmd_info;
  char * sgrp_name = Tcl_GetString(series[0]);
  
  if (!*sgrp_name) {
    Tcl_AppendStringsToObj(obj_result,
                           "invalid serie group name\"",sgrp_name,"\"",NULL);
    return TCL_ERROR;
  }
  
  /* verify if sgrp_name already exists as a group of series */
  
  if (Tcl_GetCommandInfo(interp,sgrp_name,&cmd_info) &&
      (cmd_info.objProc == Tol_SerieGrpObjCmd)) {
    Tcl_AppendStringsToObj(obj_result, "\"",sgrp_name,"\"",
                           " already exists as a group of series object",NULL);
    return TCL_ERROR;
  }

  /* check for date interval */
  BDate I0,I1;
  int isDefault = 1;
  if ( !strcmp(Tcl_GetString(series[1]),"-range") ) {
    int datec;
    Tcl_Obj ** datev;

    if ( objc < 4 ) {
      Tcl_AppendStringsToObj(obj_result,
                             "wrong # args: should be \"",
                             "tol::seriegrp create grpName -range range serName ?serName?\"", NULL );
      return TCL_ERROR;
    }

    tcl_code = Tcl_ListObjGetElements(interp,series[2],&datec,&datev);
    if (tcl_code != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    if (datec == 1) {
      /* try to convert to an interval */
      tcl_code = Tol_ParseDate(interp,Tcl_GetString(datev[0]),I0);
      if (tcl_code!=TCL_OK) {
        Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
        return TCL_ERROR;
      }
      I1 = BDate::DefaultLast();
    } else if (datec==2) {
      /* try to convert to an interval */
      tcl_code = Tol_ParseDate(interp,Tcl_GetString(datev[0]),I0);
      if (tcl_code!=TCL_OK) {
        Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
        return TCL_ERROR;
      }
      tcl_code = Tol_ParseDate(interp,Tcl_GetString(datev[1]),I1);
      if (tcl_code!=TCL_OK) {
        Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendStringsToObj(obj_result,"invalid range list",NULL);
      return TCL_ERROR;
    }
    if (I0>I1) {
      Tcl_SetStringObj(obj_result,"improper date range",-1);
      return TCL_ERROR;
    }
    isDefault = 0;
    // skip three arguments.
    objc -=3;
    series += 3;
  } else {
    I0 = BDate::DefaultFirst();
    I1 = BDate::DefaultLast();
    isDefault = 1;
    --objc;
    ++series;
  }
  if (objc==0) {
    Tcl_SetStringObj(obj_result,
                     "wrong # args, missing serie names arguments",-1);
    return TCL_ERROR;

  }
  tcl_code = Tol_CreateSerieGrpData(interp,objc,series,&sgrp,
                                    I0,I1,isDefault);
  if ( tcl_code != TCL_OK ) {
    Tol_FreeSerieGroup(sgrp);
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  
  /* now is ok to create the obj cmd */
  
  sgrp->cmd_token = Tcl_CreateObjCommand(interp,sgrp_name,Tol_SerieGrpObjCmd,
                                         (ClientData) sgrp,Tol_SerieGrpDelCmd);
  sgrp->interp = interp;
  Tcl_SetStringObj(obj_result,sgrp_name,-1);
  return TCL_OK;
}

/*
 *  Tol_CreateSerieGrp --
 *
 *    to be called directly from C++.
 */

int Tol_CreateSerieGrp(Tcl_Interp * interp,
                       char * sgrp_name, BList *series, Tcl_Obj * obj_result)
{
  int tcl_code;
  Tol_SerieGroup * sgrp;
  Tcl_CmdInfo cmd_info;
  
  if (!*sgrp_name) {
    Tcl_AppendStringsToObj(obj_result,
                           "invalid serie group name\"",sgrp_name,"\"",NULL);
    return TCL_ERROR;
  }
  
  /* verify if sgrp_name already exists as a group of series */
  
  if (Tcl_GetCommandInfo(interp,sgrp_name,&cmd_info)) {
    Tcl_AppendStringsToObj(obj_result, "\"",sgrp_name,"\"",
                           " already exists as a group of series object",NULL);
    return TCL_ERROR;
  }

  /* check for date interval */
  BDate I0,I1;
  I0 = BDate::DefaultFirst();
  I1 = BDate::DefaultLast();
  int isDefault = 1;

  tcl_code = Tol_CreateSerieGrpData(interp,series,&sgrp,
                                    I0,I1,isDefault);
  if ( tcl_code != TCL_OK ) {
    Tol_FreeSerieGroup(sgrp);
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  
  /* now is ok to create the obj cmd */
  
  sgrp->cmd_token = Tcl_CreateObjCommand(interp,sgrp_name,Tol_SerieGrpObjCmd,
                                         (ClientData) sgrp,Tol_SerieGrpDelCmd);
  sgrp->interp = interp;
  Tcl_SetStringObj(obj_result,sgrp_name,-1);
  return TCL_OK;
}

/*
 * Destroy a list of series groups.
 *
 */

int Tol_DestroySerieGrp (Tcl_Interp * interp,
                         int objc, Tcl_Obj *CONST objv[],
                         Tcl_Obj * obj_result)
{
  Tcl_CmdInfo cmd_info;
  char * sgrp_name;
  
  for (int i=0; i<objc; ++i) {
    sgrp_name = Tcl_GetString(objv[i]);
    if (Tcl_GetCommandInfo(interp,sgrp_name,&cmd_info) &&
        (cmd_info.objProc == Tol_SerieGrpObjCmd)) {
      if (Tcl_DeleteCommand(interp,sgrp_name)) {
        Tcl_AppendStringsToObj(obj_result,
                               "could not delete \"",sgrp_name,"\"",NULL);
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendStringsToObj(obj_result, 
                             "can't find serie group \"",sgrp_name,"\"",NULL);
      return TCL_ERROR;
    }
  }
  Tcl_ResetResult(interp);
  return TCL_OK;
}

/*
 *  Tol_SerieGrpObjCmd --
 *
 *    implement an specific group of series given by clientData.
 *
 *    seriegrp dating (size|idx|name)
 *
 *           seriegrp dating size --> return the basic dating size
 *
 *           seriegrp dating idx --> return the date info for the idx
 *                                   element of the basic dating.
 *
 *      seriegrp dating name --> return the name of the common (possibly)
 *            dating
 *
 *    seriegrp serie (size|(idx (name|data|stat)))
 *
 *      seriegrp serie size --> return the number of series
 *
 *      seriegrp serie idx name -> return the name of ith serie.
 *  
 *           seriegrp serie idx data -->
 *                             
 *                             return a 2 element list with the serie
 *                             data corresponding to serName. The
 *                             first element is a * BLT vector name
 *                             representing the indexes in the basic
 *                             dating, * the second one is a vector
 *                             with corresponding values.
 *
 *      seriegrp serie idx reference -> return  
 *
 *      seriegrp format ?strfmt?
 *
 *      seriegrp ticks ...
 *
 *        seriegrp ticks add tms ?tms?
 *
 *        seriegrp ticks remove tms ?tms?
 *
 *        seriegrp ticks get tms
 */


int Tol_SerieGrpObjCmd (ClientData clientData,
                        Tcl_Interp *interp,
                        int objc, Tcl_Obj *CONST objv[])
{
  int tcl_code;
  int idx;

#ifdef __RELEASE__
  try_crash {
#endif
    Tol_SerieGroup * sgrp = (Tol_SerieGroup*) clientData;
  
    if ( objc < 2 ) {
      Tcl_AppendResult(interp,
                       "wrong # args: should be \"",
                       Tcl_GetString(objv[0]),
                       " option\"", NULL );
      return TCL_ERROR;
    }
  
    char * option = Tcl_GetString(objv[1]);
    int length = strlen(option);
    char * arg = NULL;

    if ( objc >= 3)
      arg = Tcl_GetString(objv[2]);
 
    if (length && !strncmp("dating",option,length)) {
      if (objc != 3) {
        Tcl_AppendResult(interp,
                         "wrong # args: should be \"",
                         Tcl_GetString(objv[0]),
                         " dating arg\"", NULL );
        return TCL_ERROR;
      }
      if (!strcmp("name",arg))
        return sgrp->ReturnDatingName();
      else if (!strcmp("size",arg)) {
        Tcl_SetObjResult(interp,Tcl_NewIntObj(sgrp->dating_length));
        return TCL_OK;
      } else {
        tcl_code = Tol_GetIntIndex(interp,objv[2],"",
                                   sgrp->dating_length-1,&idx);
        if (tcl_code == TCL_ERROR)
          return TCL_ERROR;
        //tcl_code = Tcl_ListObjIndex(interp,sgrp->dating,idx,&obj_result);

        return sgrp->ReturnDate( idx, Tcl_GetString(objv[2]) );
      }
    } else if (length && !strncmp("serie",option,length)) {
      if ( objc < 3 ) {
        Tcl_AppendResult(interp,
                         "wrong # args: should be \"",
                         Tcl_GetString(objv[0]), 
                         " option args\"", NULL );
        return TCL_ERROR;
      }
      if (!strcmp("size",arg)) {
        if (objc != 3) {
          Tcl_AppendResult(interp,
                           "extra args: should be \"",
                           Tcl_GetString(objv[0]),
                           " serie size\"", NULL );
          return TCL_ERROR;
        }
        Tcl_SetObjResult(interp,Tcl_NewIntObj(sgrp->size));
        return TCL_OK;
      }
      if (objc != 4) {
        Tcl_AppendResult(interp,
                         "wrong # args: should be \"",
                         Tcl_GetString(objv[0]),
                         " serie idx arg\"", NULL );
        return TCL_ERROR;
      }
      tcl_code = Tol_GetIntIndex(interp,objv[2],"",
                                 sgrp->size-1,&idx);
      if (tcl_code == TCL_ERROR)
        return TCL_ERROR;
    
      Tol_SerieData * sdata = sgrp->series_data[idx];

      arg = Tcl_GetString(objv[3]);
      if (!strcmp(arg,"name")) {
        Tcl_AppendResult(interp,sdata->obj_name,NULL);
        return TCL_OK;
      }
      if (!strcmp(arg,"reference")) {
        Tcl_AppendResult(interp,sdata->name,NULL);
        return TCL_OK;
      }
      if (strcmp(arg,"data")) {
        Tcl_AppendResult(interp,"invalid option, must be: name, data or reference", NULL);
        return TCL_ERROR;

      }
      /* extract the vector names from the serie data or vector values*/
    
      Tcl_Obj * XY[2];
      Tcl_Obj ** segments = (Tcl_Obj **)Tcl_Alloc(sizeof(Tcl_Obj*)*sdata->size);
    
    
      for (int i=0; i<sdata->size; i++) {
#ifdef USE_BLT_VECTOR
        XY[0] = Tcl_NewStringObj(Blt_NameOfVector(sdata->X[i]),-1);
        XY[1] = Tcl_NewStringObj(Blt_NameOfVector(sdata->Y[i]),-1);
#else
        XY[0] = sdata->X[i];
        XY[1] = sdata->Y[i];
#endif
        segments[i] = Tcl_NewListObj(2,XY);
      }
      Tcl_SetObjResult(interp,Tcl_NewListObj(sdata->size,segments));
      Tcl_Free( (char*)segments );
      return TCL_OK;
    } else if ( !strncmp( "format", option, length ) ) {
      if ( objc > 3 ) {
        Tcl_AppendResult( interp, "wrong # args, should be: '",
                          Tcl_GetString( objv[0] ), " ", option,
                          " strformat'", NULL );
        return TCL_ERROR;
      }
      /* implementar la opcion de formato aqui */
      /* almacenar en una variable para cuando se pida una fecha */
      if ( objc == 3 )
        return sgrp->SetFormat( Tcl_GetString( objv[2] ) );
      else
        return sgrp->GetFormat();
    } else if ( !strncmp( "ticks", option, length ) ) {
      if ( objc < 4 ) {
        Tcl_AppendResult( interp, "wrong # args, should be:\n",
                          Tcl_GetString( objv[0] ), " ticks add tms ?tms?\n",
                          Tcl_GetString( objv[0] ), " ticks get tms\n",
                          Tcl_GetString( objv[0] ), " ticks remove tms ?tms?", NULL );
        return TCL_ERROR;
      }
      char * subopt = Tcl_GetString( objv[2] );
      length = strlen( subopt );
      if ( length && !strncmp( "add", subopt, length ) )
        return sgrp->AddTicks( objc-3, objv+3 );
      else if ( length && !strncmp( "get", subopt, length ) ) {
        Tol_TicksData * ttd;
        idx = sgrp->ticks_markers.Find( Tcl_GetString(objv[3] ) );
        if ( idx == -1 || 
             !((ttd=sgrp->ticks_markers.Get(idx))->valid) ) {
          Tcl_AppendResult( interp, "time set '", Tcl_GetString(objv[3]),
                            "' not found", NULL );
          return TCL_ERROR;
        }
        return ttd->ReturnTicks( interp );
      } else if ( length && !strncmp( "remove", subopt, length ) )
        return sgrp->RemoveTicks( objc-3, objv+3 );
      else {
        Tcl_AppendResult( interp, "Invalid option '", subopt,
                          "' should be: add, get or remove", NULL );
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendResult(interp, "bad option \"",option,
                       "\": must be dating, format, serie or ticks",NULL);
      return TCL_ERROR;
    }
#ifdef __RELEASE__
  } catch_crash {
    Tcl_AppendResult(interp,
                     "Fatal error in seriegrp subcommand, kill the application",
                     NULL);
    return TCL_ERROR;
  }
#endif
}

static
int Tol_GetIntIndex(Tcl_Interp * interp, Tcl_Obj * obj,
                    const char * keys, int max, int * idx)
{
  if (!strcmp("end",Tcl_GetString(obj)))
    *idx = max;
  else if (Tcl_GetIntFromObj(interp,obj,idx)==TCL_OK) {
    if (*idx < 0 || *idx > max) {
      char str_max[1024];
      sprintf(str_max,"%d",max);
      Tcl_AppendResult(interp,"index out of range \"",
                       Tcl_GetString(obj),
                       "\": must be not negative and no greater than ",
                       str_max,NULL);
      return TCL_ERROR;
    }
  } else {
    char empty[] = "";
    if (!keys)
      keys = empty;
    Tcl_AppendResult(interp,"\ninvalid index \"",
                     Tcl_GetString(obj),
                     "\": must be ",keys,", integer or end",NULL);
    return TCL_ERROR;
  }
  return TCL_OK;
}

void Tol_SerieGrpDelCmd (ClientData clientData)
{
  Tol_SerieGroup * tsg = (Tol_SerieGroup*) clientData;
  
  Tol_FreeSerieGroup(tsg);
}

static 
Tol_SerieData * Tol_AllocSerieData(void)
{
  Tol_SerieData * tsd = (Tol_SerieData *)Tcl_Alloc(sizeof(Tol_SerieData));
  
  if (tsd) {
    tsd->size = 0;
    tsd->X = tsd->Y = NULL;
    tsd->name = NULL;
    tsd->obj_name = NULL;
    tsd->tms_obj = NULL;
  }
  return tsd;
}

static
void Tol_FreeSerieData(Tol_SerieData *tsd)
{
  if (tsd) {
    if (tsd->size) {
      for (int i=0; i<tsd->size; ++i) {
#ifdef USE_BLT_VECTOR
        Blt_DeleteVector(tsd->X[i]);
        Blt_DeleteVector(tsd->Y[i]);
#else
        Tcl_DecrRefCount(tsd->X[i]);
        Tcl_DecrRefCount(tsd->Y[i]);
#endif
      }
      Tcl_Free((char*)tsd->X);
      Tcl_Free((char*)tsd->Y);
    }
    if (tsd->name)
      Tcl_Free(tsd->name);
    if (tsd->obj_name)
      Tcl_Free(tsd->obj_name);
    Tcl_Free((char*)tsd);
  }
}

static 
void Tol_InitTicksData( Tol_TicksData & ttd )
{
  ttd.length = 0;
  ttd.size = 0;
  ttd.ticks = NULL;
  ttd.name = NULL;
  ttd.valid = 0;
  ttd.type = TICK_TCL;
}

static
void Tol_FreeTicksData( Tol_TicksData & ttd )
{
  if (ttd.ticks) {
    Tcl_Free((char*)ttd.ticks);
    ttd.length = 0;
    ttd.size = 0;
    ttd.ticks = NULL;
  }
  if (ttd.name) {
    Tcl_Free(ttd.name);
    ttd.name = NULL;
  }
  ttd.valid = 0;
  ttd.type = TICK_TCL;
}

static
void Tol_ReleaseTicksData( Tol_TicksData & ttd )
{
  if ( ttd.type == TICK_TOL )
    ttd.valid = 0;
  else
    Tol_FreeTicksData(ttd);
}

static
Tol_SerieGroup * Tol_AllocSerieGroup(void)
{
  Tol_SerieGroup * tsg = (Tol_SerieGroup *)Tcl_Alloc(sizeof(Tol_SerieGroup));
  
  if (tsg) {
    tsg->interp = NULL;
    Tcl_DStringInit( &(tsg->format) );
    tsg->dating = NULL;
    tsg->dating_name = NULL;
    tsg->dating_length = 0;
    tsg->dating_size = 0;
    tsg->size = 0;
    tsg->series_data = NULL;
    tsg->ticks_markers.Init();
  }
  return tsg;
}

static
void Tol_FreeSerieGroup(Tol_SerieGroup * tsg)
{
  if (tsg) {
    int i;
    Tcl_DStringFree( &(tsg->format) );
    if (tsg->dating) {
      Tcl_Free((char*)tsg->dating);
    }
    if (tsg->dating_name) {
      Tcl_Free((char*)tsg->dating_name);
    }
    if (tsg->series_data) {
      for ( i = 0; i < tsg->size; ++i)
        Tol_FreeSerieData(tsg->series_data[i]);
      Tcl_Free((char*)tsg->series_data);
    }
    tsg->ticks_markers.Destroy();
    Tcl_Free((char*)tsg);
  }
}

static 
int Tol_GroupFindSerie(BUserTimeSerie * obj, Tol_SerieGroup * tsg)
{
  if (tsg) {
    for (int i=0; i<tsg->size; ++i)
      if (obj == tsg->series_data[i]->tms_obj)
        return i;
  }
  return -1;
}

static 
int Tol_GroupFindSerie(char * name, Tol_SerieGroup * tsg)
{
  if (tsg) {
    for (int i=0; i<tsg->size; ++i)
      if (!strcmp(name,tsg->series_data[i]->name))
        return i;
  }
  return -1;
}

static 
Tol_SerieCursor * Tol_AllocSerieCursor(void)
{
  Tol_SerieCursor * tsc = (Tol_SerieCursor *)Tcl_Alloc(sizeof(Tol_SerieCursor));
  
  if (tsc) {
    tsc->serie = NULL;
    tsc->tms = NULL;
    tsc->_x_ = NULL;
    tsc->_y_ = NULL;
    tsc->idx0 = tsc->idx1 = -1;
    tsc->tsd = NULL;
    tsc->size=0;
    tsc->useful = 0;
  }
  return tsc;
}

static
void Tol_FreeSerieCursor(Tol_SerieCursor * tsc)
{
  if (tsc) {
    if (tsc->_x_)
      Tcl_Free((char*)tsc->_x_);
    if (tsc->_x_)
      Tcl_Free((char*)tsc->_y_);
    Tcl_Free((char*)tsc);
  }
}

/*
 * Tol_AllocMemoryForCursor --
 *
 */

int Tol_AllocMemoryForCursor(Tol_SerieCursor * tsc) {
  int length;
  
  if (tsc->lowerU || tsc->upperU) {
    /* delay allocation until all bounded series are processed */
    tsc->useful = 0;
    return 1;
  }
  
  tsc->cur_date = tsc->tms->FirstNoLess(tsc->cur_date);
  tsc->until_date = tsc->tms->FirstNoGreat(tsc->until_date);
  tsc->useful = 1;
  length = tsc->tms->Difference(tsc->cur_date,tsc->until_date)+1;
  if (length<=0) {
    tsc->useful = 0;
    /* no tiene datos, no es error pero no es util */
    return 1;
  }
  tsc->_x_ = (int*)Tcl_Alloc(sizeof(int)*length);
  if (!tsc->_x_)
    return 0;
  tsc->_y_ = (double*)Tcl_Alloc(sizeof(double)*length);
  if (!tsc->_y_)
    return 0;
#ifdef USE_BLT_VECTOR
  tsc->tsd->X = (Blt_Vector**)Tcl_Alloc(sizeof(Blt_Vector*)*SEG_GROW);
#else
  tsc->tsd->X = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*)*SEG_GROW);
#endif
  if (!tsc->tsd->X)
    return 0;
#ifdef USE_BLT_VECTOR
  tsc->tsd->Y = (Blt_Vector**)Tcl_Alloc(sizeof(Blt_Vector*)*SEG_GROW);
#else
  tsc->tsd->Y = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*)*SEG_GROW);
#endif
  if (!tsc->tsd->Y)
    return 0;
  tsc->size = SEG_GROW;
  return 1;
}

/*
 * Tol_InitSerieCursor --
 *
 *    Initialize a SerieCursor.
 *
 *      tsd -> SerieData to update
 *
 *      uts -> Data source with valid dating.
 *
 *
 */

static
int Tol_InitSerieCursor (Tol_SerieCursor * tsc,
                         Tol_SerieData * tsd,
                         BUserTimeSerie * uts,
                         BDate & begin,
                         BDate & end, int RangeIsDefault)
{
  tsc->serie = uts;

  tsc->cur_date = uts->FirstDate();
  tsc->until_date = uts->LastDate();
  tsc->lowerU = !tsc->cur_date.HasValue();
  tsc->upperU = !tsc->until_date.HasValue();

  if ( RangeIsDefault ) {
    /*
    if ( tsc->lowerU )
      tsc->cur_date = begin;
    if ( tsc->upperU )
      tsc->until_date = end;
    */
    if ( !tsc->lowerD.HasValue() || (!tsc->lowerU && tsc->cur_date < tsc->lowerD) )
      tsc->lowerD = tsc->cur_date;
    if ( !tsc->upperD.HasValue() || (!tsc->upperU && tsc->until_date > tsc->upperD) )
      tsc->upperD = tsc->until_date;
  } else {
    /* Este camino se eliminara en un futuro cercano, es decir RangeIsDefault==1 */
    if ( tsc->lowerU || tsc->cur_date < begin )
      tsc->cur_date = begin;
    if ( tsc->upperU || tsc->until_date > end )
      tsc->until_date = end;
  }

  tsc->tms = uts->Dating();
  tsc->idx0 = 0;
  tsc->idx1 = -1;
  tsc->tsd = tsd;
  /* CACHE: inicializar el HashBetween aqui? 
   * [cur_date,until_date]
   */
  BHash hash;
  tsc->tms->GetHashBetween(hash, tsc->cur_date,tsc->until_date);
  return Tol_AllocMemoryForCursor(tsc);
}

inline
int BadAlloc(Tcl_Interp * interp)
{
  Tcl_AppendResult(interp,
                   "could not allocate serie group data",NULL);
  return TCL_ERROR;
}

#define _CHECK_PTR_(ptr) \
if (!ptr) return BadAlloc(interp);

struct scursors {
  int last;
  int first_good;
  int first_sorted;
  Tol_SerieCursor ** data;
  scursors() {
    last = -1;
    first_good = 0;
    first_sorted = 0;
    data = NULL;
  }
  ~scursors() {
    if (data) {
      for (int i=0; i<=last; i++)
        Tol_FreeSerieCursor(data[i]);
      Tcl_Free((char*)data);
    }
  }
  void sort();
  int HasUseful() {
    return first_good <= last;
  }
  void compact( int bad ) {
    int n = bad - first_good;
    if ( n ) {
      Tol_SerieCursor * tmp = data[bad];
      memmove( data+first_good+1, data+first_good, sizeof(data[0]) * n );
      data[first_good] = tmp;
    }
  }
};

void scursors::sort()
{
  int i,j,n;
  Tol_SerieCursor * tmp;
  
  while (first_good<=last && !data[first_good]->useful)
    ++first_good;
  if (first_good>last)
    return;
  for (i=first_sorted-1; i>=first_good; --i) {
    for (j=i+1; j<=last; j++)
      if (*(data[i])<=*(data[j]))
        break;
    n = j - i - 1;
    if ( n ) {
      tmp = data[i];
      memmove(data+i,data+i+1,sizeof(Tol_SerieCursor*)*n);
      data[i+n] = tmp;
    }
  }
  first_sorted = first_good;
}

/*
 *  Tol_CreateSerieGrpData --
 *
 *    to be called from tcl interface.
 */

static
int Tol_CreateSerieGrpData (Tcl_Interp * interp,
                            int objc, Tcl_Obj *CONST series[],
                            Tol_SerieGroup ** tsgPtr,
                            BDate & begin,
                            BDate & end,
                            int RangeIsDefault)
{
  
  BGrammar * gra = BGrammar::FindByName("Serie");
  scursors cursors;
  Tol_SerieGroup * tsg = NULL;

  *tsgPtr = NULL;
  if (!gra) {
    Tcl_AppendResult(interp,"grammar 'Serie' not found",NULL);
    return TCL_ERROR;
  }

  tsg = *tsgPtr = Tol_AllocSerieGroup();
  _CHECK_PTR_(tsg);
    
  /*
   * alloc space for serie data array
   * but for now it's empty
   */
  
  tsg->size = 0; 
  tsg->series_data = (Tol_SerieData**)Tcl_Alloc(sizeof(Tol_SerieData*)*objc);
  _CHECK_PTR_(tsg->series_data);
  
  /*
   * alloc space for array of cursors
   * but for now it's empty
   */
  
  cursors.data = (Tol_SerieCursor**)Tcl_Alloc(sizeof(Tol_SerieCursor*)*objc);
  _CHECK_PTR_(cursors.data);
  
  /*
   * build cursor & serie data for every known serie
   */
  
  Tol_SerieData * tsd; 
  BUserTimeSerie * uts;
  Tol_SerieCursor * tsc;
  char * sername;
  int str_len;
  int status;
  Tcl_DString uniBuffer;
  int flagDating = 0;
  BUserTimeSet * firstDating = 0;
  
  Tcl_DStringInit(&uniBuffer);
  Tol_SerieCursor::InitBounds();
  for (int i=0; i<objc; ++i) {
    /* verify serie */
    //sername = Tcl_UtfToExternalDString(NULL,Tcl_GetString(series[i]),-1,&uniBuffer);
    if ( Tol_GroupFindSerie(Tcl_GetString(series[i]),tsg) != -1 )
      continue;
    //uts = Tol_GetTimeSerie(interp,series[i]);
    uts = (BUserTimeSerie*)Tol_ResolveObject(interp, series[i], GraSerie());
    if ( !uts )
      return TCL_ERROR;

    /* validate the string representation */

    sername = Tcl_GetString(series[i]);
    if (!uts->HasDating()) {
      Tcl_AppendResult(interp, "\"",
                       sername,
                       "\" does not have dating", NULL );
      return TCL_ERROR;
    }
    /* exists and have dating */
    
    /* then alloc serie data */
    
    tsd = tsg->series_data[i] = Tol_AllocSerieData();
    _CHECK_PTR_(tsd);
    ++tsg->size;

    str_len = strlen(sername);
    tsd->name = Tcl_Alloc(str_len+1);
    _CHECK_PTR_(tsd->name);
    strcpy(tsd->name,sername);

    str_len = strlen(uts->Identify());
    tsd->obj_name = Tcl_Alloc(str_len+1);
    _CHECK_PTR_(tsd->obj_name);
    strcpy(tsd->obj_name,uts->Identify());

    /* then alloc corresponding cursor */
    
    tsc = cursors.data[i] = Tol_AllocSerieCursor();
    _CHECK_PTR_(tsc);
    ++cursors.last;
    status = Tol_InitSerieCursor(tsc,tsd,uts,begin,end,RangeIsDefault);
    if (!status) {
      if (tsc->useful)
        return BadAlloc(interp);
      /* por aqui no deberia pasar, REVISALO!!! */
      Tcl_AppendResult(interp,"invalid range of dates",NULL);
      return TCL_ERROR;
    }
    if ( flagDating == 0 ) {
      flagDating = 1;
      firstDating = uts->Dating();
    } else if ( (flagDating == 1) && (firstDating != uts->Dating()) )
      flagDating = 2;
  }

  /* now we have a set of cursors */
  
  if (cursors.last==-1) {
    Tcl_AppendResult(interp,"the set of serie given have no data",NULL);
    return TCL_ERROR;
  }

  if (!Tol_SerieCursor::lowerD.HasValue()) {
    /* All series are lower unbounded */
    Tol_SerieCursor::lowerD = begin;
  }
  if (!Tol_SerieCursor::upperD.HasValue()) {
    /* All series are upper unbounded */
    Tol_SerieCursor::upperD = end;
  }
    
  int iter, idx_dating = -1;
  int tcl_code;
  BDat dat;
  BDate cur_date;
  Tol_SerieCursor * iter_cursor;
  int isunbounded;

  /* Now bound the unbounded cursors */
  
  for (iter=0; iter <= cursors.last; iter++) {
    tsc = cursors.data[iter];
    isunbounded = 0;
    if (tsc->lowerU) {
      /* is lower unbounded so bound it! */
      isunbounded = 1;
      tsc->cur_date = Tol_SerieCursor::lowerD;
      /* now on is not unbounded */
      tsc->lowerU = 0;
    }
    if (tsc->upperU) {
      /* is upper unbounded so bound it! */
      isunbounded = 1;
      tsc->until_date = Tol_SerieCursor::upperD;
     /* now on is not unbounded */
      tsc->upperU = 0;
    }
    if (isunbounded) {
      /* if unbounded ==> alloc resources */
      if (!Tol_AllocMemoryForCursor(tsc))
	return BadAlloc(interp);
    }
  }

  cursors.first_good = 0;
  cursors.first_sorted = cursors.last;
  cursors.sort();

  /* grow the interval if necessary */
  /*
    I think growing is not neccesary now

  int new_length;
  int has_grown;

  for ( iter = cursors.first_good; iter <= cursors.last; ++iter ) {
    tsc = cursors.data[iter];
    has_grown = 0;
    if ( tsc->lowerU && tsc->lowerD < tsc->cur_date ) {
      tsc->cur_date = tsc->tms->FirstNoLess( tsc->lowerD );
      has_grown = 1;
    }
    if ( tsc->upperU && tsc->upperD > tsc->until_date ) {
      tsc->until_date = tsc->tms->FirstNoGreat( tsc->upperD );
      has_grown = 1;
    }
    if ( has_grown ) {
      new_length = tsc->tms->Difference(tsc->cur_date,tsc->until_date)+1;
      tsc->_x_ = (int*)Tcl_Realloc((char*)tsc->_x_, sizeof(int)*new_length);
      if (!tsc->_x_)
        return BadAlloc(interp);
      tsc->_y_ = (double*)Tcl_Realloc((char*)tsc->_y_, sizeof(double)*new_length);
      if (!tsc->_y_)
        return BadAlloc(interp);
    }
  }
  */
  
  /* iter over the cursors */

  while (cursors.HasUseful()) {
    iter = cursors.first_good;
    tsc = cursors.data[iter];
    cur_date = tsc->cur_date;

    /* add this date to basic dating */
    tcl_code = tsg->AddDate(cur_date);

    if (tcl_code != TCL_OK)
      return TCL_ERROR;
    ++idx_dating;
    /* update every serie data having this date in its dating */
    do {
      iter_cursor = cursors.data[iter];
#if defined(_TOL_FROZEN_)
      dat = (*(iter_cursor->serie))[cur_date];
#else
      dat = (*(iter_cursor->serie))(cur_date);
#endif
      if (dat.IsKnown())
        iter_cursor->append(idx_dating,dat.Value());
      else
        iter_cursor->new_segment(interp);
      if (!iter_cursor->next_date()) {
        iter_cursor->new_segment(interp);
        cursors.compact( iter );
      }
      ++iter;
    } while (iter<=cursors.last && cur_date==cursors.data[iter]->cur_date);
    if (iter>cursors.last)
      cursors.first_sorted = cursors.last;
    else
      cursors.first_sorted = iter;
    cursors.sort();
  }
  
  if ( flagDating == 1 ) {
    tsg->dating_name = Tcl_Alloc(firstDating->Name().Length()+1);
    strcpy( tsg->dating_name, firstDating->Name() );
  }

  /*tsg->dating_length = idx_dating+1;*/
  return TCL_OK;
}

/*
 *  Tol_CreateSerieGrpData --
 *
 *    to be called from C++ interface.
 */

static
int Tol_CreateSerieGrpData (Tcl_Interp * interp,
                            BList * series,
                            Tol_SerieGroup ** tsgPtr,
                            BDate & begin,
                            BDate & end,
                            int RangeIsDefault)
{ 
  int objc = LstLength(series);
  scursors cursors;
  Tol_SerieGroup * tsg = NULL;

  *tsgPtr = NULL;

  tsg = *tsgPtr = Tol_AllocSerieGroup();
  _CHECK_PTR_(tsg);
    
  /*
   * alloc space for serie data array
   * but for now it's empty
   */
  
  tsg->size = 0; 
  tsg->series_data = (Tol_SerieData**)Tcl_Alloc(sizeof(Tol_SerieData*)*objc);
  _CHECK_PTR_(tsg->series_data);
  
  /*
   * alloc space for array of cursors
   * but for now it's empty
   */
  
  cursors.data = (Tol_SerieCursor**)Tcl_Alloc(sizeof(Tol_SerieCursor*)*objc);
  _CHECK_PTR_(cursors.data);
  
  /*
   * build cursor & serie data for every known serie
   */
  
  Tol_SerieData * tsd; 
  BUserTimeSerie * uts;
  Tol_SerieCursor * tsc;
  int str_len;
  int status;
  Tcl_DString uniBuffer;
  int flagDating = 0;
  BUserTimeSet * firstDating = 0;
  
  Tcl_DStringInit(&uniBuffer);
  Tol_SerieCursor::InitBounds();
  int i = 0;
  while(series) {
    uts = (BUserTimeSerie *)Car(series);
    /* verify serie */
    if ( Tol_GroupFindSerie(uts,tsg) != -1 )
      continue;
    
    /* alloc serie data */
    
    tsd = tsg->series_data[i] = Tol_AllocSerieData();
    _CHECK_PTR_(tsd);
    ++tsg->size;

    str_len = strlen(uts->Name());
    tsd->name = Tcl_Alloc(str_len+1);
    _CHECK_PTR_(tsd->name);
    strcpy(tsd->name,uts->Name());

    str_len = strlen(uts->Identify());
    tsd->obj_name = Tcl_Alloc(str_len+1);
    _CHECK_PTR_(tsd->obj_name);
    strcpy(tsd->obj_name,uts->Identify());

    tsd->tms_obj = uts;

    /* then alloc corresponding cursor */
    
    tsc = cursors.data[i] = Tol_AllocSerieCursor();
    _CHECK_PTR_(tsc);
    ++cursors.last;
    status = Tol_InitSerieCursor(tsc,tsd,uts,begin,end,RangeIsDefault);
    if (!status) {
      if (tsc->useful)
        return BadAlloc(interp);
      /* por aqui no deberia pasar, REVISALO!!! */
      Tcl_AppendResult(interp,"invalid range of dates",NULL);
      return TCL_ERROR;
    }
    if ( flagDating == 0 ) {
      flagDating = 1;
      firstDating = uts->Dating();
    } else if ( flagDating == 1 && firstDating != uts->Dating() )
      flagDating = 2;
    ++i;
    series = Cdr(series);
  }

  /* now we have a set of cursors */
  
  if (cursors.last==-1) {
    Tcl_AppendResult(interp,"the set of serie given have no data",NULL);
    return TCL_ERROR;
  }

  if (!Tol_SerieCursor::lowerD.HasValue()) {
    /* All series are lower unbounded */
    Tol_SerieCursor::lowerD = begin;
  }
  if (!Tol_SerieCursor::upperD.HasValue()) {
    /* All series are upper unbounded */
    Tol_SerieCursor::upperD = end;
  }
    
  int iter, idx_dating = -1;
  int tcl_code;
  BDat dat;
  BDate cur_date;
  Tol_SerieCursor * iter_cursor;
  int isunbounded;

  /* Now bound the unbounded cursors */
  
  for (iter=0; iter <= cursors.last; iter++) {
    tsc = cursors.data[iter];
    isunbounded = 0;
    if (tsc->lowerU) {
      /* is lower unbounded so bound it! */
      isunbounded = 1;
      tsc->cur_date = Tol_SerieCursor::lowerD;
      /* now on is not unbounded */
      tsc->lowerU = 0;
    }
    if (tsc->upperU) {
      /* is upper unbounded so bound it! */
      isunbounded = 1;
      tsc->until_date = Tol_SerieCursor::upperD;
     /* now on is not unbounded */
      tsc->upperU = 0;
    }
    if (isunbounded) {
      /* if unbounded ==> alloc resources */
      if (!Tol_AllocMemoryForCursor(tsc))
	return BadAlloc(interp);
    }
  }

  cursors.first_good = 0;
  cursors.first_sorted = cursors.last;
  cursors.sort();

  /* grow the interval if necessary */
  /*
    I think growing is not neccesary now

  int new_length;
  int has_grown;

  for ( iter = cursors.first_good; iter <= cursors.last; ++iter ) {
    tsc = cursors.data[iter];
    has_grown = 0;
    if ( tsc->lowerU && tsc->lowerD < tsc->cur_date ) {
      tsc->cur_date = tsc->tms->FirstNoLess( tsc->lowerD );
      has_grown = 1;
    }
    if ( tsc->upperU && tsc->upperD > tsc->until_date ) {
      tsc->until_date = tsc->tms->FirstNoGreat( tsc->upperD );
      has_grown = 1;
    }
    if ( has_grown ) {
      new_length = tsc->tms->Difference(tsc->cur_date,tsc->until_date)+1;
      tsc->_x_ = (int*)Tcl_Realloc((char*)tsc->_x_, sizeof(int)*new_length);
      if (!tsc->_x_)
        return BadAlloc(interp);
      tsc->_y_ = (double*)Tcl_Realloc((char*)tsc->_y_, sizeof(double)*new_length);
      if (!tsc->_y_)
        return BadAlloc(interp);
    }
  }
  */
  
  /* iter over the cursors */

  while (cursors.HasUseful()) {
    iter = cursors.first_good;
    tsc = cursors.data[iter];
    cur_date = tsc->cur_date;

    /* add this date to basic dating */
    tcl_code = tsg->AddDate(cur_date);

    if (tcl_code != TCL_OK)
      return TCL_ERROR;
    ++idx_dating;
    /* update every serie data having this date in its dating */
    do {
      iter_cursor = cursors.data[iter];
#if defined(_TOL_FROZEN_)
      dat = (*(iter_cursor->serie))[cur_date];
#else
      dat = (*(iter_cursor->serie))(cur_date);
#endif
      if (dat.IsKnown())
        iter_cursor->append(idx_dating,dat.Value());
      else
        iter_cursor->new_segment(interp);
      if (!iter_cursor->next_date()) {
        iter_cursor->new_segment(interp);
        cursors.compact( iter );
      }
      ++iter;
    } while (iter<=cursors.last && cur_date==cursors.data[iter]->cur_date);
    if (iter>cursors.last)
      cursors.first_sorted = cursors.last;
    else
      cursors.first_sorted = iter;
    cursors.sort();
  }
  
  if ( flagDating == 1 ) {
    tsg->dating_name = Tcl_Alloc(firstDating->Name().Length()+1);
    strcpy( tsg->dating_name, firstDating->Name() );
  }

  /*tsg->dating_length = idx_dating+1;*/
  return TCL_OK;
}

/*
 *  Implement tol::autocorr
 *
 *    tol::autocor type serie ?lagsize? ?range?
 *
 *   range will be a future option.
 *
 */


enum ACFType { ACF, PACF, IACF, IPACF };
 
int Tol_GetAutoCorr( Tcl_Interp * interp,
                     int objc, Tcl_Obj *CONST objv[],
                     Tcl_Obj * obj_result )
{
  char * type_name;
  int acf_type;
  char * valid_types[] = { "ACF", "PACF", "IACF", "IPACF" };
#define NTYPES sizeof(valid_types)/sizeof(valid_types[0])
  size_t i;
  int tcl_code;


  type_name = Tcl_GetString( objv[0] );
  for ( i = 0; i < NTYPES; ++i )
    if ( !strcmp( type_name, valid_types[i] ) )
      break;
  if ( i == NTYPES ) {
    Tcl_AppendStringsToObj( obj_result, "invalid autocorrelation funtion type '",
                            type_name, "' must be: ACF, PACF, IACF or IPACF", NULL );
    return TCL_ERROR;
  }

  acf_type = ACFType(i);

  BUserTimeSerie * uts;
  //uts = Tol_GetTimeSerie( interp, objv[1] );
  uts = (BUserTimeSerie*)Tol_ResolveObject(interp, objv[1], GraSerie());
  int size_cor = -1;
  if ( !uts ) {
    Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
    return TCL_ERROR;
  }
  if ( objc == 3 ) {
    tcl_code = Tcl_GetIntFromObj( interp, objv[2], &size_cor );
    if ( tcl_code != TCL_OK ) {
      Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
      Tcl_AppendStringsToObj( obj_result,
                              "\ninvalid autocorrelation function size", NULL );
      return TCL_ERROR;
    }
  }
  BDate I0 = uts->FirstDate();
  BDate I1 = uts->LastDate();
  int length;

  if ( !I0.HasValue() )
    I0 = BDate::DefaultFirst();
  if ( !I1.HasValue() )
    I1 = BDate::DefaultLast();

  BUserTimeSet * tms = uts->Dating();

  if ( !tms ) {
    Tcl_AppendStringsToObj(obj_result,"Serie \'",
                           Tcl_GetString(objv[1]),
                           "\' does not have dating", NULL);
    return TCL_ERROR;
  }

  /* ajustamos el intervalo de fechas al fechado */

  I0 = tms->FirstNoLess(I0);
  I1 = tms->FirstNoGreat(I1);
  length = tms->Difference(I0,I1)+1;
  if ( length <= 0 ) {
    Tcl_SetStringObj(obj_result,
                     "last date must be greater or equal than first",-1);
    return TCL_ERROR;
  }

  if ( size_cor < 0 ) {
    size_cor = length / 4;
    if ( !size_cor )
      size_cor = 1;
  }

  BArray<BDat> autocor( size_cor );
  BArray<BDat> serdata( length );
  BArray<BDat> autocor1( size_cor );
  BArray<BDat> * result_data = NULL;
  double sigma;

  uts->ReadData();
  for ( i = 0; i < size_t(length); i++ ) {
#if defined(_TOL_FROZEN_)
    serdata(i) = (*uts)[I0];
#else
    serdata(i) = (*uts)(I0); 
#endif
    I0 += tms;
  }
  sigma = 1 / sqrt( (long double) length );
  AutoCor( serdata, autocor, size_cor, 1);
  switch ( acf_type ) {
    case ACF:
      result_data = &autocor;
      break;
    case PACF:
      PartAutoCor( autocor, autocor1 );
      result_data = &autocor1;
      break;
    case IACF:
      InverseAutoCor( autocor, autocor1 );
      result_data = &autocor1;
      break;
    case IPACF:
      InvPartAutoCor( autocor, autocor1 );
      result_data = &autocor1;
  }
  
  Tcl_Obj * itemv [4];
  Tcl_Obj ** subitemv = (Tcl_Obj**)Tcl_Alloc( size_cor * sizeof(Tcl_Obj*) );
  BDat c;

  for ( i = 0; i < size_t(size_cor); i++ ) {
    c = (*result_data)(i);
    if ( c.Known() )
      subitemv[i] = Tcl_NewDoubleObj( c.Value() );
    else
      subitemv[i] = Tcl_NewStringObj( "?", -1 );
  }
  itemv[0] = Tcl_NewStringObj( uts->Identify(), -1 );
  itemv[1] = Tcl_NewIntObj( size_cor );
  itemv[2] = Tcl_NewDoubleObj( sigma );
  itemv[3] = Tcl_NewListObj( size_cor, subitemv );
  Tcl_SetListObj( obj_result, 4, itemv );
  Tcl_Free( (char*)subitemv );
  return TCL_OK;
}

typedef BDat (*StatFunc)(BSyntaxObject *);

struct SerieStats {
  char * name;
  StatFunc func;
};

static SerieStats
stats_data[] = {
  {"Count",     StatCount},
  {"Maximun",   StatMax},
  {"Minimun",   StatMin},
  {"Sum",       StatSum},
  {"Average",   StatAverage},
  {"Std. Dev.", StatStDs},
  {"Varianze",  StatVarianze},
  {"Asimetry",  StatAsimetry},
  {"Kurtosis",  StatKurtosis},
  {"Median",    StatMedian}
};

#define NUMSTATS sizeof(stats_data)/sizeof(stats_data[0])


/*
 *----------------------------------------------------------------------
 *
 * Tol_ComputeSerieStat --
 *
 *       Implements the new Tcl "::tol::seriestat" command.
 *
 *       tol::seriestat serieref      
 *           -> {serie-name {list of stats names & values}}
 *
 *       tol::seriestat serieref statname ?args?
 *            -> return the stat value. (not implemented yet)
 *
 * Results:
 *       
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

int Tol_ComputeSerieStat(Tcl_Interp * interp,
                         int objc, Tcl_Obj *CONST objv[],
                         Tcl_Obj * obj_result)
{
  BUserTimeSerie * uts;
  bool found;
  
  // uts = Tol_GetTimeSerie( interp, objv[0] );
  uts = (BUserTimeSerie*)Tol_ResolveObject(interp, objv[0], GraSerie());
  if ( !uts ) {
    Tcl_AppendObjToObj(obj_result, Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  Tcl_Obj * listv[2];
  if ( objc > 1 ) {
    Tcl_Obj **stats;
    stats = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*)*2*(objc-1) );
    //const int listsize = 2*(objc-1);
    //Tcl_Obj * stats[listsize];
    BDat dat;

    listv[0] = Tcl_NewStringObj(uts->Identify(),-1);
    for ( size_t i = 0; i < size_t(objc-1); i++ ) {
      found = false;
      for ( size_t j = 0; j < NUMSTATS; j++ ) {
        if (strcmp(Tcl_GetString(objv[i+1]),stats_data[j].name) == 0) {
          found = true;
          stats[2*i] = Tcl_NewStringObj(stats_data[j].name,-1);
          dat = (*(stats_data[j].func))(uts);
          if ( dat.IsKnown() )
            stats[2*i+1] = Tcl_NewDoubleObj(dat.Value());
          else {
            BText name(dat.Name());
            stats[2*i+1] = Tcl_NewStringObj(name,-1);
          }
        }
      }
      if (found == false) {
        Tcl_AppendStringsToObj(obj_result, "bad statistics name \"",
                          Tcl_GetString(objv[i+1]),"\"", NULL);
        Tcl_Free((char*)(stats));
        return TCL_ERROR;
      }      
    }
    listv[1] = Tcl_NewListObj(2*(objc-1), stats);
    Tcl_SetListObj(obj_result, 2, listv);
    Tcl_Free((char*)(stats));
    return TCL_OK;

  } else {  
    // {Count 20 ... Median 0.23}
    const int listsize = 2*NUMSTATS;
    Tcl_Obj * stats[listsize];
    BDat dat;

    listv[0] = Tcl_NewStringObj(uts->Identify(),-1);
    for ( size_t i = 0; i < NUMSTATS; i++ ) {
      stats[2*i] = Tcl_NewStringObj(stats_data[i].name,-1);
      dat = (*(stats_data[i].func))(uts);
      if ( dat.IsKnown() )
        stats[2*i+1] = Tcl_NewDoubleObj(dat.Value());
      else {
        BText name(dat.Name());
        stats[2*i+1] = Tcl_NewStringObj(name,-1);
      }
    }
    listv[1] = Tcl_NewListObj(listsize, stats);
    Tcl_SetListObj(obj_result, 2, listv);
    return TCL_OK;
  }

}
