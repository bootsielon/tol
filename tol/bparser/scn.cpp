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
BArray<BToken*> BScanner::symbolTable_;


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
    position_	= NIL;
    nextSymbol_	= NIL;
    nextArgument_ = "";
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
    else       		    { return(StrCmp(sym1->String(), sym2->String())); }
}


//--------------------------------------------------------------------
BInt Str2SymCmp(const BAny vBChar, const BAny vBToken)

/*! Comparision function to find by name in sorted arrays of BToken*
 *  <br><i>Obsolete method. Not used by TOL code</i>
 */
//--------------------------------------------------------------------
{
    BChar*  str =	  (BChar*)   vBChar;
    BToken& sym = **(BToken**) vBToken;
    
    return(strcmp(str, sym.String()));
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
    // danirus 2003/10/23; to support deprecated type Ration to be Ratio
    if (strcmp(str, "Ration") == 0) {
		BChar* new_str = (BChar*) str;
		new_str[5] = '\0';
    }
    // danirus 2003/10/23 - end;
    BToken  tok(str);
    BToken* found = NIL;
    BInt	  index = symbolTable_.FindSorted(&tok, BTokenOrderCriterium);
    if(index>=0) { found = symbolTable_[index]; }
/*
  Std(BText("\nFinding <") + str + ">" + " in " + symbolTable_.Size() +
	    "\nAddress " + (BInt)(this) + "\nTable Address " +
	    (BInt)(&symbolTable_));
  Std(BText(" = ") + index);
*/
  return(found);
}

//--------------------------------------------------------------------
BInt BScanner::CountSymbols()

/*! Return size of \a symbolTable_ attribute.
 */
//--------------------------------------------------------------------
{
    return(symbolTable_.Size());
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
//Std(BText("\nAdding Symbol <") + sym->Name()+">");
  symbolTable_.AddSorted(sym, BTokenOrderCriterium);

/*
  BInt index = symbolTable_.FindSorted(sym, BTokenOrderCriterium);
  Std(BText(" = ") + index +  " of " + symbolTable_.Size());
*/
    return(symbolTable_.Size());
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
  BToken tok(str);
//Std(BText("\nDeleting Symbol<") + str+">");
  BInt index = symbolTable_.FindSorted(&tok, BTokenOrderCriterium);
  if(index>=0)
  {
    DESTROY(symbolTable_[index]);
    int size = symbolTable_.Size();
    for(BInt i=index; i<size-1; i++)
    {
	    symbolTable_[i] = symbolTable_[i+1];
    }
    symbolTable_.ReallocBuffer(size-1);
  }
  return(symbolTable_.Size());
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
    BMonaryToken("Struct")
};

static BTypeToken defTyp_[] =
{
    BTypeToken("Anything"),
    BTypeToken("Set"),
    BTypeToken("Date"),
    BTypeToken("CTime"),
    BTypeToken("Real"),
    BTypeToken("Complex"),
    BTypeToken("Matrix"),
    BTypeToken("VMatrix"),
    BTypeToken("Polyn"),
    BTypeToken("Ratio"),
    BTypeToken("Text"),
    BTypeToken("TimeSet"),
    BTypeToken("CTimeSet"),
    BTypeToken("Serie"),
    BTypeToken("CSeries"),
    BTypeToken("Code"),
    BTypeToken("NameBlock")
};

static BInt defOpeNum_ = sizeof(defOpe_)/sizeof(BOpenToken);
static BInt defCloNum_ = sizeof(defClo_)/sizeof(BCloseToken);
static BInt defSepNum_ = sizeof(defSep_)/sizeof(BSeparatorToken);
static BInt defBinNum_ = sizeof(defBin_)/sizeof(BBinaryToken);
static BInt defMonNum_ = sizeof(defMon_)/sizeof(BMonaryToken);
static BInt defTypNum_ = sizeof(defTyp_)/sizeof(BTypeToken);

static BInt defTokNum_ =
                   defOpeNum_ +
                   defCloNum_ +
                   defSepNum_ +
                   defBinNum_ +
                   defMonNum_ +
                   defTypNum_ ;


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
	symbolTableFilled_ = BTRUE;
	symbolTable_.ReallocBuffer(defTokNum_);
	BInt n=0, m=0;
	for(n=0; n<defOpeNum_; n++)
	{ symbolTable_[n+m]=&(defOpe_[n]); }
	m+=n;
	for(n=0;n<defCloNum_; n++)
	{ symbolTable_[n+m]=&(defClo_[n]); }
	m+=n;
	for(n=0; n<defSepNum_; n++)
	{ symbolTable_[n+m]=&(defSep_[n]); }
	m+=n;
	for(n=0; n<defBinNum_; n++)
	{ symbolTable_[n+m]=&(defBin_[n]); }
	m+=n;
	for(n=0; n<defMonNum_; n++)
	{ symbolTable_[n+m]=&(defMon_[n]); }
	m+=n;
	for(n=0; n<defTypNum_; n++)
	{ symbolTable_[n+m]=&(defTyp_[n]); }

	symbolTable_.Sort(BTokenOrderCriterium);
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
    nextArgument_ = "";
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
BTokenType BScanner::NextWordType() {
    BChar nextWord[256];
    BTokenType type = ARGUMENT;
    
    nextSymbol_ = NIL;
    if(NextChar()) {
	if(position_[0]=='\"') {
	    BInt n=0;
	    do { n++; }
	    while(position_[n] && (position_[n]!='\"'));
	    nextArgument_.Copy(position_, n);
	    nextArgument_.Replace("\\'","\"");
	    position_+=n+1;
	} else {
	    BInt nChar;
	    if(sscanf(position_, "%s%n", nextWord, &nChar))	{
		BInt rest = expression_+length_-position_;
		if(rest>nChar) { position_+=nChar; }
		else { position_=expression_+length_; }
		if(nChar>255) nextWord[255]='\0';
		if((nextSymbol_ = FindSymbol(nextWord)))
		{ type = nextSymbol_->TokenType(); }
		else { nextArgument_.Copy(nextWord,nChar); }
	    }
	}
    } else { type = NONE; }
    return(type);
}


//--------------------------------------------------------------------
/*! Reads the next word.
 *  If its type is ARGUMENT read the next word and returns its type,
 *  else returns ARGUMENT type.
 * \return Type of next word to be scanned
 * \sa BScanner::NextWordType
 */
BTokenType BScanner::ReadNextSymbol() {
    nextSymbol_	= NIL;
    nextArgument_ = "";

    BTokenType type = NextWordType();
    if(type==ARGUMENT) { type = NextWordType(); }
/*
    if(nextSymbol_)
    {
      Std(BText("\nReadNextSymbol SYM'")+nextSymbol_->Name()+"' ARG'"+nextArgument_+"')");
    }
    else
      Std(BText("\nReadNextSymbol SYM'' ARG'")+nextArgument_+"')");
*/
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
