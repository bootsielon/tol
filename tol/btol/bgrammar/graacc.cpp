/* graacc.cpp: Global variables and access functions of BGrammar
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

#include <tol/tol_bgrammar.h>
#include <tol/tol_boper.h>
#include <tol/tol_bout.h>
#include <tol/tol_barray.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bdir.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bnameblock.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BGrammar).
//--------------------------------------------------------------------
BTraceInit("graacc.cpp");

//--------------------------------------------------------------------
BGrammar* BGrammar::Grammar() const
//--------------------------------------------------------------------
{
    return((BGrammar*)this);
}


//--------------------------------------------------------------------
void BGrammar::DelObject(BSyntaxObject* var)
//--------------------------------------------------------------------
{
  if(var)
  {
    if(var->IsHashed())
    {
      symbolTable_->Kill(var);
    } /*
    else if(var->HasName() || var->LocalName().HasName())
    {
      BStackManager::Pop(obj);
    }*/
  }
}

//--------------------------------------------------------------------
//! If the variable has name then adds it to hash table if
//! is global or else to stack. Any way puts current level
//! to object
void BGrammar::AddObject(BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  if(obj)
  {
    if(Level())
    {
      if(obj->HasName() || obj->LocalName().HasName())
      {
        BStackManager::Push(obj);
      }
    }
    else if(obj->HasName() && !obj->IsHashed())
    {
      int add = symbolTable_->Add(obj);
      if(add>=0) { obj->PutHashed(true); }
    }
    obj->PutLevel(Level());
  }
}

//- DYNAMIC SCOPE --------------
#ifdef __USE_DYNSCOPE__

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindLocal(const BText& name)

/*! Looks for a BSyntaxObject atom in the list with LocalName() equal
 *  than name.
 *  If the BSyntaxObject is not in the list, returns NIL.
*/
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = BStackManager::Find(name);
  return(obj); 
}
#endif /* ifdef __USE_DYNSCOPE__ */
//------------------------------

//- LEXICAL SCOPE --------------
#ifndef __USE_DYNSCOPE__

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindLocal(const BText& name)

/*! Looks for a BSyntaxObject atom in the list with LocalName() equal
 *  than name.
 *  If the BSyntaxObject is not in the list, returns NIL.
*/
//--------------------------------------------------------------------
{
  BList* lst = stack_;
  BSyntaxObject* found = NIL;
  if(!lst || !name.HasName()) { return(NIL); }
  assert(!stack_level.empty());
  found = BNameBlock::LocalMember(name);
  if(found) { return(found); }

  //std::cout << stack_level;
  level_list_t::iterator level_iter;
  BInt current_size, fun_level;

  level_iter = stack_level.begin();
  fun_level = level_iter->level;
  current_size = BGrammar::StackSize();


    do {
      while(lst && !found && current_size > level_iter->size)
      {
  BSyntaxObject* obj = (BSyntaxObject*)Car(lst);
  if(obj)
  {
          if(obj->LocalName().HasName())
          {
            if(obj->LocalName()==name)
            {
              found = obj;
            }
          }
          else
          {
            if(obj->Name()==name)
            {
              found = obj;
            }
          }
  }
  lst = Cdr(lst);
        --current_size;
      }
      if (found) break;
      /* not found in fun_level, now look one level "down" */
      --fun_level;
      if (fun_level > 0) {
        for (++level_iter;
             level_iter != stack_level.end() && level_iter->level != fun_level;
             ++level_iter) {
          //std::cout << "skipping " << *level_iter;
          /* skip this range of objects */
          while (lst && current_size > level_iter->size) {
            lst = Cdr(lst);
            --current_size;
          }
        }
        //fun_level = level_iter->level;
        /* precondition */
        assert(level_iter != stack_level.end());
      }
    } while (fun_level > 0);
    return(found);
}
#endif /* ifndef __USE_DYNSCOPE__ */
//------------------------------


//--------------------------------------------------------------------
BSyntaxObject* BGrammar::New(const BText& name,
                             BSyntaxObject* syn) const
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = NIL;
  if(newRenamed_)
  {
    obj=(*newRenamed_)(name, syn);
  }
  return(obj);
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::NewFunArg(const BText& localName,
                                   BSyntaxObject* syn) const
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = NIL;
  if(newFunArg_)
  {
    obj=(*newFunArg_)(localName, syn);
  }
  return(obj);
}


//--------------------------------------------------------------------
BGrammar* BGrammar::Last()

/*! Returns the last used grammar.
 */
//--------------------------------------------------------------------
{
    if(!last_) { last_=GraAnything(); }
    return(last_);
}


//--------------------------------------------------------------------
BGrammar* BGrammar::FindByName(const BText& name, bool sendError)

/*! Finds a grammar with name \a name. If it doesn't exist prints an
 *  error message and returns NIL.
 * \param name Name of grammar to find
 */
//--------------------------------------------------------------------
{
  BGrammar* grammar = NULL;
  BGraHashByName::const_iterator found = graByName_.find(name);
  if(found!=graByName_.end())
  {
    grammar = found->second;
  }
  if(!grammar && sendError)
  { Error(Out()+name+I2(" is not a grammar."," no es una gramática.")); }
  return(grammar);
}

//--------------------------------------------------------------------
BGrammar* BGrammar::FindByGid(BGrammarId gid, bool sendError)

/*! Finds a grammar with gid_ == gid. If it doesn't exist prints an
 *  error message and returns NIL.
 * \param gif Grammar identifier of grammar to find
 */
//--------------------------------------------------------------------
{
  BGrammar* grammar = NULL;
  BGraHashByGid::const_iterator found = graByGid_.find(gid);
  if(found!=graByGid_.end())
  {
    grammar = found->second;
  }
  if(!grammar && sendError)
  { 
    Error(Out()+"[FATAL] "+(int)gid+
          I2(" is not a grammar identifier. TOL cannot continue.",
             " no es un identificador de gramática. TOL no puede continuar.")); 
    abort();
  }
  return(grammar);
}

//--------------------------------------------------------------------
BMethod* BGrammar::FindMethod(const BText& name, bool sendError) const

/*! Looks for a valid method with name \a name for this grammar. If
 *  it doesn't exist prints an error message and returns NIL.
 * \param name Name of method to find
 */
//--------------------------------------------------------------------
{
  BMethod* found = NULL;
  BObjectCRef aux(name);
  int f = sortedMethods_.FindSorted(&aux,AtmCmp);
  if(f>=0)
  {
    found = (BMethod*)sortedMethods_[f];
  }
  if(!found && (this!=GraAnything()))
  {
    found = GraAnything()->FindMethod(name,false);
  }
  if(!found && sendError)
  { Error(Out()+name + I2(" is not a " + Name() + " method.",
          " No es un método de " + Name())); }
    return(found);
}


//--------------------------------------------------------------------
BGrammar* BGrammar::AddCasting(BGrammar* gra)

/*! Adds a casting to list of valid operators for this grammar and
 *  returns it.
 */
//--------------------------------------------------------------------
{
  assert(gra && !hasCasting_[gra->Gid()]);
  castings_.Add(gra);
  hasCasting_[gra->Gid()] = true;
  return (gra);
}


//--------------------------------------------------------------------
void   BGrammar::AddMethod(BMethod* met)

/*! Adds a method to list of valid methods for this grammar and
 *  returns it.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nAdding method ")+met->Name()+" to grammar "+Name()+" ... ");
  if(met && !FindMethod(met->Name(),false))
  {
  methods_ = Cons((BCore*)met, methods_);
    sortedMethods_.AddSorted(met,AtmCmp);
  }
}


//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindConstant(const BText& name)const
{
    BSyntaxObject* obj = NIL;
    if(findConstant_) { obj=(*findConstant_)(name); }
    return(obj);
}


//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindVariable(const BText& name) const
{
  BObjClassify oc(this,BOBJECTMODE);
  BSyntaxObject* result = symbolTable_->Search(oc, name);
  if(result) { return(result); }
  result = BNameBlock::UsingMember(name);
  if( result && 
     (result->Mode()==BOBJECTMODE)&&
     (result->Grammar()==this))
  {
    return(result);
  }
  else
  {
    return(NULL);
  }
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::UnknownVar() const
{
         if(this==GraCode    ()) return GraCode    ()->FindVariable("UnknownCode");
    else if(this==GraComplex ()) return GraComplex ()->FindVariable("?");
    else if(this==GraDate    ()) return GraDate    ()->FindVariable("UnknownDate");
    else if(this==GraMatrix  ()) return GraMatrix  ()->FindVariable("UnknownMatrix");
    else if(this==GraPolyn   ()) return GraPolyn   ()->FindVariable("UnknownPolyn");
    else if(this==GraRatio   ()) return GraRatio   ()->FindVariable("UnknownRatio");
    else if(this==GraReal    ()) return GraReal    ()->FindVariable("?");
    else if(this==GraSerie   ()) return GraSerie   ()->FindVariable("UnknownSerie");
    else if(this==GraSet     ()) return GraSet     ()->FindVariable("EMPTY");
    else if(this==GraText    ()) return GraText    ()->FindVariable("NULL");
    else if(this==GraTimeSet ()) return GraTimeSet ()->FindVariable("W");
#ifdef __USE_TC__
    else if(this==GraCTime   ()) return GraCTime   ()->FindVariable("UnknownCTime");
    else if(this==GraCTimeSet()) return GraCTimeSet()->FindVariable("CTEmpty");
    else if(this==GraCSeries ()) return GraCSeries ()->FindVariable("UnknownCSeries");
#endif
    else return 0;
}

//--------------------------------------------------------------------
BOperator* BGrammar::FindFunction(const BText& name) const
{
  BObjClassify oc(this,BBUILTINFUNMODE);
  return((BOperator*)(symbolTable_->Search(oc, name)));
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindUserFunc(const BText& name) const
{
  BObjClassify oc(this,BUSERFUNMODE);
  BSyntaxObject* result = symbolTable_->Search(oc, name);
  if(result) { return(result); }
  result = BNameBlock::UsingMember(name);
  if( result && 
     (result->Mode()==BUSERFUNMODE)&&
     (result->Grammar()==this))
  {
    return(result);
  }
  else
  {
    return(NULL);
  }
}

//--------------------------------------------------------------------
  static BSyntaxObject* getOperand(const BGrammar* gra, BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  BSyntaxObject* found =obj;
  if(obj) 
  { 
    BGrammar* g = obj->Grammar(); 
    int m = obj->Mode(); 
    if((m!=BOBJECTMODE) || ((gra!=GraAnything()) && (g!=gra) && !gra->HasCasting(g)))
    {
      found =  NULL;
    }
  }
  return(found);
}

//--------------------------------------------------------------------
  static BOperator* getOperator(const BGrammar* gra, BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  BOperator* found = NULL;
  if(obj)
  {
    int m = obj->Mode();
    BGrammar* g = obj->Grammar();
    if(m==BSTRUCTMODE)
    {
      BStruct* str = (BStruct*)obj;
      found = str->Function();
      g = GraSet();
    }
    else if((m==BUSERFUNMODE)||(m==BBUILTINFUNMODE))
    {
      found = (BOperator*)obj;
    }
    else if((m==BOBJECTMODE)&&(g==GraCode()))
    {
      found = Code(obj).Operator();
      g = found->Grammar();
    }
    if(found && 
       (g!=gra) && 
       (gra!=GraAnything()) && 
       (g!=GraAnything()) && 
       !gra->HasCasting(g))
    {
      found = NULL;
    }
  }
  return(found);
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::FindOperand(const BText& name, 
                                     bool mayBeConst) const

/*! If name is a valid constant for this grammar returns it.
 *  Else if exist a valid operand for this grammar in stack_ with
 *  name \a name returns it.
 *  Else if exist a operand in the list of instances of this grammar
 *  with name \a name returns it.
 *  Else if has a data base finder try to find it using this finder.
 *  Else prints a error message and returns NIL.
 *  The argument stack_ is the local list of instances for a
 *  evaluable object. Those instances are only available for the
 *  following instances of stack_.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nFindOperand ") + name +" in "+Name()+
//      "\nStack = "+LstIdentify(stack_));
//InitTotalTime("BGrammar::FindOperand");
  int i;
  if(!name.HasName()) { return(NIL); }
  BSyntaxObject* found =NIL;
  BObjByClassHash* obch;
  BObjByClassHash::const_iterator f;
  const BText& graName=Name();
  if(mayBeConst) 
  { 
    found = FindConstant(name); 
    found = getOperand(this, found);
  }
  if(!found) 
  { 
    found = BNameBlock::LocalMember(name); 
    found = getOperand(this, found);
  }
  if(!found) 
  { 
    found = FindLocal(name); 
    found = getOperand(this, found);
  }
  if(!found)
  {
    if(this!=GraCode())
    {
      if(dbiFinder_) { found = (*dbiFinder_)(graName, name); }
      BObjClassify oc(this,BOBJECTMODE);
      if(!found) { found = symbolTable_->Search(oc, name); }
      if(!found && (obch = symbolTable_->Search(name)) && obch->size())
      {
        f = obch->find(oc);
        if(f!=obch->end()) { found = f->second; }
        for(int i=0; !found && (i<castings_.Size()); i++)
        {
          BObjClassify oci(castings_[i], BOBJECTMODE);
          f = obch->find(oci);
          if(f!=obch->end()) { found = f->second; }
        }
      }
    }
    else if((obch = symbolTable_->Search(name)) && obch->size())
    {
      BArray<BOperator*> options;
      BOperator* opr=NULL;
      for(f=obch->begin(); f!=obch->end(); f++)
      {
        const BGrammar* g = f->first.grammar_;
        int             m = f->first.mode_;
        if(!found)
        {
          if(m==BUSERFUNMODE) 
          { 
            opr = (BOperator*)f->second; 
            found = opr->GetCode();
          } 
          else if((m==BOBJECTMODE)&&(g==GraCode())) 
          {
            found = f->second;
            opr = Code(found).Operator();
          }
        }
        else if(m==BUSERFUNMODE)
        {
          if(BSymbolTable::AreCompatible(opr,f->second))
          { 
            options.Add((BOperator*)f->second);
          }
        }
        else if((m==BOBJECTMODE)&&(g==GraCode()))
        {
          if(BSymbolTable::AreCompatible(found,f->second))
          { 
            options.Add(Code(f->second).Operator());
          }
        }
      }
      if(opr && options.Size())
      {
        BText another = "";
        for(i=0; i<options.Size(); i++)
        {
          another << BText("  ")+options[i]->LexInfo()+";\n";
          if(opr->Grammar()->Precedence()>options[i]->Grammar()->Precedence())
          {
            opr = options[i];
          }
        }
        Warning(I2("Function symbol ", "El símbolo de función ")+name+
                I2(" is ambiguous.",   " es ambiguo.")+"\n"+
                opr->LexInfo()+"\n"+
                I2(" has been selected but there were these others ",
                   " ha sido elegido pero había estas otras ")+options.Size()+
                I2(" options"," opciones")+":\n"+another);
      }
      
    }
  }
  if(!found) 
  { 
    found = BNameBlock::UsingMember(name); 
    found = getOperand(this, found);
  }
//SumPartialTime;
  return(found);
}


//--------------------------------------------------------------------
BOperator* BGrammar::FindOperator(const BText& name)const

/*! Looks for a valid operator with name name for this grammar. If it
 *  doesn't exist prints an error message and returns NIL.
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BGrammar::FindOperator");
//Std(BText("\nFinding Operator ") + name +" in "+Name());
//Std(Out()+"\nWith Stack = \n"+LstText(stack_,"{","}","\n"));
  BOperator* found = NULL;
  if(!found) 
  { 
    found = getOperator(this, BNameBlock::LocalMember(name));
  }
  if(!found) 
  { 
    found = getOperator(this, FindLocal(name));
  }
  BGrammar* g;
  BObjByClassHash* obch=NULL;
  BObjByClassHash::const_iterator f;

  if(!found) 
  {
    BObjClassify oc(this, BUSERFUNMODE);
    found = (BOperator*)symbolTable_->Search(oc, name);
    if(!found && (obch = symbolTable_->Search(name)) && obch->size())
    {
      f = obch->find(oc);
      if(f!=obch->end()) { found = (BOperator*)f->second; }
      for(int i=0; !found && (i<castings_.Size()); i++)
      {
        BObjClassify oci(castings_[i], BUSERFUNMODE);
        f = obch->find(oci);
        if(f!=obch->end()) { found = (BOperator*)f->second; }
      }
      if(!found)
      {
        BObjClassify ock(GraCode(), BOBJECTMODE);
        f = obch->find(ock);
        if(f!=obch->end()) 
        { 
          BUserCode* uCode = UCode(f->second);
          BOperator* opr = uCode->Contens().Operator();
          if(!opr) 
          {
            Error(I2("Corrupted Code object ",
                     "Objeto Code corrupto ")+uCode->Identify());
          }
          else
          {
            g = opr->Grammar();
            if((g==GraAnything()) || (g==this) || HasCasting(g)) { found=opr; }
          }
        }
      }
    }
  }
  if(!found) 
  { 
    found = getOperator(this, BNameBlock::UsingMember(name)); 
  }
//if(found) { Std(Out()+"\nFound Operator : \n"+found->Dump()); }
//SumPartialTime;
  return(found);
}


//--------------------------------------------------------------------
BList* BGrammar::Methods() const

/*! Returns the list of valid methods for the grammar
 */
//--------------------------------------------------------------------
{
  return(methods_);
}


//--------------------------------------------------------------------
BList* BGrammar::Operators() const

/*! Returns a copy of the list of valid operators for the grammar
 */
//--------------------------------------------------------------------
{
  BList* opr;
  BObjClassify oc(this, BUSERFUNMODE);
  opr = symbolTable_->Select(NIL, oc);
  opr = BNameBlock::Select(opr, oc);
  return(opr);
}


//--------------------------------------------------------------------
BList*  BGrammar::Variables() const

/*! Returns a copy of the list of valid variables for this grammar.
 */
//--------------------------------------------------------------------
{
  BObjClassify oc(this,BOBJECTMODE);
  BList* var = symbolTable_->Select(NIL,oc);
  var = BNameBlock::Select(var, oc);
  return(var);
}


//--------------------------------------------------------------------
BList* BGrammar::GetMethods() const

/*! Returns the list of valid methods for the grammar
*/
//--------------------------------------------------------------------
{
  return(LstDuplicate(Methods()));
}


//--------------------------------------------------------------------
BList* BGrammar::GetOperators() const

/*! Returns a copy of the list of valid operators for the grammar
 */
//--------------------------------------------------------------------
{
//Std(BText("\n")+Name()+"::GetOperators()");
  BObjClassify oc(this, BUSERFUNMODE);
  BList* lst_opr = symbolTable_->Select(NIL, oc);
  BObjClassify nboc(GraNameBlock(), BOBJECTMODE);
  BList* lst_unb = symbolTable_->Select(NIL,nboc);
  BUserNameBlock* unb = NULL;
  BList* aux = lst_unb;
//Std(BText("\n")+Name()+"::GetOperators() global "+LstLength(lst_opr));
  while(lst_unb)
  {
    unb = (BUserNameBlock*)lst_unb->Car();
  //Std(BText("\n")+Name()+"::GetOperators() Exploring NameBlock "+unb->FullName());
    lst_opr = unb->Contens().SelectMembersDeep(lst_opr, oc);
    lst_unb = lst_unb->Cdr();
  }
  DESTROY(aux);
/* * /
  Std(BText("\n")+Name()+"::GetOperators() all "+LstLength(lst_opr));
  aux = lst_opr;
  while(aux)
  {
    BSyntaxObject* obj = (BSyntaxObject*)aux->Car();
    Std(BText("\n  ")+obj->Grammar()->Name()+" "+obj->FullName());
    aux = aux->Cdr();
  }
/* */
  return(lst_opr);
}

//--------------------------------------------------------------------
BList*  BGrammar::GetVariables() const

/*! Returns a copy of the list of valid variables for this grammar.
 */
//--------------------------------------------------------------------
{
    return(Variables());
}


//--------------------------------------------------------------------
BList* GraList(const BText& textList)

/*! Returns a new list with the grammars which name is in textList.
 */
//--------------------------------------------------------------------
{
    BInt error = BFALSE;
    BList* lst=NIL;
    BArray<BText> name;
    ReadAllTokens(textList, name, ' ');
    for(BInt n=name.Size()-1; n>=0; n--)
    {
  BGrammar* gra = BGrammar::FindByName(name[n]);
  if(!gra) { error= BTRUE; }
  else     { lst = Cons(gra, lst); }
    }
    if(error) { lst = LstDestroyAllCons(lst); }
    return(lst);
}


//--------------------------------------------------------------------
void BGrammar::PutLast(BGrammar* gra)

/*! Puts the last_ = grammarName.
 */
//--------------------------------------------------------------------
{
  last_=gra;
}


//--------------------------------------------------------------------
BDat BGrammar::Compare(BSyntaxObject* syn1, BSyntaxObject* syn2) const
//--------------------------------------------------------------------
{
  assert(syn1 && syn2);
  syn1->Do();
  syn2->Do();
  BDat compare;
  if(compare_ && (*compare_) &&
     (this==syn2->Grammar()) && (this==syn1->Grammar()))
  {
    compare = (*compare_)(syn1, syn2);
  }
  return(compare);
}

//--------------------------------------------------------------------
/*! Invokes \a castingFunction_ in order to convert BSyntaxObject
 * \a obj given as argument into an object of (this) Grammar.
 * \return a valid object whether a conversion has been done, null
 *         in other case.
 */
BSyntaxObject* BGrammar::Casting(BSyntaxObject* obj)
//--------------------------------------------------------------------
{
  BSyntaxObject* cast = (*castingFunction_)(obj);
  return cast;
}


//--------------------------------------------------------------------
BSyntaxObject* BAnyGrammar::FindOperand(const BText& name, bool mayBeConst) const

/*! Finds an object named name in any grammar.
 */
//--------------------------------------------------------------------
{
  int i;
  BSyntaxObject* found = NULL;
  if(mayBeConst)
  {
    if(!found) { found = GraText()->FindConstant(name); }
    if(!found) { found = GraDate()->FindConstant(name); }
    if(!found) { found = GraReal()->FindConstant(name); }
  }
  if(!found) { found = getOperand(this, BNameBlock::LocalMember(name)); }
  if(!found) { found = getOperand(this, FindLocal(name)); }
  BObjByClassHash* obch;
  BObjByClassHash::const_iterator f;
  if(!found && (obch = symbolTable_->Search(name)) && obch->size())
  {
    BObjClassify oc(last_, BOBJECTMODE);
    f = obch->find(oc);
    if(f!=obch->end()) { found = f->second; }
    for(i=0; !found && (i<last_->GetCastings().Size()); i++)
    {
      BObjClassify oc(last_->GetCastings()[i], BOBJECTMODE);
      f = obch->find(oc);
      if(f!=obch->end()) { found = f->second; }
    }
    if(!found)
    {
      BArray<BSyntaxObject*> options;
      for(f=obch->begin(); f!=obch->end(); f++)
      {
        if(f->second->Mode()!=BOBJECTMODE) { continue; }
        if(!found) 
        { 
          found = f->second; 
        }
        else if(BSymbolTable::AreCompatible(found,f->second))
        { 
          options.Add(f->second); 
        }
      }
      if(options.Size())
      {
        BGrammarId gPos = found->Grammar()->Gid();
        BText another = "";
        for(i=0; i<options.Size(); i++)
        {
          another << BText("  ")+options[i]->LexInfo()+";\n";
          if(found->Grammar()->Precedence()>options[i]->Grammar()->Precedence())
          {
            found = options[i];
          }
        }
        Warning(I2("Variable symbol ", "El símbolo de variable ")+name+
                I2(" is ambiguous.",   " es ambiguo.")+"\n"+
                found->LexInfo()+"\n;"+
                I2(" has been selected but there are this other ",
                " ha sido elegido pero había otras ")+options.Size()+":\n"+
                another);
      }
    }
  }
  if(!found)  { found = BNameBlock::UsingMember(name);  }
  return(found);
}


//--------------------------------------------------------------------
BOperator* BAnyGrammar::FindOperator(const BText& name) const

/*! Finds an object named name in any grammar.
 */
//--------------------------------------------------------------------
{
  int i;
  BOperator* found = NULL;
  if(!found) { found = getOperator(this, BNameBlock::LocalMember(name)); }
  if(!found) { found = getOperator(this, FindLocal(name)); }
  BObjByClassHash* obch;
  BObjByClassHash::const_iterator f;
  if(!found && (obch = symbolTable_->Search(name)) && obch->size())
  {
    BObjClassify oc(last_, BUSERFUNMODE);
    f = obch->find(oc);
    if(f!=obch->end()) { found = (BOperator*)f->second; }
    for(i=0; !found && (i<last_->GetCastings().Size()); i++)
    {
      BObjClassify oc(last_->GetCastings()[i], BUSERFUNMODE);
      f = obch->find(oc);
      if(f!=obch->end()) { found = (BOperator*)f->second; }
    }
    if(!found)
    {
      BArray<BSyntaxObject*> options;
      for(f=obch->begin(); f!=obch->end(); f++)
      {
        const BGrammar* g = f->first.grammar_;
        int             m = f->first.mode_;
        if(!found)
        {
          if(m==BUSERFUNMODE) 
          { 
            found = (BOperator*)f->second; 
          }
          else if(g==GraCode()) 
          { 
            BUserCode* code = UCode(f->second);
            if(code) { found = code->Contens().Operator(); }
          }
        }
        if(found && (BSymbolTable::AreCompatible(found,f->second))&&
           (m==BUSERFUNMODE))
        { 
          options.Add(f->second);
        }
      }
      if(options.Size())
      {
        BText another = "";
        for(i=0; i<options.Size(); i++)
        {
          another << BText("  ")+options[i]->LexInfo()+";\n";
          if(found->Grammar()->Precedence()>options[i]->Grammar()->Precedence())
          {
            found = (BOperator*)options[i];
          }
        }
        Warning(I2("Function symbol ", "El símbolo de función ")+name+
                I2(" is ambiguous.",   " es ambiguo.")+"\n"+
                found->LexInfo()+"\n;"+
                I2(" has been selected but there were these other ",
                   " ha sido elegido pero había estas otras ")+options.Size()+
                I2(" options"," opciones")+":\n"+another);
      }
    }
  }
  if(!found)
  {
    found = getOperator(this, BNameBlock::UsingMember(name));
  }
  return(found);
}


//--------------------------------------------------------------------
BSyntaxObject* BAnyGrammar::New(const BText& name,
              BSyntaxObject* syn) const
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    if(syn)
    {
  if(syn->Grammar() != (BGrammar*)this)
  {
      result = syn->Grammar()->New(name, syn);
  }
  else
  {
      result = syn;
  }
    }
    return(result);
}


//--------------------------------------------------------------------
BSyntaxObject* BAnyGrammar::NewFunArg(const BText& localName,
                              BSyntaxObject* syn) const
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    if(syn)
    {
  if(syn->Grammar() != (BGrammar*)this)
  {
      result = syn->Grammar()->NewFunArg(localName, syn);
  }
  else
  {
      result = syn;
  }
    }
    return(result);
}


//--------------------------------------------------------------------
BBool BAnyGrammar::InitGrammar()

/*! Initializes the Anything grammar.
 */
//--------------------------------------------------------------------
{
    if(!anyGrammar_) { anyGrammar_ = new BAnyGrammar; }
    return(BTRUE);
}


//--------------------------------------------------------------------
/* Static methods to Turn On/Off stopFlag_ attribute,
 * used to stop evaluation process
 */
void BGrammar::Turn_StopFlag_On()  { BGrammar::stopFlag_ = true;  }
void BGrammar::Turn_StopFlag_Off() { BGrammar::stopFlag_ = false; }
