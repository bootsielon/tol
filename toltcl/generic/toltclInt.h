/* toltclInt.h - Declarations of things used internally by the
                 toltcl extension.
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

#ifndef TOLTCLINT_H
#define TOLTCLINT_H 1

#include <tt_common.h>
#include <tcl.h>

#ifndef __GNUC__
#if !defined(strncasecmp)
#    define strncasecmp _strnicmp
#endif
#if !defined(strcasecmp)
#    define strcasecmp  _stricmp
#endif
#if !defined(isinf)
#    define isinf       !_finite
#endif
#endif


BEGIN_C_DECLS


extern Tcl_Interp * TT_interp;

/*! Fill in obj_result the version info corresponding to tol
 *   {major MajorNum minor MinorNum release ReleaseNum coment Comment}
 */
extern int Tol_InfoVersion _ANSI_ARGS_((Tcl_Interp * interp,
                                        Tcl_Obj * obj_result));

/*! Set the language to be used  by TOL. Fill in obj_result the previous
 *  language.
 */
extern int Tol_SetLanguage _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Obj * lang,
                                        Tcl_Obj * obj_result));

/*! Return the language used  by TOL. Fill in obj_result the current
 *  language.
 */
extern int Tol_GetLanguage _ANSI_ARGS_((Tcl_Interp * interp,
                                        Tcl_Obj * obj_result));

/*! Fill in obj_result the result of evaluating including the file
 *  given as argument: the absolute path.
 */
extern int Tol_IncludeFile _ANSI_ARGS_((Tcl_Interp * interp,
                                        Tcl_Obj * obj_file,
                                        Tcl_Obj * obj_result));

extern int Tol_DecompileFile _ANSI_ARGS_((Tcl_Interp * interp,
                                          Tcl_Obj * obj_file,
                                          Tcl_Obj * obj_result));

extern int Tol_InfoFile _ANSI_ARGS_((Tcl_Interp * interp,
                                     Tcl_Obj * file,
                                     Tcl_Obj * obj_result));

/*! Fill in obj_result the result of evaluating in TOL the given
 *  string.
 */
extern int Tol_EvalExpr _ANSI_ARGS_((Tcl_Obj * obj_expr,
                                     int hidden,
                                     Tcl_Obj * obj_result));

/*! Fill in obj_result the content of the given tol object's names. The
 *  object are searched in the console's stack.
 */
extern int Tol_ListStack _ANSI_ARGS_((Tcl_Interp * interp,
                                      int objc,
                                      Tcl_Obj *CONST objv[],
                                      Tcl_Obj * obj_result));

/*! Release the given tol object's names. The object are searched in
 *  the console's stack.
 */
extern int Tol_ReleaseStack _ANSI_ARGS_((Tcl_Interp * interp,
                                         int objc,
                                         Tcl_Obj *CONST objv[],
                                         Tcl_Obj * obj_result));

                                         
/*! Return the fields of a TOL structure.
 *  { name1 type1 ... namej typej }
 *  type could be the name of a grammar or {STRUCT struct-name}
 */
extern int Tol_GetStructure _ANSI_ARGS_(( Tcl_Interp * interp,
                                          Tcl_Obj * name, Tcl_Obj * obj_result));

/*! Puts in obj_result the global reference of the given
 *  syntaxobject. The syntaxobject could be given as set member or as
 *  global variable. In case of error leave in obj_result the message.
 *  Returns TCL_OK if succesfull or TCL_ERROR in case of error.
 */
int Tol_GetReference(Tcl_Interp *interp, int objc, 
		     Tcl_Obj *CONST objv[], Tcl_Obj * obj_result);

/*
 * Fill in obj_result the result the string representation of the
 * address of the object referenced in obj_ref.
 */
extern int Tol_GetAddressFromObject _ANSI_ARGS_( (Tcl_Interp * interp,
                                                 Tcl_Obj * obj_ref,
                                                 Tcl_Obj * obj_result));
/*! Return the path of a set if it is a file, trigger an error in other case
 */
extern int Tol_GetSetPath _ANSI_ARGS_(( Tcl_Interp * interp,
                                        Tcl_Obj * name, Tcl_Obj * obj_result));

/*! Iterate over a first level's children of a given set.  The set
 *  could be tolset if the indexes is empty or tolset[i1][i2]...[ik]
 *  for the given indexes {i1,i2,...,ik}
 */
extern int Tol_IterChildren _ANSI_ARGS_((Tcl_Interp * interp,
                                         int objc,
                                         Tcl_Obj *CONST objv[],
                                         Tcl_Obj * obj_result));

/*! Fill in obj_result the content of the matrix variable.
 */
extern int Tol_GetMatrixContent _ANSI_ARGS_((Tcl_Interp * interp,
                                             Tcl_Obj * obj_name,
                                             Tcl_Obj * obj_result));

/*! Fill in obj_result the content of the serie variable.
 */
extern int Tol_GetSerieContent _ANSI_ARGS_((Tcl_Interp * interp,
                                            int objc,
                                            Tcl_Obj *CONST objv[],
                                            Tcl_Obj * obj_result));

/*! Create a new command to manipulate a group of series.
 */
extern int Tol_CreateSerieGrp _ANSI_ARGS_((Tcl_Interp * interp,
                                           int objc,
                                           Tcl_Obj *CONST series[],
                                           Tcl_Obj * obj_result));

/*! Create a new command to manipulate a group of series.
 */
extern int Tol_DestroySerieGrp _ANSI_ARGS_((Tcl_Interp * interp,
                                           int objc,
                                           Tcl_Obj *CONST objv[],
                                           Tcl_Obj * obj_result));

/*! Create a new command to manipulate a group of sets (only one set)
 */
extern int Tol_CreateTable _ANSI_ARGS_((Tcl_Interp * interp,
                                        Tcl_Obj * tblcmd,
                                        Tcl_Obj * objref,
                                        int ismat,
                                        Tcl_Obj * obj_result));

/*! Create a new command to manipulate a group of sets (only one set)
 */
extern int Tol_DestroyTable _ANSI_ARGS_((Tcl_Interp * interp,
                                         int objc,
                                         Tcl_Obj *CONST objv[],
                                         Tcl_Obj * obj_result));

/*! Fill in obj_result the list of tol files included
 */
extern int Tol_InfoIncluded _ANSI_ARGS_((Tcl_Interp * interp,
                                         Tcl_Obj * obj_result));

/*! Fill in obj_result the list of grammars defined in TOL
 */
extern int Tol_SetGrammarsObj _ANSI_ARGS_((Tcl_Interp * interp,
                                           Tcl_Obj * obj_result));

/*! Fill in obj_result the description of gra_name.
 */
int Tol_SetGrammarDescObj _ANSI_ARGS_((Tcl_Interp * interp, Tcl_Obj * gra_name,
                                        Tcl_Obj * obj_result));

/*! Fill in obj_result the functions defined for grammar gra_name.
 */
extern int Tol_SetFunctionsObj _ANSI_ARGS_((Tcl_Interp * interp, 
					    Tcl_Obj * gra_name, 
					    Tcl_Obj * obj_result));

/*! Fill in obj_result the function's prototype and description
 *  for a given function in a given grammar.
 */
int Tol_SetFunctionInfoObj _ANSI_ARGS_((Tcl_Obj * gra_name,
                            Tcl_Obj * fun_name,
                            Tcl_Obj * obj_result));
/*! Fill in obj_result the variables defined for grammar gra_name.
 */
extern
int Tol_SetVariablesObj _ANSI_ARGS_((Tcl_Interp * interp,
                                    Tcl_Obj * gra_name,
                                    Tcl_Obj * obj_result));

/*! Fill in obj_result the variables's description for a given function 
 *  in a given grammar.
 */
extern
int Tol_SetVariableInfoObj _ANSI_ARGS_((Tcl_Obj * gra_name,
                                       Tcl_Obj * var_name,
                                       Tcl_Obj * obj_result));

/*! Create a new command to access the given TOL timeset object name.
 */
extern
int Tol_CreateTMSCmd _ANSI_ARGS_((Tcl_Interp * interp, 
                                  Tcl_Obj * tmscmd,
                                  Tcl_Obj * tmsname,
                                  Tcl_Obj * obj_result));

/*! Destroy the tcl tms commands given in objv.
 */
extern
int Tol_DestroyTMSCmd _ANSI_ARGS_((Tcl_Interp * interp, 
                                  int objc, Tcl_Obj *CONST objv[],
                                  Tcl_Obj * obj_result));

/*! Implement tol::date
 */
extern 
int Tol_DateOption _ANSI_ARGS_((Tcl_Interp * interp,
                                int objc, Tcl_Obj *CONST objv[],
                                Tcl_Obj * obj_result));

/*! Implement tol::autocorr
 */
extern 
int Tol_GetAutoCorr _ANSI_ARGS_((Tcl_Interp * interp,
                                int objc, Tcl_Obj *CONST objv[],
                                Tcl_Obj * obj_result));

/*! Implement tol::seriestat
 */
extern 
int Tol_ComputeSerieStat _ANSI_ARGS_((Tcl_Interp * interp,
                                      int objc, Tcl_Obj *CONST objv[],
                                      Tcl_Obj * obj_result));
extern
int Tol_StopEvaluation _ANSI_ARGS_(());

extern 
int Tol_LCFormat _ANSI_ARGS_((Tcl_Interp*   interp,
                              Tcl_Obj*CONST value,
                              Tcl_Obj*      obj_result));
extern
void Tol_InstallHciWriter _ANSI_ARGS_(());

extern
void Tol_InstallMethodsHook _ANSI_ARGS_(());

END_C_DECLS

//------------------------------------------------------------
#ifdef __cplusplus

//#define FINDVARIABLE FindVariable
#define FINDVARIABLE(name) FindOperand(name,true)

class BSyntaxObject;
class BGrammar;
class BDate;
class BList;

extern
int Tol_CreateSerieGrp _ANSI_ARGS_((Tcl_Interp * interp,
				    char * sgrp_name,
				    BList * series,
				    Tcl_Obj * obj_result));

extern
int Tol_ParseDate(Tcl_Interp * interp, char * what, BDate & date);

/*!
 *  Tol_ResolveObject --
 *
 *     Resolve a SyntaxObject given a toltcl object reference. A
 *     reference is {TYPE ?container? i1 i2 ... in} or a global
 *     container's name of type File|Set|NameBlock|Console. When TYPE
 *     is Console then container is not given. The grammar of the
 *     object found is matched to the given argument grammar. If the
 *     return value is NULL an error message is left in interp.
 */
extern
const BSyntaxObject * Tol_ResolveObject(Tcl_Interp *interp,
					Tcl_Obj    *obj_ref,
                                        Tcl_Obj    *obj_result/*,
                                        BGrammar   *grammar=NULL*/);

extern
int Tol_ListToDate _ANSI_ARGS_(( Tcl_Interp * interp, Tcl_Obj * list,
                    BDate & date, Tcl_Obj * obj_result ));

extern
int Tol_DateToList _ANSI_ARGS_(( BDate & date, Tcl_Obj * obj_result ));

#endif /* __cplusplus */
//------------------------------------------------------------

#endif /* TOLTCLINT_H */
