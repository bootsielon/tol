/* bfilter.h: Filter for Parser Proccess.
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

/*! \brief Declarations of BFilter Class attributes and methods.
 *         Functionality has been implemented in bbasic/bparser/fil.cpp<br>
 *         BFilter represent the beginning of filter/scanner/parser proccess
 *         inherited to all computer languages.
 */


#ifndef TOL_BFILTER_H 
#define TOL_BFILTER_H 1
#define BFILTER 50

#include <tol/tol_btext.h>

// #define TRACE_BGRAMM
#define NoneBlock      0
#define Comment1Block  1
#define Comment2Block  2
#define QuotesBlock    3
#define NormalBlock    5

//! \brief Used in filtering to substitute specifics characters strings
//--------------------------------------------------------------------
/*! There is only one object of this type in TOL. This objects is 
 *  replaceTable (defined in fil.cpp like a static object):
 *  an array of BText pairs of some specific strings. An 
 *  example of one this can illustrate us:<br>
 *  first  tab of first element is: <code>" ) \\n { "</code><br>
 *  second tab of first element is: <code>" ) #F# { "</code><br>
 *  Ocurrence of first tab string will be overwritted with second 
 *  tab string in filtering process. And so on with others rows of
 *  replaceTable object.
 */
typedef BText BReplacement[2];


//! \brief class used to store line number and code blocks
//--------------------------------------------------------------------
class BLineText
//--------------------------------------------------------------------
{
 public:
    BInt  line;
    BText text;
};


typedef BArray<BLineText> BTextBlock;



//--------------------------------------------------------------------
class BFilter

/*! Filter of strings to get more easy text for scanning and parsing.
 */
//--------------------------------------------------------------------
{
private:
    mutable bool error_;
    mutable BText errorMsg_;

  void AddNewChar(BText&, BInt&, BChar) const;
  int AnyReplace(int, BText&, int, BReplacement*) const;
  void AnyReplace(BText&, int) const;
  virtual int Inseparable(BChar ch1, BChar ch2) const;

public:
  BFilter() : error_(false), errorMsg_("") { };
  virtual ~BFilter() {};

  bool hasError() const;
  BText getErrorMsg() const;

  // Access & Manipulation:
  virtual BBool IsFirstIdentifier(BChar ch) const;
  virtual BBool IsIdentifier(BChar ch) const;
  virtual BBool IsIdentifier(const BText& expression) const;
  virtual BBool IsSymbol(BChar ch) const;
  BText Transform(const BText& txt) const;

  int GetBlocks(BText& original, BTextBlock& block) const;
  int ClassifyBlock(char* pos) const;
  void CloseBlock(char** begin,char** pos, int& num, int& lineNum,
		  BTextBlock& block) const;

  void Clean(BText& expression) const;
  void CleanQuotes(BText& expression) const;
  void Replace(BText& txt) const;
  void UnReplace(BText& txt) const;
};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BText TOL_API ToName(BText txt);


#endif // TOL_BFILTER_H
