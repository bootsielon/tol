/* setgra.cpp: Set grammars functions of GNU/TOL language.
               GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bsetgra.h>
#include <tol/tol_init.h>
#include <tol/tol_tree.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bsys.h>
#include <tol/tol_classify.h>
#include <tol/tol_gslmat.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bparser.h>
#include <tol/tol_boper.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btxtgra.h>
#include <tol/tol_bdtegra.h>
#include <tol/tol_bdatgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btsrgrp.h>
#include <tol/tol_bstruct.h>
#include <tol/tol_bclass.h>

#if defined(__USE_TC__)
#  include <tol/tol_bctmsgra.h>
#  include <tol/tol_bctmigra.h>
#endif /* __USE_TC__ */

#include "llkarma.h"

#ifdef WIN32
#define popen _popen
#define pclose _pclose
#endif

//static const char* bayesVersionId =

//--------------------------------------------------------------------
// static variables
//--------------------------------------------------------------------
BTraceInit("setgra.cpp");


//--------------------------------------------------------------------
TOL_API BSet EvalSet(const BText& expr, const BSet& defVal) 
//--------------------------------------------------------------------
{
  int stackPos = BGrammar::StackSize();
  BSyntaxObject* obj = GraText()->EvaluateExpr(expr);
  BSet result = defVal;
  BGrammar::IncLevel();
  if(obj && (obj->Grammar()==GraText())) 
  { 
    result = Set(obj);    
  }
  BGrammar::DestroyStackUntil(stackPos, obj);    
  DESTROY(obj);
  BGrammar::DecLevel();
  return(result);
}

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BSet>::Do() 
//--------------------------------------------------------------------
{ 
  if(!this->flags_.calculated_ && !this->flags_.calculating_)
  {
    this->flags_.calculating_ = true;
    BSet& c = Contens(); 
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

template<>
BGrammar* BGraContensBase<BSet>::ownGrammar_   = NIL;

DefineContensCommonOperators(BSet, "Set");


//Static Names and Descriptions to be used in built-in functions
//PREFIX_ must be defined in a different way in each 
// .cpp that uses these macroes to avoid repeated instantiations
// linker errors
#define FNDParam1(NAME)                 PrefixFNDParam1(ESTIM_, NAME);
#define FNDParam(NAME, DESC)            PrefixFNDParam(ESTIM_, NAME, DESC);
#define NewFND(TYPE, NAME, OBJ)         PrefixNewFND(TYPE, ESTIM_, NAME, OBJ)
#define NewSetFND(NAME, ELEM, STR, SUB) PrefixNewSetFND(ESTIM_, NAME, ELEM, STR, SUB)



  void ErrCannotModify(const BText& member, const BSyntaxObject* constant) 
  { 
    Error(I2("Cannot modify <","No se puede modificar <") + member +
          I2("> of constant object ","> del objeto constante ") + 
          constant->Identify()); 
  }

//--------------------------------------------------------------------
class BSetAppend: public BExternalOperator
//--------------------------------------------------------------------
{
public:
  BSetAppend()
  : BExternalOperator
    (
      "Append", GraSet(), "Set Set Real", NIL, 2, 3,
      "(Set set1, Set set2, [Real incrementalIndexByName=False])",
		  I2("Attaches the elements of set2 to the end of set1 without "
       "eliminating repeated elements and preserving the order.\n"
       "If incrementalIndexByName is true then the index of names "
       "will be incremented if original set had an index and new "
       "elements are compatible with old ones; or created if set "
       "was null"  ,
	     "Concatena los elementos del conjunto set1 al final de set2 "
       "sin eliminar elementos repetidos y conservando el orden.\n"
       "Si incrementalIndexByName es cierto entonces el índice de "
       "nombres, se mantendrá incrementalmente, siempre y cuando "
       "ya existiera previamente y los nuevos elementos sean "
       "compatibles, o bien se creará un índice nuevo si se parte "
       "del conjunto vacío."),
		  BOperClassify::SetAlgebra_
    )
  {
  }

 ~BSetAppend() {}

  BSyntaxObject* Evaluator(BList* arg) const
  { 
  //InitTotalTime("BSetAppend::Evaluator");
    if(CheckNonDeclarativeAction("Append")) { return(NULL); }
    BList* lst = arg;
    bool incrementalIndexByName = false;
    BUserSet* uSet1 = USet(lst->Car()); lst = lst->Cdr();
    if(uSet1->IsConstant()) { 
      ErrCannotModify("value",uSet1);
      return(NULL);
    }
    BUserSet* uSet2 = USet(lst->Car()); lst = lst->Cdr();
    if(lst && lst->Car())
    {
      BUserDat* uDat3 = UDat(lst->Car());
      incrementalIndexByName = uDat3->Contens()!=0.0;
    }
    uSet1->Contens().Append(uSet2->Contens(),incrementalIndexByName);
    SAFE_DESTROY(arg, uSet1);
  //SumPartialTime;
    return(uSet1);
  }
 private:
  DeclareClassNewDelete(BSetAppend);
};

//--------------------------------------------------------------------
class BSetRemove: public BExternalOperator
//--------------------------------------------------------------------
{
public:
  BSetRemove()
  : BExternalOperator
    (
      "Remove", GraSet(), "Set Real Real", NIL, 2, 3,
      "(Set set, Real n [, Real deleteOld=True])",
		  I2("Removes the n-th element of a set and tries to delete it "
         "from memory if deleteOld is true. Returns the own set "
         "with the rest of elements."
         "If the set was indexed by name then the indes will be updated.",
	       "Elimina el n-esimo elemento de un conjunto e intenta "
         "borrar el viejo de la memoria si deleteOld es cierto. "
         "Devuelve el propio conjunto con el resto de elementos."
         "Si el conjunto estaba indexado por nombre el índice será "
         "actualizado"),
		  BOperClassify::SetAlgebra_
    )
  {
  }

 ~BSetRemove() {}

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    if(CheckNonDeclarativeAction("Remove")) { return(NULL); }
    BList* lst = arg;
    bool incrementalIndexByName = false;
    BUserSet* uSet = USet(lst->Car()); lst = lst->Cdr();
    if(uSet->IsConstant()) { 
      ErrCannotModify("value",uSet);
      return(NULL);
    }
    BUserDat* uDat = UDat(lst->Car()); lst = lst->Cdr();
    bool do_deletion = true;
    if(lst)
    {
      Deprecated("Optional argument deleteOld of function Set Remove "
                 "will be eliminated taken true value");
      BUserDat* uDel = UDat(lst->Car()); 
      do_deletion = uDel->Contens().Value()!=0.0;
    }
    uSet->Contens().Remove(
     (int)(uDat->Contens().Value())-1,
     do_deletion);
    SAFE_DESTROY(arg, uSet);
    return(uSet);
  }
 private:
  DeclareClassNewDelete(BSetRemove);
};

//--------------------------------------------------------------------
class BSetReplace: public BExternalOperator
//--------------------------------------------------------------------
{
public:
  BSetReplace()
  : BExternalOperator
    (
      "Replace", GraSet(), "Set Real Anything Real", NIL, 3, 4,
      "(Set set, Real n, Anything new [, Real deleteOld])",
		  I2("Replaces the n-th element of a set by a another new one "
         "and tries to delete the old one from memory if deleteOld "
         "is true. Returns the own set with the new changed element."
         "If the set was indexed by name then the indes will be updated." ,
	       "Reemplaza el n-ésimo elemento de un conjunto por otro "
         "nuevo e intenta borrar el viejo de la memoria si deleteOld "
         "es cierto. Devuelve el propio conjunto con el elemento "
         "cambiado."
         "Si el conjunto estaba indexado por nombre el índice será "
         "actualizado"),
		  BOperClassify::SetAlgebra_
    )
  {
  }

 ~BSetReplace() {}

  BSyntaxObject* Evaluator(BList* arg) const
  { 
    if(CheckNonDeclarativeAction("Replace")) { return(NULL); }
    BList* lst = arg;
    bool incrementalIndexByName = false;
    BUserSet* uSet = USet(lst->Car()); lst = lst->Cdr();
    BUserDat* uDat = UDat(lst->Car()); lst = lst->Cdr();
    BSyntaxObject* uNew = (BSyntaxObject*)(lst->Car()); lst = lst->Cdr();
    bool do_deletion = true;
    if(lst)
    {
      Deprecated("Optional argument deleteOld of function Set Replace "
                 "will be eliminated taken true value");
      BUserDat* uDel = UDat(lst->Car()); 
      do_deletion = uDel->Contens().Value()!=0.0;
    }
    uSet->Contens().Replace(
      (int)(uDat->Contens().Value())-1, 
      uNew,
      do_deletion);
    SAFE_DESTROY(arg, uSet);
    return(uSet);
  }
 private:
  DeclareClassNewDelete(BSetReplace);
};


//--------------------------------------------------------------------
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
template<>
void BGraContensBase<BSet>::InitInstances()
{
    BTraceInit("BGraContens<BSet>::InitInstances");
    BSystemSet* empty1_ = new BSystemSet
    ("EMPTY", BSet::Unknown(), I2("Empty set", "Conjunto vacío."));
    BSystemSet* empty2_ = new BSystemSet
	  ("Empty", BSet::Unknown(), I2("Empty set", "Conjunto vacío."));
    OwnGrammar()->PutDefect(empty1_);
    BSetAppend* BSetAppend_instance = new BSetAppend;
    BSetRemove* BSetRemove_instance = new BSetRemove;
    BSetReplace* BSetReplace_instance = new BSetReplace;
}


//--------------------------------------------------------------------
/*! Returns a valid constant time set for name.
 */
template<>
BSyntaxObject* BGraContensBase<BSet>::FindConstant (const BText& name)
{ 
    return(NIL); 
}


//--------------------------------------------------------------------
/*! Returns a valid set for obj.
 */
template<>
BSyntaxObject* BGraContensBase<BSet>::Casting(BSyntaxObject* obj)
{
    if(!obj)			     { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BSet>::New(FILE* fil)
{
    return 0;
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations
//--------------------------------------------------------------------

#ifdef WIN32
#include <windows.h>
#include <winbase.h>
FNDParam(MemoryLoad, "Number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use).");
FNDParam(TotalPhys, "Total size of physical memory, in bytes.");
FNDParam(AvailPhys, "Size of physical memory available, in bytes.");
FNDParam(TotalPageFile, "Size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead.");
FNDParam(AvailPageFile, "Size of available memory to commit, in bytes. The limit is ullTotalPageFile.");
FNDParam(TotalVirtual, "Total size of the user mode portion of the virtual address space of the calling process, in bytes.");
FNDParam(AvailVirtual, "Size of unreserved and uncommitted memory in the user mode portion of the virtual address space of the calling process, in bytes.");


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetMemoryStatus);
DefExtOpr(1, BSetMemoryStatus, "MemoryStatus", 1, 1, "Real",
	  I2("(Real unused)",
	     "(Real nousado)"),
       "Returns Operative System information about current memory status including this values:\n"
       "  dwMemoryLoad:\n"
       "    Number between 0 and 100 that specifies the approximate percentage of physical memory that is in use (0 indicates no memory use and 100 indicates full memory use).\n"
       "  TotalPhys:\n"
       "    Total size of physical memory, in bytes.\n"
       "  AvailPhys:\n"
       "    Size of physical memory available, in bytes.\n"
       "  TotalPageFile:\n"
       "    Size of the committed memory limit, in bytes. This is physical memory plus the size of the page file, minus a small overhead.\n"
       "  AvailPageFile:\n"
       "    Size of available memory to commit, in bytes. The limit is ullTotalPageFile.\n"
       "  TotalVirtual:\n"
       "    Total size of the user mode portion of the virtual address space of the calling process, in bytes.\n"
       "  AvailVirtual:\n"
       "    Size of unreserved and uncommitted memory in the user mode portion of the virtual address space of the calling process, in bytes.\n"
       "This is the specification about this MICROSOFT WINDOWS (c) feature obtained from http://msdn.microsoft.com"
	     ,
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetMemoryStatus::CalcContens()
//--------------------------------------------------------------------
{
#if (defined(_MSC_VER) && (_MSC_VER>=1300))
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx (&statex);
  contens_.PrepareStore(7);
  contens_.AddElement(NewFND(BDat,MemoryLoad,          BDat((double)statex.dwMemoryLoad)));
  contens_.AddElement(NewFND(BDat,TotalPhys,           BDat((double)statex.ullTotalPhys)));
  contens_.AddElement(NewFND(BDat,AvailPhys,           BDat((double)statex.ullAvailPhys)));
  contens_.AddElement(NewFND(BDat,TotalPageFile,       BDat((double)statex.ullTotalPageFile)));
  contens_.AddElement(NewFND(BDat,AvailPageFile,       BDat((double)statex.ullAvailPageFile)));
  contens_.AddElement(NewFND(BDat,TotalVirtual,        BDat((double)statex.ullTotalVirtual)));
  contens_.AddElement(NewFND(BDat,AvailVirtual,        BDat((double)statex.ullAvailVirtual)));
#elif (defined(_MSC_VER) && (_MSC_VER<1300))
  MEMORYSTATUS statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatus (&statex);
  contens_.PrepareStore(8);
  contens_.AddElement(NewFND(BDat,MemoryLoad,          BDat((double)statex.dwMemoryLoad)));
  contens_.AddElement(NewFND(BDat,TotalPhys,           BDat((double)statex.dwTotalPhys)));
  contens_.AddElement(NewFND(BDat,AvailPhys,           BDat((double)statex.dwAvailPhys)));
  contens_.AddElement(NewFND(BDat,TotalPageFile,       BDat((double)statex.dwTotalPageFile)));
  contens_.AddElement(NewFND(BDat,AvailPageFile,       BDat((double)statex.dwAvailPageFile)));
  contens_.AddElement(NewFND(BDat,TotalVirtual,        BDat((double)statex.dwTotalVirtual)));
  contens_.AddElement(NewFND(BDat,AvailVirtual,        BDat((double)statex.dwAvailVirtual)));
#endif
}
#endif

//--------------------------------------------------------------------
void BSetOfType::CalcContens()
//--------------------------------------------------------------------
{
  //contens_ = Args();
  BList* argLst	= DupArgList();
  BList* args	  = argLst;
  BList* result = NIL;
  BList* aux	  = NIL;
  while(args)
  {
    BSyntaxObject* arg = (BSyntaxObject*)Car(args);
	  arg->Do();
	  LstFastAppend(result, aux, arg);
	  args = Cdr(args);
  }
  contens_.RobElement(result);
  DESTROY(argLst);
}


//--------------------------------------------------------------------
class BSetApply: public BSetTemporary
{
public:
    BSetApply(BList* arg)
	: BSetTemporary(arg) 
	{ 
	    CalcContens(); flags_.calculated_= BTRUE; 
	}
  void CalcContens();
  RedeclareClassNewDelete(BSetApply);
};
//--------------------------------------------------------------------

DeclareEvaluator(BSetApply);
DefExtOpr(1, BSetApply, "Apply", 2, 3, "Text Set Text",
	  I2("(Text method, Set set, Text fileName)",
	     "(Text método, Set conjunto, Text nombreFichero)"),
	  I2("Applies a method to the elements of a set and writes "
	     "resulted in it a file, if this is not void and said "
	     "method sustains it.\n"
	     "Example: Set x = Range(1,4,2);Set Apply(\"View\",x,\"\"); " 
	     ,
	     "Aplica un método a los elementos de un conjunto y escribe el "
	     "resultado en un fichero, si éste no es nulo y dicho método lo "
	     "soporta.\n"
	     "Ejemplo: Set x = Range(1,4,2);Set Apply(\"View\",x,\"\");") +
	  I2("Also it can be used directly the name of the method as "
	     "function. Example: Set View(x,\"\")"
	     ,
	     "También se puede utilizar directamente el nombre del método "
	     "como función.Ejemplo: Set View(x,\"\")"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetApply::CalcContens()
//--------------------------------------------------------------------
{
  BText	 methodName = Text(Arg(1));
  BSet&	 set	      = Set (Arg(2));
  BText	 fileName;

  if(Arg(3)) { fileName = Text(Arg(3)); }
  if(set.Card())
  {
	  BGrammar* gra = set[1]->Grammar();
	  BMethod*  met = gra->FindMethod(methodName);
    if(met) 
    { 
      BList* arg = set.ToReversedList();
      met->Apply(arg, fileName); 
      DESTROY(arg);
    }
	  else
	  { 
      Error(Out()+methodName + I2(" is not a " + Name() + " method.",
				      " No es un método de " + Name()));
	  }
    contens_ = set;
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetStruct);
DefExtOpr(1, BSetStruct, "Structure", 2, 0, "Text Anything Anything",
	  I2("(Text name, Anything field1 [, Anything field2, ...])",
	     "(Text nombre, Anything campo1 [, Anything campo2, ...])"),
	  I2("Creates a set with a given structure and a list of "
	     "objects with the type and the order indicated by such "
	     "structure. "
	     "Also it can be used directly the name of the structure "
	     "as function.\n\n"
	     
	     "Example:\n\n"
	     
	     "Struct PersonalData\n"
	     "(\n"
	     "	Text Name,\n"
	     "	Date Birth,\n"
	     "	Text Origin,\n"
	     "	Real Stature,\n"
	     "	Real Weight\n"
	     ");\n\n"
	     
	     "Set s = Structure\n"
	     "(\n"
	     "	\"PersonalData\",\n"
	     "	\"Jhon Brown\",\n"
	     "	y1953m1d12,\n"
	     "	\"New York\",\n"
	     "	1.78,\n"
	     "	87.6\n"
	     ");\n\n"
	     
	     ",or well ,\n"

	     "Set s = PersonalData\n"
	     "(\n"
	     "	\"Jhon Brown\",\n"
	     "	y1953m1d12,\n"
	     "	\"New York\",\n"
	     "	1.78,\n"
	     "	87.6\n"
	     ");"
	     
	     ,
	     
	     "Crea un conjunto con una estructura dada y una lista de "
	     "objetos con el tipo y el orden indicado por dicha "
	     "estructura. También se puede utilizar directamente el "
	     "nombre de la estructura como función. \n\n"
	     
	     "Ejemplo:\n\n"
	     
	     "Struct DatosPersonales\n"
	     "(\n"
	     "	Text Nombre,\n"
	     "	Date Nacimiento,\n"
	     "	Text Procedencia,\n"
	     "	Real Estatura,\n"
	     "	Real Peso\n"
	     ");\n\n"
	     
	     "Set s = Structure\n"
	     "(\n"
	     "	\"DatosPersonales\",\n"
	     "	\"Juan Pérez\",\n"
	     "	y1953m1d12,\n"
	     "	\"Madrid\",\n"
	     "	1.78,\n"
	     "	87.6\n"
	     ");\n\n"
	     
	     ",o bien ,\n\n"
	     
	     "Set s = DatosPersonales\n"
	     "(\n"
	     "	\"Juan Pérez\",\n"
	     "	y1953m1d12,\n"
	     "	\"Madrid\",\n"
	     "	1.78,\n"
	     "	87.6\n"
	     ");\n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetStruct::CalcContens()
//--------------------------------------------------------------------
{
  BText	   name = Text(Arg(1));
  BStruct* str	= FindStruct(name);
  if(!str)
  {
  	Error(name+I2(" is not any sructure ", " no es una estructura"));
  }
  else if(NumArgs()>1)
  {
    BList* lst = DupArgList();
	  BList* result = LstDuplicate(Cdr(lst));
	  contens_.RobStruct(result,str,BSet::Structured);
    DESTROY(lst);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPutStruct);
DefExtOpr(1, BSetPutStruct, "PutStructure", 2, 2, "Text Set",
	  I2("(Text structureName, Set set)",
	     "(Text nombreEstructura, Set conjunto)"),
	  I2("Creates a new set with a given structure from another "
	     "set whose elements must be compatible with the mentioned "
	     "structure fields. \n"
	     ,
	     "Crea un nuevo conjunto con una estructura dada a partir "
	     "de otro conjunto cuyos elementos sean compatibles con "
	     "los campos de dicha estructura. \n"),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetPutStruct::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("PutStructure")) { return; }
  BText    name = Text(Arg(1));
  BStruct* str  = FindStruct(name);
  if(!str)
  {
    Error(name+I2(" is not any sructure ", " no es una estructura"));
  }
  else if(NumArgs()>1)
  {
    contens_ = Set(Arg(2));
    contens_.PutStruct(str);
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetGrammars);
DefExtOpr(1, BSetGrammars, "Grammars", 1, 1, "Real",
	  I2("(Real unused)", "(Real noUsado)"),
	  I2("Returns a set with the names of all defined grammars.",
	     "Devuelve un conjunto con los nombres de las "
	     "gramáticas definidas."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetGrammars::CalcContens()
//--------------------------------------------------------------------
{
    BList* instances = BGrammar::Instances();
    BList* lst = NIL;
    BList* aux = NIL;
    while(instances)

    {
	BGrammar*  gra  = (BGrammar*)(Car(instances));
	BText      name = gra->Name();
	BText      desc = gra->Description();
	BUserText* uTxt = new BContensText("",name,desc);
	uTxt->PutName(name);
	LstFastAppend(lst,aux,uTxt);
	instances = Cdr(instances);
    }
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetMethods);
DefExtOpr(1, BSetMethods, "Methods", 1, 1, "Text",
	  I2("(Text grammar)", "(Text gramatica)"),
	  I2("Returns a set with the names of all defined methods for "
	     "a given grammar.",
	     "Devuelve un conjunto con los nombres de los métodos "
	     "definidos para una gramáticas determinada."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetMethods::CalcContens()
//--------------------------------------------------------------------
{
    BGrammar* grammar = Gram(Text(Arg(1)));
    if(grammar == NULL) return;
    
    BList* mthList = grammar->Methods();
    
    // run through the list for the names
    BList* lst = NIL;
    BList* aux = NIL;
    while(mthList)
    {
	BMethod* mth = (BMethod*)mthList->Car();
	BText      name = mth->Name();
	BText      desc = mth->Description();
	BUserText* uTxt = new BContensText("",name,desc);
	uTxt->PutName(name);
	LstFastAppend(lst,aux,uTxt);
	mthList = mthList->Cdr();
    }
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetNCopy);
DefExtOpr(1, BSetNCopy, "NCopy", 2, 2, "Real Anything",
	  I2("(Real n, Anything object)", "(Real n, Anything objecto)"),
	  I2("Returns a set with n copys of an object.",
	     "Devuelve un conjunto con n copias de un objeto."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetNCopy::CalcContens()
//--------------------------------------------------------------------
{
    BList* lst = NIL;
    BInt n, N = (BInt)Real(Arg(1));
    BSyntaxObject* obj = Arg(2);
    for(n=1; n<=N; n++) 
    { 
      lst = Cons(obj->CopyContens(), lst); 
      if(BGrammar::StopFlag()) { break; }
    }
    contens_.RobElement(lst);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetUnique);
DefIntOpr(1, BSetUnique, "Unique", 1, 1,
	  I2("(Set set)", "(Set cto)"),
	  I2("Returns the set of the not repeated elements of a given set.",
	     "Devuelve el conjunto de los elementos no repetidos de un "
	     "conjunto dado."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetUnique::CalcContens()
//--------------------------------------------------------------------
{
    contens_ = Set(Arg(1)).Unique();
}

//--------------------------------------------------------------------
void setConcat(const BSet& setOfSets, BSet& contens_)
//--------------------------------------------------------------------
{
  int k, n, totCard = 0;
  for(n=1; n<=setOfSets.Card(); n++)
  { 
    if(setOfSets[n]->Grammar()==GraSet())
    {
      BSet& set = Set(setOfSets[n]);
      totCard += set.Card(); 
    }
    if(BGrammar::StopFlag()) { return; }
  }
  contens_.PrepareStore(totCard);
  for(n=1; n<=setOfSets.Card(); n++)
  {
    if(setOfSets[n]->Grammar()==GraSet())
    {
      BSet& set = Set(setOfSets[n]);
      for(k=1; k<=set.Card(); k++)
      {
        contens_.AddElement(set[k]);
        if(BGrammar::StopFlag()) { return; }
      }
    }
    if(BGrammar::StopFlag()) { return; }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetConcatAll);
DefIntOpr(1, BSetConcatAll, "Concat", 1, 0,
	  "(Set set1 [, Set set2, ...])",
	  I2("Returns the concatenation of a list of sets.",
	     "Devuelve la concatenación de una lista conjuntos."),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BSetConcatAll::CalcContens()
//--------------------------------------------------------------------
{
  BSet setOfSets;
  setOfSets.PutElement(ArgList());
  setConcat(setOfSets, contens_);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetSetConcat);
DefIntOpr(1, BSetSetConcat, "SetConcat", 1, 1,
	  "(Set set)",
	  I2("Returns the concatenation of a the elements of a set of sets.",
	     "Devuelve la concatenación de los elementos de un conjunto de "
       "conjuntos."),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BSetSetConcat::CalcContens()
//--------------------------------------------------------------------
{
  BSet& setOfSets = Set(Arg(1));
  setConcat(setOfSets, contens_);
}

//--------------------------------------------------------------------
class BSetBinary: public BSetTemporary

/*! Abstract time set class to deriving binary time set operators.
 */
//--------------------------------------------------------------------
{
protected:
    BUserSet* a_;
    BUserSet* b_;
public:
    BSetBinary(BList* arg)
	: BSetTemporary(arg), a_(USet(Arg(1))), b_(USet(Arg(2))) 
	{}
  RedeclareClassNewDelete(BSetBinary);
};
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetBinary, BSetConcat);
DefIntOpr(1, BSetConcat, "<<", 2, 2,
	  I2("set1 << set2  {Set set1, Set set2}", 
	     "cto1 << cto2  {Set cto1, Set cto2}"),
	  I2("Attaches two sets without eliminating repeated elements and "
	     "preserving the order.",
	     "Concatena dos conjuntos sin eliminar elementos repetidos y "
	     "conservando el orden."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetConcat::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_ = a_->Contens() << b_->Contens(); 
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetBinary, BSetUnion);
DefIntOpr(1, BSetUnion, "+", 2, 2,
	  I2("set1 + set2  {Set set1, Set set2}", 
	     "cto1 + cto2  {Set cto1, Set cto2}"),
	  I2("Returns the union of two sets.",
	     "Devuelve la unión de dos conjuntos."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetUnion::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_ = a_->Contens()+b_->Contens(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetBinary, BSetIntersection);
DefIntOpr(1, BSetIntersection, "*", 2, 2,
	  I2("set1 * set2  {Set set1, Set set2}", 
	     "cto1 * cto2  {Set cto1, Set cto2}"),
	  I2("Returns the intersection of two sets.",
	     "Devuelve la intersección de dos conjuntos."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetIntersection::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_ = a_->Contens()*b_->Contens(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetBinary, BSetDifference);
DefIntOpr(1, BSetDifference, "-", 2, 2,
	  I2("set1 - set2  {Set set1, Set set2}", 
	     "cto1 - cto2  {Set cto1, Set cto2}"),
	  I2("Returns the difference between two sets.",
	     "Devielve la diferencia entre dos conjuntos."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetDifference::CalcContens()
//--------------------------------------------------------------------
{ 
    contens_ = a_->Contens()-b_->Contens(); 
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetCartesianProduct);
DefIntOpr(1, BSetCartesianProduct, "CartProd", 2, 0,
	  I2("(Set set1, Set set2 [, Set set3, ...])",
	     "(Set cto1, Set set2 [, Set cto3, ...])"),
	  I2("Returns the cartesian product of a list of sets.",
	     "Devuelve el producto cartesiano de una lista de conjuntos."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetCartesianProduct::CalcContens()
//--------------------------------------------------------------------
{
  BSet setOfSets;
  setOfSets.PutElement(ArgList());
  BArray<BInt> module(setOfSets.Card()+1);
  module[setOfSets.Card()] = 1;
  for(BInt n=setOfSets.Card()-1; n>=0; n--)
  { 
    module[n] = module[n+1] * Set(setOfSets[n+1]).Card(); 
    if(BGrammar::StopFlag()) { return; }
  }
  contens_.PrepareStore(module[0]);
  for(BInt k = 1; k<=module[0]; k++)
  {
    BSet set;
    set.PrepareStore(setOfSets.Card());
    for(BInt n = 1; n<=setOfSets.Card(); n++)
    {
      BInt m = 1 + ((k-1) / module[n]) % Set(setOfSets[n]).Card();
      set.AddElement(Set(setOfSets[n])[m]);
      if(BGrammar::StopFlag()) { return; }
    }
    contens_.AddElement(new BSetTuple("", set));
    if(BGrammar::StopFlag()) { return; }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPower);
DefExtOpr(1, BSetPower, "^", 2, 2, "Set Real",
	  I2("set^exponent  {Set set, Real exponent}", 
	     "cto^exponente  {Set cto, Real exponente}"),
	  I2("Returns the umpteenth power of a set, that is to say, "
	     "the umpteenth cartesian product of that set by itself.",
	     "Devuelve la enésima potencia de un conjunto, es decir, el "
	     "enésimo producto cartesiano de ese conjunto por sí mismo.")+
	  I2("Also it can be used the operator ** or the function Pow.",
	     "También se puede utilizar el operador ** o la función Pow."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetPower::CalcContens()
//--------------------------------------------------------------------
{
    BList* lst = NIL;
    for(BInt n=0; n<Dat(Arg(2)); n++) 
    { 
      lst = Cons(Arg(1), lst); 
      if(BGrammar::StopFlag()) { return; }
    }
    BSetCartesianProduct set(lst);
    contens_ = set.Contens();
}



//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetRange);
DefExtOpr(1, BSetRange, "Range", 3, 3, "Real Real Real",
	  I2("(Real a, Real b, Real p)",
	     "(Real desde, Real hasta, Real distancia)"),
	  I2("Returns the set of real numbers \n"
       " \n"
       "  a+k*p with k = 0,1,2,3,... \n"
       " \n"
       "in the interval [a,b] if p>0 or [b,a] if p<0 \n"
       " \n"
       "If p==0 shows an error and returns the empty file. \n"
       "If p>0 and a>b returns the empty file without showing any error. \n"
       "If p<0 and a<b returns the empty file without showing any error."
	     ,
	     "Devuelve el conjunto de los reales "
       " \n"
       "  a+k*p con k = 0,1,2,3,... \n"
       " \n"
       "en el intervalo [a,b] si p>0 ó [b,a] si p<0 \n"
       " \n"
       "Si p==0 muestra un error y devuelve el conjunto vacío \n"
       "Si p>0 y a>b devuelve el conjunto vacío sin mostrar error alguno. \n"
       "Si p<0 y a<b devuelve el conjunto vacío sin mostrar error alguno."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetRange::CalcContens()
//--------------------------------------------------------------------
{
  BList* result = NIL;
  BList* aux	  = NIL;
  
  BDat   fromDat    = Dat(Arg(1));
  BDat   untilDat   = Dat(Arg(2));
  BDat   stepDat    = Dat(Arg(3));
  
  if (!fromDat.IsKnown() || !untilDat.IsKnown() || !stepDat.IsKnown()) {
    Warning(I2("Some of the Range's arguments are unknown",
               "Alguno de los argumentos de Range son desconocidos"));
    return;
  }
  double from  = fromDat.Value();
  double until = untilDat.Value();
  double step  = stepDat.Value();
  int k;
  double x;
  if(step == 0.0) { /* warning!!! : this is a bad comparison */
    Warning(I2("Argument 'p' of function Range cannot be zero",
               "El argumento 'p' de la función Range no puede ser cero."));
    return;
  }
  if(((step>0.0)&&(until<from))||
     ((step<0.0)&&(until>from))) { return; }
  int K = 1+(int)floor(1.0+fabs((from-until)/step)-1.0);
  for(k=0; k<K; k++)
  {
    x = 1.0 + (from + step*k) -1.0;
    LstFastAppend(result, aux, new BContensDat("",x));
    if(BGrammar::StopFlag()) { break; }
  }
  contens_.RobElement(result);
}


//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetSort);
DefExtOpr(1, BSetSort, "Sort", 2, 2, "Set Code",
	  I2("(Set set, Code order)",
	     "(Set cto, Code orden)"),
	  I2("Orders a set with certain order criterion."
	     "A criterion C of the order O, for a form of given data, "
	     "it is a function, such that, dices two objects R and S "
	     "of said type, fulfils\n\n"
	     
	     "	C(R,S) = - 1	if and only if O(R) < O(S);\n\n"
	     "	C(R,S) =  0    if and only if O(R) = O(S);\n\n"
	     "	C(R,S) = +1    if and only if O(R) > O(S);\n\n"
	     
	     ,
	     
	     "Ordena un conjunto con cierto criterio de orden."
	     "Un criterio C del orden O, para un tipo de datos "
	     "determinado, es una función, tal que, dados dos "
	     "objetos R y S de dicho tipo, cumple\n\n"
	     
	     "	C(R,S) = -1    si y sólo si O(R) < O(S);\n\n"
	     "	C(R,S) =  0    si y sólo si O(R) = O(S);\n\n"
	     "	C(R,S) = +1    si y sólo si O(R) > O(S);\n\n"),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BSetSort::CalcContens()
//--------------------------------------------------------------------
{
    BCode&  code	 = Code(Arg(2));
    Arg(1)->Do();
    contens_ = Set(Arg(1));
    if(contens_.Card()>1)
    {
	if(!contens_.Sort(code))
	{
	    Error(I2("Cannot apply function Sort. ",
		     "No se puede aplicar la función Sort."));
	}
	BSet::CleanSort();
    }
}
//--------------------------------------------------------------------
DeclareNoLazzyClass(BDat, BDatTemporary, BDatSetIndexByName);
DefExtOpr(1, BDatSetIndexByName, "SetIndexByName", 1, 1, "Set",
"(Set set)",
I2("Index by name a non empty set of elements that have non null "
   "and unique name."
   "Returns true if all contitions are matched."
   "Afterwords a set has been indexed you can access to an "
   "element knowing just its name, by using operators [] and :: "
   "without the computing cost of a sequential search.\n"
   "If the set is modified the index of names will be deleted "
   "and searchs become sequential until you call to "
   "SetIndexByName again.\n"
   "\n"
   "Example:\n",
   "Indexa por nombre un conjunto no vacío de elements tales que "
   "todos ellos tengan un nombre único y no nulo. \n"
   "Devuelve cierto si se cumplen dichas condiciones. \n"
   "Una vez indexado se puede acceder a un elemento cualquiera, "
   "sin saber su posición sino su nombre, mediante los "
   "operadores [] y :: sin el coste computacional de la búsqueda "
   "secuencial.\n"
   "Si una vez indexado un conjunto se modifica de alguna manera "
   "dejará de estar indexado, por lo que las búsquedas volverán "
   "a ser secuenciales hasta que se vuelva a llamar a "
   "SetIndexByName.\n"
   "\n"
   "Ejemplo:\n")+
   "  Set set = {SetOfReal(a=1,b=2,c=3)};\n"
   "  Real SetIndexByName(set);\n"
   "  Text name = \"a\";\n"
   "  Real set[\"a\"];\n"
   "  Real set[name];\n"
   "  Real set::a;\n",
BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BDatSetIndexByName::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("SetIndexByName")) { return; }
  BSet& set = Set(Arg(1));
  contens_ = set.SetIndexByName();
}

//--------------------------------------------------------------------
DeclareNoLazzyClass(BDat, BDatTemporary, BDatHasIndexByName);
DefExtOpr(1, BDatHasIndexByName, "HasIndexByName", 1, 1, "Set",
"(Set set)",
I2("Returns TRUE if a set has already indexed by name with SetIndexByName and "
   "hasn't be modified afterwords in any detected way, due to in this case the "
   "index would be deleted in order to maintain referential integrity.\n"
   "Warning!: The index could be modified as secondary effect of some "
   "operations, ie. PutName(set[n]), that will not be detected.",
   "Devuelve CIERTO si un conjunto ha sido indexado por nombre con "
   "SetIndexByName y no ha sido modificado después de ninguna forma, pues "
   "entonces el índice desaparece para asegurar la integridad referencial.\n"
   "Atencion!: el indice podria haber sido modificado como efecto secundario "
   "de operaciones como PutName(set [n]), lo cual no seria detectado."),
BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BDatHasIndexByName::CalcContens()
//--------------------------------------------------------------------
{
  BSet& set = Set(Arg(1));
  contens_ = set.HasIndexByName();
}

//--------------------------------------------------------------------
DeclareNoLazzyClass(BDat, BDatTemporary, BDatFindIndexByName);
DefExtOpr(1, BDatFindIndexByName, "FindIndexByName", 2, 2, "Set Text",
"(Set set, Text name)",
I2("It returns the index position of the first element called <name> " 
   "or zero if doesn't exist such element.\n"
   "If <set> has been indexed with SetIndexByName, then it's much "
   "faster due to it takes O(1) comparissons instead of O(Card(set))."
   "\n\n"
   "Example:\n",
   "Devuelve el indicie posicional del primer elemento llamado <name> "
   "o cero si no existe tal elemento.\n"
   "Si el conjunto <set> ha sido indexado con SetIndexByName, es "
   "mucho más rápido puesto que requiere O(1) comparaciones en lugar "
   "de O(Card(set))."
   "\n\n"
   "Ejemplo:\n")+
   "  Set set = {SetOfReal(a=1,b=2,c=3)};\n"
   "  Real SetIndexByName(set);\n"
   "  Real pos_b = FindIndexByName(set,\"b\"); //=> 2\n"
   "  Real pos_u = FindIndexByName(set,\"u\"); //=> 0\n",
BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BDatFindIndexByName::CalcContens()
//--------------------------------------------------------------------
{
  BSet&  set  = Set (Arg(1));
  BText& name = Text(Arg(2));
  contens_ = set.FindIndexByName(name);
}


//--------------------------------------------------------------------
class BTolFuncClassifier : public BClassifier
//--------------------------------------------------------------------
{
 public:
  BSet&  set_;
  BCode& code_;
  BTolFuncClassifier(BSet& set, BCode& code) 
  : BClassifier(set.Card()),
    set_       (set),
    code_      (code)  
  {}
  int Relation(int i, int j, bool& error) const
  {
    int result;
    BList* args = Cons(set_[i+1],NCons(set_[j+1]));
    BSyntaxObject* obj  = code_.Evaluator(args);
    if(obj)
    {
	    BUserDat* uDat = (BUserDat*)obj;
	    uDat->Contens();
	    result = (BInt)uDat->Contens().Value();
	    DESTROY(obj);
    }
    else
    {
      result = -n_*n_;
      error = true;
    }
    return(result);
  }
};

//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetClassify);
DefExtOpr(1, BSetClassify, "Classify", 2, 3, "Set Code Text",
"(Set set, Code binaryRelation [, Text relationType = \"partial order\"])", 
I2(
"Classifies a set <s> in equivalence classes (see http://en.wikipedia.org/wiki/Binary_relation "
"and http://en.wikipedia.org/wiki/Equivalence_relation ). Returns a set of sets containing a "
"different equivalence class each one, I.E. a partition of a set in subsets with empty "
"intersections which total union is the original set.\n"
"\n"
"The equivalence relation will be generated through the given user function <f> depending "
"on the specified type of relation that can be one of these:\n"
"\n"
"1º) \"equivalence\" relation: This is the more natural way to classify a set. In this case "
"the provided user function must be a binary relation over <s> matching reflexive, symmetric, "
"and transitive properties.\n"
"Non of these three properties will be tested but all will be supossed as true, so it needs to "
"evaluate just these pairs of elements\n"
"\n"
"                              i=j\n"
"  s[i]~s[j] <=> s[i]~s[j] <=> or\n"
"                              f(s[i],s[j])=1 for i,j=1...n; i<j\n"
"\n"
"The algorithmic complexity of this method seems to be (n-1)*(n-2)/2 if all the pairs of "
"elements i<j must be checked; but normally complexitie is close to O(n), thanks to transitive "
"propierty.\n"
"\n"
"Example:\n"
"\n"
"We want to classify a set of lists of numbers following the equivalence relation \"to be a "
"permutation of\". A simple way to check if a list is a permutation of other is to sort both "
"lists and to check that resulting lists are the same one. Note that this method is generalizable "
"to a lot of situations as the comparisson of the canonical representant of the class of each "
"element.\n"
"\n"
"Set s = [[ [[1,2,2,2]], [[1,1,2,2]], [[1,2,1,2]], [[2,2,1,2]], [[1,1,1,1]], [[2,1,2,1]] ]];\n"
"Real CompareReal(Real x, Real y) { Compare(x,y) }\n"
"Real f(Set a, Set b)\n"
"{\n"
"  Text a_ = \"\"<<Sort(a, CompareReal);\n"
"  Text b_ = \"\"<<Sort(b, CompareReal);\n"
"  ok = a_ == b_;\n"
"};\n"
"Set classes = Classify(s, f, \"equivalence\");\n"
"\n"
"Returns\n"
"\n"
"[[ \n"
"  [[ [[1,2,2,2 ]], [[2,2,1,2]] ]],\n"
"  [[ [[1,1,2,2 ]], [[1,2,1,2]], [[2,1,2,1]] ]],\n"
"  [[ [[1,1,1,1 ]] ]]\n"
"]]\n"
"\n"
" \n"
"2º) \"symmetric\" relation: The transitive-reflexive closure T of a symmetric relation S is "
"defined as:\n"
"\n"
"            a = b  or\n"
" a T b <=>  a S b  or\n"
"            a S c_1 and c_1 S c_2 and ... and c_k S b for some c_1, c_2, ... , c_k\n"
"\n"
"Then T is symmetric, transitive and reflexive and is the smallest equivalence relation containing "
"the original symmetric relation. (See http://en.wikipedia.org/wiki/Transitive_closure)\n"
"The equivalence relation will be defined from the user function <f> as follows\n"
"\n"
"                        i=j or\n"
"s[i]~s[j] <=> s[j]~s[i] f(s[i],s[j])=1  or\n"
"                        f(s[i],s[k_1])=1 and f(s[h_1],s[h_2])=1 and ... and f(s[h_k],s[j])=1 for "
"some h_1,...,h_k=1...n\n"
"\n"
"\n"
"In this non transitive case, the algorithmic complexity of relations evaluating is (n-1)*(n-2)/2 "
"since all the pairs of elements i<j must be checked.\n"
"An exhaustive method for for computing the transitive closure needs O(n^3) operations but the "
"implemented method is a cuadratic one (O(n^3)), and there are even linear ones (O(n)) as Tarjan "
"algorithm (See http://www.cs.hut.fi/~enu/thesis.html).\n"
"\n"
"\n"
"Example:\n"
"\n"
"We have this set of numbers and we want to separate them in a set of classes of numbers generated "
"by mutually exclusive sets of prime numbers being all these classes not separable in lesser ones. "
"Two numbers has one or more common prime divisor if and only if its Maximum Common Divisor (MCD) "
"must be greater than 1, so, all members of different classes must have MCD equal to 1, and all "
"members a, b of the same class has MCD(a,b) > 1 or there is another elements c_1, c_2, ..., c_k "
"in the class matching MCD(a,c_1)>1, MCD(c_1,c_2)>1,...,MCD(c_k,b)>1\n"
"\n"
"Set s = [[2 , 4, 7, 15 , 26, 33, 35]];\n"
"//Maximum Common Divisor of two positive integer numbers\n"
"Real MCD(Real a, Real b)\n"
"{\n"
"  //Note that MCD is a symmetric function: MCD(a,b)=MCD(b,a)\n"
"  If(a<b,MCD(b,a),\n"
"  {\n"
"    Real c = a % b;\n"
"    If(!c, b, MCD(b,c))\n"
"  })\n"
"};\n"
"//Symmetric binary relation\n"
"Real f(Real a, Real b)\n"
"{\n"
"  MCD(a, b)>1\n"
"};\n"
"Set classes = Classify(s, f, \" symmetric\");\n"
"\n"
"Returns\n"
"\n"
"[[\n"
"  [[ 2, 4, 26 ]],      //Class generated by primes 2, 13\n"
"  [[ 7, 1 5, 33, 35 ]] //Class generated by primes 3, 5, 7, 11\n"
"]]\n"
"\n"
"Note that 33=3*11 and 7 have no common primes, but MCM(7,35)=7>1, MCM(35,15)=5>1, MCM(15,33)=3>1, "
"so both 33 and 7 are in the same class\n"
"\n"
"\n"
"3º) \"binary\" relation: The symmetric closure of an arbitrary binary relation R is the relation "
"defined as\n"
"\n"
"  a S b <=> a R b or b R a\n"
"\n"
"Relation S is obviously the lesser symmetric relation containing R, and its transitive-reflexive "
"clossure is an equivalence relation as it's explained above.\n"
"\n"
"The equivalence relation will be defined from the user function <f> as follows\n"
"\n"
"                i=j or\n"
"                f(s[i],s[j]) = 1  or\n"
"                f(s[j],s[i]) = 1  or\n"
"  s[i]~s[j] <=> f(s[i],s[k]) = 1  and f(s[k],s[j]) = 1 for some k=1...n or\n"
"                f(s[k],s[i]) = 1  and f(s[k],s[j]) = 1 for some k=1...n or\n"
"                f(s[i],s[k]) = 1  and f(s[j],s[k]) = 1 for some k=1...n or\n"
"                f(s[k],s[i]) = 1  and f(s[j],s[k]) = 1 for some k=1...n\n"
"\n"
"Complexity of relations evaluation is between (n-1)*(n-2)/2 and (n-1)*(n-2), depending on the number "
"of times that f(s[i],s[j])=0 with i<j, due to just in this case it must to compute f(s[j],s[i]).\n"
"\n"
"Example:\n"
"\n"
"Let <s> the same set of numbers than used in \"symmetric\" example, but now we want generate the "
"irreductible classes of numbers that are divisible by other one in the class.\n"
"\n"
"Set s = [[2 , 4, 7, 15 , 26, 33, 35]];\n"
"//Non symmetric binary relation\n"
"Real f(Real a, Real b)\n"
"{\n"
"  a % b == 0\n"
"};\n"
"Set classes = Classify(s, f, \"binary \");\n"
"\n"
"Returns\n"
"\n"
"[[\n"
"  [[ 2, 4, 26 ]], //Class of numbers that are divisible by 2\n"
"  [[ 7, 35 ]],    //Class of numbers that are divisible by 7\n"
"  [[ 15 ]],       //Class of numbers that are divisible by 15\n"
"  [[ 33 ]]        //Class of numbers that are divisible by 33\n"
"]]\n"
"\n"
"Note that 26 is not divisible by 4, but 26%2=0 and 4%2=0, so both are in the same class\n"
"\n"
"\n"
"4º) \"partial order\" relation: In this case the provided binary relation will be a partial order, "
"its to say reflexive, antisymmetric, and transitive. This is the default value of argument "
"<relationType> when it is not specified. In order to use internal standard C functions qsort, a "
"implementation of quicksort algorithm (see http://en.wikipedia.org/wiki/Quicksort ),  the relation "
"is passed as a comparisson function that returns\n"
"\n"
" f(a,b) = -1 if a<=b and !(b<=a)\n"
"  f(a,b) =  0 if a<=b and   b<=a\n"
"  f(a,b) = +1 if b<=a and !(a<=b)\n"
"\n"
"The equivalence relation is defined as\n"
"\n"
"  a ~ b <=> f(a,b) = 0\n"
"\n"
"Firstly, it sorts the set (see the function Sort) obtaining a set\n"
"\n"
"  s[1] <= s[2] <= s[3] <= ... <= s[n]\n"
"\n"
"and thereinafter they are taken the equal element subsets mutually\n"
"\n"
"  S1 = { s[1], ... , s[m[1]] },\n"
"  S2 = { s[m[1]+1], ... , s[m[2]] },\n"
"  ...\n"
"\n"
"such that\n"
"\n"
"  f(s[1],s[2] )=0...0=f(s[m]-1,s[m] )=0; f(s[m], s[m]+1) = -1\n"
"  f(s[m[1]+1],s[ m[1]+2])=0...0= f(s[m+1]-1,s[m+1] )=0; f(s[m+1], s[m+1]+1) = -1\n"
"  ...\n"
"\n"
"The algorithmic complexity of this method is O(n Log(n)).\n"
"\n"
"Example:\n"
"\n"
"Now we have this set of strings and we want classify by its length.\n"
"\n"
"Set s = [[ \"aefg\", \"ada\", \"fefg\", \"fadabbbg\" , \"efg\", \"abbb\" ]];\n"
"Real f(Text a, Text b)\n"
"{\n"
"  Compare( TextLength(a), TextLength(b))\n"
"};\n"
"Set classes = Classify(s, f, \"partial order\");\n"
"\n"
"or simply\n"
"\n"
"Set classes = Classify(s, f);\n"
"\n"
"Returns\n"
"\n"
"[[ \n"
"  [[ \"ada\", \"efg\" ]],           //Class of strings with length 3\n"
"  [[ \"abbb\", \"fefg\", \"aefg\" ]], //Class of strings with length 4\n"
"  [[ \"fadabbbg\" ]]              //Class of strings with length 8 \n"
"]];\n"
,
"Clasifica un conjunto <s> en clases de equivalencia (ver http://en.wikipedia.org/wiki/Binary_relation "
"and http://en.wikipedia.org/wiki/Equivalence_relation ). Devuelve un conjunto de conjuntos conteniendo "
"una clase de equivalencia distinta cada uno, es decir, una partición del conjunto en subconjuntos de "
"intersección vacía y cuya unión total da el conjunto original.\n"
"\n"
"La relación de equivalencia se generará a través de la función de usuario dada <f> dependiendo del tipo "
"de relación especificada que puede ser uno de estos:\n"
"\n"
"\n"
"1º) \"equivalence\" (relación de equivalencia) : Este es el modo más natural de clasificar un conjunto. "
"En este caso la función de usuario debe ser una relación binaria sobre <s> que cumpla las propiedades "
"reflexiva, simétrica y transitiva. Ninguna de estas propiedades serán chequeadas pero se supondrá que se "
"cumplen.\n"
"\n"
"2º) \"symmetric\" (relación simétrica): La cerradura reflexiva-transitiva T de una relación simétrica se "
"define como:\n"
"\n"
"            a = b  or\n"
" a T b <=>  a S b  or\n"
"            a S c_1 and c_1 S c_2 and ... and c_k S b for some c_1, c_2, ... , c_k\n"
"\n"
"Entonces, T es reflexiva, simétrica y transitiva y es la menor relación de equivalencia que contiene a la "
"relación original. (Ver http://en.wikipedia.org/wiki/Transitive_closure)\n"
"La relación de equivalencia no será ahora explícitamente definida por la función de usuario sino que se "
"deducirá implícitamente de este modo:\n"
"\n"
"              i=j or\n"
"s[i]~s[j] <=> s[j]~s[i] f(s[i],s[j])=1  or\n"
"              f(s[i],s[k_1])=1 and f(s[h_1],s[h_2])=1 and ... and f(s[h_k],s[j])=1 for some h_1,...,h_k=1...n\n"
"\n"
"Por no ser transitiva es necesario evaluar todos los pares i<j y la complejidad de la evaluación es "
"(n-1)*(n-2)/2. Para computar la la cerradura transitiva se requiere además O(n^3) operaciones usando el "
"método exhaustivo, pero se ha implementado un método cuadrático O(n^2) e incluso existen métodos lineales "
"O(n) como el algoritmo de Trajan (Ver http://www.cs.hut.fi/~enu/thesis.html).\n"
"\n"
"Ejemplo:\n"
"\n"
"Tenemos un conjunto de números enteros mayores que 1 que queremos separar en un conjunto de clases generadas "
"por conjuntos de primos mutuamente excluyentes entre sí y no separables en otros más pequeños. Si dos números "
"enteros mayores que 1 tienen algún diviso primo en común su Máximo Común Divisor (MCD) será mayor que 1, por "
"lo que lso elementos de clases distintas tendran MCD igual a 1, y todos los miembros de una misma clase tendrán "
"MCD mayor que 1 o bien habrá una serie de elementos c_1, c_2, ..., c_k en esa clase tales que MCD(a,c_1)>1, "
"MCD(c_1,c_2)>1,...,MCD(c_k,b)>1\n"
"\n"
"Set s = [[2 , 4, 7, 15 , 26, 33, 35]];\n"
"//Maximum Common Divisor of two positive integer numbers\n"
"Real MCD(Real a, Real b)\n"
"{\n"
"  //Note that MCD is a symmetric function: MCD(a,b)=MCD(b,a)\n"
"  If(a<b,MCD(b,a),\n"
"  {\n"
"    Real c = a % b;\n"
"    If(!c, b, MCD(b,c))\n"
"  })\n"
"};\n"
"//Symmetric binary relation\n"
"Real f(Real a, Real b)\n"
"{\n"
"  MCD(a, b)>1\n"
"};\n"
"Set classes = Classify(s, f, \"symmetric\");\n"
"\n"
"Devuelve\n"
"\n"
"[[\n"
"  [[ 2, 4, 26 ]],      //Class generated by primes 2, 13\n"
"  [[ 7, 1 5, 33, 35 ]] //Class generated by primes 3, 5, 7, 11\n"
"]]\n"
"\n"
"Nótese que 33=3*11 y 7 no tienen primos en común pero se tienen las relaciones MCM(7,35)=7>1, MCM(35,15)=5>1, "
"MCM(15,33)=3>1, por lo que ambos, 33 and 7, están en la misma clase.\n"
"\n"
"\n"
"3º) \"binary\" (relacion binaria): La cerradura simétrica de una relación binaria cualquiera se define como\n"
"\n"
"  a S b <=> a R b or b R a\n"
"\n"
"Esta relación es obviamente la mínima relación simétrica que contiene a R, y su cerradura reflexiva-transitiva "
"T es una relación de equivalencia, tal y como ya se ha visto más arriba, y que se define implicitamente a partir "
"de la función de usuario <f> como sigue\n"
"\n"
"                i=j or\n"
"                f(s[i],s[j]) = 1  or\n"
"                f(s[j],s[i]) = 1  or\n"
"  s[i]~s[j] <=> f(s[i],s[k]) = 1  and f(s[k],s[j]) = 1 for some k=1...n or\n"
"                f(s[k],s[i]) = 1  and f(s[k],s[j]) = 1 for some k=1...n or\n"
"                f(s[i],s[k]) = 1  and f(s[j],s[k]) = 1 for some k=1...n or\n"
"                f(s[k],s[i]) = 1  and f(s[j],s[k]) = 1 for some k=1...n\n"
"\n"
"La complejidad de la evaluación de las relaciones estará ahora entre (n-1)*(n-2)/2 y (n-1)*(n-2), dependiendo "
"del número de veces que no se cumpla f(s[i],s[j]) con i<j, pues sólo en tal caso hay que calcular f(s[j],s[i]).\n"
"\n"
"Ejemplo:\n"
"\n"
"Sea ahora <s> el mismo conjunto del caso simétrico pero ahora queremos generar las clases irreductibles de "
"números tal que todos son divisibles por otro de la clase. La cerradura simétrica de esta relación es \"es "
"divisible o múltiplo de\"\n"
"\n"
"Set s = [[2 , 4, 7, 15 , 26, 33, 35]];\n"
"//Non symmetric binary relation\n"
"Real f(Real a, Real b)\n"
"{\n"
"  a % b == 0\n"
"};\n"
"Set classes = Classify(s, f, \"binary\");\n"
"\n"
"Devuelve\n"
"\n"
"[[\n"
"  [[ 2, 4, 26 ]], //Class of numbers that are divisible by 2\n"
"  [[ 7, 35 ]],    //Class of numbers that are divisible by 7\n"
"  [[ 15 ]],       //Class of numbers that are divisible by 15\n"
"  [[ 33 ]]        //Class of numbers that are divisible by 33\n"
"]]\n"
"\n"
"Nótese que 26 no es is divisible por 4, pero 26%2=0 y 4%2=0, por lo que están en la misma clase.\n"
"\n"
"\n"
"4º) \"partial order\" (relación de orden parcial): En este caso la relación utilizada cumple las propiedades "
"reflexiva, antisimétrica y transitiva. Es el caso por defecto cuando no se especifica el argumento "
"<relationType>. Para poder usar internamente la función de C estándar qsort, una implementación del algoritmo "
"quicksort (ver http://en.wikipedia.org/wiki/Quicksort ), la relación se expresa como una función de "
"comparación que devuelve:\n"
"\n"
"  f(a,b) = -1 if a<=b and !(b<=a)\n"
"  f(a,b) =  0 if a<=b and   b<=a\n"
"  f(a,b) = +1 if b<=a and !(a<=b)\n"
"\n"
"La relación de equivalencia se define simplemente como\n"
"\n"
"  a ~ b <=> f(a,b) = 0\n"
"\n"
"En primer lugar se ordenan los elementos mediante la función de comparación <f>\n"
"\n"
"  s[1] <= s[2] <= s[3] <= ... <= s[n]\n"
"\n"
"y después se rompe en clases separando por aquellos elementos consecutivos\n"
"\n"
"  S1 = { s[1], ... , s[m[1]] },\n"
"  S2 = { s[m[1]+1], ... , s[m[2]] },\n"
"  ...\n"
"\n"
"tales que\n"
"\n"
"  f(s[1],s[2] )=0...0=f(s[m]-1,s[m] )=0; f(s[m], s[m]+1) = -1\n"
"  f(s[m[1]+1],s[ m[1]+2])=0...0= f(s[m+1]-1,s[m+1] )=0; f(s[m+1], s[m+1]+1) = -1\n"
"  ...\n"
"\n"
"La complejidad algorítmica es O(n Log(n))\n"
"\n"
"Ejemplo:\n"
"\n"
"Se quiere clasificar un conjunto de textos en función de su longitud. Como ésta es una magnitud comparable "
"se puede usar la clasificación por orden parcial\n"
"\n"
"Set s = [[ \"aefg\", \"ada\", \"fefg\", \"fadabbbg\" , \"efg\", \"abbb\" ]];\n"
"Real f(Text a, Text b)\n"
"{\n"
"  Compare( TextLength(a), TextLength(b))\n"
"};\n"
"Set classes = Classify(s, f, \"partial order\");\n"
"\n"
"o simplemente\n"
"\n"
"Set classes = Classify(s, f);\n"
"\n"
"Devuelve\n"
"\n"
"[[ \n"
"  [[ \"ada\", \"efg\" ]],           //Class of strings with length 3\n"
"  [[ \"abbb\", \"fefg\", \"aefg\" ]], //Class of strings with length 4\n"
"  [[ \"fadabbbg\" ]]              //Class of strings with length 8 \n"
"]];\n"),
BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetClassify::CalcContens()
//--------------------------------------------------------------------
{
  BList* result   = NIL;
  BSet& orig = Set(Arg(1));
  if(orig.Card()==0) { return; }
  else if(orig.Card()==1)
  {
    BUserSet* cl = NewSet("","Class 1",orig.ToList(),
                          NIL, BSet::Generic);
    result = NCons(cl);
    contens_.RobStruct(result,NIL,BSet::Generic);
    return;
  }
  BSet sorted = orig;
  BCode&  code    = Code(Arg(2));
  BList* aux      = NIL;
  BList* classLst = NIL;
  BList* classAux = NIL;
  Arg(1)->Do();
  BText relationType = "partial order";
  if(Arg(3)) { relationType = Text(Arg(3)); }
  if(relationType == "partial order")
  {
    BInt m = 1;
  //Std(Out()+"\nClassifing by "+Code(Arg(2))->Name()+ " criterium");
    if(!sorted.Sort(code))
    {
      Error(I2("Cannot apply function Classify. ",
               "No se puede aplicar la función Classify."));
    }
    else
    {
      LstFastAppend(classLst, classAux, sorted[1]);
    //Std(Out()+"\nClass 1");
    //Std(Out()+"\nObject 1 = " +sorted[1]->Dump());
      for(BInt n = 2; n<=sorted.Card(); n++)
      {
        if(sorted.CompareElements(n-1, n))
        {
      //Std(Out()+"\nClass "+ m);
          LstFastAppend(result, 
                        aux, 
                        NewSet("",BText("Class ")+m,classLst,
                               NIL,BSet::Generic));
          classLst = NIL;
          classAux = NIL;
          m++;
        }
      //Std(Out()+"\nObject "+ n + " = " +sorted[n]->Dump());
        LstFastAppend(classLst, classAux, sorted[n]);
        if(BGrammar::StopFlag()) { break; }
      }
      LstFastAppend(result, aux, NewSet("",BText("Class ")+m,classLst,
                    NIL,BSet::Generic));
      contens_.RobStruct(result,NIL,BSet::Generic);
    }
    BSet::CleanSort();
  }
  else
  {

    BSet& set = Set(Arg(1));
    BTolFuncClassifier classifier(set, code);
    classifier.Classify(relationType);
    if(classifier.Error())
    {
      Error(I2("Cannot apply function Classify. ",
               "No se puede aplicar la función Classify."));
    }
    else
    {
      int i, j;
      int c = classifier.GetNumberOfClasses();
      contens_.PrepareStore(c);
      for(i=0; i<c; i++)
      {
        int s = classifier.GetSizeOfClass(i);
        BUserSet* cl = NewSet("",BText("Class ")+(i+1),NULL,NULL,BSet::Generic);
        cl->Contens().PrepareStore(s);
        for(j=0; j<s; j++)
        {
          int k = (!j)?classifier.GetFirstElementOfClass(i)
                      :classifier.GetNextElementOfClass (i);
          cl->Contens().AddElement(set[k+1]);  
        }
        contens_.AddElement(cl);
        if(BGrammar::StopFlag()) { break; }
      }
    }
  }
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetExtract);
DefExtOpr(1, BSetExtract, "Extract", 2, 0, "Set Real Real",
	  I2("(Set set, Real col1 [, Real col2, ...])",
	     "(Set cto, Real col1 [, Real col2, ...])"),
	  I2("Returns a range of columns of a table-set.",
	     "Devuelve un rango de columnas de un conjunto-tabla."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetExtract::CalcContens()
//--------------------------------------------------------------------
{
    BInt	       n;
    BInt	       m;
    BSet&	       set    = Set (Arg(1));
    BList*       aux    = NIL;
    BList*       result = NIL;
    BReal	       minCol = Real(Arg(2));
    BReal	       maxCol = Real(Arg(2));
    BArray<BInt> col(NumArgs()-1);
    
    for(m=0; m<col.Size(); m++)
    {
	col[m] = (BInt)Real(Arg(m+2));
	if(col[m]<minCol) { minCol=col[m]; }
	if(col[m]>maxCol) { maxCol=col[m]; }
    }
    
    for(n=1; n<=set.Card(); n++)
    {
	if((minCol<1)||
	   (set[n]->Grammar()!=Grammar())||
	   (Set(set[n]).Card()<maxCol))
	{
	    Error(I2("Cannot extract from", "No se puede extraer de ")+
		  Arg(1)->Identify());
	    DESTROY(result);
	    return;
	}
	BList* auxCol = NIL;
	BList* resCol = NIL;
	for(m=0; m<col.Size(); m++)
	{
	    LstFastAppend(resCol, auxCol, Set(set[n])[col[m]]);
	}
	LstFastAppend(result, aux, NewSet("","",resCol,NIL,BSet::Generic));
    }
    contens_.RobStruct(result,NIL,BSet::Table);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetExtractByIndex);
DefExtOpr(1, BSetExtractByIndex, "ExtractByIndex", 2, 2, "Set Set",
	  I2("(Set set, Set index)",
	     "(Set cto, Set indice)"),
	  I2("Returns a set with the selected elements by index."
	     "Index argument must be a Set of integer numbers",
	     "Devuelve un conjunto con los elementos seleccionados en indice."
	     "El argumento indice debe ser un conjunto de números enteros"),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetExtractByIndex::CalcContens()
//--------------------------------------------------------------------
{
    BInt	       n;
    BSet&	       set    = Set (Arg(1));
    BSet&	       index  = Set (Arg(2));
    BList*       aux    = NIL;
    BList*       result = NIL;
    
    for(n=1; n<=index.Card(); n++)
    {
	BSyntaxObject* indexn = index[n];
	if(indexn && (indexn->Grammar()->Name()=="Real"))
	{
	    BInt m = (BInt)Real(indexn);
	    if((m <= set.Card()) && set[m])
	    {
		LstFastAppend(result, aux, set[m]);
	    }
      if(BGrammar::StopFlag()) { break; }
	}
    }
    contens_.RobElement(result);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetAddColumns2);
DefIntOpr(1, BSetAddColumns2, "|", 2, 2,
	  I2("set1 | set2  {Set set1, Set set2}", 
	     "cto1 | cto2  {Set cto1, Set cto2}"),
	  I2("Adds the columns of two table-sets.",
	     "Concatena dos conjuntos-tabla por columnas."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetAddColumns2::CalcContens()
//--------------------------------------------------------------------
{
    BInt	       n;
    BSet&	       set1   = Set (Arg(1));
    BSet&	       set2   = Set (Arg(2));
    BList*       aux    = NIL;
    BList*       result = NIL;
    
    BInt n1 = set1.Card();
    BInt n2 = set2.Card();
    
    if(!n1 || (n1!=n2))
    {
	Error(I2("Cannot add columns from ", "No se puede adjuntar "
		 "columnas de ")+
	      Arg(1)->Identify() + I2(" and ", " y ") + Arg(2)->Identify());
	return;
    }
    for(n=1; n<=n1; n++)
    { 
      LstFastAppend(result,aux,new BSetTuple("",Set(set1[n])<<Set(set2[n]))); 
      if(BGrammar::StopFlag()) { break; }
    }
    contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetTraspose);
DefIntOpr(1, BSetTraspose, "Traspose", 1, 1,
	  I2("(Set set)", "(Set cto)"),
	  I2("Traspose rows and columns of table-set.",
	     "Traspone las filas y columnas de un conjunto-tabla."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetTraspose::CalcContens()
//--------------------------------------------------------------------
{
    BInt	       n;
    BInt	       m;
    BSet&	       set    = Set (Arg(1));
    BList*       aux    = NIL;
    BList*       result = NIL;
    
    if(!set.Card() || (set[1]->Grammar()!=Grammar()))
    {
	Error(I2("Cannot traspose ", "No se puede trasponer ")+
	      Arg(1)->Identify());
	return;
    }
    BInt lin = Set(set[1]).Card();
    BInt col = set.Card();
    for(m=1; m<=col; m++)
    {
	if((set[m]->Grammar()!=Grammar()) || (Set(set[m]).Card()!=lin))
	{
	    Error(I2("Cannot traspose ", "No se puede trasponer ")+
		  Arg(1)->Identify());
	    return;
	}
    }
    for(n=1; n<=lin; n++)
    {
	BList* auxCol = NIL;
	BList* resCol = NIL;
	for(m=1; m<=col; m++) {
	    LstFastAppend(resCol, auxCol, Set(set[m])[n]); 
      if(BGrammar::StopFlag()) { break; }
	}
	LstFastAppend(result, aux, NewSet("","",resCol,NIL,BSet::Generic));
      if(BGrammar::StopFlag()) { break; }
    }
    contens_.RobStruct(result,NIL,BSet::Table);
}


//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetFor);
DefExtOpr(1, BSetFor, "For", 3, 3, "Real Real Code",
	  "(Real n, Real m, Code f)",
	  I2("Returns a set whose elements are the evaluations of the "
	     "function f for each entire number between n and m, both "
	     "included.",
	     "Devuelve un conjunto cuyos elementos son las evaluaciones "
	     "de la función f para cada número entero entre n y m, "
	     "ambos incluídos."),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BSetFor::CalcContens()
//--------------------------------------------------------------------
{
//InitTotalTime("BSetFor::CalcContens");
  BStandardOperator* opr  = (BStandardOperator*)GetOperator(UCode(Arg(3)));
  int from  = (int)Real(Arg(1));
  int until = (int)Real(Arg(2));
  BSyntaxObject* arg     = NIL;
  BSyntaxObject* objCode = NIL;
  int n;

  if(from<=until) 
  { 
    contens_.PrepareStore(until-from+1);
    if(!opr || (opr->MaxArg()!=1) || (opr->GrammarForArg(1)!=GraReal()))
    {
      Error(I2("Wrong argument function in For function calling."
         "It must have just one argument of Real type.",
         "Función argumento errónea en llamada a la función For."
         "Ésta debe tener exactamente un argumento de tipo Real."));
    }
    else
    {
      for(n = from; n<=until; n++)
      {
        arg = new BContensDat(n);
        BCore::MemAssignInfo mai(arg);
        if(objCode = opr->Evaluator(NCons(arg)))
        {
          contens_.AddElement(objCode);
        }
        else
        {
          mai.SafeDestroy();
          char es[64];
          char en[64];
          sprintf(en, "<For> function failed at iteration %d.", n);
          sprintf(es, "Fallo en función <For> en iteración %d.", n);
          Error(I2(en ,es));
        }
        if(BGrammar::StopFlag()) { break; }
      }
    }
  }
//SumPartialTime;
}


//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetEvalSet);
DefExtOpr(1, BSetEvalSet, "EvalSet", 2, 2, "Set Code",
	  "(Set set, Code doit)",
	  I2("Returns the set of the successive evaluations of a "
	     "function, taking as argument each element of a set.",
	     "Devuelve el conjunto de las evaluaciones sucesivas de "
	     "una función, tomando como argumento cada elemento de "
	     "un conjunto."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetEvalSet::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject*     objCode;
  BSet&		     set    = Set (Arg(1));
  BCode&	     code   = Code(Arg(2));
  BGrammar*	   gra    = code.Grammar();
  BList*	     aux    = NIL;
  BList*	     result = NIL;
  BInt		     n	    = 1;
   
  if(!code.Operator() || (code.Operator()->NumArg()!=1))
  {
	  Error(I2("Wrong argument function in EvalSet function calling."
		         "It must have just one argument.",
		         "Función argumento errónea en llamada a la función EvalSet."
		         "Ésta debe tener exactamente un argumento."));
	  return;
  }
  if(set.Card())
  {
	  do
	  {
      BList* lst = NCons(set[n]);
	    objCode = code.Evaluator(lst);
	    if(!objCode)
	    {
		    Error(I2("EvalSet failed.","Fallo en EvalSet."));
		    DESTROY(result);
		    return;
	    }
	    LstFastAppend(result, aux, objCode);
	    n++;
      if(BGrammar::StopFlag()) { break; }
	  }
	  while(n<=set.Card());
  }
  contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetEvalSetNth);
DefExtOpr(1, BSetEvalSetNth, "EvalSetNth", 4, 4, "Real Set Set Code",
    "(Real nth, Set args, Set set, Code doit)",
    I2("Returns the set of the successive evaluations of a "
       "function, taking as the k-th argument each element of a set."
       "The rest of arguments are fixed.",
       "Devuelve el conjunto de las evaluaciones sucesivas de "
       "una función, tomando como k-ésimo argumento cada elemento de "
       "un conjunto. El resto de argumentos permanecen fijos."),
    BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetEvalSetNth::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject* objCode, *arg;
  int n = (int)Real (Arg(1));
  BSet& args = Set (Arg(2));
  BSet& set = Set (Arg(3));
  BCode& code = Code(Arg(4));
  BGrammar* gra = code.Grammar();
  BList* aux1 = NIL;
  BList* aux2 = NIL;
  BList* aux3 = NIL;
  BList* result = NIL;
  int i;
  int k = 1;
  int m = args.Card();
  int K = set.Card();
  bool okArg = code.Operator()!=NULL;
  int minArg = 0;
  int maxArg = 0;
  if(okArg)
  {
    minArg = code.Operator()->MinArg();
    maxArg = code.Operator()->MaxArg();
    okArg = (minArg<=m) && (m<=maxArg);
  }
  if(!okArg)
  {
    Error(I2("Wrong argument function in EvalSetNth function calling."
             "It must admit "+m+" arguments.",
             "Función argumento errónea en llamada a la función EvalSetNth."
             "Ésta debe admitir "+m+" argumentos."));
    return;
  }
  if(K)
  {
    do
    {
      aux1 = NIL;
      aux2 = NIL;
      for(i=1;i<=m;i++)
      {
        if(i==n) { arg = set[k]; }
        else     { arg = args[i]; }
        LstFastAppend(aux1, aux2, arg);
      }
      objCode = code.Evaluator(aux1);
      if(!objCode)
      {
        Error(I2("EvalSetNth failed.","Fallo en EvalSetNth."));
        DESTROY(result);
        return;
      }
      LstFastAppend(result, aux3, objCode);
      k++;
      if(BGrammar::StopFlag()) { break; }
    }
    while(k<=K);
  }
  contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetEvalSetOfCode);
DefExtOpr(1, BSetEvalSetOfCode, "EvalSetOfCode", 2, 2, "Set Anything",
	  I2("(Set functions, Anything arg)", "(Set funciones, Anything arg)"),
	  I2("Returns the set of the successive evaluations of a "
	     "set of functions taking the same argument.",
	     "Devuelve el conjunto de las evaluaciones sucesivas de "
	     "un conjunto de funciones tomando el mismo argumento."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetEvalSetOfCode::CalcContens()
//--------------------------------------------------------------------
{
  BSyntaxObject*  objCode;
  BSet&           setOfCode = Set (Arg(1));
  BSyntaxObject*  argument  = Arg(2);
  BList* result=NULL;
  BList* aux=NULL;
  for(int n=1; n<setOfCode.Card(); n++)
  {
    if(setOfCode[n]->Grammar()==GraCode())
    {
      BCode&       code   = Code(setOfCode[n]);
      BGrammar*    gra    = code.Grammar();
      BInt         n      = 1;
      if(code.Operator()->NumArg()!=1)
      {
        Error(I2("Wrong argument function number ",
                 "Función argumento number ")+
              n+" <"+code.Operator()->Identify()+">"+
              I2("in EvalSetOfCode function calling."
                 "Each one must have just one argument.",
                 "errónea en llamada a la función EvalSet."
                 "Cada una debe tener exactamente un argumento."));
        return;
      }
      objCode = code.Evaluator(NCons(argument));
      if(!objCode)
      {
        Error(I2("EvalSetOfCode failed.","Fallo en EvalSetOfCode."));
        DESTROY(result);
        return;
      }
      LstFastAppend(result, aux, objCode);
    }
    if(BGrammar::StopFlag()) { break; }
  }
  contens_.RobElement(result);
}


//--------------------------------------------------------------------
DeclareNoLazzyClass(BSet, BSetTemporary, BSetSelect);
DefExtOpr(1, BSetSelect, "Select", 2, 2, "Set Code",
	  I2("(Set set, Code condition)",
	     "(Set cto, Code condicion)"),
	  I2("Select the elements of a set that satisfy a condition "
	     "given by a function that returns 0 or 1.",
	     "Selecciona los elementos de un conjunto que satisfacen "
	     "una condición dada por una función que devuelve 0 o 1."),
	  BOperClassify::SetAlgebra_);

//--------------------------------------------------------------------
void BSetSelect::CalcContens()
//--------------------------------------------------------------------
{
  //InitTotalTime("BSetSelect::CalcContens()");
  BSyntaxObject* objCode;
  BSet& set = Set (Arg(1));
  BUserCode* uCod = UCode(Arg(2));
  BList* aux = NIL;
  BList* result = NIL;
  BText error;
  BCode& cond = uCod->Contens();
  BGrammar* g = cond.Grammar();
  if(g!=GraReal())
  {
    Error(I2("The condition function for Select should return a Real "
             "object",
             "La función de condición para Select debe devolver un "
             "objeto de tipo Real")+":\n"+
          cond.Operator()->LexInfo());
    return;
  }
  for(BInt n=1; n<=set.Card(); n++)
  {
    objCode  = cond.Evaluator(NCons(set[n]));
    if(!objCode)
    {
      error = I2("[Select] Wrong condition definition.",
                 "[Select] Condición mal definida.");
      Error(error);
      break;
    }
    BDat dat = Dat(objCode);
    BBool valid = dat!=0;
    if(!dat.IsKnown())
    {
      error = I2("[Select] condition function cannot return unknown values.",
                 "[Select] la función de condición no puede devolver valores desconocidos.");
      Error(error);
      DESTROY(objCode);
      break;
    }
    DESTROY(objCode);
    if(valid) { LstFastAppend(result, aux, set[n]); }
    if(BGrammar::StopFlag()) { break; }
  }
  contens_.RobElement(result);
  //SumPartialTime;
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetIncludeText);
DefExtOpr(1, BSetIncludeText, "IncludeText", 1, 1, "Text",
	  I2("(Text expression)", "(Text expresion)"),
	  I2("Analyze the content of valid expressions in TOL separated "
	     "by semicolon and with free format (see IncludeTOL) and "
	     "returns the set of all the objects defined in said "
	     "expressions.",
	     "Analiza el contenido de expresiones válidas en TOL "
	     "separadas por punto y coma y con formato libre (ver "
	     "IncludeTOL) ,y devuelve el conjunto de todos los "
	     "objetos definidos en dichas expresiónes."),
	  BOperClassify::FileIncluding_);
//--------------------------------------------------------------------
void BSetIncludeText::CalcContens()
//--------------------------------------------------------------------
{
    BList* result = MultyEvaluate(Text(Arg(1)));
    contens_.RobStruct(result,NIL,BSet::Generic);
}




//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDates);
DefExtOpr(1, BSetDates, "Dates", 3, 3, "TimeSet Date Date",
	  I2("(TimeSet dating, Date from, Date until)",
	     "(TimeSet fechado, Date desde, Date hasta)"),
	  I2("Returns a set which elements are the dates between two "
	     "dates in a dating.",
	     "Devuelve el conjunto de las fechas entre dos dadas en "
	     "un fechado."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BSetDates::CalcContens()
//--------------------------------------------------------------------
{
  BUserTimeSet* tms    = Tms (Arg(1));
  BDate&	      from   = Date(Arg(2));
  BDate&	      until  = Date(Arg(3));
  BList*	      aux    = NIL;
  BList*	      result = NIL;
  
  if(tms && from.HasValue() && until.HasValue() && (from<=until))
  {
    BDate dte  = from;
    BHash hash; tms->GetHashBetween(hash, dte, until);
    for(int k=0; k<hash.Size(); k++)
   	{
      dte.PutHash(hash[k]); 
      LstFastAppend(result, aux, new BContensDate("",dte,""));
      if(BGrammar::StopFlag()) { break; }
    }
  }
  else
  {
    BText tmsId = tms?tms->GetIdentify():BText("NULL");
	  Error(I2("Wrong dating ","Fechado ")+tmsId+" "+
          I2("or dates for Dates function of Set type between ",
		         "o fechas erróneos en la función Dates de tipo Set entre ")+
          "["+from+","+until+"]");
	}
  contens_.RobElement(result);
}

//--------------------------------------------------
#if defined(__USE_TC__)

DeclareContensClass(BSet, BSetTemporary, BSetDeepInside);
DefExtOpr(1, BSetDeepInside, "cDeepInside", 1, 1, "CTime",
	  "(CTime tmi)",
	  I2("Returns a set whose elements are the CTimes contained into "
	     "the given CTime, with the next lower granularity.",
	     "Devuelve un conjunto cuyos elementos son los CTime contenidos "
	     "en el CTime dado, con la granularidad inmediatamente inferior."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BSetDeepInside::CalcContens()
{
    BCTime uTmi = CTime(Arg(1));
    BList *result=0, *aux=0;
    granularity granul=UNK;

    if(uTmi.getGranul()>SECONDS)
	granul = static_cast<granularity>(
	    static_cast<int>(uTmi.getGranul())-1);
    else
	Error(I2("The given tmi already has the smallest granularity, "
		 "cannot be divided.", "El tmi dado ya tiene la granularidad "
		 "más pequeña. No puede ser dividido."));
    
    if(granul<uTmi.getGranul()) {
	BCTime tmi(uTmi, granul);
	
	while(uTmi.includes(tmi)) {
	    LstFastAppend(result, aux, new BContensCTime("",tmi,""));
	    tmi.successor(1);
	}
    }
    
    contens_.RobElement(result);    
}

#endif /* __USE_TC__ */
//--------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetMat);
DefExtOpr(1, BSetMat, "MatSet", 1, 1, "Matrix",
	  I2("(Matrix matrix)",
	     "(Matrix matriz)"),
	  I2("Converts by rows a matrix in a set of sets of real numbers.",
	     "Convierte por filas una matriz en un conjunto de conjuntos "
	     "de reales."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BSetMat::CalcContens()
//--------------------------------------------------------------------
{
  BMat	mat    = Mat (Arg(1));
  BList*	result = NIL;
  BList*	aux    = NIL;
  BInt	i,j;
  for(i = 0; i < mat.Rows(); i++)
  {
    BList* lsta	  = NIL;
    BList* auxa	  = NIL;
    for(j = 0; j < mat.Columns(); j++)
    { 
      LstFastAppend(lsta, auxa, new BContensDat("",mat(i,j),""));  
      if(BGrammar::StopFlag()) { break; }
    }
    LstFastAppend(result, aux,  NewSet("","",lsta,NIL,BSet::Generic));
    if(BGrammar::StopFlag()) { break; }
  }
  contens_.RobStruct(result,NIL,BSet::Table);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetSerMat);
DefExtOpr(1, BSetSerMat, "MatSerSet", 3, 3, "Matrix TimeSet Date",
	  I2("(Matrix matrix, TimeSet dating, Date firstDate)",
	     "(Matrix matriz, TimeSet fechado, Date fechaInicial)"),
	  I2("Converts by rows a matrix in a set of series with the "
	     "given dating and first date.",
	     "Convierte por filas una matriz en un conjunto de series "
	     "con el fechado y la fecha inicial dadas."),
	  BOperClassify::Conversion_);
//--------------------------------------------------------------------
void BSetSerMat::CalcContens()
//--------------------------------------------------------------------
{
  BMat& mat = Mat (Arg(1));
  BUserTimeSet* dating = Tms (Arg(2));	
  if(!dating) { return; }
  BDate& first = Date(Arg(3));
  BDate last = dating->Next(first,mat.Columns()-1);
  BList*	  result = NIL;
  BList*	  aux    = NIL;
  BInt	  i;
  for(i=0; i<mat.Rows(); i++)
  {
    BTsrPrimary* serie = new BTsrPrimary(
     "","",dating,first,last,mat.SubRow(i).Data());
    serie->PutName(BText("S")+(i+1));
    LstFastAppend(result, aux, serie);
    if(BGrammar::StopFlag()) { break; }
  }
  contens_.RobStruct(result,NIL,BSet::Table);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetDir);
DefExtOpr(1, BSetDir, "GetDir", 1, 1, "Text",
	  "(Text dir)",
	  I2(".",
	     "Dado un directorio del sistema, devuelve un conunto compuesto por un "
	     "conjunto que contiene los subdirectorios y otro con los ficheros."),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetDir::CalcContens()
//--------------------------------------------------------------------
{
  BText	 path = Text(Arg(1));
//Std(BText("\nSearching directory ")+path);
  BDir	 dir(path);
  contens_.Delete();
  if(dir.IsDir())
  {
	  BInt i;
	  BSet dirs, files;
    contens_.PrepareStore(2);
	  files.PrepareStore(dir.NumFiles());
	  dirs .PrepareStore(dir.NumDirs());
  //Std(BText("\nFiles number ")+dir.NumFiles());
  //Std(BText("\nDirectories number ")+dir.NumDirs());
	  for(i = 0; i < dir.NumFiles(); i++)
	  {
      BText f = dir.FileName(i);
      files.AddElement(new BContensText("",f,f));
      if(BGrammar::StopFlag()) { break; }
	  }
	  for(i = 0; i < dir.NumDirs(); i++)
	  {
      BText d = dir.DirName(i);
	    dirs.AddElement(new BContensText("",d,d));
      if(BGrammar::StopFlag()) { break; }
	  }
	  contens_.AddElement(new BContensSet("",files,"Files"));
	  contens_.AddElement(new BContensSet("",dirs, "Directories"));
  }
  else
  {
    Error(I2(BText("Directory ")+path+" is not found",
		         BText("No se encuentra el direcorio ")+path));
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetGetGlobalFunctions);
DefExtOpr(1, BSetGetGlobalFunctions, "GetGlobalFunctions", 1, 1, "Text",
"(Text grammar)",
I2("Returns all global functions returning specified grammar",
	 "Devuelve todas las funciones globales que devuelven el tipo "
   "especificado"),
BOperClassify::System_);
//--------------------------------------------------------------------
void BSetGetGlobalFunctions::CalcContens()
//--------------------------------------------------------------------
{
  BText& graName = Text(Arg(1));
  BGrammar* gra = BGrammar::FindByName(graName, true);
  if(!gra) { return; }
  BList* oprLst = gra->GetOperators();
  BList* codeLst = NULL;
  BList* aux = oprLst;
  while(oprLst)
  {
    BSyntaxObject* obj = (BSyntaxObject*)Car(oprLst);
    if(obj)
    {
      if(obj->Mode()==BOBJECTMODE)
      {
        codeLst = Cons(obj, codeLst);
      }
      else
      {
        BOperator* opr = (BOperator*)obj;
        BUserFunCode* funCode = opr->GetCode();
        if(funCode)
        {
          codeLst = Cons(funCode, codeLst);
        }
      }
    }
    oprLst = Cdr(oprLst);
  };
  DESTROY(aux);
  contens_.RobElement(codeLst);
}


#if !defined(USE_DELAY_INIT)

//--------------------------------------------------------------------
static BStruct* IndexStruct_ = NewStructSymbol
//--------------------------------------------------------------------
(
    "@IndexStruct"
    ,
    "Text:Type,"
    "Text:Name,"
    "Text:Formula,"
    "Text:Description,"
    "Text:Parents"
    );

#else

static void * cloneIndexStruct_()
{
    return  NewStructSymbol( "@IndexStruct"
			     ,
			     "Text:Type,"
			     "Text:Name,"
			     "Text:Formula,"
			     "Text:Description,"
			     "Text:Parents" );
}

static BStruct* IndexStruct_ = (BStruct*)(__delay_init((void**)(&IndexStruct_),
						       &cloneIndexStruct_));

#endif

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetHierarchy);
DefExtOpr(1, BSetHierarchy, "Hierarchy", 1, 1, "Set",
	  I2("(Set index)",
	     "(Set indice)"),
	  I2("",
	     ""
	      ),
	  BOperClassify::System_);

//--------------------------------------------------------------------
void BSetHierarchy::CalcContens()
//--------------------------------------------------------------------
{
    BSet&	 index = Set(Arg(1));
    BInt	 N     = index.Card();
    BList* lst   = NIL;
    BList* aux   = NIL;
    BInt i;
    for(i=1; i<=N; i++)
    {
	BSyntaxObject* uReg = index[i];
	if(uReg->Grammar()==GraSet())
	{
	    BSet& reg = Set(uReg);
	    if(reg.Struct()==IndexStruct_)
	    {
		BText type = Text(reg[1]);
		BText name = Text(reg[2]);
		if(type.HasName() && name.HasName())
		{
		    BGrammar* gra = BGrammar::FindByName(type);
		    if(gra) {
			BText	   formula     = Text(reg[3]);
			BText	   description = Text(reg[4]);
			BText	   parents     = Text(reg[5]);
			BSyntaxObject* obj	       = gra->FindVariable(name);
			if(!obj && formula.HasName()) {
			    obj = gra->EvaluateExpr(name +" = "+formula);
			}
			if(obj) {
			    if(description.HasName()) { 
				obj->PutDescription(description); 
			    }
			}
		    }
		}
	    }
	}
    }
    
    for(i=1; i<=N; i++) {
	BSyntaxObject* uReg = index[i];
	if(uReg->Grammar()==GraSet()) {
	    BSet& reg = Set(uReg);
	    if(reg.Struct()==IndexStruct_) {
		BText type = Text(reg[1]);
		BText name = Text(reg[2]);
		if(type.HasName() && name.HasName()) {
		    BGrammar* gra = BGrammar::FindByName(type);
		    if(gra) {
			BText	   parents = Text(reg[5]);
			BSyntaxObject* obj	   = gra->FindVariable(name);
			if(obj)	{
			    BBool isIndexed = BFALSE;
			    if(parents.HasName()) {
				BArray<BText> tok;
				ReadAllTokens(parents,tok,',');
				for(BInt j=0; j<tok.Size(); j++) {
				    BSyntaxObject* p = GraSet()->FindVariable(Compact(tok[j]));
				    if(p && (p->Grammar()==GraSet())) {
					USet(p)->Contens().AddElement(obj);
					isIndexed = BTRUE;
				    }
				}
			    }
			    if(!isIndexed) {
				LstFastAppend(lst,aux,obj);
			    }
			}
		    }
		}
	    }
	}
    }
    contens_.RobElement(lst);
}

//--------------------------------------------------------------------
BUserSet* ParsingTreeToSet( const List* ATree )
    
/*! Translates a parser-generated syntax tree into a set of sets
 *  structure.
 */
//--------------------------------------------------------------------
{
    BToken*		Node;	       // node contens of ATree
    int			BranchIdx;     // Current branch index of ATree
    List*		ABranch;       // Current branch of Atree
    BUserSet*		BranchesAsSet; // Translation of branches into a set
    BUserSet*		BranchAsSet;   // Current branch as set
    BUserText*		Symbol;	       // Parsed symbol at ATree node
    BUserText*		Type;	       // Parsed symbol type at ATree node
    BUserSet*		ASet;	       // ATree once translated
    
    if (ATree && ATree->IsList() )
    {
	Node	      = BParser::treToken(ATree);
	BranchesAsSet = new BContensSet;

	// Translate symbol and type
	Symbol = new BContensText("",Node->Name());
	switch(Node->TokenType())
	{
	    case NONE:	    Type = new BContensText("","none");      break;
	    case OPEN:	    Type = new BContensText("","open");      break;
	    case CLOSE:	    Type = new BContensText("","close");     break;
	    case SEPARATOR: Type = new BContensText("","separator"); break;
	    case MONARY:    Type = new BContensText("","monary");    break;
	    case BINARY:    Type = new BContensText("","binary");    break;
	    case ARGUMENT:  Type = new BContensText("","argument");  break;
	    case FUNCTION:  Type = new BContensText("","function");  break;
	    case TYPE:	    Type = new BContensText("","type");      break;
	    default:	      Type = new BContensText("","unknown");   break;
	}

	// Enter subtree and translate each branch
	BranchIdx = 1;
//	ABranch	= TreBranch((BList *)ATree,BranchIdx);
	ABranch = Tree::treBranch((List*)ATree, BranchIdx);
	while (ABranch)
	{
	    // Translate current branch
	    BranchAsSet = ParsingTreeToSet(ABranch);
	    BranchesAsSet->Contens().AddElement(BranchAsSet);
	    // Next Branch
	    BranchIdx++;
//	    ABranch   = TreBranch((BList*)ATree,BranchIdx);
	    ABranch = Tree::treBranch((List*)ATree, BranchIdx);
    if(BGrammar::StopFlag()) { break; }
	} // end while
	
	// Translate all node stuff into a new set
	ASet	    = new BContensSet;
	ASet->PutDescription(Node->Name());
	ASet->Contens().AddElement(Symbol);
	ASet->Contens().AddElement(Type);
	ASet->Contens().AddElement(BranchesAsSet);
  if(Node->Close())
  {
    BUserSet*	brackets  = new BContensSet;
	  BContensText* close = new BContensText("",Node->Close()->Name());
	  BContensText* open  = new BContensText("",Node->Close()->Open()->Name());
    brackets->Contens().AddElement(open);
    brackets->Contens().AddElement(close);
  	ASet->Contens().AddElement(brackets);
  }
	// return new set
	return ASet;
	
    }  // end if (Atree
    
    // return Empty set
    return NULL;
    
} // end ParsingTreeToSet


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetParse);
DefExtOpr(1, BSetParse, "Parse", 1, 1, "Text",
	  I2("(Text Expression)", "(Text Expresion)"),
	  I2("Creates a set corresponding to the given TOL Expression "
	     "syntax tree using the following format: the first element "
	     "will be a symbol name, the second one will be the "
	     "symbol's type, the third one will be a set of trees. "
	     "Each of those trees will be the corresponding translation of "
	     "each symbol's argument."
	     ,
	     "Devuelve un conjunto representando el arbol sintactico "
	     "correspondiente a Expresion.Dicho conjunto contiene: "
	     "un simbolo como primer elemento, a continuacion el tipo "
	     "de dicho simbolo,y por ultimo, un conjunto. "
	     "Este conjunto contiene un arbol sintactico por cada "
	     "argumento del simbolo."
	      ),
	  BOperClassify::SetAlgebra_);
//--------------------------------------------------------------------
void BSetParse::CalcContens()
//--------------------------------------------------------------------
{
    Tree* ParsingTree = BParser::parsing(Text(Arg(1)));
    List* tree = ParsingTree->getConstTree();
    BUserSet  *TreeAsSet;
    if (ParsingTree && tree)
    {
	// There were no parsing errors, translate
	TreeAsSet = ParsingTreeToSet(tree);
	contens_  = TreeAsSet->Contens();
	delete ParsingTree;
	delete TreeAsSet;
    }
}

static
Tcl_EvalPtr tcl_evaluator = NULL;

//--------------------------------------------------------------------
int TolTclIsEnabled()
//--------------------------------------------------------------------
{
  return(tcl_evaluator!=NULL);
}

//--------------------------------------------------------------------
Tcl_EvalPtr InstallTclEval(Tcl_EvalPtr new_evaluator)
//--------------------------------------------------------------------
{
  Tcl_EvalPtr old_evaluator = tcl_evaluator;

  tcl_evaluator = new_evaluator;
  return old_evaluator;
}

static
Tcl_EvalExPtr tcl_evaluatorEx = NULL;

//--------------------------------------------------------------------
Tcl_EvalExPtr InstallTclEvalEx(Tcl_EvalExPtr new_evaluator)
//--------------------------------------------------------------------
{
  Tcl_EvalExPtr old_evaluator = tcl_evaluatorEx;

  tcl_evaluatorEx = new_evaluator;
  return old_evaluator;
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetTclEval);
DefExtOpr(1, BSetTclEval, "Tcl_Eval", 1, 1, "Text",
          I2("(Text script)", "(Text script)"),
          I2("Evaluate a Tcl script. The return value is a Set with two elements\n"
             "    status : a Real indicating success (1) or fail (0)\n"
             "    result : a Text with the evaluation result\n"
             "Example:\n\n"
             "    Set tcl_res = Tcl_Eval(\"set tcl_platform(platform)\");\n"
             "    If (tcl_res[\"status\"], WriteLn(\"Running on platform \"+tcl_res[\"result\"]), WriteLn(\"Error in Tcl_Eval\"))",
             "Evalua un script Tcl. El valor de retorno es un Real con dos elementos\n"
             "    status : un Real indicando éxito (1) or fallo (0)\n"
             "    result : un Text con el resultado de la evaluación\n"
             "Ejemplo:\n\n"
             "    Set tcl_res = Tcl_Eval(\"set tcl_platform(platform)\");\n"
             "    If (tcl_res[\"status\"], WriteLn(\"Ejecutando en plataforma \"+tcl_res[\"result\"]), WriteLn(\"Error en Tcl_Eval\"))"),
          BOperClassify::System_);
//--------------------------------------------------------------------
void BSetTclEval::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Tcl_Eval")) { return; }
  BText &script = Text(Arg(1));
  BList * result = NIL;
  BSyntaxObject * so;
  BInt status = 0;
  const char * str_result = "Unimplemented";
  
  if (tcl_evaluator)
    status = (*tcl_evaluator)(script.Buffer(), &str_result);
  
  result = Cons(so = new BContensDat("", status), result);
  so->PutName("status");
  result = Cons(so = new BContensText("", str_result), result);
  so->PutName("result");
  contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetTclEvalEx);
DefIntOpr(1, BSetTclEvalEx, "Tcl_EvalEx", 1, 1,
          "(Set Args)",
          I2("Evaluate a Tcl script. The return value is a Set with two elements\n"
             "    status : a Real indicating success (1) or fail (0)\n"
             "    result : a Text with the evaluation result\n\n"
             "The Tcl scripts is given as a Set in the form of:\n"
             "    cmd arg1 arg2 ... argn\n\n"
             "Example:\n\n"
             "    Set tcl_res = Tcl_EvalEx([[\"set\", \"tcl_platform(platform)\"]]);\n"
             "    If (tcl_res[\"status\"], WriteLn(\"Running on platform \"+tcl_res[\"result\"]), WriteLn(\"Error in Tcl_Eval\"))",
             "Evalua un script Tcl. El valor de retorno es un Real con dos elementos\n"
             "    status : un Real indicando éxito (1) or fallo (0)\n"
             "    result : un Text con el resultado de la evaluación\n\n"
             "The script es especificado mediante un Set con el siguiente significado:\n"
             "    cmd arg1 arg2 ... argn\n\n"
             "Ejemplo:\n\n"
             "    Set tcl_res = Tcl_EvalEx([[\"set\", \"tcl_platform(platform)\"]]);\n"
             "    If (tcl_res[\"status\"], WriteLn(\"Ejecutando en plataforma \"+tcl_res[\"result\"]), WriteLn(\"Error en Tcl_Eval\"))"),
          BOperClassify::System_);
//--------------------------------------------------------------------
void BSetTclEvalEx::CalcContens()
//--------------------------------------------------------------------
{
  if(CheckNonDeclarativeAction("Tcl_EvalEx")) { return; }
  BSet &cmd = Set(Arg(1));
  BList * result = NIL;
  BSyntaxObject * so;
  BInt status = 0;
  const char *str_result = "Unimplemented";
  
  if (tcl_evaluatorEx)
    status = (*tcl_evaluatorEx)(cmd, &str_result);
  
  result = Cons(so = new BContensDat("", status), result);
  so->PutName("status");
  result = Cons(so = new BContensText("", str_result), result);
  so->PutName("result");
  contens_.RobElement(result);
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetLLKR);
DefExtOpr(1, BSetLLKR, "LLKNoise", 3, 5, "Matrix Polyn Polyn Real Real",
          "(Matrix R, Polyn pAR, Polyn pMA [,Real sigma2=0, Real k=0])",
          I2("Compute loglikelihood for Noise R where R follows an ARMA model.\n"
             "The resulting Set  contains the estimate of the variance of the residuals as the first item and the loglikelihood as the second item.\n"
             "Example:\n\n"
             "    Matrix w = Gaussian(350,1,0,1);\n"
             "    Polyn pAR = 1;\n"
             "    Polyn pMA = 1+0.2*B+0.5*B^2;\n"
             "    Ration pR = pMA/pAR;\n"
             "    Matrix N = DifEq(pR, w);\n"
             "    Set llk = LLKNoise(N,pAR,pMA);\n",
             "Calcula el log de la verosimilitud de un ruido R con estructura ARMA.\n"
             "El conjunto resultado contiene en el primer elemento el valor estimado de la varianza de los residuos y el valor de del log de la verosimilitud en el segundo elemento.\n"
             "Ejemplo:\n\n"
             "    Matrix w = Gaussian(350,1,0,1);\n"
             "    Polyn pAR = 1;\n"
             "    Polyn pMA = 1+0.2*B+0.5*B^2;\n"
             "    Ration pR = pMA/pAR;\n"
             "    Matrix N = DifEq(pR, w);\n"
             "    Set llk = LLKNoise(N,pAR,pMA);\n"),
          BOperClassify::Conversion_);

void BSetLLKR::CalcContens()
//--------------------------------------------------------------------
{
  BMat * R = &Mat(Arg(1));
  BPol * pAR = &Pol(Arg(2));
  BPol * pMA = &Pol(Arg(3));
  BDat * pS2 = NULL;
  BDat * pK = NULL;
  BList * result = NIL;
  BSyntaxObject * so;
  
  double sigma2 = 0.0;
  int k = 0;
  
  if (!R || R->Columns()!=1 || R->Rows()==0) {
    Error(I2("Invalid R matrix, should be a column vector",
             "Matriz R invalida, debe ser un vector columna"));
    return;
  }
  if (!pAR) {
    Error(I2("Invalid pAR polynomial",
             "Polinomio pAR inválido"));
    return;
  }
  if (!pMA) {
    Error(I2("Invalid pMA polynomial",
             "Polinomio pMA inválido"));
    return;
  }
  if (NumArgs()>3) {
    pS2 = &Dat(Arg(4));
    sigma2 = pS2->Value();
    if (!pS2->IsKnown() || sigma2<0) {
      Warning(I2("In LLKNoise: sigma2 argument must non negative. Assuming 0",
                 "En LLKNoise: argumento k debe ser no negativo. Asumiendo 0"));
      sigma2 = 0.0;
    }    
    if (NumArgs()>4) {
      pK = &Dat(Arg(5));
      k = int(pK->Value());
      if (!pK->IsKnown() || k<0) {
        Warning(I2("In LLKNoise: k argument must be non negative. Assuming 0",
                   "En LLKNoise: argumento k debe ser no negativo. Asumiendo 0"));
        k = 0;
      }
    }
  }
  
  gsl_vector * gsl_R = gsl_vector_alloc(R->Rows());
  gsl_vector * gsl_pAR = gsl_vector_alloc(pAR->Degree()+1);
  gsl_vector * gsl_pMA = gsl_vector_alloc(pMA->Degree()+1);
  int r;
  
  for (r = 0; r < R->Rows(); r++)
    gsl_vector_set(gsl_R, r, (*R)(r,0).Value());
  gsl_vector_set_zero(gsl_pAR);
  gsl_vector_set(gsl_pAR,0,(*pAR)(0).Coef().Value());
  for (r = 1; r < pAR->Size(); r++)
    gsl_vector_set(gsl_pAR, (*pAR)(r).Degree(), -(*pAR)(r).Coef().Value());
  gsl_vector_set_zero(gsl_pMA);
  gsl_vector_set(gsl_pMA,0,(*pMA)(0).Coef().Value());
  for (r = 1; r < pMA->Size(); r++)
    gsl_vector_set(gsl_pMA, (*pMA)(r).Degree(), -(*pMA)(r).Coef().Value());
  result = Cons(so = new BContensDat("", LLKR(gsl_R, gsl_pAR, gsl_pMA, sigma2, k)),
                result);
  so->PutName("llk");
  result = Cons(so = new BContensDat("", sigma2), result);
  so->PutName("sigma2");
  contens_.RobElement(result);
  gsl_vector_free(gsl_R);
  gsl_vector_free(gsl_pAR);
  gsl_vector_free(gsl_pMA);
}


//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetClassAscentOf);
DefExtOpr(1, BSetClassAscentOf, "ClassAscentOf", 1, 1,
	  "Text",
	  "(Text className)",
	  I2("Returns the ascent classes of a given class\"\"",
	     "Retorna las clases ascendientes de una clase dada \"\""),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BSetClassAscentOf::CalcContens()
//--------------------------------------------------------------------
{
  const BText& name = Text(Arg(1));
  BClass* found = FindClass(name,1);
  if(found)
  {
    BMemberOwner::BClassByNameHash* hash = found->ascentHash_;
    BMemberOwner::BClassByNameHash::const_iterator iterC;
    contens_.PrepareStore(hash->size());
    for(iterC=hash->begin(); iterC!=hash->end(); iterC++)
    {
      BText fn = iterC->second->FullName();
    //Std(BText(""\nBOisCreator::Write BMemberOwner parent ")+fn);
      contens_.AddElement(new BContensText("",fn,""));
      if(BGrammar::StopFlag()) { break; }
    }
  }
}

//--------------------------------------------------------------------
DeclareContensClass(BSet, BSetTemporary, BSetPExec);
DefExtOpr(1, BSetPExec, "PExec", 1, 1,
	  "Set", 
	  "(Set Args)",
	  I2("Execute an external program. The output of the program is returned in the member output.",
	     "Ejecuta un programa externo. la salida del programa se retorna en el miembro ouput del conjunto resultado."),
	  BOperClassify::System_);

#ifdef WIN32
void escapeCmdArg( const char *input, BText &output )
{
  const char *pCh = input;
  int bs = 0;
  int need = 0;
  
  while ( *pCh ) {
    if ( !bs && *pCh == ' ' ) {
      need = 1;
      break;
    }
    bs = ((*pCh == '\\') && !bs);
    ++pCh;
  }
  if ( need ) {
    //Std( BText( "Needs protection\n" ) );
    output = "\"";
    output += input;
    output += "\"";
  } else {
    output = input;
  }
}
#else
void escapeCmdArg( const char *input, BText &output )
{
  const char *pCh = input;
  output = "";
  int bs = 0;
  
  while ( *pCh ) {
    if ( !bs && *pCh == ' ' ) {
      output += "\\ ";
    } else {
      output += *pCh;
    }
    bs = ((*pCh == '\\') && !bs);
    ++pCh;
  }
}
#endif

//--------------------------------------------------------------------
void BSetPExec::CalcContens()
//--------------------------------------------------------------------
{
  const BSet& args = Set(Arg(1));
  BDat status;
  BText desc, output;
  if ( args.Card() ) {
    BText cmd;
    int badArg = 0;
    for ( int i = 1; i <= args.Card(); i++ ) {
      BSyntaxObject* arg  = args[ i ];
      if ( arg->Grammar() != GraText() ) {
        status = 0;
        desc = I2( "all argument must be of type Text",
                   "todos los argumentos deben ser de tipo texto" );
        badArg = 1;
      }
      BText &_arg = Text( arg ), escapedArg;
      escapeCmdArg( _arg, escapedArg );
      if ( i > 1 ) {
        cmd += " ";
      }
      cmd += escapedArg;
    }
    if ( !badArg ) {
      //Std( BText( "*** '" ) + cmd + "' ***" );
      FILE* pipe = popen(cmd, "r");
      if ( pipe ) {
        char buffer[128];
        output = "";
        while( !feof( pipe ) ) {
          if( fgets( buffer, 128, pipe ) != NULL )
            output += buffer;
        }
        status = 1;
        desc = "";
        pclose( pipe );
      }
    } else {
      status = 0;
      BText desc = I2( "Could not execute: ", "No se pudo ejecutar: " );
      desc += cmd; 
      BText output = "";
    }
  } else {
    status = 0;
    BText desc = I2( "Nothing to execute", "Nada que ejecutar" );
    BText output = "";
  }
  BList * result = NIL;
  BSyntaxObject * so;
  result = Cons(so = new BContensDat("", status), result);
  so->PutName("status");
  result = Cons(so = new BContensText("", desc), result);
  so->PutName("description");
  result = Cons(so = new BContensText("", output), result);
  so->PutName("output");
  contens_.RobElement(result);  
}
