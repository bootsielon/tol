/* txt.cpp: implements BText basic class behavior
            GNU/TOL Language.

   Copyright (C) 2003,2004 - Bayes Decision, SL (Spain [EU])

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

//#define TRACE_LEVEL 4

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bout.h>
#include <tol/tol_btimer.h>
#include <tol/tol_bsys.h>
#include <assert.h>

//--------------------------------------------------------------------
  static void SetLanguagePageCode(int language)
//--------------------------------------------------------------------
{
  if(language)
  {
    #ifdef UNIX
  //char* spanishCodePage = "spanish";            //FAIL
  //char* spanishCodePage = "ISO-8859-1";         //FAIL
  //char* spanishCodePage = "es_ES.UTF-8";        //FAIL 
  //char* spanishCodePage = "es_ES.latin1";       //FAIL 
  //char* spanishCodePage = "es_ES.iso-8859-1";   //FAIL 
  //  const char* spanishCodePage = "es_ES.utf-8";        //FAIL 
  //  setlocale(LC_CTYPE, spanishCodePage);
    #else
    const char* spanishCodePage = "Spanish_Spain.1252"; //OK!
    setlocale(LC_CTYPE, spanishCodePage);
    #endif
  //printf("setlocale LC_CTYPE = \"%s\" -> \"%s\"", spanishCodePage, codepage.String());
  }
  else
  {
    #ifdef UNIX
    //  const char* englishCodePage = "en_US.ISO-8859-1";           //FAIL
    // setlocale(LC_CTYPE, englishCodePage);
    #else
    const char* englishCodePage = "English_United States.1252"; //OK!
    setlocale(LC_CTYPE, englishCodePage);
    #endif
   //printf("setlocale LC_CTYPE = \"%s\" -> \"%s\"", spanishCodePage, codepage.String());
  }
}

//--------------------------------------------------------------------
static BBool ReadLanguage()

/*! Reads a language from file 'language.txt' and returns it
 */
//--------------------------------------------------------------------
{
  int language = 1;
  FILE* lan = fopen(BSys::TolAppData()+"language.txt","r");
  if(!lan)
  {
    lan = fopen("language.txt","r");
    if(lan)
    {
      fclose(lan); 
      BSys::Copy("language.txt", BSys::TolAppData()+"language.txt");
      lan = fopen(BSys::TolAppData()+"language.txt","r");
    }
  }
  if(lan)
  {
    fscanf(lan,"%d",&language);
    fclose(lan);
  }
  SetLanguagePageCode(language);
  return((BBool)language);
};


BTraceInit("txt.cpp");


BChar  BText::overflow_;
BInt   BText::margin_;
BInt   BText::defectSize_;
BBool  BText::language_;
const BChar* BText::formatBInt_;
const BChar* BText::formatBReal_;
const BChar* BText::formatBInt64_;
BText  BText::none_   (StaticInit());
BText  BText::unknown_(StaticInit());

#define _COUNT_TEXT_
#ifdef _COUNT_TEXT_
static int _count_instances_ = 0;
static double _count_MegaBytes_ = 0;
static double _megabyte_ = 1.0/(1024.0*1024.0);
int BText_count_instances() { return(_count_instances_); }
double BText_count__MegaBytes() { return(_count_MegaBytes_); }
#else
int BText_count_instances() { return(0); }
double BText_count__MegaBytes() { return(0.0); }
#endif 

//--------------------------------------------------------------------
  DefIsAlreadyInitilialized(BText);
  bool BText::InitializeClass()
//--------------------------------------------------------------------
{
  if(!IsAlreadyInitilialized(BText)) 
  {
    BText::overflow_     = '\0';
    BText::margin_       = 10;
    BText::defectSize_   = 16;
    BText::none_         = "";
    BText::unknown_      = "UNKNOWN TEXT";
    BText::formatBInt_   = "%ld";
    BText::formatBReal_  = "%lg";
    #if defined(_MSC_VER) && (_MSC_VER<1400)
    BText::formatBInt64_ = "%I64d";
    #else
    BText::formatBInt64_ = "%lld";
    #endif
    BText::language_     = ReadLanguage();
  }
  return(true);
}

//--------------------------------------------------------------------
//Enable this section only for statistical analysis of BText 
//reallocating behaviour
//#define STATREALLOC
#ifdef STATREALLOC
  static BINT64 nRealloc_ = 0;
  static BINT64 bRealloc_ = 0;
  static BINT64 tRealloc_ = 0;
  static BINT64 pRealloc_[32];

  bool InitStatRealloc()
  {
    BText::PutDefectSize(1);
    for(int i=0; i<32; i++)
    {
      pRealloc_[i]=0;
    }
    return(true);
  }
  static bool initStatRealloc_ = InitStatRealloc();

  void StatRealloc()
  {
    BText ld = "";
    for(int i = 0; i<32; i++)
    {
      ld<<"\n  "<<i<<" -> "<<pRealloc_[i];
    }
    Std(BText("\nBText::ReallocateBufferCall ")+
    "\t Called = " + nRealloc_+
    "\t Bytes  = "  + bRealloc_+
    "\t Clock  = "  + tRealloc_+
    "\n log2-density:"+ld);
  }

#else
void StatRealloc() {}
#endif
//--------------------------------------------------------------------


/*! PURPOSE: Returns BText overflow character */
BChar& BText::Overflow ()
{
  return(BText::overflow_); 
}

/*! PURPOSE: Returns BText integer format string */
const BChar* BText::FormatBInt ()
{
  return(formatBInt_); 
}

/*! PURPOSE: Returns BText real format string */
const BChar* BText::FormatBReal()
{
  return(formatBInt_); 
}

/*! PURPOSE: Returns BText language */
BBool BText::Language ()
{
  return(language_); 
}

/*! PURPOSE: Returns BText margin */
BInt BText::Margin () 
{
  return(margin_); 
}

/*! PURPOSE: Returns BText defectSize */
BInt BText::DefectSize ()
{ 
  return(defectSize_); 
}

/*! PURPOSE: Returns BText unknown object */
BText& BText::Unknown () 
{ 
  return(unknown_); 
}

/*! PURPOSE: Returns BText none object */
BText& BText::None () 
{ 
  return(none_); 
}

/*! PURPOSE: Sets integer format \a f */
void BText::PutFormatBInt (const char* f)
{
  formatBInt_ =f; 
}

/*! PURPOSE: Sets real format \a f */
void BText::PutFormatBReal (const char* f) 
{
  formatBReal_=f; 
}

/*! PURPOSE: Sets language \a l */
void BText::PutLanguage (BBool l) 
{
  SetLanguagePageCode(language_=l);
}

/*! PURPOSE: Sets margin \a m */
void BText::PutMargin (BInt m) 
{
  margin_=m; 
}

/*! PURPOSE: Sets defectSize \a d */
void BText::PutDefectSize (BInt d) 
{ 
  defectSize_ =d; 
}

//! Returns a pointer to BChar with an empty char.
//--------------------------------------------------------------------
  void BText::InitBuffer()

/*! Returns a pointer to BChar with an empty char.
 */
//--------------------------------------------------------------------
{
  if(BText::DefectSize())
  {
    AllocItems(BText::DefectSize());
    buffer_[0] = 0;
  }
  else
  {
    buffer_ = NULL;
  }
};

//--------------------------------------------------------------------
bool BText::IsAllOk() const
//--------------------------------------------------------------------
{
  if(!buffer_) 
  { 
    return(!size_ && !length_); 
  }
  else
  { 
    return(HasAssignedItems() && (size_>0) && (length_>=0)); 
  }
}

//--------------------------------------------------------------------
short BText::HasAssignedItems() const
//--------------------------------------------------------------------
{
  if(!buffer_) { return(false); }
# if (__USE_POOL__==__POOL_NONE__)
  return(-1);
# elif (__USE_POOL__==__POOL_BFSMEM__)
  assert(size_>=0);
  int baseSize = sizeof(char)*size_;
  return(BFixedSizeMemoryBase::
    Instance(baseSize)->IsAssigned(buffer_,_bfsm_VectorPageNum__));
#endif
}

//--------------------------------------------------------------------
void BText::AllocItems(unsigned int numItems) 
//--------------------------------------------------------------------
{ 
  assert(!buffer_ && !size_ && numItems); 
  size_ = numItems; 
# if (__USE_POOL__==__POOL_NONE__)
  buffer_ = new BChar[numItems]; 
# elif (__USE_POOL__==__POOL_BFSMEM__)
    int baseSize = sizeof(char)*size_;
  buffer_ = (char*)BFixedSizeMemoryBase::
    Instance(baseSize)->New(baseSize,_bfsm_VectorPageNum__); 
# endif
  assert(buffer_); 
#ifdef _COUNT_TEXT_
  _count_MegaBytes_ += numItems*_megabyte_;
#endif 

} 

//--------------------------------------------------------------------
void BText::FreeItems() 
//--------------------------------------------------------------------
{ 
  if(buffer_)
  {
    assert(size_);
#   if (__USE_POOL__==__POOL_NONE__)
    delete [] buffer_; 
#   elif (__USE_POOL__==__POOL_BFSMEM__)
    int baseSize = sizeof(char)*size_;
    BFixedSizeMemoryBase::
      Instance(baseSize)->Delete(buffer_,_bfsm_VectorPageNum__);
#   endif
#ifdef _COUNT_TEXT_
  _count_MegaBytes_ -= size_*_megabyte_;
#endif 
    buffer_ = NULL; 
    size_ = 0; 
  }
  else
  {
    assert(!size_ && !length_);
  }
} 

//--------------------------------------------------------------------
BChar* BText::AllocItems(int numItems, unsigned short& pgNm) 
//--------------------------------------------------------------------
{ 
  assert(numItems); 
# if (__USE_POOL__==__POOL_NONE__)
  BChar* buffer = new BChar[numItems]; 
# elif (__USE_POOL__==__POOL_BFSMEM__)
  assert(numItems);
    int baseSize = sizeof(char)*size_;
  char* buffer = (char*)BFixedSizeMemoryBase::
    Instance(baseSize)->New(baseSize,pgNm);
# endif
  assert(buffer_); 
#ifdef _COUNT_TEXT_
  _count_MegaBytes_ += numItems*_megabyte_;
#endif 
  return(buffer); 
} 

//--------------------------------------------------------------------
void BText::FreeItems(BChar* buffer, int numItems, unsigned short pgNm) 
//--------------------------------------------------------------------
{ 
  assert(buffer && numItems); 
#ifdef _COUNT_TEXT_
  _count_MegaBytes_ -= numItems*_megabyte_;
#endif 
# if (__USE_POOL__==__POOL_NONE__)
  delete [] buffer; 
# elif (__USE_POOL__==__POOL_BFSMEM__)
  int baseSize = sizeof(char)*numItems;
  BFixedSizeMemoryBase::
    Instance(baseSize)->Delete(buffer,pgNm);
# endif
} 

//--------------------------------------------------------------------
BText::BText() 
/*! Creates a text and fills it with an empty character.
    \sa InitBuffer
*/
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
}


//--------------------------------------------------------------------
BText::BText(BInt size) 

/*! Creates a text of the size specified by \a size and fills it
 *  with the null character.
 * \param size Size of text
*/
//--------------------------------------------------------------------
: buffer_(NIL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  if(size < 1) { size=1; }
  AllocItems(size);
  length_ = size_ - 1;
  if(length_)
  {
    memset(buffer_, '\0', size_);
  }
}


//--------------------------------------------------------------------
BText::BText(BInt size, BChar fill)

/*! Creates a text and fills it with char \a fill repeated \a size
 *  times.
 * \param size Size of text
 * \param fill Char to fill text
 */
//--------------------------------------------------------------------
: buffer_(NIL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  size++;
  if(size < 1) { size=1; }
  AllocItems(size);
  length_ = size_ - 1;

  if(length_)
  {
    memset(buffer_, fill, length_);
  }
  buffer_[length_] = '\0';
}


//--------------------------------------------------------------------
BText::BText(const BChar* str)

/*! Creates a text and fills it with the string \a str.
 * \param str String to fill text
 * \sa Copy (const BChar*)
 */
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
  if(str && str[0]) 
  {
    Copy(str);
  }
}


//--------------------------------------------------------------------
BText::BText(const BChar* str, BInt length)
/*! Creates a text and fills it with the first \a length chars of
 *  string \a str.
 * \param str String to fill text
 * \param length Length 
 * \sa Copy (const BChar*, BInt)
*/
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
  Copy(str, length);
}


//--------------------------------------------------------------------
BText::BText(const BChar* str, BInt from, BInt until)
/*! Creates a text and fills it with the substring [\a from, 
 * \a until] of the string \a str
 * \param str String to fill text
 * \param from Starting position of string to fill text
 * \param until Ending position of string to fill text
 * \sa Copy (const BChar*, BInt, BInt)
 */
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
  if(str && str[from]) { Copy(str, from, until); }
}


//--------------------------------------------------------------------
BText::BText(const BText& txt, BInt from, BInt until)
/*! Creates a text and fills it with the substring [\a from,
 * \a until] of the text object \a txt
 * \param txt BText object to fill text
 * \param from Starting position of string to fill text
 * \param until Ending position of string to fill text
 * \sa Copy (const BText&, BInt, BInt)
*/
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
  Copy(txt, from, until);
}


//--------------------------------------------------------------------
BText::BText(const BText& txt)
/*! Creates a text and fills it with the text object \a txt.
 * \param txt BText object to fill text
 * \sa Copy (const BText&)
 */
//--------------------------------------------------------------------
: buffer_(NULL), length_(0), size_(0)
{
#ifdef _COUNT_TEXT_
  _count_instances_ ++;
#endif 
  InitBuffer();
  Copy(txt);
}


/*! Text destructor.Frees text buffer's memory
    \sa DeleteBuffer
*/
BText::~BText()
{
#ifdef _COUNT_TEXT_
  _count_instances_ --;
#endif 
  DeleteBuffer();
}


//--------------------------------------------------------------------
void BText::DeleteBuffer()

/*! Frees the text buffer's memory.
 */
//--------------------------------------------------------------------
{
  FreeItems();
  length_=0;
}


//--------------------------------------------------------------------
void BText::AllocateBuffer(BInt size, BChar fill)

/*! Allocate memory for the text buffer.
 * \param size Size of text buffer
 * \param fill Character to fill text
 */
//--------------------------------------------------------------------
{
  if(buffer_ || size_ || length_)
  {
    Error(I2(" FATAL : Bad memory allocation in text creation.",
             " FATAL : Fallo en asignación de memoria para textos."));
    assert(!buffer_);
    assert(!size_);
    assert(!length_);
  }
  if(size < 1) { size=1; }
  AllocItems(size);
  memset(buffer_, fill, size);
  buffer_[length_=size_-1]='\0';
}

//--------------------------------------------------------------------
void BText::ReallocateBuffer(BInt newSize)

/*! Reallocate memory for the text buffer just to the new size.
 * \param newSize New size for text buffer
 */
//--------------------------------------------------------------------
{
  assert(IsAllOk());
  if(newSize > size_)
  {
#   ifdef STATREALLOC
    nRealloc_ ++;
    BTimer tms;
    int p_ = int(log(double(newSize))/log(2.0));
    pRealloc_[p_]++;
    bRealloc_ += (newSize-size_);
#   endif
    if(!(buffer_ && (length_>=0) && (size_>=0) && (length_<size_)))
    {
      Error(I2(" FATAL : Text memory reallocation fail.",
               " FATAL : Fallo en reasignación de memoria de textos."));
    }
    BInt nextSize = BInt(double(newSize)*(100.0+margin_)*0.01);
    if(nextSize < newSize) { nextSize = newSize; }
    if (nextSize<=0) 
    {
      cout << "Error, se pide realocar de " << size_  
           << " a " << nextSize << endl;
    } 
    else 
    {
      BChar* oldBuffer  = buffer_;
#if (__USE_POOL__==__POOL_BFSMEM__)
      int oldPgNm = _bfsm_VectorPageNum__;
#else
      int oldPgNm = 0;
#endif
      int oldSize = size_;
      buffer_ = NULL;
      size_   = 0;
      AllocItems(nextSize);
      if(length_) 
      { 
        memcpy(buffer_, oldBuffer, length_); 
      }
      buffer_[length_]='\0';
      FreeItems(oldBuffer, oldSize, oldPgNm);
    }
#   ifdef STATREALLOC
    tRealloc_ += tms.Clk();
#   endif
  }
}




//--------------------------------------------------------------------
BInt BText::BinWrite(FILE* fil) const

/*! Appends this BText to a binary file.
 * \param fil Binary file stream to write in
 */
//--------------------------------------------------------------------
{
  fwrite(&length_, sizeof(BInt), 1, fil);
  fwrite(String(), sizeof(BChar), length_+1, fil);
  return((length_+1)*sizeof(BChar) + sizeof(BInt));
}


//--------------------------------------------------------------------
BInt BText::BinRead(FILE* fil)

/*! Reads this BText from a binary file.
 * \param fil Binary file stream to read from
 */
//--------------------------------------------------------------------
{
#ifdef TRACE_LEVEL
  BText fun = BText(" BText::BinRead");
#endif
  TRACE_SHOW_LOW(fun,"Begin");
  BInt l;
  fread(&l, sizeof(BInt), 1, fil);
  TRACE_SHOW_MEDIUM(fun,"1");
  if(l<0) 
  { 
    TRACE_SHOW_MEDIUM(fun,"2");
    Error(I2("Negative text length. Cannot execute function",
             "Longitud de texto negativa. No se puede ejecutar la función")+
          " BText::BinRead");
    TRACE_SHOW_LOW(fun,"End error");
    return(-1); 
  }
  TRACE_SHOW_MEDIUM(fun,"3");
  ReallocateBuffer(l+1);
  TRACE_SHOW_MEDIUM(fun,"4");
  length_=l;
  TRACE_SHOW_MEDIUM(fun,"5");
  fread( Buffer(), sizeof(BChar), length_+1, fil);
  TRACE_SHOW_LOW(fun,"End normal");
  return((length_+1)*sizeof(BChar) + sizeof(BInt));
}


/*! PURPOSE: Returns BText object */
const BText& BText::Name () const
{
  return(*this); 
}

/*! PURPOSE: Dumps the content of the actual text*/
BText BText::Dump () const 
{
  return(BText("\"")+Name()+"\""); 
}

/*! PURPOSE: Returns information about BText object */
BText BText::Info () const
{
  return(Dump()); 
}

/*! PURPOSE: Returns the string of BText object */
const BChar* BText::String () const 
{
  assert(!buffer_ || HasAssignedItems());
  return(buffer_);  
}

/*! PURPOSE: Returns the text's buffer */
BChar* BText::Buffer ()
{
  assert(!buffer_ || HasAssignedItems());
  return(buffer_); 
}

/*! PURPOSE: Checks if BText object has name */
BBool BText::HasName () const 
{
  return(String() && (Length()>0)); 
}

/*! PURPOSE: Returns the text's size */
BInt BText::Size () const 
{
  return(size_); 
}

/*! PURPOSE: Returns the text's length */
BInt BText::Length () const
{
  return(length_); 
}

/*! Sets the text's length
    \param n New length of BText object
*/
void BText::PutLength (BInt n)
{
  assert(IsAllOk());
  assert((n>=0)&&((n<size_)||((size_==0)&&(n==0))));
  if((buffer_!=NULL) && (n>=0) && (n<size_)) 
  { 
    length_=n; 
    buffer_[n]='\0';
  }
}

/*! PURPOSE: Calculates and sets text's length */
void BText::CalcLength() 
{
  length_=strlen(buffer_); 
}

BText& BText::operator= (const BText& txt) { return(Copy(txt)); }
BText& BText::operator= (const BChar* str) { return(Copy(str)); }

BText& BText::operator+= (const BText&      txt) { return (Concat(txt)); }
BText& BText::operator+= (const BChar*      str) { return (Concat(str)); }
BText& BText::operator+= (      BInt        val) { return (Concat(val)); }
BText& BText::operator+= (      long int    val) { return (Concat(val)); }
BText& BText::operator+= (      BINT64      val) { return (Concat(val)); }
BText& BText::operator+= (      BReal       val) { return (Concat(val)); }
BText& BText::operator+= (      long double val) { return (Concat(val)); }
BText& BText::operator+= (      BChar       ch ) { return (Concat(ch )); }

BText& BText::operator<< (const BText&      txt) { return (Concat(txt)); }
BText& BText::operator<< (const BChar*      str) { return (Concat(str)); }
BText& BText::operator<< (      BInt        val) { return (Concat(val)); }
BText& BText::operator<< (      long int    val) { return (Concat(val)); }
BText& BText::operator<< (      BINT64      val) { return (Concat(val)); }
BText& BText::operator<< (      BReal       val) { return (Concat(val)); }
BText& BText::operator<< (      long double val) { return (Concat(val)); }
BText& BText::operator<< (      BChar       ch ) { return (Concat(ch )); }

bool BText::operator== (const BText& txt) const
{
  return (StrCmp(String(), txt.String()) == 0);
}

bool BText::operator!= (const BText& txt) const
{
  return (StrCmp(String(), txt.String()) != 0); 
}

bool BText::operator< (const BText& txt) const
{
  return (StrCmp(String(), txt.String()) <  0); 
}

bool BText::operator> (const BText& txt) const
{
  return (StrCmp(String(), txt.String()) >  0); 
}

bool BText::operator<= (const BText& txt) const
{
  return (!(*this > txt)); 
}

bool BText::operator>= (const BText& txt) const
{
  return (!(*this < txt)); 
}

bool BText::operator== (const BChar* str) const
{
  return (StrCmp(String(), str) == 0); 
}

bool BText::operator!= (const BChar* str) const
{ 
  return (StrCmp(String(), str) != 0); 
}

bool BText::operator< (const BChar* str) const
{ 
  return (StrCmp(String(), str) <  0); 
}

bool BText::operator> (const BChar* str) const
{ 
  return (StrCmp(String(), str) >  0); 
}

bool BText::operator<= (const BChar* str) const
{ 
  return (!(*this > str)); 
}

bool BText::operator>= (const BChar* str) const
{ 
  return (!(*this < str)); 
}


//--------------------------------------------------------------------
void  BText::EnsureDimensions (BInt& i) const

/*! Checks if a position is greater than the length of the buffer and
 *  if so sends an error
 * \param i Position to check
 */
//--------------------------------------------------------------------
{
  if((i<0)||(i>=length_))
  {
    Error(I2("Forbidden BText access ","Acceso prohibido de BText ") +
             "0 < "+i+" < "+(int)length_);
  }
}


//--------------------------------------------------------------------
BChar BText::operator() (BInt i) const
//--------------------------------------------------------------------
{
  EnsureDimensions(i);
  return(String()[i]);
}


//--------------------------------------------------------------------
BChar BText::Get (BInt i) const

/*! Returns the \a i-th character of the buffer. If the position
 *  is greater than the size of text it returns overflow character
 * \param i Position of character
 */
//--------------------------------------------------------------------
{
  return(((0<=i)&&(i<size_)) ? String()[i] : BText::Overflow());
}


//--------------------------------------------------------------------
BChar BText::Last (BInt index) const

/*! Returns the \a index-th character of the buffer beginning from 
 *  the end of the buffer. 
 * \param index Position of character begging from the end. 0 by default
 */
//--------------------------------------------------------------------
{
  int p = length_-1-index;
  if(!length_ || (p<0) || (p>=length_) ) { return('\0'); }
  return(operator()(p)); 
}



//--------------------------------------------------------------------
ostream& operator<< (ostream& os, const BText& txt)
//--------------------------------------------------------------------
{
  os << txt.String();
  return (os);
}



