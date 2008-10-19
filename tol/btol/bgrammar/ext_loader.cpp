/* ext_loader.cpp: BGrammar methods for Extensions Loader of TOL language.

   Copyright (C) 2006 - Bayes Decision, SL (Spain [EU])
   author: Daniel Rus <danirus@tol-project.org>

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

#include <tol/tol_bgrammar.h>
#include <tol/tol_bext.h>

#ifdef __USE_EXT_LOADER__

#ifdef UNIX
#  include <dlfcn.h>
#  define GET_FUNC dlsym
#  define UNLOAD_MODULE dlclose
#  define CHECK_ERROR(f) dlerror()
#  define LOAD_MODULE(f) dlopen(f,RTLD_NOW|RTLD_GLOBAL)
#  define EXTENSION ".so"
   typedef void* hdl_type;
#else // Windows
char *checkF(void *f) 
{
  if (f==NULL) 
    return "Could not find call.";
  else return NULL;
}
#  include <windows.h>
#  define GET_FUNC GetProcAddress
#  define UNLOAD_MODULE !FreeLibrary
#  define CHECK_ERROR(f) checkF(f)
#  define LOAD_MODULE(f) LoadLibrary(f)
#  define EXTENSION ".dll"
   typedef HINSTANCE hdl_type;
#endif // UNIX

#define RC_GOOD  1
#define RC_ERROR 0

xtd_bif_list* BGrammar::extensions_ = NULL;

//--------------------------------------------------------------------
int BGrammar::Ext_Load(const BText& extName)
{
    int rc = RC_GOOD;
    char* errormsg = NULL;
    BExtension *tolext_loaded = NULL;

    /* Load the Module */
    hdl_type module = NULL;
    // module = lt_dlopenext(extName.String());
    module = LOAD_MODULE(extName.String());
    
    if(!module)
	// errormsg = (char*) lt_dlerror();
	errormsg = CHECK_ERROR(module);

    /* Find the entry point */
    if(module)
    {
	//void* symbol = lt_dlsym(module, "Tol_Ext");
	void* symbol = GET_FUNC(module, "TolExt");
	//if(!symbol) errormsg = (char*) lt_dlerror();
	if(!symbol) errormsg = (char*) CHECK_ERROR(symbol);
	tolext_loaded = (BExtension*) symbol;
	
	//if(errormsg) lt_dlclose(module);
	if(errormsg) UNLOAD_MODULE(module);
    }

    if(errormsg) 
    {
	Error(errormsg);
	module = NULL;
	rc = RC_ERROR;
    }

    if(module && rc)
    {
	/* Store a new BExtension */
	void *symbol = malloc(sizeof(xtd_bif_list));
	if(!symbol)
	    Error("Memory exhausted!");

	xtd_bif_list* entry = (xtd_bif_list*) symbol;
	entry->next = NULL;
	entry->module = module;
	entry->name = extName.DupBuffer();
	entry->tolext = tolext_loaded;

	if(extensions_) 
	    entry->next = extensions_;

	extensions_ = entry;
    }

    return rc;
}

//--------------------------------------------------------------------
BOperator* BGrammar::Ext_FindOperator(const BText& name) const
{
    BOperator* oper = 0;
    xtd_bif_list* aux_list = BGrammar::extensions_;

    while(aux_list && !oper)
    {
	if(aux_list->tolext) 
	    oper = aux_list->tolext->findOperator(name, this);
	
	aux_list = aux_list->next;
    }

    return oper;
}

//--------------------------------------------------------------------
int BGrammar::Ext_Unload(const BText& extName)
{
    int rc = RC_GOOD;
    bool found = false;

    xtd_bif_list 
	*cur_list = extensions_,
	*prev_list = NULL;

    while(cur_list && !found)
    {
	if(!strcmp(extName.String(), cur_list->name))
	{
	    /* Remove from Symbols Table */
	    BExtension* ext = cur_list->tolext;
	    ext->removeOperators();
	    
	    /* dl_lthandle close */

	    if(UNLOAD_MODULE(cur_list->module)) // ERROR
	    {
		//char* errormsg = (char*) lt_dlerror();
		char* errormsg = (char*) CHECK_ERROR();
		Error(errormsg);
		rc = RC_ERROR;
	    } 

	    /* Remove from Extensions List */
	    if(!prev_list) /* is the 1st. element */
		extensions_ = cur_list->next;
	    else prev_list->next = cur_list->next;

	    free(cur_list);
	    found = true;
	}
	else
	{
	    prev_list = cur_list;
	    cur_list = cur_list->next;
	}
    }

    if(!found)
    {
	Error(I2("Extension not found\n",
		 "No se ha encontrado la Extensión"));
	rc = RC_ERROR;
    }

    return rc;
}

#endif
