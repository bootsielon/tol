/* tol_bmethod.h: Class and functions header.
                  GNU/TOL Language.

   Copyright (C) 2003 - Bayes Decision, SL (Spain [EU]) (Spain [EU ])

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

#ifndef TOL_BMETHOD_H
#define TOL_BMETHOD_H 1

#include <tol/tol_bsyntax.h>

class BList;

//--------------------------------------------------------------------
class BMethod: public BSyntaxObject

/*! Abstract class to derive method functions.
 */
//--------------------------------------------------------------------
{
private:
  BGrammar* grammar_;
  BList*    function_;

public:
  // Constructors and destructors:
  BMethod(const BText&, BGrammar*);
  BMethod(const BText&, const BText&);
 ~BMethod();

  // Access & Manipulation: inline
  virtual BInt Mode() const;
  BGrammar* Grammar() const;
  void	    BuildFunction  ();
  void	    DestroyFunction();
  // Implementation
  virtual void	Apply	  (BList* lst, const BText& nameFile) const = 0;
  virtual BBool Validate  (BList* ) const;

  void	Apply	  (BList* lst) const;

};

class BMethodView : public BMethod
{
public:
  BMethodView(BGrammar* grammar);
  void Apply(BList* lst, const BText& nameFile) const;
};

class BMethodShow : public BMethod
{
public:
  BMethodShow(BGrammar* grammar);
  void Apply(BList* lst, const BText& nameFile) const;
};


//--------------------------------------------------------------------
// inline functions
//--------------------------------------------------------------------

#if defined(USE_DELAY_INIT)
#  define CALL_INITGRAMMARS
#else
#  define CALL_INITGRAMMARS InitGrammars();
#endif

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define DeclareMethodClass(BMethodClass, name, grammar)		      \
class BMethodClass : public BMethod				      \
{								      \
private:							      \
  static BMethodClass* object_;					      \
public:								      \
  BMethodClass() : BMethod(name, grammar) { CALL_INITGRAMMARS }       \
 ~BMethodClass() {}						      \
  void Apply(BList*, const BText&) const;			      \
  static const BMethodClass& Object() {return (*object_); }	      \
};
								      
#if !defined(USE_DELAY_INIT)

#define DefineMethodObject(BMethodClass)			      \
BMethodClass* BMethodClass::object_ = new BMethodClass ;

#else

#define DefineMethodObject(BMethodClass)			      \
static void * clone##BMethodClass() \
{ \
  return new BMethodClass ; \
} \
BMethodClass* BMethodClass::object_ = (BMethodClass*)(__delay_init((void**)(&BMethodClass::object_), \
								   &clone##BMethodClass));
#endif

#define DefineMethodClass(BMethodClass, name, grammar)		      \
DeclareMethodClass(BMethodClass, name, grammar)			      \
DefineMethodObject(BMethodClass)


#define WriteFileOrStd(fileName,out,txt)  \
{					  \
  if(fileName!="Std")			  \
  {					  \
    out << txt;				  \
  }					  \
  else					  \
  {					  \
    Std(txt);				  \
  }					  \
}


#endif // TOL_BMETHOD_H
