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
#include <tol/tol_bgencon.h>

//these warning disabling macros don't work from included headers
#ifdef _MSC_VER
#pragma warning(disable : 4275)
#pragma warning(disable : 4251)
#endif

//--------------------------------------------------------------------
// forward references
//--------------------------------------------------------------------
class BUserNameBlock;

//--------------------------------------------------------------------
class TOL_API BNameBlock: public BObject
//--------------------------------------------------------------------
{
private:
  BObjByNameHash public_;
  BObjByNameHash private_;
  BSet set_;
  int evLevel_;
  int level_;
  const BNameBlock* father_;
  static const BNameBlock* current_;
  static BNameBlock* unknown_;
  static BObjByNameHash using_;
  static BObjByNameHash usingSymbols_;
  static BObjByClassNameHash usingSymbolsByClass_;
  BText localName_;
  bool createdWithNew_;
public:
  static bool Initialize();
  static BNameBlock&  Unknown() { return(*unknown_); }
  static BDat Compare(const BNameBlock* ns1, const BNameBlock* ns2)
  {
    return(BSet::Compare(&ns1->Set(),&ns2->Set()));
  }
  static const BNameBlock* Current() { return(current_); }
  static const BNameBlock* SetCurrent(const BNameBlock* ns);
  static BSyntaxObject* EvaluateTree(const List* tre);
  static bool Using  (const BSyntaxObject* uns);
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

  int Level() const;
  const BText& LocalName() const { return(localName_); };
  void PutLocalName(const BText& localName) { localName_ = localName; };
  const BObjByNameHash& Public () const { return(public_ ); }
        BObjByNameHash& Public ()       { return(public_ ); }
  const BObjByNameHash& Private() const { return(private_); }
        BObjByNameHash& Private()       { return(private_); }
  const BNameBlock*             Father  () const { return(father_); }
  const BSet&                   Set     () const { return(set_); }
        BSet&                   Set     ()       { return(set_); }
  BText Info() const { return(set_.Info()); }
  int BinWrite(FILE*) const { return 0; }
  int BinRead (FILE*)       { return 0; }

  bool Fill(const BSet& set);
  bool Build();
  BSyntaxObject* Member       (const BText& memberName) const;
  BSyntaxObject* PublicMember (const BText& memberName) const;
  BSyntaxObject* PrivateMember(const BText& memberName) const;
  BList* SelectMembers(BList* lst, const BObjClassify&  oc);
  BList* SelectMembersDeep(BList* lst, const BObjClassify&  oc);
  void RebuildFullNameDeep(BText parentFullName);

  short EnsureIsAssigned() const;

  DeclareClassNewDelete(BNameBlock);


};

//--------------------------------------------------------------------
//Gramatical items
//--------------------------------------------------------------------

#define BUserNameBlock       BGraContensBase  < BNameBlock >
#define BContensNameBlock    BGraContens      < BNameBlock >
#define BSystemNameBlock     BGraConstant     < BNameBlock >
#define BNameBlockTemporary  BTmpContens      < BNameBlock >

template class TOL_API BGraContensBase < BNameBlock >;
template class TOL_API BGraContens     < BNameBlock >;
template class TOL_API BTmpObject      < BUserNameBlock >;
template class TOL_API BTmpContens     < BNameBlock >;

CastingsDeclaration(NameBlock);


#endif // TOL_BNAMEBLOCK_H
