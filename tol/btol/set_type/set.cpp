/* set.cpp: Set functions.
            GNU/TOL Language.

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

#include <tol/tol_bset.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bnameblock.h>
#include <tol/tol_bstruct.h>

//--------------------------------------------------------------------
// Static variables.
//--------------------------------------------------------------------
BTraceInit("set.cpp");
BCode	         BSet::order_;
BText	         BSet::orderError_;
BSyntaxObject* BSet::objCond_	= NIL;
BSet	         BSet::unknown_;

//--------------------------------------------------------------------
BSet::BSet()

//! BSet constructor
//--------------------------------------------------------------------
: sourcePath_(NULL), struct_(NIL), subType_(Generic), array_(0),
  indexByName_(NULL),
  nameBlock_(NULL),
  oisLocalDependencies_(NULL)
{
}


//--------------------------------------------------------------------
BSet::BSet(const BSet& set)

/*! BSet constructor
 */
//--------------------------------------------------------------------
: sourcePath_(NULL), struct_(NIL), subType_(Generic), array_(0),
  indexByName_(NULL),
  nameBlock_(NULL),
  oisLocalDependencies_(NULL)
{ 
  Copy(set);
}


//--------------------------------------------------------------------
BSet::BSet(BList* arg, BStruct* str, BSubType sub)

/*! BSet constructor
 */
//--------------------------------------------------------------------
: sourcePath_(NULL), struct_(NIL), subType_(Generic), array_(0),
  indexByName_(NULL),
  nameBlock_(NULL),
  oisLocalDependencies_(NULL)
{ 
  RobStruct(arg,str,sub); 
}


//--------------------------------------------------------------------
BSet::~BSet()

/*! BSet destructor
 */
//--------------------------------------------------------------------
{
  Delete();
  DESTROY(oisLocalDependencies_);
}

//--------------------------------------------------------------------
void BSet::Delete()

/*! BSet destructor
 */
//--------------------------------------------------------------------
{
  int n;
  for(n=array_.Size()-1; n>=0; n--)
  {
    if(array_[n])
    {
      array_[n]->DecNRefs();
      DESTROY(array_[n]);
    }
  }
  array_.DeleteBuffer();
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
}

//--------------------------------------------------------------------
void BSet::Alloc(BInt n)
//--------------------------------------------------------------------
{
  array_.ReallocBuffer(n);
  for(int i=0; i<n; i++) { array_[i]=NULL; }
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
}

//--------------------------------------------------------------------
void BSet::Realloc(BInt n)
//--------------------------------------------------------------------
{
  int size = array_.Size();
  array_.ReallocBuffer(n);
  for(int i=size; i<n; i++) { array_[i]=NULL; }
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
}

//--------------------------------------------------------------------
void BSet::Copy(const BSet& set)
//--------------------------------------------------------------------
{
  int n;
  Delete(); 
  array_.ReallocBuffer(set.Card());
  bool hasIdx = set.HasIndexByName()!=0;
  for(n=0; n<array_.Size(); n++)
  {
	  array_[n] = set[n+1];
    if(array_[n])
    {
      if(hasIdx && !array_[n]->HasName())
      {
        array_[n]->PutName(set[n+1]->Name());
      }
    }
    array_[n]->IncNRefs();
  };
  PutStruct(set.Struct());
  PutSubType(set.SubType());
  sourcePath_ = set.SourcePath();
  nameBlock_  = set.GetNameBlock();
  if(hasIdx)
  {
    SetIndexByName();
  }
  
}

//--------------------------------------------------------------------
BSet& BSet::operator = (const BSet& set)

/*! Asigns the argument to this and returns it self.
 */
//--------------------------------------------------------------------
{
  Copy(set);
  return(*this);
}

//--------------------------------------------------------------------
BList* BSet::ToList() const
//--------------------------------------------------------------------
{
  BList* lst = NIL;
  for(int n=0; n<array_.Size(); n++)
  {
    lst = Cons(array_[n], lst);
  };
  return(lst);
}

//--------------------------------------------------------------------
BList* BSet::ToReversedList() const
//--------------------------------------------------------------------
{
  BList* lst = NIL;
  for(int n=array_.Size()-1; n>=0; n--)
  {
    lst = Cons(array_[n], lst);
  };
  return(lst);
}

//--------------------------------------------------------------------
void BSet::PutElement(BList* element)
//--------------------------------------------------------------------
{
  Delete(); 
  array_.ReallocBuffer(LstLength(element));
  for(int n=0; n<array_.Size(); n++)
  {
	  array_[n] = (BSyntaxObject*)Car(element);
    array_[n]->IncNRefs();
    element = Cdr(element);
  };
}

//--------------------------------------------------------------------
void BSet::RobElement(BList* element)
//--------------------------------------------------------------------
{
  PutElement(element);
  DESTROY(element);
}

//--------------------------------------------------------------------
void BSet::RobStruct (BList* arg, BStruct* str,	BSubType sub)
//--------------------------------------------------------------------
{
  if(arg && str)
  {
    BList* argLst = arg;
    BList* lst	  = NIL;
    BList* aux	  = NIL;
    for(BInt n=0; (n<str->Size()); n++)
    {
//    Std(BText("\nRobStruct Field ")+n+" "+(*str)[n].Name()+" -> ");
      if(argLst)
      {
	      BSyntaxObject* obj = (BSyntaxObject*)Car(argLst);
	      BText name = obj->Name();
	      BText desc = obj->Description();
	      if((*str)[n].Grammar()!=obj->Grammar())
	      {
	        BText id = name;
	        if(!id.HasName()) { id = obj->Dump(); }
	        if(!id.HasName()) { id = desc; }
	        Error(id + I2(" is not an object of type ",
			                  " no es un objeto de tipo ") +
		           (*str)[n].Grammar()->Name()+ ".\n "+
		           I2("Cannot create field ","No se puede crear el campo")+
		              (*str)[n].Name()+
		           I2(" of structure "," de la estructura ")+str->Name());
	      }
        LstFastAppend(lst,aux,obj);
	      argLst = Cdr(argLst);
      }
      else
      {
	      Error(I2("Too few parameters to create a set with structure ",
		             "Faltan argumentos para crear un conjunto con estructura ")+
		    str->Name());
      }
    }
    if(argLst)
    {
	    Error(I2("Too mach parameters to create a set with structure ",
		           "Sobran argumentos para crear un conjunto con estructura ")+
	          str->Name());
    }
    DESTROY(arg);
    RobElement(lst);
    PutStruct (str);
    PutSubType(sub);
  }
  else
  {
    RobElement(arg);
    PutSubType(sub);
  }
}

//--------------------------------------------------------------------
void BSet::PutStruct(BStruct* str)
//--------------------------------------------------------------------
{
  BStruct* str_=BStruct::Alive(str,false);
  if(str_ && str_->Match(*this))
  {
    struct_ = str_;
    PutSubType(BSet::Structured);
  }
  else if(str_)
  {
    Error(I2("Cannot apply structure ",
             "No se puede aplicar la estructura ")+str_->Name());
  }
}


//--------------------------------------------------------------------
void BSet::ChangeElement(BInt n, BSyntaxObject* obj)

/*! Changes the n-th element of a set by another one object
 */
//--------------------------------------------------------------------
{
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
  if((n>=1) && (n<=array_.Size()))
  {
    BAtom* old = array_[n-1];
    array_[n-1] = obj;
    obj->IncNRefs();
    old->DecNRefs();
    DESTROY(old);
  }
  else
  {
	  Error(I2(BText("Attempt to access ")+n+"-th element of a set with "+
   	         array_.Size() + " elements",
	           BText("Intento de acceso al elemento ") + n + "-esimo de un " +
		         "conjunto de " +array_.Size() + " elementos"));
  }
}


//--------------------------------------------------------------------
void BSet::AddElement(BSyntaxObject* syn)
//--------------------------------------------------------------------
{
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
  BInt size = array_.Size();
  if(size==array_.MaxSize()) { Realloc((BInt)((size+1)*1.1)); }
  array_.ReallocBuffer(size+1);
  array_[size]=syn;
  array_[size]->IncNRefs();
}

//--------------------------------------------------------------------
static bool DoIndexElement(BObjByIdxNameHash** indexByName, 
                           BSyntaxObject* obj,
                           int i)
//--------------------------------------------------------------------
{
  bool ok = true;
  if(indexByName && *indexByName && obj)
  {
    const BText& name = obj->Name();
    if(name.HasName())
    {
      BObjByIdxNameHash::const_iterator found = (*indexByName)->find(name);
      if(found!=(*indexByName)->end())
      {
        Error(I2("Duplicated index name ",
                 "Nombre de �ndice duplicado ")+
              name);
        ok = false;
      }
      (**indexByName)[name] = BIndexedSyntaxObject(i,obj);
    }
    else
    {
      Error(I2("A indexed by name set cannot have elements without a valid name ",
               "Un conjunto indexado por nombres no puede tener elementos sin un nombre v�lido ")
            +"'"+name+"'");
      ok = false;
    }
   if(!ok) { delete *indexByName; *indexByName = NULL; }
  }
  return(ok);
}

//--------------------------------------------------------------------
void BSet::Append(const BSet& set, bool incrementalIndex)
//--------------------------------------------------------------------
{
  if(!incrementalIndex && indexByName_) 
  { delete indexByName_; indexByName_ = NULL; } 
  if(incrementalIndex && !array_.Size()) 
  {
    indexByName_ = new BObjByIdxNameHash;
    SetEmptyKey((*indexByName_), NULL);
  }
  int m=Card(), n=set.Card(), i, j=m;
  array_.ReallocBuffer(m+n);
  for(i=1; i<=n; i++, j++)
  {
    array_[j]=set[i];
    array_[j]->IncNRefs();
    DoIndexElement(&indexByName_,array_[j],j);
  }
}

//--------------------------------------------------------------------
void BSet::Remove(int n, bool deleteOld)
//--------------------------------------------------------------------
{
  int s = array_.Size();
  if((n<0)||(n>=s)) { return; }
  BSyntaxObject* found = array_[n];
  int i;
  for(i=n; i<s-1; i++)
  {
    array_[i] = array_[i+1];
  }
  array_.ReallocBuffer(s-1);
  found->DecNRefs();
  if(deleteOld) { DESTROY(found); }
}

//--------------------------------------------------------------------
void BSet::Replace(int n, BSyntaxObject* newObject, bool deleteOld)
//--------------------------------------------------------------------
{
  int s = array_.Size();
  if((n<0)||(n>=s)) { return; }
  BSyntaxObject* found = array_[n];
  newObject->IncNRefs();
  array_[n] = newObject;
  found->DecNRefs();
  if(deleteOld) { DESTROY(found); }
}

//--------------------------------------------------------------------
BSyntaxObject* BSet::operator[] (BInt n)  const

/*! Returns the n-th element
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  if((n>=1) && (n<=array_.Size()))
  {
	  result = array_[n-1];
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BSet::GetElement(BInt n)  const

/*! Returns the n-th element
 */
//--------------------------------------------------------------------
{
  if(nameBlock_ && nameBlock_->EnsureIsAssigned())
  {
    Error(I2("Cannot access to NameBlock members with operator ",
             "No se puede acceder a los miembros de un NameBlock con el operador ")+
             "[...]\n"+
             nameBlock_->Name()+"["+n+"]");
    return NIL;
  }
  BSyntaxObject* result = operator[](n);
  if(!result)
  {
	  Error(I2(BText("Attempt to access ")+n+"-th element of a set with "+
   	         array_.Size() + " elements",
	           BText("Intento de acceso al elemento ") + n + "-esimo de un " +
		         "conjunto de " +array_.Size() + " elementos"));
  }
  return(result);
}

//--------------------------------------------------------------------
  BBool BSet::SetIndexByName()
//--------------------------------------------------------------------
{
  if(indexByName_) { delete indexByName_; indexByName_ = NULL; }
  indexByName_ = new BObjByIdxNameHash;
  SetEmptyKey((*indexByName_), NULL);

  int i;
  if(!array_.Size())
  {
    Error(I2("Cannot index by name an empty set.",
             "No se puede indexar por nombre un conjunto vac�o."));
    return(false);
  }
  for(i=0; i<array_.Size(); i++)
  {
    assert(array_[i]);
    if(!DoIndexElement(&indexByName_,array_[i],i)) { return(false); }
  }
  return(true);
}

//--------------------------------------------------------------------
  int BSet::FindIndexByName(const BText& name) const
//--------------------------------------------------------------------
{
  int indexPos = 0;
  if(indexByName_) 
  { 
    BObjByIdxNameHash::const_iterator found = indexByName_->find(name);
    if(found!=indexByName_->end())
    {
      indexPos = 1 + found->second.position_;
    }
  }
  else
  {
    for(int n=0; !indexPos && (n<array_.Size()); n++)
    {
      BSyntaxObject* obj = array_[n];
      if (obj->Name() == name) { indexPos = n+1; }
    };
  }
  return(indexPos);
}  

//--------------------------------------------------------------------
  BBool BSet::HasIndexByName() const
//--------------------------------------------------------------------
{
  return(indexByName_!=NULL);
}  

//--------------------------------------------------------------------
BSyntaxObject* BSet::operator[] (const char * name) const
  
/*! Returns the the first element whose name is matched by 'name'
 */
//--------------------------------------------------------------------
{
  if(indexByName_) 
  { 
    BObjByIdxNameHash::const_iterator found = indexByName_->find(name);
    if(found==indexByName_->end())
    {
      return(NULL);
    }
    else
    {
      return(found->second.object_);
    }
  }
  else
  {
    for(int n=0; n<array_.Size(); n++)
    {
      BSyntaxObject* obj = array_[n];
      if (obj->Name() == name) { return obj; }
    };
  }
  return NIL;
}

//--------------------------------------------------------------------
BSyntaxObject* BSet::GetElement(const char * name) const
  
/*! Returns the the first element whose name is matched by 'name'
 */
//--------------------------------------------------------------------
{
  if(nameBlock_ && nameBlock_->EnsureIsAssigned())
  {
    Error(I2("Cannot access to NameBlock members with operator ",
             "No se puede acceder a los miembros de un NameBlock con el operador ")+
             "[\"...\"]\n"+
             nameBlock_->Name()+"[\""+name+"\"]");
    return NIL;
  }
  BSyntaxObject* result = operator[](name);
  if(!result)
  {
    Error(I2(BText("No element found with name ") + name,
             BText("No se ha encontrado elemento con nombre ") + name));
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BSet::Field(const BText& fieldName) const

/*! Returns the element whose field name is the given text.
 */
//--------------------------------------------------------------------
{
  BSyntaxObject* result = NIL;
  if(Struct(true))
  {
	  BInt     numField = struct_->FindField(fieldName);
	  if(numField<0)
    {
	    Error(fieldName+
		  I2(" is not a field of structure ",
		     " no es campo de la estructura ")+
		  struct_->Name());
	  }
	  else
	  {
	    result = array_[numField];
	  }
  }
  else if(nameBlock_ && nameBlock_->EnsureIsAssigned())
  {
    Error(I2("Cannot access to NameBlock members with ",
             "No se puede acceder a los miembros de un NameBlock con ")+"->"+fieldName);
    return NIL;
  }
  else
  {
	  Error(I2("Attempt to access to field ",
		         "Intento de acceso al campo ") + fieldName +
	        I2(" for an unstructured set",
		         " para un conjunto sin estrutura"));
  }
  return(result);
}

//--------------------------------------------------------------------
static void AddName(BText& txt, BSyntaxObject* syn)
//--------------------------------------------------------------------
{ 
  if(syn) { txt += syn->Dump(); } 
}


//--------------------------------------------------------------------
BText BSet::ToText(const BText& separator) const

/*! Returns its elements separated by ','
 */
//--------------------------------------------------------------------
{
  BText txt;
  BText end;
  if(nameBlock_&& nameBlock_->EnsureIsAssigned()) 
  { 
    txt = "NameBlock [["; end = "]]"; 
  } else 
  if(Struct()) { txt = struct_->Name() + "("; end = ")"; }  
  else { txt = "[[ "; end = "]]"; }
  txt += separator;
  if(array_.Size())
  {
    BInt n;
	  for(n=0; n<Card(); n++)
	  {
	    AddName(txt, array_[n]);
	    if(n!=Card()-1)
	    {
		    txt += ",";
		    txt += separator;
	    }
	  }
  }
  txt += end;
  return(txt);
}


//--------------------------------------------------------------------
BText BSet::Name() const

/*! Returns its elements separated by ','
 */
//--------------------------------------------------------------------
{
  BText separator;
  if((SubType()==BDTFile)||
     (SubType()==BSTFile)||
     (SubType()==BMTFile)  ) 
  { separator = "\n"; }
  else		       
  { separator = " " ; }
  return(ToText(separator));
}

//--------------------------------------------------------------------
BText BSet::Dump() const

/*! Returns its foundamental contens information
 */
//--------------------------------------------------------------------
{
  return(ToText(" "));
}


//--------------------------------------------------------------------
BText BSet::Info() const

/*! Returns its foundamental contens information
 */
//--------------------------------------------------------------------
{
  BInt n = Card();
  return(BText("")+n+I2(" elements"," elementos"));
}


//--------------------------------------------------------------------
BBool BSet::Includes(BSyntaxObject* syn) const

/*! Returns BTRUE if syn belongs to this set
 */
//--------------------------------------------------------------------
{
  BBool	 includes = BFALSE;
  for(BInt n=1; (n<=Card()) && !includes; n++)
  { includes = !syn->Grammar()->Compare(syn, (*this)[n]); }
  return(includes);
}


//--------------------------------------------------------------------
BBool BSet::operator == (const BSet& set) const

/*! Returns BTRUE if both set are equal
 */
//--------------------------------------------------------------------
{ return((Card() == set.Card()) && (*this <= set) && (set <= *this)); }


//--------------------------------------------------------------------
BBool BSet::operator != (const BSet& set) const
//--------------------------------------------------------------------
{ return(!(*this == set)); }


//--------------------------------------------------------------------
BBool BSet::operator < (const BSet& set) const

/*! Returns BTRUE if the first set is strictly included in the second
 *  set
 */
//--------------------------------------------------------------------
{
  BBool	 includes = (Card() < set.Card());
  for(BInt n=1; (n<=Card()) && includes; n++)
  { 
    includes = set.Includes((*this)[n]); 
  }
  return(includes);
}


//--------------------------------------------------------------------
BBool BSet::operator > (const BSet& set) const

/*! Returns BTRUE if the second set is strictly included in the first
 *  set
 */
//--------------------------------------------------------------------
{ 
  return(set < *this); 
}


//--------------------------------------------------------------------
BBool BSet::operator <= (const BSet& set) const

/*! Returns BTRUE if the first set is included in the second set
 */
//--------------------------------------------------------------------
{
  BBool	 includes = (Card() <= set.Card());
  for(BInt n=1; (n<=Card()) && includes; n++)
  { 
    includes = set.Includes((*this)[n]); 
  }
  return(includes);
}


//--------------------------------------------------------------------
BBool BSet::operator >= (const BSet& set) const

/*! Returns BTRUE if the second set is included in the first set
 */
//--------------------------------------------------------------------
{ 
  return(set <= *this); 
}


//--------------------------------------------------------------------
BSet BSet::Unique() const

/*! Returns a new set with all different elements.
 */
//--------------------------------------------------------------------
{
  BSet res;
  for(BInt n=1; n<=Card(); n++)
  { 
    if(!res.Includes((*this)[n])) 
    { 
      res.AddElement((*this)[n]); 
    } 
  }
  return(res);
}


//--------------------------------------------------------------------
BSet BSet::operator << (const BSet& set) const

/*! Returns a new set with all elements in the first or second set.
 */
//--------------------------------------------------------------------
{
  BSet res  = (*this);
//res.PutStruct(NULL);
  res.struct_ = NULL;
  for(BInt n=1; n<=set.Card(); n++)
  { 
    res.AddElement(set[n]); 
  }
  return(res);
}


//--------------------------------------------------------------------
BSet BSet::operator + (const BSet& set) const
       
/*! Returns a new set with all elements in the first or second set.
 */
//--------------------------------------------------------------------
{
  int n;
  BSet res;
  res.PrepareStore(Card()+set.Card());
  for(n=0; n<array_.Size(); n++)
  { 
    res.AddElement(array_[n]); 
  }
  for(n=1; n<=set.Card(); n++)
  { 
    if(!Includes(set[n])) 
    { 
      res.AddElement(set[n]); 
    } 
  }
  return(res);
}


//--------------------------------------------------------------------
BSet BSet::operator * (const BSet& set) const

/*! Returns a new set with all elements in the first and second set.
 */
//--------------------------------------------------------------------
{
  BSet res;
  for(BInt n=1; n<=Card(); n++)
  {
    if(set.Includes((*this)[n]) && (!res.Includes((*this)[n])))
    {
      res.AddElement((*this)[n]);
    }
  }
  return(res);
}


//--------------------------------------------------------------------
BSet BSet::operator - (const BSet& set) const

/*! Returns a new set with all elements in the first but the second set.
 */
//--------------------------------------------------------------------
{
  BSet res;
  for(BInt n=1; n<=Card(); n++)
  { 
    if(!set.Includes((*this)[n])) 
    { 
      res.AddElement((*this)[n]); 
    } 
  }
  return(res);
}


//--------------------------------------------------------------------
// Static variables for sorting methods
//--------------------------------------------------------------------
static BInt numCompare = 0;

//--------------------------------------------------------------------
BInt BSet::CompareElements(int n0, int n1)

/*! Compares between two objects of the set
 */
//--------------------------------------------------------------------
{
  return(BSet::CompareObjects(&array_[n0-1], &array_[n1-1]));
}

//--------------------------------------------------------------------
BInt BSet::CompareObjects(const BAny any0, const BAny any1)

/*! Compares between two syntax objects
 */
//--------------------------------------------------------------------
{
  BInt valid = 0;
//numCompare++;
//Std(".");
//if(!(numCompare%100)) { Std(Out() + "\nCompare"); }
  BSyntaxObject** syn00 = (BSyntaxObject**)(any0);
  BSyntaxObject** syn10 = (BSyntaxObject**)(any1);
  if(syn00 && syn10)
  {
	  BSyntaxObject* syn0 = *syn00;
	  BSyntaxObject* syn1 = *syn10;
	  if(syn0 && syn1)
	  {
	    BGrammar* gra = order_.Operator()->GrammarForArg(1);
	    if(!orderError_.HasName())
	    {
		    if((gra!=GraAnything()) &&
		       ((syn0->Grammar()!=gra) || (syn1->Grammar()!=gra)))
		    {
		      orderError_ = I2("They do not coincide the types of the "
				                   "elements to order with the order "
				                   "criterion ",
				                   "No coinciden los tipos de los "
				                   "elementos a ordenar con el criterio "
				                   "de orden ")+order_.Name();
		      Error(orderError_);
        }
		    else
		    {
		      BList* args = Cons(syn0,NCons(syn1));
		      objCond_  = order_.Evaluator(args);
		      if(!objCond_)
		      {
			      orderError_ = I2("Wrong order criterium "
					                   +order_.Name(),
					                   "Criterio de orden "
					                   +order_.Name()+" mal definido");
			      Error(orderError_);
		      }
		      else
		      {
			      BUserDat* uDat = (BUserDat*)objCond_;
			      uDat->Contens();
			      valid = (BInt)uDat->Contens().Value();
			      DESTROY(objCond_);
		      }
		    }
	    }
	  }
  }
  return(valid);
}


//--------------------------------------------------------------------
BBool BSet::Sort(const BCode& order)

/*! Sorts a set with an order criterium
 */
//--------------------------------------------------------------------
{
  numCompare = 0;
  orderError_="";
  if(Card()<2)
  {
	  orderError_ = I2
	  (
		  "It can not be ordered a set with less than two elements.",
		  "No se puede ordenar un conjunto con menos de dos elementos."
		);
	  Warning(orderError_);
  }
  else if(!order.Operator()			 ||
	        (order.Operator()->NumArg()!=2)	 ||
	        (order.Operator()->GrammarForArg(1)!=
	         order.Operator()->GrammarForArg(2)))
  {
	  orderError_ = I2
	  (
		  "The order criterion "+order.Name()+" is wrong defined. "
		  "This must have exactly two arguments of the same type.",
		  "El criterio de orden "+order.Name()+" est� mal definido. "
		  "�ste debe tener exactamente dos argumentos del mismo tipo."
	  );
	  Error(orderError_);
  }
  else
  {
//  Std(Out()+"\nSorting by "+order.Name()+ " criterium");
	  order_ = order;
	  array_.Sort(CompareObjects);
  }
  return(!orderError_.HasName());
}


//--------------------------------------------------------------------
BInt BSet::FindSorted(BSyntaxObject* searched)

/*! Finds an object in a sorted set with an order criterium
 */
//--------------------------------------------------------------------
{
  BInt found = -1;
  if(order_.Operator()) 
  {
	  found = array_.FindSorted(searched, CompareObjects);
  }
  return(found);
}


//--------------------------------------------------------------------
BDat BSet::Compare(const BSet* set1, const BSet* set2)

/*! Compares between two sets
 */
//--------------------------------------------------------------------
{
  BDat compare;
  const BSet& s1 = *set1;
  const BSet& s2 = *set2;
  BBool le = s1<=s2;
  BBool ge = s2<=s1;
  if( le&& ge) { compare= 0; }
  if( le&&!ge) { compare=-1; }
  if(!le&& ge) { compare= 1; }
  return(compare);
}

//--------------------------------------------------------------------
void BSet::PutNameBlock (const BNameBlock* nameBlock)
//--------------------------------------------------------------------
{
  nameBlock_ = nameBlock;
}

//--------------------------------------------------------------------
  BSyntaxObject* BSet::Member(const BText& memberName,
                                    BText& errMsg) const
//--------------------------------------------------------------------
{
  if(!nameBlock_ || !nameBlock_->EnsureIsAssigned())
  {
    errMsg = I2("Cannot access to member ",
                "No se puede acceder al miembro ")+memberName +
             I2(" of a non NameBlock set ",
                " de un conjunto que no es un NameBlock ");
    return NIL;
  }
  BSyntaxObject* result = nameBlock_->Member(memberName);
  return(result);
}


//--------------------------------------------------------------------
  BSyntaxObject* BSet::PublicMember(const BText& memberName,
                                          BText& errMsg) const
//--------------------------------------------------------------------
{
  if(!nameBlock_ || !nameBlock_->EnsureIsAssigned())
  {
    errMsg = I2("Cannot access to member ",
                "No se puede acceder al miembro ")+memberName +
             I2(" of a non NameBlock set ",
                " de un conjunto que no es un NameBlock ");
    return NIL;
  }
  else if(memberName[0]=='_')
  {
    errMsg = I2("Cannot access to private member ",
                "No se puede acceder al miembro privado ")+memberName +
             I2(" of NameBlock ",
                " del NameBlock ")+nameBlock_->Name();
    return NIL;
  }
  else 
  { 
    return(nameBlock_->Member(memberName)); 
  }
}

//--------------------------------------------------------------------
  BStruct* BSet::Struct(bool showError) const 
//--------------------------------------------------------------------
{ 
  BSet* T = (BSet*)this;
  T->struct_ = BStruct::Alive(struct_, showError);
  return(struct_); 
}