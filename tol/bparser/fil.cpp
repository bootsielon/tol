/* bfilter.h: Filter for Parser Proccess.
              GNU/TOL Language.

   Copyright (C) 2003-2005, Bayes Decision, SL (Spain [EU])

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

#include <tol/tol_bfilter.h>
#include <tol/tol_barray.h>
#include <tol/tol_bout.h>
#include <tol/tol_bparser.h>

#include <ctype.h>
#include <assert.h>


BTraceInit("fil.cpp");

//--------------------------------------------------------------------
//! \brief Replace first column string instance by second column string
/*! Each time the first column string appear in code will be 
 *  replaced with the second column string. This is done to
 *  make parsing proccess more easy.<br>
 *  At this moment this data structure is not used, in
 *  substitution its work is done by Clean method.
 * \sa Clean
 */
static BReplacement replaceTable[] = 
{
    { BText(") \n {") , BText(") #F# {")	   },
    { BText("[[")     , BText("SetOfAnything #{#") },
    { BText("]]")     , BText("#}#")	           },
    { BText("[")      , BText("#E# #(#")	   },
    { BText("]")      , BText("#)#")	           }
};

static BReplacement cutReplaceTable[] =
{
    { BText(") \n {") , BText(") #F#") },
    { BText("[")      , BText("#E#")   }
};

static BInt replaceNumber = sizeof(replaceTable) / sizeof(BReplacement);
static BInt cutReplaceNumber = sizeof(cutReplaceTable) / sizeof(BReplacement);

//--------------------------------------------------------------------
bool BFilter::hasError() const {
    return error_;
}

//--------------------------------------------------------------------
BText BFilter::getErrorMsg() const {
    return errorMsg_;
}

//--------------------------------------------------------------------
BBool BFilter::IsFirstIdentifier(BChar ch) const

/*! Returns true if \a ch can be the first character of an identifier.
 */
//--------------------------------------------------------------------
{
  return
  (
    (ch=='ñ')   || 
    (ch=='Ñ')   ||
    (ch=='_')   ||
    isalpha((unsigned char)ch)
  );
}

//--------------------------------------------------------------------
BBool BFilter::StartIdentifier(BChar ch) const

/*! Returns true if \a ch can be the first character of an identifier.
 */
//--------------------------------------------------------------------
{
  return
  (
    IsFirstIdentifier(ch) || (ch=='@')
  );
}
//--------------------------------------------------------------------
//! Returns true if ch can belong to an identifier.
/*! Returns true if ch can belong an identifier.<br>
 *  Valid identifier characters can be <code>0-9</code>,
 *  <code>a-z</code>, <code>A-Z</code>, or each one of these:<br>
 *  <code>. _ ñ Ñ \ </code>
 * \param ch BChar that will be evaluated
 */
BBool BFilter::IsIdentifier(BChar ch) const 
{
  if(IsFirstIdentifier(ch)) { return(true); }
  if((ch<=0)||(ch>127)) { return(false); }
  return(isdigit(ch)||(ch=='.')||(ch=='\''));
}


//--------------------------------------------------------------------
//! Return true if the expression is an identifier.
/*! Return true if the expression is an identifier.<br>
 *  Valid \a expression need to begin with character between a-z or
 *  A-Z and has only characters between 0-9, a-z and A-Z. In other
 *  case \a expression isn't an identifier.
 * \param expression Evaluated BText to know if this is a valid expression
 */
BBool BFilter::IsIdentifier(const BText& expression) const 
{
  if(!expression.HasName()) { return(BFALSE); }
  if((expression[0]=='?')&&(expression[1]=='\0')) { return(BTRUE); }
  if(expression[0]=='#')
  {
    if(expression=="#Embed") { return(BTRUE); }
    if(expression=="#Require") { return(BTRUE); }
  }
  int n;
  char ch = expression.Get(0);
  if(!StartIdentifier(ch)) { return(BFALSE); }
  for(n=1; (ch=expression.Get(n)); n++)
  {
    if(!IsIdentifier(ch)) { return(BFALSE); }
  }
  return(BTRUE);
}


//--------------------------------------------------------------------
//! Returns true if ch is a symbol.
/*! Return true if ch is a symbol.<br>
 *  \a ch can be a symbol if is one of these:<br><code>
 *  ' ( ) { } [ ] ; , : - ! + * / % ^ ! & | < = > @ \\\ \\n \r \t ? ~ $*
 * \param ch BChar that will be evaluated
 */
BBool BFilter::IsSymbol(BChar ch) const 
{
    return (strchr("'(){}[];,:-!+*/%^!&|<=>\\\n\r\t?~$", ch)!=NIL);
}

//--------------------------------------------------------------------
/*! Returns true if ch can be condidered a blank symbol.<br>
 *  Symbols like " " or "\\n" or "\t" or "\r" will be considered 
 *  blank.<br>
 *  This static function is being used only in fil.cpp and isn't
 *  a member of the class BFilter.
 * \param ch BChar that will be evaluated
 */
static BBool IsBlank(BChar ch)
{
    return (strchr(" \n\t\r", ch)!=NIL);
}


//--------------------------------------------------------------------
//! Returns true if joint of ch1 and ch2 characters are a known sign.
/*! Returns true if two joint characters are a known sign.<br>
 *  Here is the list of inseparables characters:<br><ol><code>
 *  <li>&& <li>|| <li>-> <li><: <li>:> <li><<
 *  <li>>> <li>** <li>[!:<>=]=  </code></ol>
 *  Last combination means first character could be one of each 
 *  included into brackets combined with =.
 */
BInt BFilter::Inseparable(BChar ch1, BChar ch2) const 
{
   return ( ((ch1=='&') && (ch2 == '&')) ||
	     ((ch1=='$') && (ch2 == '*')) ||
	     ((ch1=='$') && (ch2 == '/')) ||
	     ((ch1=='|') && (ch2 == '|')) ||
	     ((ch1=='-') && (ch2 == '>')) ||
	     ((ch1=='<') && (ch2 == ':')) ||
	     ((ch1==':') && (ch2 == '>')) ||
	     ((ch1=='<') && (ch2 == '<')) ||
	     ((ch1=='>') && (ch2 == '>')) ||
	     ((ch1=='*') && (ch2 == '*')) ||
	     ((ch1=='!') && (ch2 == '=')) ||
	     ((ch1==':') && (ch2 == '=')) ||
       ((ch1==':') && (ch2 == ':')) ||
	     (strchr("=<>", ch1) && (ch2=='=')) );
}


//--------------------------------------------------------------------
//! Used from Clean method to add new char to filtered BText argument.
/*! Adds new character \a ch to filtered code (in BText \a cl) in 
 *  order to insert one blank character between symbols and 
 *  identifiers.
 * \param cl is a BText where, by adding \a ch, filtered code is put
 * \param n is the position in \a cl where \a ch is put
 * \param ch is the character to be added
 * \sa Clean
 */
void BFilter::AddNewChar(BText& cl, BInt& n, BChar ch) const 
{
  static int length = 0;
  BBool isSym  = IsSymbol(ch);
  char& cn = cl.Buffer()[n];
  BBool isIdCl = IsIdentifier(cn) || cn=='#' || (cn=='@');
  BBool isId   = IsIdentifier(ch) || ch=='#';
  if(!isIdCl && (ch=='@')) 
  { isId=true; }

  if (isSym && !isIdCl && Inseparable(cn, ch)) { cl.PutChar(++n,ch); } 
  else if(ch=='\r') { } 
  else if((cn=='\n')&&((ch=='\n')||(ch=='\t')||(ch==' '))) { } 
  else if(cn==' ') 
  {
    if(isId || isSym) 
    { 
      length = n;
      cl.PutChar(++n,ch); 
    }
  } 
  else if(isIdCl) 
  {
    if(isId) 
    {
      if((n-length)==255) 
      {
        error_ = true;
        errorMsg_ = I2("Parser: Identifier too long",
                       "Parser: Identificador demasiado largo");
      } 
      else 
      {
        cl.PutChar(++n,ch);
      }
    } 
    else if(isSym) 
    {
      cl.PutChar(++n,' ');
      length = n;
      cl.PutChar(++n,ch);
    } 
    else 
    {
      cl.PutChar(++n,' ');
    }
  } 
  else 
  {
    cl.PutChar(++n,' ');
    if(isId || isSym) 
    { 
      length = n;
      cl.PutChar(++n,ch); 
    }
  }
}

//! Return type of block code filtered in \a pos parameter
//--------------------------------------------------------------------
/*! Verify firsts characters of \a pos BChar array and return
 *  type of block detected.<br>
 * \param pos BChar* that will be classified
 * \return type of block code
 */
int BFilter::ClassifyBlock(char* pos) const
{
    if(!pos)                              { return(NoneBlock);     }
    else if((pos[0]=='/')&&(pos[1]=='/')) { return(Comment1Block); }
    else if((pos[0]=='/')&&(pos[1]=='*')) { return(Comment2Block); }
    else if( pos[0]=='\"')		  { return(QuotesBlock);   }
    else				  { return(NormalBlock);   }
}


//--------------------------------------------------------------------
void BFilter::CloseBlock(char**      begin,
			 char**      pos,
			 int&        num,
			 int&        lineNum,
			 BTextBlock& block) const
{

    BInt length = (*pos) - (*begin);

    if(!length) { return; }

    BInt same = BFALSE;
    BInt newBlock = ClassifyBlock(*begin);

    if(num>0) {
	BInt lastBlock = ClassifyBlock(block[num-1].text.Buffer());
	//  Std(BText("\nLast = ") + lastBlock+" New = " + newBlock);
	if(lastBlock==newBlock)	{
	    same = lastBlock;
	}
    }

    if(same/*!=NormalBlock*/) {
	BText& last = block[num-1].text;
	if(same==QuotesBlock) {
	    while(IsBlank(last.Last(0))) {
		last.PutLength(last.Length()-1);
	    }
	    last.PutLength(last.Length()-1);
	    while(IsBlank(**begin)) {
		(*begin)++;
		length--;
	    }
	    (*begin)++;
	    length--;
	}
	BText txt(*begin,length);
	last+=txt;
    } else {
	if(num>=block.Size()) { block.ReallocBuffer(2*(num+10)); }
	block[num].line=lineNum;
	block[num].text.Copy(*begin,length);
	num++;
    }

    *begin = *pos;
}


//--------------------------------------------------------------------
//! Separate code into pieces like: comments, quoted text, and normal code.
/*! Extract blocks from \a original parameter and stores it 
 *  in \a block parameter(a BArray of BLineText).<br>
 *  We consider a "block" each set of code of the following list:
 *  <ol>
 *    <li>Delimited by quotations marks.
 *    <li>One line comments.
 *    <li>Multiple line comments.
 *    <li>Code delimited by quotation marks.
 *  </ol>
 * \param original source code where blocks will be extracted
 * \param block BArray with extracted blocks 
 * \sa ClassifyBlock
 */
BInt BFilter::GetBlocks(BText& original, BTextBlock& block) const
{
  BChar* pos= original.Buffer();
  BChar* begin = pos;
  BInt num = 0;
  BInt lineNumber = 0;
  block.ReallocBuffer(100);
  BInt newClass;

  for(; pos[0];) 
  {
	  newClass = ClassifyBlock(pos);
	  if(newClass==Comment1Block) 
    {
	    if(pos[0]) 
      {
		    CloseBlock(&begin,&pos,num,lineNumber,block);
		    pos+=2;
		    while(pos[0] && (pos[0]!='\n')) { pos++; }
		    if(pos[0]) { pos++; }
		    lineNumber++;
	    }
  	} 
    else if(newClass==Comment2Block) 
    {
	    if(pos[0]) 
      {
		    CloseBlock(&begin,&pos,num,lineNumber,block);
		    pos+=2;
		    BInt openLineNumber = lineNumber;
		    while(pos[0] && ((pos[0]!='*')||(pos[1]!='/'))) 
        {
		      pos++;
		      if(pos[0]=='\n') { lineNumber++; }
		    }
		    if (!pos[0]) 
        {
		      Error(I2("Comment symbol /* that were opened in the line ",
 		           "El símbolo de comentario que se abrió en la línea ")+
			    openLineNumber +
			    I2(" have not been closed with */.",
			       " no se ha cerrado con */."));
		      return(BFALSE);
		    } 
        else 
        {
		      pos+=2;
		    }
	    }
	  } 
    else if(newClass==QuotesBlock) 
    {
	    if(pos[0]) 
      {
		    CloseBlock(&begin,&pos,num,lineNumber,block);
		    BInt openLineNumber = lineNumber;
		    BBool end = BFALSE;
        BBool bs  = BFALSE;
		    do 
        {
		      pos++;
		      if(pos[0]=='\"') { end = !bs; }
		      if(pos[0]=='\\') { bs = !bs; }
          else if(bs)      { bs = false; }
		      if(pos[0]=='\n') { lineNumber++; }
		    }
	      while(pos[0] && !end);
	      if (pos[0]!='\"') 
        {
	        Error(I2("The quotation marks that were opened in line ",
	    	           "Las comillas que se abrieron en la línea ")+
		            openLineNumber +
		            I2(" have not been closed.",
		               " no se han cerrado."));
	        return(BFALSE);
	      } 
        else 
        {
	        pos++;
	        while(pos[0] && IsBlank(pos[0])) 
          {
		        if(pos[0]=='\n') { lineNumber++; }
		        pos++;
	        }
	      }
      }
    } 
    else 
    {
      if(pos[0]) 
      {
	      CloseBlock(&begin,&pos,num,lineNumber,block);
	      do 
        {
	        if(pos[0]=='\n') { lineNumber++; }
	        pos++;
	        newClass = ClassifyBlock(pos);
	      } while(pos[0] && (newClass==NormalBlock));
      }
    }
  }
  CloseBlock(&begin,&pos,num,lineNumber,block);
  block.ReallocBuffer(num);
  return(BTRUE);
}


//--------------------------------------------------------------------
//! Prepare an expression for parser deleting comments and putting spaces
/*! Filters \a expression of comments, and replaces all consecutive
 *  non valid characters (including multiple spaces) by a symple
 *  space. It also inserts a simple space between a symbol-character
 *  and an identifier-character and viceversa.
 * \param expression BText with code that will be modified
 * \sa AddNewChar
 */
void BFilter::Clean(BText& expression) const {
    BChar* pos = expression.Buffer();

    if(pos) {
	BInt maximum = 10*(expression.Length()+10);
	BText cleaned;
	cleaned.ReallocateBuffer(maximum + 1);

	for(BInt k=0; k<=maximum; k++) { cleaned.Buffer()[k] = ' '; }

	BInt n = 0;

	for(; pos[0]; pos++) {
	    assert(n<maximum);

	    if(pos[0]==')') {
		BInt k=1;
		while(pos[k] && IsBlank(pos[k])) { k++; }
		if(pos[k]=='{') {
		    pos+=k;
		    strncpy(cleaned.Buffer()+n+1," ) #F# { ",9);
		    n+=9;
		} else{
		    AddNewChar(cleaned, n, pos[0]);
		}

	    } else if(pos[0]=='[') {
		if(pos[1]=='[') {
		    pos++;
		    strncpy(cleaned.Buffer()+n+1," SetOfAnything #{# ",19);
		    n+=19;
		} else{
		    strncpy(cleaned.Buffer()+n+1," #E# #(# ",9);
		    n+=9;
		}

	    } else if(pos[0]==']') {
		if(pos[1]==']') {
		    pos++;
		    strncpy(cleaned.Buffer()+n+1," #}# ",5);
		    n+=5;
		} else {
		    strncpy(cleaned.Buffer()+n+1," #)# ",5);
		    n+=5;
		}

	    } else if((isdigit((unsigned char)pos[0])||(pos[0]=='.')) && 
		      (tolower((unsigned char)pos[1])=='e') &&
		      ((pos[2]=='+')||(pos[2]=='-')) && 
		      (isdigit((unsigned char)pos[3])))
	    {
	      int i;
	      for(i=0; pos[i] && !IsSymbol(pos[i]) && !IsBlank(pos[i]); i--);
	      if(pos[i+1] && isalpha(pos[i+1]))
		AddNewChar(cleaned, n, pos[0]);
	      else
		{
		  if(IsSymbol(cleaned.Get(n))) { cleaned.PutChar(++n, ' '); }
		  cleaned.PutChar(++n, pos[0]);
		  cleaned.PutChar(++n, pos[1]);
		  cleaned.PutChar(++n, pos[2]);
		  cleaned.PutChar(++n, pos[3]);
		  pos++;
		  pos++;
		  pos++;
		}
	    } else {
		AddNewChar(cleaned, n, pos[0]);
	    }
	}
	cleaned.PutLength(++n);
	expression=cleaned;
    }
}


//--------------------------------------------------------------------
//! Filters special characters in quoted \a expression for parser 
/*! Filters quoted expression. This function is called to Clean
 *  quoted \a expression from Transform.<br>
 *  Ensure that certain special characters that could be appear
 *  between quotes (like \t \\n \\\ and \') will be filtered to 
 *  final expression returned to parser by Transform method.
 * \param expression BText with code that appears between quotes
 * \sa Transform, Clean
 */
void BFilter::CleanQuotes(BText& expression) const
{
    BInt lineNumber = 0;
    BChar* pos = expression.Buffer()+1;

    if(pos && pos[0]) {
	BInt maximum = 10*(expression.Length()+10);
	BText cleaned;
	cleaned.ReallocateBuffer(maximum);
	cleaned="\"";
	BInt n = 0;
	do {
	    if(pos[0]=='\n') { lineNumber++; }
	    if(pos[0]=='\\') {
		if(pos[1]=='\\') {
		    pos++;
		    cleaned.PutChar(++n,'\\');
		} else if(pos[1]=='\"') {
		    pos++;
		    cleaned.PutChar(++n,'\\');
		    cleaned.PutChar(++n,'\'');
		} else if(pos[1]=='n') {
		    pos++;
		    cleaned.PutChar(++n,'\n');
		} else if(pos[1]=='t') {
		    pos++;
		    cleaned.PutChar(++n,'\t');
		} else {
		    cleaned.PutChar(++n,'\\');
		}
	    } else {
		cleaned.PutChar(++n,pos[0]);
	    }
	    pos++;
	} while(pos[0]);
	n++;
	cleaned.PutLength(n);
	expression=cleaned;
    }
}

//--------------------------------------------------------------------
//! Entry point in Filter proccess. Prepare code for Parser.
/*! Entry point in Filter proccess. Basically calls to Clean and 
 *  CleanQuotes methods.<br>
 *  Receive user code as argument and prepare it for parser.
 *  First action called: GetBlocks divide code into some special
 *  units. Then analize those blocks, ignoring comments. Normal code
 *  blocks are treated by Clean method and quoted code by
 *  CleanQuotes method. The resulted code is returned to Parser.
 * \param original Is the code that will be filtered by BFilter
 * \return Filtered code without comments
 * \sa GetBlocks, Clean, CleanQuotes, ClassifyBlock
 */
BText BFilter::Transform(const BText& original) const 
{
  error_ = false;
  errorMsg_ = "";
  BText expression = original;
  BTextBlock block;
  GetBlocks(expression, block);
  expression = "";
  BText cleaned = "";
  for(BInt k = 0; k<block.Size(); k++) 
  {
    BInt blockClass=ClassifyBlock(block[k].text.Buffer());
    if((blockClass==NormalBlock)) 
    {
      cleaned += block[k].text + " ";
    }
    if(blockClass==QuotesBlock) 
    {
      Clean(cleaned);
      CleanQuotes(block[k].text);
      expression += cleaned + " " + block[k].text + " ";
      cleaned = "";
    }
  }
  if(cleaned.HasName()) 
  {
    Clean(cleaned);
    expression += cleaned;
  }

  while(expression.Length() && strchr(" \t\n;",expression.Last())) 
  {
    expression.PutLength(expression.Length()-1); 
  }
  if(error_) { return errorMsg_; }
  else 
  {
    BText s20(expression,0,20);
    s20.Compact();
  //Std(BText("BFilter::Transform 1 : '")<<s20<<"'");
    if(s20.BeginWith("#Embed"))
    {
      expression = BText("Write ( \"\" ) ; ") << expression;
    };
    return expression;
  }
}


//--------------------------------------------------------------------
//! Make replacements in BText corresponding to n-th row of replaceTable.
/*! In expression, it replaces certain sequences of characters,
 *  indicated in the first column and n-th row of replaceTable,
 *  by the correspondent sequence in the second column.<br>
 *  At this moment this method is not used. Its work is done in Clean
 *  method.
 * \param n Row of replaceTable where happens actions of this method
 * \param expression string code where replacements will be done
 * \param dir Direction of changes -> col1 to col2 or viceversa
 * \sa Clean
 */
int BFilter::AnyReplace (int n, 
			 BText& expression, 
			 int dir, 
			 BReplacement* repTable) const 
{
    BInt replacements = 0;

    if(expression.HasName()) {
	const BChar* pos;
	const BChar* last = expression.String();
	BText replaced;
	
	replaced.ReallocateBuffer(3*expression.Length()+1);

	while(last && last[0] &&
	      (pos = strstr(last, repTable[n][dir].String()))) {
	    replacements++;
	    replaced += BText(last, 0, pos-last-1);
	    replaced += repTable[n][!dir];
	    last = pos + repTable[n][dir].Length();
	}

	if(replacements) {
	    replaced << last;
	    expression = replaced;
	}
    }

    return(replacements);
}

//--------------------------------------------------------------------
//! Search in BText to performs pertinent replacements.
/*! In expression, it replaces all sequences of characters,
 *  indicated in the first column of replaceTable, by the
 *  correspondent sequences in the second column.<br>
 * \param expression string code where replacements will be done
 * \param dir Direction of changes -> col1 to col2 or viceversa
 */
void BFilter::AnyReplace(BText& expression, int dir) const 
{
    if(expression.HasName()) {
	BInt n,replacements;
	do {
	    replacements = 0;
	    if(!dir) {
		for(n=0; n<replaceNumber; n++) {
		    replacements += AnyReplace(n, expression, dir,
					       replaceTable); 
		}
	    } else {
		for(n=replaceNumber-1; n>=0; n--) { 
		    replacements += AnyReplace(n, expression, dir,
					       replaceTable); 
		}
		for(n=cutReplaceNumber-1; n>=0; n--) { 
		    replacements += AnyReplace(n, expression, dir, 
					       cutReplaceTable); 
		}
	    }
	} while (replacements);
    }
}

//--------------------------------------------------------------------
//! Make string replacements defined in replaceTable static var
/*! Only performs a call to AnyReplace(txt, BFALSE);
 * \sa AnyReplace
 */
void BFilter::Replace(BText& txt) const { 
    AnyReplace(txt, 0); 
}

//--------------------------------------------------------------------
//! Unreplace replacements done by Replace method
/*! Only performs a call to 
 *   <code>AnyReplace(txt, BTRUE);</code>
 * \sa AnyReplace
 */
void BFilter::UnReplace(BText& txt) const { 
    AnyReplace(txt, 1); 
}

//- external function ------------------------------------------------
//! Replace invalid characters with "_"
/*! Replace with "_" all characters considered invalid by 
 *  IsIdentifier method
 * \param txt BText with Identifier to check
 * \return modified BText with "_" character
 * \sa BFilter::IsIdentifier
 */
BText ToName(BText txt) 
{
  static BFilter filter;
  BText name = txt;
  if(!filter.StartIdentifier(name[0]))
  {
    name = BText("_")+name;
  }
  int i, len = name.Length();
  BChar ch = name(0);
  // following chars:
  for(i=1; i<len; i++) 
  {
	  ch = name(i);
	  if(!filter.IsIdentifier(ch)) 
    {
  	  name.PutChar(i,'_');
	  }
  }
  return(name);
}

//--------------------------------------------------------------------
BText UnscapeTOLString(const BText& expression)
//--------------------------------------------------------------------
{
  BText unscaped = expression;
  BParser::DefaultParser()->Filter()->CleanQuotes(unscaped);
  return(unscaped);
} 