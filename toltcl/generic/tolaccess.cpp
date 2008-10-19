/* tolaccess.cpp: This file implements an interface between object in tol
                  and "objects" in tclThis file define a set of utils
                  functions.
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

#include <ctype.h>

#if defined(_MSC_VER)
#  include <win_tolinc.h>
#endif

#include <tol/tol_init.h>
#include <tol/tol_version_def.h>
#include <tol/tol_bcommon.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bmethodhook.h>

#include <toltclInt.h>
#include <tt_pool.h>


/*
 * if SyntaxObject has IsFile implemented, uncomment the following
 * define:
 */

/* #define ISFILE_IMPLEMENTED */

typedef void (* getmethod)( const BSyntaxObject *, BText & );
typedef int  (* checkmethod)( const BSyntaxObject * );

static
int QuickSort( Tcl_Interp * interp, Tcl_Obj * list );

inline 
char * skipWhite(char * pch)
{
  while (*pch && isspace(*pch))
    ++pch;
  return pch;
}

static
const BText& GetObjectName(const BSyntaxObject * so)
{
  return so->HasName() ? so->Name() : so->LocalName();
}

static
void GetName( const BSyntaxObject * so, BText & res )
{
  res = so->Name();
}

static
void GetIdentify( const BSyntaxObject * so, BText & res )
{
  res = so->Identify();
}

static
void GetPath( const BSyntaxObject * so, BText & res )
{
  res = so->TolPath();
}

static
int
ExcludeAnything( const BSyntaxObject * so )
{
  return so != GraAnything();
}

static
int HasNoSource( const BSyntaxObject * so )
{
  return !so->Source();
}

static
int BList2TclList( Tcl_Interp * interp,
                   BList * blist, Tcl_Obj * tcl_list,
                   getmethod m, checkmethod accept = NULL)
{
  char * str;
  Tcl_Obj * name;
  Tcl_DString dstr;
  BText tmp;

  while(blist)
  {
    BSyntaxObject* so = (BSyntaxObject*)Car(blist);
    if(!accept || (*accept)(so))
    {
      (*m)(so,tmp);
      str = Tcl_ExternalToUtfDString(NULL,tmp,-1,&dstr);
      name = Tcl_NewStringObj(str,-1);
      Tcl_DStringFree(&dstr);
      if ( Tcl_ListObjAppendElement(interp, tcl_list, name) != TCL_OK ) {
        Tcl_DecrRefCount(name);
        return TCL_ERROR;
      }
    }
    blist = Cdr(blist);
  }
  return TCL_OK;
}

/*
 * Fill in obj_result the result of avaluating in TOL the given
 * string.
 *
 */

/* cdr of console_stack point to first item */
static BList console_stack;
// Read only stack
static BList console_stack_hidden;

static
void my_concat(BList* head, BList* tail)
{
  if (head->Cdr()) {
    my_concat(head->Cdr(), tail);
  } else {
    head->PutCdr(tail);
  }
}

int Tol_EvalExpr (Tcl_Obj * obj_expr, int hidden, Tcl_Obj * obj_result)
{
  BList * tol_result;
  Tcl_DString dstr;
  char * expr;
  
  Tcl_DStringInit(&dstr);
  expr = Tcl_UtfToExternalDString(NULL,Tcl_GetString(obj_expr),-1,&dstr);
/*
  Std(BText("\nTol_EvalExpr ")+
      "LEVEL = "+BGrammar::Level()+ "\n"
      "BEGIN {\n"+expr+"\n} END Tol_EvalExpr\n");
*/
  tol_result = MultyEvaluate(expr);
  Tcl_DStringFree(&dstr);
  if(tol_result) {
    my_concat(hidden?&console_stack_hidden:&console_stack, tol_result);
  }
  /*  Tcl_AppendStringsToObj(obj_result,
      "couldn't evaluate tol expresion \"",
      Tcl_GetString(obj_expr), "\"", NULL);
      return TCL_ERROR;*/
  return TCL_OK;
}

int Tol_GetStructure( Tcl_Interp * interp, Tcl_Obj * name, Tcl_Obj * obj_result )
{
  Tcl_DString dstr;
  BStruct * structure;
  BStruct * substruct;

  Tcl_DStringInit( &dstr );
  Tcl_UtfToExternalDString( NULL, Tcl_GetString(name), -1, &dstr );
  if ( Tcl_DStringLength( &dstr ) && (structure = FindStruct(Tcl_DStringValue(&dstr)))) {
    Tcl_Obj * tmp[2];
    Tcl_Obj * info[2];
    Tcl_Obj ** fields;
    BText btxt;
    int nfields;
    
    nfields = structure->Size();
    fields = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*) * nfields );
    for ( int i = 0; i < nfields; i++ ) {
      BField & field = (*structure)[i];
      btxt = field.Name();
      Tcl_DStringFree(&dstr);
      Tcl_ExternalToUtfDString( NULL, btxt, -1, &dstr );
      info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
      Tcl_DStringFree( &dstr );
      if ( (substruct = field.Struct()) ) {
        btxt = substruct->Name();
        Tcl_ExternalToUtfDString( NULL, btxt, -1, &dstr );
        tmp[0] = Tcl_NewStringObj( "STRUCT", 6 );
        tmp[1] = Tcl_NewStringObj( Tcl_DStringValue(&dstr), -1);
        info[1] = Tcl_NewListObj( 2, tmp );
      } else if ( field.Grammar() ) {
        btxt = field.Grammar()->Name();
        Tcl_ExternalToUtfDString( NULL, btxt, -1, &dstr );
        info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
      }
      fields[i] = Tcl_NewListObj(2,info);
    }
    Tcl_SetListObj( obj_result, nfields, fields );
    Tcl_Free( (char*)fields );
  }
  return TCL_OK;
}

/*
 * Return the path of a set if it is a file, trigger an error in other case
 *
 */

int Tol_GetSetPath( Tcl_Interp * interp, Tcl_Obj * name, Tcl_Obj * obj_result )
{
  Tcl_Obj ** items;
  int n;
  int tcl_code;
  Tcl_DString uniBuffer;
  const BSyntaxObject * syn;

 /* if name contains spaces the we should try it first as a string */
  Tcl_DStringInit(&uniBuffer);
  Tcl_UtfToExternalDString(NULL,Tcl_GetString(name),-1,&uniBuffer);
  BText fname(Tcl_DStringValue(&uniBuffer));
  syn = (BSyntaxObject*)BSetFromFile::FindCompiled(fname);
  Tcl_DStringFree(&uniBuffer);
  if (!syn) {
    /* look for subitem given in name */
    tcl_code = Tcl_ListObjGetElements(interp,name,&n,&items);
    if (tcl_code != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    if ( n > 1 ) {
      /* is a list & should be a member of a set */
      
      syn = Tol_FindSet( items[0], obj_result );
      if ( !syn )
        return TCL_ERROR;
      syn = Tol_FindChild(interp,syn,items+1,n-1,obj_result);
    }//  else
//       syn = Tol_FindSet(name,obj_result);
  }
  if ( !syn )
    return TCL_ERROR;
  BSet & set = ((BUserSet*)syn)->Contens();
  if ( set.SubType() >= BSet::MODFile && set.SubType() <= BSet::BMIFile ) {
    Tcl_DString dstr;
    BText path( syn->TolPath() );

    Tcl_DStringInit( &dstr );
    Tcl_ExternalToUtfDString(NULL,path,-1,&dstr);
    Tcl_SetStringObj( obj_result, Tcl_DStringValue(&dstr), -1 );
    return TCL_OK;
  } else {
    Tcl_AppendStringsToObj( obj_result, Tcl_GetString(name),
                            " is not a valid file object", NULL );
    return TCL_ERROR;
  }
}

/*
 * Fill in obj_result the content of the given tol object's names. The
 * object are searched in the console's stack.
 *
 * Return:
 *
 *   {GRAMMAR NAME CONTENT PATH DESCRIPTION ISFILE [Has SubSet? SUBTYPE STRUCTURE]}
 */

int Tol_ListStack(Tcl_Interp * interp,int objc,Tcl_Obj *CONST objv[],
                  Tcl_Obj * obj_result)
{
  BSyntaxObject * so;
  BText btxt;
  Tcl_DString dstr;
  BList * tol_item;
  Tcl_Obj * robjv[9];
  int robjc;
  Tcl_Obj * tcl_item;
  int tcl_result;
  BSet * ptrSet = 0;
  int tmpCard;
  int j;
  
  Tcl_DStringInit(&dstr);
  
  tol_item = console_stack.Cdr();
  while (tol_item) {
    so = (BSyntaxObject*)Car(tol_item);
    
    /* GRAMMAR NAME */
    btxt = so->Grammar()->Name();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    robjv[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* OBJECT NAME */
//  btxt = so->Identify();
    btxt = GetObjectName(so);
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    robjv[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* OBJECT CONTENT */
    btxt = so->Info();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    robjv[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* OBJECT PATH */
    btxt = so->SourcePath();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    robjv[3] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* OBJECT DESCRIPTION */
    btxt = so->Description();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    robjv[4] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* IS THE OBJECT A FILE? */
#ifdef ISFILE_IMPLEMENTED
    robjv[5] = Tcl_NewIntObj(so->IsFile());
#else
    robjv[5] = Tcl_NewIntObj(0);
#endif
    if ( so->Grammar()==GraSet() ) {
      /* HAS SUBSET */
      ptrSet = &(((BUserSet*)so)->Contens());
      tmpCard = ptrSet->Card();
      for (j=1; j<=tmpCard; ++j )
        if ( (*ptrSet)[j]->Grammar()==GraSet() )
          break;
      robjv[6] = Tcl_NewIntObj(j<=tmpCard);
        
      /* SUBTYPE INFO */
      robjv[7] = Tcl_NewIntObj( ptrSet->SubType() );
        
      /* STRUCT INFO */
      if ( ptrSet->Struct() ) {
        btxt = ptrSet->Struct()->Name();
        Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
        robjv[8] = Tcl_NewStringObj( Tcl_DStringValue(&dstr), -1 );
        Tcl_DStringFree( &dstr );
      } else
        robjv[8] = Tcl_NewStringObj( NULL, 0 );
      robjc = 9;
    } else
      robjc = 6;
    
    /* build the tcl item as a list */
    tcl_item = Tcl_NewListObj(robjc,robjv);
    
    /* append to the end of the result */
    tcl_result = Tcl_ListObjAppendElement(interp,obj_result,tcl_item);
    if (tcl_result != TCL_OK) {
      Tcl_SetStringObj(obj_result,"",-1);
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    tol_item = Cdr(tol_item);
  }
  return TCL_OK;
}

struct tcl_info_t
{
  Tcl_Interp *interp;
  int objc;
  Tcl_Obj* CONST* objv;
  Tcl_DString &dstr;
};

/*
 * Tol_ReleaseStack --
 *
 * Recursive version in order to release stack in reverse order
 *
 * Preconditions:
 *
 *   P != NULL
 */
static
int Tol_ReleaseStack(tcl_info_t &info, BList *P)
{
  int result;
  
  assert(P != NULL);

  if (P->Cdr()) {
    /* keep on looking for rest of items */
    result = Tol_ReleaseStack(info, P->Cdr());
    /* process this item */
    BList *current = P->Cdr();
    BSyntaxObject * so;
    int i;
    so = (BSyntaxObject*)(current->Car());
    if (info.objc) {
      const char* name = so->Name().String();
      for (i = 0; i<info.objc; ++i) {
        Tcl_UtfToExternalDString(NULL,Tcl_GetString(info.objv[i]),-1,
                                 &info.dstr);
        if (!strcmp(Tcl_DStringValue(&info.dstr), name)) {
          Tcl_DStringFree(&info.dstr);
          break;
        }
        Tcl_DStringFree(&info.dstr);
      }
    } else {
      /* mark current as being found */
      i = -1;
    }
    if (i<info.objc) {
      BList *next = current->Cdr();
      /* delete will DecNRefs to the Car */
      delete current;
      /* now try to release the BSyntaxObject */
      DESTROY(so);
      /* chain the list */
      P->PutCdr(next);
    }
  } else {
    /* EOR */
    result = TCL_OK;
  }
  return result;
}

int Tol_ReleaseStack(Tcl_Interp * interp, int objc, Tcl_Obj *CONST objv[],
                     Tcl_Obj * obj_result)
{
  Tcl_DString dstr;
  tcl_info_t tcl_info = {interp, objc, objv, dstr};
  Tcl_DStringInit(&dstr);

  return Tol_ReleaseStack(tcl_info, &console_stack);
}

/*!
 * Tol_GetConsoleObject --
 *
 *     Returns the object with given index in the console object list.
 */
BSyntaxObject * Tol_GetConsoleObject(int index )
{
  BList * current = console_stack.Cdr();
  int i = 1;
  while (current) {
    if (i==index) break;
    current = Cdr(current);
    ++i;
  }
  return current ? (BSyntaxObject*)(current->Car()) : NULL;
}


/*
 * return the set with name obj_name, in case of error return NULL
 * and leave a description in obj_result. Also looks for files.
 * 
 */
const BSyntaxObject * Tol_FindSet(Tcl_Obj * obj_name, Tcl_Obj * obj_result)
{
  int is_file;

  return Tol_FindSet(obj_name, obj_result, is_file);
}

const BSyntaxObject * Tol_FindSet(Tcl_Obj * obj_name, Tcl_Obj * obj_result, int& is_file)
{
  BGrammar * gra = GraSet();

  if (!gra) {
    Tcl_SetStringObj( obj_result, "grammar 'Set' not found", -1);
    return NULL;
  }
  Tcl_DString uniBuffer;

  Tcl_DStringInit(&uniBuffer);
  Tcl_UtfToExternalDString(NULL,Tcl_GetString(obj_name),-1,&uniBuffer);
  BSyntaxObject* var = gra->FINDVARIABLE(Tcl_DStringValue(&uniBuffer));
  if (var) {
    is_file = 0;
  } else {
    BText fname(Tcl_DStringValue(&uniBuffer));
    var = (BSyntaxObject*)BSetFromFile::FindCompiled(fname);
    if (var) {
      is_file = 1;
    } else {
      Tcl_AppendStringsToObj(obj_result,"\"",
                             Tcl_GetString(obj_name),
                             "\" isn't a variable in grammar Set", NULL );
    }
  }
  Tcl_DStringFree(&uniBuffer);
  return var;
}

/*
 * given a tol set in syn_obj, returns its child corresponding to
 * the set of indexes indexes of size n.
 * In case of error return NULL and leave a description in obj_result
 *
 */

const BSyntaxObject * Tol_FindChild( Tcl_Interp *    interp,
				     const BSyntaxObject * syn_obj,
				     Tcl_Obj **      indexes, int n,
				     Tcl_Obj *       obj_result)
{
  int tcl_code;
  Tcl_DString dstr;
  BSet * set;
  int idx, card;
  int i;

  for (i=0; i<n; ++i) {
    //set = Set(syn_obj);
    /* ojo con este cambio */
    set = &(((BUserSet*)syn_obj)->Contens());
    card = set->Card();
    tcl_code = Tcl_GetIntFromObj(interp,indexes[i],&idx);
    if (tcl_code != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      break;
    }
    if (idx >= 1 && idx<=card) {
      syn_obj = (*set)[idx];
      if (i < n - 1 && syn_obj->Grammar() != GraSet()) {
        BText info(syn_obj->Info());
        Tcl_DStringInit(&dstr);
        Tcl_ExternalToUtfDString(NULL,info,-1,&dstr);
        Tcl_AppendStringsToObj(obj_result,"object \"", Tcl_DStringValue(&dstr),"\" at position \"",
                               Tcl_GetString(indexes[i]), "\" is not a set",NULL);
        Tcl_DStringFree(&dstr);
        break;
      }
    } else {
      Tcl_AppendStringsToObj(obj_result,"Index out of range \"",
                             Tcl_GetString(indexes[i]), "\"",NULL);
      break;
    }
  }
  return i == n ? syn_obj : NULL;
}


/*!
 *  Tol_ResolveObject --
 *
 *     Resolve a SyntaxObject given a toltcl object reference. A reference is
 *     {setname i1 i2 ... in} or a global name
 *     setname is a global set name (including a file name) or, if empty, is
 *     stack's console. The grammar of the object found is matched to the
 *     given argument grammar. If the return value is NULL the corresponding
 *     message is left in interp.
 */

const BSyntaxObject * Tol_ResolveObject(Tcl_Interp * interp,
					Tcl_Obj * obj_ref,
					BGrammar *  match_gra)
{
  Tcl_Obj ** items;
  int n;
  int tcl_code;

  /* look for subitem given in obj_ref */
  tcl_code = Tcl_ListObjGetElements(interp, obj_ref, &n, &items);
  if (tcl_code != TCL_OK)
    return NULL;

  // this Tcl_Obj is to put error messages
  Tcl_Obj * obj_result = Tcl_NewObj();
  const BSyntaxObject * syn = NULL;
  const char * graname = match_gra?match_gra->Name().String():"";

  if (n > 1) {
    /* is a list & should be a member of a set */
    if (*Tcl_GetString(items[0])) {
      if ((syn=Tol_FindSet(items[0], obj_result))) {
        // global set found
        if ((syn=Tol_FindChild(interp, syn, items+1, n-1, obj_result)))
          if (match_gra && syn->Grammar()!=match_gra) {
            // grammar missmatch
            Tcl_AppendStringsToObj(obj_result, "object '",
                                   Tcl_GetString(obj_ref), "' ",
                                   "found but does not belong to grammar ",
                                   "'", graname, NULL);
            syn = NULL;
          }
      }
    } else {
      // is a reference in the console
      int i1;
      if ((tcl_code = Tcl_GetIntFromObj(interp,items[1],&i1)!=TCL_OK)) {
        Tcl_DecrRefCount(obj_result);
        return NULL;
      }
      if (!(syn=Tol_GetConsoleObject(i1)))
        Tcl_AppendStringsToObj(obj_result, "index ",
                               Tcl_GetString(items[1]),
                               " not found in console", NULL);
      else {
        // first index found in console
        if (n>2) {
          // there are more indexes & syn should be a Set
          if (syn->Grammar()==GraSet())
            syn=Tol_FindChild(interp, syn, items+2, n-2, obj_result);
          else {
            Tcl_AppendStringsToObj(obj_result, "invalid object reference in console '",
                                   Tcl_GetString(obj_ref),
                                   "' first index '", Tcl_GetString(items[1]),
                                   "' should be a Set", NULL);
            syn = NULL;
          }
        }
        // match grammar
        if (syn && match_gra && syn->Grammar()!=match_gra) {
          // grammar missmatch
          Tcl_AppendStringsToObj(obj_result, "object '",
                                 Tcl_GetString(obj_ref), "' ",
                                 "found but does not belong to grammar ",
                                 "'", graname, "'", NULL);
          syn = NULL;
        }
      }
    }
  } else if (n==1) {
    // look for a global variable
    char * var_name = Tcl_GetString(items[0]);
    if (match_gra==GraSet())
      syn = Tol_FindSet(items[0], obj_result);
    else {
      // look global var in match_gra
      Tcl_DString dstr;
      Tcl_DStringInit(&dstr);
      Tcl_UtfToExternalDString(NULL,var_name,-1,&dstr);
      if (!(syn = match_gra->FINDVARIABLE(Tcl_DStringValue(&dstr))))
        Tcl_AppendStringsToObj(obj_result, "variable '",
                               var_name, "' not found in grammar '",
                               graname, "'", NULL);
      Tcl_DStringFree(&dstr);
    }
  } else {
    // n == 0
    Tcl_AppendStringsToObj(obj_result,
                           "empty set reference in ResolveObject",
                           NULL);
  }
  // always set the object result & release obj_release.
  Tcl_SetObjResult(interp, obj_result);
  return syn;
}

int Tol_FindSOInSet(BSet * ptrSet, const BSyntaxObject * syn,
		    ToltclPool & indexes)
{
  int j;
  const BSyntaxObject * so;
  
  for (j = 1; j <= ptrSet->Card(); j++) {
    so = (*ptrSet)[j];
    if (so == syn || (so->Grammar() == GraSet() &&
		      Tol_FindSOInSet(&(((BUserSet*)so)->Contens()),
				      syn, indexes))) {
      /* found!!! */
      indexes.AppendObject((void*)j);      
      return 1;
    }
  }

  return 0;
}
 
/*
 *  Puts in obj_result the global reference of the given
 *  syntaxobject. The syntaxobject could be given as set member or as
 *  global variable. In case of error leave in obj_result the message.
 *  Returns TCL_OK if succesfull or TCL_ERROR in case of error.
 */

int Tol_GetReference(Tcl_Interp *interp, int objc, Tcl_Obj *CONST objv[],
                     Tcl_Obj * obj_result)
{
  Tcl_Obj ** items;
  int n;
  int tcl_code;
  const BSyntaxObject * source, * syn;
  BSet * ptrSet;
  ToltclPool indexes;
  Tcl_DString dstr;
  int j, idx, is_file;
  char * graname = "Set";

  assert(!Tcl_IsShared(obj_result));
  
  //Tcl_DStringInit(&dstr);
  if (objc == 2) {
  // global access
    BGrammar * gra;
    graname = Tcl_GetString(objv[0]);
    gra = BGrammar::FindByName(graname);
    if (!gra) {
      Tcl_AppendStringsToObj( obj_result, "\"",
			      graname,
			      "\" isn't a grammar", NULL);
      return TCL_ERROR;
    }
    Tcl_UtfToExternalDString(NULL,Tcl_GetString(objv[1]),-1,&dstr);
    syn = gra->FINDVARIABLE(Tcl_DStringValue(&dstr));
    Tcl_DStringFree(&dstr);
    if (!syn) {
      Tcl_AppendStringsToObj(obj_result, "\"",
			     Tcl_GetString(objv[1]),
			     "\" isn't a variable in grammar ",
			     graname, NULL);
      return TCL_ERROR;
    }
  } else {
    // member access
    tcl_code = Tcl_ListObjGetElements(interp,objv[0],&n,&items);
    if (tcl_code != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    if (n<=1) {
      Tcl_AppendStringsToObj(obj_result, "invalid set element reference \"",
			     Tcl_GetString(objv[0]),
			     "\" : should be a list of of the form {set_name i1 ... ik}",
			     NULL);
      return TCL_ERROR;      
    }
    syn = Tol_FindSet(items[0], obj_result, is_file);
    if (!syn)
      return TCL_ERROR;
    if (n > 1) {
      if (!Tol_FindChild(interp, syn, items+1, n-1, obj_result))
	return TCL_ERROR;
      if (is_file) {
        /* here we have in objv[0] the answer */
        Tcl_SetListObj(obj_result, n, items);
        return TCL_OK;
      }
      // append the indexes in reverse order
      
      for (j=n-1; j > 0; j--) {
	Tcl_GetIntFromObj(interp, items[j], &idx);
	indexes.AppendObject((void*)idx);
      }
    }  
  }
  source = syn->Source();
  if (!source) {
    /* is a global variable not defined inside a file */
    Tcl_ExternalToUtfDString(NULL, syn->Name(), -1, &dstr);
    Tcl_AppendStringsToObj(obj_result, Tcl_DStringValue(&dstr), NULL);
    Tcl_DStringFree(&dstr);
    return TCL_OK;
  } else {
    ptrSet = &(((BUserSet*)source)->Contens());
    if (Tol_FindSOInSet(ptrSet, syn, indexes)) {
      items = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*) * (indexes.GetSize()+1));
      //items = (Tcl_Obj**)malloc(sizeof(Tcl_Obj*) * (indexes.GetSize()+1));
      Tcl_ExternalToUtfDString(NULL, source->Name(), -1, &dstr);
      items[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
      Tcl_DStringFree(&dstr);
      int last = indexes.GetLast();
      for(j = last; j >= 0; j--)
        items[last+1-j] = Tcl_NewIntObj(long(*indexes._GetObject(j)));
      Tcl_SetListObj(obj_result, indexes.GetSize()+1, items);
      Tcl_Free((char*)items);
      //free(items);
      return TCL_OK;
    } else {
      /* this should not happend because syn should be inside syn->source */
      Tcl_AppendStringsToObj(obj_result,
                             "internal error: syn not found in syn->Source()",
                             NULL);
      return TCL_ERROR;
    }
  }
}


/*
 * Iterate over a first level's children of a given set.  The set
 * could be tolset if the indexes is empty or tolset[i1][i2]...[ik]
 * for the given indexes {i1,i2,...,ik}
 *
 */

/* OJO con la declaracion de tmpSet */

int Tol_IterChildren(Tcl_Interp * interp,
                     int objc,
                     Tcl_Obj *CONST objv[],
                     Tcl_Obj * obj_result)
{
#define ARG_SCRIPT    0
#define ARG_GRAMMAR   ARG_SCRIPT+1
#define ARG_NAME      ARG_GRAMMAR+1
#define ARG_CONTENT   ARG_NAME+1
#define ARG_PATH      ARG_CONTENT+1
#define ARG_DESC      ARG_PATH+1
#define ARG_INDEXES   ARG_DESC+1
#define ARG_ISFILE    ARG_INDEXES+1
#define ARG_HASSUBSET ARG_ISFILE+1
#define ARG_SUBTYPE   ARG_HASSUBSET+1
#define ARG_STRUCT    ARG_SUBTYPE+1
#define NUM_ARGS      ARG_STRUCT+1

  const BSyntaxObject* var_set;
  Tcl_DString dstr;
  Tcl_Obj * ArgIndexes;
  int tcl_code, cont_loop;
  int i, card ;
  int n, index_length;
  Tcl_Obj ** items;
  
  if (!(var_set=Tol_ResolveObject(interp, objv[0], GraSet()))) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));    
    return TCL_ERROR;
  }
  int datac;
  Tcl_Obj * datav[NUM_ARGS];
  // is ok to ask for a list
  Tcl_ListObjGetElements(interp,objv[0],&n,&items);
  if (n>1)
    ArgIndexes = Tcl_NewListObj(n-1,items+1);
  else
    ArgIndexes = Tcl_NewListObj(0,NULL);
  datav[ARG_SCRIPT] = objv[1];
  
  datav[ARG_GRAMMAR] = Tcl_NewObj();
  datav[ARG_NAME]    = Tcl_NewObj();
  datav[ARG_CONTENT] = Tcl_NewObj();
  datav[ARG_PATH]    = Tcl_NewObj();
  datav[ARG_DESC]    = Tcl_NewObj();
  Tcl_Obj * last_index = Tcl_NewIntObj(0);
  Tcl_ListObjAppendElement(NULL,ArgIndexes,last_index);
  datav[ARG_INDEXES] = ArgIndexes;
  Tcl_ListObjLength(NULL,datav[ARG_INDEXES],&index_length);
  datav[ARG_ISFILE] = Tcl_NewObj();
  datav[ARG_HASSUBSET] = Tcl_NewObj();
  datav[ARG_SUBTYPE] = Tcl_NewObj();
  datav[ARG_STRUCT] = Tcl_NewObj();
  for (i=ARG_GRAMMAR; i<NUM_ARGS; ++i)
    Tcl_IncrRefCount(datav[i]);
  
  /* var_set is a good tol set */
  
  BSet & set = ((BUserSet*)var_set)->Contens(); //Set(var);
  card = set.Card();
  BSyntaxObject* syn_i;
  /* now iter over its children */
  
  Tcl_DStringInit(&dstr);
  BText btxt;
  
  /* used to verify if a set has subsets */
  BSet * ptrSet = 0;
  int tmpCard;
  int j;
  
  for (i=1; i<=card; ++i) {
    syn_i = set[i];
    
    /* read the grammar */
    
    btxt = syn_i->Grammar()->Name();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    if (Tcl_IsShared(datav[ARG_GRAMMAR])) {
      Tcl_DecrRefCount(datav[ARG_GRAMMAR]);
      datav[ARG_GRAMMAR] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
      Tcl_IncrRefCount(datav[ARG_GRAMMAR]);
    } else 
      Tcl_SetStringObj(datav[ARG_GRAMMAR], Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* read the name */
    
    //btxt = syn_i->Identify();
    btxt = GetObjectName(syn_i);
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    if (Tcl_IsShared(datav[ARG_NAME])) {
      Tcl_DecrRefCount(datav[ARG_NAME]);
      datav[ARG_NAME] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
      Tcl_IncrRefCount(datav[ARG_NAME]);
    } else 
      Tcl_SetStringObj(datav[ARG_NAME], Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* read the content */
    
    btxt = syn_i->Info();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    if (Tcl_IsShared(datav[ARG_CONTENT])) {
      Tcl_DecrRefCount(datav[ARG_CONTENT]);
      datav[ARG_CONTENT] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
      Tcl_IncrRefCount(datav[ARG_CONTENT]);
    } else 
      Tcl_SetStringObj(datav[ARG_CONTENT],Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* read the path */
    
    btxt = syn_i->SourcePath();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    if (Tcl_IsShared(datav[ARG_PATH])) {
      Tcl_DecrRefCount(datav[ARG_PATH]);
      datav[ARG_PATH] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
      Tcl_IncrRefCount(datav[ARG_PATH]);
    } else 
      Tcl_SetStringObj(datav[ARG_PATH],Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    /* read the description */
    
    btxt = syn_i->Description();
    Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
    if (Tcl_IsShared(datav[ARG_DESC])) {
      Tcl_DecrRefCount(datav[ARG_DESC]);
      datav[ARG_DESC] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
      Tcl_IncrRefCount(datav[ARG_DESC]);
    } else 
      Tcl_SetStringObj(datav[ARG_DESC],Tcl_DStringValue(&dstr),-1);
    Tcl_DStringFree(&dstr);
    
    if (syn_i->Grammar()==GraSet()) {
      /* update last index */
      if (Tcl_IsShared(last_index)) {
        last_index = Tcl_NewIntObj(i);
        Tcl_ListObjReplace(NULL, datav[ARG_INDEXES], index_length-1,
                           1, 1, &last_index);
      }
      /*
      if (Tcl_IsShared(datav[ARG_INDEXES])) {
        Tcl_DecrRefCount(datav[ARG_INDEXES]);
        datav[ARG_INDEXES] = Tcl_DuplicateObj(datav[ARG_INDEXES]);
        Tcl_IncrRefCount(datav[ARG_INDEXES]);
        Tcl_ListObjLength(NULL,datav[ARG_INDEXES],&n);
        Tcl_ListObjIndex(NULL,datav[ARG_INDEXES],n-1,&last_index);
      }
      */
      /* set the last value index */
      Tcl_SetIntObj(last_index,i);
      /* to refresh the string */
      Tcl_InvalidateStringRep(datav[ARG_INDEXES]);
      if (Tcl_IsShared(datav[ARG_ISFILE])) {
        Tcl_DecrRefCount(datav[ARG_ISFILE]);
#ifdef ISFILE_IMPLEMENTED
        datav[ARG_ISFILE] = Tcl_NewIntObj(syn_i->IsFile());
#else
        datav[ARG_ISFILE] = Tcl_NewIntObj(0);
#endif
        Tcl_IncrRefCount(datav[ARG_ISFILE]);
      } else
#ifdef ISFILE_IMPLEMENTED
        Tcl_SetIntObj(datav[ARG_ISFILE],syn_i->IsFile());
#else
      Tcl_SetIntObj(datav[ARG_ISFILE],0);
#endif
      /* verify if has subset */
      ptrSet = &(((BUserSet*)syn_i)->Contens());
      tmpCard = ptrSet->Card();
      for (j=1; j<=tmpCard; ++j )
        if ( (*ptrSet)[j]->Grammar()==GraSet() )
          break;
      if (Tcl_IsShared(datav[ARG_HASSUBSET])) {
        Tcl_DecrRefCount(datav[ARG_HASSUBSET]);
        datav[ARG_HASSUBSET] = Tcl_NewIntObj(j<=tmpCard);
        Tcl_IncrRefCount(datav[ARG_HASSUBSET]);
      } else
        Tcl_SetIntObj(datav[ARG_HASSUBSET],j<=tmpCard);
        
      /* subtype info */
        
      if (Tcl_IsShared(datav[ARG_SUBTYPE])) {
        Tcl_DecrRefCount(datav[ARG_SUBTYPE]);
        datav[ARG_SUBTYPE] = Tcl_NewIntObj(ptrSet->SubType());
        Tcl_IncrRefCount(datav[ARG_SUBTYPE]);
      } else
        Tcl_SetIntObj(datav[ARG_SUBTYPE],ptrSet->SubType());
        
      /* STRUCT INFO */
      if ( ptrSet->Struct() ) {
        btxt = ptrSet->Struct()->Name();
        Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
      }
      if (Tcl_IsShared(datav[ARG_STRUCT])) {
        Tcl_DecrRefCount(datav[ARG_STRUCT]);
        datav[ARG_STRUCT] = Tcl_NewStringObj(Tcl_DStringValue(&dstr),-1);
        Tcl_IncrRefCount(datav[ARG_STRUCT]);
      } else 
        Tcl_SetStringObj(datav[ARG_STRUCT],Tcl_DStringValue(&dstr),-1);
      Tcl_DStringFree(&dstr);
      datac = NUM_ARGS;
    } else
      datac = ARG_INDEXES;
    tcl_code = Tcl_EvalObjv(interp,datac,datav,TCL_EVAL_GLOBAL);
    if (tcl_code != TCL_OK) {
      /* liberar los objetos tcl */
      for (i=ARG_GRAMMAR; i<NUM_ARGS; ++i)
        Tcl_DecrRefCount(datav[i]);
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return tcl_code;
    }
    /* check if break loop */
    cont_loop = 1;
    tcl_code = Tcl_GetIntFromObj(interp,Tcl_GetObjResult(interp),&cont_loop);
    if (tcl_code == TCL_OK && !cont_loop)
      break;
  }
  
  /* liberar los objetos tcl */
  for (i=ARG_GRAMMAR; i<NUM_ARGS; ++i)
    Tcl_DecrRefCount(datav[i]);
  return TCL_OK;
}

/*
 * Fill in obj_result the content of the matrix variable.
 *
 */

int Tol_GetMatrixContent(Tcl_Interp * interp,
                         Tcl_Obj * obj_name,
                         Tcl_Obj * obj_result)
{
  //BGrammar * gra = NULL;
  const BSyntaxObject * tol_obj = NULL;
   
  if (!(tol_obj=Tol_ResolveObject(interp, obj_name, GraMatrix()))) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));    
    return TCL_ERROR;
  }
     
#define IT_NAME 0
#define IT_ROWS 1
#define IT_COLS 2
#define IT_DATA 3
#define IT_SIZE 4
     
  Tcl_Obj * items[4];
  Tcl_Obj ** subitems;
  int r, Rows, c, Cols;
  BDat & dat = BDat::Unknown();
  
  BMatrix<BDat> & mat = (BMatrix<BDat>&)(((BUserMat*)tol_obj)->Contens());
  
  //items[IT_NAME] = Tcl_NewStringObj(tol_obj->Identify(),-1);
  items[IT_NAME] = Tcl_NewStringObj(GetObjectName(tol_obj),-1);
  Rows = mat.Rows();
  Cols = mat.Columns();
  items[IT_ROWS] = Tcl_NewIntObj(Rows);
  items[IT_COLS] = Tcl_NewIntObj(Cols);
  subitems = (Tcl_Obj**)Tcl_Alloc( sizeof(Tcl_Obj*)*Rows*Cols );
  for ( r = 0; r < Rows; ++r )
    for ( c = 0; c < Cols; ++c ) {
      dat = mat(r,c);
      if (dat.IsKnown())
        subitems[r*Cols+c] = Tcl_NewDoubleObj(dat.Value());
      else
        subitems[r*Cols+c] = Tcl_NewStringObj("?",-1);
    }
  items[IT_DATA] = Tcl_NewListObj( Rows*Cols, subitems );
  Tcl_SetListObj( obj_result,IT_SIZE, items );
  Tcl_Free( (char*)subitems );
  return TCL_OK;
}

/*
 * Fill in obj_result the content of the serie variable.
 *
 */

int Tol_GetSerieContent(Tcl_Interp * interp,
                        int objc,
                        Tcl_Obj *CONST objv[],
                        Tcl_Obj * obj_result)
{
  BGrammar * gra = BGrammar::FindByName("Serie");

  if (!gra) {
    Tcl_SetStringObj( obj_result, "grammar 'Serie' not found", -1);
    return TCL_ERROR;
  }
 
  Tcl_Obj * ser_name = objv[0];

  const BSyntaxObject* var = gra->FINDVARIABLE(Tcl_GetString(ser_name));
  if (!var) {
    Tcl_AppendStringsToObj( obj_result, "\"",
                            Tcl_GetString(ser_name),
                            "\" isn't a variable in grammar Serie", NULL );
    return TCL_ERROR;
  }

  // parse dates arguments

  
  BDate I0, I1;
  int tcl_code;

  BUserTimeSerie * serie = (BUserTimeSerie*)var;
  serie->ReadData();

  if (objc == 1) {
    I0 = serie->FirstDate();
    I1 = serie->LastDate();
  } else {
    tcl_code = Tol_ParseDate(interp,Tcl_GetString(objv[1]),I0);
    if (tcl_code != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    if (objc == 3) {
      tcl_code = Tol_ParseDate(interp,Tcl_GetString(objv[2]),I1);
      if (tcl_code != TCL_OK) {
        Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
        return TCL_ERROR;
      }
    }
  }
 
  if ( !I0.HasValue() ) 
    I0 = BDate::DefaultFirst();
  if ( !I1.HasValue() ) 
    I1 = BDate::DefaultLast();

  BUserTimeSet * tms;
  Tcl_Obj ** obj_array;
  int length;

  char buffer[1024];

  tms = serie->Dating();

  if ( !tms ) {
    Tcl_AppendStringsToObj(obj_result,"Serie \"",
                           Tcl_GetString(ser_name),
                           "\" does not have dating", NULL);
    return TCL_ERROR;
  }

  // ajustamos el intervalo de fechas al fechado

  I0 = tms->FirstNoLess(I0);
  I1 = tms->FirstNoGreat(I1);
  length = tms->Difference(I0,I1)+1;
  if ( length <= 0 ) {
    Tcl_SetStringObj(obj_result,
                     "last date must be greater or equal than first",-1);
    return TCL_ERROR;
  }

  BDat  value;
  BText tmp;
  Tcl_Obj * subitems[2];

  // ++ because we identify the TimeSet as the first element

  ++length;
  obj_array = (Tcl_Obj**)ckalloc(length*sizeof(Tcl_Obj*));

  //tmp = tms->Identify();
  tmp = GetObjectName(tms);
  obj_array[0] = Tcl_NewStringObj(tmp,-1);
  for ( int i = 1; i < length; i++ ) {
    tmp = I0.GetText();
    subitems[0] = Tcl_NewStringObj(tmp,-1);
#if defined(_TOL_FROZEN_)
    value = (*serie)[I0];
#else
    value = (*serie)(I0);
#endif
    if ( value.IsKnown() )
      subitems[1] = Tcl_NewDoubleObj(value.Value());
    else
      subitems[1] = Tcl_NewStringObj("?",-1);
    obj_array[i] = Tcl_NewListObj(2,subitems);
    I0 += tms;

    sprintf(buffer,"%d",i);
    Tcl_VarEval(interp, "puts ", buffer, NULL);
  }
  Tcl_SetListObj(obj_result,length,obj_array);
  Tcl_Free( (char*)obj_array );
  return TCL_OK;
}

int Tol_ParseDate(Tcl_Interp * interp, char * what, BDate & date)
{
  int    year, month, day, hour, min;
  double sec;
  
  int tcl_code;
  Tcl_Obj * tcl_result;
  int objc;
  Tcl_Obj ** objv;

  month = day = 1;
  hour  = min = 0;
  sec   = 0.0;
  char re_pat[] = "{^(?:y([0-9]+)(?:m([0-9]+))?(?:d([0-9]+))?)$}";

  tcl_code = Tcl_VarEval(interp, "regexp -inline ", re_pat, " ",what, NULL);
  if ( tcl_code != TCL_OK )
    return TCL_ERROR;
  tcl_result = Tcl_DuplicateObj(Tcl_GetObjResult(interp)); 
  /* this should be a valid list, unless Tcl be corrupt */
  Tcl_ListObjGetElements(interp,tcl_result,&objc,&objv);
  if ( objc == 0 ) {
    Tcl_AppendStringsToObj(Tcl_GetObjResult(interp),
                           "invalid date: \"",what,"\"",NULL);
    Tcl_DecrRefCount(tcl_result);
    return TCL_ERROR;
  }

  /* and objc must be 4 */

  Tcl_GetIntFromObj(interp,objv[1],&year);
  Tcl_GetIntFromObj(interp,objv[2],&month);
  Tcl_GetIntFromObj(interp,objv[3],&day);

  /* now is save to release tcl_result */

  Tcl_DecrRefCount(tcl_result);

  date.PutDate(year,month,day);
  if ( !date.HasValue() ) {
    Tcl_ResetResult(interp);
    char buffer[1024];
    sprintf(buffer,"y%dm%dd%d",year,month,day);
    tcl_result = Tcl_GetObjResult(interp);
    Tcl_AppendStringsToObj(tcl_result,
                           what," = ",buffer," is valid but unknown",NULL);
    return TCL_ERROR;
  }
  return TCL_OK;
}

/*
 * Set the language to be used  by TOL. Fill in obj_result the previous
 * language.
 *
 */

int Tol_SetLanguage(Tcl_Interp * interp, Tcl_Obj * lang, Tcl_Obj * obj_result)
{
  BBool curLang = BText::Language();
  BIdiomEnum nextLang;
 
  char * strLang = Tcl_GetString( lang );
  if ( !strcasecmp( strLang, "es" ) )
    nextLang = BSPANISH;
  else {
    /* anything else will be assumed as english */
    nextLang = BENGLISH;
  }
  /* set the previous language as result */
  if ( curLang == BSPANISH )
    Tcl_SetStringObj( obj_result, "es", -1 );
  else
    Tcl_SetStringObj( obj_result, "en", -1 );
  /* tell TOL what language to use from now on */
  BText::PutLanguage( nextLang );
  return TCL_OK;
}

/*
 * Return the language used  by TOL. Fill in obj_result the current
 * language.
 *
 */

int Tol_GetLanguage(Tcl_Interp * interp, Tcl_Obj * obj_result)
{
  BBool curLang = BText::Language();
  
  if ( curLang == BSPANISH )
    Tcl_SetStringObj( obj_result, "es", -1 );
  else
    Tcl_SetStringObj( obj_result, "en", -1 );
  return TCL_OK;
}

/*
 * Fill in obj_result the version info corresponding to tol
 *
 *   {major MajorNum minor MinorNum release ReleaseNum coment Comment}
 *
 */

int Tol_InfoVersion( Tcl_Interp * interp,  Tcl_Obj * obj_result )
{
#define VERSION_INFO_SIZE 4*2
 
  Tcl_Obj * items[VERSION_INFO_SIZE];
  char buffer[64];

  /* Major Info */
  items[0] = Tcl_NewStringObj( "major", -1 );
  items[1] = Tcl_NewIntObj( VERSION_MAJOR );
  /* Minor Info */
  items[2] = Tcl_NewStringObj( "minor", -1 );
  items[3] = Tcl_NewIntObj( VERSION_MINOR );
  /* Release Info */
  items[4] = Tcl_NewStringObj( "release", -1 );
  sprintf(buffer,"%d", VERSION_RELEASE);
  items[5] = Tcl_NewStringObj(buffer, -1);
  /* Comment Info */
  items[6] = Tcl_NewStringObj( "comment", -1 );
  items[7] = Tcl_NewStringObj(TOLVersionBuild(), -1 );
  /* asign the items to the result list */
  Tcl_SetListObj( obj_result, VERSION_INFO_SIZE, items ); 
  return TCL_OK;
}

/*
 * Fill in obj_result the list of files included in TOL
 *
 */


int Tol_InfoIncluded (Tcl_Interp * interp,Tcl_Obj * obj_result)
{
  BList* lst = BSetFromFile::Compiled();

  BText txt(LstText(lst,"","",","));
  return BList2TclList(interp,lst,obj_result,&GetPath,&HasNoSource);
}

/* OJO con la declaracion de tmpSet */

int Tol_InfoFile(Tcl_Interp * interp, Tcl_Obj * file,
                 Tcl_Obj * obj_result)
{
  Tcl_DString dstr;
  const BSyntaxObject * so;

  Tcl_DStringInit(&dstr);
  Tcl_UtfToExternalDString(NULL,Tcl_GetString(file),-1,&dstr);
  
  BText fname(Tcl_DStringValue(&dstr));
  so = (const BSyntaxObject*)BSetFromFile::FindCompiled(fname);
  Tcl_DStringFree(&dstr);
  if (!so) {
    Tcl_AppendStringsToObj(obj_result,"file \"",
                           Tcl_GetString(file),
                           "\" isn't included", NULL );
    return TCL_ERROR;
  }
  Tcl_Obj * info[6];
  /* adicionar la informacion del subtype y si tiene algun set
   * como descendiente
   */
  BText soInfo(so->Info());
  BText soPath(so->SourcePath());
  BText soDesc(so->Description());

  char * pch = soInfo;
  if (pch) pch = skipWhite(pch);
  else pch = "";

  Tcl_ExternalToUtfDString(NULL,pch,-1,&dstr);
  info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);

  Tcl_ExternalToUtfDString(NULL,soPath,-1,&dstr);
  info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);

  Tcl_ExternalToUtfDString(NULL,soDesc,-1,&dstr);
  info[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);

  /* HAS SUBSET */
  BSet * ptrSet = &(((BUserSet*)so)->Contens());
  int tmpCard = ptrSet->Card();
  int j;
  for (j=1; j<=tmpCard; ++j )
    if ( (*ptrSet)[j]->Grammar()==GraSet() )
      break;
  info[3] = Tcl_NewIntObj(j<=tmpCard);
    
  /* SUBTYPE INFO */
  info[4] = Tcl_NewIntObj( ptrSet->SubType() );
  /* STRUCTURE */
  info[5] = Tcl_NewStringObj( NULL, 0 );
  Tcl_SetListObj(obj_result, 6, info);
  return TCL_OK;
}

/*
 * Fill in obj_result the result of evaluating including the file
 * given as argument: the absolute path.
 *
 */

int Tol_IncludeFile(Tcl_Interp * interp,Tcl_Obj * obj_file,Tcl_Obj * obj_result)
{
  Tcl_DString dstr;
  BText fName;

  Tcl_DStringInit(&dstr);
  fName = Tcl_UtfToExternalDString(NULL,Tcl_GetString(obj_file),-1,&dstr);
  BSetFromFile* uSet = (BSetFromFile*)BSetFromFile::FindCompiled(fName);
  Tcl_DStringFree(&dstr);

  if(uSet) {
    DESTROY(uSet);
  }
  IncludeFile(fName);
  return TCL_OK;
}

int Tol_DecompileFile( Tcl_Interp * interp,
                       Tcl_Obj * obj_file,
                       Tcl_Obj * obj_result )
{
  Tcl_DString dstr;
  BText fName;

  Tcl_DStringInit(&dstr);
  fName = Tcl_UtfToExternalDString(NULL,Tcl_GetString(obj_file),-1,&dstr);
  BSetFromFile* uSet = (BSetFromFile*)BSetFromFile::FindCompiled(fName);
  Tcl_DStringFree(&dstr);

  if(uSet) {
    DESTROY(uSet);
  } else {
    Tcl_AppendStringsToObj( obj_result, "file '",
                            Tcl_GetString(obj_file),
                            "' is not compiled", NULL );
    return TCL_ERROR;
  }
  if ( uSet ) {
    Tcl_AppendStringsToObj( obj_result, "could not decompile file '",
                            Tcl_GetString(obj_file), "'", NULL );
    return TCL_ERROR;
  }
  return TCL_OK;
}
/*
 *
 * Fill in obj_result the list of grammars defined in TOL
 *
 */

int Tol_SetGrammarsObj(Tcl_Interp * interp, Tcl_Obj * obj_result )
{

  BList* lstGra = BGrammar::Instances();

  if (BList2TclList(interp,lstGra,obj_result,&GetName,&ExcludeAnything)==TCL_OK) {
    Tcl_Obj * name = Tcl_NewStringObj("Anything", -1);
    if (Tcl_ListObjAppendElement(interp,obj_result,name) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    if (QuickSort(interp,obj_result) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
  return TCL_ERROR;
}

/*
 *
 * Fill in obj_result the description of gra_name.
 *
 */

int Tol_SetGrammarDescObj(Tcl_Interp * interp, Tcl_Obj * gra_name,
                          Tcl_Obj * obj_result )
{
  BGrammar * gra = BGrammar::FindByName(Tcl_GetString(gra_name));

  if (gra) {
    BText desc(gra->Description());
    Tcl_SetStringObj(obj_result, desc, -1);
    return TCL_OK;
  }
  Tcl_AppendStringsToObj( obj_result, "\"",
                          Tcl_GetString(gra_name),
                          "\" isn't a grammar", NULL );
  return TCL_ERROR;
}

/*
 *  Tol_FillFunctionInfo --
 *
 *    Used in Tol_SetFunctionsObj & Tol_SetFunctionInfoObj to return the info
 *    for the function requested.
 */

static
int Tol_FillFunctionInfo(BOperator * opr, Tcl_Obj * obj_result)
{
  Tcl_Obj * info[3];
  Tcl_DString dstr_info;
  Tcl_DString dstr;

  Tcl_DStringInit(&dstr_info);
  BText args(opr->Arguments());
  
  if(args.Last()!='}') // name doesn't comes within the arguments expression
  {   
      // grammar name
      Tcl_DStringAppend(&dstr_info, opr->Grammar()->Name(), -1);
      Tcl_DStringAppend(&dstr_info, " ", -1);
      // function name
      Tcl_DStringInit(&dstr);
      Tcl_ExternalToUtfDString(NULL, opr->Name(), -1, &dstr); 
      Tcl_DStringAppend(&dstr_info, Tcl_DStringValue(&dstr), -1);
      Tcl_DStringFree(&dstr);
  }

  Tcl_ExternalToUtfDString(NULL,args.String(),-1,&dstr);
  Tcl_DStringAppend(&dstr_info, Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr_info), -1);
  Tcl_DStringFree(&dstr_info);
  Tcl_ExternalToUtfDString(NULL,opr->SourcePath(),-1,&dstr);
  info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  Tcl_ExternalToUtfDString(NULL,opr->Description(),-1,&dstr);
  info[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  Tcl_SetListObj(obj_result, 3, info);
  return TCL_OK;
}

/*
 *
 * Fill in obj_result the functions defined for grammar gra_name.  If gra_name
 * is a list it is considered as an object reference {set i1 ... ik}
 *
 */

int Tol_SetFunctionsObj (Tcl_Interp * interp, Tcl_Obj * gra_name,
                         Tcl_Obj * obj_result)
{
  Tcl_Obj * name;
  Tcl_Obj ** items;
  int n;
  int tcl_code;

  /* first try gra_name as a list */
  
  tcl_code = Tcl_ListObjGetElements(interp,gra_name,&n,&items);
  if (tcl_code != TCL_OK) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  if ( n > 1 ) {
    /* is a list & should be a member of a set */
    
    const BSyntaxObject * syn = Tol_FindSet( items[0], obj_result );

    if ( !syn )
      return TCL_ERROR;
    syn = Tol_FindChild(interp,syn,items+1,n-1,obj_result);
    if ( !syn )
      return TCL_ERROR;
    if (syn->Grammar() != GraCode()) {
      Tcl_AppendStringsToObj(obj_result, "'", Tcl_GetString(gra_name),
			     "' is not a function object", NULL);
      return TCL_ERROR;
    }
    BOperator * opr =  ((BGraContens<BCode>*)syn)->Contens().Operator();
    return Tol_FillFunctionInfo(opr, obj_result);
  }
  /* gra_name should be a grammar name */
  if (!strcmp(Tcl_GetString(gra_name), "Anything")) {
    const BSpFunHash& spf = BSpecialFunction::Instances();
    BSpFunHash::const_iterator iter;
    for(iter=spf.begin(); iter!=spf.end(); iter++)
    {
      name = Tcl_NewStringObj(iter->second->Name(), -1);
      Tcl_ListObjAppendElement(interp, obj_result, name);
    }
    if (QuickSort(interp,obj_result) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    return TCL_OK;
  }

  BGrammar * gra = BGrammar::FindByName(Tcl_GetString(gra_name));
  
  if (gra) {
    BList * lstOper = gra->GetOperators();
    if (BList2TclList(interp,lstOper,obj_result,&GetName) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      DESTROY(lstOper);
      return TCL_ERROR;
    }
    DESTROY(lstOper);
    if (QuickSort(interp,obj_result) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  Tcl_AppendStringsToObj( obj_result, "\"",
                          Tcl_GetString(gra_name),
                          "\" isn't a grammar", NULL );
  return TCL_ERROR;
}

/*
 * Fill in obj_result the function's prototype and description
 * for a given function in a given grammar.
 *
 */

int Tol_SetFunctionInfoObj (Tcl_Obj * gra_name,
                            Tcl_Obj * fun_name,
                            Tcl_Obj * obj_result)
{
  Tcl_DString dstr;
  Tcl_Obj * info[3];

  if (!strcmp(Tcl_GetString(gra_name), "Anything")) {
    Tcl_DStringInit(&dstr);
    Tcl_UtfToExternalDString(NULL,Tcl_GetString(fun_name),-1,&dstr);
    const BSpecialFunction * spf = BSpecialFunction::Get(Tcl_DStringValue(&dstr));
    Tcl_DStringFree(&dstr);
    if (spf) {
      char * name, * args;
      size_t len;
      
      name = ((BText&)spf->Name()).Buffer();
      args = ((BText&)spf->Arguments()).Buffer();
      len = strlen(args);
      if(!len || args[len-1]!='}') {
        info[0] = Tcl_NewStringObj("Anything ", -1);
        Tcl_AppendStringsToObj(info[0],name,args,NULL);
      } else {
        info[0] = Tcl_NewStringObj("", -1);;
        Tcl_AppendStringsToObj(info[0],args,NULL);
      }
      info[1] = Tcl_NewStringObj("",-1);
      Tcl_ExternalToUtfDString(NULL,spf->Description(),-1,&dstr);
      info[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
      Tcl_DStringFree(&dstr);
      Tcl_SetListObj(obj_result, 3, info);
      return TCL_OK;
    }
    Tcl_AppendStringsToObj( obj_result, "\"",
                            Tcl_GetString(fun_name),
                            "\" isn't a function in grammar Anything", NULL );
  } else {
    BGrammar * gra = BGrammar::FindByName(Tcl_GetString(gra_name));
  
    if (gra) {
      Tcl_DStringInit(&dstr);
      Tcl_UtfToExternalDString(NULL,Tcl_GetString(fun_name),-1,&dstr);
      BOperator* opr = gra->FindOperator(Tcl_DStringValue(&dstr));
      Tcl_DStringFree(&dstr);
      if (opr) {
	return Tol_FillFunctionInfo(opr, obj_result);
	/*
	Tcl_DString dstr_info;
	Tcl_DStringInit(&dstr_info);
	Tcl_DStringAppend(&dstr_info, Tcl_GetString(gra_name), -1);
	Tcl_DStringAppend(&dstr_info, " ", -1);
	// dstr comes with the function name
	Tcl_DStringAppend(&dstr_info, Tcl_DStringValue(&dstr), -1);
	Tcl_DStringFree(&dstr);
        Tcl_ExternalToUtfDString(NULL,opr->Arguments(),-1,&dstr);
	Tcl_DStringAppend(&dstr_info, Tcl_DStringValue(&dstr), -1);
        Tcl_DStringFree(&dstr);
        //Tcl_ExternalToUtfDString(NULL,opr->Info(),-1,&dstr);
        info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr_info), -1);
        Tcl_DStringFree(&dstr_info);
        Tcl_ExternalToUtfDString(NULL,opr->SourcePath(),-1,&dstr);
        info[1] = Tcl_NewStringObj(dstr.string, -1);
        Tcl_DStringFree(&dstr);
        Tcl_ExternalToUtfDString(NULL,opr->Description(),-1,&dstr);
        info[2] = Tcl_NewStringObj(dstr.string, -1);
        Tcl_DStringFree(&dstr);
        Tcl_SetListObj(obj_result, 3, info);
        return TCL_OK;
	*/
      }
      // release the previous use of dstr
      //Tcl_DStringFree(&dstr);
      Tcl_AppendStringsToObj( obj_result, "\"",
                              Tcl_GetString(fun_name),
                              "\" isn't a function in grammar ",
                              Tcl_GetString(gra_name), NULL );
    } else
      Tcl_AppendStringsToObj( obj_result, "\"",
                              Tcl_GetString(gra_name),
                              "\" isn't a grammar", NULL );
  }
  return TCL_ERROR;
}

/* ojo info debe tener espacio para 7 Tcl_Obj como maximo */
/* retorna cuantos Tcl_Obj creo */

int SynObj2TclObj( const BSyntaxObject * var, Tcl_Obj * info[] )
{
  Tcl_DString dstr;
  int infoc;
  
  Tcl_DStringInit(&dstr);
  BText varInfo(var->Info());
  BText varPath(var->SourcePath());
  BText varDesc(var->Description());
  
  /* OBJECT CONTENT */
  char * pch = varInfo;
  if (pch) pch = skipWhite(pch);
  else pch = "";
  
  Tcl_ExternalToUtfDString(NULL,pch,-1,&dstr);
  info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  
  /* OBJECT PATH */
  Tcl_ExternalToUtfDString(NULL,varPath,-1,&dstr);
  info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  
  /* OBJECT DESCRIPTION */
  Tcl_ExternalToUtfDString(NULL,varDesc,-1,&dstr);
  info[2] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
  Tcl_DStringFree(&dstr);
  
#ifdef ISFILE_IMPLEMENTED
  info[3] = Tcl_NewIntObj(var->IsFile());
#else
  info[3] = Tcl_NewIntObj(0);
#endif
  //info[4] = Tcl_NewIntObj(var->System());
  
  if ( (var->Mode()==BOBJECTMODE) && (var->Grammar() == GraSet() )) {
    BSet * ptrSet = &(((BUserSet*)var)->Contens());
    /* HAS SUBSET */
    int tmpCard = ptrSet->Card();
    int j;
    for (j=1; j<=tmpCard; ++j )
      if ( (*ptrSet)[j]->Grammar()==GraSet() )
        break;
    info[4] = Tcl_NewIntObj(j<=tmpCard);
      
    /* SUBTYPE */
    info[5] = Tcl_NewIntObj( ptrSet->SubType() );
      
    /* STRUCTURE */
    if ( ptrSet->Struct() ) {
      BText btxt( ptrSet->Struct()->Name() );
      Tcl_ExternalToUtfDString(NULL,btxt,-1,&dstr);
      info[6] = Tcl_NewStringObj( Tcl_DStringValue(&dstr), -1 );
      Tcl_DStringFree( &dstr );
    } else
      info[6] = Tcl_NewStringObj( NULL, 0 );
    infoc = 7;
  } else
    infoc = 4;
  
  return infoc;
}

/*!
 * Fill in obj_result the variables defined for grammar gra_name.
 * Or the element set info if gra_name is a reference of an element 
 * of a global set.
 */
int Tol_SetVariablesObj (Tcl_Interp * interp, Tcl_Obj * gra_name,
                         Tcl_Obj * obj_result)
{
  BGrammar * gra;
  /* verificar primero si gra_name es la referencia a un item de un
   * conjunto, esto es [llength $gra_name] > 1
   */

  Tcl_Obj ** items;
  int n;
  int tcl_code;
    
  /* look for subset given in objv[1]*/
  tcl_code = Tcl_ListObjGetElements(interp,gra_name,&n,&items);
  if (tcl_code != TCL_OK) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  if ( n > 1 ) {
    const BSyntaxObject * syn;

    if (!(syn=Tol_ResolveObject(interp, gra_name, NULL))) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    Tcl_DString dstr;
    Tcl_Obj * info[9];
    int infoc;

    BText grname(syn->Grammar()->Name());
    //BText synname(syn->Identify());
    BText synname(GetObjectName(syn));

    /* GRAMMAR */
    info[0] = Tcl_NewStringObj(grname, -1);

    /* NAME */
    Tcl_DStringInit(&dstr);
    Tcl_ExternalToUtfDString(NULL,synname,-1,&dstr);
    info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
    Tcl_DStringFree(&dstr);
    
    /* REST OF FIELDS */
    infoc = SynObj2TclObj( syn, info+2 ) + 2;
    Tcl_SetListObj(obj_result,infoc,info);
    return TCL_OK;
  }
  gra = BGrammar::FindByName(Tcl_GetString(gra_name));
  if (gra) {
    BList * lstVar = gra->GetVariables();
    if (BList2TclList(interp,lstVar,obj_result,&GetIdentify) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      DESTROY(lstVar);
      return TCL_ERROR;
    }
    DESTROY(lstVar);
    if (QuickSort(interp,obj_result) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  Tcl_AppendStringsToObj( obj_result, "\"",
                          Tcl_GetString(gra_name),
                          "\" isn't a grammar", NULL );
  return TCL_ERROR;

  // The code below is death!!!!!

  /* look for subset given in objv[1]*/
  tcl_code = Tcl_ListObjGetElements(interp,gra_name,&n,&items);
  if (tcl_code != TCL_OK) {
    Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
    return TCL_ERROR;
  }
  if ( n > 1 ) {
    /* is a list & should be a member of a set */
    
    const BSyntaxObject * syn = Tol_FindSet( items[0], obj_result );
    Tcl_DString dstr;

    if ( !syn )
      return TCL_ERROR;
    syn = Tol_FindChild(interp,syn,items+1,n-1,obj_result);
    if ( !syn )
      return TCL_ERROR;
    Tcl_Obj * info[9];
    int infoc;
    BText grname(syn->Grammar()->Name());
    //BText synname(syn->Identify());
    BText synname(GetObjectName(syn));

    /* GRAMMAR */
    Tcl_ExternalToUtfDString(NULL,grname,-1,&dstr);
    info[0] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
    Tcl_DStringFree(&dstr);

    /* NAME */

    Tcl_ExternalToUtfDString(NULL,synname,-1,&dstr);
    info[1] = Tcl_NewStringObj(Tcl_DStringValue(&dstr), -1);
    Tcl_DStringFree(&dstr);
    
    /* REST OF FIELDS */
    infoc = SynObj2TclObj( syn, info+2 ) + 2;
    Tcl_SetListObj(obj_result,infoc,info);
    return TCL_OK;
  }
  gra = BGrammar::FindByName(Tcl_GetString(gra_name));
  if (gra) {
    BList * lstVar = gra->GetVariables();
    if (BList2TclList(interp,lstVar,obj_result,&GetIdentify) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      DESTROY(lstVar);
      return TCL_ERROR;
    }
    DESTROY(lstVar);
    if (QuickSort(interp,obj_result) != TCL_OK) {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
    return TCL_OK;
  }
  Tcl_AppendStringsToObj( obj_result, "\"",
                          Tcl_GetString(gra_name),
                          "\" isn't a grammar", NULL );
  return TCL_ERROR;
}

/*
 * Fill in obj_result the variables's description for a given variable 
 * in a given grammar.
 *
 */

int Tol_SetVariableInfoObj (Tcl_Obj * gra_name,
                            Tcl_Obj * var_name,
                            Tcl_Obj * obj_result)
{
  BGrammar * gra = BGrammar::FindByName(Tcl_GetString(gra_name));
  Tcl_DString dstr;
  
  Tcl_DStringInit(&dstr);
  if (gra) {
    Tcl_UtfToExternalDString(NULL,Tcl_GetString(var_name),-1,&dstr);
    const BSyntaxObject* var = gra->FINDVARIABLE(Tcl_DStringValue(&dstr));
    Tcl_DStringFree(&dstr);

    if (var) {
      Tcl_Obj * info[7];
      int infoc;
      infoc = SynObj2TclObj( var, info );
      Tcl_SetListObj(obj_result,infoc,info);
//#define __INFO_PRINT_NREFS__
#ifdef __INFO_PRINT_NREFS__
      printf("NREF = %d\n", var->NRefs());
#endif
      return TCL_OK;
    }
    Tcl_AppendStringsToObj( obj_result, "\"",
                            Tcl_GetString(var_name),
                            "\" isn't a variable in grammar ",
                            Tcl_GetString(gra_name), NULL );
  } else
    Tcl_AppendStringsToObj( obj_result, "\"",
                            Tcl_GetString(gra_name),
                            "\" isn't a grammar", NULL );
  return TCL_ERROR;
}

/*
 * We must be sure that the strings use 7 bit ASCII chararters.
 * Otherwise we must take care of the Utf representation.
 *
 */

static
int StrObjCompare(const void * O1, const void * O2)
{
  char * str1 = (*(Tcl_Obj**)O1)->bytes;
  char * str2 = (*(Tcl_Obj**)O2)->bytes;
  int check1, check2;

  check1 = isalpha(str1[0]);
  check2 = isalpha(str2[0]);

  if ( check1 && !check2 )
    return 1;
  if ( check2 && !check1 )
    return -1;
  return strcmp(str1,str2);
}

static
int QuickSort( Tcl_Interp * interp, Tcl_Obj * list )
{
  Tcl_Obj ** obj_array;
  int length;

  if (Tcl_ListObjGetElements(interp, list, &length, &obj_array) != TCL_OK) {
    return TCL_ERROR;
  }
  qsort((void*)obj_array, length, sizeof(Tcl_Obj*), &StrObjCompare);
  return TCL_OK;
}

static
void Tol_HciWriter( const BText & str )
{
  Tcl_CmdInfo cmd_info;

  if (TT_interp && Tcl_GetCommandInfo(TT_interp,"Tol_HciWriter",&cmd_info) ) {
    Tcl_DString dstr;
    Tcl_Obj * objv[2];
    
    Tcl_DStringInit(&dstr);
    Tcl_IncrRefCount(objv[0]=Tcl_NewStringObj("Tol_HciWriter",-1));
    Tcl_ExternalToUtfDString(NULL, ((BText&)str).Buffer(), -1, &dstr);
    Tcl_IncrRefCount(objv[1]=Tcl_NewStringObj(Tcl_DStringValue(&dstr), Tcl_DStringLength(&dstr)));
    Tcl_DStringFree(&dstr);
  
    int status = Tcl_EvalObjv(TT_interp, 2, objv, TCL_EVAL_GLOBAL);
    if (status != TCL_OK) {
      printf("message: %s\n", ((BText&)str).Buffer());
      printf("objv[1] : %s\n", Tcl_GetString(objv[1]));
      printf("%s\n", TT_interp->result);
    }
    /* release the objects */
    Tcl_DecrRefCount(objv[0]);
    Tcl_DecrRefCount(objv[1]);
  }
}

void Tol_InstallHciWriter()
{
  BOut::PutHciWriter(Tol_HciWriter);
  BOut::PutLogHciWriter(Tol_HciWriter);
}

static 
void Tol_SerieChart(BList * series, const BText& FileName )
{
#define CMD_SERIECHART "Tol_SerieChartMethod"
#define GRP_NAME "__grptmp_chart__%d"

  Tcl_CmdInfo cmd_info;
  static int id = 0;

  if (TT_interp && Tcl_GetCommandInfo(TT_interp,CMD_SERIECHART,&cmd_info) ) {
    Tcl_DString dstr;
    Tcl_Obj * objv[3];
    Tcl_Obj * obj_result=Tcl_NewObj();
    int tcl_result;
    char grpname[1024];
    
    Tcl_IncrRefCount(obj_result);

    // first create the group of series
    sprintf(grpname, GRP_NAME, ++id);
    tcl_result = Tol_CreateSerieGrp(TT_interp, grpname, series, obj_result);
    if (tcl_result==TCL_ERROR) {
      printf("%s -- error in Tol_CreateSerieGrp: %s\n", CMD_SERIECHART,
             Tcl_GetString(obj_result));
      Tcl_DecrRefCount(obj_result);
      return;
    }
    // release the obj_result
    Tcl_DecrRefCount(obj_result);    
    // chart command
    Tcl_IncrRefCount(objv[0]=Tcl_NewStringObj(CMD_SERIECHART,-1));
    // serie group argument
    Tcl_IncrRefCount(objv[1]=Tcl_NewStringObj(grpname,-1));    
    // file name argument
    Tcl_DStringInit(&dstr);
    Tcl_ExternalToUtfDString(NULL, ((BText&)FileName).Buffer(), -1, &dstr);
    Tcl_IncrRefCount(objv[2]=Tcl_NewStringObj(Tcl_DStringValue(&dstr), Tcl_DStringLength(&dstr)));
    Tcl_DStringFree(&dstr);
    int status = Tcl_EvalObjv(TT_interp, 3, objv, TCL_EVAL_GLOBAL);
    if (status != TCL_OK) {
      printf("%s -- error in Tcl_EvalObjv:\n", CMD_SERIECHART);
      printf("%s\n", TT_interp->result);
      printf("objv[1] : %s\n", Tcl_GetString(objv[1]));
      printf("objv[2] : %s\n", Tcl_GetString(objv[2]));
    }
    /* release the objects */
    for (int i=0; i<3; i++) 
      Tcl_DecrRefCount(objv[i]);
    /* the group of series should be destroyied in the Hook */
    /*Tcl_VarEval(TT_interp, "tol::seriegrp destroy ", grpname, NULL);*/
  }
}

int Tol_Tcl_Eval(char * script, const char ** result)
{
  int status;
  
  status = Tcl_Eval(TT_interp, script);
  *result = Tcl_GetStringResult(TT_interp);
  return status==TCL_OK;
}

Tcl_Obj *TolObj2TclObj(BSyntaxObject *tolObj)
{
  const char *grammar = tolObj->Grammar()->Name().String();

  if (!strcmp(grammar, "Text")) {
    const BText &value = Text(tolObj);
    return Tcl_NewStringObj(value.String(),-1);
  } else if (!strcmp(grammar, "Real")) {
    const BDat &value = Dat(tolObj);
    return Tcl_NewDoubleObj(value.Value());
  } else {
    return Tcl_NewStringObj(tolObj->Dump().String(),-1);    
  }
}

int Tol_Tcl_EvalEx(const BSet &Args, const char ** result)
{
  int status;
  Tcl_Obj **objv;
  int card = Args.Card();
  
  if (!Args.Card()) {
    *result = "";
    status = TCL_OK;
  } else {
    objv = (Tcl_Obj**)Tcl_Alloc(sizeof(Tcl_Obj*)*card);
    for(int i=1; i<=card; i++) {
      objv[i-1] = TolObj2TclObj(Args[i]);
      Tcl_IncrRefCount(objv[i-1]);
    }
    status = Tcl_EvalObjv(TT_interp, card, objv, TCL_EVAL_GLOBAL);
    *result = Tcl_GetStringResult(TT_interp);
    for(int i=0; i<card; i++) {
      Tcl_DecrRefCount(objv[i]);
    }
  }
  return status==TCL_OK;
}

int Tol_StopEvaluation()
{
  StopFlagOn();
  return TCL_OK;
}

/*
 * Fill in obj_result the format .
 *
 */

int Tol_LCFormat(Tcl_Interp*    interp,
                 Tcl_Obj *CONST obj_value,
                 Tcl_Obj *      obj_result)
{
  double value;
  char buffer[256];
  
  /* read the double value from tcl obj */
  if (Tcl_GetDoubleFromObj(TT_interp, obj_value, &value)==TCL_ERROR)
    {
      Tcl_AppendObjToObj(obj_result,Tcl_GetObjResult(interp));
      return TCL_ERROR;
    }
  lc_format(value, buffer);
  Tcl_AppendStringsToObj(obj_result, buffer, NULL);
  return TCL_OK;
}

void Tol_InstallMethodsHook()
{
  InstallSerieChartHook(&Tol_SerieChart);
  InstallTclEval(&Tol_Tcl_Eval);
  InstallTclEvalEx(&Tol_Tcl_EvalEx);
}
