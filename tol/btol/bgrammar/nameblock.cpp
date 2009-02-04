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

//--------------------------------------------------------------------
// Initialization
//   Global variables (static in the class BNameBlock) 
//--------------------------------------------------------------------
BTraceInit("nameblock.cpp");

const BNameBlock*         BNameBlock::current_ = NULL;
      BNameBlock*         BNameBlock::unknown_ = NULL;
      BObjClassify        BObjClassify::null_;
      BObjByNameHash      BNameBlock::using_;
      BObjByNameHash      BNameBlock::usingSymbols_;
      BObjByClassNameHash BNameBlock::usingSymbolsByClass_;


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
  SetEmptyKey  (using_,               NULL);
  SetEmptyKey  (usingSymbolsByClass_, BObjClassify::null_);
  SetEmptyKey  (usingSymbols_,        NULL);
  SetDeletedKey(usingSymbols_,        name_del_key());
  return(true);
}

//--------------------------------------------------------------------
BNameBlock::BNameBlock() 
//--------------------------------------------------------------------
: BObject   (),
  public_   (),
  private_  (), 
  set_      (),
  evLevel_  (BGrammar::Level()),
  level_    (-999999999),
  father_   (current_),
  localName_(),
  owner_    (NULL)
{
  SetEmptyKey(public_ ,NULL);
  SetEmptyKey(private_,NULL);
  short isAssigned = BObject::IsAssigned();
  createdWithNew_ = isAssigned!=-1;
}

//--------------------------------------------------------------------
BNameBlock::BNameBlock(const BText& fullName, const BText& localName) 
//--------------------------------------------------------------------
: BObject   (fullName), 
  public_   (),
  private_  (),
  set_      (),
  evLevel_  (BGrammar::Level()),
  level_    (-999999999),
  father_   (current_),
  localName_(localName),
  owner_    (NULL)
{
  SetEmptyKey(public_ ,NULL);
  SetEmptyKey(private_,NULL);
  short isAssigned = BObject::IsAssigned();
  createdWithNew_ = isAssigned!=-1;
}

//--------------------------------------------------------------------
BNameBlock::BNameBlock(const BNameBlock& ns) 
//--------------------------------------------------------------------
: BObject   (ns.Name    ()), 
  public_   (ns.Public  ()),
  private_  (ns.Private ()),
  set_      (ns.Set     ()),
  father_   (ns.Father  ()),
  localName_(ns.LocalName()),
  owner_    (NULL)
{
  short isAssigned = BObject::IsAssigned();
  createdWithNew_ = isAssigned!=-1;
  //VBR: La copia de NameBlock tiene un problema si los miembros
  //referenciados en ambos son los mismos, ¿cuál es su padre?
}

//--------------------------------------------------------------------
BNameBlock::~BNameBlock() 
//--------------------------------------------------------------------
{
}

//--------------------------------------------------------------------
BNameBlock& BNameBlock:: operator= (const BNameBlock& ns)
//--------------------------------------------------------------------
{
  PutName(ns.Name());
  owner_     = NULL;
  set_       = ns.Set();
  father_    = ns.Father();
  public_    = ns.Public();
  private_   = ns.Private();
  localName_ = ns.LocalName();
  //VBR: La copia de NameBlock tiene un problema si los miembros
  //referenciados en ambos son los mismos, ¿cuál es su padre?
  return(*this);
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
  if(createdWithNew_) { return(BObject::IsAssigned()); }
  else                { return(true); }
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
  const BNameBlock* BNameBlock::SetCurrent(const BNameBlock* ns) 
//--------------------------------------------------------------------
{ 
  const BNameBlock* old = current_; 
  current_ = ns; 
  return(current_); 
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::EvaluateTree(const List* tre)
//--------------------------------------------------------------------
{
  static BText currentFullName = "";
  const BText& name = BEqualOperator::CreatingName();
//Std(BText("BNameBlock::EvaluateTree:\n")+BParser::treWrite((List*)tre,"  "));
  BBool hasName = name.HasName();
  BText fullName = name;
  BText oldFullName = currentFullName;
  if(hasName) 
  {
    if(currentFullName.HasName())
    {
      fullName = currentFullName+"::"+name; 
    }
    currentFullName = fullName;
  }
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
  }
  else
  {
  //const BNameBlock* oldNameBlock = BNameBlock::current_;
    ns_result = new BGraContensP<BNameBlock>(new BNameBlock);
    BNameBlock& newNameBlock  = ns_result->Contens();
    newNameBlock.PutName(fullName);
    newNameBlock.PutLocalName(name);
  //BNameBlock::current_ = &newNameBlock;
    int oldErr = (int)TOLErrorNumber().Value();
    BGrammar::IncLevel();
    int stackPos = BGrammar::StackSize();
    BSyntaxObject* set_result = GraSet()->EvaluateTree(tre);
    BGrammar::DestroyStackUntil(stackPos, set_result);
    BGrammar::DecLevel();
    int numErr = (int)TOLErrorNumber().Value()-oldErr;
    if(!set_result || (set_result->Grammar()!=GraSet()))
    {
      DESTROY(set_result);
      DESTROY(ns_result);
    }
    if(set_result)
    {
      BUserSet* uSet = USet(set_result);
      if(!newNameBlock.Fill(uSet->Contens()))
      {
        DESTROY(set_result);
        DESTROY(ns_result);
      }
    }
  //BNameBlock::current_ = oldNameBlock;
    if(!set_result)
    {
      Error(I2("Cannot build NameBlock ",
               "No se puede contruir el NameBlock  ")+name);
      bool assigned = (ns_result && (ns_result->IsAssigned()==1));
      if(assigned) { DESTROY(ns_result); }
    }
    else 
    {
      if(numErr)
      {
        Warning(BText("NameBlock ")+name+" "+
                I2("has been built with",
                   "se ha construido con") + " "+numErr+" "+
                I2("errors.","errores."));
      }
      DESTROY(set_result);
    }
  }
  currentFullName = oldFullName;
  return(ns_result);
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
  bool BNameBlock::Build()
//--------------------------------------------------------------------
{
  int i, j, k;
  BSyntaxObject* obj;

  for(i=j=k=0; i<set_.Card(); i++)
  {
    obj = set_[i+1];
    if(!obj) { continue; }
    const BText& name = obj->Name();
    if(name.HasName() && 
       BParser::DefaultParser()->Filter()->IsIdentifier(name))
    {
      obj->PutNameBlock(this);
      BSyntaxObject* mem = Member(name);
      if(mem && (mem!=obj))
      {
        Warning(I2("Duplicated member name ",
                 "Nombre de miembro duplicado ")+ name+"\n"+
                I2("It will be skiped.","Sera ignorado."));
        continue;
      }
      if(name[0]!='_')
      {
        k++;
        public_[name] = obj;
      //Std(BText("\nBNameBlock::Build added public member ")+Name()+"::"+name);
      }
      else
      {
        if(name[1]=='.') 
        { 
          //read only member
          k++; 
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
          if(!opr->System() && !opr->NameBlock())
          {
            opr->PutNameBlock(this);
          }
        }
      }
    }
    else
    {
      Error(I2("A NameBlock cannot have members without a valid name ",
               "Un NameBlock no puede tener miembros sin un nombre válido ")
            +"'"+name+"'");
      return(false);
    }
  }
  if(k==0)
  {
    Error(I2("A NameBlock must have at least a public or read only member.",
             "Un NameBlock ha de tener al menos un miembro público o de sólo lectura."));
    return(false);
  }
  return(true);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::PublicMember(const BText& memberName) const
//--------------------------------------------------------------------
{
  BObjByNameHash::const_iterator found = public_.find(memberName);
  if(found==public_.end())
  {
    return(NULL);
  }
  else
  {
    return(found->second);
  }
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::PrivateMember(const BText& memberName) const
//--------------------------------------------------------------------
{
  BObjByNameHash::const_iterator found = private_.find(memberName);
  if(found==private_.end())
  {
    return(NULL);
  }
  else
  {
    return(found->second);
  }
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
  bool BNameBlock::Using(const BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  assert(obj && (obj->Grammar()==GraNameBlock()));
  BUserNameBlock* uns = UNameBlock((BSyntaxObject*)obj);
  BText name = uns->Name();
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
  BNameBlock& ns = uns->Contens();
  BObjByNameHash& pbm = ns.Public();
  BObjByNameHash::const_iterator iter;
  BObjByNameHash::const_iterator found;
  BObjByClassNameHash::iterator fc;
  for(iter=pbm.begin(); iter!=pbm.end(); iter++)
  {
    found = usingSymbols_.find(iter->first);
    if(found==usingSymbols_.end())
    {
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
  BSyntaxObject* BNameBlock::LocalMember(const BText& memberName)
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NULL;
  const BNameBlock* cns = BNameBlock::Current();
  if(cns)
  {
    result = cns->Member(memberName);
    if(result && (result->Mode()==BSTRUCTMODE))
    {
      BStruct* bstr = (BStruct*)result;
      result = bstr->Function();
    }
  }
  return(result);
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::UsingMember(const BText& memberName)
//--------------------------------------------------------------------
{
  BObjByNameHash::const_iterator found = usingSymbols_.find(memberName);
  if(found==usingSymbols_.end())
  {
    return(NULL);
  }
  else
  {
    return(found->second);
  }
}

//--------------------------------------------------------------------
  BSyntaxObject* BNameBlock::UsingMember(const BObjClassify&  oc, 
                                         const BText&    memberName)
//--------------------------------------------------------------------
{
  BObjByClassNameHash::iterator fc = usingSymbolsByClass_.find(oc);
  if(fc==usingSymbolsByClass_.end()) { return(NULL); }
  BObjByNameHash::const_iterator found = fc->second->find(memberName);
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
  void BNameBlock::RebuildFullNameDeep(BText parentFullName)
//--------------------------------------------------------------------
{
//Std(BText("\n")+"BNameBlock::SelectMembersDeep Exploring NameBlock "+Name());
  BSyntaxObject* obj;
  BUserNameBlock* unb;
  BObjByNameHash::const_iterator iter;
  if(parentFullName.HasName()) 
  { 
    parentFullName = parentFullName+"::"+LocalName(); 
  }
  else
  {
    parentFullName = LocalName(); 
  }
  PutName(parentFullName);
  for(iter=public_.begin(); iter!=public_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      unb = (BUserNameBlock*)obj;
      unb->Contens().RebuildFullNameDeep(parentFullName);
    }
  }
  for(iter=private_.begin(); iter!=private_.end(); iter++)
  {
    obj = iter->second;
    if((obj->Grammar()==GraNameBlock()) && (obj->Mode()==BOBJECTMODE))
    {
      unb = (BUserNameBlock*)obj;
      unb->Contens().RebuildFullNameDeep(parentFullName);
    }
  }
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
    int i, n = c.Card();
    for(i=1; i<n; i++)
    {
      if(c[i])
      {
        c[i]->Do();
      }
    }
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
  BSystemNameBlock* unknown_ = new BSystemNameBlock
  (
    "UNKNOWN", 
    BNameBlock::Unknown(), 
    I2("Unknown NameBlock", "NameBlock desconocido.")
  );
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
    contens_.RebuildFullNameDeep(currentFullName);
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
    uns_ = UNameBlock(Arg(1));
    contens_ = BNameBlock::Using(uns_);
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
  DefExtOpr(1, BDatUsingNameBlock, "UsingNameBlock",1,1,"NameBlock",
  "(NameBlock nameBlock)",
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
      "back an error if it is tried"
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
     "error si se intenta."),
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


