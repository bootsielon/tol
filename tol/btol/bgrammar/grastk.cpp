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


//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BStackManager and internal 
//   classes).
//--------------------------------------------------------------------
short BStackManager::numAllowedChar_ =     0;
int   BStackManager::branchPoolSize_ = 65536;
short BStackManager::branchPoolLen_  =   978;
BArray<BStackManager::BDictionaryNode*> 
       BStackManager::BDictionaryNode::branchPool_;
int    BStackManager::BDictionaryNode::currentBranch_ = 0;

//#define _DEBUG_STACK_

#ifdef _DEBUG_STACK_
static BText objName_ = "";
#endif

//--------------------------------------------------------------------
  bool BStackManager::BDictionaryNode::Initialize()
//! Initializes the dictionary static members
//--------------------------------------------------------------------
{
  short i=0;
  short c;
  for(c=0; c<256; c++) 
  { 
    if(BParser::DefaultParser()->Filter()->IsIdentifier((char)c))
    {
      allowed_[charPos_[c] = i++]=(char)c; 
    }
    else
    {
      charPos_[c] = -1; 
    }
  }
/*
  for(c=0; c<256; c++) { charPos_[c] = -1; }
  for(c='a'; c<='z'; c++) { allowed_[charPos_[c] = i++] = (char)c; }
  for(c='A'; c<='Z'; c++) { allowed_[charPos_[c] = i++] = (char)c; }
  for(c='0'; c<='9'; c++) { allowed_[charPos_[c] = i++] = (char)c; }
  allowed_[(int)(charPos_[(int)'_' ] = i++)] = '_';
  allowed_[(int)(charPos_[(int)'.' ] = i++)] = '.';
  allowed_[(int)(charPos_[(int)'\''] = i++)] = '\'';
*/
  numAllowedChar_ = i;
  branchPool_.AllocBuffer(32);
  branchPool_.AllocBuffer(0);
  currentBranch_=branchPoolLen_;
  return(true);
}

//--------------------------------------------------------------------
  void BStackManager::BDictionaryNode::CreateBranches()
//! Creates the array of branches
//--------------------------------------------------------------------
{
//branch_ = new BDictionaryNode[numAllowedChar_];

  int n = branchPool_.Size()-1;
  if(currentBranch_==branchPoolLen_)
  {
    n++;
    currentBranch_ = 0;
    if(n==branchPool_.MaxSize()-1)
    {
      branchPool_.ReallocBuffer((n*120)/100);
    //Std(BText("\nBStackManager::BDictionaryNode::CreateBranches ")+ (n*branchPoolLen_));
    }
    branchPool_.ReallocBuffer(n+1);
    branchPool_[n] = new BDictionaryNode[branchPoolLen_*numAllowedChar_];
  //Std(BText("\nBStackManager::BDictionaryNode::CreateBranches ")+ (n*branchPoolLen_));
  }
  branch_ = branchPool_[n]+currentBranch_*numAllowedChar_;
  currentBranch_++;

}

//--------------------------------------------------------------------
  BStackManager::BDictionaryNode*
  BStackManager::BDictionaryNode::Find(const BText& name, short n)
//! Finds recursively the dictionary node correspondent of a name begining
//! from n-th character
//--------------------------------------------------------------------
{
  if(name.Length()==n)
  {
    return(this);
  }
  else
  {
    short  c = name[n];
    if(c<0) { c+=256; }
    short i = charPos_[c];
    if(i<0)
    { return(NULL); }
    if(!branch_) { CreateBranches(); }
    return(branch_[i].Find(name, n+1));
  }
}

//--------------------------------------------------------------------
  bool BStackManager::Initialize()
//! Initializes the stack handler
//--------------------------------------------------------------------
{
  bool ok = BDictionaryNode::Initialize();
  root_ = new BDictionaryNode();
  root_->CreateBranches();
  return(ok);
}


//--------------------------------------------------------------------
  void BStackManager::Push(BSyntaxObject* obj)
//! Enters a new object in both stack and dictionary structures
//--------------------------------------------------------------------
{
  const BText* name__ = &obj->LocalName();
  if(!name__->HasName()) { name__ = &obj->Name(); }
  const BText& name = *name__;
//if(!name.HasName())
//{ SendError("Cannot push unnamed objects."); }

#ifdef _DEBUG_STACK_
  if(!objName_.HasName() || (name==objName_))
  Std(BText("\nBStackManager::Push[")+currentEntries_+"]"+obj->Grammar()->Name()+" "+name+" at level "+obj->Level());
#endif

  BDictionaryNode* dNode = root_->Find(name,0);
  if(dNode)
  { 
    dNode->entry_.Push(obj);
  }
  stack_ = new BStackNode(obj, stack_, dNode);
  obj->IncNRefs();
  currentEntries_++;
}

//--------------------------------------------------------------------
  BSyntaxObject* BStackManager::Find(const BText& name)
//! Returns the last created object with specified name if exists or null else
//--------------------------------------------------------------------
{
#ifdef _DEBUG_STACK_
  if(!objName_.HasName() || (name==objName_))
  Std(BText("\nBStackManager::Find ")+name+" at level "+BGrammar::Level());
#endif
  if(!name.HasName())
  { return(NULL); }
//{ SendError("Cannot find unnamed objects."); }

  BDictionaryNode* dNode = root_->Find(name,0);
  if(!dNode || !dNode->entry_.heap_)
  {
# ifdef _DEBUG_STACK_
    if(!objName_.HasName() || (name==objName_))
      Std(BText("\nBStackManager::Find NOT Found ")+name);
#  endif
    return(NULL);
  }
  else
  {
# ifdef _DEBUG_STACK_
    BSyntaxObject* obj = dNode->entry_.heap_->obj_;
    if(!objName_.HasName() || (name==objName_))
      Std(BText("\nBStackManager::Found ")+obj->Grammar()->Name()+" "+name+" from level "+obj->Level());
#  endif
    return(dNode->entry_.heap_->obj_);
  }
}


//--------------------------------------------------------------------
  BStruct* BStackManager::FindStruct(const BText& name)
//! Returns the last created struct with specified name if exists or null else
//--------------------------------------------------------------------
{
  if(!name.HasName())
  { SendError("Cannot push unnamed structures."); }
  BDictionaryNode* dNode = root_->Find(name,0);
  if(!dNode || !dNode->entry_.heap_)
  {
    return(NULL);
  }
  else
  {
    BStruct* bstr=NULL;
    BDictionaryEntryNode* lst = dNode->entry_.heap_;
    while(lst && !bstr)
    {
      if(lst->obj_->Mode()==BSTRUCTMODE) { bstr=(BStruct*)lst->obj_; }
      lst = lst->next_;
    }
    return(bstr);
  }
}

//--------------------------------------------------------------------
  BClass* BStackManager::FindClass(const BText& name)
//! Returns the last created struct with specified name if exists or null else
//--------------------------------------------------------------------
{
  if(!name.HasName())
  { SendError("Cannot push unnamed classes."); }
  BDictionaryNode* dNode = root_->Find(name,0);
  if(!dNode || !dNode->entry_.heap_)
  {
    return(NULL);
  }
  else
  {
    BClass* bcls=NULL;
    BDictionaryEntryNode* lst = dNode->entry_.heap_;
    while(lst && !bcls)
    {
      if(lst->obj_->Mode()==BCLASSMODE) { bcls=(BClass*)lst->obj_; }
      lst = lst->next_;
    }
    return(bcls);
  }
}


//--------------------------------------------------------------------
  void BStackManager::Pop(BSyntaxObject* except)
//! Removes the object on the top of stack and its correspondent
//! dictionary entry node that must be on the top of referenced dictionary
//! node. If object is not the except one then destroy it
//--------------------------------------------------------------------
{
  BSyntaxObject* obj=stack_->obj_;
  assert(obj);
#ifdef _DEBUG_STACK_
  const BText* name__ = &obj->LocalName();
  if(!name__->HasName()) { name__ = &obj->Name(); }
  const BText& name = *name__;
  if(!name.HasName())
  { SendError("Cannot push unnamed objects."); }

  if(name==objName_)
  Std(BText("\nBStackManager::Pop[")+currentEntries_+"]"+obj->Grammar()->Name()+" "+name+" from level "+obj->Level());
#endif
/* */
  if(stack_->dNode_ && stack_->dNode_->entry_.heap_)
  {
    if(stack_->dNode_->entry_.heap_->obj_!=obj)
    { 
      SendError("Corrupted local dictionary."); 
    }
    else
    {
      stack_->dNode_->entry_.Pop();
    }
  }
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
