/* tolext.cpp: Methods definition for BTolExtension class.

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

#include <tol/tol_bext.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bsymboltable.h>

//--------------------------------------------------------------------
BExtension::~BExtension()
{
    boperator_list* aux_list = op_list_;

    while(aux_list)
    {
	op_list_ = aux_list->next;
	free(aux_list);
	aux_list = op_list_;
    }
    
    delete name_;
}

//--------------------------------------------------------------------
bool BExtension::addOperator(BOperator* oper)
{
    void* vp = malloc(sizeof(boperator_list));
    if(!vp)
    {
	Error("Memory exhausted!");
	return false;
    }

    boperator_list* entry = (boperator_list*) vp;
    entry->next = NULL;
    entry->op = oper;

    if(op_list_)
	entry->next = op_list_;

    op_list_ = entry;
    
    return true;
}

//--------------------------------------------------------------------
BOperator* BExtension::findOperator(const BText& name, 
				    const BGrammar* gra) const
{
    BOperator* oper = NULL;
    boperator_list* aux_list = op_list_;
    
    while(aux_list && !oper)
    {
	if(aux_list && aux_list->op && 
	   (aux_list->op->Name()==name) &&
	   (aux_list->op->Grammar()==gra)) 
	{ 
	    oper = aux_list->op;
	}
	else { aux_list = aux_list->next; }
    }
    
    return oper;
}

//--------------------------------------------------------------------
void BExtension::removeOperators()
{
    
    boperator_list* cur_list = op_list_;

    while(cur_list)
    {
      BObjClassify oc(cur_list->op->Grammar(), BBUILTINFUNMODE);
      BSyntaxObject* oper = 
	BGrammar::SymbolTable().Search(oc, cur_list->op->Name());
      if(oper) BGrammar::SymbolTable().Kill(oper);
      cur_list = cur_list->next;
    }
}

