/* tol_btext.h: BText basic class, attributes and methods.
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

/*! \class BText
//  \brief Defining a high level text type and operations for handling.
           Functionality has been implemented in txt.cpp and txtalgeb.cpp
*/

#ifndef TOL_BTEXT_H
#define TOL_BTEXT_H 1

#include <tol/tol_bcommon.h>


//--------------------------------------------------------------------
/* BText Macros */
#define I2(E,S) I2Function(Out()+E,Out()+S)

#ifdef ENSURE_DIM
#  define BTEXT_ENSURE_DIM(i) EnsureDimensions(i);
#else
#  define BTEXT_ENSURE_DIM(i)
#endif


/* End BText Macros */
//--------------------------------------------------------------------


//! Define language of BText object
enum BIdiomEnum { BENGLISH, BSPANISH };
//! Define text alignment for BText object
enum BAlignType { BLEFT, BRIGHT };

template < class Any > class BArray;

#define BTextCell BArray< BArray<BText> >

#define NOTXT BText::None()

//--------------------------------------------------------------------
inline BInt StrCmp(const BChar* str1, const BChar* str2)

/*! Makes a classic strcmp but preserving of some error when receives
 *  NIL pointers.
 * \param str1 Null-terminated string to compare
 * \param str2 Null-terminated string to compare
 * \return Value indicating the lexicographical relation between the strings:
 *         <ul><li> -1 string1 is less than string2
 *             <li>  0 string1 is the same as string2
 *             <li>  1 string1 is greater than string2</ul>
 */
//--------------------------------------------------------------------
{
  if (str1 && str2)  { return strcmp(str1, str2); }
  if (str1 && *str1) { return 1; }
  if (str2 && *str2) { return -1; }
  return 0;
}


//--------------------------------------------------------------------
class TOL_API BText

/*! Hight level text.
 *  Implements class BText to do high level operations with texts
 *  in Tol.
 */
//--------------------------------------------------------------------
{
 private:

  BChar*  buffer_; //!< Array of chars containing text
  int length_; //!< Length of \a buffer_. From 0 to first '\0' character.
  int size_;   //!< Size of \a buffer_
  unsigned short bfsmem_numPage_;

  /* static attributes */

  static BChar  overflow_;     //!< overflow in BTexts storing operations
  static const BChar* formatBInt_;   //!< Format for integers
  static const BChar* formatBInt64_; //!< Format for integers of 64 bits
  static const BChar* formatBReal_;  //!< Format for reals
  static BBool  language_;     //!< Lenguage selected (BEnglish or BSpanish)
  static BInt	  defectSize_;   //!< Default size for BText objects
  static BInt	  margin_;       //!< Space percentaje of overflow area
  static BText  unknown_;      //!< Text for unknown texts
  static BText  none_;         //!< Empty text

 public:
  static bool InitializeClass();
  /* Constructors and destructors: txt.cpp */

  //! Does nothing for static members initialization
  BText(const BStaticInit& unused) { }

  //! Creates a text and fills it with an empty character
  BText();

  //! Creates a text of size \a size and fills it with the null character
  BText(BInt size);

  //! Creates a text of a given size and fills it with a repeated char
  BText(BInt size, BChar fill);

  //! Creates a text and fills it with an string
  BText(const BChar* str);

  //! Creates a text and fills it with the first chars of a string
  BText(const BChar* str, BInt length);

  //! Creates a text and fills it with a substring of a string
  BText(const BChar* str, BInt from, BInt until);

  //! Creates a text and fills it with a substring of a text object
  BText(const BText& txt, BInt from, BInt until);

  //! Creates a text and fills it with a text object
  BText(const BText& txt);

  //! Text destructor. Frees text buffer's memory
  ~BText();
  
  //! Appends this BText to a binary file
  BInt BinWrite(FILE*) const;

  //! Reads this BText from a binary file
  BInt BinRead (FILE*);

  //! Reads characters form an input stream
  void GetLine(istream& is, BInt maxChars=256, BChar until='\n');
  //! Reads characters form an input stream
  void GetLine(FILE* is, BInt maxChars=256, BChar until='\n');


  /* Access and manipulation: inline */

  //! Returns BText object
  const BText& Name () const;

  //! Returns the content of the actual text
  BText Dump () const;

  //! Returns information about BText object
  BText Info () const;

  //! Returns the string of BText object
  const BChar* String	() const;

  //! Returns the text's buffer
  BChar* Buffer ();

  //! Overloads type operator const BChar*
  operator const BChar* () const
  {
    return String(); 
  }

  //! Overloads type operator BChar*
  operator BChar* ()
  { 
    return Buffer(); 
  }

  //! Returns a duplicate of the buffer
  BChar* DupBuffer () const;

  //! Returns if BText object has name
  BBool HasName () const;

  //! Returns the text's size
  BInt Size () const;

  //! Returns the text's length
  BInt Length () const;

  //! Sets the text's length
  void PutLength (BInt n);

  //! Calculates and sets text's length
  void CalcLength();

  //! Gets BText overflow character
  static BChar& Overflow   ();

  //! Gets BText integer format string
  static const BChar* FormatBInt ();

  //! Gets BText real format string
  static const BChar* FormatBReal();

  //! Gets BText language
  static BBool Language ();

  //! Gets BText margin
  static BInt	Margin ();

  //! Gets BText defectSize
  static BInt DefectSize ();
  
  //! Gets BText Unknown object
  static BText& Unknown ();

  //! Gets BText None object
  static BText& None ();

  //! Sets BText integer format
  static void PutFormatBInt (const char* f);

  //! Sets BText real format
  static void PutFormatBReal (const char* f);

  //! Sets BText language
  static void PutLanguage (BBool l);

  //! Sets BText margin
  static void PutMargin (BInt  m);

  //! Sets BText defectSize
  static void PutDefectSize (BInt  d);

  /*! Compares two BText objects.
    \param txt1 BText object to compare
    \param txt2 BText object to compare
    \return Value indicating the lexicographical relation between the texts.
    \sa StrCmp (BChar*, BChar*)
  */
  static BInt Compare(const BText* txt1, const BText* txt2) 
  {
    return (StrCmp(txt1->String(), txt2->String()));
  }

  /* Text albgebra: */

  //! Copies a string to the actual text
  BText& Copy  (const BChar*);

  //! Copies some chars of a string to the actual text
  BText& Copy  (const BChar*, BInt lenght);

  //! Copies a substring of a string to the actual text
  BText& Copy  (const BChar*, BInt from, BInt until);

  //! Copies a substring of a BText object to the actual text
  BText& Copy  (const BText&, BInt from, BInt until);

  //! Copies the contens of a BText object to the actual text
  BText& Copy  (const BText& txt);

  //! Copies a integer number to the actual text
  BText& Copy  (BInt  number, const BChar* format = NIL);

  //! Copies a integer number to the actual text
  BText& Copy  (unsigned int  number, const BChar* format = NIL);

  //! Copies a long integer number to the actual text
  BText& Copy  (long int number, const BChar* format = NIL);
  
  //! Copies a long integer number to the actual text
  BText& Copy  (unsigned long int number, const BChar* format = NIL);
  
  //! Copies a BINT64 number to the actual text
  BText& Copy  (BINT64 number, const BChar* format = NIL);
  
  //! Copies a real number to the actual text
  BText& Copy  (BReal number, const BChar* format = NIL);

  //! Copies a long number to the actual text
  BText& Copy  (long double number, const BChar* format = NIL);

  //! Copies a character to the actual text
  BText& Copy  (BChar);

  //! Concatenates a string to the actual text
  BText& Concat(const BChar*);

  //! Concatenates the contens of a BText object to the actual text
  BText& Concat(const BText& txt);

  //! Concatenates a integer number to the actual text
  BText& Concat(BInt,  const BChar* format = NIL);

  //! Concatenates a integer number to the actual text
  BText& Concat(unsigned int,  const BChar* format = NIL);

  //! Concatenates a long integer number to the actual text
  BText& Concat(long int,  const BChar* format = NIL);

  //! Concatenates a long integer number to the actual text
  BText& Concat(unsigned long int,  const BChar* format = NIL);

  //! Concatenates a BINT64 to the actual text
  BText& Concat(BINT64, const BChar* format = NIL);

  //! Concatenates a real number to the actual text
  BText& Concat(BReal, const BChar* format = NIL);

  //! Concatenates a long  number to the actual text
  BText& Concat(long double, const BChar* format = NIL);

  //! Concatenates a character to the actual text
  BText& Concat(BChar);

  //! Returns a substring of the actual text
  BText SubString(BInt from, BInt until) const;

  //! Returns a new text with a new size, rigth filling with a fill character
  BText RightPad (BInt newSize, BChar fill=' ') const;

  //! Returns a new text with a new size, left filling with a fill character
  BText LeftPad  (BInt newSize, BChar fill=' ') const;

  //! Returns a new text with a new size, filling with a fill character
  BText Pad (BAlignType align, BInt newSize, BChar fill=' ') const;

  /*! Returns a new text with a new size, concatenating a text at the end,
      filling between them with a fill character
  */
  BText InsidePad(BInt newSize, BChar fill=' ', const BText& last="") const;

  /*! Returns a new text with a new size, concatenating a number at the
      end, filling between them with a fill character
  */
  BText InsidePad(BInt newSize, BChar fill=' ', BInt number=0) const;


  /* BText searching functions: */

  //! Returns BTRUE if the first characters of this text make pattern
  BBool BeginWith(const BText& pattern) const;

  //! Returns BTRUE if the last characters of this text make pattern.
  BBool EndAt(const BText& pattern) const;
  
  //! Finds a text in the actual text
  BInt Find (const BText& txt, BInt from=0) const;

  //! Find advanced a text in the actual text
  BInt FindAdv(const BText& txt, BInt from, BInt ocurrence=1, 
               BInt direction=1) const;

  //! Finds a char in the actual text
  BInt Find (BChar  ch,  BInt from=0) const;

  //! Finds the last ocurrence of a char in the actual text
  BInt FindLast (BChar ch, BInt from=0) const;  

  //! Returns the number of occurrences of a given character in this text
  BInt Occurrences (BChar searched) const;

  //! Returns the number of occurrences of a given text in this text
  BInt Occurrences (const BText& searched) const;

  //! Replaces all \a oldChar ocurrences by \a newChar in the actual text
  BInt Replace (BChar oldChar, BChar newChar);

  //! Replaces all \a oldTxt ocurrences by \a newTxt in the actual text
  BInt Replace (const BText& oldTxt, const BText& newTxt);

  //! Reverse the actual text
  BText& Reverse ();

  //! Converts all characters of the actual text to lower case
  BText& ToLower ();

  //! Converts all characters of the actual text to upper case
  BText& ToUpper ();
  
  //! Compacts the actual text
  BText& Compact ();

  //! Deletes all blanks at left side
  BText LeftCompact ();

  //! Deletes all blanks at right side
  BText RightCompact ();

  //! Deletes all blanks at both left and right sides
  BText ExternCompact ();

  //! Checks if a given pattern matchs with the actual text
  BBool Match (const BText &pattern, BBool caseSensitive=BFALSE) const;

  //! Returns the wrapped text \a txt compacted
  BText Wrap (
    int lineSize, 
    const BText& wordSep_, 
    const BText& prefix_);

  friend ostream& operator<<(ostream& os, const BText& txt);

  //! Replaces all \a oldChar ocurrences by \a newChar in a text \a txt
  friend BText Replace (const BText& txt,
		                    BChar oldChar, BChar newChar);

  //! Replaces all \a oldTxt ocurrences by \a newTxt in a text \a txt
  friend BText Replace (const BText& txt,
		                    const BText& oldTxt,
		                    const BText& newTxt);
  
  //! Returns the text \a txt reversed
  friend BText Reverse (const BText& txt)
  { return(BText(txt).Reverse()); }

  //! Converts all characters of the text \a txt to lower case
  friend BText ToLower (const BText& txt);

  //! Converts all characters of the text \a txt to upper case
  friend BText ToUpper (const BText& txt);

  //! Returns the text \a txt compacted
  friend BText Compact (const BText& txt);


	friend BText TmpFileName(const BText& path=BText::None(), const BText& prefix=BText::None());


  /* Operator overloading: */

  BText& operator= (const BText& txt);
  BText& operator= (const BChar* str);

  BText& operator+= (const BText&      txt);
  BText& operator+= (const BChar*      str);
  BText& operator+= (      BInt        val);
  BText& operator+= (      unsigned int val);
  BText& operator+= (      long int    val);
  BText& operator+= (      unsigned long int    val);
  BText& operator+= (      BINT64      val);
  BText& operator+= (      BReal       val);
  BText& operator+= (	     long double val);
  BText& operator+= (	     BChar       ch );
  
  BText operator+ (const BText&      txt) const;
  BText operator+ (const BChar*      str) const;
  BText operator+ (      BInt        val) const;
  BText operator+ (      unsigned int val) const;
  BText operator+ (      long int    val) const;
  BText operator+ (      long unsigned int val) const;
  BText operator+ (      BINT64      val) const;
  BText operator+ (	     BReal       val) const;
  BText operator+ (	     long double val) const;
  BText operator+ (	     BChar       ch ) const;

  BText& operator<< (const BText&      txt);
  BText& operator<< (const BChar*      str);
  BText& operator<< (	     BInt	       val);
  BText& operator<< (	     long int  	 val);
  BText& operator<< (	     BINT64   	 val);
  BText& operator<< (	     BReal	     val);
  BText& operator<< (	     long double val);
  BText& operator<< (	     BChar	     ch );

  //! Compares two BText objects
  //! friend BInt Compare(const BText* txt1, const BText* txt2);

  bool operator== (const BText& txt) const;
  bool operator!= (const BText& txt) const;
  bool operator<  (const BText& txt) const;
  bool operator>  (const BText& txt) const;
  bool operator<= (const BText& txt) const;
  bool operator>= (const BText& txt) const;

  bool operator== (const BChar* str) const;
  bool operator!= (const BChar* str) const;
  bool operator<  (const BChar* str) const;
  bool operator>  (const BChar* str) const;
  bool operator<= (const BChar* str) const;
  bool operator>= (const BChar* str) const;


  //! Puts a char in a given position of the buffer
  void PutChar (BInt i, BChar ch);

  //! Checks if a position is greater then the length of the buffer
  void EnsureDimensions (BInt& i) const;

  //! Overloads call operator ()
  BChar operator() (BInt i) const;

  //! Returns the \a i-th position of buffer
  BChar Get (BInt i) const;

  /*! Returns the \a index-th position of buffer beginning from
      the end of buffer
  */
  BChar Last(BInt index=0) const;

  //! Reallocate memory for the the text buffer
  void ReallocateBuffer(BInt newSize);

  void base64_encode(const BArray<unsigned char>& binary);
  int base64_decode(BArray<unsigned char>& binary) const;
  void base64_encode_from_file(const BText& path);
  int base64_decode_to_file(const BText& path) const;

 private:

  /* Miscelaneus internal operations: */

  //! Initializes with default values
  void InitBuffer();

  //! Allocate memory for the text buffer
  void AllocateBuffer(BInt size, BChar fill = '\0');

  //! Frees the text buffer's memory
  void DeleteBuffer ();

 private:	
  bool IsAllOk() const;
  short HasAssignedItems() const;
  void AllocItems(unsigned int numItems);
  void FreeItems();
  BChar* AllocItems(int numItems, unsigned short& pgN) ;
  void FreeItems(BChar* buffer, int numItems, unsigned short pgNm) ;

# if (__USE_POOL__==__POOL_BFSMEM__)
  unsigned short _bfsm_VectorPageNum__;
# endif
};

/* ARM-style name-injection of friend declaration not ANSI/C++
   standard. */
//! Replaces all \a oldChar ocurrences by \a newChar in a text \a txt
BText Replace (const BText& txt, BChar oldChar, BChar newChar);
//! Replaces all \a oldTxt ocurrences by \a newTxt in a text \a txt
BText Replace (const BText& txt, const BText& oldTxt, const BText& newTxt);
//! Converts all characters of the text \a txt to lower case
BText ToLower (const BText& txt);
//! Converts all characters of the text \a txt to upper case
BText ToUpper (const BText& txt);
//! Returns the text \a txt compacted
BText Compact (const BText& txt);
BText TmpFileName(const BText& path, const BText& prefix);

//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------

//! Converts a system time value standard localtime format
TOL_API BText TimeToText(BInt t, BInt maxSize = 64, const BChar* fmt = "%a%b%H%S");

//! Returns the number of ocurrences of a character int a text object
TOL_API BInt NumberOfOccurrences(const BText& line, BChar searched='\n');

//! Breaks a text each time appears a separator character
TOL_API BInt ReadAllTokens(const BText& line, BArray<BText>& txt, BChar sep='\n');

//! Breaks a text each time appears a separator character, but along quoted tokens, separator characters will be skiped
TOL_API BInt ReadAllTokens(const BText& line, BArray<BText>& txt, BChar sep, BChar quote, BChar scape);

//! Constructs a matrix of text cells breaking a text into rows by a line
//! separator and columns by a word separator
TOL_API BInt ReadAllCells(const BText& txt,
		                  BArray< BArray<BText> >& cell,
		                  BChar lineSep = '\n',
		                  BChar wordSep = ';');

//! Comparision function to sort by name in arrays of BText
TOL_API BInt BTextOrderCriterium(const BAny txt1, const BAny txt2);

//! Overloads operator >>
TOL_API istream & operator>>  (istream& is, BText&);

//! Reads a maximum of \a maxChars characters from the input stream \a is
//!  stoping at the first occurrence of character \a until.
TOL_API istream & GetLine(istream& is, BText& txt, BInt size=1024, BChar until='\n');


//! Reads characters from an input stream and stores it in a BText object.
TOL_API BInt Read(istream& is, BText& txt, BInt size=1024, BChar until='\0');


//! Reads all characters from an file input stream given its name ands writes
//! them into a text object.
TOL_API BBool ReadFile(BText& data,	const BText& fName);


//! Reads all characters from an file input stream given its name, breaks them
//! into rows by a line separator characer and into columns by a row separator
//! character and writes them into a matrix of text cells
TOL_API BBool ReadCell (BTextCell& cell, const BText& fName, char l='\n', char w=';');

//! Overwrites a file with a given text
TOL_API BBool OverWrite (const BText& fileName, const BText& txt);


//! Appends a given text at the end of a file
TOL_API BBool AppendWrite (const BText& fileName, const BText& txt);


//--------------------------------------------------------------------
// INLINE functions
//--------------------------------------------------------------------

//--------------------------------------------------------------------
inline BChar* StrDup(const BText& txt)

/*! Returns a duplicate of txt->buffer_.
 *  \param txt BText object
*/
//--------------------------------------------------------------------
{ return(strdup(txt.String())); }


//! Return what is called a white char. Used incorrectly by NextChar
inline const char * WhiteChar() { return (" /t/n"); }

//--------------------------------------------------------------------
inline const BChar* NextChar(const BChar* expression)

/*! Returns the direction of the next no white char in \a expression.
 * \param expression String to find next no white char
 */
//--------------------------------------------------------------------
{
//cout << "expression[0]:" << expression[0] ;
//cout << ", strchr(WhiteChar(), expression[0]):" << strchr(WhiteChar(), expression[0]) << endl;
  while(expression[0] && strchr(WhiteChar(), expression[0])) 
  {
    expression++;
  //cout << "expression[0]:" << expression[0];
  //cout << ", strchr(WhiteChar(), expression[0]):" << strchr(WhiteChar(), expression[0]) << endl;
  }
  return(expression);
}


//--------------------------------------------------------------------
inline BText I2Function(const BText& english, const BText& spanish)

/*! Returns the message corresponding to the actual language.
 * \param english Text if the actual language is english
 * \param spanish Text if the actual language is spanish
 */
//--------------------------------------------------------------------
{
  switch(BText::Language())
  {
    case(BENGLISH) : return(english);
    case(BSPANISH) : return(spanish);
  }
  return(english);
}


//--------------------------------------------------------------------
inline BInt StrCmp(const BText& txt1, const BText& txt2)

//! Compare to BText objects
//--------------------------------------------------------------------
{
  return(StrCmp(txt1.String(),txt2.String()));
}

//--------------------------------------------------------------------
   inline int CompareText(const void* v1, const void* v2)
// Compares the entry number of two received pointers to 
// BAlias
//--------------------------------------------------------------------
{
  BText* obj1 = (BText*)v1;
  BText* obj2 = (BText*)v2;
	return( StrCmp(*obj1, *obj2));
}

//! Returns a text object initilized with a formated integer
//--------------------------------------------------------------------
inline BText TxtFmt(BInt d, const BChar* format = "%ld")

/*! Returns a text object initilized with a formated integer \a d
 *  with a given format \a format.
 * \param d Integer number to fill text
 * \param format Format of number
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Concat(d,format);
    return(txt);
}


//! Returns a text object initilized with a formated double
//--------------------------------------------------------------------
inline BText TxtFmt(double d, const BChar* format = "%.16lg")

/*! Returns a text object initilized with a formated double \a d
 *  with a given format \a format.
 * \param d Double number to fill text
 * \param format Format of number
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Concat(d,format);
    return(txt);
}


//! Returns a text object initilized with a formated long
//--------------------------------------------------------------------
inline BText TxtFmt(long double d, const BChar* format = "%.16Lg")

/*! Returns a text object initilized with a formated long \a d with
 *  a given format \a format.
 * \param d Long number to fill text
 * \param format Format of number
 */
//--------------------------------------------------------------------
{
    BText txt;
    txt.Concat(d,format);
    return(txt);
}

std::string base64_encode(const BArray<unsigned char>& binary);
int base64_decode(BArray<unsigned char>& binary, std::string const& encoded_string);

#endif // TOL_BTEXT_H

