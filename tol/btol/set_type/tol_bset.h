/* tol_bset.h: Definition of class BSet (set).
               GNU/TOL Language.

   Copyright (C) 2003, 2004 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BSET_H
#define TOL_BSET_H 1


#include <tol/tol_bcode.h>

//--------------------------------------------------------------------
//Forward references
//--------------------------------------------------------------------
class BSourcePath;
class BNameBlock;

struct BIndexedSyntaxObject
{
  int            position_;
  BSyntaxObject* object_;
  BIndexedSyntaxObject()
  : position_(0),
    object_  (NULL)
  {
  }
  BIndexedSyntaxObject(int position, BSyntaxObject* object)
  : position_(position),
    object_  (object)
  {
  }
};

typedef hash_map_by_name<BIndexedSyntaxObject>::dense_ BObjByIdxNameHash;

//--------------------------------------------------------------------
class TOL_API BSet

/*!  Representation of set of objects
 */
//--------------------------------------------------------------------
{
public:
  enum  BSubType
  {
    Generic,
    MODFile,
    PRJFile,
    DIRFile,
    TOLFile,
    BDCFile,
    BDTFile,
    BSTFile,
    BMTFile,
    BSIFile,
    TSIFile,
    BMIFile,
    Structured,
    Table
  };

private:

#if defined(_MSC_VER)
#pragma warning (disable : 4251)
#endif
  BArray< BSyntaxObject* >  array_;
#if defined(_MSC_VER)
#pragma warning (default : 4251)
#endif
  BSubType subType_;
  BStruct* struct_;
  const BSourcePath* sourcePath_;
  BObjByIdxNameHash* indexByName_;
  const BNameBlock* nameBlock_;

  static BText          orderError_;
  static BCode          order_;
  static BSyntaxObject* objCond_;
  static BSet           unknown_;

protected:
  void   Alloc   (BInt n);
  void   Realloc (BInt n);

public:
  // Constructors and destructors:
  BSet();
  BSet(const BSet& set);
  BSet(BList* arg, BStruct* str,  BSubType sub);
 ~BSet();
  // Messages implementation:
  BText ToText(const BText& separator) const;
  BText Name() const;
  BText Dump() const;
  BText Info() const;
  BSubType SubType() const { return(subType_); }
  void  PutSubType(BSubType t) { subType_ = t; }

  // Instead of arrays start from zero, sets must start from one
  // because this is the most inituitive form for end users.
  BSyntaxObject* GetElement(BInt n)  const;
  BSyntaxObject* GetElement(const char *name)  const;
  BSyntaxObject* operator[] (BInt n)  const;
  //! access operator by name. Return the first element whose name is matched by 'name'
  BSyntaxObject* operator[] (const char *name) const;

  BSyntaxObject* Field(const BText& fieldName) const;

//BAtom** GetBuffer(BInt n=1) { return(array_.GetBuffer()+(n-1)); }
  BInt Card() const { return(array_.Size()); }

  BStruct* Struct(bool showError=false) const;
  const BSourcePath* SourcePath() const { return(sourcePath_); }

  BList* ToList () const;
  BList* ToReversedList () const;
  void   Delete  ();
  void   PrepareStore(int n) { Alloc(n); Alloc(0); }
  void   Append(const BSet& set);
  void   AddElement(BSyntaxObject*);
  void   PutElement(BList* element = NIL);
  void   RobElement(BList* element = NIL);
  void   PutStruct (BStruct* str);
  void   RobStruct (BList* element, BStruct* str, BSubType sub);
  void   ChangeElement(BInt n, BSyntaxObject* obj);
  void   PutSourcePath(const BSourcePath* sp) { sourcePath_ = sp; }

  const BNameBlock* GetNameBlock() const { return(nameBlock_); }
  void   PutNameBlock (const BNameBlock* nameBlock);
  BSyntaxObject* Member(const BText& memberName,
                              BText& errMsg) const;
  BSyntaxObject* PublicMember(const BText& memberName,
                                    BText& errMsg) const;

  BBool   Includes(BSyntaxObject*) const;

  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  void   Copy(const BSet& set);
  BSet&   operator =  (const BSet&  set);
//BSet&   operator =  (BList* element);


  BBool   operator == (const BSet&  set) const;
  BBool   operator != (const BSet&  set) const;
  BBool   operator <  (const BSet&  set) const;
  BBool   operator >  (const BSet&  set) const;
  BBool   operator <= (const BSet&  set) const;
  BBool   operator >= (const BSet&  set) const;

  BSet   Unique       () const;

  BSet   operator << (const BSet&  set) const;
  BSet   operator +  (const BSet&  set) const;
  BSet   operator *  (const BSet&  set) const;
  BSet   operator -  (const BSet&  set) const;

  static BSet&  Unknown() { return(unknown_); }
  static BDat Compare(const BSet* set1, const BSet* set2);

  static void  CleanSort     () { DESTROY(objCond_); }
  static BInt  CompareObjects (const BAny any0, const BAny any1);
  BInt  CompareElements (int n0, int n1);
  BBool  Sort        (const BCode& order);
  BInt  FindSorted    (BSyntaxObject* searched);
  BBool SetIndexByName();
  BBool HasIndexByName();
  int   FindIndexByName(const BText& name);

};

#endif // TOL_BSET_H
