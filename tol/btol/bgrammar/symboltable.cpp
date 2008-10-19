/* symboltable.cpp: Stores and manages the global symbols table of TOL 
       for GNU/TOL Language.

   Copyright (C) 2003-2005 Bayes Decision, SL (Spain [EU])

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
#include <win_tolinc.h>
#endif

#include <tol/tol_bsymboltable.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bout.h>
#include <tol/tol_barith.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bstat.h>

BTraceInit("txthash.cpp");



//--------------------------------------------------------------------
BSymbolTable::BSymbolTable()
//! BSymbolTable constructor
//--------------------------------------------------------------------
: symbolsByClassName_(),
  symbolsByNameClass_()
  
{
//Initializes hash_map's
  SetEmptyKey  (symbolsByNameClass_, NULL);
  SetDeletedKey(symbolsByNameClass_, name_del_key());
  SetEmptyKey  (symbolsByClassName_, BObjClassify::EmptyKey ());
  SetDeletedKey(symbolsByClassName_, BObjClassify::DeletedKey());
}

//--------------------------------------------------------------------
BInt BSymbolTable::Add(BSyntaxObject* obj)
//! Adds a new symbol
//--------------------------------------------------------------------
{
  int result = 1;
  BGrammar* gra = obj->Grammar();
  int mode = obj->Mode();
  BObjClassify oc(gra,mode);
  const char* name = obj->String();
#ifndef NDEBUG
  assert(BParser::DefaultParser()->Filter()->IsIdentifier(name) || 
         (mode==BBUILTINFUNMODE) || 
         (mode==BSOURCEPATHMODE));
  BSyntaxObject* old = Search(oc,name);
  if(old)
  {
    if(AreCompatible(old,obj))
    {
      result=0;
      old=NULL;
    }
  }
  if(old)
  {
    //name is in a currently used NameBlock
    Error(old->LexInfo()+
          I2(" is already in use as global. "
             "So cannot add ",
             " ya se está usando como global. "
             "Por lo tanto no se puede añadir ")+
          obj->LexInfo());
    return(-1);
  }
#endif
  const char * key_name;

//Inserting in name-class hash_map
  {
    BObjByNameClassHash::const_iterator fc =
symbolsByNameClass_.find(name);
    BObjByClassHash* x;
    if(fc!=symbolsByNameClass_.end())
    {
      key_name = name;
      x = fc->second;
    }
    else
    {
      x = new BObjByClassHash;
      SetEmptyKey  ((*x), BObjClassify::EmptyKey  ());
      SetDeletedKey((*x), BObjClassify::DeletedKey());
      key_name = strdup(name);
      symbolsByNameClass_[key_name] = x;
      assert(symbolsByNameClass_.find(key_name)!=
             symbolsByNameClass_.end());
    }
    assert(x->find(oc)==x->end());
    (*x)[oc]=obj;
    assert(x->find(oc)!=x->end());
  }
//Inserting in class-name hash_map
  {
    BObjByClassNameHash::const_iterator fc = symbolsByClassName_.find(oc);
    BObjByNameHash* x;
    if(fc!=symbolsByClassName_.end())
    {
      x = fc->second;
    }
    else
    {
      x = new BObjByNameHash;
      SetEmptyKey  ((*x), NULL);
      SetDeletedKey((*x), name_del_key());
      symbolsByClassName_[oc] = x;
      assert(symbolsByClassName_.find(oc)!=symbolsByClassName_.end());
    }
    assert(x->find(name)==x->end());
    (*x)[key_name]=obj;
    assert(x->find(key_name)!=x->end());
  }
  return(result);
}


//--------------------------------------------------------------------
BInt BSymbolTable::Kill(BSyntaxObject* obj)
//! Eliminates an stored  symbol
//--------------------------------------------------------------------
{
  BInt result=0;
  const char* name = obj->String();
  if(!name || !name[0]) { return(-1); }
  BGrammar* g = obj->Grammar();
  int       m = obj->Mode();
  int errCode = 0;
  BObjClassify oc(g,m);
  {
    BObjByClassNameHash::iterator fc = symbolsByClassName_.find(oc);
    if(fc==symbolsByClassName_.end())
    {
      assert(0);
      errCode += 1;
    }
    else
    {
      if(!fc->second->erase(name)) { errCode += 10; }
    }
    assert(fc->second->find(name)==fc->second->end());
  }
  {
    BObjByNameClassHash::iterator fc = symbolsByNameClass_.find(name);
    if(fc==symbolsByNameClass_.end())
    {
      assert(0);
      errCode += 100;
    }
    else
    {
      if(!fc->second->erase(oc)) 
      { 
        assert(0);
        errCode += 1000; 
      }
    }
    assert(fc->second->find(oc)==fc->second->end());
  }
  return(result);
}

//--------------------------------------------------------------------
BObjByClassHash* BSymbolTable::Search(const BText&    name) const
//! Returns the symbol with the specified name
//--------------------------------------------------------------------
{
  BObjByNameClassHash::const_iterator fc = symbolsByNameClass_.find(name.String());
  if(fc==symbolsByNameClass_.end()) { return(NULL); }
  else                              { return(fc->second); }
}

//--------------------------------------------------------------------
BSyntaxObject* BSymbolTable::Search(const BObjClassify& oc, 
                                    const BText& name) const
//! Returns the symbol with the specified class and name
//--------------------------------------------------------------------
{
  BObjByClassNameHash::const_iterator fc = symbolsByClassName_.find(oc);
  if(fc==symbolsByClassName_.end()) { return(NULL); }
  BObjByNameHash::const_iterator found = fc->second->find(name.String());
  if(found==fc->second->end())
  {
    return(NULL);
  }
  else
  {
    return(found->second);
  }
}

//--------------------------------------------------------------------
BList* BSymbolTable::Select(BList* lst, const BObjClassify& oc) const
//! Creates a list of all symbols with a specified class and 
//! concats it to the given list 
//--------------------------------------------------------------------
{
  BObjByClassNameHash::const_iterator fc = symbolsByClassName_.find(oc);
  if(fc==symbolsByClassName_.end()) { return(NULL); }
  BSyntaxObject* obj;
  BObjByNameHash::const_iterator iter;
  for(iter=fc->second->begin(); iter!=fc->second->end(); iter++)
  {
    obj = iter->second;
    #ifndef NDEBUG
    assert(obj);
    int       m = obj->Mode();
    BGrammar* g = obj->Grammar();
    assert(g==oc.grammar_);
    assert((m==oc.mode_) || 
           (m==BUSERFUNMODE    && oc.mode_==BBUILTINFUNMODE) || 
           (m==BBUILTINFUNMODE && oc.mode_==BUSERFUNMODE));
    #endif
    lst = Cons(obj,lst);
  }
  return(lst);
}


//--------------------------------------------------------------------
bool BSymbolTable::AreCompatible(const BSyntaxObject* a,
                                 const BObjClassify& oc)
//! Is true if is possible to create a new global variable with the 
//! same name than a and class given by oc
//--------------------------------------------------------------------
{
  const BGrammar* ga = a->Grammar();
  int ma = a->Mode();
  const BGrammar* gb = oc.grammar_;
  int mb = oc.mode_;

  if((ga==GraCode()) && (ma==BOBJECTMODE) &&
          ((mb==BUSERFUNMODE)||(mb==BBUILTINFUNMODE)))
  {
    BUserCode* uCode = UCode((BSyntaxObject*)a);
    BOperator* opr   = GetOperator(uCode);
    assert(opr);
    BGrammar* goa = opr->Grammar();
    if(goa==gb) 
    { return(false); }
  }
  else if((ga==gb)&&(ma==mb)) 
  { 
    return(false);
  }
  return(true);
}

//--------------------------------------------------------------------
bool BSymbolTable::AreCompatible(const BSyntaxObject* a, 
                                 const BSyntaxObject* b)
//! Is true if two variables/functions can have the same global name 
//--------------------------------------------------------------------
{
  if(a==b) { return(false); }
  BGrammar* ga = a->Grammar();
  BGrammar* gb = b->Grammar();
  int ma = a->Mode();
  int mb = b->Mode();
  if((ga==GraCode()) && (ma==BOBJECTMODE) &&
     (gb==GraCode()) && (mb==BOBJECTMODE))
  {
    BUserCode* ua = UCode((BSyntaxObject*)a);
    BOperator* oa = GetOperator(ua);
    BUserCode* ub = UCode((BSyntaxObject*)b);
    BOperator* ob = GetOperator(ub);
    assert(oa&&ob);
    BGrammar* goa = oa->Grammar();
    BGrammar* gob = ob->Grammar();
    if(goa==gob) 
    { return(false); }
  }
  else if((ga==GraCode()) && (ma==BOBJECTMODE) &&
          ((mb==BUSERFUNMODE)||(mb==BBUILTINFUNMODE)))
  {
    BUserCode* uCode = UCode((BSyntaxObject*)a);
    BOperator* opr   = GetOperator(uCode);
    assert(opr);
    BGrammar* goa = opr->Grammar();
    if(goa==gb) 
    { return(false); }
  }
  else if((gb==GraCode()) && (mb==BOBJECTMODE) &&
          ((ma==BUSERFUNMODE)||(ma==BBUILTINFUNMODE)))
  {
    BUserCode* uCode = UCode((BSyntaxObject*)b);
    BOperator* opr   = GetOperator(uCode);
    assert(opr);
    BGrammar* gob = opr->Grammar();
    if(ga==gob) 
    { return(false); }
  }
  else if((ga==gb)&&(ma==mb)) 
  { 
    return(false);
  }
  return(true);
}

//--------------------------------------------------------------------
void BSymbolTable::LexInfo(BText& info, 
                           const BText& name, 
                           int mode) const 
//! Returns lexical information about all objects with specified name
//--------------------------------------------------------------------
{ 
  BObjByClassHash* obch = Search(name);
  if(!obch) { return; }
  BObjByClassHash::const_iterator f;
  for(f=obch->begin(); f!=obch->end(); f++)
  {
    if(f->first.mode_==mode)
    {
      info << "  " <<f->second->LexInfo()+";\n"; 
    }
  }
}

//--------------------------------------------------------------------
void BSymbolTable::LexInfo(BText& info, const BText& name) const
//! Returns lexical information about all objects with specified name 
//! and mode
//--------------------------------------------------------------------
{ 
  BObjByClassHash* obch = Search(name);
  BObjByClassHash::const_iterator f;
  for(f=obch->begin(); f!=obch->end(); f++)
  {
    info << "  " <<f->second->LexInfo()+";\n"; 
  }
}

//--------------------------------------------------------------------
void BSymbolTable::Dump() const
//! Shows in standard output a report of current status
//--------------------------------------------------------------------
{
  BObjByClassNameHash::const_iterator fc;
  char aux[121];
  sprintf(aux,"\n%16s%30s%12s%12s%8s",
              "TYPE","MODE","MAXSIZE","NUMBER","PERCENT");
  Std(aux);
  for(fc=symbolsByClassName_.begin(); fc!=symbolsByClassName_.end(); fc++)
  {
    #if (__USE_HASH_MAP__==__HASH_MAP_GOOGLE__)
    double ms = fc->second->bucket_count();
    #else
    double ms = fc->second->size();
    #endif
    double s  = fc->second->size();
    double p  = 100.0*s/ms;
    sprintf(aux,"\n%16s%30s%12.0lf%12.0lf%7.2lf%%",
            fc->first.grammar_->String(),
            GetModeName(fc->first.mode_),
            ms,
            s,
            p);
    Std(aux);
  }
}
