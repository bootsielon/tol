#include <tcl.h>
#include "toltcl.h"

int AppInit(Tcl_Interp *interp)
{
  if( Tcl_Init(interp) == TCL_ERROR )
    {
    return TCL_ERROR;
    }

  /*Tcl_SetVar( interp, "tclsh_StartApp", "1", TCL_GLOBAL_ONLY );*/
  Tcl_PkgRequire( interp, "TolshApp", TOLTCL_VERSION, 1);

  return TCL_OK;
}

int main(int argc, char *argv[])
{
  Tcl_Main( argc, argv, AppInit );
  return 0;
}
