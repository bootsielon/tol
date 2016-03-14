#include <tcl.h>
#include "toltcl.h"

#if defined(WIN32)
#include "windows.h"
#else
#include "tol/tol_init.h"
#endif

static int g_argc = 0;
static char **g_argv = NULL;

static
void SetupArgv( Tcl_Interp *interp )
{
  Tcl_Obj *argvPtr = Tcl_NewListObj(0, NULL);
  int i;
  for( i = 1; i < g_argc; i++ )
    {
      Tcl_DString ds;
      Tcl_ExternalToUtfDString(NULL, g_argv[i], -1, &ds);
      Tcl_ListObjAppendElement(NULL, argvPtr, 
			       Tcl_NewStringObj(Tcl_DStringValue(&ds),
						Tcl_DStringLength(&ds)));
      Tcl_DStringFree(&ds);
    }
    Tcl_SetVar2Ex(interp, "argv", NULL, argvPtr, TCL_GLOBAL_ONLY);
    Tcl_SetVar2Ex(interp, "argc", NULL, Tcl_NewIntObj(g_argc-1), TCL_GLOBAL_ONLY);
}

#if defined(WIN32)
void SetupDllSearchPath( const char *argv0 )
{
  int n;
  //const char **strArr;
  const char **ptr;
  Tcl_DString dstr;
  
  //ptr = &strArr;
  Tcl_SplitPath( argv0, &n, &ptr );
  if( n >= 2 )
    {
	  Tcl_DStringInit( &dstr );
      Tcl_JoinPath( n - 1, ptr,  &dstr );
	  SetDllDirectory( Tcl_DStringValue( &dstr ) );
	  Tcl_DStringFree( &dstr );
    }
  Tcl_Free( (char*)ptr );
}
#endif

int AppInit(Tcl_Interp *interp)
{
  if( Tcl_Init(interp) == TCL_ERROR )
    {
    return TCL_ERROR;
    }
  SetupArgv( interp );
  const char *argv0 = Tcl_GetNameOfExecutable();
#if defined(WIN32)
  SetupDllSearchPath( argv0 );
  Tcl_VarEval( interp, "lappend", " auto_path", " [file join",
               " [file dir [file dir {", argv0,
               "} ] ] lib ]", NULL );
#else
  Tcl_VarEval( interp, "lappend", " auto_path", " [file join",
               " [file dir [file dir [expr {[file type {", argv0, "}] eq {link} ? [file readlink {", argv0, "}] : {", argv0 ,"}} ] ] ] lib ]", NULL );
#endif   
  if ( !Tcl_PkgRequire( interp, "TolshApp", TOLTCL_VERSION, 1) )
    {
    printf( "%s\n", Tcl_GetStringResult( interp ) );
    Tcl_Exit( -1 );
    }
  return TCL_OK;
}

int main(int argc, char *argv[])
{
  g_argc = argc;
  g_argv = argv;
  #if !defined(WIN32)
  const char * version = TOLVersion();
  #endif
  Tcl_Main( 1, argv, AppInit );
  return 0;
}
