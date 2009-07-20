/* txtalgeb.cpp: Algebraic functions
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

//#include <config.h>
#include <tol/tol_btext.h>
#include <tol/tol_btimer.h>
#include <tol/tol_barray.h>
//#include <tol/tol_bfilter.h> no parece que haga falta aqui

#include <ctype.h>
#include <assert.h>

BTraceInit("txtalgeb.cpp");

#define BMEDIUM_SIZE_STRING 1024
#define BLONG_SIZE_STRING 65536

//--------------------------------------------------------------------
BInt BTextOrderCriterium(const BAny txt1, const BAny txt2)

/*! Comparision function to sort by name in arrays of BText
 * \param txt1 First object to compare (it's casted to BText)
 * \param txt2 Second object to compare (it's casted to BText)
 */
//--------------------------------------------------------------------
{
  BText& sym1 = *((BText*)txt1);
  BText& sym2 = *((BText*)txt2);
  BInt cmp = StrCmp(sym1, sym2);
  return(cmp);
}


//--------------------------------------------------------------------
void BText::PutChar (BInt i, BChar ch)

/*! Puts a char \a ch in the \a i-th position of buffer if the char
 *  is not null and \a i is less or equal than sizeof buffer.
 * \param i  position of buffer
 * \param ch character to put in the buffer
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(ch && (i<size_) && (i>=0))
  {
    buffer_[i]=ch;
  }
  else
  {
    assert(0);
  }
}


//--------------------------------------------------------------------
BChar* BText::DupBuffer() const

/*! Returns a duplicate of the buffer.
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BChar* dup = NULL;
  if(buffer_)
  {
    dup = new BChar[length_+1];
    memcpy(dup, buffer_, length_);
    dup[length_]='\0';
  }
  return(dup);
}


//--------------------------------------------------------------------
BText& BText::Copy(const BText& txt)

/*! Copies the contents of the BText object \a txt to the actual 
 *  text.
 * \param txt BText object to be copied
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Copy BChar*");
  if(this!=&txt)
  {
    if(!buffer_) { AllocateBuffer(0); } 
    else if(length_) { buffer_[length_=0]='\0'; }
    BInt len = txt.Length();
    if(len>0)
    {
      ReallocateBuffer(len+1);
      memcpy(buffer_, txt.String(), len); 
      buffer_[length_=len]='\0';
    }
  }
  assert(IsAllOk());
  //SumPartialTime;
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(const BChar* str)

/*! Copies the contents of the string \a str to the actual text.
 * \param str String to be copied
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Copy BChar*");
  assert(IsAllOk());
  if(buffer_!=str)
  {
    if(!buffer_) { AllocateBuffer(0); } 
    else if(length_) { buffer_[length_=0]='\0'; }
    if(str && str[0])
    {
      BInt len = strlen(str);
      assert(len>0);
      ReallocateBuffer(len+1);
      memcpy(buffer_, str, len);
      buffer_[length_=len]='\0';
    }
  }
  assert(IsAllOk());
//SumPartialTime;
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(const BChar* str, BInt length)

/*! Copies the first \a length chars of the string \a str to the 
 *  actual text.
 * \param str String to be copied
 * \param length Number of chars of the string to be copied
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Copy BChar* BInt");
  assert(IsAllOk());
  if(buffer_!=str)
  {
    if(!buffer_) { AllocateBuffer(0); } 
    else if(length_) { buffer_[length_=0]='\0'; }
    if(str && str[0])
    {
      ReallocateBuffer(length+1);
      memcpy(buffer_,str,length);
      buffer_[length_=length]='\0';;
    }
}
//SumPartialTime;
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(const BChar* str, BInt from, BInt until)

/*! Copies the contents of the substring [\a from, \a until] of the 
 *  string \a str to the actual text.
 * \param str String to be copied
 * \param from Starting position of string to be copied
 * \param until Ending position of string to be copied
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Copy BChar* BInt BInt");
  assert(IsAllOk());
  if(buffer_!=str)
  {
    if(!buffer_) { AllocateBuffer(0); } 
    else if(length_) { buffer_[length_=0]='\0'; }
    if(str && str[from])
    {
      BInt len = strlen(str);
      if((from<len)&&(from<=until))
      {
        if(from <0)    { from  = 0; }
        if(until<0)    { until = 0; }
        if(until>=len) { until = len-1; }
        len = until - from + 1;
        ReallocateBuffer(len+1);
        memcpy(buffer_, str+from, len);
        buffer_[length_=len]='\0';
      }
    }
  }
//SumPartialTime;
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(const BText& txt, BInt from, BInt until)

/*! Copies the contents of the substring [\a from, \a until] of the
 *  BText objetc \a txt to the actual text.
 * \param txt BText object to be copied
 * \param from Starting position of BText object to be copied
 * \param until Ending position of BText object to be copied
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(this!=&txt)
  {
    if(!buffer_) { AllocateBuffer(0); } 
    else if(length_) { buffer_[length_=0]='\0'; }
    BInt len = txt.Length();
    if((from<len)&&(from<=until))
    {
      if(from <0)    { from  = 0;      }
      if(until<0)    { until = 0;      }
      if(until>=len) { until = len-1;  }
      len = until - from + 1;
      ReallocateBuffer(len+1);
      memcpy(buffer_, txt.String()+from, len);
      buffer_[length_=len]='\0';
    }
  }
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(BInt number, const BChar* format)

/*! Copies the integer \a number with a given format \a format to
 *  the actual text.
 * \param number Integer to be copied
 * \param format Format of number to be copied
 */
//--------------------------------------------------------------------
{
  static char txt [BMEDIUM_SIZE_STRING];
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(0); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  char* fmt=(char*)format;
  if(!format)
  {
    fmt = (char*)formatBInt_;
  }
  BInt len = sprintf(txt, fmt, number);
  assert(len>0);
  ReallocateBuffer(len+1);
  memcpy(buffer_,txt,len);
  buffer_[length_=len]='\0';
  assert(IsAllOk());
  return(*this);
}

//--------------------------------------------------------------------
BText& BText::Copy(long int number, const BChar* format)

/*! Copies the integer \a number with a given format \a format to
 *  the actual text.
 * \param number long int to be copied
 * \param format Format of number to be copied
 */
//--------------------------------------------------------------------
{
  static char txt [BMEDIUM_SIZE_STRING];
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(0); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  char* fmt=(char*)format;
  if(!format)
  {
    fmt = (char*)formatBInt_;
  }
  BInt len = sprintf(txt, fmt, number);
  assert(len>0);
  ReallocateBuffer(len+1);
  memcpy(buffer_,txt,len);
  buffer_[length_=len]='\0';
  assert(IsAllOk());
  return(*this);
}

//--------------------------------------------------------------------
BText& BText::Copy(BINT64 number, const BChar* format)

/*! Copies the integer \a number with a given format \a format to
 *  the actual text.
 * \param number Integer to be copied
 * \param format Format of number to be copied
 */
//--------------------------------------------------------------------
{
  static char txt [BMEDIUM_SIZE_STRING];
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(0); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  char* fmt=(char*)format;
  if(!format)
  {
    fmt = (char*)formatBInt64_;
  }
  BInt len = sprintf(txt, fmt, number);
  assert(len>0);
  ReallocateBuffer(len+1);
  memcpy(buffer_,txt,len);
  buffer_[length_=len]='\0';
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(BReal number, const BChar* format)

/*! Copies the real \a number with a given format \a format to the
 *  actual text.
 * \param number Real to be copied
 * \param format Format of number to be copied
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Copy BReal");
  static char txt [BMEDIUM_SIZE_STRING];
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(0); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  char* fmt=(char*)format;
  if(!format)
  {
    fmt = (char*)formatBReal_;
  }
  BInt len = sprintf(txt, fmt, number);
  assert(len>0);
  ReallocateBuffer(len+1);
  memcpy(buffer_,txt,len);
  buffer_[length_=len]='\0';
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(long double number, const BChar* format)

/*! Copies the long \a number with a given format \a format to the
 *  actual text.
 * \param number Double to be copied
 * \param format Format of number to be copied
 */
//--------------------------------------------------------------------
{
  static char txt [BMEDIUM_SIZE_STRING];
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(0); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  char* fmt=(char*)format;
  if(!format)
  {
    //VBR: ¿es la misma cadena de formato para linux y windows?
    fmt = (char*)formatBReal_;
  }
  BInt len = sprintf(txt, fmt, number);
  assert(len>0);
  ReallocateBuffer(len+1);
  memcpy(buffer_,txt,len);
  buffer_[length_=len]='\0';
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Copy(BChar ch)

/*! Copies the character \a ch to the actual text.
 * \param ch Character to be copied
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!buffer_) { AllocateBuffer(2); } 
  else if(length_) { buffer_[length_=0]='\0'; }
  ReallocateBuffer(2);
  buffer_[0]=ch;
  buffer_[length_=1]='\0';
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(const BChar* str)

/*! Concatenates the contents of the string \a str to the actual
 *  text.
 * \param str String to be concatenated
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Concat BChar*");
  if(buffer_!=str)
  {
    if(str && str[0])
    {
      if(!length_) { Copy(str); }
      else
      {
        BInt strLen = strlen(str);
        BInt newLen = length_+strLen;
        ReallocateBuffer(newLen+1);
        memcpy(buffer_+length_, str, strLen);
        buffer_[length_=newLen]='\0';
      }
    }
  }
//SumPartialTime;
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(const BText& txt)

/*! Concatenates the contents of the BText object \a txt to the
 *  actual text.
 * \param txt BText object to be concatenated
 */
//--------------------------------------------------------------------
{
//InitTotalTime("BText::Concat BText");
  if(this!=&txt)
  {
    if(txt.HasName())
    {
      if(!length_) { Copy(txt); }
      else
      {
        BInt strLen = txt.Length();
        BInt newLen = length_+strLen;
        ReallocateBuffer(newLen+1);
        memcpy(buffer_+length_, txt.String(), strLen);
        buffer_[length_=newLen]='\0';
      }
    }
  //SumPartialTime;
    assert(IsAllOk());
  }
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(BInt number,  const BChar* format)
       
/*! Concatenates the integer number \a number with a given format \a
 *  format to the actual text.
 * \param number Integer to be concatenated
 * \param format Format of number to be concatenated
 */
//--------------------------------------------------------------------
{
  BText txt;
  txt.Copy(number, format);
  Concat(txt);
  return(*this);
}

//--------------------------------------------------------------------
BText& BText::Concat(long int number,  const BChar* format)
       
/*! Concatenates the integer number \a number with a given format \a
 *  format to the actual text.
 * \param number Integer to be concatenated
 * \param format Format of number to be concatenated
 */
//--------------------------------------------------------------------
{
  BText txt;
  txt.Copy(number, format);
  Concat(txt);
  return(*this);
}

//--------------------------------------------------------------------
BText& BText::Concat(BINT64 number,  const BChar* format)
       
/*! Concatenates the integer number \a number with a given format \a
 *  format to the actual text.
 * \param number Integer to be concatenated
 * \param format Format of number to be concatenated
 */
//--------------------------------------------------------------------
{
  BText txt;
  txt.Copy(number, format);
  Concat(txt);
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(BReal number,  const BChar* format)

/*! Concatenates the real number \a number with a given format \a
 *  format to the actual text.
 * \param number Real to be concatenated
 * \param format Format of number to be concatenated
 */
//--------------------------------------------------------------------
{
  BText txt;
  txt.Copy(number, format);
  Concat(txt);
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(long double number,  const BChar* format)

/*! Concatenates the long number \a number with a given format \a
 *  format to the actual text.
 * \param number Long double number to be concatenated
 * \param format Format of number to be concatenated
 */
//--------------------------------------------------------------------
{
  BText txt;
  txt.Copy(number, format);
  Concat(txt);
  return(*this);
}


//--------------------------------------------------------------------
BText& BText::Concat(BChar ch)

/*! Concatenates the character \a ch  to the actual text.
 * \param ch Character to be concatenated
 */
//--------------------------------------------------------------------
{
  BInt pos = length_;
  ReallocateBuffer(length_+2);
  buffer_[pos] = ch;
  buffer_[length_=pos+1]='\0';
  assert(IsAllOk());
  return(*this);
}


//--------------------------------------------------------------------
BText BText::operator+(const BText& txt) const

/*! Concatenates \a txt to the actual text in a new BText.
 */
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += txt);
}


//--------------------------------------------------------------------
BText BText::operator+(const BChar* str) const

/*! Concatenates \a str to the actual text in a new BText.
 */
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += str);
}


//--------------------------------------------------------------------
BText BText::operator+(BInt val) const

/*! Concatenates \a val to the actual text in a new BText.
*/
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += val);
}

//--------------------------------------------------------------------
BText BText::operator+(long int val) const

/*! Concatenates \a val to the actual text in a new BText.
*/
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += val);
}

//--------------------------------------------------------------------
BText BText::operator+(BINT64 val) const

/*! Concatenates \a val to the actual text in a new BText.
*/
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += val);
}


//--------------------------------------------------------------------
BText BText::operator+(BReal val) const

/*! Concatenates \a val to the actual text in a new BText.
 */
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += val);
}


//--------------------------------------------------------------------
BText BText::operator+(long double val) const

/*! Concatenates \a val to the actual text in a new BText.
*/
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += val);
}


//--------------------------------------------------------------------
BText BText::operator+(BChar  ch ) const

/*! Concatenates \a ch to the actual text in a new BText.
*/
//--------------------------------------------------------------------
{
  BText t = (*this);
  return(t += ch);
}


//--------------------------------------------------------------------
BText BText::SubString(BInt from, BInt until) const

/*! Returns a copy of the contents of the substring [\a from, \a 
 *  until] of this text.
 * \param from Starting position of text
 * \param until Ending position of text
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(HasName())
  {
    if(from>=length_)  { from  = length_; }
    if(until>=length_) { until = length_; }
    BText txt(*this, from, until);
    return(txt);
  }
  else
  {
    BText txt;
    return(txt);
  }
}

//--------------------------------------------------------------------
BInt BText::Find(const BText& txt, BInt from) const

/*! Returns the index of the next occurrence of \a txt in the actual
 *  text from the position \a from. Returns -1 if there are no
 *  occurrences.
 * \param txt  Text to be found
 * \param from Position to find from
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!HasName()) { return(-1); }
  if(txt.Length()==1) { return(Find(txt(0), from)); }
  BInt indexText=-1;
  if(HasName() && txt.HasName() && (Length()>=from+txt.Length()))
  {
    const BChar* beginText;
    if(beginText = strstr(String() + from, txt.String()))
    {
      indexText = (beginText - String());
      if(indexText < 0) { indexText = -1; }
    }
  }
  return(indexText);
}

//--------------------------------------------------------------------
BInt BText::FindAdv(const BText& txt, BInt from, BInt ocurrence, 
          BInt direction) const

/*! Returns the index of the next occurrence of \a txt in the actual
 *  text from the position \a from. Returns -1 if there are no
 *  occurrences.
 * \param txt  Text to be found
 * \param from Position to find from
 * \param ocurence Number the ocurrence to find. Default 1
 * \param direction Direction the find text. Default(1) left -> rigth
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!HasName()) { return(-1); }
  if ((txt.Length()==1) && (ocurrence==1) && (direction==1)) 
  { 
    return(Find(txt(0), from)); 
  };
  BInt indexText=0;
  BText txt2 = txt;
  if (direction==-1) 
  {
    from = Length() - from - 1;
  }
  if((HasName()) && (txt.HasName()) && 
   (Length()>=(from+txt.Length()+ocurrence-1)))
  {
    BText txtAux = *this;
    if (direction==-1) 
    {
      txtAux.Reverse();
      txt2.Reverse();
    }
    while ((ocurrence>=1) && (indexText!=-1))
    {
      indexText = txtAux.Find(txt2, from);
      from = indexText+1;
      ocurrence --;
    };
    if (ocurrence<0) {indexText=-1;};
  } 
  else 
  {
    indexText=-1; 
  }
  if ((direction==-1) && (indexText>=0)) 
  {
    indexText = (Length() - indexText - txt2.Length());   
  }
  return(indexText);
}


//--------------------------------------------------------------------
BInt BText::Find(BChar ch, BInt from) const

/*! Returns the index of the next occurrence of the char \a ch in
 *  the actual text from the position \a from. Returns -1 if there
 *  are no occurrences.
 * \param ch Character to be found
 * \param from Position to find from
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!HasName()) { return(-1); }
  BInt indexChar=-1;
  if(ch && from>=0 && from<length_)
  {
    const char * pos = strchr(String()+from, ch);
    if (pos != NULL)
    {
      indexChar = pos - String();
    }
  };
  return(indexChar);
}


//--------------------------------------------------------------------
BInt BText::FindLast(BChar ch, BInt from) const

/*! Returns the index of the last occurrence of the char \a ch in the
 *  actual text from  the position \a from. Returns -1 if there are
 *  no occurrences.
 * \param ch Character to be found
 * \param from Position to find from
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!HasName()) { return(-1); }
  BInt indexChar=-1;
  if(ch && from>=0 && from<length_)
  {
    const char * pos = strrchr(String()+from, ch);
    if (pos != NULL)
    {
      indexChar = pos - String();
    }
  };
  return(indexChar);
}


//--------------------------------------------------------------------
BInt BText::Replace(BChar oldChar, BChar newChar)
       
/*! Replaces all \a oldChar occurrences by \a newChar in the actual
 *  text.
 * \param oldChar Character to be replaced
 * \param newChar Character to replace with
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!HasName()) { return(0); }
  if(oldChar==newChar) { return(0); }
  BInt number = 0;
  number = 0;
  BChar* pos=Buffer();
  while(pos=strchr(pos, oldChar)) 
  { 
    *pos = newChar; 
    number++; 
  }
  return(number);
}


//--------------------------------------------------------------------
BText Replace(const BText& txt, BChar oldChar, BChar newChar)

/*! Replaces all \a oldChar occurrences by \a newChar in a given text
 *  \a txt.
 * \param txt Text where is done the replacement
 * \param oldChar Character to be replaced
 * \param newChar Character to replace with
 * \sa Replace (BChar, BChar)
 */
//--------------------------------------------------------------------
{
  BText txtCopy=txt;
  txtCopy.Replace(oldChar, newChar);
  return(txtCopy);
}


//--------------------------------------------------------------------
BInt BText::Replace(const BText& oldText, const BText& newText)

/*! Replaces all \a oldText occurrences by \a newText in the actual 
 *  text.
 * \param oldText Text to be replaced
 * \param newText Text to replace with
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(!buffer_) { return(0); }
  BInt number = 0;
  if(HasName() && oldText!=BText(""))
  {
    if((oldText.Length()==1)&&(newText.Length()==1))
    {
      return(Replace(oldText(0), newText(0)));
    }
    BInt   oldPos = 0;
    BChar* pos=Buffer();
    BInt   oldLength = oldText.Length();
    BText  txt("", size_);
    BInt   advance;
    do
    {
      BInt scanned = pos-Buffer();
      if(txt.Length()*2>txt.Size())
      { 
        txt.ReallocateBuffer(2*txt.Size()); 
      }
      if(pos=strstr(Buffer()+oldPos, oldText.String()))
      {
        number++;
        advance = pos-Buffer();
        BText interWord(Buffer(), oldPos, advance-1);
        txt += interWord;
        txt += newText;
        oldPos = advance+oldLength;
      }
      else
      {
        txt += (Buffer()+oldPos);
      }
    }
    while(pos);
    *this = txt;
  }
  return(number);
}


//--------------------------------------------------------------------
BText Replace(const BText& txt, const BText& oldText, const BText& newText)
       
/*! Replaces all \a oldText occurrences by \a newText in a given
 *  text \a txt.
 * \param txt Text where is done the replacement
 * \param oldText Text to be replaced
 * \param newText Text to replace with
 * \sa Replace (BText&, BText&)
 */
//--------------------------------------------------------------------
{
  BText txtCopy = txt;
  txtCopy.Replace(oldText, newText);
  return(txtCopy);
}


//--------------------------------------------------------------------
BText& BText::Compact()

/*! Replaces all tabs (\t) or carriage returns (\\n\r) with whites
 *  characters. Removes the initial and the final whites characters.
 *  And replaces the secuences of white characters with a single one.
 *  Example:
 *    "    Alfa\\n\\nBeta\\n\\t End \\t " -> "Alfa Beta End".
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(length_)
  {
    Replace('\n', ' ');
    Replace('\r', ' ');
    Replace('\t', ' ');
    BText txt = *this;
    BInt j=0;
    BChar oldChar=' ';
    for(const BChar* pos=txt.String(); *pos; pos++)
    {
      if((oldChar!=' ')||(*pos!=' ')) { buffer_[j++] = *pos; oldChar=*pos; }
    }
    if((oldChar==' ')&&(j>0)) { buffer_[length_=j-1]='\0'; }
    else                      { buffer_[length_=j  ]='\0'; }
  }
  if(*this==" ") { *this=""; }
  return(*this);
}
//--------------------------------------------------------------------
BText Compact(const BText& txt) 
/*! Returns the text \a txt compacted*/
//--------------------------------------------------------------------
{
  return(BText(txt).Compact()); 
}


//--------------------------------------------------------------------
BText& BText::Reverse()

/*! Reverses the actual text (Alfa -> aflA).
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(length_)
  {
    BInt  len   = length_;
    BInt  len_1 = len-1;
    BInt  len_2 = len_1/2;
    BInt  len_n;
    BChar aux;
    for(BInt n=0; n<=len_2; n++)
    {
      len_n     = len_1 - n;
      aux       = buffer_[n];
      buffer_[n]     = buffer_[len_n];
      buffer_[len_n] = aux;
    }
  }
  return(*this);
}
/*! Returns the \a txt reversed */
//BText Reverse(const BText& txt) { return(BText(txt).Reverse()); }


//--------------------------------------------------------------------
BText& BText::ToLower()

/*! Converts all characters in the actual text to lower case.
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(HasName())
  { 
    for(BChar* pos=Buffer(); *pos; pos++) 
    { 
      *pos=(BChar)tolower(*pos); 
    }
  }
  return(*this);
}
/*! Returns Converts all characters of the text \a txt to lower case */
BText ToLower(const BText& txt) { return(BText(txt).ToLower()); }


//--------------------------------------------------------------------
BText& BText::ToUpper()

/*! Converts all characters in the actual text to upper case.
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(HasName())
  { 
    for(BChar* pos=Buffer(); *pos; pos++) 
    { 
      *pos=(BChar)toupper(*pos); 
    } 
  }
  return(*this);
}

//--------------------------------------------------------------------
BText ToUpper(const BText& txt) 
/*! Converts all characters of the text \a txt to upper case */
//--------------------------------------------------------------------
{ 
  return(BText(txt).ToUpper()); 
}


//--------------------------------------------------------------------
BBool BText::Match(const BText pattern, BBool caseSensitive) const

/*! Returns true if the pattern \a pattern makes match with the
 *  BText. Works or not in a case sensitive way. The wild character
 *  * only can be used at the begining or end of the pattern. The
 *  wild character can match with any, one or more characters.<br>
 *  Examples:<ul>
 *       <li>(Alfa, Alfa) -> BTRUE  <li>(Alfa, Alf*) -> BTRUE,
 *       <li>(Alfa, *lfa) -> BTRUE  <li>(Alfa, *lf*) -> BTRUE,
 *       <li>(Alfa, *Alfa)-> BTRUE, <li>(Alfa, *lfa*) ->BTRUE,
 *       <li>(Alfa, *lfaX)-> BFALSE <li>(Alfa, *l*f*)-> BFALSE.</ul>
 * \param pattern Text to match with text
 * \param caseSensitive Boolean to indicate if function must be case
 *        sensitive or not
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BBool Ok=BFALSE;
  if(!HasName() || !pattern.HasName()) { Ok=BFALSE; }
  else
  {
    BText t=*this, p=pattern, aux;
    t.Compact();
    p.Compact();
    if(!caseSensitive)
    {
      t.ToLower();
      p.ToLower();
    }
    BInt lp=p.length_;
    BInt lt=t.length_;

    if(p=="*") { Ok=BTRUE; }
    else if(p.Get(0)=='*' && p.Get(lp-1)=='*')
    {
      aux.Copy(p,1,lp-2);
      if(((lp-2)<=lt) && (t.Find(aux)>=0)) { Ok=BTRUE; }
    }
    else if(p.Get(0)=='*' && p.Get(lp-1)!='*') // End with
    {
      aux.Copy(p,1,lp-1);
      Ok=t.EndAt(aux);
//    if(((lp-1)<=lt) && (t.Find(aux)==(1+lt-lp))) { Ok=BTRUE; }
    }
    else if(p.Get(0)!='*' && p.Get(lp-1)=='*') // Begin with
    {
      aux.Copy(p,0,lp-2);
      Ok=t.BeginWith(aux);
//    if(((lp-1)<=lt) && (t.Find(aux)==0)) { Ok=BTRUE; }
    }
    else
    {
      if(t==p) { Ok=BTRUE; }
    }
  }
  return(Ok);
}


//--------------------------------------------------------------------
BText TimeToText(BInt tm, BInt maxSize, const BChar* fmt)
       
/*! Converts a system time value standard localtime format
 * \param tm Time (seconds elapsed since 00:00 hours, Jan 1, 1970 UTC)
 * \param maxSize Max size of buffer
 * \param fmt Format specifiers
 * \return A text with the formated time
 */
//--------------------------------------------------------------------
{
  time_t t = tm;
  static char buf[BMEDIUM_SIZE_STRING];
  assert(maxSize<BMEDIUM_SIZE_STRING);
  BText txt;
  BInt len;
  if(len=strftime(buf, maxSize, fmt, localtime(&t)))
  {
    txt.Copy(buf,len-1);
  }
  return(txt);
}


//--------------------------------------------------------------------
BInt BText::Occurrences(BChar searched) const

/*! Returns the number of occurrences of character \a searched in
 *  this text.
 * \param searched Character to search
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BInt n;
  BInt occurrences=0;
  for(n=0; buffer_[n]!='\0'; n++)
  { 
    assert(n<length_);
    if(buffer_[n]==searched) { occurrences++; } 
  }
  return(occurrences);
}


//--------------------------------------------------------------------
BInt BText::Occurrences(const BText& searched) const

/*! Returns the number of occurrences of text \a search in this text
 * \param searched Text to search
 */
//--------------------------------------------------------------------
{
  if(searched.Length()==1) { return(Occurrences(searched.Get(0))); }
  assert(IsAllOk());
  BInt  n=0;
  BInt  occurrences=0;
  while((n>=0) && (n+searched.Length()<=Length()))
  {
    n = Find(searched, n);
    if(n>=0) {
	  occurrences++;
      n += searched.Length();
	}
  }
  return(occurrences);
}


//--------------------------------------------------------------------
BInt NumberOfOccurrences(const BText& line, BChar searched)

/*! Returns the number of occurrences of character \a searched in
 *  the text object \a line
 * \param line BText object to search in
 * \param searched Character to search
 */
//--------------------------------------------------------------------
{
  return(line.Occurrences(searched));
}


//--------------------------------------------------------------------
BInt ReadAllTokens(const BText& line, BArray<BText>& txt, BChar sep)

/*! Breaks a text \a line each time appears a separator character \a
 *  sep, allocates an array of BText and puts all tokens into.
 * \param line BText object to break
 * \param txt Array of BText to store the tokens
 * \param sep Separator character
 * \return  Number of tokens.
 */
//--------------------------------------------------------------------
{
  BInt maxItems = 1+NumberOfOccurrences(line, sep);
  BInt n=0;
  txt.ReallocBuffer(maxItems);
  const BChar* oldPos = line.String();
  const BChar* pos    = oldPos;
  for(n=0; (n<maxItems) && pos && oldPos[0]; n++)
  {
    if(pos = strchr(oldPos, sep))
    {
      txt[n].Copy(oldPos, 0, pos-oldPos-1);
      oldPos=pos+1;
    }
    else { txt[n].Copy(oldPos); }
  }
//if(!txt[n-1].HasName()) { txt.ReallocBuffer(n-1); }
  return(n);
}

//--------------------------------------------------------------------
BInt ReadAllTokens(const BText& line, BArray<BText>& txt, BChar sep, BChar quote, BChar scape)

/*! Breaks a text line each time appears a separator character
 *  sep, allocates an array of BText and puts all tokens into.
 *  Along quoted tokens, separator characters will be skiped
 * \param line BText object to break
 * \param txt : Array of BText to store the tokens
 * \param sep : Separator character. Usually ;
 * \param quote : String quotes indicator. Usually " 
 * \param quote : Scape character into quoted strings. Usually \
 * \return  Number of tokens.
 */
//--------------------------------------------------------------------
{
  static char token_[BLONG_SIZE_STRING];
  bool skiping = false;
  int n, k;
  txt.AllocBuffer(0);
  const BChar* pos = line.String();
  if(!pos) { return(0); }
  if(sep==quote) 
  { 
    Error(BText("ReadAllEnclosedTokens: cannot use the same separator and quote character ")+sep); 
    return(0);
  }
  if(sep==scape) 
  { 
    Error(BText("ReadAllEnclosedTokens: cannot use the same separator and scape character ")+sep); 
    return(0);
  }
  if(quote==scape) 
  { 
    Error(BText("ReadAllEnclosedTokens: cannot use the same quote and scape character ")+quote); 
    return(0);
  }
  txt.AllocBuffer(10);
  for(n=k=0; pos[0]; pos++)
  {
    if(!skiping)
    {
      skiping = pos[0]==quote;
    }
    else if((pos[0]==quote)&&(pos[-1]!=scape))
    {
      skiping = false;
    }
    if(!skiping) 
    { 
      if(pos[0]!=sep) 
      { 
        token_[k++] = pos[0]; 
      }
      else 
      { 
        if(n==txt.Size()) { txt.ReallocBuffer(2*n); }
        token_[k++] = '\0'; 
        txt[n++].Copy(token_,k);
        k = 0;
      }
    }
    else
    {
      token_[k++] = pos[0]; 
    }
  }
//if(k)
  {
    token_[k++] = '\0'; 
    txt[n++].Copy(token_,k);
  }
  txt.ReallocBuffer(n);
  return(n);
}
/* */

//--------------------------------------------------------------------
BInt ReadAllCells(const BText&             txt,
            BArray< BArray<BText> >& cell,
            BChar             lineSep,
            BChar             wordSep)
    
/*! Constructs a matrix of text cells \cell breaking a text \a txt
 *  into rows by \a lineSep character and into columns by \a wordSep
 *   character.
 * \param txt BText object to break
 * \param cell Matrix of BText to store the tokens
 * \param lineSep Row separator character
 * \param wordSep Columns separator character
 * \return  Number of lines (rows).
 */
//--------------------------------------------------------------------
{
  BArray<BText> line; ReadAllTokens(txt, line, lineSep);

  cell.AllocBuffer(line.Size());
  for(BInt n = 0; n<line.Size(); n++)
  {
    ReadAllTokens(line[n], cell[n], wordSep);
    for(BInt m = 0; m<cell[n].Size(); m++) { cell[n][m].Compact(); }
  }
  return(line.Size());
}


//--------------------------------------------------------------------
BText BText::RightPad(BInt newSize, BChar fill) const

/*! Returns a new BText with a new size \a newSize, right filling
 *  this with the \a fill fill character. If this btext has more
 *  characters than \a newSize returns a copy of this without cut.
 * \param newSize New size of text
 * \param fill Character to fill
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BText pad(1+newSize-Length(),fill);
  return((*this)+pad);
}


//--------------------------------------------------------------------
BText BText::LeftPad(BInt newSize, BChar fill) const

/*! Returns a new BText with a new size \a newSize, left filling this
 *  with the necesary \a fill characters. If this btext has more
 *  characters than \a newSize returns a copy of this without cut.
 * \param newSize New size of text
 * \param fill Character to fill
 */ 
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BText pad(1+newSize-Length(),fill);
  pad+=(*this);
  return(pad);
}


//--------------------------------------------------------------------
BText BText::Pad(BAlignType align, BInt newSize, BChar fill) const

/*! Returns a new BText with a new size \a newSize, filling this with
 *  the necesary \a fill characters. Filling can be either left or
 *  rigth depending on the parameter \a algin. If this btext has more
 *  characters than \a newSize returns a copy of this without cut.
 * \param align Alignment for the pad
 * \param newSize New size of text
 * \param fill Character to fill
 */
//--------------------------------------------------------------------
{
  switch(align)
  {
    case(BRIGHT) : return(RightPad(newSize, fill));
    case(BLEFT)  : return( LeftPad(newSize, fill));
  }
  return(*this);
}


//--------------------------------------------------------------------
BText BText::InsidePad(BInt newSize, BChar fill, const BText& last) const

/*! Returns a new BText with a new size \a newSize. Build a text in
 *  the following way: If text is "Alfa" and parameters are
 *  12,'.',"Beta" -> "Alfa....Beta". If sum of texts have more
 *  characters  than \a newSize returns a simple concat.
 * \param newSize New size of text
 * \param fill Character to fill
 * \param last Text to concat at the end
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BText pad((*this)+BText(1+newSize-Length()-last.Length(),fill)+last);
  return(pad);
}


//--------------------------------------------------------------------
BText BText::InsidePad(BInt newSize, BChar fill, BInt number) const

/*! Returns a new BText with a new size \newSize. Build a text in the
 *  following way: If text is "Alfa" and parameters are
 *  12,'.',9007-> "Alfa....9007". If sum of text and the number has
 *  more characters than \a newSize returns a simple concat. To
 *  transform the number the default format is used.
 * \param newSize New size of text
 * \param fill Character to fill
 * \param last Number to concat at the end
 */
//--------------------------------------------------------------------
{
  BText last("");
  last=last+number;
  return(InsidePad(newSize,fill,last));
}


//--------------------------------------------------------------------
BText BText::LeftCompact()

/*! Deletes all blanks at left side
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BInt n=0;
  while((n<Length()-1) && strchr(" \n\r\t",(*this)(n))) { n++; }
  return(BText(Buffer(), n, length_-1));
}


//--------------------------------------------------------------------
BText BText::RightCompact()

/*! Deletes all blanks at right side
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BInt n=length_-1;
  while((n>0) && strchr(" \n\r\t",(*this)(n))) { n--; }
  return(BText(Buffer(), 0, n));
}


//--------------------------------------------------------------------
BText BText::ExternCompact()

/*! Deletes all blanks at both left and right sides
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  BInt n=0;
  BInt m=length_-1;
  while((n<Length()-1) && strchr(" \n\r\t",(*this)(n))) { n++; }
  while((m>0)          && strchr(" \n\r\t",(*this)(m))) { m--; }
  return(BText(Buffer(), n, m));
}


//--------------------------------------------------------------------
BBool OverWrite(const BText& fileName, const BText& txt)

/*! Overwrites a file \a fileName with a given text \a txt
 * \param fileName File to be overwritten
 * \param txt Text to overwrite with
 */
//--------------------------------------------------------------------
{
  ofstream file(fileName.String(), ios::out);
  if(!file.good())
  {
    Warning(I2("Can not open for writing file ",
         "No se puede abrir para escritura el fichero ") + fileName);
    return(BFALSE);
  }
  file << txt.String();
  return(BTRUE);
}


//--------------------------------------------------------------------
BBool AppendWrite(const BText& fileName, const BText& txt)

/*! Appends a given text \a txt at the end of a file \a fileName
 * \param fileName File to write
 * \param txt Text to append to the file
 */
//--------------------------------------------------------------------
{
  ofstream file(fileName.String(), ios::app);
  if(!file.good())
  {
    Warning(I2("Can not open for writing file ",
         "No se puede abrir para escritura el fichero ") + fileName);
    return(BFALSE);
  }
  file << txt.String();
  return(BTRUE);
}


//--------------------------------------------------------------------
istream& operator>> (istream& is, BText& txt)

/*! Reads a word from an input stream
 */
//--------------------------------------------------------------------
{
  BChar buf[256];
  is >> buf;
  txt=buf;

  return(is);
}

#define READ_CRLF_AS_LF 


//--------------------------------------------------------------------
void BText::GetLine(istream& is, BInt maxChars, BChar until)

/*! Reads a maximum of \a maxChars characters from the input stream
 *  \a is stoping at the first occurrence of character \a until.
 *  Puts the read text in txt.Actually doesn't work as description 
 *  says.
 * \param is Input stream to read from
 * \param maxChars Maximun number of characters copied
 * \param until Stop character
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(size_<maxChars+1)
  {
    FreeItems();
    AllocItems(maxChars+1);
  }
  length_ = 0;
  BChar ch  = '\0';
  bool untilIsLF = until=='\n';
  for(;(length_<=maxChars)&&(ch!=until)&&(!is.eof()); length_++)
  {
    is.get(ch);
#   ifdef READ_CRLF_AS_LF
    if(untilIsLF && (ch == '\r')&&(!is.eof()))
    {
      is.get(ch);
      if(ch!='\n') { is.unget(); ch = '\r'; }
    }
#   endif
    buffer_[length_] = ch;
  }
  if(ch==until) { length_--; }
  if(ch=='\0')  { length_--; }
  buffer_[length_] = '\0';

}

//--------------------------------------------------------------------
void BText::GetLine(FILE* file, BInt maxChars, BChar until)

/*! Reads a maximum of \a maxChars characters from the input stream
 *  \a is stoping at the first occurrence of character \a until.
 *  Puts the read text in txt.Actually doesn't work as description 
 *  says.
 * \param is Input stream to read from
 * \param maxChars Maximun number of characters copied
 * \param until Stop character
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(size_<maxChars+1)
  {
    FreeItems();
    AllocItems(maxChars+1);
  }
  length_ = 0;
  BChar ch  = '\0', c;
  bool untilIsLF = until=='\n';
  for(;(length_<=maxChars)&&(ch!=until)&&(!feof(file)); length_++)
  {
    ch = fgetc(file);
#   ifdef READ_CRLF_AS_LF
    if(untilIsLF && (ch == '\r')&&(!feof(file)))
    {
      ch = fgetc(file);
      if(ch!='\n') { ungetc(c, file); ch = '\r'; }
    }
#   endif
    buffer_[length_] = ch;
  }
  if(ch==until) { length_--; }
  if(ch=='\0')  { length_--; }
  buffer_[length_] = '\0';
}

//--------------------------------------------------------------------
BBool BText::BeginWith(const BText& pattern) const

/*! Returns BTRUE if the first characters of this text make pattern
 *  \a pattern 
 * \param pattern Pattern
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  int len = pattern.Length();
  if(length_<len) { return(false); }
  const char* a=String();
  const char* b=pattern.String();
  for(int n=0;n<len;n++)
  {
    if(a[n]!=b[n]) { return(false); }
  }
  return(true);
}

//--------------------------------------------------------------------
BBool BText::EndAt(const BText& pattern) const

/*! Returns BTRUE if the last characters of this text make pattern.
 * \param pattern Pattern
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  int len = pattern.Length();
  if(length_<len) { return(false); }
  const char* a=String()+length_-1;
  const char* b=pattern.String()+len-1;
  for(int n=0;n<len;n++)
  {
    if(a[-n]!=b[-n]) { return(false); }
  }
  return(true);
}

//--------------------------------------------------------------------
istream& GetLine(istream& is, BText& txt, BInt maxChars, BChar until)

/*! Reads a maximum of \a maxChars characters from the input stream
 *  \a is stoping at the first occurrence of character \a until.
 *  Puts the read text in \a txt.
 * \param is Input stream to read from
 * \param txt BText object to store the read characters
 * \param maxChars Maximun number of characters copied
 * \param until Stop character
 */
//--------------------------------------------------------------------
{
  txt.GetLine(is,maxChars,until);
  return(is);
}


//--------------------------------------------------------------------
BInt Read(istream& is, BText& txt, BInt size, BChar end)

/*! Reads all characters from an input stream \a is stoping at the
 *  first occurrence of character \a end.
 *  Puts the read text in \a txt.
 * \param is Input stream to read from
 * \param txt BText object to store the read characters
 * \param size Initial size of buffer
 * \param end Stop character
 */
//--------------------------------------------------------------------
{
//InitTotalTime("Read");
  if(size<256) { size=256; }
  txt.PutLength(0);
  txt.ReallocateBuffer(size);
  BInt n=-1;
  char ch;
  do
  {
    n++;
    if(n==txt.Size()-1)
    {
      size=2*txt.Size();
      txt.PutLength(n);
      txt.ReallocateBuffer(size);
    }
    ch = (BChar)is.get();
#   ifdef READ_CRLF_AS_LF
    if((ch == '\r')&&(!is.eof()))
    {
      ch = is.get();
      if(ch!='\n') { is.unget(); ch = '\r'; }
    }
#   endif
    txt.PutChar(n,ch);
  }
  while(!(is.eof()) && (txt.String()[n]!=end));
  txt.PutLength(n++);
//SumPartialTime;
  return(n+1);
}


//--------------------------------------------------------------------
BBool ReadFile(BText& data, const BText& fName)

/*! Reads all characters from an file input stream which name is
 *  given by a text object \a fName, and writes then into the text
 *  object \a data.
 * \param data BText object to store the read characters
 * \param fName Name of file
 * \return BTRUE is file has been successfully read or false if not
 */
//--------------------------------------------------------------------
{
  ifstream in(fName.String(), OPENFLAG);
  if(!in.good())
  {
    Warning(I2("Can not open for reading file ",
         "No se puede abrir para lectura el fichero ") + fName);
    return(BFALSE);
  }
  Read(in,data);
  return(BTRUE);
//Std(data);
}



//--------------------------------------------------------------------
BBool ReadCell(BTextCell& cell,const BText& fName,char l,char w)

/*! Reads all characters from an file input stream which name is
 *  given by a text object \a fName, and writes then into the text
 *  object \a data.
 * \param cell BText object to store the read characters
 * \param fName Name of file
 * \param l Row separator character
 * \param w Columns separator character
 * \return BTRUE is file has been successfully read or false if not
 */
//--------------------------------------------------------------------
{
  ifstream in(fName.String(), OPENFLAG);
  if(!in.good())
  {
    Error(I2("Can not open file ",
             "No se puede abrir el fichero ") + fName);
    return(BFALSE);
  }
  cell.ReallocBuffer(100);
  char line[1024];
  in.getline(line, 1023, l);
  ReadAllTokens(Compact(line),cell[0],w);
  BInt numRows=0, numCols=cell[0].Size();
  while(!in.eof())
  {
    for(BInt k=0; k<numCols; k++)
    {
      in.getline(line, 1023, w);
      BText lin = line; lin.Compact();
      if(!in.eof())
      {
        if(k==0)
        {
          numRows++;
          if(numRows>=cell.Size()) { cell.ReallocBuffer(2*numRows); }
          cell[numRows].ReallocBuffer(numCols);
        }
        cell[numRows][k] = lin;
      }
    }
    in.getline(line, 1023, l);
  }
  cell.ReallocBuffer(numRows+1);
  return(BTRUE);
}



