/* qfile.cpp - GNU/TOL Language.

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

#if defined(_MSC_VER)
#include <win_tolinc.h>
#endif

#include <tol/tol_bqfile.h>
#include <tol/tol_bout.h>
#include <tol/tol_bdir.h>
#include <tol/tol_btimer.h>

#ifndef OPENFLAG
#  if (defined(UNIX) || (defined(_MSC_VER) && (_MSC_VER>=1300)))
#    define OPENFLAG ios::in
#  else
#    define OPENFLAG ios::nocreate
#  endif
#endif

BTraceInit("qfile.cpp");

//--------------------------------------------------------------------
// INICIALIZATION
//   Global variables (static in the class BQFile).
//--------------------------------------------------------------------
static BArray<BQClassifyStruct> _imp_1__;
BInt BQFile::MaxFieldLength_ = 1024;

//--------------------------------------------------------------------
BInt& BQFile::MaxFieldLength()
//--------------------------------------------------------------------
{
    return(MaxFieldLength_);
}

//--------------------------------------------------------------------
BQFile::BQFile(const BText& path)

/*! Constructor
 */
//--------------------------------------------------------------------
    : path_(""), size_(0), isOpen_(BFALSE), regNumber_(0), separator_('\0'),
      regWidth_(0)
{
    Open(path);
    ResetFields();
}


//--------------------------------------------------------------------
BBool BQFile::Open(const BText& path)
//--------------------------------------------------------------------
{
  if(path.HasName())
  {
    path_ = GetAbsolutePath(path);
    Open();
  }
  return(isOpen_);
}


//--------------------------------------------------------------------
BBool BQFile::Open()

/*! Opens the input file
 */
//--------------------------------------------------------------------
{
    if(!isOpen_)
    {
	input_ = new ifstream(path_.String(), OPENFLAG);
	if(!(*input_) || !input_->good()) // Something wrong happens
	{
	    Error(BText("\nCannot open QFile ") + path_);
	    isOpen_ = BFALSE; 
	} else {
	    size_ = BDir(path_).Bytes();
	    regWidth_	 = 0;
	    input_->seekg(0,ios::beg);
	    while(!input_->eof() && (input_->get()!='\n')) {  }
	    regWidth_ = input_->tellg();
	    if(regWidth_<1) {
		Error(BText("\nCannot read from QFile ") + path_);
		input_->close();
		isOpen_ = BFALSE; 
	    } else {
		if(!regNumber_) regNumber_ = size_ / regWidth_;
		input_->seekg(0,ios::beg);
		isOpen_ = BTRUE;
	    }
	}
    }
    return(isOpen_);
}


//--------------------------------------------------------------------
void BQFile::Close()

/*! Closes the input file
 */
//--------------------------------------------------------------------
{
//Std(BText("Closing QFile ")+path_);
    input_->close();
    isOpen_ = BFALSE;
}


//--------------------------------------------------------------------
BChar BQFile::GetChar(BInt n) const

/*! Returns the n-th character
 */
//--------------------------------------------------------------------
{
//    ifstream* inp = (ifstream*) &input_;
    ifstream* inp = input_;
    BInt pos = inp->tellg();
    inp->seekg(n-pos,ios::cur);
    return((BChar)inp->get());
}


//--------------------------------------------------------------------
BText BQFile::GetText(BInt from, BInt len) const

/*! Returns len characters from a given position
 */
//--------------------------------------------------------------------
{
    if(from>size_)      { return(""); }
    if(from<0)	        { from = 0; }
    if(len <0)	        { len  = 0;  }
    if(from+len>=size_) { len  = size_-from-1; }
    
//    ifstream* inp = (ifstream*) &input_;
    ifstream* inp = input_;
    
    BText txt(len+1);
    inp->seekg(from-inp->tellg(),ios::cur);
//input_.get(txt.Buffer(), len+1);
    BBool endField = BFALSE;
    for(BInt n=0; n<len; n++)
    {
	txt.Buffer()[n] = (BChar)inp->get();
    }
    txt.PutLength(len);
    return(txt);
}


//--------------------------------------------------------------------
BReal	BQFile::GetDatField  (BInt reg, BInt field)
//--------------------------------------------------------------------
{
	char * stop;
	BText txt(Compact(GetField(reg,field)));

	return strtod(txt.String(), &stop);
}

//--------------------------------------------------------------------
BDate BQFile::GetDateField (BInt reg, BInt field)
//--------------------------------------------------------------------
{
    BDate date = ConstantDate(Compact(GetField(reg,field)));
    return(date);
}


//--------------------------------------------------------------------
BText BQFile::GetRegister (BInt reg) const

/*! Returns a register
 */
//--------------------------------------------------------------------
{
    if((reg<0)||(reg>=regNumber_)) { return(""); }
    BInt from  = RegPos(reg);
    BInt until = RegPos(reg+1);
    BInt len   = until - from;
    return(GetText(from, len));
}


//--------------------------------------------------------------------
BText BQFile::SelectFields(BInt reg, const BArray<BInt>& fields) const

/*! Returns the contens of each selected field from a reg
 */
//--------------------------------------------------------------------
{
    BText txt;
    for(BInt i = 0; i<fields.Size(); i++)
    {
//  Std(BText("\n  BQFile::SelectFields field ")+i+" : ");
	txt += GetField(reg, fields[i]);
    }
//Std(txt);
    return(txt);
}


//--------------------------------------------------------------------
void BQFile::ResetFields()

/*! Reset fields
 */
//--------------------------------------------------------------------
{
    fieldName_ .ReallocBuffer(100);
    fieldPos_  .ReallocBuffer(100);
    fieldWidth_.ReallocBuffer(100);
    fieldName_ .ReallocBuffer(0);
    fieldPos_  .ReallocBuffer(0);
    fieldWidth_.ReallocBuffer(0);
}


//--------------------------------------------------------------------
void BQFile::AddFields(istream& defFile)

/*! Constructor
 */
//--------------------------------------------------------------------
{
    if(defFile.good())
    {
	BText def;
	Read(defFile,def);
	AddFields(def);
    }
}


//--------------------------------------------------------------------
void BQFile::AddFields(BChar sep, const BText& def)

/*! Constructor
 */
//--------------------------------------------------------------------
{
  BText line;
  separator_ = sep;
  if(!isOpen_) Open();       // Last operation could has closed input_
  if(!(*input_)) return;     // something goes wrong
  BText name = "";
  BInt	width = 0;
//  BChar ch = ' ';
  BChar ch = '\0';
  BInt n=0;
  input_->seekg(0,ios::beg);
  do
  {
    input_->get(ch);
    width++;
//  Std(BText("\nChar ") + (++n) + " = " +ch);
    if((ch==separator_)||(ch=='\n'))
    {
      AddField(name, width);
      name = "";
      width = 0;
    }
    else
    {
      name += ch;
    }
  }
  while(!input_->eof() && ch && (ch!='\n'));
  input_->seekg(0,ios::beg);
  if(def.HasName())
  {
    BArray<BText> tok;
    ReadAllTokens(def,tok,separator_);
    for(BInt i=0; i<fieldName_.Size(); i++)
    {
      fieldName_[i] = Compact(tok[i]);
    }
  }
}


//--------------------------------------------------------------------
void BQFile::AddFields(const BText& def)

/*! Constructor
 */
//--------------------------------------------------------------------
{
    BArray< BArray<BText> > cell;
    ReadAllCells(def,cell,',',':');
    for(BInt i=0; i<cell.Size(); i++)
    {
	BInt  width = atol(Compact(cell[i][1]).String());
	BText name	= cell[i][0];
	AddField(name, width);
    }
}




//--------------------------------------------------------------------
void BQFile::AddField(const BText& name, BInt width)

/*! Returns the position of a given register
 */
//--------------------------------------------------------------------
{
    BText cName = Compact(name);
    BInt size = fieldPos_.Size();
    fieldName_ .ReallocBuffer(size+1);
    fieldPos_  .ReallocBuffer(size+1);
    fieldWidth_.ReallocBuffer(size+1);
    fieldName_  [size] = cName;
    fieldWidth_ [size] = width;
    if(size==0) { fieldPos_[size] = 0; }
    else        { fieldPos_[size] = fieldPos_[size-1]+fieldWidth_[size-1]; }
/*
    Std(BText("\nField : <")	 + size + ">" +
	      "\tName : <"	 + cName+ ">" +
	      "\tPos : <"	 + fieldPos_[size]+ ">" +
	      "\tWidth : <"	 + width + ">");
*/
}


//--------------------------------------------------------------------
BInt BQFile::Field(const BText& field) const

/*! Returns the index of a field or -1 if this not exists
 */
//--------------------------------------------------------------------
{
    return(fieldName_.Find(field, AtmRCmp));
}


//--------------------------------------------------------------------
BText BQFile::FieldByName (BInt reg, const BText& field) const

/*! Returns the contens of field from a reg
 */
//--------------------------------------------------------------------
{
    return(GetField(reg,Field(field)));
}


//--------------------------------------------------------------------
BText BQFile::SelectFieldsByName(BInt reg, const BText& fields) const

/*! Returns the contens of each selected field from a reg
 */
//--------------------------------------------------------------------
{
    BArray<BText> tok;
    ReadAllTokens(fields, tok, ' ');
    BText txt;
    for(BInt i = 0; i<tok.Size(); i++)
    {
	txt += FieldByName(reg, tok[i]);
    }
    return(txt);
}


//--------------------------------------------------------------------
// Static variables for QFile sorting methods
//--------------------------------------------------------------------
static const BQFile*	    current_ = NIL;
static const BArray<BInt>*  fields_  = NIL;
static	     BInt	    cmpNum_  = 0;

//--------------------------------------------------------------------
BInt BQFileCompareOrder(const BAny any0, const BAny any1)
//--------------------------------------------------------------------
{
    cmpNum_++;
//if(!(cmpNum_%10000))
//{ Std(BText("\nBQFileCompareOrder ") + cmpNum_ + "\t" +BTimer::Text()); }

    BInt n0 = *(BInt*)(any0);
    BInt n1 = *(BInt*)(any1);
//BText txt0  = current_->SelectFields(n0,*fields_);
//BText txt1  = current_->SelectFields(n1,*fields_);
    for(BInt i=0; i<fields_->Size(); i++)
    {
	BText txt0	= current_->GetField(n0,(*fields_)[i]);
	BText txt1	= current_->GetField(n1,(*fields_)[i]);
	BInt  cmp	= BText::Compare(&txt0, &txt1);
	if(cmp) { return(cmp); }
    }
    return(0);
};

//--------------------------------------------------------------------
void BQFile::Sort(const BArray<BInt>& fields,
		        BArray<BInt>& order) const
//--------------------------------------------------------------------
{
    BInt i;
    order.ReallocBuffer(regNumber_);
    for(i=0; i<regNumber_; i++) { order[i] = i; }
    current_ = this;
    fields_  = &fields;
    cmpNum_  = 0;
    BTimer tms(BText("BQFile::Sort ")+regNumber_);
    order.Sort(BQFileCompareOrder);
    Std(BText("\nBQFile::Sort Numero de comparaciones = ") + cmpNum_);
    current_ = NIL;
    fields_  = NIL;
};


//--------------------------------------------------------------------
void BQFile::Classify (const BArray<BInt>& fields,
		             BArray<BQClassifyStruct>& classes) const
//--------------------------------------------------------------------
{
  BInt i, j, k=0, n=fields.Size();
  BArray<BText> newFields(n);

  classes.ReallocBuffer(100);
  classes[0].Fields.ReallocBuffer(n);
  classes[0].FirstPosition = 0;
  for(j=0; j<n; j++) { classes[0].Fields[j] = GetField(0,fields[j]); }
  BTimer tms(BText("BQFile::Classify ")+regNumber_);
  for(i=1; i<regNumber_; i++)
  {
    BBool isEqual = BTRUE;
    for(j=0; j<n; j++)
    {
      newFields[j] = GetField(i,fields[j]);
      isEqual	   = isEqual && (newFields[j] == classes[k].Fields[j]);
    }
    if(!isEqual)
    {
      if(k==classes.Size()-1) { classes.ReallocBuffer(2*(1+k)); }
      classes[k].LastPosition = i-1;
      k++;
      classes[k].Fields = newFields;
      classes[k].FirstPosition = i;
    }
  }
  classes[k].LastPosition = i-1;
  k++;
  Std(BText("\nBQFile::Sort Numero de clases = ") + k);
  classes.ReallocBuffer(k);
};


//--------------------------------------------------------------------
BQFixedFile::BQFixedFile(const BText& data)

/*! Constructor
 */
//--------------------------------------------------------------------
    : BQFile(data)
{
//Std(BText("\nCreating BQFixedFile ")+Name());
    if(size_ % regWidth_)
    {
	Warning(I2("Length of fixed file ",
		   "La longitud del fichero de longitud fija ") + Name() +
		I2(" is not a multiple of first line length.",
		   " no es múltiplo de la de la primera línea."));
    }
}


//--------------------------------------------------------------------
BInt BQFixedFile::RegPos(BInt reg) const

/*! Returns the position of a given register
 */
//--------------------------------------------------------------------
{
    BInt pos = reg*regWidth_;
/*
  BChar ch = GetChar(pos-1);
  if((ch!='\n')&&((ch!='\r')))
  {
    Error(I2("File ","El fichero ") + Name() +
	  I2(" is not of fixed length of fields. "
	     "Fail found in line ",
	     " no es de longitud de campos fija. "
	     "Fallo encontrado en la línea ") + reg);
  }
*/
    return(pos);
}


//--------------------------------------------------------------------
BText BQFixedFile::GetField(BInt reg, BInt field) const

/*! Returns a field from a register
 */
//--------------------------------------------------------------------
{
    if((field<0)||(field>=FieldNumber())) { return(""); }
    BInt from   = RegPos(reg) + fieldPos_[field];
    BInt len    = fieldWidth_[field];
    if(separator_)
    {
	len--;
/*
    if(field>0)
    {
      BChar ch = GetChar(from-1);
      if(ch!=separator_)
      {
	Error(I2("File ","El fichero ") + Name() +
	      I2(" is not of fixed length of fields. "
		 "Fail found in line ",
		 " no es de longitud de línea fija. "
		 "Fallo encontrado en la línea ") + reg +
	      I2(" and field ", " y el campo ") +
	      field + " (" + fieldName_[field] + ")");
      }
    }
*/
  }
    return(GetText(from, len));
}


//--------------------------------------------------------------------
BQSepFile::BQSepFile(const BText& data)

/*! Constructor
 */
//--------------------------------------------------------------------
    : BQFile(data), regPosition_(0)
{
//Std(BText("\nCreating BQSepFile ")+Name());
    if(IsOpen()) SetRegPosition();
}

//--------------------------------------------------------------------
void BQSepFile::SetRegPosition()

/*! Constructor
 */
//--------------------------------------------------------------------
{
  BChar ch;
  regPosition_.ReallocBuffer(2*regNumber_);
  regNumber_ = 0;
  if(input_->good())
  {
    input_->seekg(0,ios::beg);

    do {
      regPosition_[regNumber_] = input_->tellg();

      do { 
	  input_->get(ch); 
      } while((ch!='\n')&&(!input_->eof()));

      if(!input_->eof()) {
	regNumber_++;
	if(regNumber_>=regPosition_.Size()-1)
	    regPosition_.ReallocBuffer(2*regNumber_);
      }
    } while(!input_->eof());

    if(!(*input_)) {
	isOpen_ = BFALSE;
	Open();
    }

    regPosition_[regNumber_] = size_;
    regPosition_.ReallocBuffer(regNumber_+1);
  }
}


//--------------------------------------------------------------------
BInt BQSepFile::RegPos(BInt reg) const

/*! Returns the position of a given register
 */
//--------------------------------------------------------------------
{
    if(reg<regNumber_) { return(regPosition_[reg]); }
    else  	       { return(size_); }
}


//--------------------------------------------------------------------
BText BQSepFile::GetField(BInt reg, BInt field) const

/*! Returns a field from a register
 */
//--------------------------------------------------------------------
{
  if((field<0)||(field>=FieldNumber())) { return(""); }
  BChar ch;
  BInt	from = RegPos(reg);
  BInt	len  = 0;

//  ifstream* inp = (ifstream*) &input_;
  ifstream* inp = input_;
  inp->seekg(from,ios::beg);

  BText txt(MaxFieldLength()+1);
  for(BInt i=0; i<field; i++)
  {
    do
    {
      inp->get(ch);
      from++;
    } while((ch!=separator_)&&(ch!='\n')&&ch&&!inp->eof());
  }
  BBool endField = BFALSE;
  do
  {
    inp->get(ch);
    if(len==MaxFieldLength())
    {
      Warning(I2("Maximum length of field in BDB file exceeded. See Real BDBMaxFieldLength variable.",
		 "Ha sido sobrepasada la máxima longitud de campo en ficheros BDB. Mirar variable Real BDBMaxFieldLength."));
    }
    endField = (ch==separator_)||(ch=='\n')||(!ch)||inp->eof()||(len==MaxFieldLength());
    if(!endField)
    {
      txt.Buffer()[len] = ch;
      len++;
    }
    else
    {
      txt.PutLength(len);
    }
  } while(!endField);

//  return(GetText(from, len-1));
  return(txt);
}


//--------------------------------------------------------------------
void BQuery::Select(BArray<BInt>& sel)

/*! Puts in sel the indexes of all registers that match this query
 */
//--------------------------------------------------------------------
{
  sel.ReallocBuffer(10);
  BInt m=0;
  for(BInt n=0; n<file_->RegNumber(); n++)
  {
    if(Check(n))
    {
      if(m>=sel.Size()) { sel.ReallocBuffer(4*n); }
      sel[m]=n;
      m++;
    }
  }
}


//--------------------------------------------------------------------
   void BQuery::Select(BText& sel)

/*! Puts in sel the text of all registers that match this query
 */
//--------------------------------------------------------------------
{
  for(BInt n=0; n<file_->RegNumber(); n++)
  {
    if(Check(n))
    {
      sel += file_->GetRegister(n) + "\n";
    }
  }
}


//--------------------------------------------------------------------
void BQuery::SelectFields(BText& sel, const BArray<BInt>& fields)

/*! Puts in sel the text of all registers that match this query and
 *  all selected fields
 */
//--------------------------------------------------------------------
{
    for(BInt n=0; n<file_->RegNumber(); n++)
    {
	if(Check(n))
	{
//    Std(BText("\nBQuery::SelectFields register ")+n);
	    sel += file_->SelectFields(n, fields) + "\n";
	}
    }
}


//--------------------------------------------------------------------
void BQuery::SelectFieldsByName(BText& sel, const BText& fields)

/*! Puts in sel the text of all registers that match this query and
 *  all selected fields
 */
//--------------------------------------------------------------------
{
    BArray<BText> tok;
    ReadAllTokens(fields, tok, ' ');
    BArray<BInt>	fld(tok.Size());
    for(BInt n=0; n<tok.Size(); n++) { fld[n] = file_->Field(tok[n]); }
    SelectFields(sel, fld);
}


//--------------------------------------------------------------------
BBool BCmpQuery::Check(BInt reg)
//--------------------------------------------------------------------
{
    BText val = file_->GetField(reg, field_).ExternCompact();
    if(comparisson_=="==") { return(val==value_); }
    if(comparisson_=="!=") { return(val!=value_); }
    if(comparisson_==">" ) { return(val> value_); }
    if(comparisson_=="<" ) { return(val< value_); }
    if(comparisson_==">=") { return(val>=value_); }
    if(comparisson_=="<=") { return(val<=value_); }
    return(0);
}
