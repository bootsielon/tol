/* tol_btoken.h: Token representation in parser proccess of
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

#ifndef TOL_BTOKEN_H
#define TOL_BTOKEN_H 1

#include <tol/tol_bobject.h>

#define MAXPRECEDENCE 1000


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BCloseToken;


//--------------------------------------------------------------------
// types
//--------------------------------------------------------------------
/*! Every token red from scanner has a type. This enum, BTokenType, is a
 *  list of each type of token that scanner could identify in the input.
 */
enum BTokenType
{
  NONE	    = -1,
  OPEN	    =  0,      //!< for open braquet symbols
  CLOSE	    =  1,      //!< for close braquet symbols
  SEPARATOR =  2,      //!< for argTokens separator symbols
  MONARY    =  3,      //!< for monary operator symbols
  BINARY    =  4,      //!< for binary operator symbols
  ARGUMENT  =  5,      //!< for variable and constant symbols
  FUNCTION  =  6,      //!< for function symbols
  TYPE	    =  7,      //!< for function symbols
  MACRO	    =  8       //!< for parsing macroes
};


/*! \brief Declarations of BToken Class attributes and methods.<br>
 *  Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BToken : public BObject

/*! Each word red from scanner represent a BToken. Btoken is 
 *  a general class that represents each type of token. In
 *  Parser proccess each token will be stored in Syntactical Tree.
 *  <br>
 *  This class creates Data Structure Type BToken. Define simply 
 *  methods to "get" or "set" values from/to attributes.
 */
//--------------------------------------------------------------------
{
private:
  BTokenType    tokenType_;  //!< Specify type of this token \sa BTokenType
  BInt          precedence_; //!< Precedence of this Token
  BCloseToken*  close_;
  BText	        unparsed_;

  const void*   object_;     //!< Allows to link a semantical object to a sintax token
  unsigned char objectClass_;  
  unsigned char constantType_;  
  int           level_;
  bool          isSystem_; 
public:
  // Constructors and destructors:
  //! Default constructor
  BToken(const BText&     name,
	       BTokenType typ = NONE,
	       BInt       pre = MAXPRECEDENCE);
  //! Copy constructor
  BToken(const BToken* tok);
  //! Empty destructor method
 ~BToken();
  // Access & Manipulation: inline
  //! returns isSystem_
  bool IsSystem() { return(isSystem_); }
  //! returns level_
  int Level() { return(level_); }
  //! returns constant type
  unsigned char ConstantType() { return(constantType_); }
  //! returns object class
  unsigned char ObjectClass() { return(objectClass_); }
  //! returns object value
  const void* Object() { return(object_); }
  //! Return \a unparsed_ attribute
  const BText& Unparsed()      const { return(unparsed_); }
  //! Return value of constante MAXPRECEDENCE
  static BInt  MaxPrecedence()	     { return(MAXPRECEDENCE); }
  //! Return int value indicating type of this token
  BTokenType   TokenType()     const { return(tokenType_); }
  //! Return int value indicating precedence of this token
  BInt	       Precedence()    const { return(precedence_); }
  //! Return \a close_ attribute (if exist) asociatted with this BToken
  BCloseToken* Close()	       const { return (close_); }

  //! Set object system flag
  void PutIsSystem(bool isSystem) { isSystem_ = isSystem; }
  //! Set object level
  void PutLevel(int level) { level_ = level; }
  //! Set object value
  void PutObject(const void* obj) { object_ = obj; }
  //! Set object class value
  void PutObjectClass(unsigned char objCl) { objectClass_ = objCl; }
  //! Set \a unparsed_ (BText) attribute value
  void PutUnparsed(const BText& txt) { unparsed_ = txt; }
  //! Set \a close_ (BCloseToken) attribute value
  void PutClose(BCloseToken* close)  { close_ = close; }
  //! Set \a tokenType_ (enumerated int) attribute value
  void PutTokenType(BTokenType typ)  { tokenType_ = typ; }
  //! Set \a precedence_ (int) attribute value
  void PutPrecedence(BInt pre)	     { precedence_ = pre; }
  //! Set \a precedence_ attribute to MAXPRECEDENCE value
  void PutMaxPrecedence()	     { precedence_ = MAXPRECEDENCE; }

  //! Return new BToken from object that receive method invokation.
  /*! Return new BToken object from this.
   *  It's like copy constructor.
   */
  virtual BToken* Duplicate() { return(new BToken(this)); }

  DeclareClassNewDelete(BToken);
};


/*! \brief Declarations of BOpenToken Class attributes and methods.<br>
 *  Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BOpenToken : public BToken

/*! Open bracket symbols representation.<br>
 *  \a defOpe_ is a static variable with all BOpenToken of TOL.
 *  \sa defOpe_
 */
//--------------------------------------------------------------------
{
public:
  BOpenToken(const BText& name); //!< Default Constructor
  BOpenToken(BOpenToken* tok);   //!< Copy Constructor
  ~BOpenToken();                 //!< Destructor. Empty Method

  // Access & Manipulation: inline
  //! Return new BToken from object that receive method invokation.
  /*! PURPOSE: Return new BToken object from this.
  //  It's like copy constructor.
  */
  BToken* Duplicate() { return(new BOpenToken(this)); }
  DeclareClassNewDelete(BOpenToken);
};


/*! \brief Declarations of BCloseToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BCloseToken : public BToken

/*! PURPOSE: Close bracket symbols representation.<br>
 *  \a defClo_ is a static variable with all BCloseToken of TOL.
 *  \sa defClo_
 */
//--------------------------------------------------------------------
{
private:
  BOpenToken* open_;  //!< open symbol associated

public:
  BCloseToken(const BText& name, BOpenToken* open); //!< Default Constructor
  BCloseToken(BCloseToken* tok);                //!< Copy Constructor
  ~BCloseToken();                               //!< Destructor. Empty Method

  // Access & Manipulation: inline
  //! Set \a open_ attribute with \a open (BOpenTable) parameter
  void PutOpen(BOpenToken* open) { open_ = open; }
  //! Return pointer to \a open_ (BOpenToken) attribute
  BOpenToken* Open() const { return (open_); }
  //! Method with the same functionality like Copy Constructor
  BToken* Duplicate() { return(new BCloseToken(this)); }

  RedeclareClassNewDelete(BCloseToken);
};


/*! \brief Declarations of BSeparatorToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BSeparatorToken : public BToken

/*! Separator symbols representation.<br>
 *  \a defSep_ is a static variable with all BSeparatorToken of TOL.
 *  \sa defSep_
 */
//--------------------------------------------------------------------
{
public:
    BSeparatorToken(const BText& name, BInt pre); //!< Default Constructor
    BSeparatorToken(BSeparatorToken* tok); //!< Copy Constructor
    ~BSeparatorToken(); //!< Destructor. Empty Method

  // Access & Manipulation: inline
  //! Method with the same functionality like Copy Constructor
  BToken* Duplicate() { return(new BSeparatorToken(this)); }

  RedeclareClassNewDelete(BSeparatorToken);
};


/*! \brief Declarations of BTypeToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BTypeToken : public BToken

/*! Type symbols representation.<br>
 *  \a defTyp_ is a static variable with all BTypeToken of TOL.
 *  \sa defTyp_
 */
//--------------------------------------------------------------------
{
public:
  enum BType 
  {
    BSYSTEM,
    BSTRUCT,
    BCLASS,
    BTOIS
  };
  BType type_;

  BTypeToken(const BText& name, BType); //!< Default Constructor Method
  BTypeToken(const BTypeToken* tok); //!< Copy Constructor Method
 ~BTypeToken(); //!< Destructor. Empty Method

    //! Method with the same functionality like Copy Constructor
    BToken* Duplicate() { return(new BTypeToken(this)); }

  RedeclareClassNewDelete(BTypeToken);
};

/*! \brief Declarations of BTypeToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */

/*! \brief Declarations of BBinaryToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BBinaryToken : public BToken

/*! Binary operator symbols representation.<br>
 *  \a defBin_ is a static variable with all BBinaryToken of TOL.
 *  \sa defBin_
 */
//--------------------------------------------------------------------
{
public:
    BBinaryToken(const BText& name, BInt pre); //!< Default Constructor
    BBinaryToken(const BBinaryToken* tok); //!< Copy Constructor
    ~BBinaryToken(); //!< Destructor. Empty Method

    //! Method with the same functionality like Copy Constructor
    BToken* Duplicate() { return(new BBinaryToken(this)); }

  RedeclareClassNewDelete(BBinaryToken);
};


/*!\brief Declarations of BMonaryToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BMonaryToken : public BToken

/*! Monary operator symbols representation.<br>
 *  \a defMon_ is a static variable with all BMonaryToken of TOL.
 *  \sa defMon_
 */
//--------------------------------------------------------------------
{
private:
    BBinaryToken* binary_;

public:
    BMonaryToken(const BText& name); //! Default Constructor
    //! Default Constructor with \a binary attribute
    BMonaryToken(const BText& name, BToken* binary);
    BMonaryToken(const BMonaryToken* tok); //!< Copy Constructor
    ~BMonaryToken(); //!< Destructor. Empty Method

    // Access & Manipulation: inline
    //! Return \a binary_ (BBinaryToken) attribute
    BBinaryToken* Binary() const { return (binary_); }
    //! Set \a binary_ (BBinaryToken) attribute 
    void PutBinary(BBinaryToken* binary) { binary_ = binary; }

    //! Method with the same functionality like Copy Constructor
    BToken* Duplicate() { return(new BMonaryToken(this)); }

  RedeclareClassNewDelete(BMonaryToken);
};


/*! \brief Declarations of BMacroToken Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/tok.cpp
 */
//--------------------------------------------------------------------
class BMacroToken : public BToken

/*! Parsing macro symbols representation.<br>
 */
//--------------------------------------------------------------------
{
public:
    BMacroToken(const BText& name); //!< Default Constructor
    BMacroToken(const BMacroToken* tok); //!< Copy Constructor
   ~BMacroToken(); //!< Destructor. Empty Method

    //! Method with the same functionality like Copy Constructor
    BToken* Duplicate() { return(new BMacroToken(this)); }

  RedeclareClassNewDelete(BMacroToken);
};


#endif // BTOKEN

