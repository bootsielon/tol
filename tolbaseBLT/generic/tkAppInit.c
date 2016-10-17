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
 *
 * RCS: @(#) $Id: tkAppInit.c,v 1.4 2004/07/13 17:24:10 josp Exp $
 */

#include <tk.h>
#include <locale.h>
#include <string.h>

#include <sys/stat.h>
#ifdef __WIN32__
#include <windows.h>
#define setenv(a,b,c) SetEnvironmentVariable(a,b)

static void		setargv _ANSI_ARGS_((int *argcPtr, char ***argvPtr));
static void		WishPanic _ANSI_ARGS_(TCL_VARARGS(char *,format));

static BOOL consoleRequired = TRUE;

#else
#include <unistd.h>
/*
 * The following variable is a special hack that is needed in order for
 * Sun shared libraries to be used for Tcl.
 */

extern int matherr();
int *tclDummyMathPtr = (int *) matherr;
#endif

int  LoadMainTcl          _ANSI_ARGS_ ((Tcl_Interp * interp));
void TolTk_SetEnvironment _ANSI_ARGS_ ((Tcl_Interp * interp, int * argc, char *** argv));

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
    
    Tcl_Interp *interp;

    interp = Tcl_CreateInterp();

    TolTk_SetEnvironment(interp,&argc,&argv);
    Tk_MainEx(argc, argv, TK_LOCAL_APPINIT,interp);
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
  int tcl_code;

  if (Tcl_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }

 if (Tk_Init(interp) == TCL_ERROR) {
    return TCL_ERROR;
  }
  Tcl_StaticPackage(interp, "Tk", Tk_Init, Tk_SafeInit);
  
  
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
   *     */
  
  // Tcl_SetVar(interp, "tcl_rcFileName", "~/.wishrc", TCL_GLOBAL_ONLY);

  

  //tcl_code = LoadMainTcl( interp );

  return TCL_OK;
}

int LoadMainTcl( Tcl_Interp * interp )
{
  Tcl_DString ds;
  CONST char * pathv[2];
  int tcl_code = TCL_ERROR;

  pathv[0] = Tcl_GetVar(interp, "toltk_library", TCL_GLOBAL_ONLY);
  if ( pathv[0] ) {
    Tcl_DStringInit(&ds);
    pathv[1] = "toltk/init_toltk.tcl";
    Tcl_JoinPath(2, pathv, &ds);
    tcl_code = Tcl_EvalFile(interp,ds.string );
  }
  return tcl_code;
}

/*
 *----------------------------------------------------------------------
 *
 * TolTk_SetEnvironment --
 *
 *   Set the variable toltk_library to:
 *
 *     ..\lib in case of instalation
 *
 *     ..\..\lib in case of development  
 *
 *----------------------------------------------------------------------
 */

void TolTk_SetEnvironment( Tcl_Interp * interp, int * argc, char *** argv )
{
  static char ** new_argv;
  
  int pathc;
  CONST char ** pathv, * pathv1[2];
  char * argv0;
  Tcl_DString ds, ds1, ds_argv0;
  struct stat stat_buf;
  int sys_res, i;

  if ( (*argv)[0] ) {
    Tcl_DStringInit(&ds_argv0);
    argv0 = Tcl_ExternalToUtfDString(NULL, (*argv)[0], -1, &ds_argv0);
    Tcl_SplitPath(argv0, &pathc, &pathv);

    /* set toltk_bin */

    /* look for ../lib */
    
    Tcl_DStringInit(&ds);
    pathv[pathc - 1] = "../lib";
    Tcl_JoinPath(pathc,pathv,&ds);
    if ( stat(ds.string, &stat_buf) == -1 ) {
      Tcl_DStringFree( &ds );

      /* look for ../../lib */
      
      pathv[pathc - 1] = "../../lib";
      Tcl_JoinPath(pathc,pathv,&ds);
      if ( stat(ds.string, &stat_buf) == -1 ) {
	      Tcl_DStringFree( &ds );
	      Tcl_Free((char*)pathv);
	      /* lib not found */
      	return;
      }
    }
    /* lib has being found */
    
    Tcl_SetVar(interp, "toltk_library", ds.string, TCL_GLOBAL_ONLY);

    /* set TCL_LIBRARY */

    Tcl_DStringInit(&ds1);
    pathv1[0] = ds.string;
    pathv1[1] = "tcl8.4";
    Tcl_JoinPath(2, pathv1, &ds1);
    sys_res = setenv("TCL_LIBRARY", ds1.string, 1);
    Tcl_SetVar(interp, "tcl_library", ds1.string, TCL_GLOBAL_ONLY);
	Tcl_VarEval(interp, "set tcl_library", NULL);

    /* set TK_LIBRARY */
    
    Tcl_DStringFree(&ds1);
    pathv1[1] = "tk8.4";
    Tcl_JoinPath(2, pathv1, &ds1);
    sys_res = setenv("TK_LIBRARY", ds1.string, 1);
    Tcl_SetVar(interp, "tk_library", ds1.string, TCL_GLOBAL_ONLY);

    /* set BLT_LIBRARY */

    Tcl_DStringFree(&ds1);
    pathv1[1] = "blt";
    Tcl_JoinPath(2, pathv1, &ds1);
    sys_res = setenv("BLT_LIBRARY", ds1.string, 1);
    Tcl_SetVar(interp, "blt_libPath", ds1.string, TCL_GLOBAL_ONLY);

    Tcl_DStringFree(&ds1);
    Tcl_DStringFree(&ds);
    Tcl_DStringFree(&ds_argv0);
    Tcl_Free((char*)pathv);

    /* now insert init_toltk.tcl  as the first argument */
    new_argv = (char**)Tcl_Alloc(sizeof(char*)*(*argc+1));
    new_argv[0] = (*argv)[0];
    for (i = 1; i < *argc; i++)
      new_argv[i+1] = (*argv)[i];
    /*new_argv[1] = strdup("-");*/
    
    pathv1[0] = Tcl_GetVar(interp, "toltk_library", TCL_GLOBAL_ONLY);
    pathv1[1] = "toltk/init_toltk.tcl";
    Tcl_DStringInit(&ds);
    Tcl_JoinPath(2, pathv1, &ds);
    new_argv[1] = strdup(Tcl_DStringValue(&ds));
    Tcl_DStringFree(&ds);
    ++*argc;
    *argv = new_argv;
  }
}

#ifdef __WIN32__
int WINAPI WinMain( HINSTANCE hInstance,  // handle to current instance
                    HINSTANCE hPrevInstance,  // handle to previous instance
                    LPSTR lpCmdLine,      // pointer to command line
                    int nCmdShow          // show state of window
                  )
{
    char **argv;
    int argc;
    char buffer[MAX_PATH+1];
    char *p;

    /*
     * Increase the application queue size from default value of 8.
     * At the default value, cross application SendMessage of WM_KILLFOCUS
     * will fail because the handler will not be able to do a PostMessage!
     * This is only needed for Windows 3.x, since NT dynamically expands
     * the queue.
     */

    SetMessageQueue(64);

    Tcl_SetPanicProc(WishPanic);

    /*
     * Create the console channels and install them as the standard
     * channels.  All I/O will be discarded until Tk_CreateConsoleWindow is
     * called to attach the console to a text widget.
     */

    consoleRequired = TRUE;

    /*
     * Set up the default locale to be standard "C" locale so parsing
     * is performed correctly.
     */

    setlocale(LC_ALL, "C");
    setargv(&argc, &argv);

    /*
     * Replace argv[0] with full pathname of executable, and forward
     * slashes substituted for backslashes.
     */

    GetModuleFileName(NULL, buffer, sizeof(buffer));
    argv[0] = buffer;
    for (p = buffer; *p != '\0'; p++) {
	if (*p == '\\') {
	    *p = '/';
	}
    }

    return main(argc,argv);

}
 

/*
 *----------------------------------------------------------------------
 *
 * WishPanic --
 *
 *	Display a message and exit.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Exits the program.
 *
 *----------------------------------------------------------------------
 */

void
WishPanic TCL_VARARGS_DEF(char *,arg1)
{
    va_list argList;
    char buf[1024];
    char *format;
    
    format = TCL_VARARGS_START(char *,arg1,argList);
    vsprintf(buf, format, argList);

    MessageBeep(MB_ICONEXCLAMATION);
    MessageBox(NULL, buf, "Fatal Error in Wish",
	    MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
#ifdef _MSC_VER
    DebugBreak();
#endif
    ExitProcess(1);
}
/*
 *-------------------------------------------------------------------------
 *
 * setargv --
 *
 *	Parse the Windows command line string into argc/argv.  Done here
 *	because we don't trust the builtin argument parser in crt0.  
 *	Windows applications are responsible for breaking their command
 *	line into arguments.
 *
 *	2N backslashes + quote -> N backslashes + begin quoted string
 *	2N + 1 backslashes + quote -> literal
 *	N backslashes + non-quote -> literal
 *	quote + quote in a quoted string -> single quote
 *	quote + quote not in quoted string -> empty string
 *	quote -> begin quoted string
 *
 * Results:
 *	Fills argcPtr with the number of arguments and argvPtr with the
 *	array of arguments.
 *
 * Side effects:
 *	Memory allocated.
 *
 *--------------------------------------------------------------------------
 */

static void
setargv(argcPtr, argvPtr)
    int *argcPtr;		/* Filled with number of argument strings. */
    char ***argvPtr;		/* Filled with argument strings (malloc'd). */
{
    char *cmdLine, *p, *arg, *argSpace;
    char **argv;
    int argc, size, inquote, copy, slashes;
    
    cmdLine = GetCommandLine();	/* INTL: BUG */

    /*
     * Precompute an overly pessimistic guess at the number of arguments
     * in the command line by counting non-space spans.
     */

    size = 2;
    for (p = cmdLine; *p != '\0'; p++) {
	if ((*p == ' ') || (*p == '\t')) {	/* INTL: ISO space. */
	    size++;
	    while ((*p == ' ') || (*p == '\t')) { /* INTL: ISO space. */
		p++;
	    }
	    if (*p == '\0') {
		break;
	    }
	}
    }
    argSpace = (char *) Tcl_Alloc(
	    (unsigned) (size * sizeof(char *) + strlen(cmdLine) + 1));
    argv = (char **) argSpace;
    argSpace += size * sizeof(char *);
    size--;

    p = cmdLine;
    for (argc = 0; argc < size; argc++) {
	argv[argc] = arg = argSpace;
	while ((*p == ' ') || (*p == '\t')) {	/* INTL: ISO space. */
	    p++;
	}
	if (*p == '\0') {
	    break;
	}

	inquote = 0;
	slashes = 0;
	while (1) {
	    copy = 1;
	    while (*p == '\\') {
		slashes++;
		p++;
	    }
	    if (*p == '"') {
		if ((slashes & 1) == 0) {
		    copy = 0;
		    if ((inquote) && (p[1] == '"')) {
			p++;
			copy = 1;
		    } else {
			inquote = !inquote;
		    }
                }
                slashes >>= 1;
            }

            while (slashes) {
		*arg = '\\';
		arg++;
		slashes--;
	    }

	    if ((*p == '\0')
		    || (!inquote && ((*p == ' ') || (*p == '\t')))) { /* INTL: ISO space. */
		break;
	    }
	    if (copy != 0) {
		*arg = *p;
		arg++;
	    }
	    p++;
        }
	*arg = '\0';
	argSpace = arg + 1;
    }
    argv[argc] = NULL;

    *argcPtr = argc;
    *argvPtr = argv;
}

#endif
