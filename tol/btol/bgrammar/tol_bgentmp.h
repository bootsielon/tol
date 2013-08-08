/* tol_bgentmp.h: Template classes for generate temporary objects.
                  GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BGENTMP_H
#define TOL_BGENTMP_H 1

#include <tol/tol_bset.h>
#include <tol/tol_bgrammar.h>


//--------------------------------------------------------------------
/*! Base class to generate user's renamed objects of class Any.
 */
template<class Any>
class BRefObject: public Any
//--------------------------------------------------------------------
{
private:
  Any* result_;
public:
  // Constructors and destructors: inline
  BRefObject(BSyntaxObject* r)
  : Any(), result_((Any*)r) 
  {
    TRACE_MEMORY_SHOW(this,"BRefObject::BRefObject");
    if(result_) { result_->IncNRefs(); }
  }
  BRefObject(const BText& name, BSyntaxObject* r)
  : Any(name), result_((Any*)r) 
  {
    TRACE_MEMORY_SHOW(this,"BRefObject::BRefObject");
    if(result_) { result_->IncNRefs(); }
  }
 ~BRefObject() 
  { 
    TRACE_MEMORY_SHOW(this,"BRefObject::~BRefObject");
    if(result_) 
    {
      result_->DecNRefs();
      DESTROY(result_);
      result_ = NULL;
    }
  }
  Any* GetResult() const 
  { 
    if(result_->Mode()!=BOBJECTMODE)
    {
      Error("[FATAL] Non expect mode of syntax object.");
      return(NULL);
    }
    return(result_); 
  }
 const BText& Expression  () const 
  { 
    if(GetResult()) { return(result_->Expression()); }
    else            { return(this->CBTextNullRef()); }
  }
  const BText& TolPath     () const
  { 
    if(GetResult()) { return(result_->TolPath()); }
    else            { return(this->CBTextNullRef()); }
  }
  BText Dump() const 
  {
	if(GetResult()) { return(result_->Dump()); }
	else	          { return("NULL"); }
  }
  BText Info() const 
  {
	if(GetResult()) { return(result_->Info()); }
	else	          { return("NULL"); }
  }
  void ReCalc() 
  { 
	if(GetResult()) { result_->ReCalc(); } 
  }
};


//--------------------------------------------------------------------
/*! Base class to generate user's functions arguments
 */
template<class Any>
class BFunArgObject: public BRefObject<Any>
//--------------------------------------------------------------------
{
private: 
  BText localName_;  
public: 
  const BText& LocalName() const                 
  { 
    return(localName_);
  } 
  void PutLocalName (const BText& locNam)  
  { 
    localName_ = locNam; 
  } 
  void PutName (const BText&   name  )
  { 
    this->GetResult()->PutName(name);
  } 

public:
  // Constructors and destructors: inline
  BFunArgObject(const BText& localName, BSyntaxObject* r)
    : BRefObject<Any>(r), localName_(localName)
  {
    TRACE_MEMORY_SHOW(this,"BFunArgObject::BFunArgObject");
  }
 ~BFunArgObject() 
  {
    TRACE_MEMORY_SHOW(this,"BFunArgObject::~BFunArgObject");
  }
    
  const char*  String      () const { return(this->GetResult()->String()); }
  const BText& Name        () const { return(this->GetResult()->Name()); }
  const BText& Description () const { return(this->GetResult()->Description()); }
    
  static BSyntaxObject* NewLocal(const BText& localName, BSyntaxObject* d) 
  { 
    BFunArgObject<Any>* obj = new BFunArgObject<Any>(localName,d);
    return((BSyntaxObject*)obj); 
  }

};


//--------------------------------------------------------------------
/*! Base class to generate temporary objects of class Any.
 */
template<class Any> 
class BTmpObject: public Any
//--------------------------------------------------------------------
{
protected:
  BList* args_;	 //!< List of arguments for evaluate it.
  int    card_;
  BSyntaxObject** array_;
public:
  // Constructors & destructors: inline
  BTmpObject(BList* arg=NULL)
	: Any(), args_(NULL), card_(0), array_(NULL)
	{
    TRACE_MEMORY_SHOW(this,"BTmpObject::BTmpObject");
    args_ = arg;
    card_ = LstLength(args_);
    if(card_) { array_ = new BSyntaxObject*[card_]; }
    int n = 0;
   	for(BList* lst=args_; lst; lst=Cdr(lst))
    {
      array_[n++] = (BSyntaxObject*)lst->Car();
    }
  }

 ~BTmpObject() 
  { 
    TRACE_MEMORY_SHOW(this,"BTmpObject::~BTmpObject");
    if(array_ && args_) 
    { 
      DESTROY(args_); 
      delete[] array_; 
    }
  }

  void ForgetArgs() { 
    if(array_ && args_) 
    { 
      DESTROY(args_); 
      delete[] array_; 
      array_ = NULL;
    }
    card_ = 0;
  }
  // Implementation: inline
  
  BSyntaxObject* Arg(BInt n) const 
  {
    if((n<=0)||(n>card_)) { return(NULL); }
	  return(array_[n-1]); 
  }
  BList* DupArgList() const { return(LstDuplicate(args_)); }
  BList* ArgList() { return(args_); }
  int NumArgs() const { return(card_); }
  void ReCalc () 
  {
   	for(int n=0; n<card_; n++)
	  {
      array_[n]->ReCalc();
	  }
	  Any::ReCalc();
  }
};

#endif // TOL_BGENTMP_H


