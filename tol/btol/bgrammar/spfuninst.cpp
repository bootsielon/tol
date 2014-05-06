/* spfuninst.cpp: Special functions of BGrammar of GNU/TOL language.
                  GNU/TOL Language.

   Copyright (C) 2003-2005  Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bspfun.h>
#include <tol/tol_bout.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bclass.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bpolmatgra.h>
#include <tol/tol_ois.h>
#ifdef __USE_TC__
#  include <tol/tol_bctmigra.h>
#  include <tol/tol_bctmsgra.h>
#  include <tol/tol_bctsrgra.h>
#endif
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bvmatgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_blanguag.h>
#include <tol/tol_bparser.h>
#include <tol/tol_bsymboltable.h>
#include <tol/tol_bnameblock.h>
#include <contrib/cint/tolCint.h>


BTraceInit("spfuninst.cpp");

//--------------------------------------------------------------------
BText BSpecialFunction::LstIdentify(const BList* lst,
			 const BText& openSign,
			 const BText& closeSign,
			 const BText& separatorSign)
    
/*! Transform in text any kind of list. This function work with plane
 *  lists and with recursive ones.
 */
//--------------------------------------------------------------------
{
    BBool	   first=BTRUE;
    BText	   txt(openSign);
    
    while(lst)
    {
	if(first) { first = BFALSE;       }
	else      { txt += separatorSign; }
	
	BSyntaxObject* car = (BSyntaxObject*)Car(lst);
	if(car)	{
	    if(IsList(car)) { 
		txt += BSpecialFunction::LstIdentify((BList*)car, openSign, closeSign, separatorSign); 
	    } else { 
		txt += car->Identify(); 
	    }
	} else { 
	    txt += "NIL"; 
	}
	
	lst = Cdr(lst);
    }
    txt += closeSign;
    
    return(txt);
}


//--------------------------------------------------------------------
BInt BSpecialFunction::NumBranches(const List* tre)

/*! Evaluate If expressions
 */
//--------------------------------------------------------------------
{
//    return(LstLength(Cdr(tre)));
    if (tre && tre->cdr())
	return (((List*)tre)->cdr()->length());
    else return 0;
}

//--------------------------------------------------------------------
BBool BSpecialFunction::TestNumArg(
  const BText& name, BInt min, BInt n, BInt max)

/*! Returns true if the number of branch in a tree if correct
 */
//--------------------------------------------------------------------
{
  BBool ok = BTRUE;
  if(n<min)
  {
    Error(BText(I2("Insuficient arguments in call to ",
                   "Número de argumentos insuficientes para ")) + name);
    ok = BFALSE;
  }
  if(max && (n>max))
  {
    Warning(I2("Too many arguments in call to ",
               "Demasiados argumentos para ") + name);
  }
  return(ok);
}


//--------------------------------------------------------------------
void BSpecialFunction::GetArg(BSet& set, 
                   const List* tre, 
                   BGrammar* graParam[], 
                   int numParam)
    
/*! Returns true if the number and types of branches in a tree is correct
 */
//--------------------------------------------------------------------
{
  BList* lst = NIL;
  BList* aux = NIL;
  BInt n = numParam;
  for(BInt i=0; i<n; i++)
  {
    BInt j = Minimum(i, numParam-1);
    BGrammar* gra = graParam[j];
    assert(gra);
    List* branch = Branch(tre,i+1);
    assert(branch);
    BSyntaxObject* arg = gra->EvaluateTree(branch);
    if(arg && ((gra==GraAnything()) || (arg->Grammar()==gra)))
    {
      LstFastAppend(lst,aux,arg);
    }
    else
    {
      Error(BParser::Unparse(branch)+
            I2(BText(" is not a ") + gra->Name() + " object.",
            BText(" no es un objeto de tipo ") + gra->Name()));
      DESTROY(lst);
      return;
    }
  }
  set.RobElement(lst);
}


//--------------------------------------------------------------------
BSyntaxObject* BSpecialFunction::TestResult(
  const BText&        name,
       BSyntaxObject* result,
  const List*        tre,
       BSyntaxObject* defect,
       BBool        warn)
    
/*! Sends an error message if result is NULL
 */
//--------------------------------------------------------------------
{
  if(!result) 
  {
    if(warn) 
    {
      Warning(I2(name+" function failed at : ",
                 BText("La función ")+name+" ha fallado en : ")+"\n"+
              BParser::Unparse(tre,"  ", "\n"));
    }
    return(defect);
  } else 
  {
//  result->Do();
    return(result);
  }
}

//--------------------------------------------------------------------
BSyntaxObject* BSpecialFunction::TestResult(
  const BText&         name,
       BSyntaxObject* result,
  const List*          tre,
       BSyntaxObject* defect,
       BBool          warn,
  const BText&         msg)
    
/*! Sends an error message if result is NULL
 */
//--------------------------------------------------------------------
{
  if(!result) 
  {
    if(warn) 
    {
      Warning(msg+
              I2(name+" function failed at : ",
                 BText("La función ")+name+" ha fallado en : ")+"\n"+
              BParser::Unparse(tre,"  ", "\n"));
    }
    return(defect);
  } else 
  {
//  result->Do();
    return(result);
  }
}



//--------------------------------------------------------------------
void BSpecialFunction::ArgumentFail(
  const BText & function, const BText & arg, const List* tre)
//--------------------------------------------------------------------
{
  Error(I2(BText("Fail evaluating argument '")+arg+"' at function "+function,
           BText("Fallo en la evaluación del argumento '")+arg+"' en la función "
           +function)+"\n"+BParser::Unparse(tre,"  ", "\n"));
}

//--------------------------------------------------------------------
// special functions evaluator
//--------------------------------------------------------------------

//--------------------------------------------------------------------
static BSyntaxObject* EvIf(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate If expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "If";
  BSyntaxObject* result = NIL;
  BSyntaxObject* condition = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 3))
  {
  //Std(BText("\nEvaluating ")+BParser::Unparse(tre, "  ", "\n"));
    List* b1 = Branch(tre,1);
    List* b2 = Branch(tre,2);
    List* b3 = Branch(tre,3);
    condition = GraReal()->EvaluateTree(b1);
    BGrammar::PutLast(gra);
    bool ok = false;
    if(condition)
    {
      if(condition->Grammar()==GraReal())
      {
        BDat cond_ = Dat(condition);
        if(cond_.IsKnown())
        {
          ok = true;
          BBool cond = cond_.Value()!=0;
          if(cond)
          { 
            result = gra->EvaluateTree(b2); 
          }
          else if (b3)
          { 
            result = gra->EvaluateTree(b3); 
          }
        }
        else 
        {
          Warning(I2("[If] condition function cannot return unknown values.",
                     "[If] la función de condición no puede devolver valores desconocidos."));
        }
      }
    }
    if(!ok)
    {
      result = gra->UnknownVar();
    }
    SAFE_DESTROY(condition,result);
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BFALSE);
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvCase(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Case expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Case";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  BList* stack = NIL;
  bool unknowCond = false;
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 0))
  {
    BBool cond = BFALSE;
    BInt n=0;
    for(; (n<=nb)&&(!cond)&&(!unknowCond); n+=2)
    {
      BSyntaxObject* obj = GraReal()->EvaluateTree(Branch(tre,n+1));
      BGrammar::PutLast(gra);
      if(obj)
      {
        BDat cond_ = Dat(obj);
        unknowCond = cond_.IsUnknown()!=0;
        cond =  !unknowCond && cond_.Value();
        stack = Cons(obj, stack);
      }
    }
    if(cond && (n<=nb))
    { 
      result = gra->EvaluateTree(Branch(tre,n)); 
    }
  }
  if(unknowCond)
  {
    assert(!result);
    Warning(I2("[Case] condition function cannot return unknown values.",
               "[Case] la función de condición no puede devolver valores desconocidos."));
    result = gra->UnknownVar();
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BFALSE);
  SAFE_DESTROY(stack,result);
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvFind(BGrammar* gra, const List* tre, BBool left )

/*! Evaluate Find expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Find";
  static BGrammar* graParam[1] = 
  { 
    GraText()
  };
  BSyntaxObject* result = NIL;
  BSet args; BSpecialFunction::GetArg(args, tre, graParam, 1);
  BText txt = "";
  if(args.Card()==1)
  {
	  txt = Text(args[1]);
	  result = gra->FindLocal(txt);
	  if(!result) { result = gra->FindVariable(txt); }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,gra->Defect(),BTRUE);
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvMakeGlobal(      BGrammar* gra,
				   const List* tre,
				         BBool left)

/*! Evaluate Find expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "MakeGlobal";
  if(CheckNonDeclarativeAction(_name_)) { return(NULL); }
    BSyntaxObject* result = NIL;
    BSyntaxObject* uTxt	= NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
    {
//	Std("\nEvMakeGlobal 1");
	result = gra->EvaluateTree(Branch(tre,1));
	if(result)
	{
	    BText name = result->Name();
//	    Std(BText("\nEvMakeGlobal for ")+name);
	    if(!name.HasName())
	    {
		Error(I2("Cannot make global an object without name.",
			 "No se puede hacer global un objeto sin nombre."));
		result = NIL;
	    }
	    else if(gra->FindVariable(name))
	    {
		Error(I2("Cannot make global the object ",
			 "No se puede hacer global el objeto ")+
		      name+I2(" of type ", " de tipo ") +gra->Name() +
		      I2(" because is already defined."," porque ya existe."));
		result = NIL;
	    }
	    else
	    {
    int add = BGrammar::SymbolTable().Add(result);
    if(add>=0) 
    { 
      result->PutHashed(true); 
  		result->PutGlobal();
    }
    const BSetFromFile* gs = result->GlobalSource();
    if(gs)
    {
      gs->AddMakeGlobal(result);
    }
    else
    {
		  Warning(I2("Cannot make global object out of a TOLfile. ",
			           "No se puede hacer global un objeto fuera de un fichero TOL. ")+
            name+I2(" will be deleted at local scope if it is not returned to global one.",
                    " será eliminada en el ámbito local si no se devuelve al global."));
    }
	    }
	}
    }
    result=BSpecialFunction::TestResult(_name_,result,tre,gra->Defect(),BTRUE);
    SAFE_DESTROY(uTxt,result);
    return(result);
}


// ///////////////////////////////////////////////////////////////////////////
static BSyntaxObject* EvEval(BGrammar* gra, const List* tre, BBool left)

/*! PURPOSE: Evaluate Eval expressions
*/
// ///////////////////////////////////////////////////////////////////////////
{
  static BText _name_ = "Eval";
  static BGrammar* graParam[1] = 
  { 
    GraText()
  };
    BSyntaxObject* result = NIL;
    BSet args; 
    BSpecialFunction::GetArg(args, tre, graParam, 1);
    if(args.Card()==1)
    {
      const BText& expr = Text(args[1]);
      if(gra==GraAnything())
      {
        int posBlank = expr.Find(" ");
        if(posBlank>0)
        {
          BText usedGraNam(expr,0,posBlank-1);
          BGrammar* usedGra = BGrammar::FindByName(usedGraNam,false);
          if(usedGra) 
          {  
            result = usedGra->EvaluateExpr(expr);
          }
        }
      }
	    if(!result)
      {
        result = gra->EvaluateExpr(expr);
      }
    }
    result=BSpecialFunction::TestResult(_name_,result,tre,gra->Defect(),BTRUE);
    return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvEvalArg(BGrammar* gra, const List* tre,BBool left)

/*! Evaluate EvalArg expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "EvalArg";
  static BGrammar* graParam[2] = 
  { 
    GraText(),
    GraSet()
  };
  BSyntaxObject* result = NIL;
  BSet args; BSpecialFunction::GetArg(args, tre, graParam, 2);
  if(args.Card()==2)
  {
    BText txt  = args[1]->Dump();
    txt = txt.SubString(1,txt.Length()-2);
    BStandardOperator* opr = (BStandardOperator*)gra->FindOperator(txt);
    if(opr)
    {
    //VBR: reverse list to pass arguments to Evaluator
      BList* arg = Set(args[2]).ToReversedList();
      result = opr->Evaluator(arg);
    }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,gra->Defect(),BTRUE);
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvGroup(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Group expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Group";
  static BGrammar* graParam[3] = 
  { 
    GraText(),
    GraSet(),
    GraAnything()
  };
  BSyntaxObject* result = NIL;

  BInt nb = BSpecialFunction::NumBranches(tre);
  BSet args; 
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 3))
  {  
    BSpecialFunction::GetArg(args, tre, graParam, nb);
  }
  if(args.Card()==nb)
  {
    BSyntaxObject* uTxt  = NIL;
    BSyntaxObject* uSet  = NIL;
    if(gra==GraAnything())
    {
      Error(I2("Cannot eval Group returning an object of type Anything.",
               "No se puede evaluar Group para devolver un objeto de tipo Anything."));
    }
    else
    {
      uTxt = args[1];
      uSet = args[2];
      if(uTxt && uSet)
      {
        BSet& set = Set(uSet);
        BText txt  = uTxt->Dump();
        txt = txt.SubString(1,txt.Length()-2);
        BStandardOperator* ope = (BStandardOperator*)gra->FindOperator(txt);
        if(ope)
        {
          if(set.Card()==0) 
          {
            if(nb==3)
            {
              if(gra==args[3]->Grammar()) { result = gra->New("",args[3]); }
              else
              {
                Error(I2("Group default value type should be ",
                         "El valor por defecto de Group debería ser de tipo ")+gra->Name()+
                      I2(" instead of "," en lugar de ")+args[3]->Grammar()->Name());
              }
            }
            else
            {
              Error(I2("Cannot evaluate Group over empty set without "
                       "a default value.",
                       "No se puede evaluar Group sobre un conjunto vacío "
                       "sin valor por defecto."));
            }
          }
          else  
          {
            BBool ok = BTRUE;
            BGrammar* g;
            for(BInt n=1; ok&&(n<=set.Card()); n++) 
            {
              g = set[n]->Grammar();
              if((gra!=GraAnything()) && 
                 !(ok = (g==gra))) 
              {
                Error(I2("An element for Group function is not an"
                         " object of type ",
                         "Algún elemento para la funcion Group no"
                         " es un objeto de tipo ")+ gra->Name()+":\n"+
                         set[n]->LexInfo());
              }
            }
            if(ok) 
            {
              if(set.Card()==1) 
              {
                result = gra->New("",set[1]);
              //result = set[1];
              } 
              else 
              {
              //VBR: reverse list to pass arguments to Evaluator
                BList* arg = Set(args[2]).ToReversedList();
              //Std(BText("\nGroup(")+uTxt->Dump()+",[["+set.Name()+"]])");
                result = ope->Evaluator(arg);
              }
            }
          }
        }
        else
        {
          Error(BText("Group : ") +txt +
                I2(" is not a " + gra->Name() + " operator.",
                " no es un operador de tipo " + gra->Name()));
        }
      }
    }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}


//--------------------------------------------------------------------
/*! Evaluate BinGroup expressions
 */
static BSyntaxObject* EvBinGroup(BGrammar* gra, const List* tre, BBool left)
//--------------------------------------------------------------------
{
  static BText _name_ = "BinGroup";
  BInt n;
  BSyntaxObject* result = NIL;
  BSyntaxObject* uTxt   = NIL;
  BSyntaxObject* uSet   = NIL;
  if(gra==GraAnything())
  {
    Error(I2("Cannot eval BinGroup returning an object of type Anything.",
             "No se puede evaluar BinGroup para devolver un objeto de tipo Anything."));
  }
  else
  {
    static BGrammar* graParam[2] = 
    { 
      GraText(),
      GraSet()
    };
  //Std(BText("\nEvBinGroup ")+BParser::Unparse(tre));
    BSet args; BSpecialFunction::GetArg(args, tre, graParam, 2);
    if(args.Card()==2) 
    {
      uTxt = args[1];
      uSet = args[2];
      if(uTxt && uSet) 
      {
        BText txt	 = uTxt->Dump();
				txt = txt.SubString(1,txt.Length()-2);
        BStandardOperator* ope = (BStandardOperator*)gra->FindOperator(txt);
        if(ope)
        {
	        BSet& set = Set(uSet);
	        BInt  N	  = set.Card();
	        if(N>0)	
          {
	          BBool ok = BTRUE;
	          for(n=1; ok&&(n<=N); n++) 
            {
		          if((gra!=GraAnything()) && 
		             !(ok = (set[n]->Grammar()==gra))) 
              {
		            Error(I2("An element for BinGroup function is not"
			                   " an object of type ", "Algún elemento "
			                   "para la funcion BinGroup no es un "
			                   "objeto de tipo ")+ gra->Name());
		          }
	          }
	          if(ok) 
            {
/*
            //VBR: This is the old behaviour in reverse order and 
            //using incremental references
	            result = gra->New("",set[N]);
              for(n = N-1; n>=1; n--)	
              {
		            BList* args = Cons(set[n],NCons(result));
		            result = ope->Evaluator(args);
		          }
*/
            //VBR: This is the new behaviour in direct order and 
            //breaking references in each step by copying partial results
            //This feature is in quarentine
	            result = set[1]->CopyContens();
              for(n = 2; n<=N; n++)	
              {
		            BList* args = Cons(result,NCons(set[n]));
		            BSyntaxObject* r = ope->Evaluator(args);
                result = r->CopyContens();
                if(result!=r)
                {
                  DESTROY(r);
                }
		          }
	          }
	        }
	        else 
          {
            result = gra->UnknownVar();
          }
        }
        else
        {
	        Error(_name_+BText(": ")+ txt +
	             I2(" is not a " + gra->Name() + " operator.",
		              " no es un operador de tipo " + gra->Name()));
        }
      }
    }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvField(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Field expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = BText("Field (")+I2("or","ó")+" -> )";
  BSyntaxObject* result = NIL;
  BSyntaxObject* uSet	= NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2))
  {
    if(left) { uSet = GraSet() ->LeftEvaluateTree(Branch(tre,1)); }
    else     { uSet = GraSet() ->EvaluateTree(Branch(tre,1)); }
    BGrammar::PutLast(gra);
    BText fieldName = BParser::treToken(Branch(tre,2))->Name();
    if(uSet)
    {
      BSet& set = Set(uSet);
      result = set.Field(fieldName);
    }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  if(!left) { SAFE_DESTROY(uSet,result); }
  return(result);
}

//--------------------------------------------------------------------
/*! Evaluate Element expressions
 */
static BSyntaxObject* EvRef(BGrammar* gra, const List* tre, BBool left)
//--------------------------------------------------------------------
{
  static BText _name_ = BText("$");
  static BText _str_list_ = 
  "  @Anything,@Code,@Date,@Polyn,@Ratio,@Real,@Complex,\n"
  "  @Matrix,@VMatrix,@Set,@NameBlock,@Text,@TimeSet,@Serie\n";
  static BText _error_any_eng = BText("Operator $ applies only over "
  "sets with one of these referential structures:\n") + _str_list_;
  static BText _error_any_esp = BText("El operador $ se aplica sólo sobre "
  "conjuntos con una de estas estructuras referenciales:\n") + _str_list_;

  assert(gra != NIL);
  BSyntaxObject* result = NIL;
  BSyntaxObject* uSet  = NIL;
  BSyntaxObject * uIndex = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  bool deleteUIndex = false;
  bool needCleanTree = false;
/*Std(BText("EvElement(")+gra->Name()+","+
      BParser::Unparse(tre,"","")+","+(int)left+")\n\n"+
      BParser::treWrite((List*)tre,"  ",false));  */
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
  {
  //Std(BText("\nEvElement tre=")+BParser::Unparse(tre));
    uSet = GraSet()->LeftEvaluateTree(Branch(tre,1));
    if (uSet) 
    {
      BSet& set = Set(uSet);
      BStruct* str = set.Struct();
      if(!str)
      {
        Error(I2("Cannot apply operator $ over a non structured ",
                 "No se puede aplicar el operador $ sobre "
                 "un conjunto sin estructura ")+"\n"+
              I2("It was expected a set with referential structure ",
                 "Se esperaba un conjunto con estructura ")+
              "@"+gra->Name()+I2(" or "," o ")+"@Anything");
      }
      else if(gra==GraAnything())
      {
        if(str==RefAnything() ||
           str==RefCode() ||
           str==RefDate() ||
           str==RefPolyn() ||
           str==RefRatio() ||
           str==RefReal() ||
           str==RefComplex() ||
           str==RefMatrix() ||
           str==RefVMatrix() ||
           str==RefSet() ||
           str==RefNameBlock() ||
           str==RefText() ||
           str==RefTimeSet() ||
           str==RefSerie())
        {
          result = set[1];
        }
        else
        {
          Error(I2("Cannot apply operator $ over structure ",
                   "No se puede aplicar el operador $ sobre la estructura ")+
                str->Name()+"\n"+
                I2(_error_any_eng,_error_any_esp));
        }
      }
      else if(str==RefAnything())
      {
        result = set[1];
      }
      else if((gra==GraCode()      && str==RefCode()) ||
              (gra==GraDate()      && str==RefDate()) ||
              (gra==GraPolyn()     && str==RefPolyn()) ||
              (gra==GraRatio()     && str==RefRatio()) ||
              (gra==GraReal()      && str==RefReal()) ||
              (gra==GraComplex()   && str==RefComplex()) ||
              (gra==GraMatrix()    && str==RefMatrix()) ||
              (gra==GraVMatrix()   && str==RefVMatrix()) ||
              (gra==GraSet()       && str==RefSet()) ||
              (gra==GraNameBlock() && str==RefNameBlock()) ||
              (gra==GraText()      && str==RefText()) ||
              (gra==GraTimeSet()   && str==RefTimeSet()) ||
              (gra==GraSerie()     && str==RefSerie()))
      {
        result = set[1];
      }
      else
      {
        Error(I2("Cannot apply operator $ over structure ",
                 "No se puede aplicar el operador $ sobre la estructura ")+
                 str->Name()+"\n"+
              I2("It was expected a set with referential structure ",
                 "Se esperaba un conjunto con estructura ")+
              "@"+gra->Name()+I2(" or "," o ")+"@Anything");
      }
    }
  }
  result = BSpecialFunction::TestResult(_name_, result, tre, NIL, BFALSE);
  BText resGraName = (result)?result->Grammar()->Name():"";
  if (result && gra!=GraAnything() && result->Grammar() != gra) 
  {
  	BSyntaxObject *cast = gra->Casting(result);
	  if(cast) result = cast;
	  else 
    {
	    BText msg(_name_);
	    Error(I2("Incompatible types in " ,"Tipos incompatibles en ") + 
		  msg + ": " + result->Identify() + I2(" is a ", " es de tipo ") +
		  result->Grammar()->Name() +
		  I2(" but not a ", ", pero no de tipo ") + gra->Name() + ".");
	    result = NIL;
	  }
  }
  return(result);
}

//--------------------------------------------------------------------
/*! Evaluate Element expressions
 */
static BSyntaxObject* EvElement(BGrammar* gra, const List* tre, BBool left)
//--------------------------------------------------------------------
{
  static BText _name_ = BText("Element (")+I2("or","ó")+" [ ] )";
  assert(gra != NIL);
  BSyntaxObject* result = NIL;
  BSyntaxObject* uSet  = NIL;
  BSyntaxObject * uIndex = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  bool deleteUIndex = false;
  bool needCleanTree = false;
/*Std(BText("EvElement(")+gra->Name()+","+
      BParser::Unparse(tre,"","")+","+(int)left+")\n\n"+
      BParser::treWrite((List*)tre,"  ",false));  */
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2))
  {
  //Std(BText("\nEvElement tre=")+BParser::Unparse(tre));
    if(left) uSet = GraSet()->LeftEvaluateTree(Branch(tre,1));
    else     uSet = GraSet()->EvaluateTree(Branch(tre,1));
    BGrammar::PutLast(gra);
    if (uSet) 
    {
      BSet& set = Set(uSet);
      List* b2 = Branch(tre,2);
      BToken* tok  = BParser::treToken(b2);
      if(tok->Name()[0]!='\"')
      {
        bool oldEnabled = BOut::Disable();
        int oldObjNum = BSyntaxObject::NSyntaxObject();
	      uIndex = GraReal()->EvaluateTree(b2);
        BGrammar::PutLast(gra);
        needCleanTree = true;
        deleteUIndex = (oldObjNum < BSyntaxObject::NSyntaxObject());
        if(oldEnabled) { BOut::Enable(); }
      }
	    if (uIndex) 
      {
		    if (uIndex->Grammar() != GraReal()) 
        {
		      BText msg("Unexpected result: Real Evaluate return an "
			              "object of type ");
		      msg += uIndex->Grammar()->Name();
		      msg += BText(". Please report this to "
				               "http://bugs.tol-project.org/");
		      Warning(msg);
        } 
        else 
        {
		      result = set.GetElement((BInt)Real(uIndex));
		    }
	    } 
      else 
      {
        int oldObjNum = BSyntaxObject::NSyntaxObject();
        if(needCleanTree) { BGrammar::CleanTreeCache(b2, true); }
		    uIndex = GraText()->EvaluateTree(b2);
        BGrammar::PutLast(gra);
        deleteUIndex = (oldObjNum < BSyntaxObject::NSyntaxObject());
		    if (uIndex) 
        {
		      if (uIndex->Grammar() != GraText()) 
          {
			      BText msg("Unexpected result: Text Evaluate return "
				              "an object of type ");
			      msg += uIndex->Grammar()->Name();
			      msg += BText(". Please report this to "
				                 "http://bugs.tol-project.org/");
			      Warning(msg);
		      } 
          else 
          {
			      result = set.GetElement(Text(uIndex).String());
		      }
        } 
        else 
        {
		      BText msg(_name_);
		      Error(I2(msg + ": invalid Set index type, must be "
			             "Real or Text", msg + ":Indice de conjunto "
			             "inválido, debe ser Real o Text"));
		    }
	    }
	  }
  }
  result = BSpecialFunction::TestResult(_name_, result, tre, NIL, BFALSE);
  BText resGraName = (result)?result->Grammar()->Name():"";
  if (result && gra!=GraAnything() && result->Grammar() != gra) 
  {
  	BSyntaxObject *cast = gra->Casting(result);
	  if(cast) result = cast;
	  else 
    {
	    BText msg(_name_);
	    Error(I2("Incompatible types in " ,"Tipos incompatibles en ") + 
		  msg + ": " + result->Identify() + I2(" is a ", " es de tipo ") +
		  result->Grammar()->Name() +
		  I2(" but not a ", ", pero no de tipo ") + gra->Name() + ".");
	    result = NIL;
	  }
  }
  if(uIndex && deleteUIndex) { SAFE_DESTROY(uIndex,result); }
  if(!left) { SAFE_DESTROY(uSet,result); }
  return(result);
}

//bool& BSyntaxObjectDisplayInfo();


//--------------------------------------------------------------------
static BSyntaxObject* EvPutValue(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate PutValue expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "PutValue";
  if(CheckNonDeclarativeAction(_name_)) { return(NULL); }
  // bool error = false; // unused
  BSyntaxObject* result = NIL;
  BGrammar *type = gra;
  BInt nb = BSpecialFunction::NumBranches(tre);
  //BSyntaxObjectDisplayInfo() = true;
  //BText unparsed = BParser::Unparse(tre,"","");
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2)) 
  {
    List* left  = Branch(tre,1);
    List* right = Branch(tre,2);
    result = gra->LeftEvaluateTree(left);
    if(result)
    {
      if(gra!=result->Grammar()) { type = result->Grammar(); }
      if(type == GraTimeSet()) 
      {
        BSpecialFunction::TestResult(_name_,NIL,tre,NIL,BTRUE,
                   I2("Cannot change a value of type ",
                      "No se puede cambiar un valor de tipo ") 
                   + type->Name());
        return(NIL);
      }
      BSyntaxObject* val = type->EvaluateTree(right);
      if(result && val && (val!=result)) 
      {
        if(type==GraReal()) {
          UDat(result)->PutContens(Dat(val)); }
        else if(type==GraComplex()) {
          UCmp(result)->PutContens(Cmp(val)); }
        else if(type==GraDate()) {
          UDate(result)->PutContens(Date(val)); }
        else if(type==GraText()) {
          UText(result)->PutContens(Text(val)); }
        else if(type==GraPolyn()) {
          UPol(result)->PutContens(Pol(val)); }
        else if(type==GraRatio()) {
          URat(result)->PutContens(Rat(val)); }
        else if(type==GraMatrix()) {
          UMat(result)->PutContens(Mat(val)); }
        else if(type==GraVMatrix()) {
          ((BUserVMat*)(result))->PutContens(VMat(val)); }
        else if(type==GraPolMat()) {
          UPolMat(result)->PutContens(PolMat(val)); }
        else if(type==GraSet()) {
          USet(result)->PutContens(Set(val)); }
        else if(type==GraCode()) {
          UCode(result)->PutContens(Code(val)); }
        else if(type==GraSerie()) {
          BUserTimeSerieDo* s1 = (BUserTimeSerieDo*)result;
          BUserTimeSerieDo* s2 = (BUserTimeSerieDo*)val;
          s1->PutContens(s2); }
        else if(type==GraNameBlock())  {
          BUserNameBlock* unb1 = (BUserNameBlock*)(result);
          BUserNameBlock* unb2 = (BUserNameBlock*)(val);
            unb1->PutContens(unb2->Contens()); }
        else {
          BSpecialFunction::TestResult(_name_,NIL,tre,NIL,BTRUE,
               I2("Incompatible types: ",
                  "Tipos incompatibles: ") + 
               result->Name() + I2(" is a ", " es de tipo ") +
               result->Grammar()->Name() + I2(" but not a ",
                 ", pero no de tipo ") + type->Name() + ".");
          return(NIL);
        }
        SAFE_DESTROY(val,result);
      }
    }
  }
  result = BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
//BSyntaxObjectDisplayInfo() = false;
  return(result);
}



//--------------------------------------------------------------------
static BSyntaxObject* EvPutName(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate PutName expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "PutName";
  if(CheckNonDeclarativeAction(_name_)) { return(NULL); }
  BGrammar::IncLevel();
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2))
    {
	BSyntaxObject* namObj = GraText()->EvaluateTree(Branch(tre,1));
  BGrammar::PutLast(gra);
	BSyntaxObject* obj	  = gra->LeftEvaluateTree(Branch(tre,2));
	if(namObj && obj)
	{
	    BText newName = Text(namObj);
	    BText oldName = obj->Name();
      BSyntaxObject* found = gra->FindVariable(newName);
	    if (found && found->Level()==BGrammar::Level()) {
		Error(I2("Cannot make ","No se puede hacer ") + 
		      "PutName : "+newName+
		      I2(" already exists", " existe todavía"));
	    } else {
		gra->DelObject(obj);
		obj->PutName(newName);
        if(BParser::DefaultParser()->Filter()->IsIdentifier(newName))
        {
		  gra->AddObject(obj);
        }
		result = obj;
	    }
	}
	else
	{
	    BSpecialFunction::TestResult(_name_,NIL,tre,NIL,BTRUE);
	}
	SAFE_DESTROY(namObj,obj);
    }
  BGrammar::DecLevel();
    return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvPutDescription(      BGrammar* gra,
               const List* tre, 
               BBool left)

/*! Evaluate PutDescription expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "PutDescription";
  static BText third_arg_error =
    I2("Optional third argument of PutDescription must be the name of a member "
       "of the class specified in second one",
       "El tercer argumento opcional de PutDescription debe ser el nombre de "
       "un miembro de la clase especificada en el segundo argumento");

//if(CheckNonDeclarativeAction(_name_)) { return(NULL); }
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 3))
  {
    BSyntaxObject* desObj = GraText()->EvaluateTree(Branch(tre,1));
    if(!desObj) { return(NULL); }
    BText desc = Text(desObj);
    BGrammar::PutLast(gra);
    BText tokenName = BParser::treToken(Branch(tre,2))->Name();
    BSyntaxObject* obj = NULL;
    if(tokenName[0]=='\"')
    {
      tokenName = tokenName.SubString(1,tokenName.Length());
      obj = FindClass(tokenName,-1);
      if(obj && nb==3)
      {
        BText memberName = BParser::treToken(Branch(tre,3))->Name();
        if(memberName[0]=='\"')
        {
          memberName = memberName.SubString(1,memberName.Length());
          BClass* cls = (BClass*)obj;
          if(!cls->PutMemberDescription(memberName, desc, false))
          {
            BSpecialFunction::TestResult(_name_,NIL,tre,NIL,BTRUE);
          }
          SAFE_DESTROY(desObj,obj);
          return(NULL);
        }
        else
        {  
          Error(third_arg_error);
        }
      } 
      else if(!obj && (nb==3))
      {
        Error(third_arg_error);
      }
      else if(!obj)
      {
        obj = FindStruct(tokenName);
        if(!obj) { obj = gra->LeftEvaluateExpr(tokenName); }
      }
    }
    else if (nb==3)
    {
      Error(third_arg_error);
    }
    if(!obj)
    {
      obj = gra->LeftEvaluateTree(Branch(tre,2));
    };
    BGrammar::PutLast(gra);
    if(obj)
    {
      obj->PutDescription(desc);
      if(obj->Grammar()==GraCode())
      {
        BUserCode* code = UCode(obj);
        if(code)
        {
          BOperator* found = code->Contens().Operator();
          if(found)
          {
            found->PutDescription(code->Description());
          }
        }
      }
      else if(obj->Mode()==BUSERFUNMODE)
      {
        BOperator* opr = (BOperator*)obj;
        BUserFunCode* code = opr->GetCode();
        if(code)
        {
          code->PutDescription(opr->Description());
        }
      }
    }
    else
    {
      BSpecialFunction::TestResult(_name_,NIL,tre,NIL,BTRUE);
    }
    SAFE_DESTROY(desObj,obj);
  }
  return(NULL);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvWhile(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate While expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "While";
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2))
    {
	BBool cond = BFALSE;
	BInt  n = 0;
	BSyntaxObject* uDat;
	List* tre1 = Branch(tre,1);
	List* tre2 = Branch(tre,2);
	do
	{
	    cond = BFALSE;
	    uDat = GraReal()->EvaluateTree(tre1);
      BGrammar::PutLast(gra);
	    if(uDat && Dat(uDat).IsKnown())
	    {
                if((cond = (Real(uDat)!=0)))
		{
		    result = GraAnything()->EvaluateTree(tre2);
        BGrammar::PutLast(gra);
		    if(result)
		    {
			result->Do();
			DESTROY(result);
		    }
		    else
		    {
			cond = BFALSE;
		    }
		}
	    }
	    SAFE_DESTROY(uDat,result);
	    n++;
	} while(cond);
    }
//result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(NIL);
}



//--------------------------------------------------------------------
static BSyntaxObject* EvCopy(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Copy expressions
*/
//--------------------------------------------------------------------
{
  static BText _name_ = "Copy";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
  {
    BGrammar::IncLevel();
    int stackPos = BGrammar::StackSize();
    BSyntaxObject* toCopy = gra->EvaluateTree(Branch(tre,1));
	  if(toCopy)
	  {
	    result = toCopy->CopyContens();
	    if(result!=toCopy)
	    {
		    SAFE_DESTROY(toCopy,result);
	    }	
    }
    BGrammar::DestroyStackUntil(stackPos, result);    
    BGrammar::DecLevel();
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}


//--------------------------------------------------------------------
BSyntaxObject* CopyReal(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BDat dat = Dat(obj);
  if(p.Size()) { dat = p[pos++]; }
  return(new BContensDat("",dat,""));
};

//--------------------------------------------------------------------
BSyntaxObject* CopyPolyn(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BPol pol = Pol(obj);
  pol.Aggregate();
  BInt j;
  if(p.Size()&&((pol.Size()!=1)||(pol[0].Coef()!=0)))
  {
	  for(j=0; j<pol.Size(); j++) { pol[j].PutCoef(p[pos++]); }
  }
  return(new BContensPol("",pol,""));
};

//--------------------------------------------------------------------
BSyntaxObject* CopyRatio(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BRat rat = Rat(obj);
  BPol num = rat.Numerator  ();
  BPol den = rat.Denominator();
  BInt j;
  if(p.Size())
  {
	  for(j=1; j<num.Size(); j++) { num[j].PutCoef(p[pos++]); }
	  for(j=1; j<den.Size(); j++) { den[j].PutCoef(p[pos++]); }
	  rat.PutNumerator  (num);
	  rat.PutDenominator(den);
  }
  return(new BContensRat("",rat,""));
};

//--------------------------------------------------------------------
BSyntaxObject* CopyMatrix(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BMat mat = Mat(obj);
  if(p.Size())
  {
	  for(BInt i=0; i<mat.Rows(); i++)
	  {
	    for(BInt j=0; j<mat.Columns(); j++)
	    {
		    mat(i,j) = p[pos++];
	    }
	  }
  }
  return(new BContensMat("",mat,""));
};

//--------------------------------------------------------------------
BSyntaxObject* CopyPolMat(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BPolMat P = PolMat(obj);
  int i,j,k,r=P.Rows(),c=P.Columns();
  if(p.Size())
  {
    for(i=0; i<r; i++)
	  {
	    for(j=0; j<c; j++)
	    {
        BPol& pol = P(i,j);
       pol.Aggregate();
        if(((pol.Size()!=1)||(pol[0].Coef()!=0)))
        {
	        for(k=0; k<pol.Size(); k++) { pol[k].PutCoef(p[pos++]); }
        }	    
	    }
	  }
  }
  return(new BContensPolMat("",P,""));
};

//--------------------------------------------------------------------
BSyntaxObject* CopyCode(BSyntaxObject* obj)
{
  BCode& cod = Code(obj);
  BUserFunction* opr = (BUserFunction*)cod.Operator();
  if(opr)
  {
    if(opr->Mode()==BBUILTINFUNMODE)
    {
      BCode newCod;
      newCod.Replicate(Code(obj));
      return(new BContensCode("",newCod,""));
    }
    else if(opr->Mode()==BUSERFUNMODE)
    {
      BUserFunction* usf = new BUserFunction("",opr->Grammar());
      BUserFunCode* result = usf->GetCode();
      usf->PutName(opr->Name());
      usf->PutDescription(opr->Description());
      usf->SetExpression(opr->Declaration(), opr->Definition()); 
      result->PutName(opr->Name());
      result->PutDescription(opr->Description());
      return(result);
    }
  }
  Error(BText("Cannot copy Code ")<<obj->Identify());
  return(NULL);
};
/*
//--------------------------------------------------------------------
BSyntaxObject* CopyNameBlock(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
  BUseerNameBlock* unb = (BUseerNameBlock*)obj
  BNameBlock mat = unb->Contens();
  if(p.Size())
  {
	  for(BInt i=0; i<mat.Rows(); i++)
	  {
	    for(BInt j=0; j<mat.Columns(); j++)
	    {
		mat(i,j) = p[pos++];
	    }
	}
    }
    return(new BContensMat("",mat,""));
};
*/

//--------------------------------------------------------------------
BSyntaxObject* CopySet(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos);

//--------------------------------------------------------------------
void DeepCopy(const BSet& sample,BSet& copy,const BArray<BDat>& p,BInt& pos)
{
  BList* lst = NIL;
  BList* aux = NIL;
  BInt i;
  for(i=1; i<=sample.Card(); i++)
  {
    if(sample[i])
    {
      BSyntaxObject* obj = NIL;
      BGrammar*	     gra = sample[i]->Grammar();
           if(gra==GraReal   ()) { obj = CopyReal   (sample[i],p,pos); }
      else if(gra==GraPolyn  ()) { obj = CopyPolyn  (sample[i],p,pos); }
      else if(gra==GraRatio  ()) { obj = CopyRatio  (sample[i],p,pos); }
      else if(gra==GraMatrix ()) { obj = CopyMatrix (sample[i],p,pos); }
      else if(gra==GraPolMat ()) { obj = CopyPolMat (sample[i],p,pos); }
      else if(gra==GraSet    ()) { obj = CopySet    (sample[i],p,pos); }
      else if(gra==GraCode   ()) { obj = CopyCode   (sample[i]);       }
      else		                   { obj = sample[i]->CopyContens ();    }
      if(obj && !obj->HasName())
      {
        obj->PutName(sample[i]->Name());
      }
      LstFastAppend(lst,aux,obj);
    }
  }
  copy.RobStruct(lst,sample.Struct(),sample.SubType());
  if(sample.HasIndexByName())
  {
    copy.SetIndexByName();
  }
};


//--------------------------------------------------------------------
BSyntaxObject* CopySet(BSyntaxObject* obj,const BArray<BDat>& p,BInt& pos)
{
    BSet& set = Set(obj);
    BSet	copy;
    DeepCopy(set,copy,p,pos);
    copy.PutSubType(set.SubType());
    copy.PutStruct(set.Struct());
    return(new BContensSet("",copy,""));
};

//--------------------------------------------------------------------
static BSyntaxObject* EvDeepCopy(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Copy expressions
*/
//--------------------------------------------------------------------
{
  static BText _name_ = "DeepCopy";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  if((gra==GraSet()) || (gra==GraAnything()))
  {
    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 3))
    {
      BGrammar::IncLevel();
      int stackPos = BGrammar::StackSize();
      BSyntaxObject* toCopy = GraSet()->EvaluateTree(Branch(tre,1));
	    if(toCopy && (toCopy->Grammar()==GraSet()))
	    {
        BUserSet*    uS = (BUserSet*)toCopy;
        BContensSet* res = new BContensSet;
        result = res;
        BArray<BDat> p;
        BInt pos = -1;
        if(nb==2)
        {
          BUserMat* uM = (BUserMat*)GraMatrix()->EvaluateTree(Branch(tre,2)); 
          if(uM && (uM->Grammar()==GraMatrix()))
          { 
            BMat& M = uM->Contens();
            p	= M.Data();
            pos = 0;
            bool multiple_instances = false;
            if(nb==3)
            {
              BUserDat* mi = (BUserDat*)GraReal()->EvaluateTree(Branch(tre,3));
              if(mi)
              multiple_instances = Real(mi)!=0;
            }
            if(!multiple_instances)
            {
              DeepCopy(uS->Contens(),res->Contens(),p,pos);
            }
            else
            {
              int i;
              int r = M.Rows();
              int c = M.Columns();
              res->Contens().PrepareStore(r);
              for(i=0; i<r; i++)
              {
                BContensSet* res_i = new BContensSet;
                DeepCopy(uS->Contens(),res_i->Contens(),p,pos); 
                res->Contens().AddElement(res_i);
              }
            }
          }
          SAFE_DESTROY(uM,result);
        }
        if(pos==-1) { DeepCopy(uS->Contens(),res->Contens(),p,pos); }
        SAFE_DESTROY(toCopy,result);
      }
      BGrammar::DestroyStackUntil(stackPos, result);    
      BGrammar::DecLevel();
    }
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvDo(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Do expressions
*/
//--------------------------------------------------------------------
{
  static BText _name_ = "Do";
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
    {
	result = gra->EvaluateTree(Branch(tre,1));
	if(result) {  result->Do();	 }
    }
//result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
    return(result);
}


// ///////////////////////////////////////////////////////////////////////////
static BSyntaxObject* EvReCalc(BGrammar* gra, const List* tre, BBool left)

/*! PURPOSE: Evaluate ReCalc expressions
*/
// ///////////////////////////////////////////////////////////////////////////
{
  static BText _name_ = "ReCalc";
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
    {
	result = gra->EvaluateTree(Branch(tre,1));
	if(result) {  result->ReCalc(); result->Do(); }
    }
    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
    return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvTOLPath(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate TOLPath expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "TOLPath";
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);
    BUserText* uTxt=NULL;
    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
    {
	uTxt = (BUserText*)GraText()->EvaluateTree(Branch(tre,1));
  BGrammar::PutLast(gra);
	if(uTxt)
	{
	    BText& path = Text(uTxt);
	    if(path.HasName())
	    {
/*
	if(path[0]=='\"')
	{
	  path = path.String()+1;
	  if(path.Last()=='\"') { path.PutLength(path.Length()-1); }
	}
*/
	//Std(BText("\nEvTOLPath(")+path+")");
		result = BSetFromFile::FindObject(path);
	    }
	}
    }
    SAFE_DESTROY(uTxt,result);
    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
    return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvGetObjectFromAddress(
  BGrammar* gra, const List* tre, BBool left)

/*! Evaluate TOLPath expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "GetObjectFromAddress";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  BUserText* uTxt=NULL;
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
  {
    uTxt = (BUserText*)GraText()->EvaluateTree(Branch(tre,1));
    BGrammar::PutLast(gra);
    if(uTxt)
    {
      BText& address = Text(uTxt);
      if(address.HasName())
      {
        result = BSyntaxObject::GetObjectFromAddress(address);
      }
    }
  }
  SAFE_DESTROY(uTxt,result);
//result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvDating(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate TOLPath expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Dating";
    BSyntaxObject* result = NIL;
    if((gra==GraTimeSet()) || (gra==GraAnything()))
    {
	BInt nb = BSpecialFunction::NumBranches(tre);
	if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
	{
	    BSyntaxObject* ser = NIL;
	    ser = GraSerie()->EvaluateTree(Branch(tre,1));
      BGrammar::PutLast(gra);
	    if(ser) { result = Tsr(ser)->Dating(); }
	    SAFE_DESTROY(ser,result);
	}
    }
    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
    return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvWrite(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate TOLPath expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Write";
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 2))
  {
    BText type = "U";
    BSyntaxObject* uTxt = GraText()->EvaluateTree(Branch(tre,1));
    BGrammar::PutLast(gra);
    if(uTxt && uTxt->Grammar()==GraText())
    {
      BText& txt = Text(uTxt);
      if(nb==2)
      {
        BSyntaxObject* uType = GraText()->EvaluateTree(Branch(tre,2));
        BGrammar::PutLast(gra);
        if(uType)
        {
          type = Text(uType);
          switch(type[0])
          {
            case 'E': Error  (txt); break;
            case 'W': Warning(txt); break;
            case 'U': Info   (txt); break;
            case 'S': Std    (txt); break;
            case 'T': Trace  (txt); break;
          }
          DESTROY(uType);
        }
      }
      else 
      {
        Info(txt);
      }
      DESTROY(uTxt);
    }
  }
  return(NIL);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvWriteLn(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate TOLPath expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "WriteLn";
  BInt nb = BSpecialFunction::NumBranches(tre);
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 2))
  {
    BText type = "U";
    BSyntaxObject* uTxt = GraText()->EvaluateTree(Branch(tre,1));
    BGrammar::PutLast(gra);
    if(uTxt && uTxt->Grammar()==GraText())
    {
      BText txt(Text(uTxt));
      txt += "\n";
      if(nb==2)
      {
        BSyntaxObject* uType = GraText()->EvaluateTree(Branch(tre,2));
        BGrammar::PutLast(gra);
        if(uType)
        {
          type = Text(uType);
          switch(type[0])
          {
            case 'E': Error  (txt); break;
            case 'W': Warning(txt); break;
            case 'U': Info   (txt); break;
            case 'S': Std    (txt); break;
            case 'T': Trace  (txt); break;
          }
          DESTROY(uType);
        }
      }
      else 
      {
        Info(txt);
      }
      DESTROY(uTxt);
    }
  }
  return(NIL);
}


int MHWS_put_extra(int id_mh_ws, int idx_so, BSyntaxObject * so);
int MHWS_get_extra(int id_mh_ws, int idx_so, BSyntaxObject * &so);

static
BSyntaxObject* EvMHWSPut(BGrammar* gra, const List* tre, BBool left)
{
  static BText _name_ = "MHWSPut";
  if(CheckNonDeclarativeAction(_name_)) { return(NULL); }
  BSyntaxObject *result = NULL;
  BSyntaxObject *arg;
  BInt nb = BSpecialFunction::NumBranches(tre);

  if (BSpecialFunction::TestNumArg(_name_, 3, nb, 3)) {
    int id_mh_ws, idx_obj;
    BDat * ptr_dat;
    
    arg = GraReal()->EvaluateTree(Branch(tre,1)); /* ID_WS */
    BGrammar::PutLast(gra);
    if (!arg) {
      BSpecialFunction::ArgumentFail(_name_, "ID_WS", tre);
      return NULL;
    }
    ptr_dat = &(((BUserDat*)arg)->Contens());
    if (!ptr_dat->IsKnown()) {
      DESTROY(arg);
      Error(I2("Unknown value in ID_WS argument at function MHWSPut",
               "Valor desconocido en argumento ID_WS de la función MHWSPut")+
            BParser::Unparse(tre,"  ", "\n"));
      return NULL;
    }
    id_mh_ws = int(ptr_dat->Value());
    DESTROY(arg);
    arg = GraReal()->EvaluateTree(Branch(tre,2)); /* IdxObj */
    BGrammar::PutLast(gra);
    if (!arg) {
      BSpecialFunction::ArgumentFail(_name_, "IdxObj", tre);
      return NULL;
    }
    ptr_dat = &(((BUserDat*)arg)->Contens());
    if (!ptr_dat->IsKnown()) {
      DESTROY(arg);
      Error(I2("Unknown value in IdxObj argument at function MHWSPut",
               "Valor desconocido en argumento IdxObj de la función MHWSPut")+
            BParser::Unparse(tre,"  ", "\n"));
      return NULL;
    }
    idx_obj = int(ptr_dat->Value());
    DESTROY(arg);
    arg = gra->EvaluateTree(Branch(tre,3));
    BGrammar::PutLast(gra);
    if (!arg) {
      BSpecialFunction::ArgumentFail(_name_, "NewValue", tre);
      return NULL;
    }
    int ret = MHWS_put_extra(id_mh_ws, idx_obj-1, arg);
    // DESTROY(arg); /* release if no more references to arg */
    switch (ret) {
        case -1:
          Error(I2("MHWSPut, IdxObj argument: object's index in MetropolisHastings workspace is invalid or out of range.",
                   "MHWSPut, argumento IdxObj: el índice de objeto es inválido en espacio de trabajo del MetropolisHasting")+BParser::Unparse(tre,"  ", "\n"));
          DESTROY(arg);
          break;
        case -2:
          Error(I2("MHWSPut, ID_WS argument: invalid MetropolisHastings workspace identifier",
                   "MHWSPut, argumento ID_WS: identificador de espacio de trabajo del MetropolisHasting inválido")+BParser::Unparse(tre,"  ", "\n"));
          DESTROY(arg);
          break;
        case 0:
          result = arg;
          break;
    }
  } else {
    /* unparse because BSpecialFunction::TestNumArg does not do it */
    Error(BParser::Unparse(tre,"  ", "\n"));
  }
  return result;
}

static
BSyntaxObject* EvMHWSGet(BGrammar* gra, const List* tre, BBool left)
{
  static BText _name_ = "MHWSGet";
  BSyntaxObject *result = NULL;
  BSyntaxObject *arg;
  BInt nb = BSpecialFunction::NumBranches(tre);

  if (BSpecialFunction::TestNumArg(_name_, 2, nb, 2)) {
    int id_mh_ws, idx_obj;
    BDat * ptr_dat;
    
    arg = GraReal()->EvaluateTree(Branch(tre,1)); /* ID_WS */
    BGrammar::PutLast(gra);
    if (!arg) {
      BSpecialFunction::ArgumentFail(_name_, "ID_WS", tre);
      return NULL;
    }
    ptr_dat = &(((BUserDat*)arg)->Contens());
    if (!ptr_dat->IsKnown()) {
      DESTROY(arg);
      Error(I2("Unknown value in ID_WS argument at function MHWSGet",
               "Valor desconocido en argumento ID_WS de la funcion MHWSGet")+
            BParser::Unparse(tre,"  ", "\n"));
      return NULL;
    }
    id_mh_ws = int(ptr_dat->Value());
    DESTROY(arg);
    arg = GraReal()->EvaluateTree(Branch(tre,2)); /* IdxObj */
    BGrammar::PutLast(gra);
    if (!arg) {
      BSpecialFunction::ArgumentFail(_name_, "IdxObj", tre);
      return NULL;
    }
    ptr_dat = &(((BUserDat*)arg)->Contens());
    if (!ptr_dat->IsKnown()) {
      DESTROY(arg);
      Error(I2("Unknown value in IdxObj argument at function MHWSGet",
               "Valor desconocido en argumento IdxObj de la funcion MHWSGet")+
            BParser::Unparse(tre,"  ", "\n"));
      return NULL;
    }
    idx_obj = int(ptr_dat->Value());
    DESTROY(arg);
    int ret = MHWS_get_extra(id_mh_ws, idx_obj-1, arg);
    switch (ret) {
        case -1:
          Error(I2("MHWSGet, IdxObj argument: object's index in MetropolisHastings workspace is invalid or out of range.",
                   "MHWSGet, argumento IdxObj: el índice de objeto es inválido en espacio de trabajo del MetropolisHasting")+BParser::Unparse(tre,"  ", "\n"));
          break;
        case -2:
          Error(I2("MHWSGet, ID_WS argument: invalid MetropolisHastings workspace identifier",
                   "MHWSGet, argumento ID_WS: identificador de espacio de trabajo del MetropolisHasting inválido")+BParser::Unparse(tre,"  ", "\n"));
          break;
        case 0:
          result = arg;
          break;
    }
  } else {
    /* unparse because BSpecialFunction::TestNumArg does not do it */
    Error(BParser::Unparse(tre,"  ", "\n"));
  }
  return result;
}

//--------------------------------------------------------------------
static BSyntaxObject* EvIsUnknownObject(BGrammar* gra, 
					const List* tre, 
					BBool left)
{
  static BText _name_ = "IsUnknown";
    BSyntaxObject* result = NIL;
    BInt nb = BSpecialFunction::NumBranches(tre);

    if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
    {
	BSyntaxObject* bob = GraAnything()->EvaluateTree(Branch(tre,1));
  BGrammar::PutLast(gra);
	if(bob)
	{
	    BBool known = BTRUE;
	    
	    if(bob->Grammar()==GraReal())
		known = Dat(bob).IsKnown();
	    else if(bob->Grammar()==GraCode())
		known = (bob->Name()!="UnknownCode") ? BTRUE : BFALSE;
	    else if(bob->Grammar()==GraDate())
		known = (bob->Dump()!="Unknown") ? BTRUE : BFALSE;
	    else if(bob->Grammar()==GraComplex())
		known = Cmp(bob).IsKnown();
	    else if(bob->Grammar()==GraMatrix())
		known = Mat(bob).IsKnown();
	    else if(bob->Grammar()==GraPolyn())
		known = Pol(bob).IsKnown();
	    else if(bob->Grammar()==GraRatio())
		known = Rat(bob).IsKnown();
	    else if(bob->Grammar()==GraSerie())
		known = (bob->Dump()!="UnknownSerie") ? BTRUE : BFALSE;
	    /* Text and TimeSet always have known values (empty, but known) */
	    
	    if(known)
		result = GraReal()->FindConstant("0");
	    else result = GraReal()->FindConstant("1");
	}
    }

    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
    return(result);
}

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
void BSpecialFunction::AddInstance(const BText&	           name,
                                   const BText&	           arguments,
                                   const BText&	           description,
                                         BSpecialEvaluator function)
//--------------------------------------------------------------------
{
//  Std(BText("\nAdding special funcion instance ")+name+
//	" ("+instances_.Size() +")");
    BSpecialFunction* spf = new BSpecialFunction
	(name,arguments,description,function);
  instances_[spf->Name()] = spf;
}

//--------------------------------------------------------------------
void BSpecialFunction::AddLeftInstance(const BText&            name,
                                       const BText&            arguments,
                                       const BText&            description,
                                             BSpecialEvaluator function)
//--------------------------------------------------------------------
{
//Std(BText("\nAdding special funcion left instance ")+name+
//    " ("+instances_.Size()+","+leftInstances_.Size()+")");
  BSpecialFunction* spf = new BSpecialFunction
			      (name,arguments,description,function);
  instances_	  [spf->Name()] = spf;
  leftInstances_[spf->Name()] = spf;
}


//--------------------------------------------------------------------
static BSyntaxObject* EvMemberArg
//--------------------------------------------------------------------
(
  const BText& _name_,
  BGrammar* gra, 
  List* branch2, 
  BToken* arg2,
  const BText& memberName,
  const BText& ownerName,
  BText& errMsg,
  BSyntaxObject* result
)
{
//Std(BText("\nTRACE EvMemberArg ")+ownerName+"::"+memberName+"\n"); 
  BTokenType tt = arg2->TokenType();
  if(result && branch2->cdr())
  {
    if(tt==FUNCTION)
    {
      if(result->Grammar()!=GraCode())
      {
        BText fullName = ownerName+_name_+memberName;
        BText graName  = "UNKNOWN";
        if(result->Grammar()) { graName = result->Grammar()->Name(); }
        errMsg = fullName+
                 I2(" is not a function but an object of type ", 
                    " no es una función sino un objeto de tipo ")+
                 graName;
        result = NULL;
      }
      else
      {
        BUserCode* uCode = UCode(result);
        BOperator* opr   = GetOperator(uCode);
        if(opr)
        {
          result = opr->Evaluate(branch2->cdr());
          DESTROY(opr);
        }
      }
    }
    if(result && (tt==TYPE)&&(result->Mode()==BSTRUCTMODE))
    {
      BStruct* str = (BStruct*)result;
      BSyntaxObject* instance = str->Function()->Evaluate(branch2->cdr());
      if(instance)
      {
        result = instance;
        DESTROY(str);
      }
    }
    if(result && (tt==TYPE)&&(result->Mode()==BCLASSMODE))
    {
      BClass* cls = (BClass*)result;
      
      BSyntaxObject* instance = NULL;
      List* b2cdr = branch2->cdr();
      List* arg2 = NULL;
      if(b2cdr)
      {
        if(b2cdr->car() && !b2cdr->car()->IsListClass())
        {
          arg2 = b2cdr;
        }
        else
        {
          arg2 = (List*)(b2cdr->car());  
        }
      } 
      instance = GraNameBlock()->EvaluateTree(arg2);
      if(instance)
      {  
        result = instance;
        BUserNameBlock* unb = NULL;
        if(result && (result->Grammar()==GraNameBlock()))
        {
          unb = (BUserNameBlock*)result;
        }
        const BClass* unbCls = (!unb)?NULL:unb->Contens().Class();
        if(!unb || !unbCls->InheritesFrom(cls))
        {
          errMsg = I2("It was expected an instance of Class ",
                   "Se esperaba una instancia de Class ") +cls->Name();
          DESTROY(result);
          result=NULL;
        }
        DESTROY(cls);
      }
    }
  }
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvMember(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Member expressions 
     Set::PublicName
     NameBlock::PublicName
     Class::StaticMember
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "::";
/* * /
    BText ups = BParser::Unparse(tre, "  ");
  //Std(ups+"\n"); 
  //Std(BText("\nEvMember branch1='")+BParser::Unparse(branch1,"  ")+"'\n"); 
  //Std(BText("\nEvMember branch2='")+BParser::Unparse(branch2,"  ")+"'\n"); 
    int nObject0 = BSyntaxObject::NSyntaxObject();
    Std(BText("\nTRACE EvMember BEGIN NObject=")+nObject0+" tre='"+ups+"'\n"); 
/* */
  BSyntaxObject* result = NIL;
  int stackPos = BGrammar::StackSize();
  BInt nb = BSpecialFunction::NumBranches(tre);
  BText errMsg;
  BSyntaxObject* uns = NULL;
  bool needsDeleteUns = false;
  if(BSpecialFunction::TestNumArg(_name_, 2, nb, 2))
  {
    List* branch1 = Branch(tre,1);
    List* branch2 = Branch(tre,2);
/* * /
    printf((BText("\nTRACE EvMember branch1=")+BParser::treWrite(branch1,"  ")+"'\n").String()); 
    printf((BText("\nTRACE EvMember branch2=")+BParser::treWrite(branch2,"  ")+"'\n").String()); 
/* */
    int nObjOld = BSyntaxObject::NSyntaxObject();
    BToken* arg1 = BParser::treToken(branch1);
    if(!branch1->cdr())
    {
      if(arg1->TokenType()==ARGUMENT)
      {
        uns = GraNameBlock()->FindOperand(arg1->Name(),false);
        if(!uns)
        {
          uns = GraSet()->FindOperand(arg1->Name(),false);
        }
      }
      else if(arg1->TokenType()==TYPE)
      {
        BTypeToken* tt = (BTypeToken*)arg1;
        uns = FindClass(tt->Name(),1); 
      }
    }
    if(!uns)
    {
      uns = GraAnything()->LeftEvaluateTree(branch1,false);
      if(uns)
      {
        BGrammar* uns_gra = uns->Grammar();
        if(uns && (uns_gra!=GraNameBlock()) && (uns_gra!=GraSet()))
        {
          int mode = uns->Mode();
          if(mode!=BCLASSMODE)
          {
            Error(I2("Evaluating expression ",
                     "Evaluando la expresión ")+
                     gra->Name()+" '"+BParser::Unparse(tre)+"'\n"+
                  I2("Left term is not a valid Set or NameBlock but a ",
                     "El término a la izquierda no es un Set ni un NameBlock sino un ")+
                     uns_gra->Name()+" '"+BParser::Unparse(branch1)+"'\n");
            DESTROY(uns);
            return(NULL);
          }
        }
      }
    }
    int nObjNew = BSyntaxObject::NSyntaxObject();
    needsDeleteUns = (uns!=NULL) && (nObjNew>nObjOld);
    BToken* arg2 = BParser::treToken(branch2);
    if(uns && arg2)
    {
      const BText& memberName = arg2->Name();
      if(uns->Mode()==BCLASSMODE)
      {
        BClass* cls = (BClass*)uns;
        result = cls->FindStatic(memberName,false);
        BSyntaxObject* r = EvMemberArg
         (_name_, gra, branch2, arg2, memberName,cls->Name(), errMsg, result);
        DESTROY(result);
        result = r;
      }
      else if(uns->Grammar()==GraSet())
      {
        BUserSet* uSet = (BUserSet*)uns;
        result = uSet->Contens().GetElement(memberName);
      }
      else if(uns->Grammar()==GraNameBlock())
      {
        BNameBlock& ns = NameBlock(uns);
        if((memberName[0]=='_')&&(memberName[1]=='.'))
        {
          result = ns.PrivateMember(memberName);
        //if(result) { result = result->CopyContens(); }
        }
        else if((memberName[0]=='_'))
        {
          errMsg = I2("Cannot access to private member ",
                      "No se puede acceder al miembro privado ")+memberName +
                   I2(" of NameBlock ",
                      " del NameBlock ")+ns.Name();
        }
        else
        {
          result = ns.PublicMember(memberName);
        }
        BSyntaxObject* r = EvMemberArg
          (_name_, gra, branch2, arg2, memberName, ns.Name(), errMsg, result);
        DESTROY(result);
        result = r;
      }
    }
  }
  if(errMsg.HasName())
  {
    assert(!result);
    Error(I2("Evaluating expression ",
             "Evaluando la expresión ")+"'"+
             BParser::Unparse(tre)+"'\n"+errMsg);
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  if(needsDeleteUns) { SAFE_DESTROY(uns, result); }
//SAFE_DESTROY(uns, result); 
  BGrammar::DestroyStackUntil(stackPos, result);    
//if(result) Std(BText("\nTRACE EvMember result:'")+result->Identify()+"' numRefs="+result->NRefs()+"\n"); 
//int nObject1 = BSyntaxObject::NSyntaxObject();
//int nObjectDif = nObject1-nObject0;
//Std(BText("\nTRACE EvMember END NObject=")+nObject1+" dif= "+nObjectDif+" tre='"+ups+"'\n"); 
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvGlobalFind(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Member expressions ::GlobalName
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "::";
  BSyntaxObject* result = NIL;
  BInt nb = BSpecialFunction::NumBranches(tre);
  List* branch = Branch(tre,1);
  BToken* arg = BParser::treToken(branch);
  const BText& globalName = arg->Name();
  BText errMsg;
  if(gra!=GraAnything())
  {
    result = gra->FindVariable(globalName);   
  }
  else 
  {
    result = BGrammar::Last()->FindVariable(globalName);
    int gid;
    for(gid=BGI_Anything+1; !result && gid<BGI_LastId; gid++)
    {
      BGrammar* g = BGrammar::Gra((BGrammarId)gid);
      if(g!=BGrammar::Last())
      {
        result = g->FindVariable(globalName);   
      }
    }
  }
  if(errMsg.HasName())
  {
    assert(!result);
    Error(I2("Evaluating expression ",
             "Evaluando la expresión ")+"'"+
             BParser::Unparse(tre)+"'\n"+errMsg);
  }
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BTRUE);
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvClass(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Case expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Class";
  BSyntaxObject* result = BClass::Evaluate(tre);
  result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BFALSE);
  return(result);
}

//--------------------------------------------------------------------
static BSyntaxObject* EvAvoidErrNonDecAct(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Case expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "AvoidErr.NonDecAct";
  BInt nb = BSpecialFunction::NumBranches(tre);
  BSyntaxObject* result = NULL;
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
  {
    bool oldUseModule = BOis::RunningUseModule();
    BOis::SetRunningUseModule(false);
    result = gra->EvaluateTree(Branch(tre,1));
    BOis::SetRunningUseModule(oldUseModule);
    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BFALSE);
  }
  return(result);
}


//--------------------------------------------------------------------
static BSyntaxObject* EvCint_export_to_tol(BGrammar* gra, const List* tre, BBool left)

/*! Evaluate Case expressions
 */
//--------------------------------------------------------------------
{
  static BText _name_ = "Cint.export_to_tol";
  BInt nb = BSpecialFunction::NumBranches(tre);
  BSyntaxObject* result = NULL;
  if(BSpecialFunction::TestNumArg(_name_, 1, nb, 1))
  {
    bool oldUseModule = BOis::RunningUseModule();
    BSyntaxObject* uNam = GraText()->EvaluateTree(Branch(tre,1));
    if(uNam && (uNam->Grammar()==GraText()))
    {
      const BText& name = Text(uNam);
      int ptr = 0;
      if(gra==GraReal())
      {
        BDat cnt = Cint_double(Cint_calc(name));
/*
        g = Cint_calc(name);
        bool ok = true;
        switch (g.type) 
        {
          case 100: cnt = g.obj.d; break;
          case 105: cnt = g.obj.i; break;
          default:  ok = false;
        }
        if(!ok)
        {
          Error(I2("Unknown CINT numerical type ",
                   "Tipo CINT numérico desconocido ")<<g.type);
        }
        else
*/
        {
          result = new BContensDat("",cnt,"");
        }
      }
      else if(gra==GraMatrix())
      {
        int rows = -1, columns = -1;
        double* buffer = NULL;
        rows = Cint_int(Cint_calc(name+".rows_"));
        columns = Cint_int(Cint_calc(name+".columns_"));
        buffer = (double*)Cint_int(Cint_calc(BText("(int)(")+name+".buffer_)"));
        if(rows>0 && columns>0 && buffer!=NULL)
        {
          BMat cnt(rows, columns, (BDat*)buffer);  
          result = new BContensMat("",cnt,"");
        }
      }
      else if(gra==GraText())
      {
        int length = -1;
        char* buffer = NULL;
        length = Cint_int(Cint_calc(name+".length_"));
        buffer = (char*)Cint_int(Cint_calc(BText("(int)(")+name+".buffer_)"));
        if(length>0 && buffer!=NULL)
        {
          BText cnt(buffer, length);  
          result = new BContensText("",cnt,"");
        }
      }
      else
      {
        Error(BText("[")+_name_+"] "+
              I2("Not allowed TOL type ","Tipo TOL no permitido ")+
              gra->Name());
      }
      if(result)
      {
        result->PutName(name);
        BGrammar::AddObject(result);
      }
      DESTROY(uNam);
    }
    result=BSpecialFunction::TestResult(_name_,result,tre,NIL,BFALSE);
  }
  return(result);
}

//--------------------------------------------------------------------
static bool BSpecialFunction_IsInitialized()
//--------------------------------------------------------------------
{
  static bool initialized_ = false;
  if(initialized_) { return(true); }
  initialized_ = true;
  return(false);
}


//--------------------------------------------------------------------
bool BSpecialFunction::Initialize()
//--------------------------------------------------------------------
{
  if(BSpecialFunction_IsInitialized()) { return(false); }

  SetEmptyKey(instances_,     NULL);
  SetEmptyKey(leftInstances_, NULL);
    
  AddInstance("PutName",
  I2("(Text newName, Anything object)",
     "(Text nuevoNombre, Anything objeto)"),
  I2("Puts a new name to an object",
     "Pone un nuevo nombre a un objeto."
     "El uso de PutName no debe interaccionar nunca con el reconocedor sintáctico. "
     "No se debe cambiar el nombre de variables usadas en el lenguaje, ni locales "
     "ni globales ni miembros. Los efectos secundarios de PutName son inevitables "
     "si se hace un mal uso de esta funcionalidad.\n"
     "Digamos que sólo debe cambiar el nombre de elementos de conjuntos no accesibles "
     "desde el lenguaje y por alguno de estos motivos:\n"
     " * A efectos decorativos, para que salgan los títulos en gráficas, tablas, etc.\n"
     " * Para poder reconocer los elementos de conjuntos sin tener que usar estructuras.\n"
     " * Para indexar por nombre y poder hacer luego búsquedas rápidas por nombre.\n"
     " * Ocasionalmente es posible querer cambiar el nombre de una variable local que "
     "ya no va a ser usada en el ámbito de creación pero que es la salida de un EvalSet "
     "o un For y va a formar parte de un Set. Es lo mismo que cambiar el nombre después "
     "y puede ser más rápido y cómodo.\n"
     "Ejemplo de uso: \n"
     "Set s = [[2,\"a\",y2011]];\n"
     "Real PutName(\"s1\",s[1]);\n"
     "Text PutName(\"s2\",s[2]);\n"
     "Date PutName(\"s3\",s[3]);\n"
     "\n"),
  EvPutName);

  AddInstance("PutDescription",
  I2("(Text newDescription, Anything object [, Text member])",
     "(Text nuevaDescripción, Anything objeto [, Text miembro])"),
  I2("Puts a new description to an object. "
     "You also can modify description of a Struct or a Class by "
     "passing its name between quotes.",
     "Pone una nueva descripción a un objeto. Es posible modificar "
     "la descripción de un Struct o un Class pasándo el nombre de "
     "la misma entre comillas dobles")+
  I2("Optional third argument of PutDescription must be the name of a member "
     "of the class specified in second one",
     "El tercer argumento opcional de PutDescription debe ser el nombre de "
     "un miembro de la clase especificada en el segundo argumento"),
  EvPutDescription);

  AddInstance("PutValue",
  I2("(Anything newValue, Anything object)",
     "(Anything nuevoValor, Anything objeto)"),
  I2("Puts a new value to an object. "
     "It can not be applied to series neither to time sets.",
     "Pone un nuevo valor a un objeto. "
     "No se puede aplicar a series ni a conjuntos temporales."),
     EvPutValue);

  AddInstance(":=",
  I2("object := newValue {Anything object, Anything newValue}",
     "objeto := nuevoValor {Anything objeto, Anything nuevoValor}"),
  I2("Puts a new value to an object. "
     "It can not be applied to series neither to time sets.",
     "Pone un nuevo valor a un objeto. "
     "No se puede aplicar a series ni a conjuntos temporales."),
     EvPutValue);

  AddLeftInstance("Field",
  I2("(Set s, Text fieldName)",
     "(Set s, Text nombreDeCampo)"),
  I2("Returns a field of a structured set.Also it can be used the "
     "equivalent operator ->\n\n"
     "Example:\n"
     "Struct MyStruct(Anything field1, Anything field2);\n "
     "Set s = MyStruct(a,b);\n"
     "Anything x = Field(s,field1)\n"
     "Anything y = s->field2;\n\n"
     "Result:\n"
     "x == a\n"
     "y == b",
     "Devuelve un campo de un conjunto estructurado. También se puede usar "
     "el operador equivalente ->\n\n"
     "Ejemplo:\n"
     "Struct MiEstructura(Anything campo1, Anything campo2);\n "
     "Set s = MiEstructura(a,b);\n"
     "Anything x = Field(s,field1)\n"
     "Anything y = s->campo2;\n\n"
     "Resultado:\n"
     "x == a\n"
     "y == b"),
     EvField);

  AddLeftInstance("->",
  I2("s->fieldName {Set s, Text fieldName}",
     "s->nombreDeCampo {Set s, Text nombreDeCampo}"),
  I2("Returns a field of a structured set.Also it can be used the "
     "equivalent function Field\n\n"
     "Example:\n"
     "Struct MyStruct(Anything field1, Anything field2);\n "
     "Set s = MyStruct(a,b);\n"
     "Anything x = Field(s,field1)\n"
     "Anything y = s->field2;\n\n"
     "Result:\n"
     "x == a\n"
     "y == b",
     "Devuelve un campo de un conjunto estructurado. También se puede usar "
     "la función equivalente Field\n\n"
     "Ejemplo:\n"
     "Struct MiEstructura(Anything campo1, Anything campo2);\n "
     "Set s = MiEstructura(a,b);\n"
     "Anything x = Field(s,field1)\n"
     "Anything y = s->campo2;\n\n"
     "Resultado:\n"
     "x == a\n"
     "y == b"),
     EvField);


  AddLeftInstance("Element",
  "(Set s, Real n)",
  I2("Returns the n-th element of a set. Also it can be used "
     "the brackets []\n\n"
     "Example:\n"
     "Set s = SetOfAnything(a,b);\n"
     "Anything x = Element(s,1);\n"
     "Anything y = s[2];\n\n"
     "Result:\n"
     "x == a\n"
     "y == b",
     "Devuelve el enésimo elemento de un conjunto. También se puede "
     "usar los corchetes []\n\n"
     "Ejemplo:\n"
     "Set s = SetOfAnything(a,b);\n"
     "Anything x = Element(s,1);\n"
     "Anything y = s[2];\n\n"
     "Resultado:\n"
     "x == a\n"
     "y == b"),
  EvElement);

  AddLeftInstance("$",
  "(Set s)",
  I2("Returns the referenced object used as a referencer set, i.e.,"
     " a set with a referential structure called as character '@' "
     "followed by a native TOL type as "
     "@Real, @Date, @Anything, @Code, ...\n"
     "You can create a referencer set as\n"
     "  @<type> R = [[ <type_ref> ]];\n"
     "where <type_ref> is a reference of an object of the specified type, i.e.,"
     "the name of an object, an element of a Set, a member of a NameBlock, or "
     "any TOL expression returning one of those.",
     "Devuelve el objeto referenciado por un conjunto referenciador, esto es, "
     "un conjunto con una estructura espcial nombrada como el caracter '@' "
     "seguido por el nombre de un tipo nativo de TOL, como "
     "@Real, @Date, @Anything, @Code, ...\n"
     "Se puede crear un conjunto referencial\n"
     "  @<type> R = [[ <type_ref> ]];\n"
     "donde <type_ref> es unaa referencia de un objeto del tipo especificado, o sea,"
     "el nomb re de un objeto, un elemento de un Set, un mimebro of a NameBlock, or "
     "any TOL expression returning one of those."
     )+".\n"+
     I2("Examples:\n","Ejemplos:\n")+
     "  Set s = SetOfAnything(1,2,\"txt\");\n"+
     "  @Real     R1 = [[ s[1] ]];\n"+
     "  @Anything R2 = [[ s[2] ]];\n"+
     "  @Text     R3 = [[ s[3] ]];\n"+
     "  Real x = $R1-1;\n"+
     "  Real y = $R2;\n"+
     "  Text z = $R3;\n"+
     "  Real y := 5;\n"+
     "  Real $R1 := 6;\n",
  EvRef);

  AddLeftInstance("#E#",
  "(Set s, Real n)",
  I2("Returns the umpteenth element of a set. Also it can be used "
     "the function Element\n\n"
     "Example:\n"
     "Set s = SetOfAnything(a,b);\n"
     "Anything x = Element(s,1);\n"
     "Anything y = s[2];\n\n"
     "Result:\n"
     "x == a\n"
     "y == b",
     "Devuelve el enésimo elemento de un conjunto. También se puede "
     "usar la función Element\n\n"
     "Ejemplo:\n"
     "Set s = SetOfAnything(a,b);\n"
     "Anything x = Element(s,1);\n"
     "Anything y = s[2];\n\n"
     "Resultado:\n"
     "x == a\n"
     "y == b"),
  EvElement);

  AddInstance("Eval",
  I2("(Text expression)",
     "(Text expresión)"),
  I2("Returns the result of evaluating an expression.\n\n"
     "Example:\n"
     "Anything x = Eval(\"a+b\");\n\n"
     "Result:\n"
     "x : a+b"
     ,
     "Devuelve el resultado de evaluar una expresión.\n\n"
     "Ejemplo:\n"
     "Anything x = Eval(\"a+b\");\n\n"
     "Resultado:\n"
     "x : a+b"),
  EvEval);


  AddInstance("EvalArg",
  I2("(Text function, Set arguments)",
     "(Text funcion, Set argumentos)"),
  I2("Returns the result of evaluating function with given arguments.\n\n"
     "Example:\n"
     "Anything x = EvalArg(\"+\",[[a,b]]);\n\n"
     "Result:\n"
     "x : a+b",
     "Devuelve el resultado de evaluar una función con los argumentos dados.\n\n"
     "Ejemplo:\n"
     "Anything x = EvalArg(\"+\",[[a,b]]);\n\n"
     "Resultado:\n"
     "x : a+b"),
  EvEvalArg);


  AddInstance("MakeGlobal",
  I2("(Anything object)",
     "(Anything objeto)"),
  I2("Gives global visibility to an object. Accepts also an expression\n\n",
     "Da visibilidad global a un objeto. También acepta una expresión\n\n"),
  EvMakeGlobal);


  AddLeftInstance("Find",
  I2("(Text name)",
     "(Text nombre)"),
  I2("Returns the object whose name is the argument.\n\n"
     "Example:\nAnything x = Find(\"a\");\n\n"
     "Result:\n"
     "x == a",
     "Devuelve el objeto cuyo nombre es el argumento.\n\n"
     "Ejemplo:\n"
     "Anything x = Find(\"a\");\n\n"
     "Resultado:\n"
     "x == a"),
  EvFind);

  AddInstance("Group",
  I2("(Text function, Set s [, Anything defaultForEmpty])",
     "(Text función, Set s [, Anything defaultForEmpty])"),
  I2("Evaluates a function taking as arguments the elements of a set.\n\n"
     "Example:\n"
     "Set s = SetOfAnything(a,b,c,d);\n"
     "Anything x = Group(\"+\",s);\n\n"
     "Result:\n"
     "x == a+b+c+d",
     "Evalúa una función tomando como argumentos los elementos de un "
     "conjunto.\n\n"
     "Ejemplo:\n"
     "Set s = SetOfAnything(a,b,c,d);\n"
     "Anything x = Group(\"+\",s);\n\n"
     "Resultado:\n"
     "x == a+b+c+d"),
  EvGroup);


  AddInstance("BinGroup",
  I2("(Text function, Set s)",
     "(Text función, Set s)"),
  I2("Evaluates cumulatively a function of two arguments on the elements "
     "of a set.\n\n"
     "Example:\n"
     "Set s = SetOfAnything(a,b,c,d);\n"
     "Anything x = BinGroup(\"+\",s);\n\n"
     "Result:\n"
     "x == a+b+c+d",
     "Evalúa acumulativamente una función de dos argumentos sobre "
     "los elementos de un conjunto.\n\n"
     "Ejemplo:\n"
     "Set s = SetOfAnything(a,b,c,d);\n"
     "Anything x = BinGroup(\"+\",s);\n\n"
     "Resultado:\n"
     "x == a+b+c+d"),
  EvBinGroup);


  AddInstance("While",
  I2("(Real condition, Anything whatToDo)",
     "(Real condición, Anything quéhacer)"),
  I2("While it is fulfilled a given condition, evaluates an expression."
     "In such expression it should be to change the value of the condition "
     "so as not to fall in an infinite cycle. It returns no value due to it's "
     "a non declarative entity. So it cannot be assigned to a variable nor "
     "used as operand nor argument of any expression.",
     "Mientras se cumpla una condición dada, evalúa una expresión."
     "En dicha expresión se debe cambiar el valor de la condición "
     "para no caer en un ciclo infinito. No retorna ningún objeto por tratarse "
     "de una entidad no declarativa. Por tanto no se puede asignar a una "
     "variable ni usarse como operando ni argumento de ninguna expresión."),
  EvWhile);


  AddInstance("If",
  I2("(Real condition, Anything then [, Anything else])",
     "(Real condición, Anything entonces [, Anything siNo])"),
  I2("If the given condition is fulfilled is returned the second argument."
     "If this is not fulfilled and there is a third argument, is returned "
     "this last. In other case returns nothing.",
     "Si la condición dada se cumple se devuelve el segundo argumento."
     "Si ésta no se cumple y hay un tercer argumento, se devuelve éste "
     "último. En otro caso no devuelve nada."),
  EvIf);


  AddInstance("IF",
  I2("(Real condition, Anything then [, Anything else])",
     "(Real condición, Anything entonces [, Anything siNo])"),
  I2("If the given condition is fulfilled is returned the second argument."
     "If this is not fulfilled and there is a third argument, is returned "
     "this last. In other case returns nothing.",
     "Si la condición dada se cumple se devuelve el segundo argumento."
     "Si ésta no se cumple y hay un tercer argumento, se devuelve éste "
     "último. En otro caso no devuelve nada."),
  EvIf);


  AddInstance("Case",
  I2("(Real condition1, Anything result1, "
     "[Real condition2, Anything result2, ... ])",
     "(Real condición1, Anything resultado1, "
     "[Real condición2, Anything resultado2, ... ])"),
  I2("If is fulfilled the first condition, returns the first result."
     "In other case, if is fulfilled the second condition returns the "
     "second result; and thus successively. "
     "If is not fulfilled no condition, is not returned any result.",
     "Si se cumple la primera condición, devuelve el primer resultado."
     "En otro caso, si se cumple la segunda condición devuelve el segundo "
     "resultado; y así sucesivamente. "
     "Si no se cumple ninguna condición, no se devuelve ningún resultado."),
  EvCase);


  AddInstance("Copy",
    "(Anything obj)",
  I2("Makes a copy of an object of any type except Code, TimeSet and "
     "infinite Series",
     "Hace una copia de un objeto de cualquier tipo a excepción de "
     "funciones, conjuntos temporales, y series infinitas, los cuales son "
     "imposibles de reproducir dado su carácter abstracto."),
  EvCopy);

  AddInstance("DeepCopy",
    "(Set sample [, Matrix matrix, Real oneInstanceByEachRow=False])",
  I2("Builds a set with the same deep shape that a given sample."
     "substituting optionaly number data by cells on given matrix. "
     "If optional argument 'oneInstanceByEachRow' is false or not submitted, "
     "the argument must be a matrix with so cells as number of "
     "numeric elements in sample set."
     "If optional argument 'oneInstanceByEachRow' is true, then it "
     "will be created a set of sets with an instance for each row of "
     "given matrix, and the number of columns will be equal to the number "
     "of numeric elements in sample set.",
	   "Construye un conjunto semejante al ejemplo dado en el que "
	   ", opcionalmente, se sustituyen los datos numéricos por las celdas de "
	   "la matriz dada. "
     "Si el argumento opcional 'oneInstanceByEachRow' es falso o no se "
     "cumplimenta, el argumento matriz debe ser una matriz fila con tantos "
	   "datos como datos numericos haya en el conjunto ejemplo. "
     "Si el argumento opcional 'oneInstanceByEachRow' es cierto, entonces "
     "se devolverá un conjunto de conjuntos con una instancia semejante "
     "al ejemplo para cada fila de la matriz, por lo que el número de "
     "columnas será el de mismo que el de datos numéricos en el conjunto "
     "de ejemplo."),
  EvDeepCopy);


  AddInstance("Do",
  I2("(Anything obj)",
     "(Anything obj)"),
  I2("Forzes evaluation of an expression breaking lazzy evaluation mode.",
     "Fuerza la evaluación de una expresión rompiendo el modo de evaluación "
     "'lazzy' o pereceso que es el modo por defecto en TOL y que consiste en "
     "postponer la evaluación de las expresiones al máximo, de forma que "
     "sólo sean evaluadas las estrictamente necesarias."),
  EvDo);


  AddInstance("ReCalc",
  I2("",
     ""),
  I2("Exclusively for internal use",
     "Exclusivamente para uso interno"),
  EvReCalc);

  AddInstance("IsUnknownObject",
  I2("(Anything var)","(Anything var)"),
  I2("Returns 1 whether \"var\" has an unknown value.",
     "Devuelve 1 si \"var\" tiene un valor desconocido."),
  EvIsUnknownObject);

  AddLeftInstance("TOLPath",
  I2("(Text path)",
     "(Text camino)"),
  I2("Returns the TOL object specified by it TOLPath, that correspond to "
     "the path file where it was created follow by a list of indexes. \n"
     "For sample, 'c:/proof1.tol,4,7' its the seventh element of the fourth "
     "element created in the file 'c:/proof1.tol'.",
     "Devuelve el objeto TOL especificado por su TOLPath, que corresponde al "
     "camino del fichero en el que fue creado seguido de una lista de "
     "índices. \n"
     "Por ejemplo 'c:/prueba1.tol,4,7' es el séptimo elemento del "
     "cuarto elemento creado en el fichero 'c:/prueba1.tol'."),
  EvTOLPath);

  AddLeftInstance("GetObjectFromAddress",
  I2("(Text path)",
     "(Text camino)"),
  I2("Returns the TOL object specified by its internal address obtained by "
     "mean of calling GetAddressFromObject.",
     "Devuelve el objeto TOL especificado por su dirección interna que se "
     "obtiene llamando a la función GetAddressFromObject."),
  EvGetObjectFromAddress);

  AddLeftInstance("Dating",
  I2("(Serie ser)",
     "(Serie ser)"),
  I2("Returns a reference to the dating of a serie",
     "Devuelve una referencia al fechado de una serie"),
  EvDating);


  AddInstance("Write",
  I2("(Text string [, Text messageType='U'])",
     "(Text cadena [, Text messageType='U'])"),
  I2("Writes a text in the standard output.\n"
     "The optional argument <messageType> indicates the type of message:\n"
     "  'E' : Errors \n" 
     "  'W' : Warnings \n" 
     "  'S' : System Information\n" 
     "  'U' : User Information\n" 
     "  'T' : Tracing information for debuging\n" 
     "Returns no object.",
     "Escribe una cadena en la salida estándar.\n"
     "El argumento opcional <messageType> indica el tipo de :\n"
     "  'E' : Errores \n" 
     "  'W' : Avisos (Warning) \n" 
     "  'S' : Información de Sistema\n" 
     "  'U' : Información del Usuario\n" 
     "  'T' : Información de trazado para depuración\n" 
     "No devuelve ningún objeto."),
  EvWrite);

  AddInstance("WriteLn",
  I2("(Text string [, Text messageType='U'])",
     "(Text cadena [, Text messageType='U'])"),
  I2("Writes a text in the standard output and adds a line jump.\n"
     "The optional argument <messageType> indicates the type of message:\n"
     "  'E' : Error \n" 
     "  'W' : Warning \n" 
     "  'S' : System Information\n" 
     "  'U' : User Information\n" 
     "  'T' : Tracing information for debuging\n" 
     "Returns no object.",
     "Escribe una cadena en la salida estándar y añade un salto de línea.\n"
     "El argumento opcional <messageType> indica el tipo de :\n"
     "  'E' : Error \n" 
     "  'W' : Avisos (Warning) \n" 
     "  'S' : Información de Sistema\n" 
     "  'U' : Información del Usuario\n" 
     "  'T' : Información de trazado para depuración\n" 
     "No devuelve ningún objeto."),
  EvWriteLn);

  
  AddInstance("MHWSPut",
              "(Real ID_WS, Real IdxObj, Anything NewValue)",
              I2("Replace, in the Metropolishastings workspace identified by 'ID_WS', the object at position 'IdxObj' with the value 'NewValue'.\n"
                 "Returns 'NewValue'",
                 "Reemplaza, en el espacio de trabajo del 'MetropolisHastings' identificado por 'ID_WS', el objeto que está en la posición 'IdxObj' por el valor 'NewValue'.\n"
                 "Retorna 'NewValue'"),
              EvMHWSPut);
  
  AddInstance("MHWSGet",
              "(Real ID_WS, Real IdxObj)",
              I2("Returns, in the Metropolishastings workspace identified by 'ID_WS', the object at position 'IdxObj'",
                 "Retorna, en el espacio de trabajo del 'MetropolisHastings' identificado por 'ID_WS', el objeto que está en la posición 'IdxObj'"),
              EvMHWSGet);
              
  AddLeftInstance("::",
  I2("(NameBlock nb, Text memberName)",
     "(NameBlock nb, Text nombreDeMiembro)"),
  I2("Returns a public member of a NameBlock set.\n\n"
     "Example:\n"
     "Set MySN = NameBlock [[Real _aux=1, Real xprtd=_aux+2]];\n "
     "Real a = MySN::xprtd;\n"
     "This line returns error because members begining with _ are private:"
     "Real b = MySN::_aux;\n",
     "Devuelve un miembro público de un NameBlock.\n\n"
     "Ejemplo:\n"
     "Set MySN = NameBlock [[Real #aux=1, Real xprtd=.aux+2]];\n "
     "Real a = MySN::xprtd;\n"
     "Esta línea devuelve un error porque los miembros que comienzan por _ son privados:"
     "Real b = MySN::_aux;\n"),
     EvMember);

  AddLeftInstance(" :: ",
  I2("(Text globalName)",
     "(Text nombreGlobal)"),
  I2("Returns a global variable or function.\n\n"
     "Example:\n",
     "Devuelve una variable o función global.\n\n"
     "Ejemplo:\n")+
     "Real ::Tolerance;\n",
     EvGlobalFind);

  AddLeftInstance("Class",
     " Class name [: parentClass1, ...] { members }",
  I2("Creates a user defined class as an specific API of NameBlock",
     "Crea una clase definida por el usuario como una API específica de NameBlock"),
     EvClass);

  AddInstance("AvoidErr.NonDecAct",
     "(Anything expression)",
  I2("Avoid error message about not declarative actions while you are creating "
     "an OIS module. Use with great care and only if you really want to load "
     "during OIS not run these activities.",
     "Evita el mensaje de error sobre acciones no declarativas mientras "
     "se está creando un módulo OIS. Úsese con mucho cuidado y sólo si realmente "
     "se quiere que durante la carga del OIS no se ejecuten dichas acciones."),
     EvAvoidErrNonDecAct);

 BText cint_url_link();

  AddInstance("Cint.export_to_tol",
     "(Text name)",
     I2("Exports global CINT variables to current TOL scope.\n"
     "CINT variables must have a valid name in TOL and C languages.\n"
     "At this moment these are the available TOL types :\n",    
     "Exporta variables CINT globales al ámbito actual de TOL\n"
     "Las variables TOL deben tener un nombre válido en los lenguajes "
     "TOL y C\n"
     "Los tipos TOL admitidos para la importación son los siguientes:\n")+
     "  TOL       CINT\n"
     "  Real      double\n"
     "  Matrix    class Matrix\n"
     "  Text      class Text\n"+
     I2("You can view the declaration of corresponding C/C++ types at ",    
     "Puede ver la declaración de los correspondientes tipos C/C++ en ")+
     "\nhttps://www.tol-project.org/browser/tolp/trunk/tol/"
     "stdlib/general/TolCint/tol_cint_casting.cpp"+
     cint_url_link(),
     EvCint_export_to_tol);


  return(true);
}

