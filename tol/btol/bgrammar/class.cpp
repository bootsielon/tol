/* class.cpp: User defined classes in TOL. TOL language.

   Copyright (C) 2005-2007, Bayes Decision, SL (Spain [EU])

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

#ifdef NDEBUG
//#define TRACE_LEVEL 4
#endif

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bclass.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bscanner.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bnameblock.h>

//--------------------------------------------------------------------
// Initialization
//   Global variables (static in the class BClass) 
//--------------------------------------------------------------------
BTraceInit("class.cpp");

//--------------------------------------------------------------------
BMember::BMember()
//--------------------------------------------------------------------
: branch_      (NULL),
  isGood_      (false),
  name_        (""),
  declaration_ (""),
  definition_  (""),
  isMethod_    (false),
  parent_      (NULL),
  deleteBranch_(false)
{
}

//--------------------------------------------------------------------
BMember::BMember(BMemberOwner* parent, List* branch)
//--------------------------------------------------------------------
: branch_      (branch),
  isGood_      (false),
  name_        (""),
  declaration_ (""),
  definition_  (""),
  isMethod_    (false),
  parent_      (NULL),
  deleteBranch_(false)
{
  List* cdr;
  BCore* car;
  parent_ = parent;
  BToken* tok = (BToken*)branch->car();
  isGood_ = true;
  if((tok->TokenType()==BINARY)&&(tok->Name()=="="))
  {
  //If is a '=' expression then is not a method and has default value
    isMethod_ = false;
    BText expression = BParser::Unparse(branch,"","");
    if(expression[0]=='[')
    {
      expression = expression.SubString(2,expression.Length()-4);
    }
    else if(expression[0]=='{')
    {
      expression = expression.SubString(1,expression.Length()-2);
    }
    int posEq = expression.Find("=");
    int posSp = expression.Find(" ");
    name_ = expression.SubString(posSp+1,posEq-1);
    declaration_ = expression.SubString(0,posEq-1);
    definition_ = expression.SubString(posEq+1,expression.Length());
  }
  else if((tok->TokenType()==BINARY)&&(tok->Name()=="#F#"))
  {
  //If is a '#F#' expression then is a method and has default value
    isMethod_ = true;
    cdr = branch->cdr();
    if(cdr)
    {
      car = cdr->car();
      if(car && car->IsListClass())
      {
        declaration_ = BParser::Unparse((List*)car,"","");
        if(((List*)car)->cdr() && 
           ((List*)car)->cdr()->car() &&
           ((List*)car)->cdr()->car()->IsListClass())
        {
          car = ((List*)((List*)car)->cdr()->car())->car();
          tok = (BToken*)car;
          name_ = tok->Name();
          cdr = cdr->cdr();
          if(cdr)
          {
            car = cdr->car();
            if(car && car->IsListClass())
            {
              definition_  = BParser::Unparse((List*)car,"","");
            }
            else
            {
              isGood_ = false;
            }
          }
        }
      }
    }
  }
  else if(tok->TokenType()==TYPE)
  {
  //If is a 'Type name ...' expression then hasn't default value
    declaration_ = BParser::Unparse(branch,"","");
    car = branch->cdr()->car();
    car = (car->IsListClass())?((List*)car)->car():car;
    tok = (BToken*)car;
    name_ = tok->Name();
  //If main token is FUNCTION then member is a method
    isMethod_ = tok->TokenType()==FUNCTION;
  }
  //Cleaning open and close parenthesis
  if(declaration_[0]=='{')
  {
    declaration_=declaration_.SubString(1,declaration_.Length()-2);
  }
  //Checking mandatory information: name and declaration. 
  //Definition is optional
  if(!name_.HasName() || !declaration_.HasName())
  {
    isGood_ = false;
    Error(I2("Wrong syntax in member declaration ",
             "Sintaxis incorrecta en declaración de miembro ")+
          BParser::Unparse(branch,"","\n")+
          I2(" of Class ", "de Class ")+parent->getName());
  }
}

//--------------------------------------------------------------------
BMember::BMember(const BMember& mbr) 
//--------------------------------------------------------------------
: branch_      (NULL),
  isGood_      (false),
  name_        (""),
  declaration_ (""),
  definition_  (""),
  isMethod_    (false),
  parent_      (NULL),
  deleteBranch_(false)  
{ 
  Copy(mbr); 
}

//--------------------------------------------------------------------
BMember::~BMember()
//--------------------------------------------------------------------
{
  if(deleteBranch_) { delete branch_; }
}

//--------------------------------------------------------------------
  void BMember::Copy(const BMember& mbr)
//--------------------------------------------------------------------
{
  branch_       = mbr.branch_->duplicate();
  isGood_       = mbr.isGood_;
  name_         = mbr.name_; 
  declaration_  = mbr.declaration_; 
  definition_   = mbr.definition_;  
  isMethod_     = mbr.isMethod_;    
  parent_       = mbr.parent_;      
  deleteBranch_ = true;
}

//--------------------------------------------------------------------
  bool BMember::HasDefVal() const 
//! True if member has default value
//--------------------------------------------------------------------
{ 
  return(definition_.HasName()!=0); 
}

//--------------------------------------------------------------------
  BText BMember::FullExpression() const
//! Text expression
//--------------------------------------------------------------------
{
  BText dec = Replace(Compact(declaration_), BText(" ")+name_, 
                      BText(" ")+parent_->getName()+"::"+name_);
  return(dec+((!HasDefVal())?BText(""):
         ((!isMethod_)?BText("="):BText(""))+definition_));
}

//--------------------------------------------------------------------
BMemberOwner::BMemberOwner()
//--------------------------------------------------------------------
: isGood_      (true),
  tree_        (NULL),   
  parentHash_  (NULL),
  ascentHash_  (NULL),
  mbrDecHash_  (NULL),
  mbrDefHash_  (NULL),
  memberHash_  (NULL),
  lastPosition_(0)
{
}

//--------------------------------------------------------------------
BMemberOwner::BMemberOwner(const BMemberOwner& mbrOwn) 
//--------------------------------------------------------------------
: isGood_      (true),
  tree_        (NULL),   
  parentHash_  (NULL),
  ascentHash_  (NULL),
  mbrDecHash_  (NULL),
  mbrDefHash_  (NULL),
  memberHash_  (NULL),
  lastPosition_(0)
{ 
  Copy(mbrOwn); 
}

//--------------------------------------------------------------------
BMemberOwner::~BMemberOwner()
//--------------------------------------------------------------------
{
  DestroyAll();
}

//--------------------------------------------------------------------
void BMemberOwner::DestroyAll()
//--------------------------------------------------------------------
{
  DestroyMemberStore();
  DestroyParentHashes();
  DESTROY(tree_);
}

//--------------------------------------------------------------------
void BMemberOwner::CreateMemberHashes()
//--------------------------------------------------------------------
{
  //Reset hash_map's
  mbrDecHash_  = new BMemberByNameHash;
  mbrDefHash_  = new BMemberByNameHash;
  memberHash_  = new BMbrNumByNameHash;
  SetEmptyKey  ((*mbrDecHash_), NULL);
  SetEmptyKey  ((*mbrDefHash_), NULL);
  SetEmptyKey  ((*memberHash_), NULL);
  SetDeletedKey((*mbrDecHash_), name_del_key());
  SetDeletedKey((*mbrDefHash_), name_del_key());
  SetDeletedKey((*memberHash_), name_del_key());
}

//--------------------------------------------------------------------
void BMemberOwner::CreateParentHashes()
//--------------------------------------------------------------------
{
  //Reset hash_map's
  parentHash_  = new BClassByNameHash;
  ascentHash_  = new BClassByNameHash;
  SetEmptyKey  ((*parentHash_), NULL);
  SetEmptyKey  ((*ascentHash_), NULL);
}

//--------------------------------------------------------------------
  void BMemberOwner::Copy(const BMemberOwner& mbrOwn)
//--------------------------------------------------------------------
{
  int n=0;
  if(!mbrOwn.isGood_) 
  {
    Error(I2("FATAL: Cannot copy corrupted BMemberOwner  ",
             "FATAL: No se puede copiar un BMemberOwner corrupto"));
    return; 
  }
  DestroyAll();

  tree_ = (mbrOwn.tree_)?mbrOwn.tree_->duplicate():NULL;

  if(mbrOwn.parentHash_)
  {
    CreateParentHashes();
    BClassByNameHash::const_iterator iterC;
    BClassByNameHash& par = *(mbrOwn.parentHash_);
    for(iterC=par.begin(); iterC!=par.end(); iterC++, n++)
    {
      AddParent(iterC->second);
    }
  }
  if(mbrOwn.memberHash_)
  {
    BMbrNumByNameHash::const_iterator iterM;
    BMbrNumByNameHash& mbr = *(mbrOwn.memberHash_);
    for(iterM=mbr.begin(); iterM!=mbr.end(); iterM++, n++)
    {
      AddMember(iterM->second->member_);
    }
  }
  lastPosition_ = mbrOwn.lastPosition_;
}

//--------------------------------------------------------------------
  void BMemberOwner::DestroyMemberStore()
//--------------------------------------------------------------------
{
  if(!memberHash_) { return; }
  int n=0;
  //Deletes owned members
  BMbrNumByNameHash::const_iterator iterM;
  BMbrNumByNameHash& mbr = *memberHash_;
  for(iterM=mbr.begin(); iterM!=mbr.end(); iterM++, n++)
  {
    if(!iterM->second)
    {
      Error(I2("FATAL: Corrupted BMemberOwner member ",
               "FATAL: Miembro corrupto de BMemberOwner"));
    }
    //Skype inherited members
    else 
    {
      if(iterM->second->member_->parent_==this)
      {
        delete iterM->second->member_;
      }
      delete iterM->second;
    }
  }
  if(mbrDecHash_) { delete mbrDecHash_; }
  if(mbrDefHash_) { delete mbrDefHash_; }
  if(memberHash_) { delete memberHash_; }
  mbrDecHash_ = NULL;
  mbrDefHash_ = NULL;
  memberHash_ = NULL;
}

//--------------------------------------------------------------------
  void BMemberOwner::DestroyParentHashes()
//--------------------------------------------------------------------
{
  if(parentHash_)
  {
    int n=0;
    BClassByNameHash::const_iterator iterC;
    BClassByNameHash& par = *parentHash_;
    for(iterC=par.begin(); iterC!=par.end(); iterC++, n++)
    {
      BClass* parent = iterC->second;
      parent->DecNRefs();
      DESTROY(parent);
    };
    delete parentHash_; 
  }
  if(ascentHash_) { delete ascentHash_; }
}

//--------------------------------------------------------------------
  void BMemberOwner::PutTree(List* tree)
//--------------------------------------------------------------------
{
  tree_ = tree;
}

//--------------------------------------------------------------------
int MbrNumCmp(const void* v1, const void* v2)
//--------------------------------------------------------------------
{
  BMbrNum* mbr1 = *((BMbrNum**)v1);
  BMbrNum* mbr2 = *((BMbrNum**)v2);
  return(mbr1->position_ - mbr2->position_);
}

//--------------------------------------------------------------------
  void BMemberOwner::SortMembers()
//--------------------------------------------------------------------
{
  if(!memberHash_) { return; }
  int n=0;
  //Deletes owned members
  BMbrNumByNameHash::const_iterator iterM;
  BMbrNumByNameHash& mbr = *memberHash_;
  member_.AllocBuffer(mbr.size());
  for(iterM=mbr.begin(); iterM!=mbr.end(); iterM++, n++)
  {
    member_[n] = iterM->second;
  }
  member_.Sort(MbrNumCmp);
}

//--------------------------------------------------------------------
  bool BMemberOwner::InheritesFrom(const BText& name) const
//! True if inherites from a Class called as given name
//--------------------------------------------------------------------
{
  if(!isGood_ || !ascentHash_) { return(false); }
  BClassByNameHash::const_iterator found = ascentHash_->find(name);
  return(found!=ascentHash_->end());
}

//--------------------------------------------------------------------
  bool BMemberOwner::InheritesFrom(const BClass* parent) const
//! True if inherites from a Class called as given name
//--------------------------------------------------------------------
{
  return(InheritesFrom(parent->Name()));
}

//--------------------------------------------------------------------
  BMbrNum* BMemberOwner::FindMbrNum(const BText& name) const
//--------------------------------------------------------------------
{
  if(!isGood_ || !memberHash_) { return(NULL); }
  BMbrNumByNameHash::const_iterator found;
  found = memberHash_->find(name);
  if(found==memberHash_->end()) { return(NULL); }
  else                          { return(found->second); }
}

//--------------------------------------------------------------------
  BMember* BMemberOwner::FindMember(const BText& name) const
//--------------------------------------------------------------------
{
  BMbrNum* mbr = FindMbrNum(name);
  if(!mbr) { return(NULL); }
  else     { return(mbr->member_); }
}

//--------------------------------------------------------------------
  BMember* BMemberOwner::FindDefMember(const BText& declaration) const
//! Searchs a member with given declaration and default value
//--------------------------------------------------------------------
{
  if(!isGood_ || !memberHash_) { return(NULL); }
  BMemberByNameHash::const_iterator found;
  found = mbrDefHash_->find(declaration);
  if(found==mbrDefHash_->end()) { return(NULL); }
  else                          { return(found->second); }
}

//--------------------------------------------------------------------
  BMember* BMemberOwner::FindDecMember(const BText& declaration) const
//! Searchs a member with given declaration without default value
//--------------------------------------------------------------------
{
  if(!isGood_ || !memberHash_) { return(NULL); }
  BMemberByNameHash::const_iterator found;
  found = mbrDecHash_->find(declaration);
  if(found==mbrDecHash_->end()) { return(NULL); }
  else                          { return(found->second); }
}

//--------------------------------------------------------------------
  bool BMemberOwner::AddMember(BMember* newMember)
//! Adds a new member if it's compatible with existant ones
//--------------------------------------------------------------------
{
  if(!isGood_) { return(false); }
  if(!memberHash_) { CreateMemberHashes(); }
  BMbrNum* newMbrNum = new BMbrNum;
  newMbrNum->member_ = newMember;
  newMbrNum->position_ = lastPosition_++;
  const BText& dec  = newMember->declaration_;
  const BText& name = newMember->name_;
  assert(name.HasName()&& dec.HasName());
  bool ok = true;
  bool  hasDefVal = newMember->HasDefVal();
  //Searches for existant member with same name
  BMbrNum* mbrNum = FindMbrNum(name);
  BMember* member = (mbrNum)?mbrNum->member_:NULL;
  //Searches for existant member with same declaration but no default value
  BMember* mbrDec = FindDecMember(dec);
  //Searches for existant member with same declaration and with default value
  BMember* mbrDef = FindDefMember(dec);
  if(!member)
  {
    //If there isn't a member with same name there is no problem on adding it
    //to members map hashed by name
    (*memberHash_)[name] = newMbrNum;
    //It's also added to correspondent map hashed by declaration
    if(hasDefVal) { (*mbrDefHash_)[dec] = newMember; }
    else          { (*mbrDecHash_)[dec] = newMember; }
  }
  else if(member->parent_==this)
  {
    //If exists a previous non inherited member with same name it's an error
    ok= false;
    Error(I2("Member ","El miembro ")+name+" "+
          I2("is already defined","ya está definido")+
          "\n"+member->FullExpression()+"\n"+
          I2(" by declaration ", " por la declaración ")+
          "\n"+newMember->FullExpression()+"\n");
  }
  else if(!mbrDef && !mbrDec)
  {
    //If exists an inherited member with same name but distinct declaration
    //it's an error
    ok = false;
    Error(I2("Inheritage conflict: ",
             "Conflicto de herencia: ")+
          I2("Cannot replace member declaration ",
             "No se puede reemplazar la declaración de miembro ")+
          "\n"+member->FullExpression()+"\n"+
          I2(" by an incompatible declaration ", 
             " por la declaración incompatible ")+
          "\n"+newMember->FullExpression()+"\n");    
  }
  //If exists an inherited member with same name and declaration then
  else if(hasDefVal)
  {
    //If new member has default value then
    if(mbrDef)
    {
      //If it had default value then is replaced by new one
      memberHash_->erase(name);
      assert(!FindMember(name));
      mbrDefHash_->erase(dec);
      assert(!FindDefMember(dec));
      (*memberHash_)[name] = newMbrNum;
      (*mbrDefHash_)[dec ] = newMember;
    }
    else if(mbrDec)
    {
      //If it had no default value then is added the new one
      memberHash_->erase(name);
      assert(!FindMember(name));
      mbrDecHash_->erase(dec);
      assert(!FindDecMember(dec));
      (*memberHash_)[name]= newMbrNum;
      (*mbrDefHash_)[dec] = newMember;
    }
  }
  else if(mbrDef)
  {
    //If new member has no default value and old one had it then it's an error
    ok = false;
    Error(I2("Inheritage conflict: ",
             "Conflicto de herencia: ")+
          I2("Cannot replace member definition ",
             "No se puede reemplazar la definición de miembro ")+
          "\n"+mbrDef->FullExpression()+"\n"+
          I2(" by declaration ", " por la declaración ")+
          "\n"+newMember->FullExpression()+"\n");
  }
  //If new member nor old one with same name and declaration has no default 
  //value then does nothing: no error showing and no member adding
  if(!ok) { delete newMbrNum; }
  return(ok);
}

//--------------------------------------------------------------------
  bool BMemberOwner::AddParent(BClass* parent)
//! Adds a new parent class
//--------------------------------------------------------------------
{
  if(!isGood_) { return(false); }
  if(!parent) { return(true); }
  if(!parentHash_) { CreateParentHashes(); }
//Std(BText("\n")+Name()+"::AddParent("+parent->Name()+")");
  bool ok = true;
  int n;
  const BText& name = parent->Name();
  //Checks parent is already a parent class
  BClassByNameHash::const_iterator found = parentHash_->find(name);
  if(found==parentHash_->end())
  {
    //Increment reference to parent class to avoid deletions
    parent->IncNRefs();
    //Adds class to hashed map of parents
    (*parentHash_)[name] = parent;
    //Adds parent members to 
    n=0;
    for(n=0; n<parent->member_.Size(); n++)
    {
      ok = ok && AddMember(parent->member_[n]->member_);
    }
  }
  found = ascentHash_->find(name);
  if(found==ascentHash_->end())
  {
    (*ascentHash_)[name] = parent;
    BClassByNameHash::const_iterator iterC;
    BClassByNameHash& cls = *(parent->ascentHash_);
    n=0;
    for(iterC=cls.begin(); iterC!=cls.end(); iterC++, n++)
    {
      const BText& ascName = iterC->second->Name();
      found = ascentHash_->find(ascName);
      if(found==ascentHash_->end())
      {
        (*ascentHash_)[ascName] = iterC->second;
      }
    }
  }
  //Store error status
  isGood_ = ok;
  return(ok);
}

//--------------------------------------------------------------------
  bool BMemberOwner::SetParentArray(const BArray<BClass*>& parent)
//! Sets the full array of parent classes
//--------------------------------------------------------------------
{
  if(!isGood_) { return(false); }
  if(!parentHash_) { CreateParentHashes(); }
  int n;
  bool ok = true;
  for(n=0; ok && (n<parent.Size()); n++)
  {
    ok = ok && AddParent(parent[n]);
  }
  isGood_ = ok;
  return(ok);
}

//--------------------------------------------------------------------
  bool BMemberOwner::AddMemberList(List*  memberLst)
//! Adds the list of parsed members
//--------------------------------------------------------------------
{
  if(!isGood_ ) { return(false); }
  if(!memberHash_) { CreateMemberHashes(); }
  bool ok = true;
  BMember* mbr = NULL;
  List* lst = NULL;
  if(memberLst)
  {
    if(memberLst->car()->IsListClass())
    {
      //If branch is a list adds all items
      lst = memberLst;
      while(ok && lst)
      {
        if(ok = lst->car()->IsListClass()!=0)
        {
          mbr = new BMember(this, (List*)lst->car());
        }
        if(ok = ok && mbr->isGood_)
        {
          ok = ok && AddMember(mbr);
          lst = lst->cdr();
        }
        else
        {
          delete mbr;
        }
      }
    }
    else
    {
      BToken* tok = (BToken*)memberLst->car();
      if(tok->TokenType()==SEPARATOR)
      {
        //If branch is a separator adds all child items
        lst = memberLst->cdr();
        while(ok && lst)
        {
          if(ok = lst->car()->IsListClass()!=0)
          {
            mbr = new BMember(this, (List*)lst->car());
          }
          if(ok = ok && mbr->isGood_)
          {
            ok = ok && AddMember(mbr);
            lst = lst->cdr();
          }
          else
          {
            delete mbr;
          }
        }
      }
      else if (tok->Close() && (tok->Close()->Name()=="}"))
      {
        //In there is just one member it is added directly 
        BMember* mbr = new BMember(this, memberLst);
        ok = ok && AddMember(mbr);
      }
    }
  }
  if(!ok)
  {
    Error(I2("Wrong syntax on member area of Class declaration",
             "Sintaxis errónea en el área de de miembros de "
             "declaración de Class")+ getDump());
  }
  isGood_ = ok;
  return(ok);
}

//--------------------------------------------------------------------
BClass::BClass()
//--------------------------------------------------------------------
: BSyntaxObject(),
  BMemberOwner()
{
  CreateMemberHashes();
  CreateParentHashes();
}

//--------------------------------------------------------------------
BClass::BClass(const BText& name,
               List*  tree)
//--------------------------------------------------------------------
: BSyntaxObject(name),
  BMemberOwner()
{
  PutTree(tree_);
  CreateMemberHashes();
  CreateParentHashes();
  //Adds object to language searchers
  BGrammar::AddObject(this);
}

//--------------------------------------------------------------------
BClass::~BClass()
//--------------------------------------------------------------------
{
  //Destroy parser symbnol
  BScanner::DelSymbol(Name());
  //Remove symbol from language searchers
  BGrammar::DelObject(this);
}

//--------------------------------------------------------------------
	BGrammar* BClass::Grammar() const
//--------------------------------------------------------------------
{
  return(GraAnything());
}

//--------------------------------------------------------------------
	BText BClass::Dump() const
//--------------------------------------------------------------------
{
  return(BParser::Unparse(tree_,"","\n"));
}

//--------------------------------------------------------------------
//! Evaluates a parsed tree with a Class declaration
BSyntaxObject* BClass::Evaluate(const List* _tree)
//--------------------------------------------------------------------
{
  BClass* class_ = NULL;
  bool ok = false;
  //Duplicates parsed tree to be stored in class
  List* tree = _tree->duplicate();
  //Skypes Class token
  List* cdr = tree->cdr();
  BCore* car = (cdr)?cdr->car():NULL;
  BText name;
  List* memberLst = NULL;
  BArray<BClass*> parentArr(2);
  parentArr.ReallocBuffer(0);
  if(car && car->IsListClass())
  {
    cdr = ((List*)car)->cdr();
    car = ((List*)car)->car();
    BToken* tok = (BToken*)car;
    int nParent = 0;
    if((tok->TokenType()==BINARY)&&
            (tok->Name()==":"))
    {
      //Class with inheriting clause
      tok = (BToken*)((List*)(cdr->car()))->car();
      name = tok->Name();
      cdr = cdr->cdr();
      ok = true;
      while(ok && !memberLst && cdr && 
            cdr->car() && cdr->car()->IsListClass())
      {
        car = ((List*)(cdr->car()))->car();
        tok = (BToken*)car;
        if((tok->TokenType()==TYPE) && 
           (((BTypeToken*)tok)->type_==BTypeToken::BCLASS))
        {
          parentArr.ReallocBuffer(nParent+1);
          parentArr[nParent] = FindClass(tok->Name());
          if(parentArr[nParent])
          {
            nParent++;
            cdr = ((List*)(cdr->car()))->cdr();
          }
          else
          {
            ok = false;
            parentArr.ReallocBuffer(0);
            Error(BText("'")+tok->Name()+"' "+
                  I2("is not a valid Class identififer",
                     "no es un identificador de Class"));
            break;
          }
        }
        else
        {
          memberLst = (List*)(cdr->car());
        }
      }
      if(ok && !memberLst && cdr)
      {
        memberLst = (List*)(cdr->car());
      }
    }
    else if(cdr && ((tok->TokenType()==FUNCTION)||
                    (tok->TokenType()==TYPE)))
    {
      //Class without inheriting clause
      name = tok->Name();
      memberLst = cdr;
      ok = true;
    }
  }
  BClass* old = NULL;
  if(name.HasName())
  {
    if(ok)
    {
      //Checking existent class with the same name
      old = FindClass(name);
      if(old)
      {
        ok = false;
        Error(BText("Class '") + name +
          I2("' is already defined as ",
             "' ya fue definida como \"") + old->Dump() + "\"\n");
      }
      else
      {
        class_ = new BClass("", tree);
        class_->SetParentArray(parentArr);
        class_->AddMemberList(memberLst);
        if(!class_->isGood_)
        {
          //If Class declaration failed shows an error and destroy all
          Error(I2("Cannot create ","No se pudo crear ") + 
            BParser::Unparse(tree,"","\n"));
          DESTROY(class_);
          class_ = NULL;
          tree = NULL;
        }
        else
        {
          class_->SortMembers();
          class_->PutName(name);
          BGrammar::AddObject(class_);
        }
        assert(!class_ || FindClass(name));
      }
    }
  }
  if(!class_)
  {
    //If no class was created shows an error
    Error(I2("Se esperaba una declaración de Class instead of ",
             "Se esperaba una declaración de Class en lugar de ") +"\n"+ 
          BParser::Unparse(_tree,"","\n"));
    if(tree) { delete tree; }
    if(name.HasName() && !old)
    {
      //If name of class has been detected then removes parser symbol
      BScanner::DelSymbol(name);
    }
  }
  return(class_);
}


//--------------------------------------------------------------------
BClass* FindClass(const BText& name)

/*! Searches a user class wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BClass* bcls = BStackManager::FindClass(name);
  if(!bcls)
  {
    const BNameBlock* cns = BNameBlock::Current();
    if(cns)
    {
      result = cns->Member(name);
      if(result && (result->Mode()==BCLASSMODE))
      {
        bcls = (BClass*)result;
      }
    }
  }
  if(!bcls) 
  {
    BObjClassify oc(GraAnything(),BCLASSMODE);
    bcls= (BClass*)BGrammar::SymbolTable().Search(oc, name);
  }
  if(!bcls) 
  {
    result = BNameBlock::UsingMember(name);
    if(result && (result->Mode()==BCLASSMODE))
    {
      bcls = (BClass*)result;
    }
  }
  return(bcls);
}

