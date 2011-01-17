/* codgra.cpp: Set grammars functions of GNU/TOL language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bcodgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bspfun.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_btxtgra.h>

//static const char* bayesVersionId =

//--------------------------------------------------------------------
// Codeic variables
//--------------------------------------------------------------------
BTraceInit("codgra.cpp");
template <>
BGrammar* BGraContensBase<BCode>::ownGrammar_ = NIL;

DefineContensCommonOperators(BCode, "Code");

//--------------------------------------------------------------------
  template<>
  void BGraContensBase<BCode>::Do() 
//--------------------------------------------------------------------
{ 
  Contens(); 
}

//--------------------------------------------------------------------
  BUserFunCode::BUserFunCode()
//--------------------------------------------------------------------
: BContensCode() 
{
}

//--------------------------------------------------------------------
  BUserFunCode::BUserFunCode(const BText& name, 
                             const BCode& aCode, 
                             const BText& desc)
//--------------------------------------------------------------------
: BContensCode(name, aCode, desc) 
{
  aCode.Operator()->PutCode(this);
}

//--------------------------------------------------------------------
BUserFunCode::BUserFunCode(const BText& name, 
                           BStandardOperator* opr, 
                           const BText& desc)
//--------------------------------------------------------------------
: BContensCode() 
{
  PutName(name);
  PutDescription(desc);
  contens_.PutOperator(opr);
  BGrammar::AddObject(this);
}

//--------------------------------------------------------------------
BOperator* GetOperator(BUserCode* uCode)
//--------------------------------------------------------------------
{
  assert(uCode);
  int m = uCode->Mode();
/*
  Std(BText("\nGetOperator(")+uCode->Name()+")"+
  " Mode="+uCode->ModeName()+
  " Type="+uCode->Grammar()->Name());
*/
  BOperator* opr = NIL;
  if((m==BBUILTINFUNMODE)||(m==BUSERFUNMODE)) 
  { 
    opr = (BOperator*) uCode;     
  }
  else if((m==BOBJECTMODE)&&(uCode->Grammar()==GraCode()))                     
  { 
    opr = Code(uCode).Operator(); 
  }
  else 
  {
    assert(0);
  }
  return(opr);
}


//--------------------------------------------------------------------
template<>
void BGraContensBase<BCode>::InitInstances()
    
/*! Initializes the TOL system instances of this type as static
 *  of this function. This function is called from InitGrammar to
 *  forze all statics instances of this file are presents after
 *  InitGrammar is called.
 */
//--------------------------------------------------------------------
{
    BTraceInit("BGraContens<BCode>::InitInstances");
    // There aren't system instances of this type
    BUserFunCode* unknown_ = new BUserFunCode
	(
	    "UnknownCode",
	    NIL,
	    I2("Unknown Code.", "Funcion desconocida")
	    );
    OwnGrammar()->PutDefect(unknown_);
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BCode>::FindConstant (const BText& name)

/*! There aren't any constant in Code grammar
 */
//--------------------------------------------------------------------
{
    return(NIL);
}


//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContensBase<BCode>::Casting(BSyntaxObject* obj)

/*! Returns a valid code for obj.
 */
//--------------------------------------------------------------------
{
    if(!obj)			     { return(NIL); }
    if(obj->Grammar()==OwnGrammar()) { return(obj); }
    return(NIL);
}

//--------------------------------------------------------------------
template<>
BSyntaxObject* BGraContens<BCode>::New(FILE* fil)
{
    return 0;
}

//--------------------------------------------------------------------
// Algebraic temporary class declarations.
//--------------------------------------------------------------------


//--------------------------------------------------------------------
DeclareContensClass(BCode, BCodeTemporary, BCodeFindOperator);
DefExtOpr(1, BCodeFindOperator, "FindCode", 2, 2, "Text Text",
	  I2("(Text rType, Text fName)",
	     "(Text tipoR, Text nombreF)"),
	  I2("Seeks a function whose name will be fName and that "
	     "returns an object with type rType.\n\n"
	     "Example: \n"
	     "Code f = FindCode(\"Real\",\"+\");\n\n"
	     "Result:\n"
	     "f(3,4) == 3+4",
	     "Busca una función cuyo nombre sea nombreF y que devuelva "
	     "un objeto de tipo tipoR.\n\n"
	     "Ejemplo: \n"
	     "Code f = FindCode(\"Real\",\"+\");\n\n"
	     "Resultado:\n"
	     "f(3,4) == 3+4"),
	  BOperClassify::General_);

//--------------------------------------------------------------------
void BCodeFindOperator::CalcContens()
//--------------------------------------------------------------------
{
  BGrammar* gra  = Gram(Text(Arg(1)));
  if(gra)
  {
    BText name = Text(Arg(2));
    const BSpecialFunction * spf = BSpecialFunction::Get(name);
    if(spf)
    {
      contens_.PutSpecialFunction(spf);
    }
    else
    {
      BUserFunction* opr = (BUserFunction*)gra->FindOperator(name);
      contens_.PutOperator(opr);
    }
  }
  else
  {
    contens_.PutOperator(NIL);
  }
}


/*
#include <bpolgra.h>
#include <bratgra.h>
//////////////////////////////////////////////////////////////////////////////
   class BRRPolyn : public BRRFunction

// PURPOSE: Real to Real polynomial functions.
//
//////////////////////////////////////////////////////////////////////////////
{
private :
  BPol pol_;

public:
  BRRPolyn(const BPol& pol) : BRRFunction(), pol_(pol) { }
  void	Evaluate(BDat& y, const BDat& x)  { y = pol_.Eval(x); }
};


//////////////////////////////////////////////////////////////////////////////
   class BRRRatio : public BRRFunction

// PURPOSE: Real to Real rational functions.
//
//////////////////////////////////////////////////////////////////////////////
{
private :
  BRat rat_;
public:
  BRRRatio(const BRat& rat): BRRFunction(), rat_(rat) { }
  void	Evaluate(BDat& y, const BDat& x)  { y = rat_.Eval(x); }
};



//////////////////////////////////////////////////////////////////////////////
  DeclareContensClass(BCode, BCodeTemporary, BCodePolinomial);
  DefExtOpr(1, BCodePolinomial, "Polynomial", 1, 1, "Polyn",
  "(Polyn pol)",
  I2("",
     "Devuelve la función polinomial real correspondiente al polinomio dado"),
  BOperClassify::Conversion_);
  void BCodePolinomial::CalcContens()
//////////////////////////////////////////////////////////////////////////////
{
  contens_ = BPol
}

*/
