#include <tcl.h>
#include "toltcl.h"

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

int AppInit(Tcl_Interp *interp)
{
  if( Tcl_Init(interp) == TCL_ERROR )
    {
    return TCL_ERROR;
    }
  SetupArgv( interp );
  Tcl_PkgRequire( interp, "TolshApp", TOLTCL_VERSION, 1);
  return TCL_OK;
}

int main(int argc, char *argv[])
{
  g_argc = argc;
  g_argv = argv;
  Tcl_Main( 1, argv, AppInit );
  return 0;
}
