/* tolExcel.cpp: API for read and write excel files.

   Based on ExcelFormat from  http://shell.franken.de/svn/sky/excel/trunk/ExcelFormat

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

#include "ExcelFormat.h"
using namespace ExcelFormat;

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_tree.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bsetgra.h>

class TolExcel {
public:
  TolExcel( );
  TolExcel( char *path );
  ~TolExcel();

  bool IsValid()
  {
    return m_ptrXLS != NULL;
  }
  
  size_t NumberOfWorkSheets()
  {
    return m_ptrXLS ? m_ptrXLS->GetTotalWorkSheets() : 0;
  }
  
  bool HasActiveWS()
  {
    return m_ptrActiveWS != NULL;
  }

  bool SetActiveWS( size_t num )
  {
    m_ptrActiveWS = num < m_ptrXLS->GetTotalWorkSheets()
      ? m_ptrXLS->GetWorksheet( num ) : NULL;
    return m_ptrActiveWS != NULL;
  }

  bool SetActiveWS( const char * sheetName )
  {
    m_ptrActiveWS = m_ptrXLS->GetWorksheet( sheetName );
    return m_ptrActiveWS != NULL;
  }
  
  const char *GetActiveWSName()
  {
    return m_ptrActiveWS ? m_ptrActiveWS->GetAnsiSheetName( ) : NULL;
  }
  
  BSyntaxObject *GetCellAnything( const BText &err_name,
                                  size_t row, size_t col );

  bool GetCellReal( const BText &err_name,
                    size_t row, size_t col, BDat &result )
  {
    BasicExcelCell * cell = GetCell( err_name, row, col );

    return cell ? GetCellReal( cell, err_name, row, col, result ) : false;
  }

  bool GetCellReal( BasicExcelCell *cell, const BText &err_name,
                    size_t row, size_t col, BDat &result )
  {
    assert( cell != NULL );
    
    char cell_coord[ 64 ];
    double d;

    if ( cell->Get( d ) ) {
      result = BDat( d );
      return true;
    }
    snprintf( cell_coord, 64, "(%d,%d)", row, col );
    Warning( err_name + ": " +
             I2("the cell ", "la celda " ) + cell_coord +
             I2(" does not contain a Real", " no contiene un Real" ) );
    result = BDat::Unknown( );
    return false;
  }
  
  bool GetCellText( const BText &err_name,
                    size_t row, size_t col, BText &result )
  {
    BasicExcelCell * cell = GetCell( err_name, row, col );

    return  cell ? GetCellText( cell, err_name, row, col, result ) : false;
  }
  
  bool GetCellText( BasicExcelCell *cell, const BText &err_name,
                    size_t row, size_t col, BText &result )
  {
    assert( cell != NULL );

    char cell_coord[ 64 ];
    const char *str;
    
    if ( ( str = cell->GetString( ) ) ) {
      result = str;
      return true;
    }
    snprintf( cell_coord, 64, "(%d,%d)", row, col );
    Warning( err_name + ": " +
             I2("the cell ", "la celda " ) + cell_coord +
             I2(" does not contain a Text", " no contiene un Text" ) );
    result = "";
    return false;
  }
  
  bool GetCellDate( const BText &err_name,
                    size_t row, size_t col, BDate &result )
  {
    BasicExcelCell * cell = GetCell( err_name, row, col );

    return cell ? GetCellDate( cell, err_name, row, col, result ) : false;
  }

  bool GetCellDate( BasicExcelCell *cell, const BText &err_name,
                    size_t row, size_t col, BDate &result )
  {
    assert( cell != NULL );

    char cell_coord[ 64 ];
    double d;

    if ( cell->Get( d ) ) {
      int D, M, Y, H, Mi, S;
      
      double t = ExcelSerialDateToDMY( d, D, M, Y );
      ExcelSerialTimeToHMS( t, H, Mi, S );

      result = BDate( Y, M, D, H, Mi, S );
      return true;
    }
    snprintf( cell_coord, 64, "(%d,%d)", row, col );
    Warning( err_name + ": " +
             I2("the cell ", "la celda " ) + cell_coord +
             I2(" does not contain a date/time",
                " no contiene una fecha/hora" ) );
    result = BDate::Unknown( );
    return false;
  }
  
  static double code_addr( TolExcel* ptr )
  {
    double addr = 0.0;
    *((TolExcel**)(&addr)) = ptr;
    return addr;
  }
  
  static TolExcel* decode_addr( double addr )
  {
    return *((TolExcel**)(&addr));
  }

  static double ExcelSerialDateToDMY( double SerialDate, int &nDay, 
                                      int &nMonth, int &nYear);
  static double ExcelSerialTimeToHMS( double SerialTime,
                                      int &nHour, int &nMinute, int &nSecond )
  {
    nHour = int( floor( SerialTime *24 ) );
    double tH = SerialTime - nHour;
    nMinute = int( floor( tH * 60 ) );
    double tM = tH - nMinute;
    nSecond = int( floor( tM * 60 ) );
    return tM - nSecond;
  }

  static int DMYToExcelSerialDate( int nDay, int nMonth, int nYear );

  static double HMSToExcelSerialTime( int nHour, int nMinute, int nSecond )
  {
    const double nHiD = 24.0;
    const double nMiD = 24.0 * 60;
    const double nSiD = 24.0 * 60 * 60;
    
    return nHour / nHiD + nMinute / nMiD + nSecond / nSiD;
  }

  static size_t decodeColumn( char * cellName, size_t length );
  static bool decodeCoordinates( char * cellName,
                                 size_t &row, size_t &column );
  static bool getCellCoordinates( const BText & name,
                                  BSyntaxObject *cell,
                                  size_t &row, size_t &column );

protected:

  BasicExcelCell *GetCell( const BText &err_name,
                           BasicExcelWorksheet * ptrWS,
                           size_t row, size_t col );

  BasicExcelCell *GetCell( const BText &err_name, size_t row, size_t col )
  {
    if ( !HasActiveWS( ) ) {
      Error( err_name + ": " +
             I2("there is no active work sheet",
                "no hay hoja de trabajo activa") );
      return NULL;
    }
    return GetCell( err_name, m_ptrActiveWS, row, col );
  }

  BasicExcelCell *GetCell( const BText &err_name,
                           size_t iWS, size_t row, size_t col )
  {
    char strNumWS[ 16 ];
    
    BasicExcelWorksheet* ptrWS = m_ptrXLS->GetWorksheet( iWS );
    if ( !ptrWS ) {
      snprintf( strNumWS, 16, "%d", iWS );
      Error( err_name + ": " +
             I2("worksheet with index ",
                "la hoja de trabajo con indice ") + strNumWS +
             I2( " was not found", " no existe" ) );
      return NULL;
    }
    return GetCell( err_name, ptrWS, row, col );    
  }
  
private:
  bool m_isOpen;
  BasicExcel *m_ptrXLS;
  XLSFormatManager *m_ptrFmtMgr;
  BasicExcelWorksheet *m_ptrActiveWS;
};

size_t TolExcel::decodeColumn( char * cellName, size_t length )
{
  if ( !length ) return 0;
  char c = cellName[ --length ];
  if ( c < 'A' || c > 'Z')
    return 0;
  return (c - 'A' + 1) + TolExcel::decodeColumn( cellName, length ) * 26;
}

bool TolExcel::decodeCoordinates( char * cellName, size_t &row, size_t &column )
{
  size_t i = 0;

  // decode column A-based
  char ch = cellName[ i ];
  while ( ch >= 'A' && ch <= 'Z' ) { ch = cellName[ ++i ]; }
  column = TolExcel::decodeColumn( cellName, i );
  if ( !column ) {
    return false;
  }
  // decode row 1-based
  row = 0;
  while ( ch >= '1' && ch <= '9' ) {
    row = row * 10 + ch - '0';
    ch = cellName[ ++i ];
  }
  if ( !row ) {
    return false;
  }
  // cell coordinates are 0-based internally
  --row;
  --column;
  return true;
}

bool TolExcel::getCellCoordinates( const BText & name, BSyntaxObject *cell,
                                   size_t &row, size_t &column )
{
  bool status;
  
  if ( cell->Grammar() == GraText() ) {
    BText & cellName = Text( cell );
    
    status = TolExcel::decodeCoordinates( cellName.Buffer(),
                                               row, column );
    if ( !status ) {
      Error( name + ": " +
             I2( "invalid cell coordinates ",
                 "coordenadas de celda invalidas " ) + cellName );
    }
  } else if ( cell->Grammar() == GraSet() ) {
    BSet &_cell = Set( cell );
    if ( _cell.Card() != 2 ) {
      Error( name + ": " +
             I2( "invalid cell coordinates, must be a Set of two numbers ",
                " coordenadas de celda invalidas, debe ser un conjunto de 2 "
                 "numeros" ) );
      status = false;
    } else {
      BDat &datRow = Dat( _cell[1] );
      BDat &datCol = Dat( _cell[2] );
      if ( !datRow.IsKnown() || datRow.Value() < 1 ||
           !datCol.IsKnown() || datCol.Value() < 1 ) {
        char buffer[256];
        snprintf( buffer, 256, "(%f,%f)", datRow.Value(), datCol.Value() );
        Error( name + ": " +
               I2( "invalid cell coordinates ",
                   "coordenadas de celda invalidas " ) + buffer );
        status = false;
      }
      // Cell coordinates are 0-based internally
      row = size_t( datRow.Value( ) - 1 );
      column = size_t( datCol.Value( ) - 1 );
      status = true;
    }
  } else {
    Error( name + ": " +
           I2( "invalid cell type, must be Text or Set ",
               "referencia de celda invalida debe ser de tipo Text o Set " ));
    status = false;
  }
  return status;
}

TolExcel::TolExcel( )
: m_isOpen(false),
  m_ptrXLS(NULL),
  m_ptrFmtMgr(NULL),
  m_ptrActiveWS(NULL)
{
  m_ptrXLS = new BasicExcel;
  m_ptrActiveWS = NULL;
  m_isOpen = false;
}

TolExcel::TolExcel( char *path )
: m_isOpen(false),
  m_ptrXLS(NULL),
  m_ptrFmtMgr(NULL),
  m_ptrActiveWS(NULL)
{
  m_ptrXLS = new BasicExcel;
  if ( !( m_isOpen = m_ptrXLS->Load( path ) ) ) {
    delete m_ptrXLS;
    m_ptrXLS = NULL;
  } else {
  }
  if ( m_ptrXLS ) {
    m_ptrFmtMgr = new XLSFormatManager( *m_ptrXLS );
  }
  m_ptrActiveWS = NULL;
}

TolExcel::~TolExcel()
{
  if ( m_ptrFmtMgr ) {
    delete m_ptrFmtMgr;
    m_ptrFmtMgr = NULL;
  }
  if ( m_ptrXLS ) {
    delete m_ptrXLS;
    m_ptrXLS = NULL;
  }
  m_isOpen = false;
  m_ptrXLS = NULL;
}

BSyntaxObject *TolExcel::GetCellAnything( const BText &err_name,
                                          size_t row, size_t col )
{
  BSyntaxObject *result = NULL;
  char cell_coord[ 64 ];
  
  BasicExcelCell *cell = GetCell( err_name, row, col  );
  assert( !cell || ( cell && cell->Type( ) != BasicExcelCell::UNDEFINED ) );
  if ( cell ) {
    CellFormat fmt( *m_ptrFmtMgr, cell );
    switch ( cell->Type() ) {
    case BasicExcelCell::INT:
    case BasicExcelCell::DOUBLE:
      /* habria que inferir el tipo Date a partir del formato */
      result = new BContensDat( cell->GetDouble() );
      break;
    case BasicExcelCell::STRING:
      result = new BContensText( cell->GetString() );
      break;
    case BasicExcelCell::FORMULA:
      double d;
      const char * str;
      if ( cell->Get( d ) ) {
        /* habria que inferir el tipo Date a partir del formato */
        result = new BContensDat( d );
      } else if( ( str = cell->GetString() ) ) {
        result = new BContensText( str );
      } else {
        /* no se como interpretar el resultado de la formula */
        snprintf( cell_coord, 64, "(%d,%d)", row, col );        
        Warning( err_name + ": " +
                 I2("don't know how to interpret the result in cell ",
                    "no se como interpretar el resultado en la celda " ) + cell_coord );
        result = new BContensText( "" );
      }
      break;
    default:
      snprintf( cell_coord, 64, "(%d,%d)", row, col );        
      Warning( err_name + ": " +
               I2("don't know what to do with cell ",
                  "no se que hacer con la celda " ) + cell_coord );
      result = new BContensText( "" );
    }
  }
  return result;
}

BasicExcelCell *TolExcel::GetCell( const BText &err_name,
                                   BasicExcelWorksheet * ptrWS,
                                   size_t row, size_t col )
{
  assert( ptrWS != NULL );
  
  char cell_coord[ 64 ];
  
  BasicExcelCell *cell = ptrWS->Cell( row, col );
  if ( !cell ) {
    snprintf( cell_coord, 64, "(%d,%d)", row, col );
    Warning( err_name + ": " +
             I2("the cell ",  "la celda " ) + cell_coord +
             I2(" does not exists", " no existe") );
  } else if ( cell->Type() == BasicExcelCell::UNDEFINED ) {
    snprintf( cell_coord, 64, "(%d,%d)", row, col );
    Warning( err_name + ": " +
             I2("the cell ", "la celda " ) + cell_coord +
             I2(" is undefined", " no esta definida" ) );
    cell = NULL;
  }
  return cell;
}

double TolExcel::ExcelSerialDateToDMY( double SerialDate, int &nDay, 
                                       int &nMonth, int &nYear)
{
  // extract and remove time information
  
  int nSerialDate = int( floor( SerialDate ) );
  double SerialTime = SerialDate - nSerialDate;
  
  // Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a leap
  // year, but Excel/Lotus 123 think it is...
  
  if (nSerialDate == 60) {
    nDay   = 29;
    nMonth = 2;
    nYear  = 1900;
    
    return SerialTime;
  }
  else if (nSerialDate < 60) {
    // Because of the 29-02-1900 bug, any serial date under 60 is one
    // off... Compensate.
    
    nSerialDate++;
  }
  
  // Modified Julian to DMY calculation with an addition of 2415019
  
  int l = nSerialDate + 68569 + 2415019;
  int n = int(( 4 * l ) / 146097);
  l = l - int(( 146097 * n + 3 ) / 4);
  int i = int(( 4000 * ( l + 1 ) ) / 1461001);
  l = l - int(( 1461 * i ) / 4) + 31;
  int j = int(( 80 * l ) / 2447);
  nDay = l - int(( 2447 * j ) / 80);
  l = int(j / 11);
  nMonth = j + 2 - ( 12 * l );
  nYear = 100 * ( n - 49 ) + i + l;
  
  return SerialTime;
}

int TolExcel::DMYToExcelSerialDate( int nDay, int nMonth, int nYear )
{
  // Excel/Lotus 123 have a bug with 29-02-1900. 1900 is not a leap
  // year, but Excel/Lotus 123 think it is...
  
  if (nDay == 29 && nMonth == 02 && nYear==1900)
    return 60;
  
  // DMY to Modified Julian calculatie with an extra substraction of
  // 2415019.
  
  long nSerialDate = 
    int(( 1461 * ( nYear + 4800 + int(( nMonth - 14 ) / 12) ) ) / 4) +
    int(( 367 * ( nMonth - 2 - 12 * ( ( nMonth - 14 ) / 12 ) ) ) / 12) -
    int(( 3 * ( int(( nYear + 4900 + int(( nMonth - 14 ) / 12) ) / 100) ) ) / 4) +
    nDay - 2415019 - 32075;
  
  if (nSerialDate < 60) {
    // Because of the 29-02-1900 bug, any serial date under 60 is one
    // off... Compensate.
        
    nSerialDate--;
  }
  
  return (int)nSerialDate;
}

bool ValidCellCoord( const BText & name, const BDat Row, const BDat Col,
                     size_t &r, size_t &c )
{
  
  if ( !Row.IsKnown() || Row.Value() <= 0 ||
       !Col.IsKnown() || Col.Value() <= 0 ) {
    char buffer[256];
    snprintf( buffer, 256, "(%f,%f)", Row.Value(), Col.Value() );
    Error( name + ": " +
           I2( "invalid cell coordinates ",
               "coordenadas de celda invalidas " ) + buffer );
    return false;
  }
  // Cell coordinates are 0-based internally
  r = size_t( Row.Value( ) - 1 );
  c = size_t( Col.Value( ) - 1 );
  return true;
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelOpen);
DefExtOpr(1, BDatExcelOpen, "Excel.Open", 1, 1, "Text", "(Text path)",
          I2("Read and excel file and returns the object handler. "
             "If the file does not exists 0.0 is returned",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelOpen::CalcContens()
{
  BText &path = Text( Arg( 1 ) );
  TolExcel *aux = new TolExcel( path.Buffer() );
  if ( !aux->IsValid() ) {
    delete aux;
    aux = NULL;
  }
  contens_ = BDat( TolExcel::code_addr( aux ) );
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelDestroy);
DefIntOpr(1, BDatExcelDestroy, "Excel.Destroy", 1, 1,
          "(Real ExcelHandler)",
          I2("Destroy a previously created excel handler. See also "
             "Excel.Open, Excel.Create",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelDestroy::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  TolExcel *aux = TolExcel::decode_addr( addr );
  /* debemos tener un hash de las direccion creadas con Open de forma
     tal que podamos verificar si la direccion es valida antes de
     hacer delete */
  if ( aux ) {
    delete aux;
    contens_ = BDat( 1.0 );
  } else {
    Error( BText( "Excel.Destroy: " ) +
           I2("invalid excel handler",
              "identificador de objeto excel invalido") );
    contens_ = BDat( 0.0 );
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelActivateNamedWS);
DefExtOpr(1, BDatExcelActivateNamedWS, "Excel.ActivateNamedWS", 2, 2,
          "Real Text",
          "(Real ExcelHandler, Text WorkSheet)",
          I2("Change the current active work sheet to a given work sheet name. "
             "Returns 1 if the work sheet is valid 0 in case of error",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelActivateNamedWS::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  BText &name = Text( Arg( 2 ) );
  TolExcel *xls = TolExcel::decode_addr( addr );
  if ( xls ) {
    bool status = xls->SetActiveWS( name.Buffer() );
    if ( !status ) {
      Error( BText( "Excel.ActivateNamedWS: " ) +
             I2("unable to activate worksheet ",
                "no se ha posido establecer la hoja activa " ) + name );
    }
    contens_ = BDat( status );
  } else {
    Error( BText( "Excel.ActivateNamedWS: " ) +
           I2("invalid excel handler",
              "identificador de objeto excel invalido") );
    contens_ = BDat( 0.0 );
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelActivateWS);
DefExtOpr(1, BDatExcelActivateWS, "Excel.ActivateWS", 2, 2, "Real Real",
          "(Real ExcelHandler, Real indexWS)",
          I2("Change the current active work sheet to a given work sheet "
             "index. Returns 1 if the work sheet is valid 0 in case of error",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelActivateWS::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  BDat &index = Dat( Arg( 2 ) );
  if ( index.IsKnown() ) {
    int idx = int( index.Value() );
    if ( idx > 0 ) {
      TolExcel *xls = TolExcel::decode_addr( addr );
      if ( xls ) {
        bool status = xls->SetActiveWS( size_t( idx - 1 ) );
        if ( !status ) {
          char buffer[16];
          snprintf( buffer, 16, "%d", idx );
          Error( BText( "Excel.ActivateWS: " ) +
                 I2("unable to activate worksheet ",
                    "no se ha posido establecer la hoja activa ") + buffer );
        }
        contens_ = BDat( status );
      } else {
        Error( BText( "Excel.ActivateWS: " ) +
               I2("invalid excel handler",
                  "identificador de objeto excel invalido") );
        contens_ = BDat( 0.0 );
      }
    } else {
      Error( BText( "Excel.ActivateWS: " ) +
             I2( "invalid work sheet index, must be >= 1",
                 "indice de hoja invalido, debe ser >= 1") );
      contens_ = BDat( 0.0 );
    }
  } else {
    Error( I2("Excel.ActivateWS : invalid Work Sheet index, must be known",
              "Excel.ActivateWS : indice de hoja invalido, debe ser conocido") );
    contens_ = BDat( 0.0 );
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelReadReal);
DefExtOpr(1, BDatExcelReadReal, "Excel.ReadReal", 2, 2, "Real Anything",
          "(Real ExcelHandler, Anything Cell)",
          I2("Return the contents of the given cell as a Real. If the contents "
             "of the cell is not a Real value then ? is returned. "
             "The cell can be given as Text or Set. For instance you can use "
             "either \"A1\" or [[1,1]].",
             "Retorna el contenido de una celda leido como valor numerico. "
             "Si el contenido de la celda no puede convertirse a Real se "
             "retorna ?. Las coordenadas de la celda pueden especificarse como "
             "Text o Set. Por ejemplo se puede usar indistintamente "
             "\"A1\" o [[1,1]]."),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelReadReal::CalcContens()
{
  static BText _name_( "Excel.ReadReal" );
  double addr = Dat( Arg( 1 ) ).Value();
  size_t r, c;
  
  if ( TolExcel::getCellCoordinates( _name_, Arg(2), r, c ) ) {
    TolExcel *xls = TolExcel::decode_addr( addr );
    if ( xls ) {
      xls->GetCellReal( _name_, r, c, contens_ );
    } else {
      Error( _name_ +
             I2(": invalid excel handler",
                ": identificador de objeto excel invalido") );
      contens_ = BDat::Unknown( );
    }
  } else {
    contens_ = BDat::Unknown( );
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtExcelReadText);
DefExtOpr(1, BTxtExcelReadText, "Excel.ReadText", 2, 2, "Real Anything",
          "(Real ExcelHandler, Anything Cell)",
          I2("Return the contents of the given cell as a Text. If the contents "
             "of the cell is not a Text value then \"\" is returned. "
             "The cell can be given as Text or Set. For instance you can use "
             "either \"A1\" or [[1,1]].",
             "Retorna el contenido de una celda leido como texto. "
             "Si el contenido de la celda no puede convertirse a Text se "
             "retorna ?. Las coordenadas de la celda pueden especificarse como "
             "Text o Set. Por ejemplo se puede usar indistintamente "
             "\"A1\" o [[1,1]]."),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BTxtExcelReadText::CalcContens()
{
  static BText _name_( "Excel.ReadText" );
  double addr = Dat( Arg( 1 ) ).Value();
  size_t r, c;
  
  if ( TolExcel::getCellCoordinates( _name_, Arg(2), r, c ) ) {
    TolExcel *xls = TolExcel::decode_addr( addr );
    if ( xls ) {
      xls->GetCellText( _name_, r, c, contens_ );
    } else {
      Error( _name_ +
             I2(": invalid excel handler",
                ": identificador de objeto excel invalido") );
      contens_ = "";
    }
  } else {
    contens_ = "";
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BDate, BDteTemporary, BDteExcelReadDate);
DefExtOpr(1, BDteExcelReadDate, "Excel.ReadDate", 2, 2, "Real Anything",
          "(Real ExcelHandler, Anything Cell)",
          I2("Return the contents of the given cell as a Date. If the contents "
             "of the cell is not a Date value then the unknown date is returned"
             "The cell can be given as Text or Set. For instance you can use "
             "either \"A1\" or [[1,1]].",             
             "Retorna el contenido de una celda leido como fecha. "
             "Si el contenido de la celda no puede convertirse a Date se "
             "retorna la fecha desconocida. Las coordenadas de la celda pueden "
             "especificarse como Text o Set. Por ejemplo se puede usar "
             "indistintamente \"A1\" o [[1,1]]."),          
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDteExcelReadDate::CalcContens()
{
  static BText _name_( "Excel.ReadDate" );
  double addr = Dat( Arg( 1 ) ).Value();
  size_t r, c;
  
  if ( TolExcel::getCellCoordinates( _name_, Arg(2), r, c ) ) {
    TolExcel *xls = TolExcel::decode_addr( addr );
    if ( xls ) {
      xls->GetCellDate( _name_, r, c, contens_ );
    } else {
      Error( _name_ +
             I2(": invalid excel handler",
                ": identificador de objeto excel invalido") );
      contens_ = BDate::Unknown( );
    }
  } else {
    contens_ = BDate::Unknown( );
  }
}

//---------------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetExcelReadRange);
DefExtOpr(1, BSetExcelReadRange, "Excel.ReadRange", 3, 3, "Real Anything Anything",
          "(Real ExcelHandler, Anything CornerLT, Anything CornerRB)",
          I2("Return the contents of the cells contained given cell's range. "
             "The cell corners delimiting the range can be given as Text or "
             "Set. For instance you can use either \"A1\" or [[1,1]].",
             "Retorna el contenido de las celdas contenidas en el rango de "
             "celdas dado. Las esquinas del rango pueden especificarse como "
             "Text o Set. Por ejemplo se puede usar indistintamente "
             "\"A1\" o [[1,1]]."),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BSetExcelReadRange::CalcContens()
{
  static BText _name_( "Excel.ReadRange" );
  double addr = Dat( Arg( 1 ) ).Value();
  BSyntaxObject *cornerLT = Arg(2);
  BSyntaxObject *cornerBR = Arg(3);
  size_t c0, r0, c1, r1;
  
  TolExcel *xls = TolExcel::decode_addr( addr );
  if ( xls ) {
    // bool status1 = ;
    // bool status2 = ;
    if ( TolExcel::getCellCoordinates( _name_, cornerLT, r0, c0 ) &&
         TolExcel::getCellCoordinates( _name_, cornerBR, r1, c1 ) ) {
      BList*	result = NIL;
      BList*	aux    = NIL;
      size_t	r,c;
      for( r = r0; r <= r1; r++ ) {
	BList* lsta	  = NIL;
	BList* auxa	  = NIL;
	for( c = c0; c <= c1; c++ ) {
          LstFastAppend( lsta, auxa, xls->GetCellAnything( _name_, r, c ) );
        }
	LstFastAppend( result, aux, NewSet( "", "", lsta, NIL, BSet::Generic) );
      }
      contens_.RobStruct( result, NIL, BSet::Table );
    }
  } else {
    Error( _name_ +
           I2(": invalid excel handler",
              ": identificador de objeto excel invalido") );
  }
}

//--------------------------------------------------------------------
static BSyntaxObject*
EvExcelReadCell( BGrammar* gra, const List* tre, BBool left )
/*! Evaluate Copy expressions
*/
//--------------------------------------------------------------------
{
  static BText _name_( "Excel.ReadCell" );
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if( BSpecialFunction::TestNumArg( _name_, 2, nb, 2 ) ) {
    BSyntaxObject *addr = GraReal()->EvaluateTree( Branch( tre, 1 ) );
    if ( !Dat( addr ).IsKnown( ) ) {
      Error( _name_ + ": " +
             I2( "invalid excel object address, must be known",
                 "direccion de objeto excel invalida, debe ser conocida" ) );
      return NULL;
    }
    BSyntaxObject *cell = GraAnything()->EvaluateTree( Branch( tre, 2 ) );
    if ( cell ) {
      size_t i_row, i_col;
      if ( TolExcel::getCellCoordinates( _name_, cell, i_row, i_col ) ) {
        TolExcel *xls = TolExcel::decode_addr( Dat( addr ).Value() );
        if ( !xls ) {
          Error( _name_ + ": " +
                 I2("invalid excel object address",
                    "direccion de objecto excel invalido") );
          return NULL;
        }    
        result = xls->GetCellAnything( _name_, i_row, i_col );
      }
    }
  }
  result = BSpecialFunction::TestResult( _name_, result, tre, NIL, BTRUE );
  return(result);
}

static void* cloneInitExcelReadCell_()
{
  BSpecialFunction::AddInstance( "Excel.ReadCell",
  "(Real ExcelHandler, Anything Cell)",
  I2("Read the contents of a cell in the active work sheet"
     "The cell can be given as Text or Set. For instance you can use "
     "either \"A1\" or [[1,1]].",
     "Lee el contenido de una celda en la hoja de trabajo activa"
     "Las coordenadas de la celda pueden especificarse como "
     "Text o Set. Por ejemplo se puede usar indistintamente "
     "\"A1\" o [[1,1]]."),
  EvExcelReadCell );
  return new int(1);
};

static int *_init_ExcelReadCell =
  (int*)(__delay_init((void**)(&_init_ExcelReadCell), &cloneInitExcelReadCell_));
