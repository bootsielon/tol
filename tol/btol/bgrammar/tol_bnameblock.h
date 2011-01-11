/* tol_bnameblock.h: Class and functions header. TOL Language.

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

#ifndef TOL_BNAMEBLOCK_H
#define TOL_BNAMEBLOCK_H

#include <tol/tol_bcommon.h>


#include <tol/tol_bset.h>
#include <tol/tol_bclass.h>
#include <tol/tol_bgencon.h>

//these warning disabling macros don't work from included headers
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif


#define BUserNameBlock       BGraContensBase  < BNameBlock >
#define BContensNameBlock    BGraContens      < BNameBlock >
#define BSystemNameBlock     BGraConstant     < BNameBlock >
#define BNameBlockTemporary  BTmpContens      < BNameBlock >

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
template<class Any> class BGraContensBase;

typedef hash_map_by_name<int>::dense_ BIntByNameHash;

//--------------------------------------------------------------------
class BRequiredPackage
//--------------------------------------------------------------------
{
private:
  BIntByNameHash hash_;
  BArray<BText> array_;
public:
  BRequiredPackage();
  bool AddRequiredPackage(const BText& name);
  int CountRequiredPackage() const;
  const BText& GetRequiredPackage(int k) const;
};

//--------------------------------------------------------------------
class TOL_API BNameBlock: public BObject
//--------------------------------------------------------------------
{
private:
  static const BNameBlock* current_;
  static BUserNameBlock* building_;
  static BNameBlock* unknown_;
  static BObjByNameHash using_;
  static BObjByNameHash usingSymbols_;
  static BObjByClassNameHash usingSymbolsByClass_;
  static BRequiredPackage globalRequiredPackages_;
  BObjByNameHash public_;
  BObjByNameHash private_;
  BSet set_;
  const BNameBlock* father_;
  BClass* class_;
  BGraContensBase<BNameBlock>* owner_;
  BRequiredPackage* requiredPackages_;
  BText localName_;
  bool createdWithNew_;
  int evLevel_;
  int level_;
  int nonPrivateMembers_;
  bool doingRebuildFullNameDeep;
public:
  bool startedPackage;
  static bool Initialize();
  static BNameBlock&  Unknown() { return(*unknown_); }
  static BDat Compare(const BNameBlock* ns1, const BNameBlock* ns2)
  {
    return(BSet::Compare(&ns1->Set(),&ns2->Set()));
  }
  static const BNameBlock* Current();
  static const BNameBlock* SetCurrent(const BNameBlock* ns);
  static BUserNameBlock* Building();
  static BUserNameBlock* SetBuilding(BUserNameBlock* ns);
  static BSyntaxObject* EvaluateTree(const List* tre);
  static bool Using  (
    const BSyntaxObject* uns,
    bool usingAlsoReadOnly,
    bool usingAlsoPrivate);
  static void Unusing(const BSyntaxObject* uns);
  static BSyntaxObject* LocalMember(const BText& memberName);
  static BSyntaxObject* UsingMember(const BText& memberName);
  static BSyntaxObject* UsingMember(const BObjClassify&  oc, const BText& memberName);
  static BList* Select(BList* lst, const BObjClassify&  oc);

  BNameBlock();
  BNameBlock(const BText& fullName, const BText& localName);
 ~BNameBlock();
  BNameBlock(const BNameBlock& ns);
  BNameBlock& operator= (const BNameBlock& ns);
 
  void Clean();
  BText Dump() const;
  int   Level() const;

  BGraContensBase<BNameBlock>* Owner() const;
  void PutOwner(BGraContensBase<BNameBlock>* owner);
  const BText& Name() const;
  const BText& LocalName() const;

  void PutLocalName(const BText& localName) { localName_ = localName; };
  const BObjByNameHash& Public () const { return(public_ ); }
        BObjByNameHash& Public ()       { return(public_ ); }
  const BObjByNameHash& Private() const { return(private_); }
        BObjByNameHash& Private()       { return(private_); }
  const BSet&       Set     () const { return(set_); }
        BSet&       Set     ()       { return(set_); }
  const BNameBlock* Father  () const { return(father_); }
  const BClass*     Class   () const;
  void  PutClass (BClass* cls);

  bool IsInstanceOf(const BClass* cls) const;
  bool IsInstanceOf(const BText& name) const;

  BText Info() const { return(set_.Info()); }
  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  bool AddElement(BSyntaxObject* obj, bool addToSet, bool fromOis);
  bool CheckMembers();
  bool Build();
  bool Fill(const BSet& set);

  BSyntaxObject* DeepMember   (const BText& memberName) const;
  BSyntaxObject* Member       (const BText& memberName) const;
  BSyntaxObject* PublicMember (const BText& memberName) const;
  BSyntaxObject* PrivateMember(const BText& memberName) const;
  BList* SelectMembers(BList* lst, const BObjClassify&  oc);
  BList* SelectMembersDeep(BList* lst, const BObjClassify&  oc);
  void RebuildFullNameDeep(BText parentFullName, BText memberName);

  short EnsureIsAssigned() const;

  bool AddRequiredPackage(const BText& name);
  int CountRequiredPackage() const;
  const BText& GetRequiredPackage(int k) const;

  static bool AddGlobalRequiredPackage(const BText& name);
  static int CountGlobalRequiredPackage();
  static const BText& GetGlobalRequiredPackage(int k);

  DeclareClassNewDelete(BNameBlock);

private:
  static bool add_using_symbol  (
    const BText& name,
    BObjByNameHash::const_iterator iter,
    bool usingAlsoReadOnly,
    bool usingAlsoPrivate
  );
};

//--------------------------------------------------------------------
//Gramatical items
//--------------------------------------------------------------------

template class TOL_API BGraContensBase < BNameBlock >;
template class TOL_API BGraContens     < BNameBlock >;
template class TOL_API BTmpObject      < BUserNameBlock >;
template class TOL_API BTmpContens     < BNameBlock >;

CastingsDeclaration(NameBlock);


#endif // TOL_BNAMEBLOCK_H
