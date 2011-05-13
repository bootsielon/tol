#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_init.h>
#include <tol/tol_version_def.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bset.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bspfun.h>

#include <string.h>

void Tol_HciWriterEx( const BText & str, void *data )
{
  const char *name = (const char*)data;
  
  std::cout << name << ": " << str.String();
}

void TestMultyEvaluate( int argc, char* argv[] )
{
  if ( argc != 3 ) {
    std::cout << "\nusage: " << argv[ 0 ] << " MultiEval tol_expr\n";
    return;
  }
  
  // Evaluo la expresion TOL
  BList * tol_result = MultyEvaluate( argv[ 2 ] );

  if ( tol_result ) {
    std::cout << "Evaluation was right\n";
  } else {
    std::cout << "Evaluation was wrong\n";
    return;
  }
  // Creo un objeto Set que se apropia de los objetos creados
  BSet set;
  set.RobStruct( tol_result, NIL, BSet::Generic );

  std::cout << "The result has " << set.Card() << " elements\n";

  for ( int i = 1; i <= set.Card( ); i++ ) {
    BGrammar *type = set[ i ]->Grammar();
    BText name = type->Name();
    std::cout << "item " << i << " has type " << name.Buffer() << "\n";
  }
}

void GetCode( const char* grammar, const char* name, BCode &result)
{
  BGrammar* gra  = Gram( grammar );
  if ( gra ) {
    const BSpecialFunction *spf = BSpecialFunction::Get( name );
    if ( spf ) {
      result.PutSpecialFunction(spf);
    } else {
      BUserFunction* opr = (BUserFunction*)gra->FindOperator( name );
      result.PutOperator( opr );
    }
  } else {
    result.PutOperator( NULL );
  }
}

void TestBuiltinCode( int argc, char* argv[] )
{
  std::cout << "NObject before: "<< BSyntaxObject::NSyntaxObject() << "\n"; 
  BCode code;
  GetCode( "Real", "+", code );
  BUserDat *udat1 = new BContensDat( 1 );
  BUserDat *udat2 = new BContensDat( 2 );
  BList *args = Cons( udat1, Cons( udat2, NULL ) );
  BSyntaxObject *result = code.Evaluator( args );
  if ( result ) {
    BDat dat = Dat( result );
    std::cout << "TestBuiltinCode success: "<< dat << "\n";
  } else {
    std::cout << "TestBuiltinCode failed!\n"; 
  }
  DESTROY( result );
  std::cout << "NObject after: "<< BSyntaxObject::NSyntaxObject() << "\n";
}

void TestUserCode( int argc, char* argv[] )
{
  std::cout << "NObject before: "<< BSyntaxObject::NSyntaxObject() << "\n"; 
  BCode code;
  GetCode( "Real", "ErrorMsg", code );
  BUserText *utext = new BContensText( "Esto no es un error" );
  BList *args = Cons( utext, NULL );
  BSyntaxObject *result = code.Evaluator( args );
  if ( result ) {
    BDat dat = Dat( result );
    std::cout << "TestUserCode success: "<< dat << "\n";
  } else {
    std::cout << "TestUserCode failed!\n"; 
  }
  DESTROY( result );
  std::cout << "NObject after: "<< BSyntaxObject::NSyntaxObject() << "\n";
}

void TestSpecialFunction( int argc, char* argv[] )
{
  std::cout << "NObject before: "<< BSyntaxObject::NSyntaxObject() << "\n"; 
  BCode code;
  GetCode( "Anything", "Copy", code );
  BUserDat *udat = new BContensDat( 1 );
  BList *args = Cons( udat, NULL );
  BSyntaxObject *result = code.Evaluator( args );
  if ( result ) {
    BDat dat = Dat( result );
    std::cout << "TestSpecialFunction success: "<< dat << "\n";
  } else {
    std::cout << "TestSpecialFunction failed!\n"; 
  }
  DESTROY( result );
  std::cout << "NObject after: "<< BSyntaxObject::NSyntaxObject() << "\n";
}


int main( int argc, char*argv[] )
{
  if ( argc < 2 ) {
    std::cout << "\nusage: " << argv[0] << " test_name ?arg?\n";
    return -1;
  }
  // inicializo el kernel de tol
  int lang = 1;
  char *vmode = NULL;
  const char *name = "Tester";
  BOut::PutHciWriterEx( Tol_HciWriterEx, (void*)name );
  InitTolKernel( argv[ 0 ], lang, vmode );
  std::cout << "\n";
  LoadInitLibrary( );

  if ( !strcasecmp( "MultiEval", argv[ 1 ] ) ) {
    TestMultyEvaluate( argc, argv );
  } else if( !strcasecmp( "BuiltinCode", argv[ 1 ] ) ) {
    TestBuiltinCode( argc, argv );
  } else if( !strcasecmp( "UserCode", argv[ 1 ] ) ) {
    TestUserCode( argc, argv );
  } else if( !strcasecmp( "SpecialFunction", argv[ 1 ] ) ) {
    TestSpecialFunction( argc, argv );
  } else {
    std::cout << "Unknown test: '" << argv[ 1 ] << "'\n";
  }
  
  return 0;
}
