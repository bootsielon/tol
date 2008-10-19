/* tol_bscanner.h: lexical scanner for GNU/TOL Language.

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

/*! \brief Declarations of BScanner Class attributes and methods.<br>
 *         Functionality has been implemented in bbasic/bparser/scn.cpp
 */

#ifndef TOL_BSCANNER_H
#define TOL_BSCANNER_H 1

#include <tol/tol_barray.h>
#include <tol/tol_btoken.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
template class BArray<BToken*>;


//--------------------------------------------------------------------
class BScanner

/*! Implements Lexical Analisis.<br>
 *  Basically does two importante things:<ol>
 *   <li>Creates a Symbol Table with all tokens that could be
 *      recognized as part of language.
 *   <li>Implements code analizer based on method that make a search
 *        on classyfied Symbol Table.
 *  </ol>
 */
//--------------------------------------------------------------------
{
private:
    static BArray<BToken*> symbolTable_;  //!< Tokens Symbol Table
    const BChar*  expression_;      //!< Expression that will be scanned
    const BChar*  position_;        //!< Actual position of scanning
          BInt    length_;          //!< Length of \a expression_

    BToken*    nextSymbol_;         //!< Pointer to next read token
    //! Word read from scanner when a Token isn't in \a symbolTable_
    /*! Word read will be stored as BText in \a nextArgument_ if it isnt't
     *  in \a symbolTable_. For example, name of any function or variable.
     */
    BText      nextArgument_;

  //! Puts the tokens of all types in \a symbolTable_ and sorts it.
  static void PutSymbolTable();

public:
  // Constructors and destructors:
  BScanner();
 ~BScanner();

  // Access & Manipulation: inline

  //! Initialize Scanner attributes in order to begin scan proccess
  void         Initialize     (const BText& expression);
  //! Find a symbol in \a symbolTable_ which name is \a str and returns it
  BToken*      FindSymbol     (const BChar* str);
  //! Reads the next word in parsing code
  BTokenType   ReadNextSymbol ();

  //! Return value of \a nextSymbol_ attribute
  /*! PURPOSE: Return value of \a nextSymbol_ attribute */
  BToken*      NextSymbol     () { return (nextSymbol_); }

  //! Return value of \a nextArgument_ attribute
  /*! PURPOSE: Return value of \a nextArgument_ attribute */
  BText&       NextArgument   () { return (nextArgument_); }
  //! Returns the text scanned at this moment.
  BText        Scanned        ();
  //! Return size of \a symbolTable_ attribute.
  BInt         CountSymbols   ();
  //! Store in \a symbolTable_ BToken \a sym parameter.
  BInt         AddSymbol      (BToken* sym);
  //! Remove BToken named \a str from symbolTable_ BArray structure
  BInt         DelSymbol      (const BChar* str);

private:
  //! Moves position_ to the next no blank character.
  BBool NextChar();

  //! Reads the next word from \a position_.
  BTokenType NextWordType();
};


//--------------------------------------------------------------------
// external functions 
//--------------------------------------------------------------------
char         GetIdFromTokenClose(BCloseToken* c);
BCloseToken* GetTokenCloseFromId(char k);


#endif // BSCANNER
