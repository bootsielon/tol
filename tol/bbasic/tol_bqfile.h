/* tol_bqfile.h - GNU/TOL Language.

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

#ifndef TOL_BQFILE_H
#define TOL_BQFILE_H 1

#include <tol/tol_barray.h>
#include <tol/tol_bdate.h>
#include <tol/tol_bobject.h>

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BQuery;


//--------------------------------------------------------------------
struct BQClassifyStruct
//--------------------------------------------------------------------
{
    public :
	BArray<BText> Fields;
        BInt          FirstPosition;
        BInt	      LastPosition;
};

//--------------------------------------------------------------------
class BQFile : public BAtom
//
/*! Ascii Query File
 */
//--------------------------------------------------------------------
{
protected:
  BText	    path_;
  ifstream* input_;
  BInt	    regNumber_;
  BInt	    size_;
  BBool	    isOpen_;
  BArray<BInt>	fieldPos_;
  BArray<BInt>	fieldWidth_;
  BArray<BText> fieldName_;

  BInt	regWidth_;
  BChar separator_;
  static BInt MaxFieldLength_;
public:
  BQFile(const BText& path="");

 ~BQFile() { Close(); }

  BBool Open();
  BBool Open(const BText& path);
  void	Close();
  const BText&	Name() const { return(path_); }
  BBool IsOpen() { return(isOpen_); }

  virtual BInt	RegPos	    (BInt reg) const = 0;
  virtual BText GetField    (BInt reg, BInt field) const = 0;

  static BInt& MaxFieldLength();
  BReal	GetDatField  (BInt reg, BInt field);
  BDate GetDateField (BInt reg, BInt field);

  void ResetFields();

  void PutFieldName (BInt n, const BText& name) { fieldName_[n]=name; }

  void	AddFields	  (BChar sep, const BText& def="");
  void	AddFields	  (const BText& def);
  void	AddFields	  (istream& defFile);
  void	AddField	  (const BText& name, BInt width);

  BInt	Field		  (const BText& field) const;
  BText FieldName	  (BInt n) const { return(fieldName_  [n]); }
  BInt	FieldWidth	  (BInt n) const { return(fieldWidth_ [n]); }
  BInt	FieldPos	  (BInt n) const { return(fieldPos_   [n]); }
  BInt	FieldNumber	  (	 ) const { return(fieldPos_.Size()); }
  BText FieldByName	  (BInt reg, const BText& field) const;
  BText SelectFieldsByName(BInt reg, const BText& fields) const;

  BText GetRegister (BInt reg) const;
  BText SelectFields(BInt reg, const BArray<BInt>& fields) const;
  BText GetText	    (BInt from, BInt until) const;
  BChar GetChar	    (BInt n) const;
  BChar Separator   () const { return(separator_); }
  BInt	RegNumber   () const { return(regNumber_); }
  BInt	RegWidth    () const { return(regWidth_); }
  BInt	Size	    () const { return(size_); }
  void	Sort	    (const BArray<BInt>& fields,
			   BArray<BInt>& order) const;
  void	Classify    (const BArray<BInt>& fields,
			   BArray<BQClassifyStruct>& classes) const;
};


//--------------------------------------------------------------------
class BQFixedFile : public BQFile

/*! Ascii Query File with Fixed Width
 */
//--------------------------------------------------------------------
{

private:

public:
  BQFixedFile (const BText& data);
 ~BQFixedFile() {}

  BInt	RegPos		  (BInt reg) const ;
  BText GetField	  (BInt reg, BInt field) const ;
};


//--------------------------------------------------------------------
class BQSepFile : public BQFile

/*! Ascii Query File with field separators
 */
//--------------------------------------------------------------------
{
protected:
  BArray<BInt>	regPosition_;
public:
  BQSepFile(const BText& data);
 ~BQSepFile() {}

 BInt	RegPos	       (BInt reg) const;
  BText GetField       (BInt reg, BInt field) const;
  void	SetRegPosition ();
};



//--------------------------------------------------------------------
class BQuery

/*! Query class
 */
//--------------------------------------------------------------------
{
protected:
 BQFixedFile* file_;
public:
  BQuery(BQFixedFile* file) : file_(file) {}
  virtual ~BQuery() {}

  virtual BBool Check(BInt reg) = 0;

  BQFixedFile* File		 () { return(file_); }
  void	       Select		 (BArray<BInt>& sel);
  void	       Select		 (BText& sel);
  void	       SelectFields	 (BText& sel, const BArray<BInt>& fields);
  void	       SelectFieldsByName(BText& sel, const BText& fields);
};


//--------------------------------------------------------------------
class BCmpQuery : public BQuery

/*! Simple Query class
 */
//--------------------------------------------------------------------
{
protected:
  BText fieldName_;
  BInt	field_;
  BText value_;
  BText comparisson_;
public:
  BCmpQuery(	  BQFixedFile* file,
	    const BText&  fieldName,
	    const BText&  cmp,
	    const BText&  value)
    : BQuery(file), fieldName_ (fieldName), value_  (value), comparisson_(cmp)
  {
    field_ = file_->Field(fieldName_);
    value_ = value_.ExternCompact();
  }
 ~BCmpQuery() {}
  BBool Check(BInt reg);
};


//--------------------------------------------------------------------
class BNOTQuery  : public BQuery

/*! NOT Query class
 */
//--------------------------------------------------------------------
{
protected:
  BQuery* q_;
public:
  BNOTQuery(BQuery* q)
  : BQuery(q->File()), q_(q) { }
  BBool Check(BInt reg)
  {
    return(!(q_->Check(reg)));
  }
 ~BNOTQuery() {}
};


//--------------------------------------------------------------------
class BANDQuery  : public BQuery

/*! AND Query class
 */
//--------------------------------------------------------------------
{
protected:
  BQuery* q1_;
  BQuery* q2_;
public:
  BANDQuery(BQuery* q1, BQuery* q2)
  : BQuery(q1->File()), q1_(q1), q2_(q2) { }
 ~BANDQuery() {}
  BBool Check(const BQFile* file, BInt reg)
  {
    return((q1_->Check(reg))&&(q2_->Check(reg)));
  }
};


//--------------------------------------------------------------------
class BORQuery : public BQuery

/*! AND Query class
 */
//--------------------------------------------------------------------
{
protected:
  BQuery* q1_;
  BQuery* q2_;
public:
  BORQuery(BQuery* q1, BQuery* q2)
  : BQuery(q1->File()), q1_(q1), q2_(q2) { }
 ~BORQuery() {}
  BBool Check(const BQFile* file, BInt reg)
  {
    return((q1_->Check(reg))||(q2_->Check(reg)));
  }
};


//--------------------------------------------------------------------
// external functions
//--------------------------------------------------------------------
BInt  ParamCmp(const void* v1, const void* v2);

#endif // TOL_BQFILE_H
