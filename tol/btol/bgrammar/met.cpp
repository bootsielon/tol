/* met.cpp: Member functions of BMethod.
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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bmethod.h>
#include <tol/tol_bout.h>
#include <tol/tol_bgrammar.h>
#include <tol/tol_bsetgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bsys.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BMethod).
//--------------------------------------------------------------------
BTraceInit("met.cpp");

static bool buildingMethod_ = false;
bool BuildingMethod() {  return(buildingMethod_); }

//static BMethodView	  implementatorOfBMethodView(NIL);
//static BMethodInfoTable implementatorOfBMethodInfoTable(NIL);


//--------------------------------------------------------------------
BInt BMethod::Mode() const { return(BNOMODE); }
BGrammar* BMethod::Grammar() const { return(grammar_); }
BMethodView::BMethodView(BGrammar* grammar)
: BMethod("View", grammar) { }
BMethodShow::BMethodShow(BGrammar* grammar)
: BMethod("Show", grammar) { }
//--------------------------------------------------------------------

//--------------------------------------------------------------------
BMethod::BMethod(const BText& name, BGrammar* grammar)

/*! BMethod constructor.
 */
//--------------------------------------------------------------------
    : BSyntaxObject(name), grammar_(grammar), function_(NIL)
{
    if(grammar_)
    {
	grammar_->AddMethod(this);
    }
    else
    {
	Error(I2(" FATAL : Cannot create a method without grammar:" + name,
 	      " FATAL : No se puede crear un método sin gramática:" + name));
    }
}


//--------------------------------------------------------------------
BMethod::BMethod(const BText& name, const BText& grammar)

/*! BMethod constructor.
 */
//--------------------------------------------------------------------
    : BSyntaxObject(name), grammar_(NIL), function_(NIL)
{
    if((grammar_= Gram(grammar)))
    {
	grammar_->AddMethod(this);
	BuildFunction();
    }
    else
    {
	Error(I2(" FATAL : Cannot create a method without grammar:" + name,
 	      " FATAL : No se puede crear un método sin gramática:" + name));
    }
}


//--------------------------------------------------------------------
BMethod::~BMethod()

/*! BMethod destructor.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
void	 BMethod::Apply(BList* lst) const

/*! Applys this method with a temporary file
 */
//--------------------------------------------------------------------
{ 
    Apply(lst, BSys::TempNam()); 
}


//--------------------------------------------------------------------
void BMethod::DestroyFunction()

/*! BMethod destructor.
 */
//--------------------------------------------------------------------
{
//Trace(BText("Destroying method function")+
//	Name()+" of grammar "+Grammar()->Name());
    DESTROY(function_);
}


//--------------------------------------------------------------------
void BMethod::BuildFunction()

/*! Builds a user function in order to get a faster access to this
 *  method. User can write MyMethod(MySet) instead of
 *  Apply("MyMethod",MySet)
 */
//--------------------------------------------------------------------
{
  buildingMethod_ = true;
  if(!function_)
  {
//  Std(BText("\nBuilding method function ")+Name()+" to grammar "+Grammar()->Name()+" ... ");
    BText	   name	    = Name();
    BText	   codeName = Replace(name," ","");
    BSyntaxObject* fun	    = NIL;
    if(!(fun=GraCode()->FindVariable(codeName)))
    {
      BText expression;
      expression = BText("Set ")+codeName+" (Set s, Text file)"
		   "{ Set Apply(\""+name+"\",s,file)}";
      fun=GraCode()->EvaluateExpr(expression);
//    Trace(BText("Building access function for method ")+Name());
    }
    if(fun)
    {
      fun->PutDescription(I2("Apply the method ", "Aplica el metodo ")+ name +
			  I2(" to a set of objects", " a un conjunto de objetos"));
//    Trace(BText("Found access function for method ")+
//	    name+" of grammar "+Grammar()->Name());
      function_=NCons(fun);
    }
    else
    {
      Error(I2("Cannot build access function for method ",
	       "No se pudo construir la función de acceso al método ")+
	    name+I2(" of grammar "," de la gramática ")+Grammar()->Name());
    }
  }
  buildingMethod_ = false;
}


//--------------------------------------------------------------------
BBool BMethod::Validate(BList* lst) const

/*! BMethod validator.
 */
//--------------------------------------------------------------------
{
    BBool isValid = BTRUE;
    while(lst && isValid)
    {
	if(((BSyntaxObject*)Car(lst))->Grammar()!=Grammar())
	{ isValid = BFALSE; }
    }
    return(isValid);
}




//--------------------------------------------------------------------
void BMethodView::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
    while(lst)
    {
	BSyntaxObject* car = (BSyntaxObject*)Car(lst);
	if(car)	{
	    Std( BText("\n") + car->Identify() + "=" + car->Dump());
	} else { 
	    Error(BText(I2("Can't apply method ",
			   "No se puede aplicar el método "))+
		  Name()+I2(" over none object", " sin objetos")); 
	}
	lst=Cdr(lst);
    }
}


//--------------------------------------------------------------------
void BMethodShow::Apply(BList* lst, const BText& fileName) const
//--------------------------------------------------------------------
{
    {
	ofstream out(fileName.String());
	BText uName, dump;
	while(lst)
	{
	    BSyntaxObject* car = (BSyntaxObject*)Car(lst);
	    if(car) {
		dump  = car->Dump();
		out << "\n" <<	dump.String() << "\n";
	    } else {
		Error(I2("Can't apply method ", 
			 "No se puede aplicar el método ")+
		      Name()+I2(" over none object", " sin objetos"));
	    }
	    lst=Cdr(lst);
	}
    }
    BSys::EditUnlink(fileName,0);
}

