/* datgralg.cpp: Real Grammar's logic functions
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

#include <tol/tol_bdatgra.h>
#include <tol/tol_bout.h>
#include <tol/tol_bparser.h>
#include <tol/tol_blanguag.h>

//--------------------------------------------------------------------
// Forze linker functions
//--------------------------------------------------------------------
BBool ForzeLinkerDatGraLogic() { return(BTRUE); }

BTraceInit("datgralg.cpp");

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsUnknown);
  DefIntOpr(2, BDatIsUnknown, "IsUnknown", 1, 1,
  "(Real x)",
  I2("Returns true if x is a unknown number.",
     "Devuelve true si x es un número desconocido."),
  BOperClassify::Logic_);
  void BDatIsUnknown::CalcContens()
//--------------------------------------------------------------------
{ contens_ = !(Dat(Arg(1)).IsKnown()); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsFinite);
  DefIntOpr(2, BDatIsFinite, "IsFinite", 1, 1,
  "(Real x)",
  I2("Returns true if x is a valid finite number, it's to say, it "
     "is not infinite nor unknown.",
     "Devuelve true si x es un número finito válido, es decir, si no "
     "es infinito ni indeterminado."),
  BOperClassify::Logic_);
  void BDatIsFinite::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)).IsFinite(); }

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsPosInf);
  DefIntOpr(2, BDatIsPosInf, "IsPosInf", 1, 1,
  "(Real x)",
  I2("Returns true if x is the positive infinite number."
     "In any other case returns false, including unknown.",
     "Devuelve true si x es el infinito positivo."
     "En cualquier otro caso devuelve falso, incluyendo el desconocido."),
  BOperClassify::Logic_);
  void BDatIsPosInf::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)).IsPosInf(); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsNegInf);
  DefIntOpr(2, BDatIsNegInf, "IsNegInf", 1, 1,
  "(Real x)",
  I2("Returns true if x is the negative infinite number."
     "In any other case returns false, including unknown.",
     "Devuelve true si x es el infinito negativo."
     "En cualquier otro caso devuelve falso, incluyendo el desconocido."),
  BOperClassify::Logic_);
  void BDatIsNegInf::CalcContens()
//--------------------------------------------------------------------
{ contens_ = Dat(Arg(1)).IsNegInf(); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatIsInteger);
  DefIntOpr(2, BDatIsInteger, "IsInteger", 1, 1,
  "(Real x)",
  I2("Returns true if x is a integer number.",
     "Devuelve true si x es un número entero."),
  BOperClassify::Logic_);
  void BDatIsInteger::CalcContens()
//--------------------------------------------------------------------
{
  BDat& dat = Dat(Arg(1));
  if(dat.IsUnknown()) { contens_.PutKnown(false); }
  else                { contens_ = dat.IsInteger(); }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNot);
  DefIntOpr(2, BDatNot, "Not", 1, 1,
  "(Real x)",
  I2("Returns false if x is true and conversely.",
     "Devuelve falso si x es verdadero y viceversa."),
  BOperClassify::Logic_);
  void BDatNot::CalcContens()
//--------------------------------------------------------------------
{
  BDat& dat = Dat(Arg(1));
  if(dat.IsUnknown()) { contens_.PutKnown(false); }
  else                { contens_ = dat.Value()==0; }
}
//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatCompare);
  DefExtOpr(1, BDatCompare, "Compare", 2, 2, "Anything Anything",
  "(Anything var1, Anything var2)",
  I2("Returns - 1, 0, or 1 according to will be the first argument smaller, "
     "equal or greater than the second one.",
     "Devuelve -1, 0, o 1 según sea el primer argumento menor, igual o mayor "
     "que el segundo."),
  BOperClassify::Logic_);
  void BDatCompare::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGt2);
  DefExtOpr(1, BDatGt2, ">", 2, 2, "Anything Anything",
  "var1 > var2 {Anything var1, Anything var2}",
  I2("Returns true if the first argument is greater than the second.",
     "Devuelve verdadero si el primer argumento es mayor que el segundo."),
  BOperClassify::Logic_);
  void BDatGt2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Gt(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()>0; }
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLt2);
  DefExtOpr(1, BDatLt2, "<", 2, 2, "Anything Anything",
  "var1 < var2 {Anything var1, Anything var2}",
  I2("Returns true if the first argument is less than the second.",
     "Devuelve verdadero si el primer argumento es menor que el segundo."),
  BOperClassify::Logic_);
  void BDatLt2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Lt(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()<0; }
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGe2);
  DefExtOpr(1, BDatGe2, ">=", 2, 2, "Anything Anything",
  "var1 >= var2 {Anything var1, Anything var2}",
  I2("Returns true if the first argument is greater or just as the second "
     "one.",
     "Devuelve verdadero si el primer argumento es mayor o igual que el "
     "segundo."),
  BOperClassify::Logic_);
  void BDatGe2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Ge(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()>=0; }
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLe2);
  DefExtOpr(1, BDatLe2, "<=", 2, 2, "Anything Anything",
  "var1 <= var2 {Anything var1, Anything var2}",
  I2("Returns true if the first argument is smaller or just as the second "
     "one.",
     "Devuelve verdadero si el primer argumento es menor o igual que el "
     "segundo."),
 BOperClassify::Logic_);
  void BDatLe2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Le(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()<=0; }
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatEq2);
  DefExtOpr(1, BDatEq2, "==", 2, 2, "Anything Anything",
  "var1 == var2 {Anything var1, Anything var2}",
  I2("Returns true if both arguments are equal.",
     "Devuelve verdadero si ambos argumentos son iguales."),
  BOperClassify::Logic_);
  void BDatEq2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Eq(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()==0; }
  }
}


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNe2);
  DefExtOpr(1, BDatNe2, "!=", 2, 2, "Anything Anything",
  "var1 != var2 {Anything var1, Anything var2}",
  I2("Returns true if both arguments are different.",
     "Devuelve verdadero si ambos argumentos son diferentes."),
  BOperClassify::Logic_);
  void BDatNe2::CalcContens()
//--------------------------------------------------------------------
{
  if(Arg(1)->Grammar()==GraReal())
  {
    contens_ = Ne(Dat(Arg(1)),Dat(Arg(2)));
  }
  else
  {
    contens_ = Arg(1)->Grammar()->Compare(Arg(1),Arg(2));
    if(contens_.IsKnown()) { contens_ = contens_.Value()!=0; }
  }
}

//--------------------------------------------------------------------
// When short-circuited logic is disabled old methods are used
//--------------------------------------------------------------------

#ifndef __USE_SHORT_LOGIC__

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAnd2);
  DefIntOpr(1, BDatAnd2, "&", 2, 2,
  "x1 & x2 {Real x1, Real x2}",
  I2("Returns true if both arguments are true.",
     "Devuelve verdadero si ambos argumentos lo son."),
  BOperClassify::Logic_);
  void BDatAnd2::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = And(Dat(Arg(1)),Dat(Arg(2))); 
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOr2);
  DefIntOpr(1, BDatOr2, "|", 2, 2,
  "x1 | x2 {Real x1, Real x2}",
  I2("Returns true if at least one argument is true.",
     "Devuelve verdadero si al menos uno de los argumentos lo es."),
  BOperClassify::Logic_);
  void BDatOr2::CalcContens()
//--------------------------------------------------------------------
{
  contens_ = Or(Dat(Arg(1)),Dat(Arg(2))); 
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatAnd);
  DefIntOpr(1, BDatAnd, "And", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if all its arguments are true.",
     "Devuelve verdadero si todos sus argumentos son verdaderos."),
  BOperClassify::Logic_);
  void BDatAnd::CalcContens()
//--------------------------------------------------------------------
{
  BUserDat* uDat = NULL;
  BInt	    n;
  contens_ = true;
  for(n=1; n<=NumArgs(); n++)
  {
    BDat& dat = Dat(Arg(n));
    if(dat.IsUnknown()) { contens_ = BDat::Unknown(); }
    else if(!dat.Value()) { contens_ = false; return; }
  }
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatOr);
  DefIntOpr(1, BDatOr, "Or", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if at least one of its arguments is true.",
     "Devuelve verdadero si al menos uno de sus argumentos es verdadero."),
  BOperClassify::Logic_);
  void BDatOr::CalcContens()
//--------------------------------------------------------------------
{
  BUserDat* uDat = NULL;
  BInt	    n;
  contens_ = false;
  for(n=1; n<=NumArgs(); n++)
  {
    BDat& dat = Dat(Arg(n));
    if(dat.IsUnknown()) { contens_ = BDat::Unknown(); }
    else if(dat.Value()) { contens_ = true; return; }
  }
}

//--------------------------------------------------------------------
BDat BDatTemporary::LogicShort(BDat(*comp)(const BDat&,const BDat&))const

/*! Returns the first argument Arg(1)
 */
//--------------------------------------------------------------------
{
  BInt	    n	   = 2;
  BDat	    ok	   = 1;
  BDat	    oldDat = Dat(Arg(1));
  BDat	    dat;
  int m = NumArgs();
  for(n=2; n<=m; n++)
  {
    dat    = Dat(Arg(n));
    ok     = And(ok,(*comp)(oldDat, dat));
    if(ok.IsKnown() && !ok.Value()) { break; }
    oldDat = dat;
  }
  return(ok);
}

//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLt);
  DefIntOpr(1, BDatLt, "LT", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is less than the following.",
     "Devuelve verdadero si cada argumento es menor que el siguiente."),
  BOperClassify::Logic_);
  void BDatLt::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Lt); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGt);
  DefIntOpr(1, BDatGt, "GT", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is greater than its following.",
     "Devuelve verdadero si cada argumento es mayor que el siguiente."),
  BOperClassify::Logic_);
  void BDatGt::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Gt); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatGe);
  DefIntOpr(1, BDatGe, "GE", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is equal or greater than the following.",
     "Devuelve verdadero si cada argumento es igual o mayor que el "
     "siguiente."),
  BOperClassify::Logic_);
  void BDatGe::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Ge); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatLe);
  DefIntOpr(1, BDatLe, "LE", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is equal or less than the following.",
     "Devuelve verdadero si cada argumento es igual o menor que el "
     "siguiente."),
  BOperClassify::Logic_);
  void BDatLe::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Le); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatEq);
  DefIntOpr(2, BDatEq, "Eq", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is just as the following.",
     "Devuelve verdadero si cada argumento es igual que el siguiente."),
  BOperClassify::Logic_);
  void BDatEq::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Eq); }


//--------------------------------------------------------------------
  DeclareContensClass(BDat, BDatTemporary, BDatNe);
  DefIntOpr(1, BDatNe, "NE", 1, 0,
  "(Real x1, [ Real x2 , ...])",
  I2("Returns true if each argument is different from following.",
     "Devuelve verdadero si cada argumento es diferente al siguiente."),
  BOperClassify::Statistic_);
  void BDatNe::CalcContens()
//--------------------------------------------------------------------
{ contens_ = LogicShort(Ne); }


//--------------------------------------------------------------------
// When short-circuited logic is enabled new methods are used
// These ones are temporarly disabled due to a hard to reproduce bug
//--------------------------------------------------------------------

#else //#ifdef __USE_SHORT_LOGIC__

//--------------------------------------------------------------------
class BDatLogicBase: public BInternalOperator
{
 public:
  BDatLogicBase(const BText& nam, 
                BInt min, BInt max,
	              const BText& args,
	              const BText& desc) 
  : BInternalOperator(nam, GraReal(), NULL, 
                      min, max, args, desc,
                      BOperClassify::Logic_),
  isOk_(true)
  { 
    IncNRefs(); 
  }
 ~BDatLogicBase() {}
  BSyntaxObject* Evaluate(const List* tre);
  BSyntaxObject* Evaluator(BList* arg) const;
 protected:
  bool  isOk_;
  BSyntaxObject* EnsureBranch(const List* b, int n);
  BText Desc() const
  {
    return(Grammar()->Name()+" "+Name()+" "+Arguments());
  }
  virtual bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const = 0;
  RedeclareClassNewDelete(BDatLogicBase);
};

List* Branch(const List* tre, BInt n);

//--------------------------------------------------------------------
BSyntaxObject* BDatLogicBase::EnsureBranch(const List* b, int n)
//--------------------------------------------------------------------
{
  BSyntaxObject* obj = NULL;
  int maxArg = MaxArg();
  if(b)
  {
  //Std(BText("BDatOrBase::EnsureBranch(")+n+","+BParser::Unparse(b, "  ", "\n")+")");
    if(maxArg && (maxArg<n))
    {
      isOk_ = false;
      Error(I2("Too many arguments in call to function ",
            	"Demasiados argumentos para llamar a la función ") + Desc());
    }
    else
    {
      obj = GraReal()->EvaluateTree(b);
      if(!obj || (obj->Grammar()!=GraReal()))
      {
        isOk_ = false;
        Error(I2("Argument number ","Argumento número ") + 
          n + "("+BParser::Unparse(b, "  ", "\n")+")"+
              I2(" of function "," de la función ") + Desc() +
              I2(" is not a Real object.", " no es un objeto de tipo Real."));
      }
    }
  }
  else if (n<=maxArg)
  {
    isOk_ = false;
    Error(I2("Insuficient arguments in call to function ",
             "Número de argumentos insuficientes para llamar a ") + Desc());

  }
  return(obj);
}

//--------------------------------------------------------------------
BSyntaxObject* BDatLogicBase::Evaluate(const List* tre)
//--------------------------------------------------------------------
{
  int n = 0;
  BDat v, a, b;
  bool end = false;
  const List* branch = tre;
  BList* args = NULL;
  while(!end)
  {
    end = ShortLogic(v,a,b,n);
    if(!end)
    {
      n++;
      List* br = Tree::treNode((List*)branch);
      BSyntaxObject* obj = EnsureBranch(br, n);
      if(isOk_ && obj) { a = b; b = Dat(obj); }
      else             { end = true; }
      if(obj)          { args = Cons(obj,args); }
      if(branch)       { branch = branch->cdr(); }
    }
  };
  if(v.IsKnown() && v.Value()) { v = 1; }
  DESTROY(args);
  return(new BContensDat("",v,""));
}

//--------------------------------------------------------------------
BSyntaxObject* BDatLogicBase::Evaluator(BList* arg) const
//--------------------------------------------------------------------
{
  int n = 0;
  BDat v, a, b;
  bool end = false;
  BList* args = arg;
  while(!end)
  {
    end = ShortLogic(v,a,b,n);
    if(!end)
    {
      n++;
      BSyntaxObject* obj = (!args)?NULL:(BSyntaxObject*)Car(args);
      if(isOk_ && obj) { a = b; b = Dat(obj); }
      else             { end = true; }
      if(args)         { args = Cdr(args); }
    }
  };
  if(v.IsKnown() && v.Value()) { v = 1; }
  DESTROY(arg);
  return(new BContensDat("",v,""));
}

#define DeclareLogicOperator_2(NAME) \
  static BDat##NAME##Base  * _##NAME##_2_  = (BDat##NAME##Base *)(__delay_init((void**)(&_##NAME##_2_ ), &BDat##NAME##Base ::clone_2)); 
#define DeclareLogicOperator_N(NAME) \
  static BDat##NAME##Base  * _##NAME##_N_  = (BDat##NAME##Base *)(__delay_init((void**)(&_##NAME##_N_ ), &BDat##NAME##Base ::clone_N));

#define DeclareLogicOperator(NAME) \
  DeclareLogicOperator_2(NAME); \
  DeclareLogicOperator_N(NAME);

//--------------------------------------------------------------------
  class BDatOrBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatOrBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if at least one of its arguments is true.",
       "Devuelve verdadero si al menos uno de sus argumentos es verdadero."))
  { 
  }
 ~BDatOrBase() {}
  static void * clone_2()
  {
    return new BDatOrBase("|", 2, 2, "x1 | x2 {Real x1, Real x2}");
  }
  static void * clone_N()
  {
    return new BDatOrBase("Or", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 protected:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(!iter) 
    { 
      v=0; 
      return(false); 
    }
    else
    {
      v.Or(b);
      return(v.IsKnown() && v.Value());
    }
  }
};
DeclareLogicOperator(Or);

//--------------------------------------------------------------------
  class BDatAndBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatAndBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns false if at least one argument is false.",
       "Devuelve falso si al menos uno de los argumentos lo es."))
  { 
  }
 ~BDatAndBase() {}
  static void * clone_2()
  {
    return new BDatAndBase("&", 2, 2, "x1 | x2 {Real x1, Real x2}");
  }
  static void * clone_N()
  {
    return new BDatAndBase("And", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(!iter) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator(And);

//--------------------------------------------------------------------
  class BDatEqBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatEqBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is just equal than the following.",
       "Devuelve verdadero si cada argumento es exáctamente igual que el siguiente."))
  { 
  }
 ~BDatEqBase() {}
  static void * clone_N()
  {
    return new BDatEqBase("Eq", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a==b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(Eq);

//--------------------------------------------------------------------
  class BDatNEBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatNEBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is different from following.",
       "Devuelve verdadero si cada argumento es diferente al siguiente."))
  { 
  }
 ~BDatNEBase() {}
  static void * clone_N()
  {
    return new BDatNEBase("NE", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a!=b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(NE);

//--------------------------------------------------------------------
  class BDatLEBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatLEBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is less or equal than following.",
       "Devuelve verdadero si cada argumento es menor o igual al siguiente."))
  { 
  }
 ~BDatLEBase() {}
  static void * clone_N()
  {
    return new BDatLEBase("LE", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a<=b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(LE);

//--------------------------------------------------------------------
  class BDatGEBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatGEBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is great or equal than following.",
       "Devuelve verdadero si cada argumento es mayor o igual al siguiente."))
  { 
  }
 ~BDatGEBase() {}
  static void * clone_N()
  {
    return new BDatGEBase("GE", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a>=b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(GE);

//--------------------------------------------------------------------
  class BDatLTBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatLTBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is strictly less than following.",
       "Devuelve verdadero si cada argumento es estrictamente menor que el siguiente."))
  { 
  }
 ~BDatLTBase() {}
  static void * clone_N()
  {
    return new BDatLTBase("LT", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a<b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(LT);

//--------------------------------------------------------------------
  class BDatGTBase: public BDatLogicBase
//--------------------------------------------------------------------
{
 public:
  BDatGTBase(const BText& nam, 
             BInt min, BInt max,
	           const BText& args) 
  : BDatLogicBase(nam, min, max, args, 
    I2("Returns true if each argument is strictly great than following.",
       "Devuelve verdadero si cada argumento es estrictamente mayor que el siguiente."))
  { 
  }
 ~BDatGTBase() {}
  static void * clone_N()
  {
    return new BDatGTBase("GT", 1, 0, "(Real x1, [ Real x2 , ...])");
  }
 private:
  bool ShortLogic(BDat& v, const BDat& a, const BDat& b, int iter) const
  {
    if(iter<2) 
    { 
      v=1; 
      return(false); 
    }
    else
    {
      v.And(a>b);
      return(v.IsKnown() && !v.Value());
    }
  }
};
DeclareLogicOperator_N(GT);

#endif
