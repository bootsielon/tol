/* toltcl.cpp: This file implements a Tcl interface to Tol.
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

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <string.h>
#include <gsl/gsl_errno.h>

#include <toltclInt.h>
#include <toltcl.h>
#include <tt_crash.h>

#include <tol/tol_init.h>

Tcl_Interp * TT_interp;

static int Tol_InitKernelCmd _ANSI_ARGS_((ClientData clientData,
					  Tcl_Interp *interp,
					  int objc, Tcl_Obj *CONST objv[]));

static int Tol_InitLibraryCmd _ANSI_ARGS_((ClientData clientData,
                                           Tcl_Interp *interp,
                                           int objc, Tcl_Obj *CONST objv[]));

static int Tol_IncludeCmd _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp,
                                       int objc, Tcl_Obj *CONST objv[]));

static int Tol_DecompileCmd _ANSI_ARGS_((ClientData clientData,
                                         Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[]));

static int Tol_ConsoleCmd _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp,
                                       int objc, Tcl_Obj *CONST objv[]));

static int Tol_ForAllChildCmd _ANSI_ARGS_((ClientData clientData,
                                           Tcl_Interp *interp,
                                           int objc, Tcl_Obj *CONST objv[]));

static int Tol_ForEachCmd _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp,
                                       int objc, Tcl_Obj *CONST objv[]));

static int Tol_TableSetCmd _ANSI_ARGS_((ClientData clientData,
                                      Tcl_Interp *interp,
                                      int objc, Tcl_Obj *CONST objv[]));

static int Tol_TableMatCmd _ANSI_ARGS_((ClientData clientData,
                                      Tcl_Interp *interp,
                                      int objc, Tcl_Obj *CONST objv[]));

static int Tol_TableCmd _ANSI_ARGS_((int ismat, ClientData clientData,
                                     Tcl_Interp *interp,
                                     int objc, Tcl_Obj *CONST objv[]));

static int Tol_GetSerieCmd _ANSI_ARGS_((ClientData clientData,
                                        Tcl_Interp *interp,
                                        int objc, Tcl_Obj *CONST objv[]));

static int Tol_SerieGrpCmd _ANSI_ARGS_((ClientData clientData,
                                        Tcl_Interp *interp,
                                        int objc, Tcl_Obj *CONST objv[]));

static int Tol_LanguageCmd _ANSI_ARGS_((ClientData clientData,
                                        Tcl_Interp *interp,
                                        int objc, Tcl_Obj *CONST objv[]));

static int Tol_InfoCmd _ANSI_ARGS_((ClientData clientData,
                                    Tcl_Interp *interp,
                                    int objc, Tcl_Obj *CONST objv[]));

static int Tol_InfoStruct _ANSI_ARGS_((Tcl_Interp *interp,
                                       int objc, Tcl_Obj *CONST objv[],
                                       Tcl_Obj * obj_result));

static int Tol_InfoPath _ANSI_ARGS_((Tcl_Interp *interp,
                                     int objc, Tcl_Obj *CONST objv[],
                                     Tcl_Obj * obj_result));

static int Tol_InfoGrammars _ANSI_ARGS_((Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[],
                                         Tcl_Obj * obj_result));

static int Tol_InfoPackages _ANSI_ARGS_((Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[],
                                         Tcl_Obj * obj_result));

static int Tol_InfoFunctions _ANSI_ARGS_((Tcl_Interp *interp,
                                          int objc, Tcl_Obj *CONST objv[],
                                          Tcl_Obj * obj_result));

static int Tol_InfoVariables _ANSI_ARGS_((Tcl_Interp *interp,
                                          int objc,Tcl_Obj *CONST objv[],
                                          Tcl_Obj * obj_result));

static int Tol_GetMatrixCmd _ANSI_ARGS_((ClientData clientData,
                                         Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[]));

static int Tol_TimeSetCmd _ANSI_ARGS_((ClientData clientData,
                                       Tcl_Interp *interp,
                                       int objc, Tcl_Obj *CONST objv[]));

static int Tol_DateCmd _ANSI_ARGS_((ClientData clientData,
                                    Tcl_Interp *interp,
                                    int objc, Tcl_Obj *CONST objv[]));

static int Tol_AutoCorrCmd _ANSI_ARGS_((ClientData clientData,
                                        Tcl_Interp *interp,
                                        int objc, Tcl_Obj *CONST objv[]));

static int Tol_SerieStatCmd _ANSI_ARGS_((ClientData clientData,
                                         Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[]));

static int Tol_StopEvalCmd  _ANSI_ARGS_((ClientData clientData,
                                         Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[]));

static int Tol_LCFormatCmd  _ANSI_ARGS_((ClientData clientData,
                                         Tcl_Interp *interp,
                                         int objc, Tcl_Obj *CONST objv[]));

DLLIMPORT gsl_error_handler_t * Tol_gsl_set_error_handler (gsl_error_handler_t * new_handler);
DLLIMPORT const char * Tol_gsl_strerror (const int gsl_errno);

/*
 * TT_gsl_error_handler --
 *
 *    Tcl hook to a GSL error. This should be used only to detect where the gsl
 *    errors are reported. 
 */

static void TT_gsl_error_handler(const char * reason, const char * file, int line,
				 int gsl_errno)
{
  Tcl_CmdInfo cmd_info;
  int status;

  if (TT_interp && Tcl_GetCommandInfo(TT_interp,"::tol::gsl_error_handler",&cmd_info) ) {
    Tcl_DString dstr;
    Tcl_Obj * objv[5];
    
    Tcl_DStringInit(&dstr);
    objv[0] = Tcl_NewStringObj("::tol::gsl_error_handler",-1);
    Tcl_ExternalToUtfDString(NULL, reason, -1, &dstr);
    objv[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), Tcl_DStringLength(&dstr));
    Tcl_DStringFree(&dstr);
    Tcl_ExternalToUtfDString(NULL, file, -1, &dstr);
    objv[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), Tcl_DStringLength(&dstr));
    Tcl_DStringFree(&dstr);
    objv[3] = Tcl_NewIntObj(line);
    Tcl_ExternalToUtfDString(NULL, Tol_gsl_strerror(gsl_errno), -1, &dstr);
    objv[4] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), Tcl_DStringLength(&dstr));
    Tcl_DStringFree(&dstr);
    status = Tcl_EvalObjv(TT_interp, 5, objv, TCL_EVAL_GLOBAL);
    if (status != TCL_OK) {
      printf("%s in file %s at line %d\n. str_ernno : %s",
	     reason, file, line, Tol_gsl_strerror(gsl_errno));
      printf("interp : %s\n", TT_interp->result);
    }
  }  
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InitKernelCmd --
 *
 *        Implements the new Tcl "::tol::initkernel ?lang ?vmode??" command.
 *        
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InitKernelCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;     /* Current interpreter */
     int objc;               /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  int lang;
  char* vmode;
  if (objc > 3) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
                     Tcl_GetString(objv[0]),"?lang ?vmode??\"", NULL);
    return TCL_ERROR;
  }
  lang = (objc == 1 || !strcasecmp("es", Tcl_GetString(objv[1])));
  vmode = objc == 3 ? Tcl_GetString(objv[2]) :  NULL;

  if ( !TOLHasBeenInitialized( ) ) {
    printf( "TOL no esta inicializado\n" );
    /*
     * Initialize kernel
     */
    
    InitTolKernel(Tcl_GetNameOfExecutable(), lang, vmode&&vmode[0]?vmode:NULL);
    
    /*
     * this should only be called to find out where the GSL errors are generated
     */
    
    Tol_gsl_set_error_handler(&TT_gsl_error_handler);
  } else {
    printf( "TOL esta inicializado\n" );
  }
  
  /*
   * Install tol::* commands
   */

  Tcl_CreateObjCommand( interp, "::tol::initlibrary", Tol_InitLibraryCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::include", Tol_IncludeCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::decompile", Tol_DecompileCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::info", Tol_InfoCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::language", Tol_LanguageCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::console", Tol_ConsoleCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::forallchild", Tol_ForAllChildCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::foreach", Tol_ForEachCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::tableset", Tol_TableSetCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::tablematrix", Tol_TableMatCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::getserie", Tol_GetSerieCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::seriegrp", Tol_SerieGrpCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::matrix", Tol_GetMatrixCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::timeset", Tol_TimeSetCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::date", Tol_DateCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::autocor", Tol_AutoCorrCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::seriestat", Tol_SerieStatCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
  
  Tcl_CreateObjCommand( interp, "::tol::stop", Tol_StopEvalCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );

  Tcl_CreateObjCommand( interp, "::tol::lc_format", Tol_LCFormatCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
  return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InitLibraryCmd --
 *
 *        Implements the new Tcl "::tol::initlibrary" command.
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InitLibraryCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;     /* Current interpreter */
     int objc;               /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  int loadIni;
  int status;

  if ( objc != 2 ) {
    Tcl_AppendResult(interp, "wrong # args: should be \"",
                     Tcl_GetString(objv[0]), " 0|1\"", NULL);
    return TCL_ERROR;
  }
  
  if ( (status = Tcl_GetIntFromObj( interp, objv[1], &loadIni )) == TCL_OK ) {
    LoadInitLibrary( loadIni );
  }
  return status;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_IncludeCmd --
 *
 *       Implements the new Tcl "::tol::include" command.
 *
 * Results:
 *      A standard Tcl result
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_IncludeCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc != 2) {
    Tcl_SetStringObj(obj_result,
                     "wrong # args: should be \"::tol::include tolfile\"",
                     -1);
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_IncludeFile(interp,objv[1],obj_result);
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_DecompileCmd --
 *
 *       Implements the new Tcl "::tol::decompile" command.
 *
 * Results:
 *      A standard Tcl result
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_DecompileCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc != 2) {
    Tcl_AppendStringsToObj(obj_result, 
                           "wrong # args: should be \'",
                           Tcl_GetString(objv[0]), " tolfile\'", NULL);
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_DecompileFile(interp,objv[1],obj_result);
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_ConsoleCmd --
 *
 *  Implements the new Tcl "::tol::console" command.
 *
 *    ::tol::console eval tol_expr
 *
 *        evaluate a tol_expr, if the result has name then it will be
 *        stored in the console's stack.
 *
 *    ::tol::console stack list [obj_name]
 *
 *        return the content of obj_name. If no object is given then
 *        list the set object contained in the stack.
 *
 *    ::tol::console stack release [obj_name]
 *
 *        release the specified object contained in the console's
 *        stack. If no object is given then the whole stack is
 *        released.
 *
 * Results:
 *      A standard Tcl result
 *
 * Side effects:
 *      None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_ConsoleCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc < 3) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]), " option arg ?arg?'",NULL);
    tcl_result = TCL_ERROR;
  } else {
    Tcl_DString dstr;
    Tcl_DStringInit(&dstr);
    Tcl_UtfToExternalDString(NULL,Tcl_GetString(objv[1]),-1,&dstr);
    if (!strncmp(dstr.string,"eval",dstr.length)) {
      if (objc != 3 && objc != 4) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " eval ?-hidden? tol_expr'",NULL);
        tcl_result = TCL_ERROR;
      } else {
        Tcl_Obj *obj_expr = NULL;
        int hidden = 0;

        if (objc==4) {
          if (strcmp(Tcl_GetString(objv[2]),"-hidden")) {
            Tcl_AppendStringsToObj(obj_result,
                                   "invalid option ",
                                   Tcl_GetString(objv[2]),
                                   ": should be -hidden",
                                   NULL);
            tcl_result = TCL_ERROR;
          } else {
            hidden = 1;
            obj_expr = objv[3];
          }
        } else {
          obj_expr = objv[2];
        }
        if (obj_expr) {
          tcl_result = Tol_EvalExpr(obj_expr, hidden, obj_result);
        }
      }
      Tcl_DStringFree(&dstr);
    } else if (!strncmp(dstr.string,"stack",dstr.length)) {
      Tcl_DStringFree(&dstr);
      Tcl_UtfToExternalDString(NULL,Tcl_GetString(objv[2]),-1,&dstr);
      if (!strncmp(dstr.string,"list",dstr.length))
        tcl_result = Tol_ListStack(interp,objc-3,objv+3,obj_result);
      else if (!strncmp(dstr.string,"release",dstr.length))
        tcl_result = Tol_ReleaseStack(interp,objc-3,objv+3,obj_result);
      else {
        Tcl_AppendStringsToObj(obj_result,
                               "invalid option '",
                               Tcl_GetString(objv[2]),
                               "', should be: list or release",NULL);
        tcl_result = TCL_ERROR;
      }
      Tcl_DStringFree(&dstr);
    } else {
      Tcl_AppendStringsToObj(obj_result,
                             "invalid option '",
                             Tcl_GetString(objv[1]),
                             "', should be: eval or stack",NULL);
      tcl_result = TCL_ERROR;
      Tcl_DStringFree(&dstr);
    }
  }
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_ForAllChildCmd --
 *
 *        Implements the new Tcl "::tol::forallchild" command.
 *
 *  ::tol::forallchild setName ?indexes? script
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_ForAllChildCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc != 3) {
    Tcl_AppendStringsToObj(obj_result, "wrong # args: should be '",
                           Tcl_GetString(objv[0]), " setref script'", NULL);
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_IterChildren(interp,objc-1,objv+1,obj_result);
  Tcl_SetObjResult( interp, obj_result );
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_ForEachCmd --
 *
 *        Implements the new Tcl "::tol::foreach" command.
 *
 *  ::tol::foreach varname <Set or NameBlock reference> script
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_ForEachCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc != 4) {
    Tcl_AppendStringsToObj(obj_result, "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " varName setref script'", NULL);
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_ForEach( interp, objc - 1, objv + 1, obj_result );
  Tcl_SetObjResult( interp, obj_result );
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_GetSerie --
 *
 *        Implements the new Tcl "::tol::getserie" command.
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_GetSerieCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if (objc < 2 || objc > 4) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " sername ?begin end?'", NULL );
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_GetSerieContent(interp,objc-1,objv+1,obj_result);
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tol_TableCmd --
 *
 *     Implements the new Tcl "::tol::tableset" command.
 *     Is an internal interface to the commands tol::tableset
 *     & tol::tablematrix.
 * 
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_TableCmd(ismat, clientData, interp, objc, objv)
  int ismat;
  ClientData clientData;  /* Not used. */
  Tcl_Interp *interp;     /* Current interpreter */
  int objc;               /* Number of arguments */
  Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if ( objc < 3 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " option arg ?arg?'", NULL );
    tcl_result = TCL_ERROR;
  } else {
    char * option = Tcl_GetString(objv[1]);
    int length = strlen(option);
    if (!strncmp("create",option,length)) {
      if ( objc < 4 ) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " create tblName (set|matrix)'", NULL );
        tcl_result = TCL_ERROR;
      } else {     
        tcl_result = Tol_CreateTable(interp,objv[2],objv[3], ismat, obj_result);
        if (tcl_result == TCL_ERROR)
          Tcl_AppendStringsToObj(obj_result,
                                 ": could not create '",
                                 Tcl_GetString(objv[2]),"'",NULL);
      }
    } else if (!strncmp("destroy",option,length))
      if ( objc < 3) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " destroy tblName ?args?'", NULL );
        tcl_result = TCL_ERROR;
      } else {
        tcl_result = Tol_DestroyTable( interp, objc-2, objv+2, obj_result);
      }
    else {
      Tcl_AppendStringsToObj(obj_result,
                             "bad option '",
                             option, "': must be create or destroy", NULL);
      tcl_result = TCL_ERROR;
    }
  }

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}

/*
 *----------------------------------------------------------------------
 *
 * Tol_TableSetCmd --
 *
 *     Implements the new Tcl "::tol::tableset" command.
 *  
 *     ::tol::tableset option arg ?arg?
 *
 *     ::tol::tableset ?create? tblName setReference
 *
 *     ::tol::tableset destroy tblName ?grpname ...?
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_TableSetCmd(clientData, interp, objc, objv)
  ClientData clientData;  /* Not used. */
  Tcl_Interp *interp;     /* Current interpreter */
  int objc;               /* Number of arguments */
  Tcl_Obj *CONST objv[];  /* Argument objects */
{
  return Tol_TableCmd(0, clientData, interp, objc, objv);
}

/*
 *----------------------------------------------------------------------
 *
 * Tol_TableMatCmd --
 *
 *     Implements the new Tcl "::tol::tablematrix" command.
 *  
 *     ::tol::tablematrix option arg ?arg?
 *
 *     ::tol::tablematrix ?create? tblName setReference
 *
 *     ::tol::tablematrix destroy tblName ?grpname ...?
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_TableMatCmd(clientData, interp, objc, objv)
  ClientData clientData;  /* Not used. */
  Tcl_Interp *interp;     /* Current interpreter */
  int objc;               /* Number of arguments */
  Tcl_Obj *CONST objv[];  /* Argument objects */
{
  return Tol_TableCmd(1, clientData, interp, objc, objv);
}

/*
 *----------------------------------------------------------------------
 *
 * Tol_SerieGrpCmd --
 *
 *     Implements the new Tcl "::tol::seriegrp" command.
 *  
 *     ::tol:seriegrp option arg ?arg?
 *
 *     ::tol::seriegrp ?create? grpName ?range? serName ?serName? 
 *
 *     ::tol::seriegrp destroy grpName ?grpName?
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_SerieGrpCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                       /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result= Tcl_NewObj();
  int tcl_result;
  
  if ( objc < 3 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]), " option arg ?arg?'", NULL );
    tcl_result = TCL_ERROR;
  } else {
    char * option = Tcl_GetString(objv[1]);
    int length = strlen(option);
    if (!strncmp("create",option,length)) {
      if ( objc == 3 ) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " create grpName ?-range range? serName ?serName?'", NULL );
        tcl_result = TCL_ERROR;
      } else {
        tcl_result = Tol_CreateSerieGrp(interp,objc-2,objv+2,obj_result);
        if (tcl_result == TCL_ERROR)
          Tcl_AppendStringsToObj(obj_result,
                                 ": could not create '",
                                 Tcl_GetString(objv[2]),"'",NULL);
      }
    } else if (!strncmp("destroy",option,length))
      tcl_result = Tol_DestroySerieGrp(interp,objc-2,objv+2,obj_result);
    else {
      Tcl_AppendStringsToObj(obj_result,
                             "bad option '",
                             option, "': must be create or destroy", NULL);
      tcl_result = TCL_ERROR;
    }
  }

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_LanguageCmd --
 *
 *        Implements the new Tcl "::tol::language" command.
 *
 *          ::tol::language ?lang?
 *
 * Results:
 *
 *       Set or return the language, as required by TOL only english
 *       or spanish.
 *
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_LanguageCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;     /* Current interpreter */
     int objc;               /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;

  if ( objc > 2 ) {
    Tcl_AppendStringsToObj(obj_result,"wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " ?lang?'", NULL);
    tcl_result = TCL_ERROR;
  } else {
    if ( objc == 2 )
      tcl_result = Tol_SetLanguage( interp, objv[1], obj_result );
    else
      tcl_result = Tol_GetLanguage( interp, obj_result );
  }
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoCmd --
 *
 *        Implements the new Tcl "::tol::info" command.
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InfoCmd(clientData, interp, objc, objv)
     ClientData clientData;  /* Not used. */
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;
  char * arg;
  int length;
  
  if ( objc == 1) {
    Tcl_AppendStringsToObj( obj_result, "wrong # args: should be '",
                      Tcl_GetString(objv[0]),
                      " ?arg arg ...?'", NULL );
    tcl_result = TCL_ERROR;
  } else {
    arg = Tcl_GetString(objv[1]);
    length = strlen(arg);
    if ( !strncasecmp("version", arg, length)) {
      if ( objc == 2 )
        tcl_result = Tol_InfoVersion(interp, obj_result);
      else {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0])," version'",
                               NULL);
        tcl_result = TCL_ERROR;
      }
    } else if (!strncasecmp("reference", arg, length)) {
//      if (objc > 4 || objc < 3) {
      if (objc != 3) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
//                               " reference ?grammar? obj'",
                               " reference {grammar|container objname ?indexes?}'",
                               NULL);
        tcl_result = TCL_ERROR;	
      } else
	tcl_result = Tol_GetReference(interp, objc-2, objv+2, obj_result);
    } else if (!strncasecmp("address", arg, length)) {
      if (objc != 3) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " address {grammar|container objname ?indexes?}'",
                               NULL);
        tcl_result = TCL_ERROR;	
      } else {
	tcl_result = Tol_GetAddressFromObject( interp,
                                              objv[2], obj_result );
      }
    } else if (!strncasecmp("anything", arg, length)) {
      if (objc != 3) {
        Tcl_AppendStringsToObj(obj_result,
                               "wrong # args: should be '",
                               Tcl_GetString(objv[0]),
                               " anything {grammar|container objname ?indexes?}'",
                               NULL);
        tcl_result = TCL_ERROR;	
      } else {
	tcl_result = Tol_GetAnythingType( interp,
                                          objv[2], obj_result );
      }
    } else if (!strncasecmp("included", arg, length)) {
      if (objc == 2)
        tcl_result = Tol_InfoIncluded(interp, obj_result);
      else if (objc == 3)
        tcl_result = Tol_InfoFile(interp,objv[2],obj_result);
      else {
        Tcl_AppendStringsToObj(obj_result,"wrong # args: should be '",
                               Tcl_GetString(objv[0]), " include'", NULL);
        tcl_result = TCL_ERROR;
      } 
    } else if (!strncasecmp("functions", arg, length))
      tcl_result = Tol_InfoFunctions(interp, objc-1, objv+1, obj_result);
    else if (!strncasecmp("grammars", arg, length))
      tcl_result = Tol_InfoGrammars(interp, objc-1, objv+1, obj_result);
    else if (!strncasecmp("packages", arg, length))
      tcl_result = Tol_InfoPackages(interp, objc-1, objv+1, obj_result);
    else if (!strncasecmp("variables", arg, length))
      tcl_result = Tol_InfoVariables(interp, objc-1, objv+1, obj_result);
    else if (!strncasecmp("struct", arg, length))
      tcl_result = Tol_InfoStruct(interp, objc-1, objv+1, obj_result);
    else if (!strncasecmp("path", arg, length))
      tcl_result = Tol_InfoPath(interp, objc-1, objv+1, obj_result);
    else {
      Tcl_SetStringObj( obj_result, "bad option '", -1);
      Tcl_AppendStringsToObj( obj_result, Tcl_GetString( objv[1] ),
                              "' : must be included, functions, grammars, variables, struct, path or version", NULL );
      tcl_result = TCL_ERROR;
    }
  }
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoStruct --
 *
 *        Implements the new Tcl "::tol::info struct" subcommand.
 *
 *  ::tol::info struct ?name?
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int Tol_InfoStruct(interp, objc, objv, obj_result)
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                       /* Number of arguments */
     Tcl_Obj *CONST objv[];          /* Argument objects */
     Tcl_Obj * obj_result;           /* subcommand's result */
{
  if ( objc > 2 ) {
    Tcl_AppendStringsToObj( obj_result, "wrong # args: should be '",
                      Tcl_GetString(objv[0]),
                      " struct ?struct-name?'", NULL);
    return TCL_ERROR;
  }
  if ( objc == 1 ) {
    Tcl_AppendStringsToObj(obj_result, "wrong # args: '",
                           Tcl_GetString(objv[0]),
                           " struct\' not implemented yet",
                           NULL);
    return TCL_ERROR;
  }
  return Tol_GetStructure( interp, objv[1], obj_result );
/*  return  objc == 1 ? Tol_GetAllStructures( interp, obj_result ) :
    Tol_GetStructure( interp, objv[1], obj_result ); */
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoPath --
 *
 *        Implements the new Tcl "::tol::info path" subcommand.
 *
 *  ::tol::info path set-reference
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int Tol_InfoPath(interp, objc, objv, obj_result)
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                       /* Number of arguments */
     Tcl_Obj *CONST objv[];          /* Argument objects */
     Tcl_Obj * obj_result;           /* subcommand's result */
{
  if ( objc != 2 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " path set'", NULL);
    return TCL_ERROR;
  }
  return Tol_GetSetPath(interp, objv[1], obj_result);
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoFunctions --
 *
 *        Implements the subcommand "::tol::info functions ?function?".
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InfoFunctions(interp, objc, objv, obj_result)
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
     Tcl_Obj * obj_result;   /* subcommand's result */
{
  if ( objc < 2 || objc > 3 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " functions graname ?funname?'", NULL);
    return TCL_ERROR;
  }
  return  objc == 2 ? Tol_SetFunctionsObj(interp, objv[1], obj_result) :
    Tol_SetFunctionInfoObj(objv[1], objv[2], obj_result);
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoGrammars --
 *
 *        Implements the subcommand "::tol::info grammars ?grammar?".
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InfoGrammars(interp, objc, objv, obj_result)
     Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
     Tcl_Obj * obj_result;
{
  if ( objc > 2 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]), " grammars ?graname?'",
                           NULL);
    return TCL_ERROR;
  }
  return  objc == 1 ? Tol_SetGrammarsObj(interp, obj_result) :
    Tol_SetGrammarDescObj(interp, objv[1], obj_result);
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoGrammars --
 *
 *        Implements the subcommand "::tol::info grammars ?grammar?".
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InfoPackages(interp, objc, objv, obj_result)
     Tcl_Interp *interp;       /* Current interpreter */
     int objc;                 /* Number of arguments */
     Tcl_Obj *CONST objv[];    /* Argument objects */
     Tcl_Obj * obj_result;
{
  if ( objc > 2 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           Tcl_GetString(objv[0]), " packages ?pkgName?'",
                           NULL);
    return TCL_ERROR;
  }
  return  objc == 1 ? Tol_SetPackagesObj(interp, obj_result) :
    Tol_SetPackageDescObj(interp, objv[1], obj_result);
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_InfoVariables --
 *
 *        Implements the new Tcl "::tol::info variables" command.
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_InfoVariables(interp, objc, objv, obj_result)
Tcl_Interp *interp;             /* Current interpreter */
     int objc;                                 /* Number of arguments */
     Tcl_Obj *CONST objv[];  /* Argument objects */
     Tcl_Obj * obj_result;
{
//  if ( objc < 2 || objc > 3 ) {
  if ( objc != 2 ) {
    Tcl_AppendStringsToObj(obj_result,
                     "wrong # args: should be '",
                     Tcl_GetString(objv[0]),
                     " {grammar|container ?objname? ?indexes?}",
                     NULL);
    return TCL_ERROR;
  }
//  return  objc == 2 ? Tol_SetVariablesObj(interp, objv[1], obj_result) :
//    Tol_SetVariableInfoObj(objv[1], objv[2], obj_result);
  return  Tol_SetVariablesObj(interp, objv[1], obj_result);
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_GetMatrixCmd --
 *
 *        Implements the new Tcl "::tol::matrix" command.
 *
 * Results:
 *       {matname rows colums {<data>}}
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int
Tol_GetMatrixCmd(ClientData  clientData, /* Not used. */
                 Tcl_Interp *interp,     /* Current interpreter */
                 int objc,               /* Number of arguments */
                 Tcl_Obj *CONST objv[])  /* Argument objects */
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;

  if ( objc != 2) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                            Tcl_GetString(objv[0]),
                            " matrix\"", NULL );
    tcl_result = TCL_ERROR;
  } else {
    tcl_result = Tol_GetMatrixContent(interp,objv[1],obj_result);
  }
  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_TimeSetCmd --
 *
 *        Implements the new Tcl "::tol::timeset" command.
 *
 *       tol::timeset create tms toltms
 *       tol::timeset destroy tms ?tms?
 *
 *       tms start date
 *       tms get
 *       tms next
 *       tms previous
 * 
 * Results:
 *       the new tms command
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

int Tol_TimeSetCmd ( ClientData clientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *CONST objv[] )
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;

  char * cmdname = Tcl_GetString(objv[0]);

  if ( objc < 3 ) {
    Tcl_AppendStringsToObj(obj_result,
                           "wrong # args: should be '",
                           cmdname,
                           " option arg ?arg?'", NULL );
    tcl_result = TCL_ERROR;
  } else {
    char * option = Tcl_GetString(objv[1]);
    int optlen = strlen(option);

    if ( !strncmp(option, "create", optlen) ) {
      if ( objc != 4 ) {
        Tcl_AppendStringsToObj( obj_result, "wrong # args: should be '",
                                cmdname, " ", option,
                                " tms toltms'", NULL );
        tcl_result = TCL_ERROR;
      } else
        tcl_result = Tol_CreateTMSCmd( interp, objv[2], objv[3], obj_result );
    }
    else if ( !strncmp(option, "destroy", optlen) )
      tcl_result = Tol_DestroyTMSCmd( interp, objc-2, objv+2, obj_result );
    else {
      Tcl_AppendStringsToObj(obj_result, "invalid option name '", option,
                              "', should be: create or destroy", NULL);
      tcl_result = TCL_ERROR;
    }
  }

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_DateCmd --
 *
 *        Implements the new Tcl "::tol::date" command.
 *
 *       tol::date first              -> default first date
 *       tol::date last               -> default last  date
 *       tol::date now     ?fmt?      -> today date & time
 *       tol::date weekday date       -> 0(sunday), 1(monday), ..., 6(saturday)
 *       tol::date days    year month -> number of days of month/year
 *       tol::date +day    date ?n?   -> date ++ ?n days?
 *       tol::date +month  date ?n?   -> date ++ ?n months?
 *
 * Results:
 *       a valid date
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int Tol_DateCmd ( ClientData clientData, Tcl_Interp *interp,
                         int objc, Tcl_Obj *CONST objv[] )
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;

  if ( objc < 2 || objc > 4 ) {
    char * cmdname = Tcl_GetString(objv[0]);
    Tcl_AppendStringsToObj(obj_result, "wrong # args: should be \n",
                           cmdname, " first,\n",
                           cmdname, " last,\n",
                           cmdname, " now ?fmt?,\n",
                           cmdname, " weekday,\n",
                           cmdname, " days year month,\n",
                           cmdname, " +day date ?n?, or\n",
                           cmdname, " +month date ?n?", NULL);
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_DateOption(interp, objc, objv, obj_result);

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_AutoCorCmd --
 *
 *        Implements the new Tcl "::tol::autocor" command.
 *
 *       tol::autocor type serie ?lagsize?
 *
 * Results:
 *       {sername sigma size {data of autocorrelation}}
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static int Tol_AutoCorrCmd (ClientData clientData, Tcl_Interp *interp,
                            int objc, Tcl_Obj *CONST objv[])
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;


  if ( objc < 3 || objc > 4 ) {
    Tcl_AppendStringsToObj( obj_result, "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " type serie ?lagsize?'", NULL );
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_GetAutoCorr( interp, objc-1, objv+1,
                                  obj_result ); 

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Tol_SerieStatCmd --
 *
 *       Implements the new Tcl "::tol::seriestat" command.
 *
 *       tol::seriestat serieref      
 *           -> {serie-name {list of stats names & values}}
 *
 *       tol::seriestat serieref statname ?args?
 *            -> return the stat value. (not implemented yet)
 *
 * Results:
 *       
 *
 * Side effects:
 *       None.
 *
 *----------------------------------------------------------------------
 */

static
int Tol_SerieStatCmd ( ClientData clientData, Tcl_Interp *interp,
                       int objc, Tcl_Obj *CONST objv[] )
{
  Tcl_Obj * obj_result = Tcl_NewObj();
  int tcl_result;

  if ( objc < 2 ) {
    Tcl_AppendStringsToObj( obj_result, "wrong # args: should be '",
                           Tcl_GetString(objv[0]),
                           " serieref ?statname ?args??'", NULL );
    tcl_result = TCL_ERROR;
  } else
    tcl_result = Tol_ComputeSerieStat(interp,objc-1,objv+1,obj_result); 

  Tcl_SetObjResult(interp,obj_result);
  return tcl_result;
}

/*
 * Tol_StopEvalCmd --
 *
 * Implement the command tol::stop which try to stop the current evaluation
 * flow inside Evaluate. 
 * 
 */
static
int Tol_StopEvalCmd (ClientData clientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *CONST objv[])
{
  if ( objc != 1 ) {
    Tcl_AppendResult(interp, "wrong # args: should be '",
                     Tcl_GetString(objv[0]),  "'", NULL);
    return TCL_ERROR;
  }
  return Tol_StopEvaluation();
}

/*
 * Tol_LCFormatCmd --
 *
 * Implement the command tol::lc_format which format a double given
 * the format of reals inside TOL.
 * 
 */
static
int Tol_LCFormatCmd (ClientData clientData, Tcl_Interp *interp,
                     int objc, Tcl_Obj *CONST objv[])
{
  Tcl_Obj * obj_result;
  int tcl_result;

  if ( objc != 2 ) {
    Tcl_AppendResult( interp, "wrong # args: should be '",
                      Tcl_GetString(objv[0]),  " double_value'",
                      NULL );
    tcl_result = TCL_ERROR;
  } else {
    obj_result = Tcl_NewObj();
    tcl_result = Tol_LCFormat(interp,objv[1],obj_result);
    Tcl_SetObjResult(interp, obj_result);
  }
  return tcl_result;
}


/*
 *----------------------------------------------------------------------
 *
 * Toltcl_Init --
 *
 *       Initialize the new package.  The string "Toltcl" in the
 *       function name must match the PACKAGE declaration at the top of
 *       configure.in.
 *
 * Results:
 *       A standard Tcl result
 *
 * Side effects:
 *       The TolTcl package is created.
 *      The commands: ::tol::grammars
 *
 *----------------------------------------------------------------------
 */

int
Toltcl_Init(Tcl_Interp *interp)
{
  /* Inicializo la captura de segfault */

  InitCrashHandler();
  
  TT_interp = interp;

  if (Tcl_InitStubs(interp, "8.3", 0) == NULL) {
    return TCL_ERROR;
  }
  if (Tcl_PkgRequire(interp, "Tcl", "8.3", 0) == NULL) {
    return TCL_ERROR;
  }
  if (Tcl_PkgProvide(interp, "Toltcl", TOLTCL_VERSION) != TCL_OK) {
    return TCL_ERROR;
  }
  
  /*
   *  Create the commands
   *
   */
  
  Tcl_CreateObjCommand( interp, "::tol::initkernel", Tol_InitKernelCmd,
                        (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL );
  
  /*
   *  Inititialize TOL
   *
   */
  
  //InitTolKernel();
  Tol_InstallHciWriter();
  Tol_InstallMethodsHook();
 
  return TCL_OK;
}

#ifdef __WIN32__
#ifndef STATIC_BUILD

#include <windows.h>

/*
 * The following declaration is for the VC++ DLL entry point.
 */

BOOL APIENTRY           DllMain(HINSTANCE hInst, DWORD reason, 
                                LPVOID reserved);
/*
 *----------------------------------------------------------------------
 *
 * DllMain --
 *
 *      This routine is called by the VC++ C run time library init
 *      code, or the DllEntryPoint routine.
 *
 * Results:
 *      TRUE on sucess, FALSE on failure.
 *
 * Side effects:
 *
 *----------------------------------------------------------------------
 */

BOOL APIENTRY
DllMain(hInst, reason, reserved)
     HINSTANCE hInst;    /* Library instance handle. */
     DWORD reason;       /* Reason this function is being called. */
     LPVOID reserved;    /* Not used. */
{
  switch (reason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE; 
}

#endif /* !STATIC_BUILD */
#endif /* __WIN32__ */
