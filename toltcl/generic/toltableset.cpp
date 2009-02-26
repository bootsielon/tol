/* toltableset.cpp: This file implements the command to manipulate a set
                    as a table.
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

#ifdef _MSC_VER
/* for _finite */
#  include <float.h>
#endif

#include <ctype.h>
#include <toltclInt.h>
#include <tt_crash.h>

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_bcommon.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>

#include <tt_pool.h>
#include <tt_tclutils.h>

#ifndef NDEBUG
#define _DEBUG_TABLE_
#endif

class Tol_Table;
class Tol_TableSet;

class Tol_ColumnData {
public:
  enum enumColType { Text=0, Real, Date };

  Tol_ColumnData( enumColType t );
  ~Tol_ColumnData();
  enumColType GetType() {
    return type;
  }
  int HasINF() {
    return hasINF;
  }
  int HasUNK() {
    return hasUNK;
  }

  /* return the current number of rows */

  int GetSize() {
    int size;
    
    Tcl_ListObjLength(NULL, values, &size);
    return size;
  }

  Tcl_Obj * GetValues() {
    return values;
  }

  /* name <colname> type <coltype> [unknown (yes|no) INF (yes|no)] */

  Tcl_Obj * GetInfo();

  /* interp <-- values */

  int ReturnData(Tcl_Interp * interp);
  
  /* return values[row] in cell */

  int ReturnCell(Tcl_Interp * interp, int row);

  /*
   * return the value[row], leave in interp an error message if
   * something went wrong.
   */

  Tcl_Obj * GetCell(Tcl_Interp * interp, int row);

  /*
   * apply script to values[0..] 
   *
   *  eval $script $row value[$row]
   */

  int Apply(Tcl_Interp * interp, Tcl_Obj * script);

  void SetName( const char * str );

  int AppendBDat(Tcl_Interp * interp, BDat & dat);
  int AppendSyntaxObject( Tcl_Interp * interp, BSyntaxObject * so );
  int PrependSyntaxObject( Tcl_Interp * interp, BSyntaxObject * so );
  int AppendEmpty();
  int PrependEmpty();
protected:
  /* append functions */
  int AppendReal( Tcl_Interp * interp, double v );
  int AppendInfinity();
  int AppendUnknown();
  int AppendEmptyReal();
  int AppendText( char * txt );
  /* prepend functions */
  int PrependReal( Tcl_Interp * interp, double v );
  int PrependInfinity();
  int PrependUnknown();
  int PrependEmptyReal();
  int PrependText( char * txt );
private:
  Tcl_DString dsName;
  enumColType type;
  int hasINF;
  int hasUNK;
  /* list of column values */
  Tcl_Obj * values;
};

class Tol_Table {
public:
  enum enumHeaderType { Struct, Row, Generic };

  Tol_Table(Tcl_Interp * _interp, char * name);
  ~Tol_Table();

  void CreateCommand(char * cmd_name) {
    if (cmd_status == TCL_OK )
      Tcl_CreateObjCommand(interp, cmd_name, Tol_Table::ObjCmd,
                           (ClientData)this, Tol_Table::DeleteCmd);
  }

  int CmdStatus() {
    return cmd_status;
  }
 
  static int ObjCmd( ClientData clientData,
                     Tcl_Interp *interp,
                     int objc, Tcl_Obj *CONST objv[] );

  static void DeleteCmd( ClientData clientData );

  Tcl_Interp * GetInterp(void) {
    return interp;
  }
  int GetRows() {
    return rows;
  }
  
  /*
   * implement: tableset info name
   */

  int ReturnName();

  /*
   * implement: tableset info header
   */

  int ReturnHeaderInfo();

  /*
   * implement: tableset info datarows
   */

  int ReturnRowsNumber();

  /*
   * implement: tableset info rowsnames
   */

  int ReturnRowsNames();

  /*
   * implement: tableset info columns
   */

  int ReturnColumnsInfo();

  /*
   * implement: tableset data get i j
   *
   * interp <-- set_data(row,col)
   */

  int ReturnCell( int row, int col );

  /*
   * implement: tableset data column col
   *
   * interp <-- set_data(col)->ReturnData(interp)
   */

  int ReturnColumn( int col );

  /*
   * implement: tableset data apply all script
   */

  int ApplyAll( Tcl_Obj * script );

  /*
   * implement: tableset data apply row idx script
   */

  int ApplyRow( int row, Tcl_Obj * script );

  /*
   * implement: tableset data apply column idx script
   */

  int ApplyColumn( int col, Tcl_Obj * script );
  
protected:
  Tcl_Interp * interp;

  int   rows;
  Tcl_DString dsName;
  /* how are gotten header names */
  enumHeaderType  headerType;
  int extraColumns;
  ToltclPool columns;
  Tcl_Obj ** rowsNames;
  int cmd_status;
  Tcl_Obj ** retBuffer;

  Tol_ColumnData ** Columns( int i ) {
    return (Tol_ColumnData**)columns._GetObject(i);
  }
};

class Tol_TableSet : public Tol_Table {
public:
  Tol_TableSet(Tcl_Interp * _interp, char * setname, BSet * set,
	       Tcl_Obj * obj_result);  
protected:
  int Build(BSet * set, Tcl_Obj * obj_result);
};

class Tol_TableMatrix : public Tol_Table {
public:
  Tol_TableMatrix(Tcl_Interp * _interp, char * matname, BMatrix<double> *mat,
                  Tcl_Obj * obj_result);
protected:
  int Build(BMatrix<double> *mat, Tcl_Obj * obj_result);  
};

/*
 * Tol_ColumnData implementation
 *
 */

/*
 *   Tol_ColumnData::Tol_ColumnData 
 * 
 *     Constructor
 */

Tol_ColumnData::Tol_ColumnData( Tol_ColumnData::enumColType t )
{
  Tcl_DStringInit( &dsName );
  type = t;
  hasINF = 0;
  hasUNK = 0;
  Tcl_IncrRefCount(values=Tcl_NewListObj(0, NULL));
}

/*
 *  Tol_ColumnData::~Tol_ColumnData()
 *
 *    Destructor
 */

Tol_ColumnData::~Tol_ColumnData()
{
  Tcl_DStringFree(&dsName);
  Tcl_DecrRefCount(values);
}

/* 
 *  Tol_ColumnData::GetInfo()
 *
 *    return 
 *      name <colname> type <coltype> [unknown (0|1) INF (0|1)]
 */

Tcl_Obj * Tol_ColumnData::GetInfo()
{
  Tcl_Obj * info;

#define CD_NAME    0
#define CD_NAMEV   CD_NAME  + 1
#define CD_TYPE    CD_NAMEV + 1
#define CD_TYPEV   CD_TYPE  + 1
#define CD_UNK     CD_TYPEV + 1
#define CD_UNKV    CD_UNK   + 1
#define CD_INF     CD_UNKV  + 1
#define CD_INFV    CD_INF   + 1
#define CD_SIZE    CD_INFV  + 1

  int itemc = CD_UNK;
  Tcl_Obj * itemv[CD_SIZE];

  itemv[CD_NAME] = Tcl_NewStringObj( "name", -1 );
  itemv[CD_NAMEV] = Tcl_NewStringObj( Tcl_DStringValue(&dsName), -1 );
  itemv[CD_TYPE] = Tcl_NewStringObj( "type", -1 );
  switch ( type ) {
    case Text:
      itemv[CD_TYPEV] = Tcl_NewStringObj( "Text", -1 );
      break;
    case Real:
      itemv[CD_TYPEV] = Tcl_NewStringObj( "Real", -1 );
      itemv[CD_UNK] = Tcl_NewStringObj( "unknown", -1 );
      itemv[CD_UNKV] = Tcl_NewIntObj( hasUNK );
      itemv[CD_INF] = Tcl_NewStringObj( "infinity", -1 );
      itemv[CD_INFV] = Tcl_NewIntObj( hasINF );
      itemc = CD_SIZE;
      break;
    case Date:
      itemv[CD_TYPEV] = Tcl_NewStringObj( "Date", -1 );
  }
  info = Tcl_NewListObj( itemc, itemv );
  return info;
}


/*
 *  Tol_ColumnData::ReturnData
 *
 *    return to interp the values of the column
 *
 */

int Tol_ColumnData::ReturnData(Tcl_Interp * interp)
{
  Tcl_SetObjResult(interp, values);
  return TCL_OK;
}

Tcl_Obj * Tol_ColumnData::GetCell(Tcl_Interp * interp, int row)
{
  Tcl_Obj * obj = NULL;
  
  if (Tcl_ListObjIndex(interp, values, row, &obj)==TCL_OK)
    if (!obj) {
      char buffer[256];
      sprintf(buffer,"GetCell, index '%d', out of range", row);
      Tcl_AppendResult(interp, buffer, NULL);
    }
  return obj;
}

int Tol_ColumnData::ReturnCell(Tcl_Interp * interp, int row)
{
  Tcl_Obj * obj;

  if (Tcl_ListObjIndex(interp, values, row, &obj)==TCL_OK) {
    if (obj) {
      Tcl_SetObjResult(interp, obj);
      return TCL_OK;
    }
    char buffer[256];
    sprintf(buffer,"ReturnCell, index '%d', out of range", row);
    Tcl_AppendResult(interp, buffer, NULL);
  }
  return TCL_ERROR;
}

/*
 *  Tol_ColumnData::Apply
 *
 *    apply script to values[0..] 
 *
 *      eval $script $row value[$row]
 */
int Tol_ColumnData::Apply(Tcl_Interp * interp, Tcl_Obj * scriptObj)
{
  Tcl_Obj * args[2];
  Tcl_Obj ** items;
  int count;

  Tcl_IncrRefCount(args[0]=Tcl_NewObj());
  
  /* init script evaluator */

  EvalScript script(interp, scriptObj, 2, args );
  /* get the array of elements */
  Tcl_ListObjGetElements(interp, values, &count, &items);
  for (int r = 0; (r < count) && (script.Status()==TCL_OK); r++) {
    /* set the arg row value */
    if (Tcl_IsShared(args[0])) {
      Tcl_DecrRefCount(args[0]);
      Tcl_IncrRefCount(args[0] =Tcl_NewIntObj(r));
    } else
      Tcl_SetIntObj(args[0], r);
    /* set the cell value */
    args[1] = items[r];
 
   /* invoke the script */
    
    script.Eval();
  }
  Tcl_DecrRefCount(args[0]);
  return script.Status();
}

void Tol_ColumnData::SetName( const char * str )
{
  Tcl_DStringFree( &dsName );
  Tcl_ExternalToUtfDString( NULL, str, -1, &dsName );  
}

/*
 *  Tol_ColumnData::AppendReal
 */

int Tol_ColumnData::AppendReal(Tcl_Interp * interp, double v)
{
#ifdef _DEBUG_TABLE_
  if ( type != Real ) {
    Tcl_AppendResult( interp,
                      "Tol_ColumnData::AppendReal called on not Real column", NULL );
    return TCL_ERROR;
  }
#endif
  return Tcl_ListObjAppendElement(interp, values, Tcl_NewDoubleObj(v));
}

/*
 *  Tol_ColumnData::PrependReal
 */

int Tol_ColumnData::PrependReal(Tcl_Interp * interp, double v)
{
#ifdef _DEBUG_TABLE_
  if ( type != Real ) {
    Tcl_AppendResult( interp,
                      "Tol_ColumnData::PrependReal called on not Real column", NULL );
    return TCL_ERROR;
  }
#endif
  Tcl_Obj * items[1];
  items[0] = Tcl_NewDoubleObj(v);
  return Tcl_ListObjReplace(NULL, values, 0, 0, 1, items);
}

/*
 *  Tol_ColumnData::AppendInfinity
 */

int Tol_ColumnData::AppendInfinity()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasINF = 1;
  return Tcl_ListObjAppendElement(NULL, values, Tcl_NewStringObj("INF",-1));
}

/*
 *  Tol_ColumnData::PrependInfinity
 */

int Tol_ColumnData::PrependInfinity()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasINF = 1;
  Tcl_Obj * items[1];
  items[0] = Tcl_NewStringObj("INF",-1);
  return Tcl_ListObjReplace(NULL, values, 0, 0, 1, items);
}

/*
 *  Tol_ColumnData::AppendUnknown --
 */

int Tol_ColumnData::AppendUnknown()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasUNK = 1;
  return Tcl_ListObjAppendElement(NULL, values, Tcl_NewStringObj("?",-1));
}

/*
 *  Tol_ColumnData::PrependUnknown --
 */

int Tol_ColumnData::PrependUnknown()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasUNK = 1;
  Tcl_Obj * items[1];
  items[0] = Tcl_NewStringObj("?",-1);
  return Tcl_ListObjReplace(NULL, values, 0, 0, 1, items);
}

/*
 *  Tol_ColumnData::AppendEmptyReal --
 */

int Tol_ColumnData::AppendEmptyReal()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasUNK = 1;
  return Tcl_ListObjAppendElement(NULL, values, Tcl_NewObj());
}

/*
 *  Tol_ColumnData::PrependEmptyReal --
 */

int Tol_ColumnData::PrependEmptyReal()
{
#ifdef _DEBUG_TABLE_
  if ( type != Real )
    return TCL_ERROR;
#endif
  hasUNK = 1;
  Tcl_Obj * items[1];
  items[0] = Tcl_NewObj();
  return Tcl_ListObjReplace(NULL, values, 0, 0, 1, items);
}

/*
 *  Tol_ColumnData::AppendText --
 *
 */

int Tol_ColumnData::AppendText( char * txt )
{
#ifdef _DEBUG_TABLE_
  if ( type == Real )
    return TCL_ERROR;
#endif

  while (*txt && isspace(*txt))
    ++txt;
  return Tcl_ListObjAppendElement(NULL, values, Tcl_NewStringObj(txt,-1));
}

/*
 *  Tol_ColumnData::PrependText --
 *
 */

int Tol_ColumnData::PrependText( char * txt )
{
#ifdef _DEBUG_TABLE_
  if ( type == Real )
    return TCL_ERROR;
#endif
  while (*txt && isspace(*txt))
    ++txt;
  Tcl_Obj * items[1];
  items[0] = Tcl_NewStringObj(txt,-1);
  return Tcl_ListObjReplace(NULL, values, 0, 0, 1, items);
}

/*
 * Tol_columnData::AppendBDat --
 *
 */

int Tol_ColumnData::AppendBDat(Tcl_Interp * interp, BDat & dat)
{
  if ( dat.IsKnown() ) {
    double v = dat.Value();
    if ( isinf(v) )
      AppendInfinity();
    else
      AppendReal(interp, v); 
  } else
    AppendUnknown();
  return TCL_OK;
}

/*
 *  Tol_ColumnData::AppendSyntaxObject --
 *
 */

int Tol_ColumnData::AppendSyntaxObject(Tcl_Interp * interp, BSyntaxObject * so)
{
  enumColType so_type = Text;

  if ( so->Grammar() == GraReal() )
    so_type = Real;
  else if ( so->Grammar() == GraDate() )
    so_type = Date;
  if ( type == Real ) {
    if ( so_type == Real ) {
      /* keep on Real column */

      return AppendBDat(interp, Dat(so));
      /*
      BDat dat( Dat(so) );

      if ( dat.IsKnown() ) {
        double v = dat.Value();
        if ( isinf(v) )
          AppendInfinity();
        else
          AppendReal(interp, v); 
      } else
        AppendUnknown();
      return TCL_OK;
      */
    } else {
      if ((so_type == Date) && GetSize()) {
        /* start a Date column */
        type = Date; 
      } else {
        /* convert to Text column, what a shame!!! */
        type = Text;
      }
    }
  } else if ( type == Date && so_type != Date ) {
    /* change from Date to Text */
    type = Text;
  }

  /*** SI so_type == Text adicionar el contenido de las cadenas sin las "'s ***/

  /* now add the info regardless being Text or Date */
  BText info(so->Info());
  AppendText(info.Buffer());

  return TCL_OK;
}

/*
 *  Tol_ColumnData::PrependSyntaxObject --
 *
 */

int Tol_ColumnData::PrependSyntaxObject( Tcl_Interp * interp, BSyntaxObject * so )
{
  enumColType so_type = Text;

  if ( so->Grammar() == GraReal() )
    so_type = Real;
  else if ( so->Grammar() == GraDate() )
    so_type = Date;
  if ( type == Real ) {    
    if ( so_type == Real ) {
      /* keep on Real column */
      
      BDat dat( Dat(so) );

      if ( dat.IsKnown() ) {
        double v = dat.Value();
        if ( isinf(v) )
          PrependInfinity();
        else
          PrependReal(interp, v); 
      } else
        PrependUnknown();
      return TCL_OK;
    } else {
      /* convert to Text column, what a shame!!! */
      type = Text;
    }
  } else if ( type == Date && so_type != Date ) {
    /* change from Date to Text */
    type = Text;
  }

  /* now add the info regardless being Text or Date */
  BText info(so->Info());
  PrependText(info.Buffer());

  return TCL_OK;
}

/*
 *  Tol_ColumnData::AppendEmpty --
 *
 */

int Tol_ColumnData::AppendEmpty()
{
  if ( type == Real )
    AppendEmptyReal();
  else
    AppendText("");

  return TCL_OK;
}

/*
 *  Tol_ColumnData::PrependEmpty --
 *
 */

int Tol_ColumnData::PrependEmpty()
{
  
  if ( type == Real )
    PrependEmptyReal();
  else
    PrependText("");
  return TCL_OK;
}

/*
 *  Tol_Table implementation
 *
 */

/*
 * Tol_Table::Tol_Table --
 */

Tol_Table::Tol_Table(Tcl_Interp * _interp, char * name)
{
  rows = 0;
  interp = _interp;
  rowsNames = NULL;
  retBuffer = NULL;
  Tcl_DStringInit( &dsName );
  Tcl_ExternalToUtfDString( NULL, name, -1, &dsName );    
}

/*
 *  Tol_Table::~Tol_Table()
 *
 *    Destructor
 */

Tol_Table::~Tol_Table()
{
  int i;

  Tcl_DStringFree(&dsName);
  if (rowsNames) {
    int sizeRN;
    
    sizeRN = (headerType == Row) ? rows + 1: rows;
    for ( i = 0; i < sizeRN; i++ )
      Tcl_DecrRefCount( rowsNames[i] );
    Tcl_Free( (char*)rowsNames );
    rowsNames = NULL;
  }
  if ( retBuffer ) {
    Tcl_Free( (char*)retBuffer );
    retBuffer = NULL;
  }
  for ( i = 0; i < columns.GetSize(); i++ )
    delete *Columns(i);
}
  
/*
 *  Tol_Table::ReturnName --
 *
 *    implement: tableset info name
 */

int Tol_Table::ReturnName()
{
  Tcl_AppendResult( interp, Tcl_DStringValue(&dsName), NULL );
  return TCL_OK;
}

/*
 *  Tol_Table::ReturnHeaderInfo --
 *
 *    implement: tableset info header
 */

int Tol_Table::ReturnHeaderInfo()
{
  switch ( headerType ) {
    case Struct:
      Tcl_AppendResult( interp, "struct", NULL );
      break;
    case Row:
      char buffer[256];
      
      sprintf( buffer, "row %d", extraColumns );
      Tcl_AppendResult( interp, buffer, NULL );
      break;
    case Generic:      Tcl_AppendResult( interp, "generic", NULL );
      break;
    default:
      Tcl_AppendResult( interp, "FATAL: found invalid header type", NULL );
      return TCL_ERROR;
  }
  return TCL_OK;
}

/*
 *  Tol_Table::ReturnRowsNumber --
 *
 *    implement: tableset info rows
 */

int Tol_Table::ReturnRowsNumber()
{
  char buffer[1024];

  sprintf( buffer, "%d", rows );
  Tcl_AppendResult( interp, buffer, NULL );
  return TCL_OK;
}

/*
 *  Tol_Table::ReturnRowsNames --
 *
 *    Return the list with the name of the elements of the set.
 *
 */

int Tol_Table::ReturnRowsNames()
{
  int itemc;
  
  itemc = (headerType == Row) ? rows + 1 : rows;
  Tcl_SetObjResult(interp, Tcl_NewListObj(itemc, rowsNames));
  return TCL_OK;
}

/*
 *  Tol_Table::ReturnColumnsInfo --
 *
 *    implement: tableset info columns
 */

int Tol_Table::ReturnColumnsInfo()
{
  Tol_ColumnData * ptrCol;
  int itemc, i;
  Tcl_Obj ** itemv;
  
  itemc = columns.GetSize();
  itemv = (Tcl_Obj**)Tcl_Alloc( itemc * sizeof(Tcl_Obj*) ); 
  for ( i = 0; i < itemc; i++ ) {
    ptrCol = *Columns(i);
    itemv[i] = ptrCol->GetInfo();
  }
  Tcl_SetObjResult( interp, Tcl_NewListObj(itemc, itemv) );
  Tcl_Free( (char*)itemv );
  return TCL_OK;
}

/*
 *  Tol_Table::ReturnCell
 *
 *    implement: tableset data get i j
 *
 *    interp <-- set_data(row,col)
 */

int Tol_Table::ReturnCell( int row, int col )
{
  Tol_ColumnData * ptrCol;
  char buffer[1024];

  /* validate arguments */

  if ( row < 0 || row >= rows ) {
    sprintf( buffer, "invalid row index must be >= 0 && < %d", rows );
    Tcl_AppendResult( interp, buffer, NULL );    
    return TCL_ERROR;
  }
  if ( col < 0 || col >= columns.GetSize() ) {
    sprintf( buffer, "invalid column index must be >= 0 && < %d", columns.GetSize() );
    Tcl_AppendResult( interp, buffer, NULL );    
    return TCL_ERROR;
  }
  ptrCol = *Columns(col);
  return ptrCol->ReturnCell(interp, row);
}

/*
 *  Tol_Table::ReturnColumn
 *
 *    implement: tableset data column col
 *
 *      interp <-- set_data(col)->ReturnData(interp)
 */

int Tol_Table::ReturnColumn( int col )
{
  Tol_ColumnData * ptrCol;

  /* validate column */

  if ( col < 0 || col >= columns.GetSize() ) {
    char buffer[1024];

    sprintf( buffer, "invalid column index must be >= 0 && < %d", columns.GetSize() );
    Tcl_AppendResult( interp, buffer, NULL );    
    return TCL_ERROR;
  }
  ptrCol = *Columns(col);
  return ptrCol->ReturnData(interp);
}



/*
 *  Tol_Table::ApplyAll --
 *
 *    implement: tableset apply all script
 */

int Tol_Table::ApplyAll( Tcl_Obj * scriptObj )
{
  Tcl_Obj * args[3];
  Tcl_Obj ** items;
  Tol_ColumnData * ptrCol;
  int i, r, c, count;

  for ( i = 0; i < 2; i++ ) {
    args[i] = Tcl_NewObj();
    Tcl_IncrRefCount( args[i] );
  }

  /* Init script evaluator */

  EvalScript script(interp, scriptObj, 3, args);
  for ( c = 0; (script.Status()==TCL_OK) && (c<columns.GetSize()); c++ ) {
    /* Get the column */
    ptrCol = *Columns(c);

    /* Set the arg column value */

    if ( Tcl_IsShared( args[1] ) ) {
      Tcl_DecrRefCount( args[1] );
      args[1] = Tcl_NewIntObj( c );
      Tcl_IncrRefCount( args[1] );
    } else
      Tcl_SetIntObj( args[1], c );
    r = 0;
    /* get the array of elements */
    Tcl_ListObjGetElements(interp, ptrCol->GetValues(), &count, &items);
    for (int r = 0; (r < count) && (script.Status()==TCL_OK); r++) {
      /* set the arg row value */
      if (Tcl_IsShared(args[0])) {
        Tcl_DecrRefCount(args[0]);
        Tcl_IncrRefCount(args[0] =Tcl_NewIntObj(r));
      } else
        Tcl_SetIntObj(args[0], r);
      /* set the cell value */
      args[2] = items[r];
      
      /* invoke the script */
      
      script.Eval();
    }
  }

  for ( i = 0; i < 2; i++ )
    Tcl_DecrRefCount(args[i]);
  
  return script.Status();
}

/*
 *  Tol_Table::ApplyRow
 *
 *    implement: tableset apply row idx script
 */

int Tol_Table::ApplyRow( int row, Tcl_Obj * scriptObj )
{
  Tcl_Obj * args[2];
  Tol_ColumnData * ptrCol;
  int c;
  
  /* verify the row argument */
  
  if ( row < 0 || row >= rows ) {
    char buffer[1024];

    sprintf( buffer, "invalid row index must be >= 0 && < %d", rows );
    Tcl_AppendResult( interp, buffer, NULL );
    return TCL_ERROR;
  }

  Tcl_IncrRefCount(args[0] = Tcl_NewObj());

  /* Init script evaluator */

  EvalScript script(interp, scriptObj, 2, args);
  
  /* loop over columns */

  for (c = 0; (script.Status() != TCL_ERROR) && (c < columns.GetSize()); c++ ) {

    /* Get the column */

    ptrCol = *Columns(c);

    /* Set the arg column value */
    
    if ( Tcl_IsShared( args[0] ) ) {
      Tcl_DecrRefCount( args[0] );
      args[0] = Tcl_NewIntObj( c );
      Tcl_IncrRefCount( args[0] );
    } else
      Tcl_SetIntObj( args[0], c );
    
    /* set the cell value */
    if (!(args[1] = ptrCol->GetCell(interp, row))) {
      /* there was an error getting the cell value */
      Tcl_DecrRefCount(args[0]);
      return TCL_ERROR;
    }
    /* Invoke the script */
    
    script.Eval();
  }
  Tcl_DecrRefCount(args[0]);
  return script.Status();
}

/*
 *  Tol_Table::ApplyColumn
 *
 *    implement: tableset apply column idx script
 */

int Tol_Table::ApplyColumn( int col, Tcl_Obj * script )
{
  Tol_ColumnData * ptrCol;

  /* validate the column argument */

  
  if ( col < 0 || col >= columns.GetSize() ) {
    char buffer[1024];

    sprintf( buffer, "invalid column index must be >= 0 && < %d", columns.GetSize() );
    Tcl_AppendResult( interp, buffer, NULL );
    return TCL_ERROR;
  }

  ptrCol = *Columns(col);
  
  return ptrCol->Apply(interp, script);
}

/*
 *  Tol_Table::ObjCmd --
 *
 *    implement an specific group of set given by clientData.
 *
 *    table info (name | header | rows | columns)
 *
 *    table data
 *
 *      table data get i j --> return element in row i column j
 *
 *      table data apply
 *
 *         table data apply all script  -->
 *
 *                            executes script foreach item in the table
 *                            concats to script 'i j value' (row, col and value)
 *
 *         table data apply row idx script -->
 *
 *                            executes script foreach item in row idx
 *                            concats to script 'i value' (column value)
 *
 *         table data iter column idx script -->
 *
 *                            executes script foreach item in column idx
 *                            concats to script 'i value' (row value)
 *
 *     table data column idx --> return data in column idx. If column's type
 *                             isn't real returns a list with all values. If
 *                             column's type is real returns a list with blt
 *                             vectors and probabily unknown values, ej,
 *                             {Y1 Y2 {} {} Y3 *INF}
 */


int Tol_Table::ObjCmd (ClientData clientData,
                       Tcl_Interp *interp,
                       int objc, Tcl_Obj *CONST objv[])
{
  int tcl_code;
  int i, j;
  
  char * cmd = Tcl_GetString(objv[0]);

#ifdef __RELEASE__
  try_crash {
#endif
    Tol_Table * sgrp = (Tol_Table*) clientData;
  
    if ( objc < 2 ) {
      Tcl_AppendResult( interp,
                        "wrong # args: should be \"",
                        cmd, " option\"", NULL );
      return TCL_ERROR;
    }
  
    char * option = Tcl_GetString(objv[1]);
    int length = strlen(option);
  
    if (length && !strncmp( "info", option, length )) {
      if ( objc != 3 ) {
        Tcl_AppendResult( interp,
                          "wrong # args: should be \"",
                          cmd, " info option\"", NULL );
        return TCL_ERROR;
      }
      option = Tcl_GetString( objv[2] );
      length = strlen( option );
      if ( length && !strncmp( "name", option, length ) )
        return sgrp->ReturnName();
      else if ( length && !strncmp( "header", option, length ) )
        return sgrp->ReturnHeaderInfo();
      else if ( length && !strncmp( "datarows", option, length ) )
        return sgrp->ReturnRowsNumber();
      else if ( length && !strncmp( "rowsnames", option, length ) )
        return sgrp->ReturnRowsNames();
      else if ( length && !strncmp( "columns", option, length ) )
        return sgrp->ReturnColumnsInfo();
      else {
        Tcl_AppendResult( interp,
                          "invalid  arg '", option, " should be \"",
                          " name, header, datarows, rowsnames or columns\"", NULL );
        return TCL_ERROR;
      } 
      //return sgrp->ReturnInfo();
    } else if ( length && !strncmp("data", option, length) ) {
      if ( objc < 3 || objc > 6 ) {
        Tcl_AppendResult( interp,
                          "wrong # args: should be: \n",
                          cmd, " data get i j\n",
                          cmd, " data apply all script\n",
                          cmd, " data apply row idx script\n",
                          cmd, " data apply column idx script\n",
                          cmd, " data column idx", NULL);
        return TCL_ERROR;
      }
      char * arg = Tcl_GetString(objv[2]);
    
      if ( !strcmp( "column", arg) ) {
        if (objc != 4) {
          Tcl_AppendResult( interp,
                            "wrong # args: should be \"",
                            cmd, " data column idx\"", NULL );
          return TCL_ERROR;
        }
        tcl_code = Tcl_GetIntFromObj( interp, objv[3], &i );
        if ( tcl_code != TCL_OK )
          return tcl_code;
        return sgrp->ReturnColumn( i );
      } else if ( !strcmp("get", arg) ) {
        if ( objc != 5 ) {
          Tcl_AppendResult( interp,
                            "wrong # args: should be \"",
                            cmd, " data get i j\"", NULL );
          return TCL_ERROR;
        }
        tcl_code = Tcl_GetIntFromObj( interp, objv[3], &i );
        if ( tcl_code != TCL_OK )
          return tcl_code;
        tcl_code = Tcl_GetIntFromObj( interp, objv[4], &j );
        if ( tcl_code != TCL_OK )
          return tcl_code;
        sgrp->ReturnCell( i, j );
      } else if ( !strcmp("apply", arg) ) {
        if (objc < 4) {
          Tcl_AppendResult( interp,
                            "wrong # args: should be: ",
                            cmd, " data apply all script\n",
                            cmd, " data apply row idx script\n",
                            cmd, " data apply column idx script",NULL);
          return TCL_ERROR;
        }
        arg = Tcl_GetString( objv[3] );
        if ( !strcmp("all", arg) ) {
          if ( objc != 5 ) {
            Tcl_AppendResult( interp,
                              "wrong # args: should be \"",
                              cmd,
                              " data apply all script\"", NULL );
            return TCL_ERROR;
          }
          return sgrp->ApplyAll( objv[4] );
          //OK de data apply all script
        } else if (!strcmp("row", arg) ) {
          if (objc != 6) {
            Tcl_AppendResult( interp,
                              " wrong # args: should be \"",
                              cmd,
                              " data apply row idx script\"", NULL );
            return TCL_ERROR;
          }
          tcl_code = Tcl_GetIntFromObj( interp, objv[4], &i );
          if ( tcl_code != TCL_OK )
            return tcl_code;
          return sgrp->ApplyRow( i, objv[5] );
        } else if ( !strcmp("column", arg) ) {
          if ( objc != 6 ) {
            Tcl_AppendResult( interp,
                              "wrong # args: should be \"",
                              cmd,
                              " data apply column idx script\"", NULL );
            return TCL_ERROR;
          }
          tcl_code = Tcl_GetIntFromObj( interp, objv[4], &i );
          if ( tcl_code != TCL_OK )
            return tcl_code;
          return sgrp->ApplyColumn( i, objv[5] );
        } else {
          Tcl_AppendResult( interp,
                            "wrong # args: should be: ",
                            cmd, " data apply all script\n",
                            cmd, " data apply row idx script\n",
                            cmd, " data apply column idx script",NULL);
          return TCL_ERROR;
        }
      } else {
        Tcl_AppendResult( interp,
                          "wrong # args: should be: \n",
                          cmd, " data get i j\n",
                          cmd, " data apply all script\n",
                          cmd, " data apply row idx script\n",
                          cmd, " data apply column idx script\n",
                          cmd, " data column idx", NULL);
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendResult( interp, "bad option \"",option,
			"\": must be info or data",NULL);
      return TCL_ERROR;
    }
#ifdef __RELEASE__
  } catch_crash {
    Tcl_AppendResult( interp,
		      "Fatal error in tableset subcommand, kill the application",
		      NULL);
    return TCL_ERROR;
  }
#endif
}

void Tol_Table::DeleteCmd( ClientData clientData )
{
  Tol_Table * tsg = (Tol_Table*)clientData;
  
  delete tsg;
}

/*
 * Tol_TableSet implementation
 */

/*
 *  Tol_TableSet
 *
 *     build the matrix data, header and create the ADT
 *     in case of success. CmdStatus tell us if the ADT
 *     was created.
 */

Tol_TableSet::Tol_TableSet(Tcl_Interp * _interp, char * setname, BSet * set,
                           Tcl_Obj * obj_result)
  : Tol_Table(_interp, setname)
{
  cmd_status = Build(set, obj_result);
}

/*
 *  Tol_TableSet::Build --
 *
 *    Build the data structure associated to a SetGroup
 */

int Tol_TableSet::Build( BSet * set, Tcl_Obj * obj_result )
{
  int card, subcard, i, j, k;
  int LastColWithData;
  BSyntaxObject * subso, * elem;
  BSet * subset;
  BStruct * struct0, * struct1;
  BText so_name;
  int tcl_status;
  
  Tol_ColumnData * ptrCol;
  ToltclPool firstRow;
  
  card = set->Card();
  if ( !card ) {
    Tcl_AppendStringsToObj( obj_result,
			    "could not create a table from an empty set", NULL );
    return TCL_ERROR;
  }
  
  /* rows may be equal to:
   *
   *    card if headerType == generic || headerType == struct || card == 1
   *
   *    card - 1 if headerType = row
   */
  

  rowsNames = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*) * card );

  rows = 0;
  
  /* process first row */
  
  subso = (*set)[1];
  so_name = subso->Name();
  if ( !so_name.HasName() )
    so_name = subso->Description();
  Tcl_IncrRefCount(rowsNames[0] = NewExternalStringObj(so_name.Buffer()));
  struct0 = NULL;
  headerType = Generic;
  extraColumns = 0;

  /* at least one column, i guess it will be Real */
  
  columns.AppendObject(ptrCol = new Tol_ColumnData( Tol_ColumnData::Real));

  if ( subso->Grammar() == GraSet() ) {
    /* first element is a subset */
    BText * btxt;
 
    subset =  &(((BUserSet*)subso)->Contens());
    subcard = subset->Card();
    if ( subcard ) {
      struct0 = subset->Struct();
      /* loop for text labels */
      for ( i = 1; i <= subcard; i++ ) {
        elem = (*subset)[i];
        if ( elem->Grammar() != GraText() ||!elem->Info().Length() ) {
          /* No Text: invalid column name or empty text*/
          break;
        }
	/* at least we need this column */
	btxt = &(((BUserText*)elem)->Contens());
	if (i>1) {
	  /* the first column was previously added
	   * and ptrCol point to it */
	  ptrCol = new Tol_ColumnData(Tol_ColumnData::Real);
	  columns.AppendObject(ptrCol);
	}
	ptrCol->SetName(btxt->Buffer());	
      }
      if ( i > subcard ) {
        /* could be Row type */
        headerType = Row;
        /* but only at the end could be verified */
      }
    } else {
      /* first subset is empty, so add empty cell to the first column */
      tcl_status = (*Columns(0))->AppendEmpty();
      rows = 1;
#ifdef _DEBUG_TABLE_
      if ( tcl_status != TCL_OK ) {
	Tcl_AppendStringsToObj( obj_result,
				"internal error", NULL );
        return TCL_ERROR;
      }
#endif
    }
  } else {
    /* first element is not a subset, add it as the first element of
     * the first column the header will be generic */
    (*Columns(0))->AppendSyntaxObject(interp, subso);
    rows = 1;
  }
  
  /* now going into the data, uff!!! */

  for ( i = 2; i <= card; i++, rows++ ) {
    subso = (*set)[i];
    so_name = subso->Name();

    /* set the name of this row */
    if ( !so_name.HasName() )
      so_name = subso->Description();
    Tcl_IncrRefCount(rowsNames[i-1] = NewExternalStringObj(so_name.Buffer()));

    if ( subso->Grammar() == GraSet() ) {
      /* current element is a subset */
      subset =  &(((BUserSet*)subso)->Contens());
      subcard = subset->Card();
      if ( subcard ) {
        /* current subset has elements */
        struct1 = subset->Struct();
        if ( struct0 && struct1 != struct0 ) {
          /* found mismatched struct, so forget it */
          struct0 = NULL;
        }
        for ( j = 1; j <= subcard; j++ ) {
          elem = (*subset)[j];
          if ( j > columns.GetSize() ) {
            /* create a new column, always suppose it will be Real */
            ptrCol = new Tol_ColumnData( Tol_ColumnData::Real );
            columns.AppendObject( ptrCol );
            /* insert empty values at begining */
            for ( k = 0; k < rows ; k++ )
              ptrCol->AppendEmpty();
          } else
            ptrCol = *Columns(j-1);
          ptrCol->AppendSyntaxObject(interp, elem);
        }
        LastColWithData = subcard - 1;
      } else {
        /* current subset has no elements, so prepare for an empty row */
        LastColWithData = -1;
      }
    } else {
      /* current element is not a subset, add it to the first column */
      (*Columns(0))->AppendSyntaxObject(interp, subso);
      LastColWithData = 0;
    }
    /* add empty to the rest of columns */
    for ( j = LastColWithData + 1; j <  columns.GetSize(); j++ ) {
      (*Columns(j))->AppendEmpty();
    }
  }
  
  /* NOW DECIDE WICH ARE THE NAMES OF THE COLUMNS */
  
  char buffer[1024];
  int idx;
  
  if ( struct0 ) {
    /* names are taken from struct */
    BText btxt;
    
    subso = (*set)[1];
    subset =  &(((BUserSet*)subso)->Contens());
    
    /* here we can ensure that, only if rows > 0
     *
     *     subset->Card() == columns.GetSize() == struct0->Size()
     */
    
    /* this loop is only for the case rows == 0 */

    for (i=columns.GetSize(); i<subset->Card(); i++) {
      /* create a new column, always suppose it will be Real */
      ptrCol = new Tol_ColumnData( Tol_ColumnData::Real );
      columns.AppendObject(ptrCol);
    }
      
    for ( i = 0; i < subset->Card(); i++ ) {
      btxt = (*struct0)[i].Name(); 
      (*Columns(i))->SetName(btxt.Buffer());
      
      /* Insert (prepend) at position 0 (in every column)
       * the corresponding element from firstRow
       */
      
      (*Columns(i))->PrependSyntaxObject( interp, (*subset)[i+1] );
    }
    /* so, the header type is Struct */
    headerType = Struct;
    /* and we have a one more row */
    ++rows;
  } else if (headerType == Row) {

    /* set[1] is a subset an all its members are text 
     * and was filled when headerType was determined
     */

    subset =  &(((BUserSet*)(*set)[1])->Contens());
    subcard = subset->Card();
    if (!rows) {
      /* no data so insert the first row as data */
      headerType = Generic;
      rows = 1;
      for ( i = 0; i < subcard; i++ ) {
	subso = (*subset)[i+1];
	(*Columns(i))->AppendSyntaxObject(interp, subso);
      }
      /* this will force generic names for the columns */
      i = 0;
    } else
      i = subcard;

    /* complete the extra names as columnXXX */

    for ( ;i < columns.GetSize(); i++ ) {
      sprintf( buffer, "Column%d", i+1 );
      (*Columns(i))->SetName( buffer );
      ++extraColumns;
    }
  } else {
    /* we must provide generic names for the columns
     * and prepend the first row
     */

    subso = (*set)[1];
    if ( subso->Grammar() == GraSet() ) {
      subset =  &(((BUserSet*)subso)->Contens());
      subcard = subset->Card();
      if ( subcard ) {
	/* and we'll have a one more row */
	++rows;
      }
    } else {
      /* this cell was already added at the begining */
      subset = NULL;
      subcard = 1;
    }
    const char * ptrName;
    for ( i = 0; i < columns.GetSize(); i++ ) {
      idx = i+1;
      ptrCol = *Columns(i);
      if ( subset && subcard ) {
        if ( idx <= subcard ) {
          elem = (*subset)[idx];            
          ptrCol->PrependSyntaxObject( interp, elem );
          if (elem->Name().HasName())
            ptrName = elem->Name().String();
          else {
            sprintf( buffer, "Column%d", idx );
            ptrName = buffer;
          }
        } else {
          /* the first row is too short */
          ptrCol->PrependEmpty();
          sprintf( buffer, "Column%d", idx );
          ptrName = buffer;
        }
      } else {
        sprintf( buffer, "Column%d", idx );
        ptrName = buffer;
      }
      ptrCol->SetName(ptrName);      
    }
    /* we may grow the columns, here we have a valid subset */
    
    for ( ; i < subcard; i++ ) {
      idx = i+1;
      elem = (*subset)[idx];            
      columns.AppendObject(ptrCol = new Tol_ColumnData(Tol_ColumnData::Real));
      ptrCol->AppendSyntaxObject(interp,elem);
      if (elem->Name().HasName())
        ptrName = elem->Name().String();
      else {
        sprintf( buffer, "Column%d", idx );
        ptrName = buffer;
      }
      ptrCol->SetName( ptrName );
      /* we have to fill the rest of rows with empty */
      for ( j = 1;  j < rows; j++ )
	ptrCol->AppendEmpty();
    }
  }
  
  return TCL_OK;
}

/*
 *  Tol_TableMatrix implementation.
 */

/*
 *  Tol_TableMatrix
 *
 *     build the matrix data, header and create the ADT
 *     in case of success. CmdStatus tell us if the ADT
 *     was created.
 */

Tol_TableMatrix::Tol_TableMatrix(Tcl_Interp * _interp, char * matname, BMatrix<double> *mat,
                                 Tcl_Obj * obj_result)
  : Tol_Table(_interp, matname)
{
  cmd_status = Build(mat, obj_result);
}

/*
 *  Tol_TableMatrix::Build --
 *
 *    Build the actual ADT from a BMat object. Return success state.
 */

int Tol_TableMatrix::Build(BMatrix<double> *mat, Tcl_Obj * obj_result)
{
  Tol_ColumnData * ptrCol;
  int colSize =  mat->Columns();
  char buffer[255];

  rows = mat->Rows();
  rowsNames = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*) * rows);
  extraColumns = 0;
  headerType = Generic;
  for (int col = 0; col < colSize; col++) {
    columns.AppendObject(ptrCol = new Tol_ColumnData( Tol_ColumnData::Real));
    sprintf(buffer, "column %d", col);
    ptrCol->SetName(buffer);
    for (int row = 0; row < rows; row++) {
      if (!col) {
        /* put the name to the row */
        sprintf(buffer, "row %d", row);
        Tcl_IncrRefCount(rowsNames[row]=Tcl_NewStringObj(buffer, -1));
      }
      BDat dat((*mat)(row,col));
      ptrCol->AppendBDat(interp, dat);
    }
  }
  return TCL_OK;
}

/*
 *  Tol_CreateTable --
 *
 */

int Tol_CreateTable(Tcl_Interp * interp,
                    Tcl_Obj *tblcmd, Tcl_Obj *objref,
                    int ismat, Tcl_Obj * obj_result)
{
  Tcl_CmdInfo cmd_info;
  char * tblname;

  const BSyntaxObject * syn;

  tblname = Tcl_GetString(tblcmd);
  if (!*tblname) {
    Tcl_AppendStringsToObj(obj_result,
                           "invalid table group name \"\"",NULL);
    return TCL_ERROR;
  }

  /* verify if sgrp_name already exists as a table group */

  if (Tcl_GetCommandInfo(interp, tblname, &cmd_info)) {
    if (cmd_info.objProc == Tol_Table::ObjCmd)
      Tcl_AppendStringsToObj(obj_result, "\"", tblname, "\"",
			     " already exists as a table object", NULL);
    else
      Tcl_AppendStringsToObj(obj_result, "\"", tblname, "\"",
			     " already exists as a command", NULL);
      
    return TCL_ERROR;
  }

  // syn = Tol_GetObjFromReference( interp, GraSet(), setref, obj_result );
  if (!(syn=Tol_ResolveObject(interp, objref, obj_result))) {
    return TCL_ERROR;
  }

  /* now is ok to create the obj cmd */

  BText objname( syn->Name() );
  Tol_Table * ptrTable;
  if (ismat) {
    BMatrix<double> vmat;
    BMatrix<double> &theMat = vmat;
    if (syn->Grammar()==GraVMatrix()) {
      BVMat &V = VMat((BSyntaxObject *)syn);
      V.GetDMat(vmat);
    } else if (syn->Grammar()==GraMatrix()) {
      theMat = ((DMat&)Mat((BSyntaxObject *)syn));
    } else {  
      Tcl_AppendStringsToObj( obj_result, Tcl_GetString(objref),
                              " is not a valid Matrix|VMatrix object", NULL );    
      return TCL_ERROR;
    }
    
    ptrTable = new Tol_TableMatrix( interp, objname.Buffer(),
                                    &theMat, obj_result );
  } else if ( syn->Grammar()==GraSet() ) {
    
    BSet *ptrSet = &(Set((BSyntaxObject*)syn));
    ptrTable = new Tol_TableSet( interp, objname.Buffer(),
                                 ptrSet, obj_result );
  } else {  
    Tcl_AppendStringsToObj( obj_result, Tcl_GetString(objref),
                            " is not a valid Set object", NULL );    
    return TCL_ERROR;
  }
  if ( ptrTable->CmdStatus() != TCL_OK ) {
    delete ptrTable;
    return TCL_ERROR;
  }
  ptrTable->CreateCommand(tblname);
  
  return TCL_OK;
}

/*
 * Destroy a list of table cmd.
 *
 */

int Tol_DestroyTable(Tcl_Interp * interp,
                     int objc, Tcl_Obj *CONST objv[],
                     Tcl_Obj * obj_result)
{
  Tcl_CmdInfo cmd_info;
  char * tbl_name;
  
  for (int i=0; i<objc; ++i) {
    tbl_name = Tcl_GetString( objv[i] );
    if ( Tcl_GetCommandInfo( interp, tbl_name, &cmd_info ) &&
         (cmd_info.objProc == Tol_Table::ObjCmd) ) {
      if ( Tcl_DeleteCommand( interp, tbl_name ) ) {
        Tcl_AppendStringsToObj( obj_result,
                                "could not delete \"", tbl_name, "\"", NULL);
        return TCL_ERROR;
      }
    } else {
      Tcl_AppendStringsToObj( obj_result, 
                              "can't find table \"", tbl_name, "\"", NULL );
      return TCL_ERROR;
    }
  }
  Tcl_ResetResult(interp);
  return TCL_OK;
}
