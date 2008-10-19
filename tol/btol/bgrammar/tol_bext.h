/* tol_bext.h: BExtension class declaration of TOL language.

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

#ifndef TOL_BEXT_H
#define TOL_BEXT_H 1

#include <tol/tol_boper.h>

class BOperator;

typedef struct boperator_list {
    struct boperator_list *next;
    BOperator* op;
} boperator_list;

class BExtension 
{
 private:
    char* name_;
    boperator_list* op_list_;
    
 public:
    BExtension(const char* name)
	: name_(0), op_list_(0)
	{
	    name_ = strdup(name);
	}
    
    ~BExtension();

    char* Name() const { return name_; }

    bool addOperator(BOperator* oper);
    BOperator* findOperator(const BText& name, const BGrammar* gra) const;
    void removeOperators();
};

#endif /* TOL_BEXT_H */
