/* toltime.cpp - GNU/tolTcl Library

   Copyright (C) 2001, 2002, 2003 - Bayes Decisión, SL (Spain [EU])

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
#include <tol/tol_blanguag.h>
#include <tol/tol_btmsgra.h>

#include <toltclInt.h>

struct TMS_ClientData {

  /* timeset iterated */

  BUserTimeSet * uts;
  BDate current;

  /* token used to remove the command */
  
  Tcl_Command cmd_token;

  void Init( BUserTimeSet * t) {
    if ( (uts = t) ) {
      uts->IncNRefs();
      Start( BDate::DefaultFirst() );
    }
  }
  void Destroy() {
    if ( uts ) {
      uts->DecNRefs();
      /* destroy in case NRefs has reached 0 */
      DESTROY(uts);
      uts = NULL;
    }
  }
  void Start ( BDate & d ) {
    if ( uts ) 
      current = uts->FirstNoLess( d );
  }
  void Start ( char * txt ) {
    if ( txt ) 
      current.PutDate( txt );
    else
      current = BDate::DefaultFirst();
    if ( uts ) 
      current = uts->FirstNoLess( current );
  }
  BDate & Get () {
    return current;
  }
  BDate & Next () {
    if ( uts )
      current = uts->Successor( current );
    return current;
  }
  BDate & Previous () {
    if ( uts )
      current = uts->Predecessor( current );
    //current -= uts;
    return current;
  }
  int HasValue () {
    return current.HasValue();
  }
  int    Year()       const { return current.Year();  }
  int  Month()      const { return current.Month(); }
  int    Day()        const { return current.Day();   }
  int    Hour()       const { return current.Hour(); }
  int    Minute()     const { return current.Minute(); }
  double         Second()     const { return current.Second(); }
#if defined(_TOL_FROZEN_)
  int Contain( BDate & d ) { return uts->Contain( d ); }
#else
  int Contain( BDate & d ) { return uts->Contain( d, BTRUE ); }
#endif
};

static
int Tol_TMSInstanceProc _ANSI_ARGS_((ClientData clientData,
                                    Tcl_Interp *interp,
                                    int objc, Tcl_Obj *CONST objv[]));

static
void Tol_TMSDelInstanceProc _ANSI_ARGS_((ClientData clientData));

/*
 *  Create a new command to access the given TOL timeset object name.
 *
 */

int Tol_CreateTMSCmd( Tcl_Interp * interp, Tcl_Obj * tmscmd,
                      Tcl_Obj * tmsname, Tcl_Obj * obj_result )
{
  const BSyntaxObject * tol_obj;
  BUserTimeSet * uts;
  Tcl_CmdInfo cmd_info;
  char * str_tmscmd = Tcl_GetString(tmscmd);
  
  if (!*str_tmscmd) {
    Tcl_AppendStringsToObj(obj_result,
                           "invalid timeset command name: must be not empty",
                           NULL);
    return TCL_ERROR;
  }
  
  /* verify if tmscmd already exists as a command */
  
  if (Tcl_GetCommandInfo(interp,str_tmscmd,&cmd_info) &&
    (cmd_info.objProc == Tol_TMSInstanceProc)) {
    Tcl_AppendStringsToObj(obj_result, "\"",str_tmscmd,"\"",
                           "already exists as timeset command",NULL);
    return TCL_ERROR;
  }

  /* now look for tms in tol */

  BGrammar * gra = GraTimeSet();

  if ( !gra ) {
    Tcl_AppendStringsToObj( obj_result,
                            "TimeSet grammar not found", NULL );
    return TCL_ERROR;
  }

  if (!(tol_obj=Tol_ResolveObject(interp, tmsname, obj_result/*, gra*/))) {
    return TCL_ERROR;
  }

  uts = (BUserTimeSet *)tol_obj;

  TMS_ClientData *clientData = (TMS_ClientData*)Tcl_Alloc(sizeof(TMS_ClientData));

  if (!clientData)
    return TCL_ERROR;

  clientData->Init(uts);
  clientData->cmd_token = Tcl_CreateObjCommand( interp, 
                                                str_tmscmd,
                                                &Tol_TMSInstanceProc,
                                                (ClientData)clientData,
                                                &Tol_TMSDelInstanceProc );
  
  return TCL_OK;
}

/*
 * Destroy the tcl tms commands given in objv.
 *
 */

int Tol_DestroyTMSCmd( Tcl_Interp * interp, 
                       int objc, Tcl_Obj *CONST objv[],
                       Tcl_Obj * obj_result )
{
  Tcl_CmdInfo cmd_info;
  char * cmdname;
  
  for (int i=0; i<objc; ++i) {
    cmdname = Tcl_GetString(objv[i]);
    if ( Tcl_GetCommandInfo(interp,cmdname,&cmd_info) &&
         (cmd_info.objProc == Tol_TMSInstanceProc) ) {
      if (Tcl_DeleteCommand(interp,cmdname) == -1) {
        Tcl_AppendStringsToObj( obj_result,
                                "could not delete \"",cmdname,"\"",NULL);
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendStringsToObj( obj_result, 
                              "can't find timeset command \"",
                              cmdname,"\"",NULL);
      return TCL_ERROR;
    }
  }

  return TCL_OK;
}

int Tol_TMSInstanceProc( ClientData clientData,
                         Tcl_Interp *interp,
                         int objc, Tcl_Obj *CONST objv[])
{
  char * cmd_name = Tcl_GetString( objv[0] );

  if ( objc < 2 || objc > 3 ) {
    Tcl_AppendResult( interp, "wrong # args: should be\n",
                      cmd_name, " contain date\n",
                      cmd_name, " start date\n",
                      cmd_name, " get\n",
                      cmd_name, " next\n",
                      cmd_name, " previous",
                      NULL );
    return TCL_ERROR;
  }

  char * option = Tcl_GetString( objv[1] );
  int len = strlen( option );
  TMS_ClientData * tmsData = (TMS_ClientData *)clientData;
  int tcl_code = TCL_OK;
  BDate dte;

  if ( !strncmp( "contain", option, len ) ) {
    if ( objc != 3 ) {
      Tcl_AppendResult( interp, "wrong # args: should be, ",
                        cmd_name, " contain date", NULL );
      tcl_code = TCL_ERROR;
    } else {
      tcl_code = Tol_ListToDate( interp, objv[2], dte, 
                                 Tcl_GetObjResult(interp) );
      if ( tcl_code == TCL_OK )
        Tcl_SetIntObj( Tcl_GetObjResult(interp),
                       tmsData->Contain( dte ) );
    }
    return tcl_code;
  } else if ( !strncmp( "start", option, len ) ) {
    if ( objc != 3 ) {
      Tcl_AppendResult( interp, "wrong # args: should be, ",
                        cmd_name, " start date", NULL );
      tcl_code = TCL_ERROR;
    } else {
      tcl_code = Tol_ListToDate( interp, objv[2], dte, 
                                 Tcl_GetObjResult(interp) );
      if ( tcl_code == TCL_OK )
        tmsData->Start( dte );
    }
  } else {
    if ( objc != 2 ) {
      Tcl_AppendResult( interp, "wrong # args: should be, ",
                        cmd_name, " get\n",
                        cmd_name, " next\n",
                        cmd_name, " previous", NULL );
      tcl_code = TCL_ERROR;
    } else if ( !strncmp( "next", option, len ) ) {
      tmsData->Next();
    } else if ( !strncmp( "previous", option, len ) ) {
      tmsData->Previous();
    } else if ( strncmp( "get", option, len ) ) {
      Tcl_AppendResult( interp, "invalid option '",
                        option,
                        "', should be: start, get, next or previous", NULL );
      tcl_code = TCL_ERROR;
    }
  }

  if ( tcl_code != TCL_ERROR ) {
    if ( tmsData->HasValue() )
      tcl_code = Tol_DateToList( tmsData->Get(), Tcl_GetObjResult(interp) );
    else
      Tcl_SetResult( interp, "-1 -1 -1 -1 -1 -1", TCL_VOLATILE );
  }
  return tcl_code;
}

void Tol_TMSDelInstanceProc( ClientData clientData )
{
   TMS_ClientData * tmsData = (TMS_ClientData*)clientData;

   if ( tmsData )
      tmsData->Destroy();
}


/*
 *  Implement tol::date
 *
 *       tol::date first              -> default first date
 *       tol::date last               -> default last  date
 *       tol::date now     ?fmt?       -> today date/time
 *       tol::date weekday date        -> 0(sunday), 1(monday), ..., 6(saturday)
 *       tol::date nod     year month  -> number of days of month/year
 *       tol::date +day    date ?n?    -> date ++ ?n days?
 *       tol::date +month  date ?n?    -> date ++ ?n months? 
 *
 *  Results:
 *       a valid date
 *
 *       date is always a list {Y M D h m s} 
 *
 */

int Tol_DateOption( Tcl_Interp * interp,
                    int objc, Tcl_Obj *CONST objv[],
                    Tcl_Obj * obj_result )
{
  char * cmd = Tcl_GetString( objv[0] );
  if ( objc < 2 || objc > 4 ) {
    Tcl_AppendStringsToObj( obj_result, "wrong # args, should be:\n",
                            cmd, " first\n",
                            cmd, " last\n",
                            cmd, " now ?fmt?\n",
                            cmd, " weekday date\n",
                            cmd, " days year month\n",
                            cmd, " +day date ?n?\n",
                            cmd, " +month date ?n?\n", NULL );
    return TCL_ERROR;
  }

  char * option = Tcl_GetString( objv[1] );
  int len = strlen( option );
  int tcl_code = TCL_OK;
  int opt_code;
  int min_args;
  int max_args;

#define OPT_UNKNOWN   -1
#define OPT_FIRST     0
#define OPT_LAST      1
#define OPT_NOW       2
#define OPT_WEEKDAY   3
#define OPT_DAYS      4
#define OPT_INCDAYS   5
#define OPT_INCMONTHS 6

  objc -= 2;
  objv += 2;
  opt_code = OPT_UNKNOWN;
  min_args = 0;
  max_args = 0;

  if ( !strncmp( "first", option, len ) )
    opt_code = OPT_FIRST;
  else if ( !strncmp( "last", option, len ) )
    opt_code = OPT_LAST;
  else if ( !strncmp( "now", option, len ) ) {
    opt_code = OPT_NOW;
    min_args = 0; max_args = 1;
  } else if ( !strncmp( "weekday", option, len ) ) {
    opt_code = OPT_WEEKDAY;
    min_args = max_args = 1;
  } else if ( !strncmp( "days", option, len ) ) {
    opt_code = OPT_DAYS;
    min_args = max_args = 2;
  } else if ( !strncmp( "+day", option, len ) ) {
    opt_code = OPT_INCDAYS;
    min_args = 1;
    max_args = 2;
  } else if ( !strncmp( "+month", option, len ) ) {
    opt_code = OPT_INCMONTHS;
    min_args = 1;
    max_args = 2;
  } else {
    Tcl_AppendStringsToObj( obj_result,
      "invalid option, should be: first, last, now, weekday, days, +day or +month",
      NULL );
    return TCL_ERROR;
  }

  if ( objc < min_args || objc > max_args ) {
    Tcl_AppendStringsToObj( obj_result,
      "wrong # args calling '", cmd, " ", option, "'", NULL );
    return TCL_ERROR;
  }

  BDate date;
  int YM[2], i;

  switch ( opt_code ) {
  case OPT_FIRST:
    date = BDate::DefaultFirst();
    break;
  case OPT_LAST:
    date = BDate::DefaultLast();
    break;
  case OPT_NOW:
    date = DteNow();
    if (objc == 1) {
      char * fmt = Tcl_GetString( objv[0] );
      if ( *fmt ) {
        BDateFormat bformat( fmt );
        BText dfmt( bformat.DateToText(date) );
        Tcl_AppendStringsToObj( obj_result, dfmt.Buffer(), NULL );
        return TCL_OK;
      }
    }
    break;
  case OPT_WEEKDAY:
    tcl_code = Tol_ListToDate( interp, objv[0], date, obj_result );
    if ( tcl_code != TCL_OK )
      return TCL_ERROR;
    Tcl_SetIntObj( obj_result, date.WeekDay() % 7 );
    return TCL_OK;
  case OPT_DAYS:
    for ( i = 0; i < 2; i++ ) {
      tcl_code = Tcl_GetIntFromObj( interp, objv[i], YM+i );
      if ( tcl_code != TCL_OK ) {
        Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
        return TCL_ERROR;
      }
    }
    date.PutDate( YM[0], YM[1] );
    if ( !date.HasValue() ) {
      Tcl_AppendStringsToObj( obj_result, "Invalid month '",
        Tcl_GetString(objv[0]), "-", Tcl_GetString(objv[1]),
        "'", NULL );
      return TCL_ERROR;
    }
    Tcl_SetIntObj( obj_result, DteDaysInMonth( YM[1], YM[0] ) );
    return TCL_OK;
  case OPT_INCDAYS:
    tcl_code = Tol_ListToDate( interp, objv[0], date, obj_result );
    if ( tcl_code != TCL_OK )
      return TCL_ERROR;
    i = 1;
    if ( objc == 2 ) {
      tcl_code = Tcl_GetIntFromObj( interp, objv[1], &i );
      if ( tcl_code != TCL_OK ) {
        Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
        return TCL_ERROR;
      }
    }
    date.IncDay( i );
    break;
  case OPT_INCMONTHS:
    tcl_code = Tol_ListToDate( interp, objv[0], date, obj_result );
    if ( tcl_code != TCL_OK )
      return TCL_ERROR;
    i = 1;
    if ( objc == 2 ) {
      tcl_code = Tcl_GetIntFromObj( interp, objv[1], &i );
      if ( tcl_code != TCL_OK ) {
        Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
        return TCL_ERROR;
      }
    }
    date.IncMonth( i );
  }
  return Tol_DateToList(date, obj_result );
}

int Tol_ListToDate( Tcl_Interp * interp, Tcl_Obj * list,
                    BDate & date, Tcl_Obj * obj_result )
{
  int tcl_code;
  int lst_len;
  Tcl_Obj ** lst_items;

  tcl_code = Tcl_ListObjGetElements(interp,list,&lst_len,&lst_items);
  if (tcl_code != TCL_OK) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  if ( lst_len < 1 || lst_len > 6 ) {
    Tcl_AppendStringsToObj( obj_result, "Invalid date '",
    Tcl_GetString(list), "' should be a list {Y ?M D h m s?}", NULL );
    return TCL_ERROR;
  } else {
    int arr_date[6] = {0,1,1,0,0};
    double second = 0.0;
    int top_int;

    if ( lst_len < 6 )
      top_int = lst_len;
    else {
      top_int = 5;
      tcl_code = Tcl_GetDoubleFromObj( interp, lst_items[5], &second) ;
      if ( tcl_code != TCL_OK ) {
        Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
        return TCL_ERROR;
      }      
    }
    for ( int i = 0; i < top_int; i++ ) {
      tcl_code = Tcl_GetIntFromObj( interp, lst_items[i], arr_date+i) ;
      if ( tcl_code != TCL_OK ) {
        Tcl_AppendObjToObj( obj_result, Tcl_GetObjResult(interp) );
        return TCL_ERROR;
      }
    }
    date.PutDate( arr_date[0], arr_date[1],
                  arr_date[2], arr_date[3],
                  arr_date[4], second );
  }
  if ( !date.HasValue() ) {
    Tcl_AppendStringsToObj( obj_result, "Invalid date '",
                            Tcl_GetString(list), "'", NULL );
    return TCL_ERROR;
  }
  return TCL_OK;
}

#define IT_YEAR   0
#define IT_MONTH  1
#define IT_DAY    2
#define IT_HOUR   3
#define IT_MINUTE 4
#define IT_SECOND 5
#define IT_SIZE   6

int Tol_DateToList( BDate & date, Tcl_Obj * obj_result )
{
  Tcl_Obj * date_items[IT_SIZE];

  date_items[IT_YEAR]   = Tcl_NewIntObj   ( date.Year() );
  date_items[IT_MONTH]  = Tcl_NewIntObj   ( date.Month() );
  date_items[IT_DAY]    = Tcl_NewIntObj   ( date.Day() );
  date_items[IT_HOUR]   = Tcl_NewIntObj   ( date.Hour() );
  date_items[IT_MINUTE] = Tcl_NewIntObj   ( date.Minute() );
  date_items[IT_SECOND] = Tcl_NewDoubleObj( date.Second() );
  Tcl_SetListObj( obj_result, IT_SIZE, date_items );
  return TCL_OK;
}
