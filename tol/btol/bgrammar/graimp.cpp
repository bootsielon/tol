/* graimp.cpp: Implements functions of BGrammar of GNU/TOL language.
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

//#define TRACE_LEVEL 1

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bgrammar.h>
#include <tol/tol_boper.h>
#include <tol/tol_bout.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bmatrix.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bnameblock.h>
//#include <tol/tol_btimer.h>

BTraceInit("graimp.cpp");

//#define DO_PUT_EMS 

#define UseTknCache

//#define DoTknStat

#ifdef DoTknStat
  static int    numTknFind_ = 0;     
  static int    skpTknFind_ = 0;     
  static double tmTknFind_  = 0.0; 
#else
#endif

#ifdef DoTknStat
# define StartTknStat double t0 =  clock(); numTknFind_++;
#else
# define StartTknStat
#endif

#ifdef DoTknStat
# define SkipTknStat  skpTknFind_++;
#else
# define SkipTknStat
#endif

#ifdef DoTknStat
# define EndTknStat  tmTknFind_ +=  clock()-t0;
#else
# define EndTknStat
#endif


//--------------------------------------------------------------------
  class eval_depth_t
//--------------------------------------------------------------------
{
public:

  eval_depth_t() 
    {
      if (eval_depth_t::level < 0) {
        BGrammar::Turn_StopFlag_Off();
      }
      ++eval_depth_t::level;
    }
  ~eval_depth_t() 
    {
      --eval_depth_t::level;
    }

private:
  
  static int level;
};

int eval_depth_t::level = -1;

//--------------------------------------------------------------------
  static BGrammar* TknFindGrammar(BToken* tok)
//--------------------------------------------------------------------
{
  StartTknStat;
  BGrammar* gra = NULL;
  const BText& name = tok->Name();
#ifdef UseTknCache
  if((tok->ObjectClass()=='G') && tok->Object()) 
  { 
    gra = (BGrammar*)tok->Object(); 
    SkipTknStat;
  }
  else if(!tok->ObjectClass()) 
  {
    gra=BGrammar::FindByName(name,false);
    tok->PutObject(gra);
    tok->PutObjectClass('G');
  }
#else
  gra=BGrammar::FindByName(name,false);
#endif
  EndTknStat;
  return(gra);
}


//--------------------------------------------------------------------
  static const BSpecialFunction* TknFindSpFun(BToken* tok)
//--------------------------------------------------------------------
{
  StartTknStat;
  const BSpecialFunction* spf = NULL;
#ifdef UseTknCache
  if((tok->ObjectClass()=='S') && tok->Object()) 
  { 
    spf = (const BSpecialFunction*)tok->Object(); 
    SkipTknStat;
  }
  else if(!tok->ObjectClass()) 
  {
    spf = BSpecialFunction::Get(tok->Name());
    tok->PutObject(spf);
    tok->PutObjectClass('S');
  }
#else
  spf = BSpecialFunction::Get(tok->Name());
#endif
  EndTknStat;
  return(spf);
}

//--------------------------------------------------------------------
  static BSyntaxObject* TknFindOperand(BGrammar* g, BToken* tok, 
                                       bool mayBeConst,
                                       bool& error)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("TknFindOperand(")+g->Name()+","+tok->Name();
#endif
  StartTknStat;
  BSyntaxObject* obj = NULL;
  const BText& name = tok->Name();
#ifdef UseTknCache
  if((tok->ObjectClass()=='A') && tok->Object()) 
  { 
    obj = (BSyntaxObject*)tok->Object(); 
    SkipTknStat;
  }
  else if(!tok->ObjectClass()) 
  {
    obj=g->FindOperand(name,mayBeConst);
    bool isConstant = false; //tok->ConstantType()!='\0';
    if(obj && (isConstant || obj->System()))
    {
      TRACE_SHOW_LOW(fun,BText("Cached Level(")+obj->Level()+","+BGrammar::Level()+")");
      tok->PutObject(obj);
      tok->PutObjectClass('A');
      tok->PutLevel(obj->Level());
      tok->PutIsSystem(obj->System());
    //if(isConstant) { obj->IncNRefs(); }
    }
  }
#else
  obj=g->FindOperand(name,mayBeConst&&tok->ConstantType());
#endif
  if(!obj)
  {
    error = true;
    if(name=="Stop") /* Stop flag */
	  { 
		  BGrammar::Turn_StopFlag_On(); 
		  return NIL;
	  }
	  else             /* wrong identifier */
	  {
	    BText nm = name;
      if(name.Get(0)=='\"') { nm += "\""; }
  		BText another;
      BGrammar::SymbolTable().LexInfo(another,name,BOBJECTMODE);
      if(another.HasName())
      {
		    Error(nm+
              I2(" is not an object of type ",
			              " no es un objeto de tipo ") + 
              g->Name() +
			        I2(" but there are others items with the same name",
			           " pero existen estos otros objetos con el mismo nombre ") + 
              "\n"+another);
		  } 
      else 
      {
		    Error(nm+
              I2(" is not a valid object for type ",
			           " no es un objeto valido para el tipo ") + 
              g->Name() + ".");
      } 	  
    }
  }
  EndTknStat;
  return(obj);
}


//--------------------------------------------------------------------
  static BOperator* TknFindOperator(BGrammar* g, BToken* tok, bool& error)
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText("TknFindOperator(")+g->Name()+","+tok->Name()+")";
#endif
  StartTknStat;
  BOperator* opr = NULL;
  const BText& name = tok->Name();
#ifdef UseTknCache
  if((tok->ObjectClass()=='F') && tok->Object()) 
  { 
    opr = (BOperator*)tok->Object(); 
    SkipTknStat;
  }
  else if(!tok->ObjectClass() || !tok->Object()) 
  {
    opr=g->FindOperator(name);  
    if(opr && opr->System())
    {
      TRACE_SHOW_LOW(fun,BText("Cached Level(")+opr->Level()+","+BGrammar::Level()+")");
      tok->PutObjectClass('F');
      tok->PutObject(opr);
      tok->PutLevel(opr->Level());
      tok->PutIsSystem(opr->System());
    }
  }
#else
    opr=g->FindOperator(name);
#endif
  if(!opr)
  {
    error = true;
    Error(name + I2(" is not a " + g->Name() + " operator.",
                    " no es un operador de tipo " + g->Name()));
  }
  EndTknStat;
  return(opr);
}


//--------------------------------------------------------------------
void TknFindReport()
//--------------------------------------------------------------------
{
#ifdef DoTknStat
  int saved = (int)floor(100*double(skpTknFind_)/double(numTknFind_));
  Std(BText("\n  Token searches = ")+numTknFind_+
            " skiped = "+skpTknFind_+" ("+saved+"%) "
            "Total time = "+BTimer::ClockToMiliSecond(tmTknFind_)/1000.0+" seconds\n");
#endif
}

//--------------------------------------------------------------------
void BGrammar::CleanTreeCache(const List* tre, bool forze)
//--------------------------------------------------------------------
{
#ifdef UseTknCache
//VBR: Se podría añadir un flag a BToken indicando si es necesario
//     limpiar de aquí para abajo para ahorrar tiempo en el recorrido
//     del árbol en bucles
  BToken* tok  = BParser::treToken(tre);
#ifdef TRACE_LEVEL
  BText fun = BText("CleanTreeCache(")+tok->Name()+")";
#endif
  if(forze)
  {
    tok->PutObject(NULL);
    tok->PutObjectClass('\0');
  }
  else if(tok->Object() && strchr("AF",tok->ObjectClass()) && !tok->ConstantType())  
  {
    BSyntaxObject* obj = (BSyntaxObject*)tok->Object();
    if(!tok->System())
    {
      TRACE_SHOW_LOW(fun,BText("Cached Level(")+tok->Level()+")");
      tok->PutObject(NULL);
      tok->PutObjectClass('\0');
    }
  }
	List* b = (List*) tre;
  switch (tok->TokenType()) 
  {
		case FUNCTION:
		case SEPARATOR:
		{
      while((b = b->cdr())) { CleanTreeCache((List*) (b->car()), forze); }
	    break;
		}
		case TYPE:
		case MONARY:
		{
		  CleanTreeCache((const List*) Tree::treLeft((List*)tre), forze);
      break;
		}
		case BINARY:
		{
		  CleanTreeCache((const List*) Tree::treLeft ((List*)tre), forze);
		  CleanTreeCache((const List*) Tree::treRight((List*)tre), forze);
      break; 
		}
		default: break;
  }
#endif
}


//--------------------------------------------------------------------
BSyntaxObject* BGrammar::LeftEvaluateTree(const List* tre)
//--------------------------------------------------------------------
{
  if(!tre) { return(NIL); }
  BSyntaxObject* result = NIL;
  BToken* tok  = BParser::treToken(tre);
  if(!tok || BGrammar::StopFlag()) { return(NIL); } 
  if((tok->ObjectClass()=='A') && tok->Object()) 
  { return((BSyntaxObject*)tok->Object()); }
  BTokenType type  = tok->TokenType();
  const BSpecialFunction* spFun = NIL;
  BGrammar* gra1 = NIL;
  bool error = BFALSE; // BTRUE if expGra != result->Grammar()->Name()

  /* get grammar instance of given TYPE token to ensure that the next
   * token ARGUMENT have the same grammar.
   * gra1 store this first Grammar instance.
   */
  const BText&   name  = tok->Name();
  if(type == TYPE) 
  {
/*  Std(BText("LeftEvaluateTree(")+Name()+","+
      BParser::Unparse(tre,"","")+")\n\n"+
      BParser::treWrite((List*)tre,"  ",false)); */
    gra1 = TknFindGrammar(tok);
    return(gra1->LeftEvaluateTree(Tree::treLeft((List*)tre)));
  };
  if(type == ARGUMENT)
  {
    result = TknFindOperand(this,tok,false,error);
    if(result && gra1) // gra1!=NIL -> TYPE specified previously
    { 
      BGrammar* gra2 = result->Grammar();
      if (!(gra1->Name()==gra2->Name())) 
      {
        Error(name + I2(" is not a reference.",
                        " no es una referencia de tipo ") + gra1->Name()+
                     I2(" but it's a reference of type ",
                        " sino de tipo ")+gra2->Name());
         error = true;
         result = NIL;
      }
    }
    if(!result && gra1)
    {
      result = TknFindOperand(gra1,tok,false,error);
    }
    if(!result && !error)
    {
      Error(name + I2(" is not a " + Name() + " reference.",
                      " no es una referencia de tipo " + Name()));
    }
  }
  else if((spFun = BSpecialFunction::LeftGet(name)))
  {
    result = spFun->Evaluate(this,tre,BTRUE);
  }
  return(result);
}


//--------------------------------------------------------------------
BSyntaxObject* EvaluateMatrix(const List* tre)
//--------------------------------------------------------------------
{
    bool moreThanOneRow = true;
    BSyntaxObject* result = NIL;
    List* b = (List*) tre;
    BInt rows = b->length()-1;
    BMatrix<BDat> M;
    BInt i=0;
    while((b = b->cdr()) && moreThanOneRow)
    {
	List* c;
	int columns;
	List* bTre = Tree::treNode(b);
	BToken* bTok = BParser::treToken(bTre);
	assert(bTok);

	if(bTok->TokenType()!=SEPARATOR) // is a vector (matrix of 1 row)
	{
	    moreThanOneRow = false;
	    c = (List*) tre;
	    columns = rows;
	    rows = 1;
	} else {
	    c = Tree::treNode(b);
	    columns = c->length()-1;
	}

	if(columns>0)
	{
	    if(M.Columns() && (M.Columns()!=columns)) {
		M.Alloc(0,0);
		break;
	    }
	    M.Alloc(rows,columns);
	    BInt j=0;
	    while((c = c->cdr()))
	    {
		List* cTre = Tree::treNode(c);
		BSyntaxObject* obj = GraReal()->EvaluateTree(cTre);
		if(obj) {
		    M(i,j) = Dat(obj);
		    DESTROY(obj);
		}
		j++;
	    }
	    i++;
	}
	else if(bTok->Close() && (bTok->Close()->Name()==")"))
	{
	    M.Alloc(rows,1);
	    const BText& bNam = bTok->Name();
	    M(i,0).PutValue(bNam);
	    i++;
	}
    }
    if(M.Columns()*M.Rows()==0)
    {
	Error(I2("Invalid matrix expression : \n",
		 "Expresión no válida para formación de matrices : \n") +
	      BParser::Unparse(tre, "  ", "\n"));
	return(NIL);
    }
    else
    {
	result = new BContensMat(M);
    }
  return(result);
}



//--------------------------------------------------------------------
BSyntaxObject* EvaluateSet(const List* tre)
//--------------------------------------------------------------------
{
    BSyntaxObject* result = NIL;
    List* b = (List*) tre;
    BList* lst = NIL;
    BList* aux = NIL;
    while((b = b->cdr()))
    {
	List* bTre = Tree::treNode(b);
	BSyntaxObject* obj = GraAnything()->EvaluateTree(bTre);
	if(obj)
	{
	    LstFastAppend(lst,aux,obj);
	}
    }
    BSet set;
    set.RobElement(lst);
    result = new BContensSet(set);
    return(result);
}


/*  VBR: USEFULL FOR TRACING BUGS
static int iter_ = 0;
*/

typedef hash_map_by_size_t<BSyntaxObject*>::dense_ BLocalItems;

//--------------------------------------------------------------------
  BSyntaxObject* Evaluate_Type(const List* branch, BToken* line, 
                               BGrammar*& newGrammar)
//--------------------------------------------------------------------
{
  BTypeToken* tt = (BTypeToken*)line;
  BSyntaxObject* result = NULL;
  if(tt->type_==BTypeToken::BSYSTEM)
  {
    newGrammar=TknFindGrammar(line);
    result = newGrammar->EvaluateTree(branch);
  }
  else if(tt->type_==BTypeToken::BSTRUCT)
  {
    newGrammar=GraSet();
    result = GraSet()->EvaluateTree(branch);
    BStruct* str = NULL;
    if(result && (result->Grammar()==GraSet()))
    {
      BUserSet* uset = (BUserSet*)result;
      str = uset->Contens().Struct();
    }
    if(result && (!str || (str->Name()!=tt->Name())))
    {
      Error(I2("A set with structure ",
               "Se esperaba un conjunto con estructura ")+tt->Name()+
            I2(" was expected instead of ",
               " en lugar de ")+
            result->Grammar()->Name()+ " "+
            result->Identify());
      result = NULL;
    }
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::EvaluateTree(const List* tre, BInt from_UF)
//--------------------------------------------------------------------
{
  static BText none;
  eval_depth_t eval_depth_helper;
  bool error = false;
  BGrammar* newGrammar = NIL;
  BBool enableCasting=BTRUE;
  bool isCreatingNameBlock = BEqualOperator::IsCreatingNameBlock();
  BToken* tok  = BParser::treToken(tre);
  if(!tok || BGrammar::StopFlag()) { return(NIL); } 
  if((tok->ObjectClass()=='A') && tok->Object()) 
  { return((BSyntaxObject*)tok->Object()); }

  // Token data
  const BText& name = tok->Name();

  BCloseToken* close = tok->Close();
  const BText* openName_=&none, *closeName_=&none;
  if(close) 
  {
    openName_  = &tok->Close()->Open()->Name();
    closeName_ = &close->Name();
  }
  const BText& openName  =*openName_;
  const BText& closeName =*closeName_;

#ifdef TRACE_LEVEL
  expr = BParser::Unparse(tre, "  ", "\n");
  BText fun = Name()+"->EvaluateTree("+name+")";
#endif
  TRACE_SHOW_LOW(fun,"Begin "+expr);

  // Scope control
  // BList* local = NIL; // unused
  int stackPos = -1;

  if((this==GraNameBlock()) && (name=="SetOfAnything"))
  {
    return(BNameBlock::EvaluateTree(tre));
  } 
  bool incLevel = false;
  TRACE_SHOW_MEDIUM(fun,BText("1"));
  if(openName=="{") 
  {
    TRACE_SHOW_MEDIUM(fun,"1.1");
    stackPos = BGrammar::StackSize();
    IncLevel();
    incLevel = true;
#ifndef __USE_DYNSCOPE__
    TRACE_SHOW_MEDIUM(fun,"1.2");
    if (!from_UF) { BGrammar::PushStackLevel(Level()); }
#endif
  }

  BSyntaxObject* result = NIL;
  BTokenType type = tok->TokenType();

  const BSpecialFunction* spFun = NIL;
    
  TRACE_SHOW_MEDIUM(fun,"2");
  if(type == ARGUMENT)
  {
    TRACE_SHOW_MEDIUM(fun,"2.1");
    result = TknFindOperand(this,tok,true,error);
  }
  else if(type == SEPARATOR)
  {
    TRACE_SHOW_MEDIUM(fun,"2.2");
    if((this==GraMatrix()) && (closeName==")")) 
    {
      TRACE_SHOW_LOW(fun,"End Matrix");
      return(EvaluateMatrix(tre));
    } 
    else if(((this==GraSet()) || (this==GraAnything())) && (closeName==")"))
    {
      TRACE_SHOW_LOW(fun,"End Set");
      return(EvaluateSet(tre));
    } 
    else 
    {
      List* b = (List*)tre;
      BLocalItems localItems;
      BLocalItems::const_iterator found;
      SetEmptyKey(localItems, 0);
      while((b=b->cdr())) 
      {
        if(result && !result->HasName()) { DESTROY(result); }
        BToken* line = BParser::treToken(Tree::treNode(b));
        if(line && (line->TokenType()==TYPE) && (newGrammar=TknFindGrammar(line)))
        {
          result = newGrammar->EvaluateTree((const List*) Tree::treNode(b));
        } 
        else 
        {
          result = EvaluateTree((const List*) Tree::treNode(b));
        }
        if(result) 
        {
          result->Do();
          found = localItems.find((size_t)result);
          if(found==localItems.end())
          {
            localItems[(size_t)result]=result;
            result->IncNRefs();
          }
        }
        PutLast(this);
      }
      for(found=localItems.begin(); found!=localItems.end(); found++)
      {
        BSyntaxObject* obj = (BSyntaxObject*)found->second; 
        if(obj)
        { 
          obj->DecNRefs();
          if(obj != result) { DESTROY(obj); }
        }
      }
    }
  }
  else if((type == TYPE) && (newGrammar=TknFindGrammar(tok)))
  {
    TRACE_SHOW_MEDIUM(fun,"2.3");
    result = newGrammar->EvaluateTree(Tree::treLeft((List*)tre));
    enableCasting = BTRUE;
  // is an orphan object with a level_>0 and must be removed
    if((Level()>0) && result && ((result->Level()>Level()) || (result->Grammar()==GraSet())) ) 
    {
      const BText& newName = result->Name();
      result = newGrammar->New(newName, result);
    } 
  }
  else if (type == BINARY && this == GraAnything() && 
           name != "=" && name != "#F#" && 
           !(spFun=TknFindSpFun(tok)) ) 
  {
    BSyntaxObject * first_arg;
    List* args = (List*)tre->cdr();
  
    int oldObjNum = BSyntaxObject::NSyntaxObject();
    first_arg = GraAnything()->EvaluateTree(Tree::treNode(args));
    bool delete_first_arg = (oldObjNum < BSyntaxObject::NSyntaxObject());
    //BGrammar* f_gra = NULL; // unused
    if (first_arg) 
    {
      BGrammar* f_gra = first_arg->Grammar();
      if(delete_first_arg) 
      { 
        SAFE_DESTROY(first_arg, result); 
      }
      // find BINARY in Grammar of first_arg.
      BOperator * ope = TknFindOperator(f_gra,tok,error);
      if (ope) { result = ope->Evaluate(args); }
    }
  }
  else 
  {
    TRACE_SHOW_MEDIUM(fun,"2.4");
    if(!spFun) { spFun  = TknFindSpFun(tok); }
    if( spFun) { result = spFun->Evaluate(this,tre);  } 
    else 
    {
      BOperator* ope = TknFindOperator(this,tok,error);
      if(ope) { result = ope->Evaluate(((List*)tre)->cdr()); }
    }
  }
  TRACE_SHOW_MEDIUM(fun,"3");

  if(result && (result->Mode()!=BSTRUCTMODE)&& (result->Mode()!=BCLASSMODE))
  {
    TRACE_SHOW_MEDIUM(fun,"3.1");
    BBool ok = BFALSE;
    if(((name=="=") || (name=="#F#")) && (openName!="{")) 
    { enableCasting = BFALSE; }
    TRACE_SHOW_MEDIUM(fun,"3.2");
    BGrammar* rg = result->Grammar();
    int       rm = result->Mode();
    if(result) 
    {
      TRACE_SHOW_MEDIUM(fun,"3.2.1");
      ok = ((name==":=") || 
      (this==GraAnything()) ||
      (this==rg) ||
      (BUSERFUNMODE==rm));
    }
    TRACE_SHOW_MEDIUM(fun,"3.3");
    if(result && !ok && enableCasting && castingFunction_)
    {
      TRACE_SHOW_MEDIUM(fun,"3.3.1");
      BSyntaxObject* cast = (*castingFunction_)(result);
      if(cast) { result = cast; }
      else     
      {
        BGrammar* rg = result->Grammar();
        BText rgn = (rg)?rg->Name():"UNKNOWN TYPE";
        BText tgn = this->Name();
        BText rid = result->Identify();
        TRACE_SHOW_MEDIUM(fun,"3.3.1.2");
        BText en = BText("It has not been possible to convert from ")+ 
                   rgn+ " to "+
                   tgn+ " for object '"+
                   rid+"'";
        BText es = BText("No ha sido posible convertir de ")+
                   rgn+ " hacia "+
                   tgn + " para el objeto '"+
                   rid+"'";
        Error(I2(en,es));
        result = NIL;
      }
    }
  }
  TRACE_SHOW_MEDIUM(fun,"4");
  if(incLevel) 
  {
    TRACE_SHOW_MEDIUM(fun,"4.1");
    if(result) result->IncNRefs();
    DestroyStackUntil(stackPos,result);
    if(result) result->DecNRefs();
    DecLevel();
#ifndef __USE_DYNSCOPE__
    if (!from_UF)
      BGrammar::PopStackLevel();        
#endif
  }
  TRACE_SHOW_MEDIUM(fun,"5");
  if((UnparseNodes()>1) && result && !result->Expression().HasName()) 
  {
    result->PutExpression(BParser::Unparse(tre, "  ", "\n"));
  }

  TRACE_SHOW_MEDIUM(fun,"6");
  if(result && !result->GetOisTree()) 
  { 
    TRACE_SHOW_MEDIUM(fun,"6.1");
    result->PutOisTree(tre); 
  }

//TRACE_SHOW_LOW(fun,BText("End with result type: ")<<BText((result!=NULL)?result->Grammar()->Name():BText("NULL")));
  TRACE_SHOW_LOW(fun,"END");
  if(result && (result->Grammar()==GraNameBlock()))
  {
    BUserNameBlock* unb = (BUserNameBlock*)result;
    BUserNameBlock* own = unb->Contens().Owner();
    if(unb->HasName() && (own!=unb))
    {
      BNameBlock& nb = unb->Contens();
      if(!own || (own->Level()>unb->Level()))
      {
        nb.PutOwner(unb);
        nb.PutLocalName(unb->Name());
        BText fatherName = "";
        if(isCreatingNameBlock)
        {
          fatherName = BEqualOperator::CurrentFatherName();
        }
        nb.RebuildFullNameDeep(fatherName);
      }
    }
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::EvaluateExpr(const BText& expr)
//--------------------------------------------------------------------
{
  if(!Compact(expr).HasName()) { return(NIL); }
  Tree* realTree = BParser::parsing(expr);
//Trace(BText("Parsed expression :\n")+Unparsing(tre, "	 ", "|"));
  BSyntaxObject* result = NIL;
  if(realTree && !BParser::HasError())
  {
    List* tre = realTree->getTree();
    if((result = EvaluateTree(tre)))
    {
      BGrammar::PutLast(this);
    }
    SAFE_DESTROY(tre,result);
  };
  PutLast(this);
  if(UnparseNodes() && result) 
  {
    result->PutExpression(expr);
  }
  return(result);
}

//--------------------------------------------------------------------
BSyntaxObject* BGrammar::LeftEvaluateExpr(const BText& expr)
//--------------------------------------------------------------------
{
  if(!Compact(expr).HasName()) { return(NIL); }
  Tree* realTree = BParser::parsing(expr);
//Trace(BText("Parsed expression :\n")+Unparsing(tre, "	 ", "|"));
  BSyntaxObject* result = NIL;
  if(realTree && !BParser::HasError())
  {
    List* tre = realTree->getTree();
    if((result = LeftEvaluateTree(tre)))
    {
      BGrammar::PutLast(this);
    }
    SAFE_DESTROY(tre,result);
  };
  PutLast(this);
  if(UnparseNodes() && result) 
  {
    result->PutExpression(expr);
  }
  return(result);
}


//--------------------------------------------------------------------
BSyntaxObject* Evaluate(const BText& expr)

/*! Evaluates a simple TOL expression, it's to say an expression
 *  free of commas.
 */
//--------------------------------------------------------------------
{
    BGrammar* gra = BGrammar::Last();
    BSyntaxObject* result = gra->EvaluateExpr(expr);
    return(result);
}



//--------------------------------------------------------------------
BList* MultyEvaluate(const BText& expr)

/*! Evaluates a commas separated set of simple TOL expressions.
 */
//--------------------------------------------------------------------
{
  BGrammar* gra    = GraAnything();
  BList*    result = NIL;
  BList*    aux    = NIL;
//InitTotalTime("MultyEvaluate");
//Std(BText("\nMultyEvaluate 0")+BTimer::Text());
  Tree* realTre = BParser::parsing(expr);
  List* tre = realTre->getTree();

//Std(BText("\nMultyEvaluate 1")+BTimer::Text()+"\n");
//Std(BParser::treWrite(tre,"  ")+"\n");
//Std(BParser::Unparse(tre, "  ", "\n"));

  if(!tre || BParser::HasError()) { return(NIL); }
  BToken*    tok    = BParser::treToken(tre);
  BTokenType type   = tok->TokenType();
  List*     branch = tre->cdr();
  BSyntaxObject* object;
  //BInt n =0; // unused
  BGrammar::PutLast(GraSet());

  if(type != SEPARATOR)
  {
  //Std(BText("\nMultyEvaluate NON SEPARATOR branch ")+ (n++) +" "+ BParser::Unparse(branch, "  ", "\n"));
    if(type == TYPE) 
    {
      BGrammar* newGrammar = BGrammar::FindByName(tok->Name(),false);
/*      
      BGrammar* newGrammar = NULL;
      BTypeToken* tt = (BTypeToken*)tok;
      if(tt->type_==BTypeToken::BSYSTEM)
      {
        newGrammar=TknFindGrammar(tok);
      }
      else if(tt->type_==BTypeToken::BSTRUCT)
      {
        newGrammar=GraSet();
      }
      else if(tt->type_==BTypeToken::BCLASS)
      {
        newGrammar=GraNameBlock();
      }
*/      
      if(newGrammar) { gra = newGrammar; }
    }
#   ifdef DO_PUT_EMS
    clock_t clk = clock();
#   endif
    if((object= gra->EvaluateTree(tre)))
    {
      gra = object->Grammar();
    //if(!object->System()) 
      LstFastAppend(result, aux, object); 
      object->Do();
      BGrammar::PutLast(gra);
#     ifdef DO_PUT_EMS
      if(BGrammar::UnparseNodes())
      {  
        object->PutEms((int)BTimer::ClockToMiliSecond(clock()-clk)); 
      }
#     endif
    }
  }
  else // (type==SEPARATOR)
  {
    while(branch)
    {
    //BText("\nMultyEvaluate SEPARATOR branch ");
      object = NIL;
      BToken* line = BParser::treToken(Tree::treNode(branch));
      if(line && (line->TokenType()==TYPE))
      {
        BGrammar* newGrammar = BGrammar::FindByName(line->Name(),false);
        if(newGrammar) { gra = newGrammar; }
      }
#     ifdef DO_PUT_EMS
      clock_t clk = clock();
#     endif
      if((object=gra->EvaluateTree(Tree::treNode(branch))))
      {
      //if(!object->System())
        LstFastAppend(result, aux, object);
        object->Do();
        gra = object->Grammar();
        BGrammar::PutLast(gra);
#       ifdef DO_PUT_EMS
        if(BGrammar::UnparseNodes())
        {  
          object->PutEms((int)BTimer::ClockToMiliSecond(clock()-clk)); 
        }
#       endif
      }
      branch = branch->cdr();
    }
  }
  realTre->Destroy();
//SumPartialTime;
  return(result);
}


//--------------------------------------------------------------------
BText BGrammar::Dump() const

/*! Returns a text with all important information about this grammar
 */
//--------------------------------------------------------------------
{
    return(BText("")+LstLength(Variables())+" Objects, "+
	   +LstLength(Operators())+" Functions, "+
	   +LstLength(Methods  ())+" Methods ");

}


//--------------------------------------------------------------------
BText BGrammar::DumpAll()

/*! Returns a text with all important information about all
 *  defined grammars
 */
//--------------------------------------------------------------------
{
    BList* graList = instances_;
    BText txt = "\nGRAMMAR INFORMATION";
    while(graList)
    {
	BGrammar* gra = (BGrammar*)Car(graList);
	txt += gra->Dump();
	graList = Cdr(graList);
    }
    return(txt);
}

