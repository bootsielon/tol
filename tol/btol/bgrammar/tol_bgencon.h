/* tol_bgencon.h: Template classes for generate grammatical classes
                  GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BGENCON_H
#define TOL_BGENCON_H 1

#include <tol/tol_bgentmp.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------

BBool InitGrammars ();


//--------------------------------------------------------------------
/*! Base class to generate user's objects of class Any. The class Any
 *  must be a non virtual class and a comparision method must have 
 *  been defined.
 */
template <class Any>
class TOL_API BGraContensBase: public BSyntaxObject
//--------------------------------------------------------------------
{
protected:
  static BGrammar* ownGrammar_;
public:
  // Constructors & destructors: inline
  BGraContensBase() : BSyntaxObject() 
  {
    TRACE_MEMORY_SHOW(this,"BGraContensBase::BGraContensBase");
    ownGrammar_->AddObject(this);
  }
  BGraContensBase(const BText& name)
  : BSyntaxObject(name)
  {
    TRACE_MEMORY_SHOW(this,"BGraContensBase::BGraContensBase");
    ownGrammar_->AddObject(this);
  }
  BGraContensBase(const BText& name, const BText& desc)
  : BSyntaxObject(name, desc)
  {
    TRACE_MEMORY_SHOW(this,"BGraContensBase::BGraContensBase");
    ownGrammar_->AddObject(this);
  }
 ~BGraContensBase()
  {
    TRACE_MEMORY_SHOW(this,"BGraContensBase::~BGraContensBase");
    ownGrammar_->DelObject(this);
  }
    
  // Access & manipulation: inline
  BInt Mode() const { return(BOBJECTMODE); }
  BGrammar* Grammar      () const 
  {
#     if defined(INIT_NULL_GRAMMAR)
  return OwnGrammar();
#     else
  return ownGrammar_;
#     endif
    }
    static  BGrammar*   OwnGrammar   () {
#     if defined(INIT_NULL_GRAMMAR)
  if ( !ownGrammar_ )
      InitGrammars();
#     endif
  return ownGrammar_;
  }

  BText Dump() const
  {
    BGraContensBase<Any>* nc = (BGraContensBase<Any>*)this;
    nc->Do();
    return(nc->Contens().Dump());
  }
    
  BText Info() const
  {
    BGraContensBase<Any>* nc = (BGraContensBase<Any>*)this;
    nc->Do();
    return(nc->Contens().Info());
  }
  
  void ReCalc() { flags_.calculated_ = BFALSE; CalcContens(); }
  void Do();
    
  static  BDat   Compare  (const BSyntaxObject* obj1,
                           const BSyntaxObject* obj2)
  {
    Any& any1 = ((BGraContensBase<Any>*)obj1)->Contens();
    Any& any2 = ((BGraContensBase<Any>*)obj2)->Contens();
    return (Any::Compare(&any1, &any2));
  }
    
  static BSyntaxObject* Casting(BSyntaxObject*);
  static BSyntaxObject* FindConstant(const BText& name);
  static BSyntaxObject* New(FILE* fil);
  static void  InitInstances();
  static BBool InitGrammar(int precedence, const BText& nam, const BText& des,
                           BCreator newRenamed,
                           BCreator newFunArg)
  {
    if(!ownGrammar_)
    {
      ownGrammar_=new BGrammar
      (
        precedence,
        nam,
        des,
        true,
        Casting,
        FindConstant,
        newRenamed,
        newFunArg, 
        Compare
      );
      InitInstances();
    }
    return(BTRUE);
  }
  virtual void PutContens(const Any& con) = 0;
  virtual void CalcContens() {}
  virtual Any& Contens() = 0;
  virtual BSyntaxObject* CopyContens() = 0;
  int BinWrite(FILE* file) const 
  { 
    return ((BGraContensBase<Any>*)this)->Contens().BinWrite(file); 
  }
};

//--------------------------------------------------------------------
/*! This class provides contens storement
 */
template <class Any>
class TOL_API BGraContens: public BGraContensBase<Any>
//--------------------------------------------------------------------
{
protected:
  Any contens_;
public:
  BGraContens() : BGraContensBase<Any>(), contens_() 
  { 
    TRACE_MEMORY_SHOW(this,"BGraContens::BGraContens");
  }
  BGraContens(const Any& any) 
  : BGraContensBase<Any>(), contens_(any)   
  {
    TRACE_MEMORY_SHOW(this,"BGraContens::BGraContens");
  }
  BGraContens(const BText& name, const Any& any)
  : BGraContensBase<Any>(name), contens_(any)   
  {
    TRACE_MEMORY_SHOW(this,"BGraContens::BGraContens");
  }
  BGraContens(const BText& name, const Any& any, const BText& desc)
  : BGraContensBase<Any>(name,desc), contens_(any)
  {
    TRACE_MEMORY_SHOW(this,"BGraContens::BGraContens");
  }
 ~BGraContens() 
  {
    TRACE_MEMORY_SHOW(this,"BGraContens::~BGraContens");
  }

 void PutContens(const Any& con)
     { this->flags_.calculated_=BTRUE; contens_ = con; }

  Any& Contens()
  {
      if(!this->flags_.calculated_) { 
    this->CalcContens(); 
    this->flags_.calculated_=BTRUE; 
      }
      return(contens_);
  }
  BSyntaxObject* CopyContens()
  {
    BGraContens<Any>* copy_ = new BGraContens<Any>;
    copy_->PutContens(Contens());
    return(copy_);
  }
  static BSyntaxObject* New(FILE* fil);

  static BGraContens<Any>* New(const BText& name,  const Any& aux, const BText& desc)
  {
    if(name.HasName())
    {
      BGraContens<Any>* obj = new BGraContens<Any>("", aux);
      obj->PutName(name);
      BGrammar::AddObject(obj);
      return(obj);
    }
    else
    {
      return(new BGraContens<Any>(aux));
    }
  }
  static BGraContens<Any>* New(const BText& name,  const Any& aux)
  {
    if(name.HasName())
    {
      BGraContens<Any>* obj = new BGraContens<Any>("", aux);
      obj->PutName(name);
      BGrammar::AddObject(obj);
      return(obj);
    }
    else
    {
      return(new BGraContens<Any>(aux));
    }
  }
  static BGraContens<Any>* New(const Any& aux)
  {
    return(new BGraContens<Any>(aux));
  }
  DeclareClassNewDelete(BGraContens<Any>);
};

//--------------------------------------------------------------------
/*! This class provides contens storement
 */
template <class Any>
class TOL_API BGraContensP: public BGraContensBase<Any>
//--------------------------------------------------------------------
{
protected:
  Any* contens_;
public:
  BGraContensP() : BGraContensBase<Any>(), contens_(NULL) 
  { 
    TRACE_MEMORY_SHOW(this,"BGraContensP::BGraContensP");
  }
  BGraContensP(Any* any) 
  : BGraContensBase<Any>(), contens_(any)   
  {
    TRACE_MEMORY_SHOW(this,"BGraContensP::BGraContensP");
  }
  BGraContensP(const BText& name, Any* any)
  : BGraContensBase<Any>(name), contens_(any)   
  {
    TRACE_MEMORY_SHOW(this,"BGraContensP::BGraContensP");
  }
  BGraContensP(const BText& name, Any* any, const BText& desc)
  : BGraContensBase<Any>(name,desc), contens_(any)
  {
    TRACE_MEMORY_SHOW(this,"BGraContensP::BGraContensP");
  }
 ~BGraContensP() 
  {
    DESTROY(contens_);    
    TRACE_MEMORY_SHOW(this,"BGraContensP::~BGraContensP");
  }

  void PutContens(const Any& con)
  { 
    this->flags_.calculated_=BTRUE; 
    if(!contens_) { contens_ = new Any; }
    *contens_ = con; 
  }

  Any& Contens()
  {
    if(!this->flags_.calculated_) 
    { 
      this->CalcContens(); 
      this->flags_.calculated_=BTRUE; 
    }
    return(*contens_);
  }
  BSyntaxObject* CopyContens()
  {
    BGraContensP<Any>* copy_ = new BGraContensP<Any>;
    copy_->PutContens(Contens());
    return(copy_);
  }
  static BSyntaxObject* New(FILE* fil);

  static BGraContensP<Any>* New(const BText& name,  const Any& aux, const BText& desc)
  {
    if(name.HasName())
    {
      BGraContensP<Any>* obj = new BGraContensP<Any>("", aux);
      obj->PutName(name);
      BGrammar::AddObject(obj);
      return(obj);
    }
    else
    {
      return(new BGraContensP<Any>(aux));
    }
  }
  static BGraContensP<Any>* New(const BText& name,  const Any& aux)
  {
    if(name.HasName())
    {
      BGraContensP<Any>* obj = new BGraContensP<Any>("", aux);
      obj->PutName(name);
      BGrammar::AddObject(obj);
      return(obj);
    }
    else
    {
      return(new BGraContensP<Any>(aux));
    }
  }
  static BGraContensP<Any>* New(const Any& aux)
  {
    return(new BGraContensP<Any>(aux));
  }
  DeclareClassNewDelete(BGraContensP<Any>);
};


//--------------------------------------------------------------------
/*! This class provides contens storement and Fixed Name and
 *  Description (FND). This is usefull to returned objects by
 *  built-in functions saving a lot of memory
 */
template <class Any, char* Name_, char* Description_>
class TOL_API BGraContensFND: public BGraContens<Any>
//--------------------------------------------------------------------
{
public:
  BGraContensFND() 
  : BGraContens<Any>() 
  { 
    TRACE_MEMORY_SHOW(this,"BGraContensFND::BGraContensFND");
    this->flags_.calculated_ = true; 
    this->PutName(Name_);
    this->PutDescription(Description_);
  }

  BGraContensFND(const Any& any) 
  : BGraContens<Any>(any) 
  {  
    TRACE_MEMORY_SHOW(this,"BGraContensFND::BGraContensFND");
    this->flags_.calculated_ = true; 
    this->PutName(Name_);
    this->PutDescription(Description_);
  }

 ~BGraContensFND() 
  {
    TRACE_MEMORY_SHOW(this,"BGraContensFND::~BGraContensFND");
  }

  UndeclareClassNewDelete;
};
//Parameters Name_ and Description_ for template BGraContensFND
//must be globally declared as 
//  char [] non static to file 
//These macros help to do it and avoid linker errors

//This macro must be used at .cpp global level to declare constant
//strings Name and Desc to be used by BGraContensFND
#define PrefixFNDParam1(PREFIX_,NAME)              \
  char _##PREFIX_##NAME##_FND_Name [] = #NAME;   \
  char _##PREFIX_##NAME##_FND_Desc [] = #NAME;

//This macro must be used at .cpp global level to declare constant
//strings Name and Desc to be used by BGraContensFND
#define PrefixFNDParam(PREFIX_,NAME, DESC)         \
  char _##PREFIX_##NAME##_FND_Name [] = #NAME;   \
  char _##PREFIX_##NAME##_FND_Desc [] =  DESC;

//This macro creates a BGraContensFND object using constant strings
//declared with FNDParam1 or FNDParam
#define PrefixNewFND(TYPE, PREFIX_, NAME, OBJ) \
  new BGraContensFND<TYPE, _##PREFIX_##NAME##_FND_Name, _##PREFIX_##NAME##_FND_Desc>(OBJ)


//--------------------------------------------------------------------
/*! Base class to generate system's constant objects of class Any.
 *  Provides contens storement
 *  Constant objects cannot be modified.
 */
template <class Any>
class TOL_API BGraConstant: public BGraContens<Any>
//--------------------------------------------------------------------
{
public:
    BGraConstant() : BGraContens<Any>()
  {
    TRACE_MEMORY_SHOW(this,"BGraConstant::BGraConstant");
    BGraContens<Any>::flags_.calculated_ = BTRUE;
    BGraContens<Any>::flags_.isConst_    = BTRUE;
  }
    BGraConstant(const BText& name,
                 const Any&   any,
                 const BText& desc)
  : BGraContens<Any>(name, any, desc)
  {
    TRACE_MEMORY_SHOW(this,"BGraConstant::BGraConstant");
    BGraContens<Any>::flags_.calculated_ = BTRUE;
    BGraContens<Any>::flags_.isConst_    = BTRUE;
  }
 ~BGraConstant() 
  { 
    TRACE_MEMORY_SHOW(this,"BGraConstant::~BGraConstant");
  }
  Any& Contens() { return(this->contens_); }

private:
  void ErrCannotModify(const BText& member) 
  { 
    Error(I2("Cannot modify <","No se puede modificar <") + member +
          I2("> of constant object ","> del objeto constante ") + BSyntaxObject::Identify()); 
  }
public:
  void PutName(const BChar*   name) 
  {
    if(BSyntaxObject::Name().HasName()) { ErrCannotModify("name"); }
  }
  void PutName(const BText& name) 
  {
    if(BSyntaxObject::Name().HasName()) { ErrCannotModify("name"); }
  }
  void PutDescription(const BText& desc)
  {
    if(BSyntaxObject::Name().HasName()) { ErrCannotModify("description"); }
  }
  void PutContens(const Any& con)
  {
    if(BSyntaxObject::Name().HasName()) { ErrCannotModify("value"); }
  }
  RedeclareClassNewDelete(BGraConstant<Any>);
};


//--------------------------------------------------------------------
/*! Base class to generate parameters objects of class Any.
 *  Parameters are built-in C++ global variables referencing an
 *  internal C++ variable that can be so modified by user.
 *  This class doesn't provide contens store because contens is
 *  referenced.
 */
template <class Any>
class TOL_API BGraParameter: public BGraContensBase<Any>
//--------------------------------------------------------------------
{
 private:
    Any* value_;
 public:
    // Constructors and destructors: inline
  BGraParameter(const BText& name,
                 Any  & value,
                const BText& desc)
  : BGraContensBase<Any>(name, desc), value_(&value)
  {
    TRACE_MEMORY_SHOW(this,"BGraParameter::BGraParameter");
  }
 ~BGraParameter()
  {
    TRACE_MEMORY_SHOW(this,"BGraParameter::~BGraParameter");
  }
    
    void CalcContens() {}
    Any& Contens() { return(*value_); }
    void PutContens(const Any& val) { (*value_) = val; }
    BSyntaxObject* CopyContens()
    {
      BGraContens<Any>* copy_ = new BGraContens<Any>;
      copy_->PutContens(Contens());
      return(copy_);
    }
  DeclareClassNewDelete(BGraParameter<Any>);
};

//--------------------------------------------------------------------
/*! Base class to generate user's renamed objects of class Any.
 *  This class doesn't provide contens store because contens is
 *  referenced.
 */
template<class Any>
class BCopyContens: public BGraContens<Any>
//--------------------------------------------------------------------
{
private:
  BSyntaxObject* result_;
public:
  // Constructors and destructors: inline
  BCopyContens(const BText& name, BSyntaxObject* r)
  : BGraContens<Any> (name, ((BGraContensBase<Any>*)r)->Contens()),
    result_(NULL)
  {
    TRACE_MEMORY_SHOW(this,"BCopyContens::BCopyContens");
    //Try to destroy unusefull object
    if(r->NRefs())
    {
      result_ = r;
      result_->IncNRefs();
    }
    else
    {
      DESTROY(r);
    }
  }
 ~BCopyContens() 
  { 
    TRACE_MEMORY_SHOW(this,"BCopyContens::~BCopyContens");
    if(result_) 
    { 
      result_->DecNRefs(); 
      DESTROY(result_); 
    }
  }
    
  // Implementation: tsrgra.cpp
    
  void ReCalc() { }
  static BSyntaxObject* New(const BText& name, BSyntaxObject* d) 
  { 
    return((BSyntaxObject*)new BCopyContens<Any>(name, d)); 
  }
  DeclareClassNewDelete(BCopyContens<Any>);
};


//--------------------------------------------------------------------
template<class BRefTemplate, class Any>
class BReferenceContens : public BRefTemplate, public Any
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BReferenceContens(const BText& name, BSyntaxObject* d)
  : BRefTemplate(name, d) 
  {
    TRACE_MEMORY_SHOW(this,"BReferenceContens::BReferenceContens");
  }

    void ReCalc     ()               { if(BRefTemplate::GetResult()) { BRefTemplate::GetResult()->ReCalc(); } }
    void CalcContens()               { if(BRefTemplate::GetResult()) { BRefTemplate::GetResult()->CalcContens(); } }
    void PutContens (const Any& con) { if(BRefTemplate::GetResult()) { BRefTemplate::GetResult()->PutContens(con); }  }
    Any& Contens    ()               
    { 
      if(BRefTemplate::GetResult()) 
      { 
        return(BRefTemplate::GetResult()->Contens()); 
      }
      else            
      { 
        Any& th = ((Any&)*this);
        th = Any::Unknown();
        return(th); 
      }
    }
 ~BReferenceContens() 
  {
    TRACE_MEMORY_SHOW(this,"BReferenceContens::~BReferenceContens");
  }
    
    BSyntaxObject* CopyContens()
    {
      BGraContens<Any>* copy_ = new BGraContens<Any>;
      copy_->PutContens(Contens());
      return(copy_);
    }
};

//--------------------------------------------------------------------
/*! Base class to generate user's renamed objects of class Any.
 *  This class doesn't provide contens store because contens is
 *  referenced.
 *  This class provides Optional Information
 */
template<class Any>
class BRenContens: public BReferenceContens< BRefObject< BGraContensBase<Any> >, Any>
//--------------------------------------------------------------------
{
public:
  // Constructors and destructors: inline
  BRenContens(const BText& name, BSyntaxObject* r)
  : BReferenceContens< BRefObject< BGraContensBase<Any> >, Any>(name,r)
  {
    TRACE_MEMORY_SHOW(this,"BRenContens::BRenContens");
  }
 ~BRenContens() 
  {
    TRACE_MEMORY_SHOW(this,"BRenContens::~BRenContens");
  }
  static BSyntaxObject* New(const BText& name, BSyntaxObject* d) 
  { 
    if(d && d->System())
    {
      return((BSyntaxObject*)new BCopyContens<Any>(name, d)); 
    }
    else
    {
    return((BSyntaxObject*)new BRenContens<Any>(name, d)); 
    }
  }
  DeclareClassNewDelete(BRenContens<Any>);
};


//--------------------------------------------------------------------
/*! Base class to generate user's renamed objects of class Any.
 *  This class doesn't provide contens store because contens is
 *  referenced.
 *  This class provides Optional Information
 */
template<class Any>
class BFunArgContens: public BReferenceContens< BFunArgObject< BGraContensBase<Any> >, Any>
//--------------------------------------------------------------------
{
public:
// Constructors and destructors: inline
  BFunArgContens(const BText& localName, BSyntaxObject* r)
  : BReferenceContens< BFunArgObject< BGraContensBase<Any> >, Any>(localName,r)
  {
    TRACE_MEMORY_SHOW(this,"BFunArgContens::BFunArgContens");
  }
 ~BFunArgContens() 
  {
    TRACE_MEMORY_SHOW(this,"BFunArgContens::BFunArgContens");
  }
  static BSyntaxObject* NewLocal(const BText& localName, BSyntaxObject* d) 
  { 
    BFunArgContens<Any>* obj = new BFunArgContens<Any>(localName,d);
    return((BSyntaxObject*)obj); 
  }
  DeclareClassNewDelete(BFunArgContens<Any>);
};


//--------------------------------------------------------------------
/*! Base class to generate temporary objects of class Any.
 *  Temporary objects are the nodes of the tree of each TOL expression
 *  Provides contens store.
 */
template<class Any>
class TOL_API BTmpContens: public BTmpObject< BGraContens<Any> >
//--------------------------------------------------------------------
{
 public:
    // Constructors and destructors: inline
   BTmpContens(BList* arg=NULL) : BTmpObject< BGraContens<Any> >(arg) { }

  ~BTmpContens() { }

  Any& Contens() 
  {
    if(!BGraContens<Any>::flags_.calculated_) 
    { 
      this->CalcContens(); 
      this->flags_.calculated_=BTRUE; /*args_=NIL;*/ 
      ForgetArgs();
    }
    return(this->contens_);
  }
  UndeclareClassNewDelete;
};


//------------------------------------------------------------------
#define DeclareContensClass( ANY , BTMPCLASS, BCONTENSCLASS)       \
class BCONTENSCLASS: public BTMPCLASS           \
{                   \
public:                   \
  BCONTENSCLASS(BList* arg) : BTMPCLASS(arg) {}         \
 ~BCONTENSCLASS()  {}               \
  void CalcContens();               \
  RedeclareClassNewDelete(BCONTENSCLASS); \
};                   \
DeclareEvaluator(BCONTENSCLASS);

//------------------------------------------------------------------
#define DeclareNoLazzyClass( ANY , BTMPCLASS, BCONTENSCLASS)     \
class BCONTENSCLASS: public BTMPCLASS           \
{                   \
public:                   \
  BCONTENSCLASS(BList* arg) : BTMPCLASS(arg) {}         \
 ~BCONTENSCLASS()  {}               \
  void CalcContens();               \
};                   \
DeclareNoLazzyEvaluator(BCONTENSCLASS);

//------------------------------------------------------------------
#define DefineContensCommonOperators(ANY, NAME)         \
ANY& ANY##UnKnown()               \
{                   \
  static ANY ANY##unKnown_;             \
  return(ANY##unKnown_);             \
}

//------------------------------------------------------------------
#define CastingsDeclaration(ANY)           \
                   \
inline BUser##ANY* U##ANY(BSyntaxObject* arg)         \
{                   \
  BUser##ANY* u##ANY = NIL;             \
  if(arg && (arg->Grammar()==BUser##ANY::OwnGrammar()))       \
  { u##ANY = BCast<BUser##ANY,BSyntaxObject>::c(arg); }           \
  return(u##ANY);               \
}                   \
                   \
inline BUser##ANY* U##ANY(BCore* arg)           \
{                   \
  return(U##ANY(BCast<BSyntaxObject,BCore>::c(arg)));           \
}                   \
                   \
inline BUser##ANY* U##ANY(const BText& expression)       \
{                   \
  return(U##ANY(BUser##ANY::OwnGrammar()->EvaluateExpr(expression)));  \
}                   \
                   \
TOL_API B##ANY& B##ANY##UnKnown();           \
inline B##ANY& ANY (BSyntaxObject* arg)           \
{                   \
  if(arg && (arg->Grammar()==BUser##ANY::OwnGrammar()))       \
  {                   \
    BUser##ANY*   u##ANY = U##ANY(arg);           \
    return(u##ANY->Contens());             \
  }                   \
  else                   \
  {                   \
      return(B##ANY##UnKnown());           \
  }                   \
}                   \
                   \
inline B##ANY& ANY (BCore* arg)             \
{                   \
  return(ANY(U##ANY(arg)));             \
}                   \
                   \
inline B##ANY& ANY (const BText& expr)           \
{                   \
  return(ANY(U##ANY(expr)));             \
}

#endif // TOL_BGENCON_H

