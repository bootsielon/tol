/* grastk.cpp: Local Stack manager for GNU/TOL Language.

   Copyright (C) 2003-2005 - Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bfilter.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bgrastk.h>

BTraceInit("grastl.cpp");

//! Error manager function
static void SendError(const BText& msg)
{
  Error(BText("[Stack Manager FATAL]\n")+msg);
}

class BStackNode;
class BDictionaryEntryNode
/* Dictionary entry node.
 * References to an object and the next entry node in reversed creating order
 */
{
public:
  BSyntaxObject*        obj_;  //!<Object reference
  BStackNode*           stackNode_;
  BDictionaryEntryNode* next_; //!<Next entry node

  BDictionaryEntryNode(
    BSyntaxObject* obj, 
    BDictionaryEntryNode* next,
    BStackNode* stackNode)
  : obj_(obj), next_(next), stackNode_(stackNode)
  {}
  DeclareClassNewDelete(BDictionaryEntryNode);
};
class BDictionaryEntry
/* Dictionary entry stack.
 * Each dictionary node has a stack of entries corresponding to objects with
 * the same name.
 */
{
public:
  BText                 name_;
  BDictionaryEntryNode* heap_;
  BDictionaryEntry(const BText& name) : name_(name), heap_(NULL) {}

  //! Enters a new dictionary entry node at stack top level
  void Push(BSyntaxObject* obj, BStackNode* stack)
  {
    heap_ = new BDictionaryEntryNode(obj, heap_, stack);
  }
  //! Releases the dictionary entry node at stack top level
  void Pop()
  {
    if(!heap_)
    { SendError("Heap is already empty. Cannot delete any object."); }
    BDictionaryEntryNode* next = heap_->next_;
    delete heap_;
    heap_ = next;
  }
};

#define _STACK_USE_GOOGLE_HASH_DENSE

#ifdef _STACK_USE_GOOGLE_HASH_DENSE
typedef hash_map_by_name<BDictionaryEntry*>::dense_ BDictionaryHash;
#elif defined _STACK_USE_GOOGLE_HASH_SPARSE
typedef hash_map_by_name<BDictionaryEntry*>::sparse_ BDictionaryHash;
#else
#include <unordered_map>
struct eqstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) == 0;
  }
};
//std::tr1::unordered_map<std::string, BDictionaryEntry*> stackHash_;
typedef std::tr1::unordered_map<
  const char*, 
  BDictionaryEntry*, 
  std::tr1::hash<const char*>, 
  eqstr> 
BDictionaryHash;
#endif


class BStackNode
/* Stack node.
 * Stores references to all objects in reversed creating order pointing also
 * the dictionary node where the object has its correspondent entry in order
 * to delete it from the dictionary without any searching cost.
 * When an object is added increments its number of references.
 * When an object is deleetd decrements its number of references.
 */
{
public:
  BSyntaxObject*    obj_;   //!< Referenced object
  BStackNode*       next_;  //!< Next stack node
  BDictionaryEntry* entry_;

  BStackNode(BSyntaxObject*    obj,
             BStackNode*       next,
             BDictionaryEntry* entry)
  : obj_(obj), next_(next), entry_(entry)
  {
    assert(obj);
    assert(entry);
  }
  DeclareClassNewDelete(BStackNode);
};

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BStackManager and internal 
//   classes).
//--------------------------------------------------------------------

//#define _DEBUG_STACK_

#ifdef _DEBUG_STACK_
static BText objName_ = "";
#endif

//! Heap of the stack of all available objects in reversed creating order
static BDictionaryHash stackHash_;
static BStackNode*      stack_ = NULL;
static char* emptyHashKey_ = NULL;
int   BStackManager::currentEntries_ = 0;

//--------------------------------------------------------------------
  bool BStackManager::Initialize()
//! Initializes the stack handler
//--------------------------------------------------------------------
{
  BStackManager::currentEntries_ = 0;
  stack_ = NULL;
#ifdef _STACK_USE_GOOGLE_HASH_DENSE
  SetEmptyKey  (stackHash_, emptyHashKey_);
  SetDeletedKey(stackHash_, name_del_key());
#endif
#ifdef _STACK_USE_GOOGLE_HASH_SPARSE
  SetDeletedKey(stackHash_, name_del_key());
#endif
  return(true);
}

//--------------------------------------------------------------------
  void BStackManager::Push(BSyntaxObject* obj)
//! Enters a new object in both stack and dictionary structures
//--------------------------------------------------------------------
{
  const BText* name__ = &obj->LocalName();
  if(!name__->HasName()) { name__ = &obj->Name(); }
  const BText& name = *name__;
  if(!name.HasName())
  { SendError("Cannot push unnamed objects."); }
  BDictionaryEntry* entry = NULL;
  BDictionaryHash::const_iterator found;
  found = stackHash_.find(name);
  if(found==stackHash_.end())
  {
    entry = new BDictionaryEntry(name);
    stackHash_[ entry->name_ ] = entry;
  }
  else
  {
    entry = found->second;
  }
  assert(entry); 
  stack_ = new BStackNode(obj, stack_, entry);
  entry->Push(obj, stack_);
  obj->IncNRefs();
  currentEntries_++;
  if(name=="dec") 
    printf(""); 
}

//--------------------------------------------------------------------
  void BStackManager::Pop(BSyntaxObject* except)
//! Removes the object on the top of stack and its correspondent
//! dictionary entry node that must be on the top of referenced dictionary
//! node. If object is not the except one then destroy it
//--------------------------------------------------------------------
{
  BSyntaxObject* obj=stack_->obj_;
  BDictionaryEntry* entry=stack_->entry_;
  assert(obj);
  assert(entry);
  assert(entry->heap_->obj_==obj);
  entry->Pop();
/*
  if(!entry->heap_) 
  {
    const BText* name__ = &obj->LocalName();
    if(!name__->HasName()) { name__ = &obj->Name(); }
    const BText& name = *name__;
    stackHash_.erase(name);
    delete entry;
  }
*/
  BStackNode* next = stack_->next_;
  delete stack_;
  stack_ = next;
#if(__USE_POOL__==__POOL_BFSMEM__)
  short assigned = obj->IsAssigned();
  if(assigned!=1) 
  { 
    currentEntries_--;
    return; 
  }
#endif
  obj->DecNRefs();
  if(obj!=except)
  {
    DESTROY(obj);
  }
  currentEntries_--;
}


//--------------------------------------------------------------------
  void BStackManager::DestroyUntil(int size, BSyntaxObject* except)
//! Removes objects from the stack top until stack size is the specified one
//! All released objects but except one will be deleted
//--------------------------------------------------------------------
{
  while(currentEntries_>size) { Pop(except); }
}


//--------------------------------------------------------------------
  BSyntaxObject* BStackManager::Find(const BText& name)
//! Returns the last created object with specified name if exists or null else
//--------------------------------------------------------------------
{
  if(!name.HasName())
  { return(NULL); }
  BDictionaryEntry* entry = NULL;
  BSyntaxObject* obj = NULL;
  BDictionaryHash::const_iterator found;
  found = stackHash_.find(name);
  if(found!=stackHash_.end())
  {
    entry = found->second;
    if(!entry)
    {
      printf("");
    }
    else if(!entry->heap_)
    {
      printf("");
    }
    else
    {
      obj = entry->heap_->obj_;
    }
        
  }
  return(obj);
}


//--------------------------------------------------------------------
  BStruct* BStackManager::FindStruct(const BText& name)
//! Returns the last created struct with specified name if exists or null else
//--------------------------------------------------------------------
{
  if(!name.HasName())
  { SendError("Cannot push unnamed structures."); }
  BStruct* bstr=NULL;
  BDictionaryHash::const_iterator found;
  found = stackHash_.find(name);
  if(found!=stackHash_.end() && found->second)
  {
    BDictionaryEntryNode* lst = found->second->heap_;
    while(lst && !bstr)
    {
      if(lst->obj_->Mode()==BSTRUCTMODE) { bstr=(BStruct*)lst->obj_; }
      lst = lst->next_;
    }
  }
  return(bstr);
}

//--------------------------------------------------------------------
  BClass* BStackManager::FindClass(const BText& name)
//! Returns the last created struct with specified name if exists or null else
//--------------------------------------------------------------------
{
  BClass* bcls=NULL;
  BDictionaryHash::const_iterator found;
  found = stackHash_.find(name);
  if(found!=stackHash_.end() && found->second)
  {
    BDictionaryEntryNode* lst = found->second->heap_;
    while(lst && !bcls)
    {
      if(lst->obj_->Mode()==BCLASSMODE) { bcls=(BClass*)lst->obj_; }
      lst = lst->next_;
    }
  }
  return(bcls);
}
