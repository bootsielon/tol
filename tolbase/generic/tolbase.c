/* 
 * tkAppInit.c --
 *
 *	Provides a default version of the Tcl_AppInit procedure for
 *	use in wish and similar Tk-based applications.
 *
 * Copyright (c) 1993 The Regents of the University of California.
 * Copyright (c) 1994-1997 Sun Microsystems, Inc.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#include "tk.h"
#include "locale.h"
#ifdef WIN32
#include <windows.h>
#endif

// Tcl_SetStartupScript
#if TCL_MAJOR_VERSION == 8 && TCL_MINOR_VERSION <= 5
//#include "tclIntDecls.h"
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

#ifdef TK_TEST
extern int		Tktest_Init _ANSI_ARGS_((Tcl_Interp *interp));
#endif /* TK_TEST */

/*
 *----------------------------------------------------------------------
 *
 * main --
 *
 *	This is the main program for the application.
 *
 * Results:
 *	None: Tk_Main never returns here, so this procedure never
 *	returns either.
 *
 * Side effects:
 *	Whatever the application does.
 *
 *----------------------------------------------------------------------
 */

#ifdef WIN32
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

int
main(argc, argv)
    int argc;			/* Number of command-line arguments. */
    char **argv;		/* Values of command-line arguments. */
{
    /*
     * The following #if block allows you to change the AppInit
     * function by using a #define of TCL_LOCAL_APPINIT instead
     * of rewriting this entire file.  The #if checks for that
     * #define and uses Tcl_AppInit if it doesn't exist.
     */
    
#ifndef TK_LOCAL_APPINIT
#define TK_LOCAL_APPINIT Tcl_AppInit    
#endif
    extern int TK_LOCAL_APPINIT _ANSI_ARGS_((Tcl_Interp *interp));

    //HWND hwnd = GetConsoleWindow();
    //ShowWindow(hwnd, 0);
    
    /*
     * The following #if block allows you to change how Tcl finds the startup
     * script, prime the library or encoding paths, fiddle with the argv,
     * etc., without needing to rewrite Tk_Main()
     */
    
#ifdef TK_LOCAL_MAIN_HOOK
    extern int TK_LOCAL_MAIN_HOOK _ANSI_ARGS_((int *argc, char ***argv));
    TK_LOCAL_MAIN_HOOK(&argc, &argv);
#endif

    Tk_Main(argc, argv, TK_LOCAL_APPINIT);
    return 0;			/* Needed only to prevent compiler warning. */
}

/*
 *----------------------------------------------------------------------
 *
 * Tcl_AppInit --
 *
 *	This procedure performs application-specific initialization.
 *	Most applications, especially those that incorporate additional
 *	packages, will have their own version of this procedure.
 *
 * Results:
 *	Returns a standard Tcl completion code, and leaves an error
 *	message in the interp's result if an error occurs.
 *
 * Side effects:
 *	Depends on the startup script.
 *
 *----------------------------------------------------------------------
 */

int
Tcl_AppInit(interp)
    Tcl_Interp *interp;		/* Interpreter for application. */
{
    if (Tcl_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    if (Tk_Init(interp) == TCL_ERROR) {
	return TCL_ERROR;
    }
    // Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);


    /*
     * Call the init procedures for included packages.  Each call should
     * look like this:
     *
     * if (Mod_Init(interp) == TCL_ERROR) {
     *     return TCL_ERROR;
     * }
     *
     * where "Mod" is the name of the module.
     */

    /*
     * Call Tcl_CreateCommand for application-specific commands, if
     * they weren't already created by the init procedures called above.
     */

    /*
     * Specify a user-specific startup file to invoke if the application
     * is run interactively.  Typically the startup file is "~/.apprc"
     * where "app" is the name of the application.  If this line is deleted
     * then no user-specific startup file will be run under any conditions.
     */

#if defined(WIN32)
    SetupArgv( interp );
    Tcl_VarEval( interp, "lappend", " auto_path", " [file join",
		 " [file dir [file dir ", Tcl_GetNameOfExecutable(),
		 " ] ] lib ]", NULL );
    if ( !Tcl_PkgRequire( interp, "TolbaseApp", TOLBASE_VERSION, 1) )
      {
	printf( "%s\n", Tcl_GetStringResult( interp ) );
	Tcl_Exit( -1 );
      }
    //Tcl_PkgRequire( interp, "TolbaseApp", TOLBASE_VERSION, 1);
#else
    const char* exeName = Tcl_GetNameOfExecutable();
    Tcl_VarEval( interp, "file join",
		 " [ file dir ", Tcl_GetNameOfExecutable(),
		 " ] tolbase.tcl", NULL );
    Tcl_Obj *objPath = Tcl_GetObjResult( interp );
    Tcl_SetStartupScript( Tcl_GetObjResult( interp ), NULL ); 
#endif
    return TCL_OK;
}
