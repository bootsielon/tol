/* scn.cpp: implementation of lexical scanner for GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bscanner.h>
#include <assert.h>

BTraceInit("scn.cpp");

/*! Structure where recognizables tokens of TOL language are stored.
 *  This object is created in PutSymbolTable method, called from
 *  constructor.
 *  \sa PutSymbolTable
 */
BScanner::BTokenByNameHash BScanner::symbolTable_;
BScanner::BTokenByNameHash BScanner::aliasTable_;
BScanner::BTokenByNameHash BScanner::usrDefSymbol_; //!< User defined symbols Class, Struct, ...


//--------------------------------------------------------------------
BScanner::BScanner()

/*! Create BScanner object.<br>
 *  Create and initialize \a symbolTable_ with 
 * Initialize \a position_, \a nextSymbol_, \a nextArgument_.
 */
//--------------------------------------------------------------------
{
//Std(BText("\nCreating BScanner ")+(BInt)this);
  PutSymbolTable();
  position_   = NIL;
  nextSymbol_ = NIL;
  lastSymbol_ = NIL;
  nextArgument_ = "";
  structDefininig_ = false;
}

//--------------------------------------------------------------------
BScanner::~BScanner()
//--------------------------------------------------------------------
{
//Std(BText("\nDestroying BScanner ")+(BInt)this);
}

//--------------------------------------------------------------------
BInt BTokenOrderCriterium(const BAny vBToken1, const BAny vBToken2)

/*! Comparision function to sort by name in arrays of BToken*
 */
//--------------------------------------------------------------------
{
/*
  BToken& sym1 = **((BToken**)vBToken1);
  BToken& sym2 = **((BToken**)vBToken2);
  BInt cmp = StrCmp(sym1.String(), sym2.String());
  return(cmp);
*/
  BToken* sym1 = *(BToken**)vBToken1;
  BToken* sym2 = *(BToken**)vBToken2;
  if     (!sym1 && !sym2) { return( 0); }
  else if(!sym1 &&  sym2) { return( 1); }
  else if( sym1 && !sym2) { return(-1); }
  else { return(StrCmp(sym1->String(), sym2->String())); }
}


//--------------------------------------------------------------------
BInt Str2SymCmp(const BAny vBChar, const BAny vBToken)

/*! Comparision function to find by name in sorted arrays of BToken*
 *  <br><i>Obsolete method. Not used by TOL code</i>
 */
//--------------------------------------------------------------------
{
  BChar*  str =    (BChar*)   vBChar;
  BToken& sym = **(BToken**) vBToken;
  return(strcmp(str, sym.String()));
}


//--------------------------------------------------------------------
BToken* BScanner::FindSymbol(const BText& name)

/*! Find a symbol in \a symbolTable_ which name is \a str and
 *  returns it if exists, else return NIL.
 * \param str Name of BToken symbol that will be searched
 * \return BToken found in \a symbolTable_
 * \sa BArray::FindSorted
 */
//--------------------------------------------------------------------
{
  BToken* tok = NULL;
  BTokenByNameHash::const_iterator found;
  found = symbolTable_.find(name);
  if(found==symbolTable_.end()) 
  { 
    found = aliasTable_.find(name);
    if(found!=aliasTable_.end()) 
    { 
      tok = found->second; 
    }
  }
  else 
  { 
    tok = found->second; 
  }           
  return(tok);
}

//--------------------------------------------------------------------
BToken* BScanner::FindSymbol(const BChar* str)

/*! Find a symbol in \a symbolTable_ which name is \a str and
 *  returns it if exists, else return NIL.
 * \param str Name of BToken symbol that will be searched
 * \return BToken found in \a symbolTable_
 * \sa BArray::FindSorted
 */
//--------------------------------------------------------------------
{
  BText name = str;
  return(FindSymbol(name));
}

//--------------------------------------------------------------------
BInt BScanner::CountSymbols()

/*! Return size of \a symbolTable_ attribute.
 */
//--------------------------------------------------------------------
{
  return(symbolTable_.size());
}

//--------------------------------------------------------------------
BInt BScanner::AddSymbol(BToken* sym)

/*! Store in \a symbolTable_ BToken \a sym parameter.
 * \param sym BToken that will be stores in \a symbolTable_
 * \return Size of symbolTable_
 * \sa BArray::AddSorted
 */
//--------------------------------------------------------------------
{
  const BText& name = sym->Name();
  if(!FindSymbol(name))
  {
//Std(BText("\nAdding Symbol <") + sym->Name()+">");
    symbolTable_[name] = sym;
    if(sym->TokenType()==TYPE)
    {
      usrDefSymbol_[name]=sym;
    }
  }
  return(symbolTable_.size());
}

//--------------------------------------------------------------------
BInt BScanner::DelSymbol(const BText& name)

/*! Remove BToken with \a name_ attribute equal to \a str parameter
 *  from symbolTable_.
 * \param str Char* value of \a name_ of BToken that will be removed
 * \return symbolTable_ size.
 * \sa BArray::FindSorted, BArray::ReallocBuffer
 */
//--------------------------------------------------------------------
{
  symbolTable_.erase(name);
  return(symbolTable_.size());
}

//--------------------------------------------------------------------
BInt BScanner::DelSymbol(const BChar* str)

/*! Remove BToken with \a name_ attribute equal to \a str parameter
 *  from symbolTable_.
 * \param str Char* value of \a name_ of BToken that will be removed
 * \return symbolTable_ size.
 * \sa BArray::FindSorted, BArray::ReallocBuffer
 */
//--------------------------------------------------------------------
{
  BText name = str;
  return(DelSymbol(name));
}

//--------------------------------------------------------------------
// Tables of tokens by type
//--------------------------------------------------------------------
static BOpenToken defOpe_[] =
{
  BOpenToken("("),
  BOpenToken("["),
  BOpenToken("{"),
  BOpenToken("#{#"),
  BOpenToken("#(#")
};

static BCloseToken defClo_[] =
{
  BCloseToken(")",   &(defOpe_[0])),
  BCloseToken("]",   &(defOpe_[1])),
  BCloseToken("}",   &(defOpe_[2])),
  BCloseToken("#}#", &(defOpe_[3])),
  BCloseToken("#)#", &(defOpe_[4]))
};

//--------------------------------------------------------------------
  char GetIdFromTokenClose(BCloseToken* c)
//--------------------------------------------------------------------
{
  BCloseToken* t = &(defClo_[0]);
  if(!c) { return('\0'); }
  assert((c>=t)&&(c<t+5));
  return(1+(c-t));
}


//--------------------------------------------------------------------
  BCloseToken* GetTokenCloseFromId(char k)
//--------------------------------------------------------------------
{
  assert((k>=0)&&(k<=5));
  if(!k) { return(NULL); }
  return(&defClo_[k-1]);
}

static BSeparatorToken defSep_[] =
{
  BSeparatorToken(";", 1),
  BSeparatorToken(",", 2)
};

static BToken defBin_[] =
{
  BBinaryToken("=",        3),   // 0
  BBinaryToken(":=",       3),   // 1
  BBinaryToken("#F#",      4),   // 2
  BBinaryToken("|",        5),   // 3
  BBinaryToken("&",        6),   // 4
  BBinaryToken("Of",       7),   // 5
  BBinaryToken("@",        7),   // 6
  BBinaryToken(">",        8),   // 7
  BBinaryToken("<",        8),   // 8
  BBinaryToken(">",        8),   // 9
  BBinaryToken("==",       8),   // 10
  BBinaryToken(">=",       8),   // 11
  BBinaryToken("<=",       8),   // 12
  BBinaryToken(":>",       8),   // 13
  BBinaryToken("<:",       8),   // 14
  BBinaryToken("!=",       8),   // 15
  BBinaryToken("#+#",      9),   // 16
  BBinaryToken("#-#",      9),   // 17
  BBinaryToken("*",       10),   // 18
  BBinaryToken("$*",      10),   // 19
  BBinaryToken("/",       11),   // 20
  BBinaryToken("$/",      11),   // 21
  BBinaryToken("%",       11),   // 22
  BBinaryToken("^",       12),   // 23
  BBinaryToken("**",      12),   // 24
  BBinaryToken("&&",      13),   // 25
  BBinaryToken("||",      13),   // 26
  BBinaryToken(":",       14),   // 27
  BBinaryToken("<<",      15),   // 28
  BBinaryToken(">>",      15),   // 29
  BBinaryToken("->",      16),   // 30
  BBinaryToken("#E#",     17),   // 30
  BBinaryToken("::",      18)    // 32
};

static BMonaryToken defMon_[] =
{
  BMonaryToken("+", &(defBin_[16])),
  BMonaryToken("-", &(defBin_[17])),
  BMonaryToken("~"),
  BMonaryToken("!"),
  BMonaryToken("Do"),
  BMonaryToken("Struct"),
  BMonaryToken("Class")
};

static BTypeToken defTyp_[] =
{
  BTypeToken("Anything",  BTypeToken::BSYSTEM),
  BTypeToken("Set",       BTypeToken::BSYSTEM),
  BTypeToken("Date",      BTypeToken::BSYSTEM),
  BTypeToken("CTime",     BTypeToken::BSYSTEM),
  BTypeToken("Real",      BTypeToken::BSYSTEM),
  BTypeToken("Complex",   BTypeToken::BSYSTEM),
  BTypeToken("Matrix",    BTypeToken::BSYSTEM),
  BTypeToken("VMatrix",   BTypeToken::BSYSTEM),
  BTypeToken("Polyn",     BTypeToken::BSYSTEM),
  BTypeToken("Ratio",     BTypeToken::BSYSTEM),
  BTypeToken("Text",      BTypeToken::BSYSTEM),
  BTypeToken("TimeSet",   BTypeToken::BSYSTEM),
  BTypeToken("CTimeSet",  BTypeToken::BSYSTEM),
  BTypeToken("Serie",     BTypeToken::BSYSTEM),
  BTypeToken("CSeries",   BTypeToken::BSYSTEM),
  BTypeToken("Code",      BTypeToken::BSYSTEM),
  BTypeToken("NameBlock", BTypeToken::BSYSTEM)
};

static BMacroToken defMac_[] =
{
  BMacroToken("#Embed")
};

static const char* alias_[][2] =
{
  { "Ration", "Ratio" }
};

static const char* usrDef_[] =
{
  "Struct", "Class" 
};

static BInt defOpeNum_ = sizeof(defOpe_)/sizeof(BOpenToken);
static BInt defCloNum_ = sizeof(defClo_)/sizeof(BCloseToken);
static BInt defSepNum_ = sizeof(defSep_)/sizeof(BSeparatorToken);
static BInt defBinNum_ = sizeof(defBin_)/sizeof(BBinaryToken);
static BInt defMonNum_ = sizeof(defMon_)/sizeof(BMonaryToken);
static BInt defTypNum_ = sizeof(defTyp_)/sizeof(BTypeToken);
static BInt defMacNum_ = sizeof(defMac_)/sizeof(BMacroToken);
static BInt aliasNum_  = sizeof(alias_ )/sizeof(alias_ [1]);
static BInt usrDefNum_ = sizeof(usrDef_)/sizeof(usrDef_[1]);

static BInt defTokNum_ =
            defOpeNum_ +
            defCloNum_ +
            defSepNum_ +
            defBinNum_ +
            defMonNum_ +
            defTypNum_ +
            defMacNum_ ;


//--------------------------------------------------------------------
void BScanner::PutSymbolTable()

/*! Puts all recognizables tokens of TOL in \a symbolTable_ static 
 *  attribute and sorts it.<br>
 *  Different types of tokens, stored in different static tables:<br>
 *  <ul>
 *     <li>\a defOpe_ array for tokens that represents open symbols
 *     <li>\a defClo_ array for tokens that represents close symbols
 *     <li>\a defSep_ array for tokens that represents separator symbols
 *     <li>\a defBin_ array for tokens that represents binary symbols
 *     <li>\a defMon_ array for tokens that represents monary symbols
 *     <li>\a defTyp_ array for tokens that represents types
 * </ul>
 */
//--------------------------------------------------------------------
{
  static bool symbolTableFilled_ = BFALSE;
  if (!symbolTableFilled_)
  {
    SetEmptyKey(symbolTable_, NULL);
    SetDeletedKey(symbolTable_, name_del_key());

    symbolTableFilled_ = BTRUE;
    BInt n=0, m=0;
    for(n=0; n<defOpeNum_; n++)
    { symbolTable_[defOpe_[n].Name()]=&(defOpe_[n]); }
    m+=n;
    for(n=0;n<defCloNum_; n++)
    { symbolTable_[defClo_[n].Name()]=&(defClo_[n]); }
    m+=n;
    for(n=0; n<defSepNum_; n++)
    { symbolTable_[defSep_[n].Name()]=&(defSep_[n]); }
    m+=n;
    for(n=0; n<defBinNum_; n++)
    { symbolTable_[defBin_[n].Name()]=&(defBin_[n]); }
    m+=n;
    for(n=0; n<defMonNum_; n++)
    { symbolTable_[defMon_[n].Name()]=&(defMon_[n]); }
    m+=n;
    for(n=0; n<defTypNum_; n++)
    { symbolTable_[defTyp_[n].Name()]=&(defTyp_[n]); }
    m+=n;
    for(n=0; n<defMacNum_; n++)
    { symbolTable_[defMac_[n].Name()]=&(defMac_[n]); }

    SetEmptyKey(aliasTable_, NULL);
    for(n=0; n<aliasNum_; n++)
    {
      BToken* tok = FindSymbol(alias_[n][1]);
      assert(tok);
      aliasTable_[alias_[n][0]] = tok;
    }
    SetEmptyKey(usrDefSymbol_, NULL);
    for(n=0; n<usrDefNum_; n++)
    {
      BToken* tok = FindSymbol(usrDef_[n]);
      assert(tok);
      usrDefSymbol_[usrDef_[n]] = tok;
    }
  }
}

//--------------------------------------------------------------------
void BScanner::Initialize(const BText& expression)

/*! Prepares it self to scan an expression. Initialize all non-static
 *  attributes with values extracted from \a expression parameter.
 */
//--------------------------------------------------------------------
{
  length_     = expression.Length();
  expression_ = expression.String();
  position_   = expression_;
  nextSymbol_ = NIL;
  lastSymbol_ = NIL;
  nextArgument_ = "";
  structDefininig_ = false;
}


//--------------------------------------------------------------------
BBool BScanner::NextChar()

/*! Moves position_ to the next no blank character.
 *  If there are no more characters moves position_ to NIL.
 */
//--------------------------------------------------------------------
{
  while((position_[0]==' ' ) ||
  (position_[0]=='\n') ||
  (position_[0]=='\t')) { position_++; }
  return(position_[0]);
}

//--------------------------------------------------------------------
/*! Reads the next word from \a position_.
 * <p> 
 * <b>Algorythm</b>:
 *  <ol type=a>
 *    <li> If there are no more text -> returns the type NONE.<br><br>
 *    <li> Else-If the next no blank character is a comma then:<br><br>
 *      <ol>
 *        <li> Puts the text until the next comma in \a nextArgument_ field,
 *        <li> moves \a position_ after the last comma, and
 *        <li> returns the type ARGUMENT.
 *      </ol><br><br>
 *    <li> Else reads the word until next blank character. Then<br><br>
 *      <ol>
 *        <li> Moves \a position_ after the blank character, and
 *          <ol type=a>
 *            <li> If this word is the name of a token then:
 *              <ol type=1>
 *                <li> Puts the word in \a nextSymbol_ field,
 *                <li> returns the type of the found token.
 *              </ol>
 *            <li> else
 *              <ol type=1>
 *                <li> Puts NIL in \a nextSymbol_ field,
 *                <li> puts the word in \a nextArgument_ field,
 *                <li> returns the type ARGUMENT.
 *              </ol>
 *          </ol>
 *      </ol>
 *  </ol>
 */
BTokenType BScanner::NextWordType() 
{
  BChar nextWord[256];
  BTokenByNameHash::const_iterator found;
  static BToken* structToken_ = NULL;
  static BToken* fieldToken_ = NULL;
  if(!structToken_)
  {
    structToken_ = FindSymbol("Struct");
    fieldToken_  = FindSymbol("->");
  }
  BTokenType type = ARGUMENT;
  if(nextSymbol_) { lastSymbol_ = nextSymbol_; }
  nextSymbol_  = NIL;
  if(NextChar()) 
  {
    if(position_[0]=='\"') 
    {
      BInt n=0;
      do { n++; }
      while(position_[n] && (position_[n]!='\"'));
      nextArgument_.Copy(position_, n);
      nextArgument_.Replace("\\'","\"");
      position_+=n+1;
    } 
    else 
    {
      BInt nChar;
      if(sscanf(position_, "%s%n", nextWord, &nChar))  
      {
        BInt rest = expression_+length_-position_;
        if(rest>nChar) { position_+=nChar; }
        else { position_=expression_+length_; }
        if(nChar>255) nextWord[255]='\0';

        nextSymbol_ = FindSymbol(nextWord);
        if(nextSymbol_ && lastSymbol_)
        {
          if((lastSymbol_==structToken_) && (nextSymbol_->TokenType()==OPEN))
          {
            structDefininig_ = true;
          }
          else if(structDefininig_ && (nextSymbol_->TokenType()==CLOSE))
          {
            structDefininig_ = false;
          }
          else if(structDefininig_ && (lastSymbol_->TokenType()==TYPE))
          {
            found = usrDefSymbol_.find(nextSymbol_->Name());
            if(found != usrDefSymbol_.end())
            {
              nextSymbol_ = NULL;
            }
          }
          else
          {
          //if((nextSymbol_->Name()=="Class")&&(lastSymbol_->Name()=="->"))
            if(lastSymbol_== fieldToken_)
            {
              found = usrDefSymbol_.find(nextSymbol_->Name());
              if(found != usrDefSymbol_.end())
              {
                nextSymbol_ = NULL;
              }
            }
          }
        }
        if(nextSymbol_)
        { type = nextSymbol_->TokenType(); }
        else 
        { nextArgument_.Copy(nextWord,nChar); }
      }
    }
  } 
  else 
  { 
    type = NONE; 
  }
  return(type);
}


//--------------------------------------------------------------------
/*! Reads the next word.
 *  If its type is ARGUMENT read the next word and returns its type,
 *  else returns ARGUMENT type.
 * \return Type of next word to be scanned
 * \sa BScanner::NextWordType
 */
BTokenType BScanner::ReadNextSymbol() 
{
  if(nextSymbol_) { lastSymbol_ = nextSymbol_; }
  nextSymbol_  = NIL;
  nextArgument_ = "";
  BTokenType type = NextWordType();
  if(type==ARGUMENT) { type = NextWordType(); }
  return (type);
}


//--------------------------------------------------------------------
BText BScanner::Scanned()

/*! Returns the text scanned at this moment.
 *  Its usefull for parser error messages.
 */
//--------------------------------------------------------------------
{
  return(BText(expression_,0,(position_-expression_)-1));
}
