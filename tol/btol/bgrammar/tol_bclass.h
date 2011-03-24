/* bclass.h:  User defined classes in TOL. TOL language.

   Copyright (C) 2003-2007, Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_BCLASS_H
#define TOL_BCLASS_H

#include <tol/tol_barray.h>
#include <tol/tol_list.h>
#include <tol/tol_bobject.h>
#include <tol/tol_bsyntax.h>


//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class TOL_API BMember;
class TOL_API BMemberOwner;
class TOL_API BClass;
struct TOL_API BMbrNum;
template class TOL_API BArray<BClass*>;
template class TOL_API BArray<BMember*>;
template class TOL_API BArray<BMbrNum*>;
//template class TOL_API BArray<BMember*>;

struct TOL_API BMbrNum
{
  BMember* member_;   //Member information
  int      position_; //Appearing order
};

//--------------------------------------------------------------------
class TOL_API BMember
//! User defined Class member
//--------------------------------------------------------------------
{
 public:
  List*          branch_;       //!< Parser branch of the member
  bool           isGood_;       //!< False when there are some error
  BMemberOwner*  parent_;       //!< Member owner (Class or NameBlock)
  BText          name_;         //!< Member name
  BText          declaration_;  //!< Declaration canonical expression
  BText          definition_;   //!< Definition canonical expression
  bool           isMethod_;     //!< True if is a function
  bool           deleteBranch_; //!< If true branch_ must be deleted at destructor
  bool           isStatic_;     //!< Flag for static Class members and methods
  BSyntaxObject* method_;       //!< Store for Class member
  BSyntaxObject* static_;       //!< Store for Class static member or method
 public:
  // Constructors and destructors: bgrammar\class.cpp
  BMember();
  BMember(BMemberOwner* parent, List* branch);
  BMember(const BMember& mbr);
 ~BMember();
  
  const BMember& operator = (const BMember& mbr) 
  { 
    Copy(mbr); 
    return(*this);
  }
  void Copy(const BMember& mbr);
  //! True if member has default value
  bool HasDefVal() const;
  //! Text expression
  BText FullExpression() const;
  int BuildMethod();
  int BuildStatic();
  DeclareClassNewDelete(BMember);
};

//--------------------------------------------------------------------
class TOL_API BMemberOwner
//! Storement and handling of members for Class and NameBlock
//--------------------------------------------------------------------
{
public:
  int lastPosition_;
  //! Type for store searchable members
  typedef hash_map_by_name<BMember*>::dense_ BMemberByNameHash;
  //! Type for store searchable classes
  typedef hash_map_by_name<BClass*>::dense_ BClassByNameHash;
  //! Type for store searchable and sortable members
  typedef hash_map_by_name<BMbrNum*>::dense_ BMbrNumByNameHash;

  //! Type of member owner
  enum BOwnerType { BNONE=0, BCLASS=1, BNAMEBLOCK=2 };

  List* tree_;  //!< Parser branch of the Class 
  bool isGood_;                   //!< False when there are som error
  BClassByNameHash*  parentHash_; //!< Classes which inherites from
  BClassByNameHash*  ascentHash_; //!< Classes which recursively inherites from
  BMemberByNameHash* mbrDecHash_; //!< Members with just declaration 
  BMemberByNameHash* mbrDefHash_; //!< Members with default value
  BMbrNumByNameHash* memberHash_; //!< All members hashed by name
  BArray<BMbrNum*>   member_;
  int notImplementedMethods_;
public:
  // Constructors and destructors: bgrammar\class.cpp
  BMemberOwner();
  BMemberOwner(const BMemberOwner& mbrOwn);
 ~BMemberOwner();

  void CreateMemberHashes();
  void CreateParentHashes();
  void DestroyMemberStore();
  void DestroyParentHashes();
  void DestroyAll();
  const BMemberOwner& operator = (const BMemberOwner& mbrOwn) 
  { 
    Copy(mbrOwn); 
    return(*this);
  }
  void Copy(const BMemberOwner& mbrOwn);

  virtual const BText& getName() const = 0;
  virtual BText		     getFullName() const = 0;
  virtual BText		     getDump() const = 0;
  virtual BOwnerType OwnerType() const = 0;
  void PutTree(List* tree);
  //! True if inherites from a Class called as given class
  bool InheritesFrom  (const BClass* parent) const;
  //! True if inherites from a Class called as given name
  bool InheritesFrom  (const BText& name) const;
  //! Searchs a member with given name
  BMbrNum* FindMbrNum(const BText& name) const;
  //! Searchs a member with given name
  BMember* FindMember(const BText& name) const;
  //! Searchs a member with given declaration and default value
  BMember* FindDefMember  (const BText& declaration) const;
  //! Searchs a member with given declaration without default value
  BMember* FindDecMember  (const BText& declaration) const;
  //! Adds a new member
  bool AddMember(BMember* member);
  //! Adds a new parent class
  bool AddParent(BClass* parent);
  //! Sets the full array of parent classes
  bool SetParentArray (const BArray<BClass*>& parent); 
  //! Adds the list of parsed members
  bool AddMemberList  (List*  memberLst);
  //! Sorts parsed members by appearing order
  void SortMembers();
  BList* Select(BList* lst, bool isStatic, bool isMethod) const;
  BList* SelectStaticMembers(BList* lst) const;
  BList* SelectStaticMethods(BList* lst) const;
  BList* SelectNonStaticMethods(BList* lst) const;
  DeclareClassNewDelete(BMemberOwner);
};


//--------------------------------------------------------------------
class TOL_API BNameBlockMemberOwner: public BMemberOwner
//! User defined Class
//--------------------------------------------------------------------
{
 public:
  BNameBlock* nb_;
  BNameBlockMemberOwner(BNameBlock* nb);
  BOwnerType OwnerType() const;
  const BText& getName() const;
  BText        getFullName() const;
  BText		     getDump() const;
};

//--------------------------------------------------------------------
class TOL_API BClass: public BSyntaxObject, public BMemberOwner
//! User defined Class
//--------------------------------------------------------------------
{
protected:
  BText fullName_;
public:

  bool isDefined_;
  static const BClass* currentClassBuildingInstance_;
  static const BClass* currentStatic_;
  BSyntaxObject* __destroy;
  // Constructors and destructors: bgrammar\class.cpp
  BClass(const BText& name);
 ~BClass();

  // Access & Manipulation: inline
  BOwnerType OwnerType() const { return(BCLASS); }
  const BText& getName() const { return(Name()); }
  BText        getFullName() const { return(FullName()); }
  BText		     getDump() const { return(Dump()); }
  const BText&  getFullNameRef() const;

  // Virtual functions inheritage
	BInt	    Mode	     () const { return(BCLASSMODE); }
	BGrammar* Grammar    () const;
	BText	    Dump	     () const;

  bool CheckAutoDoc();
  void PutDestroy(BSyntaxObject* destroy);
  bool DestroyInstance(BNameBlock* instance);
  //! Evaluates a parsed tree with a Class declaration
  static BClass* PredeclareClass(const BText&name, BClass*&old, bool &ok);
  static BClass* PredeclareClass(const BText&name);
  static BSyntaxObject* Evaluate(const List* tree);
  BSyntaxObject* FindMethod(const BText& memberName, bool fullAccess) const;
  BSyntaxObject* FindStatic(const BText& memberName, bool fullAccess) const;
  BSyntaxObject* FindStaticMethod(const BText& memberName, bool fullAccess) const;
  BSyntaxObject* FindStaticMember(const BText& memberName, bool fullAccess) const;
  DeclareClassNewDelete(BClass);
};

//! Searches a user class wich name is name and returns it.
TOL_API BClass* FindClass(const BText& name, int defined);

#endif // TOL_BCLASS_H
