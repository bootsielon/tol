/* tok.cpp: implementation of token representation in parser proccess of
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

#include <tol/tol_btoken.h>

BTraceInit("tok.cpp");

//--------------------------------------------------------------------
BToken::BToken(const BText& name, BTokenType typ, BInt pre)

/*! Construct a BToken general object. \a unpersed_ remain empty.
 * \param name is used to initialize \a name attribute inherited from 
 *        BObject
 * \param typ specify the type of token
 * \param pre specify the precedence related to this type of token
 */
//--------------------------------------------------------------------
: BObject(name), 
  tokenType_(typ), 
  close_(NIL), 
  precedence_(pre),
  unparsed_(""), 
  object_(NULL), 
  objectClass_('\0'), 
  constantType_('\0'),
  level_(-1),
  isSystem_(false)   
{ 
  if(name[0]=='\"')
  {
    constantType_ = 'T';
  }
  else if(strchr(".0123456789",name[0]))
  {
    constantType_ = 'R';
  } 
  else if((name.Length()>=5) &&
          (name(0)=='y')	   &&
           isdigit(name(1))	 &&
           isdigit(name(2))	 &&
           isdigit(name(3))	 &&
           isdigit(name(4))	   )
  {
    constantType_ = 'D';
  }
}

//--------------------------------------------------------------------
BToken::BToken(const BToken* tok)

/*! Make new BToken object from \a tok parameter. Invokes first 
 *  constructor method with arguments extracted from \a tok.
 * \param tok BToken to be replicated.
 */
//--------------------------------------------------------------------
: BObject(tok->Name()),
  tokenType_(tok->TokenType()),
  close_(tok->Close()),
  precedence_(tok->Precedence()),
  unparsed_(tok->Unparsed()), 
  object_(tok->object_),
  objectClass_(tok->objectClass_),
  constantType_(tok->constantType_),
  level_(tok->level_),
  isSystem_(tok->isSystem_)
{ }


//--------------------------------------------------------------------
BToken::~BToken()

/*! Destructor
 */
//--------------------------------------------------------------------
{
  if(object_ && constantType_) 
  {
    BAtom* obj = (BAtom*)object_;
    obj->DecNRefs();
    if(!obj->NRefs())
    {
      DESTROY(obj);
    }
  }
}


//--------------------------------------------------------------------
BOpenToken::BOpenToken(const BText& name)

/*! Construct BOpenToken with \a name attribute (inherited from
 *  BObject)
 */
//--------------------------------------------------------------------
    :BToken (name, OPEN)
{ }


//--------------------------------------------------------------------
BOpenToken::BOpenToken(BOpenToken* tok)

/*! Copy Constructor that create a BOpenToken object from \a tok
 *  using Copy Constructor of BToken superclass.
 */
//--------------------------------------------------------------------
    :BToken ((BToken*)tok)
{ }


//--------------------------------------------------------------------
BOpenToken::~BOpenToken()

/*! Destructor. Empty destructor method.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BCloseToken::BCloseToken(const BText& name, BOpenToken* open)

/*! Constructor.
 * \param name Symbol of this BCloseToken
 * \param open BOpenToken related with this cose token.
 */
//--------------------------------------------------------------------
    : BToken (name, CLOSE), open_(open)
{
    open_->PutClose(this);
}


//--------------------------------------------------------------------
BCloseToken::BCloseToken(BCloseToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
    : BToken ((BToken*)tok), open_(tok->Open())
{ }


//--------------------------------------------------------------------
BCloseToken::~BCloseToken()

/*! Destructor empty method.
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BSeparatorToken::BSeparatorToken(const BText& name, BInt pre)

/*! Constructor.
 */
//--------------------------------------------------------------------
    : BToken(name, SEPARATOR, pre)
{ }


//--------------------------------------------------------------------
BSeparatorToken::BSeparatorToken(BSeparatorToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
    : BToken((BToken*)tok)
{ }


//--------------------------------------------------------------------
BSeparatorToken::~BSeparatorToken()

/*! Destructor empty method
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BTypeToken::BTypeToken(const BText& name, BType type)

/*! Constructor
 */
//--------------------------------------------------------------------
    : BToken (name,TYPE), type_(type)
{ }


//--------------------------------------------------------------------
BTypeToken::BTypeToken(const BTypeToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
    : BToken((BToken*)tok), type_(tok->type_)
{ }


//--------------------------------------------------------------------
BTypeToken::~BTypeToken()

/*! Destructor empty method
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BBinaryToken::BBinaryToken(const BText& name, BInt pre)

/*! Constructor
 */
//--------------------------------------------------------------------
    : BToken (name, BINARY, pre)
{ }


//--------------------------------------------------------------------
BBinaryToken::BBinaryToken(const BBinaryToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
    : BToken((BToken*)tok)
{ }


//--------------------------------------------------------------------
BBinaryToken::~BBinaryToken()

/*! Destructor empty method
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BMonaryToken::BMonaryToken(const BText& name)

/*! Constructor
 */
//--------------------------------------------------------------------
    : BToken (name, MONARY), binary_ (NIL)
{ }


//--------------------------------------------------------------------
BMonaryToken::BMonaryToken(const BText& name, BToken* binary)
    
/*! Constructor
 */
//--------------------------------------------------------------------
    : BToken (name, MONARY), binary_ ((BBinaryToken*)binary)
{ }


//--------------------------------------------------------------------
BMonaryToken::BMonaryToken(const BMonaryToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
    :BToken ((BToken*)tok), binary_(tok->Binary())
{ }


//--------------------------------------------------------------------
BMonaryToken::~BMonaryToken()

/*! Destructor empty method
 */
//--------------------------------------------------------------------
{ }


//--------------------------------------------------------------------
BMacroToken::BMacroToken(const BText& name)

/*! Constructor
 */
//--------------------------------------------------------------------
: BToken (name, MACRO)
{ }


//--------------------------------------------------------------------
BMacroToken::BMacroToken(const BMacroToken* tok)

/*! Copy Constructor
 */
//--------------------------------------------------------------------
: BToken((BToken*)tok)
{ }


//--------------------------------------------------------------------
BMacroToken::~BMacroToken()

/*! Destructor empty method
 */
//--------------------------------------------------------------------
{ }
