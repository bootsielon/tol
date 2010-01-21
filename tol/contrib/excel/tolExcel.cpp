/* tolExcel.cpp: API for libxls : http://libxls.sourceforge.net/

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

extern "C" {
#include <libxls/xls.h>
}

#pragma pack(4)

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_tree.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bspfun.h>


class tol_excel_t {
public:
  tol_excel_t( char *path, char *encoding );
  ~tol_excel_t();

  int validWB()
  {
    return m_ptrWB != NULL;
  }
  
  int validWS()
  {
    return m_ptrActiveWS != NULL;
  }

  int activateWS( int num )
  {
    m_ptrActiveWS = xls_getWorkSheet( m_ptrWB, num );
    if ( m_ptrActiveWS ) {
      xls_parseWorkSheet( m_ptrActiveWS );
    }
    return m_ptrActiveWS != NULL;
  }

  int activateWS( const char * sheetName );
  
  int getActiveWSIndex()
  {
    return m_idxWS;
  }
  
  const char *getActiveWSName()
  {
    return m_idxWS >= 0 ? m_ptrWB->sheets.sheet[ m_idxWS ].name : NULL;
  }
  
  xlsCell *GetCell( int iWS, int row, int column )
  {
    return NULL;
  }
  
  xlsCell *GetCell( int row, int column )
  {
    return m_ptrActiveWS ? xls_cell( m_ptrActiveWS, row, column ) : NULL;
  }

  BSyntaxObject *GetObjCell( BText &err_name, int row, int col );
  
  static double code_addr( tol_excel_t* ptr )
  {
    double addr = 0.0;
    *((tol_excel_t**)(&addr)) = ptr;
    return addr;
  }
  
  static tol_excel_t* decode_addr( double addr )
  {
    return *((tol_excel_t**)(&addr));
  }

protected:
  
  int closeActiveWS( )
  {
    if ( m_ptrActiveWS ) {
      xls_close_WS( m_ptrActiveWS );
      m_ptrActiveWS = NULL;
      m_idxWS = -1;
      return 1;
    }
    return 0;
  }

  int closeWB()
  {
    if ( m_ptrWB ) {
      closeActiveWS();
      xls_close_WB( m_ptrWB );
      m_ptrWB = NULL;
      return 1;
    }
    return 0;
  }
  
private:
  xlsWorkBook* m_ptrWB;
  xlsWorkSheet* m_ptrActiveWS;
  int m_idxWS;
};

tol_excel_t::tol_excel_t( char *path, char *encoding )
{
  m_ptrWB = xls_open( path, encoding );
  m_ptrActiveWS = NULL;
  m_idxWS = -1;
}

tol_excel_t::~tol_excel_t()
{
  closeWB();
}

int tol_excel_t::activateWS( const char * sheetName )
{
  DWORD i;
  
  for ( i = 0; i < m_ptrWB->sheets.count; i++ ) {
    if ( strcmp( sheetName, m_ptrWB->sheets.sheet[i].name ) == 0 ) {
      break;
    }
  }
  
  if ( i == m_ptrWB->sheets.count) {
    return activateWS( i );
  } else {
    closeActiveWS();
  }
  return 0;
}

BSyntaxObject *tol_excel_t::GetObjCell( BText &err_name, int row, int col )
{
  BSyntaxObject *result = NULL;
  
  if ( !validWS( ) ) {
    Error( err_name + ": " +
           I2("there is no active work sheet",
              "no hay hoja de trabajo activa") );
    return NULL;
  }
  xlsCell *cell = GetCell( row, col  );
  if ( !cell || cell->ishiden ) {
    Warning( err_name + ": " +
             I2("the cell does not exists or is hiden",
                "la celda no existe o esta oculta") );
    result = new BContensText( "" );
  } else {
    if ( cell->id == 0x27e || cell->id == 0x0BD ||
         cell->id == 0x203 ) {
      result = new BContensDat( cell->d );
    } else if ( cell->id == 0x06 ) {
      // formula
      if ( cell->l == 0 ) {
        // its a number 
        result = new BContensDat( cell->d );
      } else {
        if ( !strcmp( cell->str, "bool" ) ) {
          // its boolean, and test cell->d
          result = new BContensDat( cell->d );
        } else if ( !strcmp( cell->str, "error" ) ) {
          // formula is in error
          result = new BContensText( "*error*" );
        } else {
          // ... cell->str is valid as the result of a string formula.
          result = new BContensText( cell->str );
        }
      }      
    } else if (cell->str != NULL) {
      result = new BContensText( cell->str );
    } else {
      result = new BContensText( "" );
    }
  }
  return result;
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelOpen);
DefExtOpr(1, BDatExcelOpen, "Excel.Open", 1, 2, "Text Text",
          "(Text path [, Text encoding])",
          I2("Read and excel file and returns the object handler. "
             "If the file does not exists 0.0 is returned",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelOpen::CalcContens()
{
  BText &path = Text( Arg( 1 ) );
  BText encoding = NumArgs( ) > 1 ? ToUpper(Text(Arg(2))) : "ASCII";
  tol_excel_t *aux = new tol_excel_t( path.Buffer(), encoding );
  if ( !aux->validWB() ) {
    delete aux;
    aux = NULL;
  }
  contens_ = BDat( tol_excel_t::code_addr( aux ) );
}

//---------------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatExcelClose);
DefIntOpr(1, BDatExcelClose, "Excel.Close", 1, 1,
          "(Real ExcelHandler)",
          I2("Close a previously openned excel handler. See alsoExcel.Open ",
             ""),
          BOperClassify::System_);
//----------------------------------------------------------------------------
void BDatExcelClose::CalcContens()
{
  double addr = Dat( Arg( 1 ) ).Value();
  tol_excel_t *aux = tol_excel_t::decode_addr( addr );
  /* debemos tener un hash de las direccion creadas con Open de forma
     tal que podamos verificar si la direccion es valida antes de
     hacer delete */
  if ( aux ) {
    delete aux;
    contens_ = BDat( 1.0 );
  } else {
    Error( BText( "Excel.Close: " ) +
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
  tol_excel_t *xls = tol_excel_t::decode_addr( addr );
  if ( xls ) {
    contens_ = BDat( xls->activateWS( name.Buffer() ) );
  } else {
    Error( BText( "Excel.Close: " ) +
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
    if ( idx >= 0 ) {
      tol_excel_t *xls = tol_excel_t::decode_addr( addr );
      if ( xls ) {
        contens_ = BDat( xls->activateWS( idx ) );
      } else {
        Error( BText( "Excel.ActivateWS: " ) +
               I2("invalid excel handler",
                  "identificador de objeto excel invalido") );
        contens_ = BDat( 0.0 );
      }
    } else {
      Error( BText( "Excel.ActivateWS: " ) +
             I2( "invalid work sheet index, must be >= 0",
                 "indice de hoja invalido, debe ser >= 0") );
      contens_ = BDat( 0.0 );
    }
  } else {
    Error( I2("Excel.ActivateWS : invalid Work Sheet index, must be known",
              "Excel.ActivateWS : indice de hoja invalido, debe ser conocido") );
    contens_ = BDat( 0.0 );
  }
}

//--------------------------------------------------------------------
static BSyntaxObject*
EvExcelReadCell( BGrammar* gra, const List* tre, BBool left )
/*! Evaluate Copy expressions
*/
//--------------------------------------------------------------------
{
  static BText _name_ = "Excel.ReadCell";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if( BSpecialFunction::TestNumArg( _name_, 3, nb, 3 ) ) {
    BSyntaxObject *addr = GraReal()->EvaluateTree( Branch( tre, 1 ) );
    if ( !Dat( addr ).IsKnown( ) ) {
      Error( _name_ + ": " +
             I2( "invalid excel object address, must be known",
                 "direccion de objeto excel invalida, debe ser conocida" ) );
      return NULL;
    }
    BSyntaxObject *row = GraReal()->EvaluateTree( Branch( tre, 2 ) );
    BSyntaxObject *col = GraReal()->EvaluateTree( Branch( tre, 3 ) );
    if ( row && col ) {
      BDat &dat_row = Dat( row );
      BDat &dat_col = Dat( col );
      if ( !dat_row.IsKnown() || dat_row.Value() < 0 ||
           !dat_col.IsKnown() || dat_col.Value() < 0 ) {
        Error( _name_ + ": " +
               I2("invalid cell coordinates, must be ",
                  "coordenadas de celda invalidas, deben ser ") +
               "(row,col)>=(0,0)" );
        return NULL;
      }
      WORD i_row = DWORD( dat_row.Value() );
      WORD i_col = DWORD( dat_col.Value() );
      tol_excel_t *xls = tol_excel_t::decode_addr( Dat( addr ).Value() );
      if ( !xls ) {
        Error( _name_ + ": " +
               I2("invalid excel object address",
                  "direccion de objecto excel invalido") );
        return NULL;
      }    
      result = xls->GetObjCell( _name_, i_row, i_col );    
    }
  }
  result = BSpecialFunction::TestResult( _name_, result, tre, NIL, BTRUE );
  return(result);
}

static void* cloneInitExcelReadCell_()
{
  BSpecialFunction::AddInstance( "Excel.ReadCell",
  "(Real ExcelID, Real Row, Real Column)",
  I2("Read the contents of a cell in the active work sheet",
     "Lee el contenido de una celda en la hoja de trabajo activa"),
  EvExcelReadCell );
  return new int(1);
};

static int *_init_ExcelReadCell =
  (int*)(__delay_init((void**)(&_init_ExcelReadCell), &cloneInitExcelReadCell_));
