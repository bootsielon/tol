/* nameblock.cpp: Name blocks implementation. TOL language.

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

#include <tol/tol_bnameblock.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_btxtgra.h>


#include <tol/tol_list.h>
#include <tol/tol_tree.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bclass.h>


//--------------------------------------------------------------------
// Initialization
//   Global variables (static in the class BNameBlock) 
//--------------------------------------------------------------------
BTraceInit("nameblock.cpp");

const BNameBlock*         BNameBlock::current_  = NULL;
      BUserNameBlock*     BNameBlock::building_ = NULL;
      BNameBlock*         BNameBlock::unknown_  = NULL;
      BObjClassify        BObjClassify::null_;
      BObjByNameHash      BNameBlock::using_;
      BObjByNameHash      BNameBlock::usingSymbols_;
      BObjByClassNameHash BNameBlock::usingSymbolsByClass_;
      BRequiredPackage    BNameBlock::globalRequiredPackages_;

#if (__USE_POOL__==__POOL_BFSMEM__)
static BFixedSizeMemoryBase* BFSMEM_Hndlr = 
 BFSMSingleton<sizeof(BNameBlock)>::Handler();
#endif
static BStruct* strNameBlockMemberInfo_  = NULL;
static BStruct* strClassMemberInfo_  = NULL;
static char* emptyHashKey_ = NULL;

//--------------------------------------------------------------------
//BNameBlock members
//--------------------------------------------------------------------

//--------------------------------------------------------------------
static bool BNameBlock_IsInitialized()
//--------------------------------------------------------------------
{
  static bool initialized_ = false;
  if(initialized_) { return(true); }
  initialized_ = true;
  return(false);
}

//--------------------------------------------------------------------
  bool BNameBlock::Initialize() 
//--------------------------------------------------------------------
{ 
  if(BNameBlock_IsInitialized()) { return(false); }
  unknown_ = new BNameBlock;
//emptyHashKey_ = new char(64);
//sprintf(emptyHashKey_, "_#_&¬!?_NameBlock_empty_hash_key_");
  SetEmptyKey  (using_,               emptyHashKey_);
  SetEmptyKey  (usingSymbolsByClass_, BObjClassify::null_);
  SetEmptyKey  (usingSymbols_,        emptyHashKey_);
  SetDeletedKey(usingSymbols_,        name_del_key());

  strNameBlockMemberInfo_ = NewStruct("@NameBlockMemberInfo", 
  "Text:Type,"
  "Text:Name,"
  "Text:Access,"
  "Text:DefinedAtClass,"
  "Text:Description");

 strClassMemberInfo_  = NewStruct("@ClassMemberInfo", 
  "Real:Static,"
  "Real:Method,"
  "Text:Declaration,"
  "Text:Name,"
  "Text:Access,"
  "Text:DefinedAtClass,"
  "Text:Description");

  return(true);
}

//--------------------------------------------------------------------
  BNameBlock::BNameBlock() 
//--------------------------------------------------------------------
: BObject  (),
  public_  (),
  private_ (), 
  evLevel_ (BGrammar::Level()),
  level_   (-999999999),
  set_     (),
  father_  (NULL),
  class_   (NULL),
  localName_(),
  owner_    (NULL),
  nonPrivateMembers_(0),
  requiredPackages_(NULL),
  doingRebuildFullNameDeep(false),
  startedPackage(false)
{
  SetEmptyKey(public_ ,emptyHashKey_);
  SetEmptyKey(private_,emptyHashKey_);
#if (__USE_POOL__==__POOL_BFSMEM__)
  short isAssigned = BFSMEM_Hndlr->IsAssigned(this,this->GetPageNum());
#else
  short isAssigned = -1;
#endif
  createdWithNew_ = isAssigned==1;
}

//--------------------------------------------------------------------
BNameBlock::BNameBlock(const BText& fullName, const BText& localName) 
//--------------------------------------------------------------------
: BObject  (fullName), 
  public_  (),
  private_ (),
  evLevel_ (BGrammar::Level()),
  level_   (-999999999),
  set_     (),
  father_  (NULL),
  class_   (NULL),
  localName_(localName),
  owner_    (NULL),
  nonPrivateMembers_(0),
  requiredPackages_(NULL),
  doingRebuildFullNameDeep(false),
  startedPackage(false)
{
  SetEmptyKey(public_ ,emptyHashKey_);
  SetEmptyKey(private_,emptyHashKey_);
#if (__USE_POOL__==__POOL_BFSMEM__)
  short isAssigned = BFSMEM_Hndlr->IsAssigned(this,this->GetPageNum());
#else
  short isAssigned = -1;
#endif
  createdWithNew_ = isAssigned==1;
}

//--------------------------------------------------------------------
BNameBlock::BNameBlock(const BNameBlock& ns) 
//--------------------------------------------------------------------
: BObject  (), 
  public_  (),
  private_ (),
  evLevel_ (BGrammar::Level()),
  level_   (-999999999),
  set_     (),
  father_  (NULL),
  class_   (NULL),
  localName_(),
  owner_    (NULL),
  nonPrivateMembers_(0),
  requiredPackages_(NULL),
  doingRebuildFullNameDeep(false),
  startedPackage(false)
{
  SetEmptyKey(public_ ,emptyHashKey_);
  SetEmptyKey(private_,emptyHashKey_);
#if (__USE_POOL__==__POOL_BFSMEM__)
  short isAssigned = BFSMEM_Hndlr->IsAssigned(this,this->GetPageNum());
#else
  short isAssigned = -1;
#endif
  createdWithNew_ = isAssigned!=-1;
  *this = ns;
}  

//--------------------------------------------------------------------
BNameBlock::~BNameBlock() 
//--------------------------------------------------------------------
{
  Clean();
}

//--------------------------------------------------------------------
void BNameBlock::Clean() 
//--------------------------------------------------------------------
{
  int n;
  if(class_)
  {
    class_->DestroyInstance(this);
    class_->DecNRefs();
    DESTROY(class_);
  }
  owner_ = NULL;
  for(n=1; n<=set_.Card(); n++)
  {
    if( set_[n] )
    { 
      set_[n]->PutNameBlock(NULL);
      BSyntaxObject *obj = set_[n];
      if( obj->Grammar() == GraCode() ) {
        BUserCode* uCode = UCode(obj);
        BOperator* opr   = GetOperator(uCode);
        if ( opr && opr->NameBlock() && opr->NameBlock() == this ) {
          opr->PutNameBlock( NULL );
        }
      }
    }
  }
  public_.clear();
  private_.clear();
  set_.Delete();
  father_ = NULL;
  if(requiredPackages_)
  {
    delete requiredPackages_;
    requiredPackages_ = NULL;
  }
}


void DeepCopy(const BSet& sample,BSet& copy,const BArray<BDat>& p,BInt& pos);

//--------------------------------------------------------------------
BNameBlock& BNameBlock::operator= (const BNameBlock& ns)
//--------------------------------------------------------------------
{
  Clean(); 
  PutName(ns.Name());
  localName_ = ns.LocalName();
  PutClass((BClass*)ns.Class());
  BArray<BDat> p;
  BInt pos = -1;
  BSet copy;
  DeepCopy(ns.Set(),copy, p, pos);
  Fill(copy);
  return(*this);
}


//--------------------------------------------------------------------
  void  BNameBlock::PutClass (BClass* cls)
//--------------------------------------------------------------------
{ 
  assert(!class_);
  class_ = cls; 
  if(class_)
  {
    class_->IncNRefs();
  }
}

//--------------------------------------------------------------------
BUserNameBlock* BNameBlock::Owner() const 
//--------------------------------------------------------------------
{ 
  return(owner_); 
}

//--------------------------------------------------------------------
void BNameBlock::PutOwner(BUserNameBlock* owner) 
//--------------------------------------------------------------------
{ 
  owner_ = owner; 
}

//--------------------------------------------------------------------
const BText& BNameBlock::Name() const
//--------------------------------------------------------------------
{
  return(BObject::Name());
}

//--------------------------------------------------------------------
const BText& BNameBlock::LocalName() const 
//--------------------------------------------------------------------
{ 
  return(localName_); 
};

//--------------------------------------------------------------------
  short BNameBlock::EnsureIsAssigned() const
//--------------------------------------------------------------------
{
#if (__USE_POOL__==__POOL_BFSMEM__)
  if(createdWithNew_) 
  { 
    short isAssigned = BFSMEM_Hndlr->IsAssigned(this,this->GetPageNum());
    return(isAssigned); 
  }
  else        
#endif       
  { 
    return(true); 
  }
}

//--------------------------------------------------------------------
  bool BNameBlock::IsInstanceOf(const BClass* cls) const
//--------------------------------------------------------------------
{
  if(!class_) { return(false); }
  return(class_->InheritesFrom(cls));
}

//--------------------------------------------------------------------
  bool BNameBlock::IsInstanceOf(const BText& name) const
//--------------------------------------------------------------------
{
  if(!class_) { return(false); }
  return(class_->InheritesFrom(name));
}

//--------------------------------------------------------------------
  int BNameBlock::Level() const 
//--------------------------------------------------------------------
{ 
  BNameBlock* T = (BNameBlock*)this;
  if(level_==-999999999)
  {
    if(father_) { T->level_ = father_->Level(); }
    else        { T->level_ = evLevel_; }
  }
  return(level_); 
}

//--------------------------------------------------------------------
  const BNameBlock* BNameBlock::Current() 
//--------------------------------------------------------------------
{ 
#if (__USE_POOL__==__POOL_BFSMEM__)
  if(current_ && 
     current_->createdWithNew_ &&
     !(BFSMEM_Hndlr->IsAssigned(current_,current_->GetPageNum())))
  {
    BFSMEM_Hndlr->IsAssigned(current_,current_->GetPageNum());
    current_ = NULL;
  }
#endif
  return(current_); 
}

//--------------------------------------------------------------------
  const BNameBlock* BNameBlock::SetCurrent(const BNameBlock* ns) 
//--------------------------------------------------------------------
{ 
  const BNameBlock* old = current_; 
  current_ = ns; 
  return(old); 
}

//--------------------------------------------------------------------
  BUserNameBlock* BNameBlock::Building() 
//--------------------------------------------------------------------
{ 
#if (__USE_POOL__==__POOL_BFSMEM__)
  if(building_ && 
     building_->Contens().createdWithNew_ &&
     !(BFSMEM_Hndlr->IsAssigned(&building_->Contens(),
                                building_->Contens().GetPageNum())))
  {
    BFSMEM_Hndlr->IsAssigned(&building_->Contens(),
                             building_->Contens().GetPageNum());
    building_ = NULL;
  }
#endif
  return(building_); 
}

//--------------------------------------------------------------------
  BUserNameBlock* BNameBlock::SetBuilding(BUserNameBlock* ns) 
//--------------------------------------------------------------------
{ 
  BUserNameBlock* old = building_; 
  building_ = ns; 
  return(old); 
}


//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::EvaluateTree(const List* _tre)
//--------------------------------------------------------------------
{
  int n;
  BSyntaxObject* obj = NULL;
  BToken* tok = (BToken*)_tre->car();
  bool isDefaultInstance = tok->Name()=="#DefaultInstance";
  BText name  = BEqualOperator::CreatingName();
  BText fullName = BEqualOperator::CurrentFullName();
  const BClass* cls = BEqualOperator::CreatingClass();
  if(cls && cls->notImplementedMethods_>0)
  {
    BText undefMet;
    BMemberOwner::BMemberByNameHash::const_iterator iterM;
    BMemberOwner::BMemberByNameHash& mbrDec = *(cls->mbrDecHash_);
    n=0;
    for(iterM=mbrDec.begin(); iterM!=mbrDec.end(); iterM++, n++)
    {
      BMember* mbr = iterM->second;
      if(mbr->isMethod_)
      {
        undefMet += BText("\n  ")+mbr->FullExpression();
      }
    }
    Error(I2("Cannot create NameBlock ", "No se puede crear el NameBlock ")+
          fullName+ 
          I2(" an instance of Class ", " como instancia de la clase ")+
          cls->FullName()+
          I2(" due to it have "," porque tiene ")+cls->notImplementedMethods_+
          I2(" not defined methods "," métodos no definidos ")+undefMet);
    return(NULL);
  };

  BBool hasName = name.HasName();
//Std(BText("BNameBlock::EvaluateTree:\n")+BParser::treWrite((List*)_tre,"  "));
  BUserNameBlock* ns_result = NULL;
  BSyntaxObject* result = NULL;
  int level = BGrammar::Level();
  if((level>0) && hasName)
  {
    result = BGrammar::FindLocal(name);
  }
  if(result && (result->Level()!=level)) 
  { 
    result = NULL; 
  }
  else 
  { 
    result = GraAnything()->FindVariable(name); 
  }
  if(result)
  {
    Error(I2("Cannot create NameBlock named ",
             "No se puede crear un NameBlock de nombre ")+fullName+
          I2(" due to "," porque ")+name+
          I2(" is already in use\n",
             " ya está siendo usado\n")+ ((!BNameBlock::current_)?"":
          I2("Current NameBlock is ",
             "El NameBlock en curso es ")+BNameBlock::current_->Name()));
    return(NULL);
  }
  BUserNameBlock* oldBuilding = BNameBlock::Building();
  BGrammar::IncLevel();
  int stackPos = BGrammar::StackSize();
  ns_result = new BGraContensP<BNameBlock>(name, new BNameBlock);
  
  BNameBlock::SetBuilding((BUserNameBlock*)ns_result);
  int oldBuildingLevel = -1;
  int ns_resultLevel = -1; 
  if(oldBuilding && !BStandardOperator::evaluatingFunctionArgument_)
  {
    assert(name.HasName());
    oldBuildingLevel = oldBuilding->Level();
    ns_resultLevel = ns_result->Level();
    if(oldBuildingLevel == level-1)
    { 
      oldBuilding->Contens().AddElement(ns_result, true, false);
    //BSyntaxObject* found = GraNameBlock()->EvaluateExpr(fullName);
    //assert(found);
    }
  } 
  BNameBlock& newNameBlock  = ns_result->Contens();
  newNameBlock.PutOwner(ns_result);
  newNameBlock.PutName(fullName);
  newNameBlock.PutLocalName(name);
  int oldErr = (int)TOLErrorNumber().Value();
  newNameBlock.Set().PutNameBlock(&newNameBlock);
  BGrammar::IncLevel();
  if(!cls)
  {
    List* b = _tre->cdr();
    n = 0;
    while(b)
    {
      n++;
      b = b->cdr();
    };
    b = _tre->cdr();
    newNameBlock.Set().PrepareStore(n);
    while(b)
    {
      const List* br = (const List*)b->car();
      if(br)
      { 
        BToken* tok = (BToken*)br->car();
        if((tok->TokenType()==TYPE) && (tok->Name()=="#Require"))
        {
          const BText& package = 
            ((BToken*)(((const List*)(br->cdr()->car()))->car()))->Name();
          newNameBlock.AddRequiredPackage(package);
        } 
        else
        {
          obj = GraAnything()->EvaluateTree(br);
          if(obj)
          {
            newNameBlock.AddElement(obj,true,false);
          }
        }
      }
      b = b->cdr();
    };
  }
  else
  {
    List* tre = NULL;
    if(!isDefaultInstance) { tre = _tre->duplicate(); }
    BNameBlockMemberOwner memberOwner(&newNameBlock);
    memberOwner.PutTree(tre);
    memberOwner.CreateParentHashes();
    memberOwner.CreateMemberHashes();
    memberOwner.AddParent((BClass*)cls); 
    if(!isDefaultInstance) { 
    //Std(BText("BNameBlock::EvaluateTree:\n")+BParser::treWrite((List*)tre,"  "));
      memberOwner.AddMemberList(tre->cdr());
    }
    memberOwner.SortMembers();
    int stackPos = BGrammar::StackSize();
    if(memberOwner.isGood_ && memberOwner.mbrDecHash_->size())
    {
      BText undefMem = "";
      BMemberOwner::BMemberByNameHash::const_iterator iterM;
      BMemberOwner::BMemberByNameHash& mbrDec = *memberOwner.mbrDecHash_;
      n=0;
      for(iterM=mbrDec.begin(); iterM!=mbrDec.end(); iterM++, n++)
      {
        BMember* mbr = iterM->second;
        undefMem += BText("\n  ")+mbr->FullExpression();
      }
      Error(I2("Cannot create instance ",
               "No se pudo crear la instancia ")+fullName+
            I2(" of class "," de la clase ")+cls->Name()+
            I2(" due to there are not implemented methods ",
            " debido a que hay miembros no implementados ")+
            ":"+undefMem+"\n");
      memberOwner.isGood_ = false;
    }
    if(memberOwner.isGood_)
    {
      int stackPos = BGrammar::StackSize();
      BMember* mbr= NULL;
      obj = NULL;
      for(n=0; n<memberOwner.member_.Size(); n++)
      {
        mbr = memberOwner.member_[n]->member_; 
        if(mbr && mbr->method_) 
        { 
          mbr->method_->PutNameBlock(NULL); 
          if(mbr->method_->Mode()==BOBJECTMODE)
          {
            BUserCode* uCode = UCode(mbr->method_);
            BOperator* opr = uCode->Contens().Operator();
            if(opr) { opr->PutNameBlock(NULL); }
          }
        }
        if(mbr && mbr->static_) 
        { 
          mbr->static_->PutNameBlock(NULL); 
          if(mbr->isMethod_ && mbr->static_->Mode()==BOBJECTMODE)
          {
            BUserCode* uCode = UCode(mbr->static_);
            BOperator* opr = uCode->Contens().Operator();
            if(opr) { opr->PutNameBlock(NULL); }
          }
        }
      }
      newNameBlock.Set().PrepareStore(memberOwner.member_.Size());
      const BClass* oldClass = BClass::currentClassBuildingInstance_;
      newNameBlock.PutClass((BClass*)cls); 
      BClass::currentClassBuildingInstance_ = cls; 
      for(n=0; n<memberOwner.member_.Size(); n++)
      {
        mbr = memberOwner.member_[n]->member_; 
/*
        if(cls && cls->Name()=="BlockSamplerStdLinear" && name=="aux")
        {
        //Std(BText("\nTRACE BNameBlock::EvaluateTree member [")+(n+1)+"]"+
            fullName+"::"+mbr->name_+
            "  position_="+(memberOwner.member_[n]->position_+1)+"\n");
          if(mbr->name_=="_do_buildWorkSpace")
            printf("");
        }
*/
        if(!mbr->method_ && !mbr->static_)
        {
          obj = GraAnything()->EvaluateTree(mbr->branch_);
          newNameBlock.AddElement(obj, true, false);
        }
      }
      BClass::currentClassBuildingInstance_ = oldClass;
    } 
    if(!memberOwner.isGood_) { DESTROY(ns_result) }
  }
  BGrammar::DecLevel();
  BGrammar::DestroyStackUntil(stackPos, ns_result);    
  if(ns_result)
  { 
    newNameBlock.CheckMembers();
    newNameBlock.SetAutodocMember();
  }
  int numErr = (int)TOLErrorNumber().Value()-oldErr;
  if(numErr)
  {
    Warning(BText("NameBlock ")+name+" "+
            I2("has been built with",
               "se ha construido con") + " "+numErr+" "+
            I2("errors.","errores."));
  }
  BNameBlock::SetBuilding(oldBuilding);
  BGrammar::DecLevel();
  return(ns_result);
}

//--------------------------------------------------------------------
  const BClass* BNameBlock::Class() const
//--------------------------------------------------------------------
{
  return(class_);
}
//--------------------------------------------------------------------
	BText BNameBlock::Dump() const
//--------------------------------------------------------------------
{
//return(BParser::Unparse(tree_,"","\n"));
  BText txt = "[[\n  ";
  BText end = "\n]]";
  BText separator = ";\n  ";
  BInt n;
  for(n=1; n<=set_.Card(); n++)
	{
    txt += 
      set_[n]->Grammar()->Name()+" "+
      set_[n]->Name()+" = "+
      set_[n]->Dump(); 
	  if(n!=set_.Card())
	  {
		  txt += separator;
	  }
  }
  txt += end;
  return(txt);
}

//--------------------------------------------------------------------
  bool BNameBlock::AddElement(BSyntaxObject* obj, bool addToSet, bool fromOis)
//--------------------------------------------------------------------
{
  if(!obj) { return(true); }
  const BText& name = obj->Name();
  int i;
  if(!name.HasName() || 
     !BParser::DefaultParser()->Filter()->IsIdentifier(name))
  {
    BText set_dump = "";
    for(i=1; i<=set_.Card(); i++)
    {
      set_dump += BText("  ")+set_[i]->Grammar()->Name()+
                         " "+set_[i]->Name()+"\n";
    };
    Error(I2("A NameBlock cannot have members without a valid name ",
             "Un NameBlock no puede tener miembros sin un nombre válido ")
          +"'"+name+"'\n"+set_dump+"\n"+
          I2("Cannot build NameBlock ",
             "No se puede construir el NameBlock ")+Name());

    return(false);
  }
  if(name=="_this")
  {
    Error(I2("Reserved member name ",
             "Nombre de miembro reservado  ")+ name+"\n"+
            I2("Cannot build NameBlock ",
               "No se puede construir el NameBlock ")+Name());
    return(false);
  }
  BSyntaxObject* mem = Member(name);
  if(mem)
  {
    if(mem==obj) 
    { 
      return(true); 
    }
    BUserNameBlock* obj_uns = NULL;
    BUserNameBlock* mem_uns = NULL;
    if(obj->Grammar()==GraNameBlock())
    {
      obj_uns = (BUserNameBlock*)obj;
      mem_uns = (BUserNameBlock*)mem;
      if(&obj_uns->Contens() == &mem_uns->Contens())
      { 
        return(true);  
      }
    }
    Warning(I2("Duplicated member name ",
               "Nombre de miembro duplicado ")+ name+"\n"+
            I2("It will be ignored in NameBlock ",
               "Sera ignorado en el el NameBlock ")+Name());
    return(false);  
  }
  obj->PutNameBlock(this);
/*
  if((obj->Grammar()==GraCode())&&(obj->Mode()==BOBJECTMODE))
  {
    BOperator* opr = Code(obj).Operator();
    if(!(opr->NameBlock()))
    {
      opr->PutNameBlock(this);
    }
  }
*/
  if(name[0]!='_')
  {
    nonPrivateMembers_++;
    public_[name] = obj;
  //Std(BText("\nTRACE BNameBlock::Build added public member ")+Name()+"::"+name+"\n");
  }
  else
  {
    if(name[1]=='.') 
    { 
      //read only member
      nonPrivateMembers_++; 
    //Std(BText("\nTRACE BNameBlock::Build added read only member ")+Name()+"::"+name+"\n");
      if(!fromOis && (name=="_.autodoc.dependencies"))
      {
        if(obj->Grammar()!=GraSet())
        { 
          Error(I2("Wrong declaration of ","Definición errónea de ")+
                obj->Grammar()->Name()+" "+LocalName()+"::_.autodoc.dependencies"+
                I2("It should be of type ","Debería ser de tipo ")+"Set");
          return(false);
        }
        BSet& dep = ::Set(obj); 
        if(dep.Card())
        { 
          Warning(I2("Obsolete declaration of ","Definición obsoleta de ")+
                obj->Grammar()->Name()+" "+LocalName()+"::_.autodoc.dependencies"+
                I2("It should be empty due it will be filled with required packages",
                   "Debería estar vacío porque se rellena automáticamente con los "
                   "paquetes requeridos"));
        }
        int reqNum = CountRequiredPackage();
        dep.Delete();
        dep.PrepareStore(reqNum);
        int d;
        for(d=0; d<reqNum; d++)
        {
          BText txt = GetRequiredPackage(d);
          dep.AddElement(new BContensText("",txt,""));
        }
      }
      else  if(!fromOis && (name=="_.autodoc.name"))
      {
        if(obj->Grammar()!=GraText())
        { 
          Error(I2("Wrong declaration of ","Definición errónea de ")+
                obj->Grammar()->Name()+" "+LocalName()+"::_.autodoc.name"+
                I2("It should be of type ","Debería ser de tipo ")+"Text");
          return(false);
        }
        BText& nt = Text(obj); 
        if(nt.HasName() && (nt!=LocalName()))
        {
          Error(I2("Wrong declaration of ","Definición errónea de ")+
                obj->Grammar()->Name()+" "+LocalName()+"::_.autodoc.name = \""+nt+"\";\n"+
                I2("It should be ","Debería ser \"")+LocalName()+"\"");
          return(false);
        } 
        nt = LocalName();
      }

    }
    private_[name] = obj;
  }
  if(obj->Grammar()==GraCode())
  {
    int mode = obj->Mode();
    if(mode==BOBJECTMODE)
    {
      BUserCode* uCode = UCode(obj);
      BOperator* opr   = GetOperator(uCode);
      if(opr && !opr->System() && !opr->NameBlock() && 
        (opr->Name()==obj->Name()) && 
        (opr->Level()==obj->Level()))
      {
        opr->PutNameBlock(this);
      }
    }
  }
  if(addToSet) { set_.AddElement(obj); }
  return(true);
}  

//--------------------------------------------------------------------
  bool BNameBlock::CheckMembers()
//--------------------------------------------------------------------
{
  return(true);
/*
  if((nonPrivateMembers_==0)&&!class_)
  {
    Warning(BText("NameBlock ")+Name()+" "+
            I2("should have at least a public or read only member.",
               "debería tener al menos un miembro público o de sólo lectura."));
    return(false);
  }
  else
  {
    return(true);
  }
*/
}

//--------------------------------------------------------------------
  bool BNameBlock::SetAutodocMember()
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = NULL;
  BObjByNameHash::const_iterator iter;
  for(iter=public_.begin(); iter!=public_.end(); iter++)
  {
    obj = iter->second;
    const BText& dsc = obj->Description();
    if(!dsc.HasName())
    {
      if(class_)
      {
        BMember* mbr = class_->FindMember(obj->Name());
        if(mbr) { obj->PutDescription(mbr->description_); }
      }
      else
      {
        BText autodocName = BText("_.autodoc.member.")+obj->Name();
        BSyntaxObject* autodoc = NULL;
        autodoc = Member(autodocName);
        if(autodoc && autodoc->Grammar()==GraText())
        {
          BText& desc = Text(autodoc);
          obj->PutDescription(desc);
        }
      }
    }
  }
  for(iter=private_.begin(); iter!=private_.end(); iter++)
  {
    obj = iter->second;
    const BText& dsc = obj->Description();
    if(!dsc.HasName())
    {
      if(class_)
      {
        BMember* mbr = class_->FindMember(obj->Name());
        if(mbr) { obj->PutDescription(mbr->description_); }
      }
      else
      {
        BText autodocName = BText("_.autodoc.member.")+obj->Name();
        BSyntaxObject* autodoc = NULL;
        autodoc = Member(autodocName);
        if(autodoc && autodoc->Grammar()==GraText())
        {
          BText& desc = Text(autodoc);
          obj->PutDescription(desc);
        }
      }
    }
  }
  return(true);
}


//--------------------------------------------------------------------
  bool BNameBlock::Build()
//--------------------------------------------------------------------
{
  int i, j;
  BSyntaxObject* obj;
  nonPrivateMembers_ = 0;
  for(i=j=0; i<set_.Card(); i++)
  {
    obj = set_[i+1];
    AddElement(obj, false, false);
  }
  bool ok = CheckMembers();
  if(ok) { SetAutodocMember(); }
  return(ok);
}

//--------------------------------------------------------------------
  bool BNameBlock::Fill(const BSet& set)
//--------------------------------------------------------------------
{
  set_ = set;
  set_.PutNameBlock(this);
  return(Build());
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::PublicMember(const BText& memberName) const
//--------------------------------------------------------------------
{
/* */
  BSyntaxObject* result = NULL;
  if(Class())
  {
    BSyntaxObject* met = Class()->FindMethod(memberName,false);
    if(met) 
    { 
      met->PutNameBlock(this);
      if(met->Mode()==BOBJECTMODE)
      {
        BUserCode* uCode = UCode(met);
        BOperator* opr = uCode->Contens().Operator();
        if(opr) { opr->PutNameBlock(this); }
      }
      result = met; 
    } 
  }
/* */
  if(!result)
  {
    BObjByNameHash::const_iterator found = public_.find(memberName);
    if(found!=public_.end())
    {
      result = found->second;
    }
  }
  return(result);
}


//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::PrivateMember(const BText& memberName) const
//--------------------------------------------------------------------
{
/* */
  BSyntaxObject* result = NULL;
  if(Class())
  {
    BSyntaxObject* met = Class()->FindMethod(memberName,true);
    if(met) 
    { 
      met->PutNameBlock(this);
      if(met->Mode()==BOBJECTMODE)
      {
        BUserCode* uCode = UCode(met);
        BOperator* opr = uCode->Contens().Operator();
        if(opr) { opr->PutNameBlock(this); }
      }
      result = met; 
    } 
  }
/* */
  if(!result)
  {
    BObjByNameHash::const_iterator found = private_.find(memberName);
    if(found==private_.end())
    {
      if(memberName=="_this")
      {
        result = owner_;
      }
    }
    else
    {
      result = found->second;
    }
  }
  return(result);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::Member(const BText& memberName) const
//--------------------------------------------------------------------
{
  if(memberName[0]=='_')
  {
    return(PrivateMember(memberName));
  }
  else
  {
    return(PublicMember(memberName));
  }
}

//--------------------------------------------------------------------
static bool isSpecial(const BText& name)
//--------------------------------------------------------------------
{
  return(name=="StartActions");
}

//--------------------------------------------------------------------
bool BNameBlock::add_using_symbol(
  const BText& name,
  BObjByNameHash::const_iterator iter,
  bool usingAlsoReadOnly,
  bool usingAlsoPrivate,
  bool usingAlsoSpecial
)
//--------------------------------------------------------------------
{
  if((iter->first)[0]=='_')
  {
    if(!usingAlsoReadOnly && ((iter->first)[1]=='.')) { return(true); }
    if(!usingAlsoPrivate  && ((iter->first)[1]!='.')) { return(true); }
  }
  if(!usingAlsoSpecial && isSpecial(iter->first)) { return(true); }
  bool ok = true;
  BObjByNameHash::const_iterator found ;
  BObjByClassNameHash::iterator fc;
  found = usingSymbols_.find(iter->first);
  if(found==usingSymbols_.end())
  {
  //Std(BText("\nUsing member '")+name+"::"+iter->first+"'\n");
    usingSymbols_[iter->first]=iter->second;
    assert(usingSymbols_.find(iter->first)!=usingSymbols_.end());
    BObjClassify oc(iter->second);
    fc = usingSymbolsByClass_.find(oc);
    BObjByNameHash* x;
    if(fc!=usingSymbolsByClass_.end())
    {
      x = fc->second;
    }
    else
    {
      x = new BObjByNameHash;
      SetEmptyKey  ((*x), NULL);
      SetDeletedKey((*x), name_del_key());
      usingSymbolsByClass_[oc] = x;
    }
    (*x)[iter->first]=iter->second;
  }
  else
  {
    Warning(found->second->FullName()+
            I2(" is already in use as global. "
               "So cannot add ",
               " ya se está usando como global. "
               "Por lo tanto no se puede añadir ")+
               name+"::"+iter->second->Name());
    ok = false;
  }
  return(true);
}
//--------------------------------------------------------------------
  bool BNameBlock::Using(
    const BSyntaxObject* obj,
    bool usingAlsoReadOnly,
    bool usingAlsoPrivate,
    bool usingAlsoSpecial)
//--------------------------------------------------------------------
{
  assert(obj && (obj->Grammar()==GraNameBlock()));
  BUserNameBlock* uns = UNameBlock((BSyntaxObject*)obj);
  const BText& name = uns->Name();
  if(!name.HasName()) 
  { 
    Error(I2("Cannot use unnamed NameBlock as global",
             "No se puede usar un NameBlock sin nombre como global"));
    return(false); 
  }
  BObjByNameHash::const_iterator nmspcIt = using_.find(name);
  if(nmspcIt!=using_.end()) 
  { 
    Error(BText("NameBlock ")+name+
          I2(" is already in use as global",
             " ya se está usando como global"));
    return(false);
  }
  bool ok = true;
  BObjByNameHash::const_iterator iter;
  BNameBlock& ns = uns->Contens();
  BObjByNameHash& pbm = ns.Public();
  for(iter=pbm.begin(); iter!=pbm.end(); iter++)
  {
    ok = add_using_symbol(name, iter, false, false, usingAlsoSpecial);
  }
  BObjByNameHash& prm = ns.Private();
  for(iter=prm.begin(); iter!=prm.end(); iter++)
  {
    ok = add_using_symbol(name, iter, 
     usingAlsoReadOnly, usingAlsoPrivate, usingAlsoSpecial);
  }
  return(ok);
}

//--------------------------------------------------------------------
  void BNameBlock::Unusing(const BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  BUserNameBlock* uns = UNameBlock((BSyntaxObject*)obj);
  BNameBlock& ns = uns->Contens();
  BObjByNameHash& pbm = ns.Public();
  BObjByNameHash::const_iterator iter;
  BObjByClassNameHash::iterator fc;
  for(iter=pbm.begin(); iter!=pbm.end(); iter++)
  {
    if(iter->second->NameBlock()==&ns)
    {
      usingSymbols_.erase(iter->first);
      BObjClassify oc(iter->second);
      fc = usingSymbolsByClass_.find(oc);
      assert(fc!=usingSymbolsByClass_.end());
      fc->second->erase(iter->first);
    }
  }
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::DeepMember(const BText& memberName) const
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  result = Member(memberName);
  if(!result && Class())
  {
    BSyntaxObject* met = Class()->FindMethod(memberName,true);
    if(met) 
    { 
      met->PutNameBlock(this);
      if(met->Mode()==BOBJECTMODE)
      {
        BUserCode* uCode = UCode(met);
        BOperator* opr = uCode->Contens().Operator();
        if(opr) { opr->PutNameBlock(this); }
      }
      result = met; 
    }
    if(!result && Class()->NameBlock())
    {
      result = Class()->NameBlock()->DeepMember(memberName);
    }  
  }
/*
  if(!result && Father())
  {
    if(memberName=="g")
      printf("");
    result = Father()->DeepMember(memberName);
  }
*/
  if(result && (result->Mode()==BSTRUCTMODE))
  {
    BStruct* bstr = (BStruct*)result;
    result = bstr->Function();
  }
  return(result);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::LocalMember(const BText& memberName)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  const BNameBlock* cns = BNameBlock::Current();
  if(cns) { result = cns->DeepMember(memberName); }
  return(result);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::UsingMember(const BText& memberName)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BObjByNameHash::const_iterator found = usingSymbols_.find(memberName);
  if(found!=usingSymbols_.end())
  {
    result = found->second;
  }
  return(result);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::UsingMember(const BObjClassify&  oc, 
                                         const BText&    memberName)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  BObjByClassNameHash::iterator fc = usingSymbolsByClass_.find(oc);
  if(fc==usingSymbolsByClass_.end()) { return(NULL); }
  BObjByNameHash::const_iterator found = fc->second->find(memberName);
  if(found!=fc->second->end())
  {
    result = found->second;
  }
  return(result);
}

//--------------------------------------------------------------------
  BList* BNameBlock::SelectMembers(BList* lst, const BObjClassify&  oc)
//--------------------------------------------------------------------
{
//Std(BText("\n")+"BNameBlock::SelectMembers Exploring NameBlock "+Name());
  BSyntaxObject* obj;
  BObjByNameHash::const_iterator iter;
//Std(BText("\n")+" Seaching for "+GetModeName(oc.mode_)+" "+oc.grammar_->Name());
  for(iter=public_.begin(); iter!=public_.end(); iter++)
  {
    obj = iter->second;
    if((oc.mode_==BUSERFUNMODE) && 
       (obj->Mode()==BOBJECTMODE) && 
       (obj->Grammar()==GraCode()))
    {
      BUserCode* uc = (BUserCode*)obj;
      obj = uc->Contens().Operator();
    }
    bool match = (obj->Grammar()==oc.grammar_) && (obj->Mode()==oc.mode_);
    if(match)
    {
    //Std(BText("\n")+"  "+GetModeName(obj->Mode())+" "+obj->Grammar()->Name()+" "+obj->Name()+" matches? "+int(match));
      lst = Cons(obj,lst);
    }
  }
  for(iter=private_.begin(); iter!=private_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Name()[2]=='.')&&
       (oc.mode_==BUSERFUNMODE) && 
       (obj->Mode()==BOBJECTMODE) && 
       (obj->Grammar()==GraCode()))
    {
      BUserCode* uc = (BUserCode*)obj;
      obj = uc->Contens().Operator();
    }
    bool match = (obj->Grammar()==oc.grammar_) && (obj->Mode()==oc.mode_);
    if(match)
    {
    //Std(BText("\n")+"  "+GetModeName(obj->Mode())+" "+obj->Grammar()->Name()+" "+obj->Name()+" matches? "+int(match));
      lst = Cons(obj,lst);
    }
  }
  return(lst);
}

//--------------------------------------------------------------------
  BList* BNameBlock::SelectMembersDeep(BList* lst, const BObjClassify&  oc)
//--------------------------------------------------------------------
{
//Std(BText("\n")+"BNameBlock::SelectMembersDeep Exploring NameBlock "+Name());
  lst = SelectMembers(lst, oc);
  BSyntaxObject* obj;
  BObjByNameHash::const_iterator iter;
  for(iter=public_.begin(); iter!=public_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      BUserNameBlock* unb = (BUserNameBlock*)obj;
      lst = unb->Contens().SelectMembersDeep(lst, oc);
    }
  }
  for(iter=private_.begin(); iter!=private_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Name()[2]=='.')&&
       (obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      BUserNameBlock* unb = (BUserNameBlock*)obj;
      lst = unb->Contens().SelectMembersDeep(lst, oc);
    }
  }
  return(lst);
}


//--------------------------------------------------------------------
  void BNameBlock::RebuildFullNameDeep(BText parentFullName, BText memberName)
//--------------------------------------------------------------------
{
//Std(BText("\n")+"BNameBlock::SelectMembersDeep Exploring NameBlock "+Name());
  if(doingRebuildFullNameDeep) 
  { return; }
  doingRebuildFullNameDeep = true;
  BSyntaxObject* obj;
  BUserNameBlock* unb;
  BObjByNameHash::const_iterator iter;
  if(!memberName.HasName()) 
  {
    memberName = LocalName();
  } 
  if(parentFullName.HasName()) 
  { 
    parentFullName = parentFullName+"::"+memberName; 
  }
  else
  {
    parentFullName = memberName; 
  }
  PutName(parentFullName);
  for(iter=public_.begin(); iter!=public_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      unb = (BUserNameBlock*)obj;
      unb->Contens().father_ = this;
      unb->Contens().RebuildFullNameDeep(parentFullName, obj->Name());
    }
  }
  for(iter=private_.begin(); iter!=private_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      unb = (BUserNameBlock*)obj;
      unb->Contens().father_ = this;
      unb->Contens().RebuildFullNameDeep(parentFullName, obj->Name());
    }
  }
  doingRebuildFullNameDeep = false;
}


//--------------------------------------------------------------------
  BList* BNameBlock::Select(BList* lst, const BObjClassify&  oc)
//--------------------------------------------------------------------
{
  BObjByClassNameHash::const_iterator fc = usingSymbolsByClass_.find(oc);
  if(fc==usingSymbolsByClass_.end()) { return(lst); }
  BSyntaxObject* obj;
  BObjByNameHash::const_iterator iter;
  for(iter=fc->second->begin(); iter!=fc->second->end(); iter++)
  {
    obj = iter->second;
    assert(obj->Grammar()==oc.grammar_);
    assert(obj->Mode   ()==oc.mode_);
    lst = Cons(obj,lst);
  }
  return(lst);
}

//--------------------------------------------------------------------
BRequiredPackage::BRequiredPackage()
//--------------------------------------------------------------------
: hash_(),
  array_(0)
{
  SetEmptyKey(hash_ ,NULL);
};
//--------------------------------------------------------------------
bool BRequiredPackage::AddRequiredPackage(const BText& name)
//--------------------------------------------------------------------
{
  BIntByNameHash::const_iterator fc = hash_.find(name);
  if(fc==hash_.end())
  {
    int s = array_.Size();
    array_.Add(name);
    hash_[ array_[s] ] = s;
    return(true);
  }
  else
  {
    return(false);
  }
};
//--------------------------------------------------------------------
  int BRequiredPackage::CountRequiredPackage() const
//--------------------------------------------------------------------
{
  return(array_.Size());
}

//--------------------------------------------------------------------
  const BText& BRequiredPackage::GetRequiredPackage(int k) const
//--------------------------------------------------------------------
{
  return(array_[k]);
}


//--------------------------------------------------------------------
  bool BNameBlock::AddRequiredPackage(const BText& name)
//--------------------------------------------------------------------
{
  if(!requiredPackages_)
  {
    requiredPackages_ = new BRequiredPackage;
  }
  return(requiredPackages_->AddRequiredPackage(name));
}

//--------------------------------------------------------------------
  int BNameBlock::CountRequiredPackage() const
//--------------------------------------------------------------------
{
  if(!requiredPackages_) { return(0); }
  else { return(requiredPackages_->CountRequiredPackage()); }
}

//--------------------------------------------------------------------
  const BText& BNameBlock::GetRequiredPackage(int k) const
//--------------------------------------------------------------------
{
  static BText noPackage = "";
  if(!requiredPackages_) { return(noPackage); }
  else { return(requiredPackages_->GetRequiredPackage(k)); }
}

//--------------------------------------------------------------------
  bool BNameBlock::AddGlobalRequiredPackage(const BText& name)
//--------------------------------------------------------------------
{
  return(globalRequiredPackages_.AddRequiredPackage(name));
}

//--------------------------------------------------------------------
  int BNameBlock::CountGlobalRequiredPackage()
//--------------------------------------------------------------------
{
  return(globalRequiredPackages_.CountRequiredPackage()); 
}

//--------------------------------------------------------------------
  const BText& BNameBlock::GetGlobalRequiredPackage(int k)
//--------------------------------------------------------------------
{
  return(globalRequiredPackages_.GetRequiredPackage(k));
}

//--------------------------------------------------------------------
//fast accessing to recursive member expressions as 
// a::b:: ... ::z
BSyntaxObject* BNameBlock::FindPublicMember(
  const BGrammar* gra,
  const BText& memberExpression)
//--------------------------------------------------------------------
{
  BArray<BText> tok;
  BText expression = memberExpression;
  expression.Replace("::","¬");
  ReadAllTokens(expression, tok, '¬');
  if(tok.Size()<2) { return(NULL); }
  BSyntaxObject* unb = GraNameBlock()->FindOperand(tok[0],false);
  if(!unb) { return(NULL); }
  int i;
  for(i=1; i<tok.Size()-1; i++)
  {
    BNameBlock& nb = NameBlock(unb);
    unb = nb.PublicMember(tok[i]);
    if(!unb || unb->Grammar()!=GraNameBlock()) { return(NULL); }
  }
  BNameBlock& nb = NameBlock(unb);
  BSyntaxObject* result = nb.PublicMember(tok[i]); 
  if(gra&&result&&(gra!=GraAnything())&&(gra!=result->Grammar()))
  {
    result = NULL;
  }
  return(result);
}

//--------------------------------------------------------------------
//Gramatical items
//--------------------------------------------------------------------
template<>
BGrammar* BGraContensBase<BNameBlock>::ownGrammar_   = NIL;

DefineContensCommonOperators(BNameBlock, "NameBlock");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BNameBlock>::Do() 
//--------------------------------------------------------------------
{ 
  if(!this->flags_.calculated_ && !this->flags_.calculating_)
  {
    this->flags_.calculating_ = true;
    BSet& c = Contens().Set(); 
/*
    int i, n = c.Card();
    for(i=1; i<n; i++)
    {
      if(c[i])
      {
        c[i]->Do();
      }
    }
*/
    this->flags_.calculating_ = false;
  }

}

//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BNameBlock>::InitInstances()
{
  BNameBlock::Initialize();
  BTraceInit("BGraContens<BNameBlock>::InitInstances");
  BNameBlock empty;
  new BSystemNameBlock("NoNameBlock",empty,
    I2("Unknown NameBlock", "NameBlock desconocido."));

}

//--------------------------------------------------------------------
/*! Returns a valid constant time set for name.
 */
template<>
BSyntaxObject* BGraContensBase<BNameBlock>::FindConstant (const BText& name)
{ 
    return(NIL); 
}


//--------------------------------------------------------------------
/*! Returns a valid NameBlock for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BNameBlock>::Casting(BSyntaxObject* obj)
{
  if(!obj)                         { return(NIL); }
  if(obj->Grammar()==OwnGrammar()) { return(obj); }
  return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BNameBlock>::New(FILE* fil)
//--------------------------------------------------------------------
{
  return 0;
}


//--------------------------------------------------------------------
DeclareContensClass(BNameBlock, BNameBlockTemporary, BSetToNameBlock);
DefExtOpr(1, BSetToNameBlock, "SetToNameBlock", 1, 1, "Set",
 "(Set set)",
 I2("Converts a Set in a NameBlock",
    "Convierte un Set en un NameBlock"),
 BOperClassify::System_);
//--------------------------------------------------------------------
void BSetToNameBlock::CalcContens()
//--------------------------------------------------------------------
{
  const BSet& set = Set(Arg(1));
  if(set.Card())
  {
    const BText& name = BEqualOperator::CreatingName();
    BText fullName = name;
    BText currentFullName = "";
    if(BNameBlock::Current())
    {
      currentFullName = BNameBlock::Current()->Name();
      fullName = currentFullName+"::"+name;
    }
    else
    {
      currentFullName = "";
      fullName = name;
    }
    contens_.PutName(fullName);
    contens_.PutLocalName(name);
    contens_.Fill(set);
    contens_.RebuildFullNameDeep(currentFullName,"");
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BNameBlockToSet);
DefExtOpr(1, BNameBlockToSet, "NameBlockToSet", 1, 1, "NameBlock",
 "(NameBlock nameBlock)",
 I2("Converts a Set in a NameBlock",
    "Convierte un Set en un NameBlock"),
 BOperClassify::System_);
//--------------------------------------------------------------------
void BNameBlockToSet::CalcContens()
//--------------------------------------------------------------------
{
  const BNameBlock& nb = ((BUserNameBlock*)(Arg(1)))->Contens();
  contens_ = nb.Set();
  contens_.PutNameBlock(NULL);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BNameBlockMembers);
DefExtOpr(1, BNameBlockMembers, "Members", 1, 1, "NameBlock",
 "(NameBlock nameBlock)",
 I2("Returns a set with a register of information about each member "
    " of a NameBlock by mean of structure",
    "Devuelve un conjunto con un registro de información acerca "
    "de cada uno de los miembros de un NameBlock mediante la "
    "estructura")+"\n"
    "  @NameBlockMemberInfo {\n" 
    "    Text Type,\n"
    "    Text Name,\n"
    "    Text Access,\n"
    "    Text DefinedAtClass,\n"
    "    Text Description\n}\n",
 BOperClassify::System_);
//--------------------------------------------------------------------
void BNameBlockMembers::CalcContens()
//--------------------------------------------------------------------
{
  const BNameBlock& nb = ((BUserNameBlock*)(Arg(1)))->Contens();
  const BSet& members = nb.Set();
  int i;
  contens_.PrepareStore(members.Card());
  const BClass* cls = nb.Class();
  int nu_members_instance =(cls)?cls->member_.Size():0;
  int nu_members_non_instance = members.Card();
  int nu_members = nu_members_instance + nu_members_non_instance;
  for(i=1; i<=nu_members; i++)
  {
    BSyntaxObject* mbr = NULL;
    if(i<=nu_members_instance) 
    {
      BMember* clsMbr = cls->member_[i-1]->member_;
      if(clsMbr->static_) 
      { 
        continue; 
      }
      else if(clsMbr->method_) 
      { 
        mbr = clsMbr->method_; 
      }
      else if(clsMbr->name_[0]!='_')
      { 
        mbr = nb.PublicMember(clsMbr->name_); 
      }
      else if(clsMbr->name_[1]=='.')
      {
        mbr = nb.PrivateMember(clsMbr->name_);
      }
    }
    else
    {
      mbr = members[i-nu_members_instance];
      if(cls && mbr && cls->FindMember(mbr->Name())) { mbr = NULL; }
    }
    if(!mbr) { continue; }
    const BText& name = mbr->Name();
    if(name.BeginWith("_.autodoc.member.")) { continue; }
    BText grammar;
    int mode = mbr->Mode();
         if(mode==BSTRUCTMODE) { grammar="Struct"; }
    else if(mode==BCLASSMODE)  { grammar="Class"; }
    else                       { grammar=mbr->Grammar()->Name(); }
    BSet reg;
    reg.PrepareStore(5);
    BText access = "Public";
    if(name[0]=='_')
    {
      if(name[1]=='.') { access = "Read only"; }
      else             { access = "Private"; }
    }
    BText definedAtClass = "";
    if(cls)
    {
      BMember* clsMbr = cls->FindMember(name);
      if(clsMbr && clsMbr->parent_) 
      { 
        definedAtClass = clsMbr->parent_->getFullName();
      }
    }
    reg.AddElement(new BContensText("",grammar,""));
    reg.AddElement(new BContensText("",name,""));
    reg.AddElement(new BContensText("",access,""));
    reg.AddElement(new BContensText("",definedAtClass,""));
    reg.AddElement(new BContensText("",mbr->Description(),""));
    reg.PutStruct(strNameBlockMemberInfo_);
    contens_.AddElement(new BSetTuple("", reg));
  }
}

//--------------------------------------------------------------------
class BDatUsingNameBlock : public BDatTemporary
//--------------------------------------------------------------------
{
private:
  BUserNameBlock* uns_;
public:
  BDatUsingNameBlock(BList* arg) : BDatTemporary(arg) 
  {
    BList* lst = arg;
    uns_ = UNameBlock(lst->Car()); 
    lst = lst->Cdr();
    if(CheckNonDeclarativeAction("UsingNameBlock")) 
    { 
      contens_ = false; 
    }
    else
    { 
      bool usingAlsoReadOnly = false;
      bool usingAlsoPrivate = false;
      bool usingAlsoSpecial = false;
      if(lst && lst->Car())
      {
        usingAlsoReadOnly = Real(lst->Car())!=0.0;
        lst = lst->Cdr();
      }
      if(lst && lst->Car())
      {
        usingAlsoPrivate = Real(lst->Car())!=0.0;
        lst = lst->Cdr();
      }
      if(lst && lst->Car())
      {
        usingAlsoSpecial = Real(lst->Car())!=0.0;
        lst = lst->Cdr();
      }
      contens_ = BNameBlock::Using(uns_,usingAlsoReadOnly,usingAlsoPrivate,usingAlsoSpecial); 
    }
  }
 ~BDatUsingNameBlock()
  {
  //BNameBlock::Unusing(uns_);
  }
  void CalcContens() { }
  DeclareClassNewDelete(BDatUsingNameBlock);
};
DeclareEvaluator(BDatUsingNameBlock);


//--------------------------------------------------------------------
  DefExtOpr(1, BDatUsingNameBlock, "UsingNameBlock",1,4,
  "NameBlock Real Real Real",
  "(NameBlock nameBlock [, "
    "Real usingAlsoReadOnly = False,"
    "Real usingAlsoPrivate = False,"
    "Real usingAlsoSpecial = False])",
  I2("In certain circumstances the public members of a NameBlock can "
     "pass to the global scope, that is to say, it is possible to "
     "access to them without specifying it with :: \n"
     "This feature is allowed with the operator \n"
     "\n"
     "  Real UsingNameBlock nameblock\n" 
     "\n"
     "UsingNameBlock operator returns certain if nameblock is "
     "compatible with the rest of variables of NameBlock type "
     "that are already globalized, it's to say, if it does not "
     "contain any public member with the name that another "
     "globalized one or that a global variable. \n"
     "Members that already were present will not be accessible "
     "of implicit form but by means of :: and it will be sent a "
     "warning message to notice it to the user. \n"
     "This operator does not have desired effect if it is called in "
     "local scope, which includes within a NameBlock, and will give "
     "back an error if it is tried\n."
     "If usingAlsoReadOnly is true then read only members will be "
     "also exported.\n"
     "If usingAlsoPrivate is true then private members will be "
     "also exported.\n"
     "If usingAlsoSpecial is true then special members and methods "
     "as StartActions of packages will be also exported.\n"
     ,
     "En determinadas circunstancias los miembros públicos de un "
     "NameBlock pueden pasar al ámbito global, es decir, se puede "
     "acceder a ellos sin explicitarlo con :: \n"
     "Ello se logra con el operador\n"
     "\n"
     "  Real UsingNameBlock <nameblock_id>\n"
     "\n"
     "El operador UsingNameBlock devuelve cierto si es compatible "
     "con el resto de variables de tipo NameBlock actualmente "
     "globalizadas, es decir, si no contiene ningún miembro público "
     "con el mismo nombre que otro o que una variable global. \n"
     "Los miembros que ya estuvieran presentes no serán accesibles "
     "de forma implícita pero sí mediante :: y se mostrará un "
     "mensaje de aviso para advertirlo.\n"
     "Este operador no surte efecto si se llama en ambiente local, "
     "lo cual incluye dentro de un NameBlock, y devolverá un "
     "error si se intenta.\n"
     "Si usingAlsoReadOnly es cierto los miembros de solo lectura "
     "también serán exportados.\n"
     "Si usingAlsoPrivate es cierto los miembros privados "
     "también serán exportados.\n"
     "Si usingAlsoSpecial es cierto los miembros y métodos especiales "
     "como StartActions también serán exportados.\n"),
  BOperClassify::System_);

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtNameBlock);
DefExtOpr(1, BTxtNameBlock, "GetNameBlock", 1, 1, "Anything",
	  "(Anything obj)",
	  I2("Returns the full name of the NameBlock to which an object "
       "belongs. If it does not belong to any it gives back the empty "
       "string \"\".",
	     "Devuelve el nombre completo del NameBlock al que pertenece "
       "un objeto. Si no pertenece a ninguno devuelve la cadena "
       "vacía \"\" "),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BTxtNameBlock::CalcContens()
//--------------------------------------------------------------------
{
  const BNameBlock* ns = Arg(1)->NameBlock();
  if(ns)
  {
    contens_=ns->Name();
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BText, BTxtTemporary, BTxtFullName);
DefExtOpr(1, BTxtFullName, "FullName", 1, 1, "Anything",
	  "(Anything obj)",
	  I2("Returns the full name of an object, including the prefix of "
       "its NameBlock if is inside one.",
	     "Devuelve el nombre completo de un objeto, incluyendo el "
       "prefijo de su NameBlock si está dentro de uno."),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BTxtFullName::CalcContens()
//--------------------------------------------------------------------
{
  contens_=Arg(1)->FullName();
}


//--------------------------------------------------------------------
DeclareContensClass(BDat, BDatTemporary, BDatAddMember);
DefExtOpr(1, BDatAddMember, "AddMember", 2, 2, "NameBlock Anything",
	  "(NameBlock nb, Anything member)",
	  I2("Adds a new member to a NameBlock. You cannot add an object "
       "withoput a valid and non previously used name inside the "
       "NameBlock.\n"
       "To avoid problems it is best to create the new member in a "
       "local environment:\n"
       "  Real AddMember(nb,{Real _.newMember=2}); \n"
       "If used on an instance of a class, the new member will "
       "only affect that instance.\n"
       "Returns True if success. Otherwise, the consequences could "
       "be unpredictable, so it is not advisable to use this "
       "function except for expert users and for cause.",
	     "Añade un nuevo miembro a un NameBlock. No se puede añadir "
       "un objeto sin un nombre válido y no usado previamente "
       "dentro del NameBlock.\n"
       "Para evitar problemas es mejor crear el nuevo miembro en "
       "un entorno local:\n"
       "  Real AddMember(nb,{Real _.newMember=2}); \n"
       "Si se utiliza sobre una instancia de una clase el nuevo "
       "miembro sólo afectará a esa instancia.\n" 
       "Devuelve True en caso de éxito. En caso contrario las "
       "consecuencias son imprevisibles, por lo que no se aconseja "
       "el uso de esta función salvo para usuarios expertos y con "
       "causa justificada. "),
	  BOperClassify::System_);
//--------------------------------------------------------------------
void BDatAddMember::CalcContens()
//--------------------------------------------------------------------
{
  BNameBlock& nb = ((BUserNameBlock*)(Arg(1)))->Contens();
  contens_ = nb.AddElement(Arg(2), true, false);  
}


