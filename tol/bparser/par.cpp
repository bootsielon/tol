/* par.cpp: Implements BParser behavior of GNU/TOL language.

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

#include <tol/tol_bparser.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdir.h>
#include <tol/tol_bfilter.h>
#include <tol/tol_bscanner.h>

BTraceInit("par.cpp");



const BText& TokenTypeName(BTokenType tt)
{
  static BText tokenType_ [] = {
    "NONE",
    "OPEN",
    "CLOSE",
    "SEPARATOR",
    "MONARY",
    "BINARY",
    "ARGUMENT",
    "FUNCTION",
    "TYPE",
    "MACRO" };
  return(tokenType_[tt+1]);
};

//! initialize static member of class parser
BParser* BParser::defaultParser_ = 0;

// Constants and BText Array to print syntactical tree
#define INDENT  "----> "


//--------------------------------------------------------------------
//! Initialize \a filter_, \a scan_, \a openNumber_ and \a complete_
/*! Initialize some important attributes.<br>
 *  Set to 0 \a openNumber_ (Number of non closed open symbols) and
 *  set to BFALSE \a complete_ that indicate if Syntactic Tree is
 *  complete. Furthermore, initialize \a filter_ (responsible of 
 *  filter proccess) and \a scan_ (resposible of scanner proccess).
 */
BParser::BParser()
: expression_     (""),
  filter_         (NULL),
  scan_           (NULL),
  openNumber_     (0),
  complete_       (false),
  classInheritage_(false),
  nextSymbol_     (NULL),
  nextArgument_   (NULL),
  messageError_   (""),
  newSymbol_      (NULL),
  lastSymbol_     (NULL),
  lastSymbol_2_   (NULL)
{
  filter_  = new BFilter;
  scan_    = new BScanner;
}

//--------------------------------------------------------------------
//! Destructor that remove \a filter_ and \a scan_ objects
/*! Destroy \a filter_ and \a scan_ objects.
 */
BParser::~BParser() 
{
  delete filter_;
  delete scan_;
}

//--------------------------------------------------------------------
//! Point of entry for BParser Proccess. Receive code need to be parsed.
/*! Filters and parses an expression.
 *  If an error message was found adds error location information
 *  and displays it.
 */
Tree* BParser::Parse(const BText& expr)
{
  Tree* tree = new Tree();
  expression_ = expr;
  newSymbol_ = NIL;

  BText expression = filter_->Transform(expr);
  if(filter_->hasError()) 
  { 
    Error(filter_->getErrorMsg());
  } 
  else if(expression.HasName()) 
  {
    scan_->Initialize(expression);
    openNumber_ = 0;
    complete_ = BFALSE;
    messageError_ = "";
    tree = ParseSymbol(tree, NIL);
    if(!messageError_.HasName() && (tree->isEmpty() || openNumber_)) 
    {
      messageError_+=
      I2("Unexpected end of expression. Close symbol was expected. ",
         "Fin de la expresión inesperado. "
         "Se esperaba símbolo de cierre. ");
    }
    if(tree->isEmpty() || messageError_.HasName()) 
    {
      List* lst = newSymbol_;
      while(lst) 
      {
        BObject* arg = (BObject*)(lst->car());
        scan_->DelSymbol(arg->String());
        lst = lst->cdr();
      }
      DESTROY(newSymbol_);
      BInt column=0, row=0;
      BText line = ErrorPosition(row, column);
      messageError_ += I2(" in character ", " en el carácter ") +
                       column + I2(" of line ",", línea ") + row + ":\n" +
                       line + "\n";
      Error(messageError_);
      if(NextArgument()) 
      {
        NextArgument()->Destroy();
      }
      if(NextSymbol() && (NextSymbol()!=scan_->NextSymbol())) 
      {
        NextSymbol()->Destroy();
      }
      if (tree->getTree()) 
      {
        (tree->getTree())->Destroy();
        tree->setTree(NIL);
      }
    }
  //SumPartialTime;
  //BTimer::PrintProcess();
  }
  return(tree);
}

//--------------------------------------------------------------------
//! this method is used for parsing TOL code in setgra.cpp
/*! Parses an expression with a given parser
 */
Tree* BParser::Parsing (BParser* parse, const BText& expression) 
{
  return (parse->Parse(expression));
}

//-- static method ---------------------------------------------------
//! Parses an expression with the standard parser
Tree* BParser::parsing (const BText& expression) 
{
  return (BParser::DefaultParser()->Parse(expression));
}

//- static method ----------------------------------------------------
//! Used to create unique instance of BBParser \a defaultParser_
/*! Create unique instance of BBParser
 */
void BParser::InitDefaultParser() 
{
  if ( !BParser::defaultParser_ )
  BParser::defaultParser_ = new BParser;
}

//- static method ----------------------------------------------------
//! Return a pointer to \a defaultParser_
/*! Return pointer to unique created instance of BBParser
 */
BParser* BParser::DefaultParser() 
{
  InitDefaultParser();
  return(BParser::defaultParser_);
}

//- static method ----------------------------------------------------
//! Construct a standard expression from a given Tree.
/*! Construct a standard expression from a tree.
 */
const BText& BParser::Unparsing(const Tree* tre,
        const BText& indent,
        const BText& endLine)
{
  const BText& txt = Unparse(tre->getConstTree(), indent, endLine);
  return(txt);
}

//- static method ----------------------------------------------------
//! Construct a standard expression from a given List.
/*! Construct a standard expression from a tree.
 */
const BText& BParser::Unparse (const List* tre,
             const BText& indent,
             const BText& endLine)
{
  if(tre) 
  {
    List* b = (List*) tre;
    BToken* tok = BParser::treToken(b);
    BText txt = "";
    if(tok) 
    { 
      txt = tok->Unparsed();
    }
    if(tok && !txt.HasName()) 
    {
      BText indent2   = BText("")+(indent.String()+2);
      switch (tok->TokenType()) 
      {
        case SEPARATOR:
        {
          BSeparatorToken* sep = (BSeparatorToken*)tok;
          BText newIndent = indent;
          BText oldIndent = indent;
          newIndent += "  ";
          if(oldIndent.HasName()) 
          {
            txt += indent2 + BParser::tokOpen(tok) + endLine;
          }
          while((b = b->cdr())) 
          {
            txt += indent + BParser::Unparse((List*) (b->car()),
                   newIndent, endLine);
            if(b->cdr()) { txt += sep->Name(); }
            txt += endLine;
            if(!oldIndent.HasName()) { txt += endLine; }
          }
          if(oldIndent.HasName()) 
          {
            txt +=  indent2 + BParser::tokClose(tok);
          }
          break;
        }
        case ARGUMENT:
        {
          BText aux = tok->Name();
          if(aux.Get(0)=='\"')
          {
            int len = aux.Length();
            if(aux.Last()=='\"') { len--; }
            BText aux1 = aux .SubString(1,len);
            BText aux2 = Replace(aux1,"\"","\\\"");
            BText aux3 = Replace(aux2,"\\","\\\\");
            aux = BText("\"")+aux3+"\"";
          }
          txt += BParser::tokOpen(tok) + aux + BParser::tokClose(tok);
          break;
        }
        case MONARY:
        {
          txt += BParser::tokOpen(tok);
          txt += tok->Name();
          if(isalpha(tok->Name().Get(0))) { txt += " "; }
          if((b = b->cdr())) 
          {
            txt += BParser::Unparse((const List*) Tree::treLeft((List*)tre),
                                    indent,endLine);
          }
          txt += BParser::tokClose(tok);
          break;
        }
        case FUNCTION:
        {
          txt += BParser::tokOpen(tok);
          txt += tok->Name();
          BText open;
          BText close;
          if((b = b->cdr())) 
          {
            if(b->cdr()) { open = "("; close = ")"; }
            txt += open.Name();
            txt += BParser::Unparse((List*) (b->car()), indent, endLine);
            while((b = b->cdr())) 
            {
              txt += BText(", ") + BParser::Unparse((List*) (b->car()),
                     indent,endLine);
            }
          }
          txt += close;
          txt += BParser::tokClose(tok);
          break;
        }
        case BINARY:
        {
          BText left = BParser::Unparse(Tree::treLeft((List*)tre),
                       indent,endLine);
          BText right = BParser::Unparse(Tree::treRight((List*)tre),
                        indent,endLine);
          txt += BParser::tokOpen(tok);
          BToken* tr = BParser::treToken(Tree::treRight((List*)tre));
          if (tok->Name()=="#F#") 
          {
            if(tr && tr->TokenType() != SEPARATOR) 
            {
              txt += left  + endLine + indent2 + 
                     "{" + endLine  + "  " +
                     right + endLine + indent2 + "}";
            } 
            else 
            {
              txt += left + endLine + right;
             }
          } 
          else if ((tok->Name()=="=") && tr && 
                  (tr->TokenType() == SEPARATOR))
          { 
            txt += left + "=" + endLine + right; 
          } 
          else 
          { 
            txt += left + tok->Name() + right;     
          }
          txt += BParser::tokClose(tok);
          break;
        }
        case TYPE:
        {
          txt += BParser::tokOpen(tok) + tok->Name();
          txt += " ";
          BText left = BParser::Unparse(Tree::treLeft((List*)tre),
                indent,endLine);
          txt += left;
          txt += BParser::tokClose(tok);
          break;
        } 
        default: break;
      }
      static BFilter filter;
      filter.UnReplace(txt);
      txt.Replace("#E#","");
      txt.Replace("#(#","[");
      txt.Replace("#)#","]");
      txt.Replace("#{#","[[");
      txt.Replace("#}#","]]");
      tok->PutUnparsed(txt);
    }
    return(tok->Unparsed());
  }
  return(BText::None());
}

//- static method ----------------------------------------------------
//! Returns BText object with printed syntactical tree
/*! Returns a formatted text with information of a tree for tracing.
 * \param tree List to be printed in BText returned value
 * \param indent String character used like indent
 * \param last Bool value used to control end of tree
 * \return BText with printed Syntactical Tree
 */
BText BParser::treWrite (List* tree, BText indent, BBool last) 
{
  BText txt;
  List* branch;
    
  if (tree && tree->car()) 
  {
    if (last) 
    {
      indent.PutChar(indent.Length()-1,'|');
    }
    txt << tokWrite(BParser::treToken(tree), indent);
    if (last) 
    {
      indent.PutChar(indent.Length()-1,' ');
    }
    if ((branch = tree->cdr())) 
    {
      BText newIndent = indent;
      newIndent << " |";
      while (branch) 
      {
        if (branch->car()) 
        {
          List* b = (List*) branch->car();
          txt << BParser::treWrite(b, 
                 newIndent, (branch->cdr()==NIL));
        }
        branch = branch->cdr();
      }
    }
  }
  return txt;
}

//- static method ----------------------------------------------------
//! Return the first token of a tree
/*! Returns first token of a tree. Make search in-depth to
 *  find first BToken of the \a tre Tree parameter.
 * \param tre Tree with syntactical tree
 * \return first BToken object of the \a tre Tree parameter
 */
BToken* BParser::treToken (const List* tree) 
{
  if (tree) 
  {
    BCore* cont = Tree::treNode((List*) tree);
    if (cont) 
    {
      if (Tree::isBranch(cont)) 
      {
        return BParser::treToken((List*) cont);
      }
      else 
      {
        return ((BToken*) cont);
      }
    } 
    else 
    {
      return (NIL);
    }
  } 
  else 
  {
    return (NIL);
  }
}

//- static method -------------------------------------------------
//! BTRUE if during parsing proccess has been an error
BBool BParser::HasError() 
{
  return (DefaultParser()->messageError_.HasName());
}

//--------------------------------------------------
//! Returns \a messageError_ (BText) attribute
const BText& BParser::MessageError() 
{
  return(messageError_);
}

//--------------------------------------------------
//! Returns \a filter_ (BFilter) attribute
BFilter* BParser::Filter() const 
{
  return(filter_);
}

//--------------------------------------------------
//! Returns \a scan_ (BScanner) attribute
BScanner* BParser::Scanner() const 
{
  return(scan_);
}

//--------------------------------------------------
//! Returns nextSymbol_ BToken value extracted from parsed expression
BToken* BParser::NextSymbol() const 
{
  return (nextSymbol_);
}

//--------------------------------------------------
//! Returns nextArgument_ BToken value extracted from parsed expression
BToken* BParser::NextArgument () const 
{ 
  return (nextArgument_);
}

//--------------------------------------------------------------------
//! Puts a duplicated argument in \a nextSymbol_ attribute.
/*! Puts a duplicate of sym in nextSymbol_ field.
 *  If the type of sym is CLOSE, OPEN or NONE the symbol cannot enter
 *  in the tree, so it puts NIL in nextSymbol_ field
 * \param sym Last BToken released in scanner proccess
 */
void BParser::PutNextSymbol (BToken* sym) 
{
  if(sym &&
    (sym->TokenType()!=CLOSE) &&
    (sym->TokenType()!=OPEN) &&
    (sym->TokenType()!=NONE)) 
  {
    nextSymbol_ = sym->Duplicate();
  } 
  else 
  {
    nextSymbol_ = sym;
  }
}

//--------------------------------------------------------------------
//! Puts either NIL or new BToken of type ARGUMENT in \a nextArgument_.
/*! If name is not null make a token of type ARGUMENT puts it in
 *  nextArgument_ field, else puts NIL in nextArgument_ field.
 * \param name Last BText argument released from scanner proccess
 */
void BParser::PutNextArgument (const BText& name) 
{
  if(name.HasName()) 
  {
    nextArgument_ = new BToken(name, ARGUMENT);
  } 
  else 
  {
    nextArgument_ = NULL;
  }
}

//- friend function --------------------------------------------------
//! Returns a formatted text with information of a token for tracing.
/*! Returns a formatted text with information of a token for tracing.
 * \param tok BToken to print
 * \param indent BText with spaces to make an indentation
 * \return a BText with text to be printed
 */
BText tokWrite (const BToken* tok, const BText &indent) 
{
  BText txt;
  txt << indent << INDENT << "[" << TokenTypeName(tok->TokenType()) << "]"
      << BParser::tokOpen(tok) << tok->String()
      << BParser::tokClose(tok) << "\n";
  return (txt);
}

//- static method ---------------------------------------------------
//! Returns the name of the open token of a given token tok.
/*! Returns the name of the open token of a given token tok.
 *  If tok have no open token return NIL.
 */
const BText& BParser::tokOpen(const BToken* tok) 
{
  if(tok->Close()) 
  {
    return(tok->Close()->Open()->Name());
  }
  return(BText::None());
}

//- static method ----------------------------------------------------
//! Returns the name of the close token of a given token tok.
/*! Returns the name of the close token of a given token tok.
 *  If tok have no close token return NIL.
 */
const BChar* BParser::tokClose(const BToken* tok) 
{
  return((tok->Close()) ? tok->Close()->String() : NIL);
}

//--------------------------------------------------------------------
//! Mark end of parsed expression. 2nd parameter always being null
/*! After a close symbol has been scanned, if his name is equal as
 *  "close" and the tree "tre" is already complete ( with or
 *  without "NextArgument()" ) then, if it exist, it puts this
 *  argument at most right branch in the tree, and puts BTRUE in
 *  "complete". Else it prints a message error and returns NIL.
 */
Tree* BParser::ParseNone(Tree* tre, BCloseToken* close) 
{
  if(close) 
  {
    messageError_+= I2("Unexpected end of expression. "
      "Close symbol "+ close->Name()+ " was expected",
      "Fin de la expresión inesperado. "
      "Se esperaba símbolo de cierre " + close->Name()+". ");
  }
  if(NextArgument() && (!lastSymbol_ || (lastSymbol_->Name()!="#Embed")))
  {
    tre->putMostRight(Tree::create(NextArgument(), NIL)); 
  }
  else if(!complete_ && !tre->isEmpty())
  {
    //When macro #Embed is the last line, most right branch must be cleaned
    List* aux1 = tre->getMRFree();
    List* aux2 = tre->getBranch();
    while(aux2->cdr()!=aux1) { aux2 = aux2->cdr(); }
    aux2->setCdr(NIL);
    DESTROY(aux1); 
    tre->setMRFree(NULL);
  }
  else if(!complete_)
  {
    messageError_+= I2("Unexpected end of expression",
                       "Fin de la expresión inesperado")+". ";
  }
  return(tre);
}


//--------------------------------------------------------------------
//! Parse code between red open token and its associated close token
/*! After an open symbol has been scanned, this function parses the
 *  rest of expression_ until the next close symbol that
 *  corresponds to this open symbol if tre is not already complete.
 *  else updates messageError_ and returns NIL.
 * \param tre Syntactical Tree that has been created
 * \returns Synctactical Tree updated
 */
Tree* BParser::ParseOpen (Tree* tre) 
{
  Tree*        branch = new Tree();
  BToken*      arg    = NextArgument();
  BCloseToken* cl     = NextSymbol()->Close();
  openNumber_++;
  if(!classInheritage_ && 
    (complete_ || (arg && !filter_->IsFirstIdentifier(arg->String()[0])))) 
  {
    BText sym = NextSymbol()->Name();
    filter_->UnReplace(sym);
    messageError_+=
      I2("Unexpected open symbol " + sym,
         "Símbolo de apertura "   + sym +
         " fuera de lugar")+". ";
  } 
  else 
  {
    if(classInheritage_)
    {
      complete_=false;
      classInheritage_=false;
    }
    branch = ParseSymbol(branch, cl);
    if(!messageError_.HasName()) 
    {
      if(arg) 
      {
        arg->PutTokenType(FUNCTION);
        const BText& name = arg->Name();
             if(name == "EQ")   { arg->PutName("Eq"); }
        else if(name == "Sqrt") { arg->PutName("SqRt"); }
        else if(name == "Pow")  { arg->PutName("^"); } 
        BToken* tok = ((BToken*) branch->getNode());
        if(branch && tok && (tok->TokenType()==SEPARATOR)) 
        {
          branch->setNode(NIL);
          DESTROY(tok);
          branch->setNode(arg);
        } 
        else 
        {
          branch = Tree::createMonary(arg, branch->getTree());
        }
      }
      if(!branch->isEmpty()) 
      {
        BParser::treToken(branch->getTree())->PutMaxPrecedence();
        tre->putMostRight(branch);
      }
    }
  }
  return(tre);
}


//--------------------------------------------------------------------
//! Red close token and close Tree opened in ParseOpen method
/* After a close symbol has been scanned, if his name is equal as
 * "close" and the the tree "tre" is already complete ( with or
 * without "NextArgument()" ) then, if it exist, it puts this
 * argument at most right branch in the tree, and puts BTRUE in
 * "complete". Else it prints a message error and returns NIL.
 */
Tree* BParser::ParseClose (Tree* tre, BCloseToken* close) 
{
  bool lastWasEmbed = lastSymbol_ && (lastSymbol_->Name()=="#Embed");
  openNumber_--;
  if(!close ||
     (NextSymbol()->Name() != close->Name()) ||
     (!NextArgument() && !lastWasEmbed && !complete_) ) 
  {
    BText closeStr = NextSymbol()->Name();
    filter_->UnReplace(closeStr);
    messageError_+=
      I2("Unexpected close symbol "+closeStr,
         "Símbolo de cierre "+closeStr+" fuera de lugar")+". ";
  }
  if(NextArgument() && !lastWasEmbed)
  {
    tre->putMostRight(Tree::create(NextArgument(), NIL));
  }
  if(tre && tre->getTree()) 
  {
    BParser::treToken(tre->getTree())->PutClose(close);
  }
  complete_ = BTRUE;
  return(tre);
}


//--------------------------------------------------------------------
//! Parse BTypeToken and add it to Syntactical Tree
/*! Add BTypeToken stored in \a nextSymbol_ attribute into 
 *  Syntactical Tree.
 * \param tre Syntactical Tree implemented as BList
 * \return Syntactical tree updated
 */
Tree* BParser::ParseType (Tree* tre) 
{
  if(NextArgument() || complete_) 
  {
    messageError_+= I2("Unexpected type symbol "+ NextSymbol()->Name(),
                       "Símbolo de tipo "+NextSymbol()->Name()+
                       " fuera de lugar")+". ";
  } 
  else 
  {
    NextSymbol()->PutPrecedence(9);
    Tree* branch = Tree::createMonary(NextSymbol(),NIL);
    tre->putMostRight(branch);
    if(classInheritage_)
    {
      complete_=true;
    }
  }
  return(tre);
}

//--------------------------------------------------------------------
//! Parse BMonaryToken and add it to Syntactical Tree
/*! After a monary symbol has been scanned, if the tree "tre" is not
 *  complete and not exist any argument, it puts in the most right
 *  branch of the tree a new monary branch with this symbol and a void
 *  sub-branch.
 *  Else, if there's a binary symbol whith the same name as this
 *  monary, it calls "ParseBinary".
 *  In other case it prints a message error and returns NIL.
 * \param tre Syntactical Tree implemented as BList
 * \return Syntactical tree updated
 */
Tree* BParser::ParseMonary (Tree* tre) 
{
  if(NextSymbol()->Name() == "!") { NextSymbol()->PutName("Not"); }
  const BBinaryToken* bin;
  bin = ((BMonaryToken*)(scan_->NextSymbol()))->Binary();
  if(NextArgument() || complete_) 
  {
    if(bin) 
    {
      NextSymbol()->PutTokenType(BINARY);
      NextSymbol()->PutPrecedence(bin->Precedence());  
      tre = ParseBinary(tre);
    } 
    else 
    {
      messageError_+=
      I2("Unexpected monary symbol "+ NextSymbol()->Name(),
       "Símbolo monario "+NextSymbol()->Name()+" fuera de lugar")+". ";
    }
  } 
  else 
  {
    if(bin) 
    {
      BText name;
      name << " " << NextSymbol()->Name() << " " ;
      NextSymbol()->PutPrecedence(bin->Precedence());
      NextSymbol()->PutName(name);
    } 
    else if (NextSymbol()->Name() == "Class") 
    {
      NextSymbol()->PutPrecedence(1);
    }
    else
    {
      NextSymbol()->PutPrecedence(20);
    }
    Tree* branch = Tree::createMonary(NextSymbol(),NIL);
    tre->putMostRight(branch);
  }
  return(tre);
}

//--------------------------------------------------------------------
//! Parse BBinaryToken and add it to Syntactical Tree
/*! After a binary symbol has been scanned, if the tree "tre" is not
 *  complete or there isn't any argument, it puts a new branch with
 *  this symbol at the most right branch of the tree that has less
 *  or equal precedence than the binary symbol. If th
 */
Tree* BParser::ParseBinary (Tree* tre) 
{
  bool lastIsClass = false;
  bool isInherite = false;
  if(NextSymbol()->Name() == "**") { NextSymbol()->PutName("^"); }
  else 
  {
    lastIsClass = (
      lastSymbol_ && 
      (lastSymbol_->TokenType()==MONARY) &&
      (lastSymbol_->Name()=="Class")
    )||(
      lastSymbol_2_ && 
      (lastSymbol_2_->TokenType()==MONARY) &&
      (lastSymbol_2_->Name()=="Class") );
    
    isInherite =  lastIsClass && (NextSymbol()->Name()==":");
    if(isInherite && lastIsClass)
    {
      classInheritage_ = true;
      NextSymbol()->PutPrecedence(3);
    }
  }
  if(!NextArgument() && !complete_ && !isInherite) 
  {
    messageError_+=
      I2("Unexpected binary symbol "+ NextSymbol()->Name(),
         "Símbolo binario "+NextSymbol()->Name()+" fuera de lugar")+". ";
  } 
  else 
  {
    if (tre->isEmpty()) 
    {
      if(!NextArgument()) 
      {
        messageError_+=
          I2("Argument was expected for binary operator ",
             "Se esperaba argumento para el operador binario ") +
          NextSymbol()->Name()+". ";
      } 
      else 
      {
        tre = Tree::createBinary(NextSymbol(),
                  (Tree::create(NextArgument(), NIL))->getTree(), NIL);
      }
    } 
    else 
    {
      BToken* tok = BParser::treToken(tre->getTree());
      int p1 = tok->Precedence();
      int p2 = NextSymbol()->Precedence();
      if(p1 >= p2) 
      {
        if(NextArgument()) 
        {
          tre->putMostRight(Tree::create(NextArgument(), NIL));
        }
        tre = Tree::createBinary(NextSymbol(), tre->getTree(), NIL);
      } 
      else 
      {
        List* rightCdr = tre->getMostRightBranch();
        List* right = ((List*) rightCdr->car());
        if (!right) 
        {
          Tree* branch;
          branch = Tree::createBinary(NextSymbol(),
            (Tree::create(NextArgument(),NIL)->getTree()), NIL);
          tre->putMostRight(branch);
        } 
        else 
        {
          rightCdr->setCar(NIL);
          Tree* tAux = new Tree();
          tAux->setTree(right);
          tAux->setMRFree(tre->getMRFree());
          tre->setMRFree(rightCdr);
          tre->putMostRight(ParseBinary(tAux));
        }
      }
    }
  }
  complete_=BFALSE;
  return(tre);
}


//--------------------------------------------------------------------
//! Parse BSeparatorToken
/*! After a separator symbol has been scanned, if it is compatible
 *  with "close" calls to "ParseBinary". Else it prints a message
 *  error and returns NIL.
 */
Tree* BParser::ParseSeparator (Tree* tre) 
{
  bool lastWasEmbed = lastSymbol_ && (lastSymbol_->Name()=="#Embed");
  if(!NextArgument() && !complete_ && !lastWasEmbed) 
  {
    messageError_+=
      I2("Unexpected separator symbol "+ NextSymbol()->Name(),
         "Símbolo separador "+NextSymbol()->Name()+" fuera de lugar")+". ";
  } 
  else 
  {
    if(NextSymbol()->Name()==";")
    { 
      classInheritage_=false; 
    }
    else if(classInheritage_ && (NextSymbol()->Name()==","))
    {
      complete_ = false;
      return(tre);
    }
    if(tre->isEmpty()) 
    {
      tre = Tree::createBinary(NextSymbol(),
              (Tree::create(NextArgument(),NIL))->getTree(),
             NIL);
    } 
    else 
    {
      if(NextArgument() && !lastWasEmbed)
      {
        tre->putMostRight(Tree::create(NextArgument(),NIL));
      }
      if (!BParser::treToken(tre->getTree())->Close() && 
          (BParser::treToken(tre->getTree())->TokenType()==SEPARATOR)) 
      {
        List* l = tre->getBranch();
        if(l) 
        {
          while(l->cdr() && l->car()) 
          {
            l = l->cdr();
          }
          Tree* tAux = Tree::create(NIL,NIL);
          l->setCdr(tAux->getTree());
          tre->setMRFree(tAux->getTree());
        }
      } 
      else 
      {
        tre = Tree::createBinary(NextSymbol(),tre->getTree(),NIL);
      }
    }
  }
  complete_=BFALSE;
  return(tre);
}

//--------------------------------------------------------------------
Tree* BParser::ParseMacroEmbed (Tree* tre) 
//--------------------------------------------------------------------
{
  BText& pathArg = scan_->NextArgument();
  Tree* embedded = NULL;
  if(pathArg.HasName() && (pathArg[0]=='\"'))
  {
    BText path(pathArg,1,pathArg.Length()-1);
    BText oldDir = BDir::GetCurrent();
    ifstream in(path, OPENFLAG);
    if(!in.good())
    {
      Error(I2("Cannot open TOL file to be embedded ",
               "No se puedo abrir para incrustar el fichero TOL ")+path+
               I2(" from current path ", " desde el directorio en curso ")+
               oldDir+ ". ");
    }
    else
    {
      BText fileContens;
      int size = GetFileSize(path);
      fileContens.ReallocateBuffer(size);
      Read(in,fileContens,size, '\0');
      BText currentDir = GetFilePath(path);
      BDir::ChangeCurrent(currentDir);
      BParser* childParser = new BParser;
      embedded = childParser->Parse(fileContens);
      delete childParser;
      BDir::ChangeCurrent(oldDir);
    }
  }
  if(!embedded)
  {
    messageError_+=
      I2("Expected valid file path argument for #Embed macro instead of ",
      "Se esperaba un nombre de fiechero válido para la macro #Embed en "
      "lugar de")+ pathArg+" . ";
  } 
  else if(tre->isEmpty() && (!embedded->isEmpty()))
  {
    delete tre;
    tre = embedded; 
    if(!complete_)
    {
      List* aux1 = tre->getBranch();
      if(aux1)
      {
        List* mostRight = new List;
        while(aux1->cdr()!=NULL) { aux1 = aux1->cdr(); }
        aux1->setCdr(mostRight);
        tre->setMRFree(mostRight);
//      complete_ = true;
      }
    }
  }
  else
  {
    if(!complete_ && 
       (!lastSymbol_2_ || (lastSymbol_2_->TokenType()!=SEPARATOR)))
    {
      messageError_+= I2("Unexpected macro ", "Macro inesperada ")+
                      "#Embed "+pathArg+" . ";
    }
    else if(!embedded->isEmpty())
    {
      BToken* tok = BParser::treToken(embedded->getTree());
      if(tok->TokenType()!=SEPARATOR)
      {
        List* aux2 = List::cons(embedded->getTree(),NIL);
        List* mostRight = tre->getMRFree();
        List* aux1 = tre->getBranch();
        while(aux1->cdr()!=mostRight) { aux1 = aux1->cdr(); }
        aux1->setCdr(aux2);
        while(aux1 && aux1->cdr()) { aux1 = aux1->cdr(); }
        if(aux1) { aux1->setCdr(mostRight); }
      }
      else
      {
        List* aux2 = embedded->getBranch();
        List* mostRight = tre->getMRFree();
        List* aux1 = tre->getBranch();
        while(aux1->cdr()!=mostRight) { aux1 = aux1->cdr(); }
        aux1->setCdr(aux2);
        while(aux1 && aux1->cdr()) { aux1 = aux1->cdr(); }
        if(aux1) { aux1->setCdr(mostRight); }
        delete embedded->getNode();
//      complete_ = true;
      }
    }
  }
  return(tre);
}

//--------------------------------------------------------------------
Tree* BParser::ParseMacro (Tree* tre) 
//--------------------------------------------------------------------
{
  const BText& name = NextSymbol()->Name();
  if(name=="#Embed") 
  { 
    return(ParseMacroEmbed(tre)); 
  } 
  else 
  {
    messageError_+=
      I2("Unknown parsing macro ",
       "Macro de parseo desconocida ")+ name+" . ";
  } 
}
//--------------------------------------------------------------------
//! Takes next symbol from scanner & invokes parser method related with it
/*! Takes the next symbol and argument from scanner and calls the
 *  parser function correspondent with the type of the found symbol.
 *  \nIf token read is a "struct" then add it to symbolTable_ in
 *  order to agregate this as a supported type of TOL.
 * \param tre BTree
 * \param close Close token associated or NIL at first called
 * \return BList with syntactic tree
 */
Tree* BParser::ParseSymbol (Tree* tre, BCloseToken* close) 
{
  bool needsRead = true;
  BBool ok;
  BTokenType symbolType, newSymbolType;
  const BText* name = NULL;
  do 
  {
    if(needsRead)
    {
      ok = ReadNextSymbol(symbolType); 
    }
      else
    { 
      symbolType = newSymbolType;
    }
    if(NextSymbol())
    {
      name = &NextSymbol()->Name(); 
    }
    needsRead = true;
    switch (symbolType) 
    {
      case NONE      : tre=ParseNone     (tre, close); break;
      case OPEN      : tre=ParseOpen     (tre);        break;
      case CLOSE     : tre=ParseClose    (tre, close); break;
      case SEPARATOR : tre=ParseSeparator(tre);        break;
      case TYPE      : tre=ParseType     (tre);        break;
      case MONARY    : tre=ParseMonary   (tre);        break;
      case BINARY    : tre=ParseBinary   (tre);        break;
      case MACRO     : 
        if(*name=="#Embed") 
        { 
          ok = ReadNextSymbol(newSymbolType);
          tre=ParseMacroEmbed(tre); 
          if(newSymbolType == SEPARATOR) 
          { 
          //PutNextArgument(NULL);
            needsRead = true;
          } 
          else if(newSymbolType == CLOSE)
          { 
            needsRead = false; /*
            needsRead = true;
            if(tre && tre->getTree()) 
            {
              BParser::treToken(tre->getTree())->PutClose(close);
            }
            close     = NULL;
            openNumber_--;
            complete_ = true; */
          }
          else if(newSymbolType == NONE)
          { 
            needsRead = true;
          //complete_ = true;
          }
          else
          {
            messageError_+=
              I2("Unexpected symbol ",
                 "Símbolo fuera de lugar")+NextSymbol()->Name()+"\n"+
              I2("A separator, close symbol or end of file was expected "
                 "after macro #Embed ",
                 "Se esperaba un separador, un símbolo de cierre o el final "
                 "del archivo tras la macro #Embed");
          } 
        } 
        else 
        {
          messageError_+=
            I2("Unknown parsing macro ",
             "Macro de parseo desconocida ")+ *name+" . ";
        } 
      break;
      default        : 
      {
        messageError_+= I2("Unexpected symbol ",
                           "Símbolo fuera de lugar")+". ";

      }
    }
  } while(!messageError_.HasName() &&
          (symbolType!=CLOSE) &&
          (symbolType!=NONE));
  return(tre);
}

//--------------------------------------------------------------------
BBool BParser::ReadNextSymbol(BTokenType& symbolType) 
//--------------------------------------------------------------------
{
  BBool ok = BTRUE;
  BBool structFound = BFALSE;
  BBool classFound  = BFALSE;
  lastSymbol_2_ = lastSymbol_;
  lastSymbol_   = nextSymbol_;
  if(lastSymbol_ && (lastSymbol_->TokenType()==MONARY) ) 
  {
    const BText& sym = lastSymbol_->Name();
    structFound = (sym=="Struct");
    classFound  = (sym=="Class");
  }
//  if(scan_->NextSymbol())
/* * /
  if(lastSymbol_)
  {
    Std(BText("\n  Parser[")+(int)this+"] lastSymbol_ <- '"+
      lastSymbol_->Name()+"'");
  }
  else
  {
    Std(BText("\n  Parser[")+(int)this+"] lastSymbol_ <- ''");
  }
/* */
  symbolType = scan_->ReadNextSymbol();
/* * /

  Std(BText("\n  Parser[")+(int)this+"] symbolType <- '"+TokenTypeName(symbolType));
  if(scan_->NextSymbol())
  {
    Std(BText("\n  Parser[")+(int)this+"] symbol <- '"+
      scan_->NextSymbol()->Name()+"'"
      " argument <- '"+
      scan_->NextArgument()+"'"+ 
      " complete = "+(int)complete_);
  }
  else
  {
    Std(BText("\n  Parser[")+(int)this+"] symbol <- ''"
      " argument <- '"+
      scan_->NextArgument()+"'"+ 
      " complete = "+(int)complete_);
  }
/* */
  if((symbolType==ARGUMENT) ||
     ((scan_->NextArgument()!="") && complete_)) 
  {
    BText arg = scan_->NextArgument();
    filter_->UnReplace(arg);
    messageError_+= I2("Unexpected argument "+ arg,
                       "Argumento " + arg + " fuera de lugar")+". ";
    PutNextSymbol  (scan_->NextSymbol());
    PutNextArgument(scan_->NextArgument());
    ok = BFALSE;
  } 
  else 
  {
    PutNextSymbol  (scan_->NextSymbol());
    PutNextArgument(scan_->NextArgument());
    if(structFound) 
    {
      if(!scan_->FindSymbol(scan_->NextArgument()))
      {
        BObject* arg = new BObject(scan_->NextArgument());
        scan_->AddSymbol(new BTypeToken(arg->Name(),BTypeToken::BSTRUCT));
        newSymbol_ = List::cons(arg,newSymbol_);
      }
    }
    else if(classFound) 
    {
      if(!scan_->FindSymbol(scan_->NextArgument()))
      {
        BObject* arg = new BObject(scan_->NextArgument());
        scan_->AddSymbol(new BTypeToken(arg->Name(),BTypeToken::BCLASS));
        newSymbol_ = List::cons(arg,newSymbol_);
      }
    }
  }
  return(ok);
}

//--------------------------------------------------------------------
//! Puts in row and col the coordinates of current scanned text
/*! Puts in row and col the coordinates of current scanned text.
 */
#define LINES_AROUND_ERROR 10
BText BParser::ErrorPosition (BInt& row, BInt& col)
{
  static const int margin  = LINES_AROUND_ERROR;
  static const int margin2 = 2*margin+1;
  
  const BChar* exp = expression_.String();
  BText scanned = scan_->Scanned();
  filter_->UnReplace(scanned);
  const BChar* scn = scanned.String();
  int nextWord = 0;
  int nChar = 0;
  char next[65536];

  col = 0;
  row = 0;

  while((nextWord>=0) && (nextWord<expression_.Length()) &&
        scn && scn[0] && (sscanf(scn, "%s%n", next, &nChar))) 
  {
    if(next[0]) 
    {
      BInt nextLen = strlen(next);
      do 
      {
        nextWord = expression_.Find(next, nextWord);
      } while(InAComment(expression_, nextWord));
      if((nextWord>=0)) 
      {
        nextWord+=nextLen;
        for(; exp < expression_.String()+nextWord; exp++) 
        {
          if(exp[0]=='\n') { row++; col = 0; } 
          else             { col++; }
        }
        scn += nChar;
        if(scn[0]) { scn++; } 
      } 
      else 
      {
        nextWord=-1;
      }
    }
  }
  row++;
  BInt pos   = nextWord;
  BInt pos1  = pos;
  BInt pos2  = exp - expression_.String();
  BInt first = row;
  BInt i;

  // pos1 is the position at margin lines ABOVE the line with the error (row)
  for(i=0; (pos1>0)&&(i<margin); i++) 
  {
    while((pos1>0) && (expression_.Get(pos1)!='\n')) { pos1--; }
    if(pos1 && ((i+1)<margin)) 
    { 
      // do not get another char from the code
      // because the next iteration of i is the last
      pos1--; 
      first--;         
    }
  }
  if(!pos1) 
  {
    first = 1;
  } 
  else if (expression_.Get(pos1)=='\n') 
  {
    pos1++;
  }
  // pos2 is the position at margin lines BELOW the line with the error (row)
  for(i=0; (pos2<expression_.Length())&&(i<margin); i++) 
  {
    while (pos2<expression_.Length() && 
          (expression_.Get(pos2)!='\n')) 
    {
      pos2++;
    }
    if((pos2<expression_.Length()) && ((i+1)<margin)) 
    {
      pos2++;
    }
  }
  BText sub = expression_.SubString(pos1, pos2);
  BArray<BText> lines;
  ReadAllTokens(sub, lines, '\n');
  BText txt;
  BChar numLine[20];
  if(lines.Size()>1) 
  {
    txt += I2("\nCode line ","\nLínea de código ");
  }
  i=0;
  if(lines.Size()>margin2) {
    first-=lines.Size()-margin2; i++;
  }
  for(; i<lines.Size(); i++) 
  {
    if(lines.Size()>1) 
    {
      sprintf(numLine,"\n%4d",(i+first));
      txt += numLine;
    }
    if((first+i==row)||(lines.Size()==1)) 
    {
      txt += " > ";
      txt += lines[i] + "\n";
      if(lines.Size()>1) { txt += "    "; }
      txt += BText(col,' ')+"  ^";
    } 
    else 
    {
      txt += " : ";
      txt += lines[i];
    }
  }
  return(txt);
}


//--------------------------------------------------------------------
/*! Returns the index of the next occurrence of \a txt in the actual
 *  text from the position \a from and skipping all comments. 
 *  Returns -1 if there are no occurrences.
 * \param txt  Text to be found
 * \param from Position to find from
 */
bool BParser::InAComment(BText& txt, int& curIndex) const
{
  static int begNextComment = 0;
  static int endNextComment = 0;
  int pos = curIndex;
  int commentType; // 1->one_line_comment, 2->multiple_line_comment
  char* pCode = txt.Buffer();

  // 1ST: verify if curIndex is between beg and endNextComment
  if((begNextComment < curIndex) && (curIndex < endNextComment)) 
  {
    curIndex++;
    return true;
  } 
  else if(begNextComment > curIndex) 
  {
    return false;
  }
  /* 2ND: if curIndex > endNextComment, calculate where starts
   * and ends the next comment
   */
  while((begNextComment<curIndex) && (endNextComment<curIndex)) 
  {
    pos = endNextComment;
    commentType = 0;
    // search a start comment token
    while(((pos+2)<txt.Length()) && !commentType) 
    {
      pos++;
      if((pCode[pos]=='/') && (pCode[pos+1]=='/')) 
      { 
        commentType = 1;
        begNextComment = pos;
      } 
      else if((pCode[pos]=='/') && (pCode[pos+1]=='*')) 
      {
        commentType = 2;
        begNextComment = pos;
      }
    }
    // if no more comments in the code, mark static values and returns
    if( ((pos+2) >= txt.Length()) && !commentType) 
    {
      begNextComment = txt.Length();
      endNextComment = txt.Length();
      return false;
    }
    // commentType > 0; search the end of the found comment
    while( ( (pos+1) < txt.Length() ) && (endNextComment<pos) ) 
    {
      if(((commentType==2) && (pCode[pos]!='*') && (pCode[pos+1]!='/'))
         || ((commentType==1) && (pCode[pos]!='\n'))) 
      {
        pos++;
      } 
      else 
      {
        endNextComment = pos;
      }
    }
    if( ((pos+1)==txt.Length()) && (endNextComment<pos))  
    {
      endNextComment = pos;
    }
  }
  // 3TH: verify again the 1ST condition to see what is curIndex position
  if((begNextComment < curIndex) && (curIndex < endNextComment)) 
  {
    curIndex++;
    return true;
  } 
  else if(begNextComment > curIndex) 
  {
    return false;
  }
  return false;
}
