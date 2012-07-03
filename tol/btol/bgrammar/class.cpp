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
#include <tol/tol_bstruct.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bscanner.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bsetgra.h>


//--------------------------------------------------------------------
// Initialization
//   Global variables (static in the class BClass) 
//--------------------------------------------------------------------
BTraceInit("class.cpp");
const BClass* BClass::currentClassBuildingInstance_ = NULL;
const BClass* BClass::currentStatic_ = NULL;
static BStruct* strClassMemberInfo_  = NULL;

//--------------------------------------------------------------------
BMember::BMember()
//--------------------------------------------------------------------
: branch_      (NULL),
  isGood_      (false),
  name_        (""),
  declaration_ (""),
  definition_  (""),
  isMethod_    (false),
  method_      (NULL),
  firstParent_ (NULL),
  parent_      (NULL),
  deleteBranch_(false),
  isStatic_    (false),
  static_      (NULL),
  description_ (""),
  type_        (NULL),
  class_       (NULL),
  struct_      (NULL)
{
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
  method_      (NULL),
  firstParent_ (NULL),
  parent_      (NULL),
  deleteBranch_(false),
  isStatic_    (false),
  static_      (NULL),
  description_ (""),
  type_        (NULL),
  class_       (NULL),
  struct_      (NULL) 
{ 
  Copy(mbr); 
}


//--------------------------------------------------------------------
  void CleanLeftAndRightChar(BText& expression, char begin, char end)
//--------------------------------------------------------------------
{
  int fst = 0;
  int lst = expression.Length()-1;
  while(expression[fst]==begin) { fst++; }
  while(expression[lst]==end  ) { lst--; }
  expression = expression.SubString(fst,lst);
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
  method_      (NULL),
  firstParent_ (NULL),
  parent_      (NULL),
  deleteBranch_(false),
  isStatic_    (false),
  static_      (NULL),
  description_ (""),
  type_        (NULL),
  class_       (NULL),
  struct_      (NULL) 
{
  List* cdr;
  BCore* car;
  firstParent_ = parent;
  parent_ = parent;
  isGood_ = true;
  BToken* tok = NULL;
//Std(BText("\nTRACE BMember::BMember(")+BParser::treWrite(branch,"  ")+"\n"); 

  if(parent && branch && branch->car())
  {
    tok = (BToken*)branch->car();
  }
  else
  {
    isGood_ = false;
  }
  if(isGood_ && (tok->TokenType()==MONARY) && (tok->Name()=="Static"))
  {
    isStatic_ = true;
    if(!(branch = branch->cdr()))
    {
      isGood_ = false;
    }
    else 
    {
      if(branch->car()->IsListClass())
      {
        branch = ((List*)branch->car());
      }
      else
      { 
        isGood_ = false;
      }
    }
    if(isGood_)
    {
      tok = (BToken*)branch->car();
    }
    branch_ = branch;
  }
  if(isGood_)
  {
    if((tok->TokenType()==BINARY)&&(tok->Name()=="="))
    {
    //If is a '=' expression then is not a method and has default value
      isMethod_ = false;
      BText expression = BParser::Unparse(branch,"","");
      if(expression[0]=='[')
      {
        expression = expression.SubString(2,expression.Length()-3);
      }
      else if(expression[0]=='{')
      {
        CleanLeftAndRightChar(expression,'{','}');
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
          bool isGoodBranch = false;
          if(((List*)car)->car() &&
            !((List*)car)->car()->IsListClass())
          {
            List* lst2 = ((List*)car);
            BToken* tok2 = (BToken*)(lst2->car());
            List* cdr2 = lst2->cdr();
            if((tok2->TokenType()==TYPE) &&
               cdr2 && cdr2->car() && cdr2->car()->IsListClass())
            {
              car = ((List*)((List*)car)->cdr()->car())->car();
              tok = (BToken*)car;
              name_ = tok->Name();
              isGoodBranch = true; 
            }
            else if((tok2->Name()=="::"))
            {
              List* cdr3 = cdr2->cdr();
              if(cdr3 && cdr3->car() && (cdr3->car()->IsListClass()))
              {
                List* lst3 = (List*)(cdr3->car());
                if(lst3)
                { 
                  List* cdr4 = lst3->cdr();
                  if(cdr4 && cdr4->car() && (cdr4->car()->IsListClass()))
                  { 
                    List* lst4 = (List*)(cdr4->car());
                    if(lst4)
                    {
                      BCore* car4 = lst4->car();
                      if(car4 && !(car4->IsListClass()))
                      { 
                        BToken* tok4 = (BToken*)(car4);
                        name_ = tok4->Name();
                        isGoodBranch = true; 
                      }
                    }
                  }
                }
              }
            }
            if(isGoodBranch)
            { 
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
    else if((tok->TokenType()==BINARY)&&(tok->Name()=="::"))
    {
      declaration_ = BParser::Unparse(branch,"","");
      BText simpleDec = declaration_;
      isMethod_ = simpleDec.Find('(')>=0;
      simpleDec.Replace("("," ");
      simpleDec.Compact();
      int p0 = simpleDec.Find(' ');
      if(p0>0)
      {
        int p1 = simpleDec.Find(' ', p0+1);
        if(p1<0) { p1 = simpleDec.Length()+1; }
        if(p0+1<p1-1)
        {
          name_ = simpleDec.SubString(p0+1,p1-1);
          CleanLeftAndRightChar(name_,'{','}');
        }
      }
    } 
    //Cleaning open and close parenthesis
    CleanLeftAndRightChar(declaration_,'{','}');
  }
  //Checking mandatory information: name and declaration. 
  //Definition is optional
  if(!isGood_ || !name_.HasName() || !declaration_.HasName())
  {
    isGood_ = false;
    Error(I2("Wrong syntax in member declaration \n",
             "Sintaxis incorrecta en declaración de miembro \n")+
          BParser::Unparse(branch_,"","\n")+
          I2(" of Class ", " de Class ")+parent->getFullName());
  }
  if(isGood_ && declaration_.BeginWith("Code") && !isMethod_)
  {
    Warning(I2("Potential failure due to a non standard use of OOP:\n"
               "A class should have no members of type Code as \n",
               "Error potencial debido a un uso no estándar de OOP:\n"
               "Una clase no debería tener miembros de tipo Code como \n")+
            BParser::Unparse(branch_,"","\n")+
            I2(" of Class ", " de Class ")+parent->getFullName());
  }
  BuildAll();
}

//--------------------------------------------------------------------
BMember::~BMember()
//--------------------------------------------------------------------
{
  if(deleteBranch_) { delete branch_; }
  if(method_)
  {
    method_->DecNRefs(); 
    DESTROY(method_); 
  }
  if(static_)
  {
    static_->DecNRefs(); 
    DESTROY(static_); 
  }
}

//--------------------------------------------------------------------
  BText BMember::Dump() const
//--------------------------------------------------------------------
{
  BText dump;
  const BText& name = name_;
  BClass* cls = (BClass*)(parent_);
  if(!cls) { return(dump); }
  if((name[0]=='_')&&(name[1]!='.')) { return(dump); }
  dump += BText("//")+
    BText((isStatic_)?"Static":"Non static")+ " "+
    BText((declaration_.HasName())?"":"virtual ")+
    BText((isMethod_)?"method":"member")+ 
    " defined at " + cls->FullName()+"\n";
  if(description_.HasName()) 
  { 
    dump+="/* ";  
    dump+=description_;  
    dump+=" */\n"; 
  }
  dump+=BText((isStatic_)?"Static ":"")+declaration_;
  if(definition_.HasName() && !(isMethod_))
  {
    dump+=BText(" = ")+ definition_;
  } 
  return(dump);
};

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
  firstParent_  = mbr.firstParent_;
  parent_       = mbr.parent_; 
  method_       = mbr.method_; 
  isStatic_     = mbr.isStatic_; 
  static_       = mbr.static_; 
  type_         = mbr.type_; 
  class_        = mbr.class_; 
  struct_       = mbr.struct_; 
  if(method_) { method_->IncNRefs(); }
  if(static_) { static_->IncNRefs(); }
  deleteBranch_ = true;
}

//--------------------------------------------------------------------
  int BMember::BuildMethod()
//! Builds a well defined method
//--------------------------------------------------------------------
{
  static BText __destroyDec = "Real __destroy(Real void)";
  if(!isStatic_ && !method_ && isMethod_ && definition_.HasName())
  {
    BGrammar::IncLevel();
    int stackPos = BGrammar::StackSize();
    BSyntaxObject* obj = GraAnything()->EvaluateTree(branch_);
    BGrammar::DestroyStackUntil(stackPos, obj);    
    BGrammar::DecLevel();
    BClass* cls = (BClass*)parent_; 
    if(obj)
    {
      method_ = obj;
      method_->IncNRefs(); 
      method_->PutMethod();
      bool isDestroy = method_->Name()=="__destroy";
      if(isDestroy) 
      { 
        if(Compact(declaration_)==__destroyDec)
        {
          cls->PutDestroy(method_); 
        }
        else
        {
          Error(I2("Wrong declaration of destroyer method of Class ",
                   "Declaración errónea del método destructor de Class ")+
                parent_->getFullName()+
                "\n"+declaration_+"\n"+
                I2(" should be ", " debería ser ")+__destroyDec+
                "\n"+__destroyDec+"\n");
        }  
      }
      BUserCode* uCode = UCode(method_);
      uCode->Contens().Operator()->PutMethod();
      return(1);
    }
    else
    {
      isGood_ = false;
      Error(I2("Wrong syntax in method declaration ",
               "Sintaxis incorrecta en declaración de método ")+
            BParser::Unparse(branch_,"","\n")+
            I2(" of Class ", " de Class ")+parent_->getFullName());
      return(0);
    }
  }
  else
  {
    return(-1);
  }
}

//--------------------------------------------------------------------
  int BMember::BuildStatic()
//! Builds a well static member or method
//--------------------------------------------------------------------
{
  if(!isStatic_ && name_.BeginWith("_.autodoc"))
  {
    isStatic_ = true;
  }
  if(!static_ && isStatic_)
  {
    BClass* cls = (BClass*)parent_; 
    if(!definition_.HasName())
    {
      Error(I2("Cannot declare an Static element without definig it "
               "inline",
               "No se puede declarar un elemento estático sin "
               "definirlo en línea")+":\n"+
      "  Static "+BParser::Unparse(branch_,"","\n")+
      I2(" of Class ", "de Class ")+parent_->getFullName());
      return(0);
    }
    else
    {
      BGrammar::IncLevel();
      int stackPos = BGrammar::StackSize();
      const BClass* oldStaticOwner = BClass::currentStatic_;
      BClass::currentStatic_ = cls;
      BSyntaxObject* obj = GraAnything()->EvaluateTree(branch_);
      BClass::currentStatic_ = oldStaticOwner;
      BGrammar::DestroyStackUntil(stackPos, obj);    
      BGrammar::DecLevel();
      if(obj)
      {
        static_ = obj;
        if(isMethod_)
        {
          if(obj->Mode()==BOBJECTMODE)
          {
            obj = UCode(obj)->Contens().Operator();
          }
          ((BUserFunction*)obj)->staticOwner_=(const BClass*)parent_;
        }
        static_->IncNRefs(); 
        return(1);
      }
      else
      {
        isGood_ = false;
        Error(I2("Wrong syntax in static declaration ",
                 "Sintaxis incorrecta en declaración estática ")+
              "Static "+BParser::Unparse(branch_,"","\n")+
              I2(" of Class ", "de Class ")+parent_->getFullName());
        return(0);
      }
    }
  }
  else
  {
    return(-1);
  }
}

//--------------------------------------------------------------------
  int BMember::BuildType()
//! Builds type information
//--------------------------------------------------------------------
{
  if(isMethod_) { type_ = GraCode(); }
  else
  {
    BText typeDec = declaration_+" ";
    typeDec.Replace(BText(" ")+name_+" ","");
    typeDec.Compact();
    class_ = FindClass(typeDec,-1);
    if(class_) { type_ = GraNameBlock(); }
    else
    {
      struct_ = FindStruct(typeDec);
      if(struct_) { type_ = GraSet(); }
      else
      {
        type_ = BGrammar::FindByName(typeDec,false);
        if(!type_)
        {
          isGood_ = false;
          Error(I2("Unknown type declaration of member ",
                   "Tipo descponocido en declaración de miembro ")+
          BParser::Unparse(branch_,"","\n")+
          I2(" of Class ", " de Class ")+parent_->getFullName() );
          return(-1);
        }
      }
    }
  }
  return(0);
}

//--------------------------------------------------------------------
  int BMember::BuildAll()
//--------------------------------------------------------------------
{
  if(parent_->OwnerType()==BMemberOwner::BCLASS)
  {
    BuildMethod();
    BuildStatic();
  }
  BuildType();
  return(0);
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
                      BText(" ")+parent_->getFullName()+"::"+name_);
  return(dec+((!HasDefVal())?BText(""):
         ((!isMethod_)?BText("="):BText(""))+definition_));
}

//--------------------------------------------------------------------
  void BMember::PutDescription(const BText& desc)
//--------------------------------------------------------------------
{
//if(desc=="Devuelve el valor de @C::_.A")
//  printf("");
  BSyntaxObject* obj = NULL;
  description_ = desc;
  if(method_) { obj = method_; } 
  else if(static_) { obj = static_; } 
  if(obj)
  {
    obj->PutDescription(desc);
  } 
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
  lastPosition_(0),
  notImplementedMethods_(0),
  docMbrHash_(NULL),
  docMbr_(0)
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
  lastPosition_(0),
  notImplementedMethods_(0),
  docMbrHash_(NULL),
  docMbr_(0)
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
  docMbrHash_  = new BAutodocMemberHash;
  SetEmptyKey  ((*mbrDecHash_), NULL);
  SetEmptyKey  ((*mbrDefHash_), NULL);
  SetEmptyKey  ((*memberHash_), NULL);
  SetEmptyKey  ((*docMbrHash_), NULL);
  SetDeletedKey((*mbrDecHash_), name_del_key());
  SetDeletedKey((*mbrDefHash_), name_del_key());
  SetDeletedKey((*memberHash_), name_del_key());
  SetDeletedKey((*docMbrHash_), name_del_key());
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
  for(n=0; n<=docMbr_.Size(); n++)
  {
    delete docMbr_[n];
  }
  if(mbrDecHash_) { delete mbrDecHash_; }
  if(mbrDefHash_) { delete mbrDefHash_; }
  if(memberHash_) { delete memberHash_; }
  if(docMbrHash_) { delete docMbrHash_; }
  mbrDecHash_ = NULL;
  mbrDefHash_ = NULL;
  memberHash_ = NULL;
  docMbrHash_ = NULL;
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
  if(!isGood_ || !ascentHash_ || !name.HasName()) { return(false); }
  if(name==getFullName()) { return(true); }
  BClassByNameHash::const_iterator found = ascentHash_->find(name);
  return(found!=ascentHash_->end());
}

//--------------------------------------------------------------------
  bool BMemberOwner::InheritesFrom(const BClass* parent) const
//! True if inherites from a Class called as given name
//--------------------------------------------------------------------
{
  if(!parent) { return(false); }
  if(this==parent) { return(true); }
  BClassByNameHash::const_iterator iterA;
  BClassByNameHash& ah = *ascentHash_;
  for(iterA=ah.begin(); iterA!=ah.end(); iterA++)
  {
    if(iterA->second==parent) { return(true); }
  }
  return(false); 
//return(InheritesFrom(parent->getFullNameRef()));
//return(InheritesFrom(parent->Name()));
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

//#define USE_STATIC_INHERITAGE

//--------------------------------------------------------------------
bool BMemberOwner::AddAutodocMember(
  const BText& name, const BText& desc, const BMemberOwner* parent)
//--------------------------------------------------------------------
{
  return(true);
}

//--------------------------------------------------------------------
  bool BMemberOwner::AddAutodocMember(BMember* mbrPtr)
//--------------------------------------------------------------------
{
  return(true);
}


//--------------------------------------------------------------------
  bool BMemberOwner::AddMember(BMember* newMember)
//! Adds a new member if it's compatible with existant ones
//--------------------------------------------------------------------
{
  if(!isGood_) { return(false); }
  const BText* dec  = &(newMember->declaration_);
  const BText& name = newMember->name_;
  assert(name.HasName()&& dec->HasName());

  if(OwnerType()==BCLASS)
  {
    if(name.BeginWith("_.autodoc.member."))
    {
      bool ok = AddAutodocMember(newMember);
      delete newMember;
      return(ok);
    }
  }
#ifndef USE_STATIC_INHERITAGE
  if(newMember->isStatic_ && (newMember->parent_!=this)) 
  {
    return(true);
  }
#endif
  if(!memberHash_) { CreateMemberHashes(); }
  BMbrNum* newMbrNum = new BMbrNum;
  newMbrNum->member_ = newMember;
  newMbrNum->position_ = lastPosition_++;
  bool ok = true;
  bool  hasDefVal = newMember->HasDefVal();
  //Searches for existant member with same name
  BMbrNum* mbrNum = FindMbrNum(name);
  BMember* member = (mbrNum)?mbrNum->member_:NULL;
  if(member)
  {
    if(member->type_ == newMember->type_)
    {
      if( ((member->type_==GraSet())&&member->struct_&&(member->struct_==newMember->struct_))
          ||
          ((member->type_==GraNameBlock())&&member->class_&&(member->class_==newMember->class_)) )
      {
        dec = &(member->declaration_);
      }
    } 
  }
  //Searches for existant member with same declaration but no default value
  BMember* mbrDec = FindDecMember(*dec);
  //Searches for existant member with same declaration and with default value
  BMember* mbrDef = FindDefMember(*dec);

  bool isMethod = newMember->isMethod_;
  if(!member)
  {
    //If there isn't a member with same name there is no problem on adding it
    //to members map hashed by name
    (*memberHash_)[name] = newMbrNum;
    //It's also added to correspondent map hashed by declaration
    if(hasDefVal) { (*mbrDefHash_)[*dec] = newMember; }
    else          { (*mbrDecHash_)[*dec] = newMember; }
    if(isMethod && !hasDefVal) { notImplementedMethods_++; }
  }
  else if(member->parent_==this)
  {
    //If exists a previous non inherited member with same name it's an error
    ok= false;
    Error(I2("Member ","El miembro ")+newMember->FullExpression()+" "+
          I2("is already defined","ya está definido")+
          I2(" by previous declaration ", " por la declaración previa ")+
          "\n"+member->FullExpression()+"\n");
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
    //newMbrNum->position_ = mbrNum->position_;
      memberHash_->erase(name);
      assert(!FindMember(name));
      mbrDefHash_->erase(*dec);
      assert(!FindDefMember(*dec));
      (*memberHash_)[name] = newMbrNum;
      (*mbrDefHash_)[*dec] = newMember;
      newMember->firstParent_ = mbrDef->firstParent_;
    }
    else if(mbrDec)
    {
      //If it had no default value then is added the new one
    //newMbrNum->position_ = mbrNum->position_;
      memberHash_->erase(name);
      assert(!FindMember(name));
      mbrDecHash_->erase(*dec);
      assert(!FindDecMember(*dec));
      (*memberHash_)[name] = newMbrNum;
      (*mbrDefHash_)[*dec] = newMember;
      if(isMethod) 
      { 
        assert(notImplementedMethods_>0);
        notImplementedMethods_--; 
      }
      newMember->firstParent_ = mbrDec->firstParent_;
    }
  }
  else if(mbrDef)
  {
    BClass* mbrDefParent = NULL;
    BClass* newMbrParent = NULL;
    if(mbrDef->parent_->OwnerType()==BCLASS)
    { mbrDefParent = (BClass*)mbrDef->parent_; }
    if(newMember->parent_->OwnerType()==BCLASS)
    { newMbrParent = (BClass*)newMember->parent_; }
    //Avoiding secondary effects of multiple inheritage 
    if(!mbrDefParent || !newMbrParent || 
       !mbrDefParent->InheritesFrom(newMbrParent))
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
    else
    {
      newMember->firstParent_ = mbrDef->firstParent_;
    }
  }
  //If new member nor old one with same name and declaration has no default 
  //value then does nothing: no error showing and no member adding
  if(!ok) { delete newMbrNum; }
/*
  Std(BText("\nBMemberOwner::AddMember ")+
  " \n  class:"+Name()+
  " \n  member name:"+name+
  " \n  member declaration:"+*dec+"\n");
*/
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
//Std(BText("\n")+Name()+"::AddParent("+parent->getFullNameRef()+")");
  bool ok = true;
  int n;
  const BText& name = parent->getFullNameRef();
//const BText& name = parent->Name();
  //Checks parent is already a parent class
  BClassByNameHash::const_iterator found = parentHash_->find(name);
  if(found==parentHash_->end())
  {
    //Increment reference to parent class to avoid deletions
    parent->IncNRefs();
    //Adds class to hashed map of parents
    (*parentHash_)[name] = parent;
    //Adds parent members to 
    for(n=0; n<parent->member_.Size(); n++)
    {
      ok = ok && AddMember(parent->member_[n]->member_);
    }
    for(n=0; n<parent->docMbr_.Size(); n++)
    {
      BAutoDocInf& a = *(parent->docMbr_[n]);
      ok = ok && AddAutodocMember(a.name_,a.desc_,a.parent_);
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
      const BText& ascName = iterC->second->getFullNameRef();
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
  //Std(BText("\nTRACE BMemberOwner::AddMemberList(")+BParser::treWrite(memberLst,"  ")+"\n"); 
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
        while(ok && lst && lst->car())
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
  isGood_ = ok;
  if(!ok)
  {
    Error(I2("Wrong syntax on member area of declaration ",
             "Sintaxis errónea en el área de de miembros de "
             "declaración de ")+ getDump());
  }
  return(ok);
}

//--------------------------------------------------------------------
  BList* BMemberOwner::Select(
    BList* lst, bool isStatic, bool isMethod) const
//--------------------------------------------------------------------
{
  int i;
  for(i=0; i<member_.Size(); i++)
  {
    BMember& mbr = *(member_[i]->member_);
    if((mbr.isStatic_==isStatic) && 
       (mbr.isMethod_==isMethod))
    {
           if(mbr.method_) { lst = Cons(mbr.method_,lst); }
      else if(mbr.static_) { lst = Cons(mbr.static_,lst); }
    }
  }
  return(lst);
};

//--------------------------------------------------------------------
  BList* BMemberOwner::SelectStaticMembers(BList* lst) const
//--------------------------------------------------------------------
{
  return(Select(lst, true, false));
}

//--------------------------------------------------------------------
  BList* BMemberOwner::SelectStaticMethods(BList* lst) const
//--------------------------------------------------------------------
{
  return(Select(lst, true, true));
}

//--------------------------------------------------------------------
  BList* BMemberOwner::SelectNonStaticMethods(BList* lst) const
//--------------------------------------------------------------------
{
  return(Select(lst, false, true));
}

//--------------------------------------------------------------------
  BNameBlockMemberOwner::BNameBlockMemberOwner(BNameBlock* nb)
//--------------------------------------------------------------------
: BMemberOwner(),
  nb_(nb)
{
};

//--------------------------------------------------------------------
  BMemberOwner::BOwnerType BNameBlockMemberOwner::OwnerType() const 
//--------------------------------------------------------------------
{ 
  return(BMemberOwner::BNAMEBLOCK); 
}

//--------------------------------------------------------------------
  const BText& BNameBlockMemberOwner::getName() const 
//--------------------------------------------------------------------
{ 
  return(nb_->LocalName()); 
}


//--------------------------------------------------------------------
  BText BNameBlockMemberOwner::getFullName() const 
//--------------------------------------------------------------------
{ 
  return(nb_->Name()); 
}


//--------------------------------------------------------------------
  BText BNameBlockMemberOwner::getDump() const 
//--------------------------------------------------------------------
{ 
  return(nb_->Dump()); 
}


//--------------------------------------------------------------------
BClass::BClass(const BText& name)
//--------------------------------------------------------------------
: BSyntaxObject(),
  BMemberOwner(),
  isDefined_(false),
  fullName_(""),
  __destroy(NULL)
{
  if(name=="@BsrMaster")
    printf(""); 
  CreateMemberHashes();
  CreateParentHashes();
  PutName(name);
  BToken* tok = BScanner::FindSymbol(name);
  if(!tok) 
  {
    tok = new BTypeToken(name,BTypeToken::BCLASS);
  }
  BScanner::AddSymbol(tok);
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
  void BClass::PutDestroy(BSyntaxObject* destroy) 
//--------------------------------------------------------------------
{
  if(!__destroy) { __destroy = destroy; }
}

//--------------------------------------------------------------------
  bool BClass::DestroyInstance(BNameBlock* instance)
//--------------------------------------------------------------------
{
  BClassByNameHash::const_iterator iterC;
  BClassByNameHash& par = *(parentHash_);
  bool ok = true;
  for(iterC=par.begin(); iterC!=par.end(); iterC++)
  {
    BClass* parent = iterC->second;
    ok &= parent->DestroyInstance(instance);
  }
  if(__destroy)
  {
    BUserFunCode* ufc= (BUserFunCode*)__destroy;
    BUserFunction* dstryr = (BUserFunction*)ufc->Contens().Operator();
    BList* args = NCons(new BContensDat(0.0));
    const BNameBlock* old = dstryr->NameBlock();
    dstryr->PutNameBlock(instance);
    BSyntaxObject* result = dstryr->Evaluator(args);
    if(!result) { ok = false; }
    else { ok = Real(result)!=0.0; }
    dstryr->PutNameBlock(old);
    DESTROY(result); 
  }
  return(ok);
}
//--------------------------------------------------------------------
	BGrammar* BClass::Grammar() const
//--------------------------------------------------------------------
{
  return(GraAnything());
}


//--------------------------------------------------------------------
  int BClass::GetMembers(BArray<const BMember*>& arr) const
//--------------------------------------------------------------------
{
  int  n=0;
  arr.AllocBuffer(0);
  if(memberHash_)
  {
    BMbrNumByNameHash::const_iterator iterM;
    BMbrNumByNameHash& mbr = *(memberHash_);
    arr.ReallocBuffer(mbr.size());
    arr.ReallocBuffer(0);
    for(iterM=mbr.begin(); iterM!=mbr.end(); iterM++)
    {
      if(iterM->second)
      {
        const BMember* m = iterM->second->member_;  
        if(!m->isMethod_) { arr.Add(m); n++; }
      }
    }
  }
  return(n);
}

//--------------------------------------------------------------------
  int BClass::GetMethods(BArray<const BMember*>& arr) const
//--------------------------------------------------------------------
{
  int  n=0;
  arr.AllocBuffer(0);
  if(memberHash_)
  {
    BMbrNumByNameHash::const_iterator iterM;
    BMbrNumByNameHash& mbr = *(memberHash_);
    arr.ReallocBuffer(mbr.size());
    arr.ReallocBuffer(0);
    for(iterM=mbr.begin(); iterM!=mbr.end(); iterM++)
    {
      if(iterM->second)
      {
        const BMember* m = iterM->second->member_;  
        if(m->isMethod_) { arr.Add(m); n++; }
      }
    }
  }
  return(n);
}


//--------------------------------------------------------------------
static BInt MemberCmp(const void* v1, const void* v2)

/*! Compair two atom receiving the double pointers.
 * \param v1 first atom to caompare
 * \param v2 second atom to compare
 * \return returns an integer less than, equal to, or greather then zero,
 *         if the name of \a v1is found, respectively, to be less then, to
 *         match, or be greater then name of \a v2
 */
//--------------------------------------------------------------------
{
  const BMember* m1 = *((const BMember**)v1);
  const BMember* m2 = *((const BMember**)v2);
  int cmp_met = m1->isMethod_ - m2->isMethod_;
  if(cmp_met) { return(cmp_met); }
  bool isPub1 = m1->name_[0]!='_';
  bool isPub2 = m2->name_[0]!='_';
  int cmp_pub = isPub2 - isPub1;
  if(cmp_pub) { return(cmp_pub); }
  int cmp_stt = m1->isStatic_ - m2->isStatic_;
  if(cmp_stt) { return(cmp_stt); }
  int cmp_def = m1->definition_.HasName() - m2->definition_.HasName();
  if(cmp_def) { return(cmp_def); }
  return(StrCmp(m1->name_, m2->name_));
}

//--------------------------------------------------------------------
  int BClass::GetSortedMembers(BArray<const BMember*>& arr) const
//--------------------------------------------------------------------
{
  int n = GetMembers(arr);  
  arr.Sort(MemberCmp);
  return(n);
}

//--------------------------------------------------------------------
  int BClass::GetSortedMethods(BArray<const BMember*>& arr) const
//--------------------------------------------------------------------
{
  int n = GetMethods(arr);  
  arr.Sort(MemberCmp);
  return(n);
}

//--------------------------------------------------------------------
	BText BClass::Dump() const
//--------------------------------------------------------------------
{
//return(BParser::Unparse(tree_,"","\n"));
  if(!isGood_) { return(BText("Class ")+Name()+" "); }
  BText desc = Description();
  BText dump;
  dump+=BText("\n/* API for Class ")+FullName()+" */\n";  
  dump+=BText("Class ")+Name()+" "; 
  int n; 
  if(parentHash_)
  {
    BClassByNameHash::const_iterator iterC;
    BClassByNameHash& par = *(parentHash_);
    n=0;
    for(iterC=par.begin(); iterC!=par.end(); iterC++, n++)
    {
      if(iterC->second)
      {
        if(n) { dump+=", "; }
        else  { dump+=": "; }
        dump += iterC->second->FullName(); 
        n++;
      }
    }
  }
  if(ascentHash_)
  {
    BClassByNameHash::const_iterator iterC;
    BClassByNameHash& par = *(ascentHash_);
    n=0;
    for(iterC=par.begin(); iterC!=par.end(); iterC++, n++)
    {
      if(!n)
      {
        dump += "\n/////////////////////////////////////////////////////////";
        dump += "\n//Inherites directly or indirectly from these classes: //"; 
        dump += "\n/////////////////////////////////////////////////////////\n";
      }
      if(iterC->second)
      {
        dump+="//  "; 
        dump += iterC->second->FullName() + "\n"; 
        n++;
      }
    }
  }  
  dump += "{";
  if(desc.HasName()) 
  {  
    dump+=BText("\n/* \n ");
    dump+=desc;
    dump+=BText("\n*/ \n ");
  }
  BArray<const BMember*> mem;
  BArray<const BMember*> met;
  int num_mem = GetSortedMembers(mem);
  int num_met = GetSortedMethods(met);
  const BMember* m = NULL;
  BText md;
  bool is_empty = true;
  if(num_mem)
  {    
    dump += "\n/////////////";
    dump += "\n//Members: //";
    dump += "\n/////////////\n";
    for(n=0; n<num_mem; n++)
    {
      m = mem[n];
      if(m) 
      { 
        md = m->Dump();  
        if(md.HasName())
        {
          if(is_empty) { is_empty=false; }
          else         { dump+=";\n"; }
          dump += md; 
        }
      } 
    }
  }
  if(num_met)
  {    
    if(!is_empty) 
    { 
      dump+=";\n"; 
      is_empty=true;
    }
    dump += "\n/////////////";
    dump += "\n//Methods: //";
    dump += "\n/////////////\n";
    for(n=0; n<num_met; n++)
    {
      m = met[n];
      if(m) 
      { 
        md = m->Dump();  
        if(md.HasName())
        {
          if(is_empty) { is_empty=false; }
          else         { dump+=";\n"; }
          dump += md; 
        }
      } 
    }
  }
  dump += BText("\n};\n"); 
  return(dump);
}
//--------------------------------------------------------------------
  const BText& BClass::getFullNameRef() const 
//--------------------------------------------------------------------
{ 
  if(!fullName_.HasName())
  {
    BClass* THIS = (BClass*)this;
    THIS->fullName_ = FullName();
  }
  return(fullName_); 
}

//--------------------------------------------------------------------
  bool BClass::AddAutodocMember(const BText& name, const BText& desc, const BMemberOwner* parent)
//! Adds a new member if it's compatible with existant ones
//--------------------------------------------------------------------
{
  bool ok = true;
  BAutodocMemberHash::const_iterator found = docMbrHash_->find(name);
  if(found!=docMbrHash_->end())
  {
    int s = found->second;
    BClass* cls = (BClass*)parent;
    BClass* oldCls = (BClass*)docMbr_[s]->parent_;
    if(cls==oldCls)
    {
      Error(I2("Special documentation member ",
             "El miembro especial de documentación ")+
          cls->getFullNameRef()+"::"+name+
          I2(" is already defined.",
             " ya está definido."));
      ok=false;
    }
    else
    { 
/*
      Std(BText("TRACE Changing inherited _.autodoc_member from ")+
        oldCls->getFullNameRef()+"::"+name+"='"+docMbr_[s]->desc_+"' to "+
        cls->getFullNameRef()+"::"+name+"='"+desc+"'\n");
*/
      docMbr_[s]->desc_ = desc;
      docMbr_[s]->parent_ = parent;
    }
  }
  else
  {
    int s = docMbr_.Size();
/*
    BClass* cls = (BClass*)parent;
    Std(BText("TRACE Adding [")+s+"] _.autodoc_member "+
        cls->getFullNameRef()+"::"+name+"='"+desc+"'\n");
*/
    docMbr_.Add(new BAutoDocInf(name, desc, parent));
    (*docMbrHash_)[docMbr_[s]->name_] = s;
  }
  return(ok);
}

//--------------------------------------------------------------------
  bool BClass::AddAutodocMember(BMember* mbrPtr)
//! Adds a new member if it's compatible with existant ones
//--------------------------------------------------------------------
{
  BMember& mbr = *mbrPtr;
  bool ok = true;
  if(!mbr.static_)
  {
    Error(I2("Special documentation member ",
             "El miembro especial de documentación ")+
          getFullNameRef()+"::"+mbr.name_+
          I2(" is corrupted or it didn't have declared as static.",
             " está corrupto o no se declaró como static."));
    ok = false;
  }
  else if(mbr.static_->Grammar()!=GraText())
  {
    Error(I2("Special documentation member ",
             "El miembro especial de documentación ")+
          getFullNameRef()+"::"+mbr.name_+
          I2(" should be a Text instead of a ",
             " debería ser un Text en lugar de un ")+
          mbr.static_->Grammar()->Name());
    ok = false;
  }
  else
  {
    BText desc = Text(mbr.static_);
    BText name = mbr.name_.SubString(17,mbr.name_.Length());
    ok = AddAutodocMember(name, desc, this);
  }
  return(true);
}

//--------------------------------------------------------------------
  bool BClass::PutMemberDescription(
    const BText& name, 
    const BText& desc,
    bool autodoc)
//--------------------------------------------------------------------
{
  bool ok = true;
  BMember* mbr = FindMember(name);
  if(!mbr)
  {
    ok = false;
    if(autodoc)
    {
      Warning(I2("Special documentation member ",
                 "El miembro especial de documentación ")+
              getFullNameRef()+"::_.autodoc.member."+name+
              I2(" is irrelevant due it doesn't exist a member nor method called ",
                 " es irrelevante porque no existe ningún miembro ni método llamado ")+
              name);
    }
    else
    {
      Warning(I2("Cannot apply PutDescription to inexistent member ",
                 "No se puede aplicar PutDescription al miembro inexistente ")+
              getFullNameRef()+"::"+name);
    }
  }
  else if((mbr->method_ || mbr->static_ ) && mbr->description_==desc) 
  { 
    return(true); 
  }
  else
  {
    BSyntaxObject* obj = NULL;
    BClass* parent = (BClass*)mbr->parent_;
    BClass* firstParent = (BClass*)mbr->firstParent_;
    if(!parent || !firstParent) 
    { 
      if(mbr->description_.HasName() && mbr->description_!=desc)
      {
        return(false); 
      }
      else
      {
        mbr->PutDescription(desc);
        return(true); 
      }
    }
    if(( mbr->method_ && (parent     ==this)) || 
       (!mbr->method_ && (firstParent==mbr->parent_)) )
    {
      mbr->PutDescription(desc);
    }
    else 
    {
      if(mbr->method_)
      {
        ok = false;
        Warning(I2("Cannot change description of non overloaded method ",
                   "No se puede cambiar la descripción de un método no sobrecargado ")+
                getFullNameRef()+"::"+name+
                I2(" inherited from "," heredado de ")+
                parent->getFullNameRef()+"::"+name);
      }
      else
      {
        BMember* inherited = firstParent->FindMember(name);
        ok = false;
        if(!inherited)
        {
          Warning(I2("Cannot find attribute ",
                     "No se encuentra el atributo ")+
                  getFullNameRef()+"::"+name+
                  I2(" inherited from "," heredado de ")+
                  firstParent->getFullNameRef()+"::"+name);
        }
        else 
        {
          mbr->PutDescription(inherited->description_);
        }
        Warning(I2("Cannot change description of non method member ",
                   "No se puede cambiar la descripción de un atrib ")+
                getFullNameRef()+"::"+name+
                I2(" originally inherited from "," heredado originalmente de ")+
                firstParent->getFullNameRef()+"::"+name);
      }
    }  
  }
  return(ok);
};

//--------------------------------------------------------------------
  bool BClass::CheckAutoDoc()
//--------------------------------------------------------------------
{
  bool ok = true;
  int n=0;
  BAutodocMemberHash::const_iterator iterC;
  BAutodocMemberHash& dmh = *docMbrHash_;
  for(iterC=dmh.begin(); iterC!=dmh.end(); iterC++, n++)
  {
    int s = iterC->second;
    const BText& name = docMbr_[s]->name_;
    const BText& desc = docMbr_[s]->desc_;
    PutMemberDescription(name, desc, true);
  };
  return(ok);
};

//--------------------------------------------------------------------
static BClass* FindClassInNameBlock(
  const BText& name,
  BUserNameBlock*& unb )
//--------------------------------------------------------------------
{
  BClass* cls = NULL;
  BSyntaxObject* obj;
  int pos = name.Find("::",0);
  if(pos>=0)
  {
    BText prefix;
    BText suffix;
    prefix = name.SubString(0,pos-1);
    suffix  = name.SubString(pos+2,name.Length());
    if(!unb)
    {
      obj = GraNameBlock()->FindOperand(prefix,false);
      if(obj && obj->Grammar()==GraNameBlock())
      {
        unb = (BUserNameBlock*)obj;
      }
    }
    else
    {
      BNameBlock& nb = unb->Contens();
      obj = nb.Member(prefix);
      if(obj && obj->Grammar()==GraNameBlock())
      {
        unb = (BUserNameBlock*)obj;
      }
      else
      {
        unb = NULL;
      }
    }
    if(unb) 
    {
      cls = FindClassInNameBlock(suffix, unb);         
    }
  }
  else if(unb)
  {
    BNameBlock& nb = unb->Contens();
    obj = nb.Member(name);
    if(obj && obj->Mode()==BCLASSMODE)
    {
      cls = (BClass*)obj;
    }
  }
  return(cls); 
}


//--------------------------------------------------------------------
static BClass* FindNonMemberClass(const BText& name, int defined)

/*! Searches a user class wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
  BClass* cls = BStackManager::FindClass(name);
  if(!cls)
  {
    BObjClassify oc(GraAnything(),BCLASSMODE);
    cls= (BClass*)BGrammar::SymbolTable().Search(oc, name);
  }
  return(cls);
}

//--------------------------------------------------------------------
BClass* FindClass(const BText& name, int defined)

/*! Searches a user class wich name is name and returns it.
 *  If itsn't exists returns NIL.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BUserNameBlock* unb = NULL;
  BClass* cls = FindClassInNameBlock(name, unb);
  if(!cls && BNameBlock::Current())
  {
    result = BNameBlock::Current()->Member(name);
    if(result && (result->Mode()==BCLASSMODE))
    {
      cls = (BClass*)result;
    }
  }
  if(!cls)
  {
    cls = BStackManager::FindClass(name);
  }
  if(!cls)
  {
    result = BNameBlock::LocalMember(name);
    if(result && (result->Mode()==BCLASSMODE))
    {
      cls = (BClass*)result;
    }
  }
  if(!cls) 
  {
    BObjClassify oc(GraAnything(),BCLASSMODE);
    cls= (BClass*)BGrammar::SymbolTable().Search(oc, name);
  }
  if(!cls) 
  {
    result = BNameBlock::UsingMember(name);
    if(result && (result->Mode()==BCLASSMODE))
    {
      cls = (BClass*)result;
    }
  }
  if(cls && (defined>=0) && (cls->isDefined_!=(defined!=0)))
  {
    cls = NULL;
  }
  return(cls);
}

//--------------------------------------------------------------------
  BClass* BClass::GetClassIfExist(const BText& name)
//--------------------------------------------------------------------
{
  if(name=="@BsrMaster")
    printf(""); 
  BClass* old = NULL;
  if(name.HasName())
  {
    BUserNameBlock* bnbP = BNameBlock::Building();
    if(bnbP)
    {
      old = BStackManager::FindClass(name);
      if(old && (old->Level()!=BGrammar::Level()))
      {
        old = NULL;
      }
    }
    else
    {
      old = FindClass(name,-1);
    }  
  }
  return(old); 
}

//--------------------------------------------------------------------
  BClass* BClass::PredeclareClassIfNeeded(const BText& name)
//--------------------------------------------------------------------
{
  BClass* class_ = BClass::GetClassIfExist(name);
  if(!class_) 
  {
    class_ = new BClass(name);
  }
  return(class_);
};


//--------------------------------------------------------------------
  BClass* BClass::PredeclareNewClass(
     const BText& name,
     BClass*& old,
     bool& ok)
//--------------------------------------------------------------------
{
  old = BClass::GetClassIfExist(name);
  BClass* class_ = NULL;
  //Checking existent class with the same name
  if(old && old->isDefined_)
  {
    ok = false;
    Error(BText("Class '") + name +
      I2("' is already defined as ",
         "' ya fue definida como \"") + old->Dump() + "\"\n");
  }
  else if(old) 
  {
    class_ = old;
  }
  else
  {
    class_ = new BClass(name);
  }
  return(class_);
};

//--------------------------------------------------------------------
  BClass* BClass::PredeclareNewClass(const BText& name)
//--------------------------------------------------------------------
{
  BClass* class_;
  bool ok;
  return(PredeclareNewClass(name, class_, ok));
};

//--------------------------------------------------------------------
//! Evaluates a parsed tree with a Class declaration
BSyntaxObject* BClass::Evaluate(const List* _tree)
//--------------------------------------------------------------------
{
  BToken* tok = NULL;
  BClass* old = NULL;
  BClass* class_ = NULL;
  bool ok = false;
  //Duplicates parsed tree to be stored in class
  List* tree = _tree->duplicate();
//Std(BText("\nBClass::Evaluate(")+BParser::treWrite(tree,"  ")+"\n"); 
  //Skypes Class token
  List* cdr = tree->cdr();
  BCore* car = (cdr)?cdr->car():NULL;
  BText name = "";
  List* memberLst = NULL;
  BArray<BClass*> parentArr(2);
  parentArr.ReallocBuffer(0);
  if(car && car->IsListClass())
  {
    int nParent = 0;
    cdr = ((List*)car)->cdr();
    car = ((List*)car)->car();
    tok = (BToken*)car;
    if(!cdr)
    {
      name = tok->Name();
      class_ = PredeclareNewClass(name, old, ok);
      ok = (class_!=NULL);
      memberLst=NULL;
    }
    else if((tok->TokenType()==BINARY)&&(tok->Name()==":"))
    {
      //Class with inheriting clause
      tok = (BToken*)((List*)(cdr->car()))->car();
      name = tok->Name();
      class_ = PredeclareNewClass(name, old, ok);
      ok = (class_!=NULL);
      cdr = cdr->cdr();
      BText owner = "";
      while(ok && !memberLst && cdr && 
            cdr->car() && cdr->car()->IsListClass())
      {
        car = ((List*)(cdr->car()))->car();
        tok = (BToken*)car;
        BText parentName = "";
        if(tok->Name()=="::")
        {
          ok = false;
          if(cdr->car() && cdr->car()->IsListClass())
          {
            cdr = ((List*)(cdr->car()))->cdr();
            if(cdr && cdr->car()->IsListClass())
            {
              car = ((List*)(cdr->car()))->car();
              cdr = cdr->cdr();
              BToken* tokOwner = (BToken*)car;
              if(owner.HasName()) { owner = owner + "::" + tokOwner->Name(); }
              else { owner = tokOwner->Name(); }
              ok = true;
            }
          }
          continue;
        }
        else if((tok->TokenType()==TYPE) && 
           (!tok->Close()) && 
           (((BTypeToken*)tok)->type_==BTypeToken::BCLASS))
        {
          parentArr.ReallocBuffer(nParent+1);
          BClass* parent = NULL;
          if(owner.HasName())
          {
            parent = FindClass(owner+"::"+tok->Name(),-1);
            owner = "";
          }
          else
          {
            parent = FindClass(tok->Name(),-1);
          }
          if(parent)
          {
            parentArr[nParent] = parent;
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
      if(ok && !parentArr.Size())
      {
        ok = false;
        parentArr.ReallocBuffer(0);
        Error(I2("No valid parent class list",
                 "Lista de clases padre no válida")+":\n"+
              BParser::Unparse(tree));
      }
      else if(ok && !memberLst && cdr)
      {
        memberLst = (List*)(cdr->car());
      }
    }
    else if(cdr && ((tok->TokenType()==FUNCTION)||
                    (tok->TokenType()==TYPE)))
    {
      //Class without inheriting clause
      name = tok->Name();
      class_ = PredeclareNewClass(name, old, ok);
      ok = (class_!=NULL);
      memberLst = cdr;
      if(cdr && cdr->car() && cdr->car()->IsListClass())
      {
        cdr = (List*)cdr->car();
        car = cdr->car();
        tok = (BToken*)car;
        if(tok->TokenType()==SEPARATOR)
        {
          memberLst = cdr->cdr();
        }
      }
    }    
  }
  if(class_&& ok)
  {
    class_->PutTree(tree);
    class_->SetParentArray(parentArr);
    class_->AddMemberList(memberLst);
    if(!class_->isGood_)
    {
      ok=false;
    }
    else
    {
      class_->SortMembers();
      ok = class_->CheckAutoDoc();
      class_->isDefined_ = class_->member_.Size()>0; 
    }
    assert(!class_ || FindClass(name,-1));
  }
  if(!ok || !class_)
  {
    //If Class declaration failed shows an error and destroy all
    Error(I2("Cannot create ","No se pudo crear ") + 
      BParser::Unparse(tree,"","\n"));
    DESTROY(class_);
    class_ = NULL;
    tree = NULL;
    if(tree) { delete tree; }
  }
  return(class_);
}


//--------------------------------------------------------------------
static bool checkNonPrivate(
  const BClass* cls, 
  const BText& memberName,
  bool fullAccess) 
//--------------------------------------------------------------------
{
  if(!fullAccess && (memberName[0]=='_')&&(memberName[1]!='.'))
  {
    Error(I2("Cannot access to private member ",
             "No se puede acceder al miembro privado ")+memberName +
          I2(" of Class ",
             " de Class ")+cls->getFullNameRef());
    return(false);
  }
  else
  {
    return(true);
  }
}
//--------------------------------------------------------------------
BSyntaxObject* BClass::FindMethod(const BText& memberName, bool fullAccess) const
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BMember* mbr = FindMember(memberName);
  if(mbr && mbr->method_ && checkNonPrivate(this, memberName, fullAccess))
  { 
    result = mbr->method_;
  } 
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BClass::FindStaticMethod(const BText& methodName, bool fullAccess) const
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BMember* mbr = FindMember(methodName);
  if(mbr && mbr->isStatic_ && mbr->isMethod_  && 
     checkNonPrivate(this, methodName, fullAccess))
  { 
    if(result = mbr->static_) { result->PutNameBlock(NULL); }
  } 
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BClass::FindStaticMember(const BText& memberName, bool fullAccess) const
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BMember* mbr = FindMember(memberName);
  if(mbr && mbr->static_ && !mbr->isMethod_  && 
     checkNonPrivate(this, memberName, fullAccess))
  { 
    if(result = mbr->static_) { result->PutNameBlock(NULL); }
  } 
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BClass::FindStatic(const BText& methodName, bool fullAccess) const
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BMember* mbr = FindMember(methodName);
  if(mbr && mbr->isStatic_  && checkNonPrivate(this, methodName, fullAccess))
  { 
    if(result = mbr->static_) { result->PutNameBlock(NULL); }
  } 
  return(result);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BClassMembers);
DefExtOpr(1, BClassMembers, "ClassMembers", 1, 2, "Text Real",
 "(Text className [, Real static=?] )",
 I2("Returns a set with a register of information about each member "
    " of a class. If static==0 then only registers about non static "
    "elements will be returned. If static==1 only registers about "
    "static elements will be returned. In other case all registers "
    "will be returned. The information is returned using structure\n",
    "Devuelve un conjunto con un registro de información acerca "
    "de cada uno de los miembros de una clase. Si static==0 sólo se "
    "devolverán los registros acerca de elementos no estáticos. Si "
    "static==1 sólo se devolverán los registros acerca de elementos "
    "estáticos. En otro caso se devolverán todos los registros. La "
    "información se devuelve usando la estructura:\n")+
    "  @ClassMemberInfo { \n"
    "    Real Static,\n"
    "    Real Method,\n"
    "    Text Declaration,\n"
    "    Text Name,\n"
    "    Text Access,\n"
    "    Text DefinedAtClass,\n"
    "    Text Description }\n",
 BOperClassify::System_);
//--------------------------------------------------------------------
void BClassMembers::CalcContens()
//--------------------------------------------------------------------
{
  if(!strClassMemberInfo_)
  {
    strClassMemberInfo_ = FindStruct("@ClassMemberInfo");
  }
  assert(strClassMemberInfo_);
  const BText& clsName = Text(Arg(1));
  BDat static_ = 2;
  if(Arg(2)) { static_ = Dat(Arg(2)); }
  int i;
  const BClass* cls = FindClass(clsName,true);
  if(!cls) { return; }
  contens_.PrepareStore(cls->member_.Size());
  for(i=0; i<cls->member_.Size(); i++)
  {
    BMember* mbr = NULL;
    if(cls->member_[i]) 
    { 
      BMbrNum* mn = cls->member_[i];
      if(mn) { mbr = mn->member_; }
    }
    if(!mbr) { continue; }
    if(!static_.IsUnknown())
    {
      if(static_==0 &&  mbr->isStatic_) { continue; }
      if(static_==1 && !mbr->isStatic_) { continue; }
    }
    const BText& name = mbr->name_;
  //if(name.BeginWith("_.autodoc.member.")) { continue; }
    BSet reg;
    reg.PrepareStore(6);

    BText declare = BText(" ")+Compact(mbr->declaration_)+" ";
    declare = Replace(declare,BText(" ")+name+" "," ");
    declare.Compact();

    BText access = "Public";
    if(name[0]=='_')
    {
      if(name[1]=='.') { access = "Read only"; }
      else             { access = "Private"; }
    }

    BText definedAtClass = "";
    if(mbr && mbr->parent_) 
    { 
      definedAtClass = mbr->parent_->getFullName();
    }

    reg.AddElement(new BContensDat ("",mbr->isStatic_,""));
    reg.AddElement(new BContensDat ("",mbr->isMethod_,""));
    reg.AddElement(new BContensText("",declare,""));
    reg.AddElement(new BContensText("",name,""));
    reg.AddElement(new BContensText("",access,""));
    reg.AddElement(new BContensText("",definedAtClass,""));
    reg.AddElement(new BContensText("",mbr->description_,""));

    reg.PutStruct(strClassMemberInfo_);
    contens_.AddElement(new BSetTuple("", reg));
  }
}
