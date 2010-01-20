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


#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

extern "C" {
#include <libxls/xls.h>
}

#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>

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
  if ( aux->validWB() ) {
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
  delete aux;
  contens_ = BDat( 1.0 );
}

