/* tol_oiscreator.h: Object's Indexed Serialization.
                   GNU/TOL Language.

   Copyright (C) 2005 - Bayes Decision, SL (Spain [EU])

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

#ifndef TOL_OIS_CREATOR_H
#define TOL_OIS_CREATOR_H 1

#include <tol/tol_ois.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bcomplex.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bcodgra.h>
#include <tol/tol_bmatgra.h>
#include <tol/tol_bratgra.h>
#include <tol/tol_btsrgra.h>
#include <tol/tol_bclass.h>
#include <tol/tol_bnameblock.h>

class BVMat;


//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define EWrite(x,stream) Ensure(Write(x,stream))

//--------------------------------------------------------------------
  class BOisCreator: public BOis
//--------------------------------------------------------------------
{
public:
  BUserSet* data_;
  int       curHrchyEntry_;

  BArray<BText> allSourcePath_;
  BArray<BSyntaxObject*> tolEnvironment_;
  BArray<BSyntaxObject*> saved_;

  bool WriteBlock(const void* v, size_t size, size_t count, BStream* stream);
  bool Write(const void* v, size_t size, size_t count, BStream* stream);
//bool Write(const          bool&    x, BStream* stream) { return(stream->Write((const void*)&x,1,1)==1); }
  bool Write(const          char&    x, BStream* stream) { return(stream->Write((const void*)&x,1,1)==1); }
  bool Write(const unsigned char&    x, BStream* stream) { return(stream->Write((const void*)&x,1,1)==1); }
  bool Write(const          short&   x, BStream* stream) { return(stream->Write((const void*)&x,2,1)==1); }
  bool Write(const unsigned short&   x, BStream* stream) { return(stream->Write((const void*)&x,2,1)==1); }
  bool Write(const          int&     x, BStream* stream);
  bool Write(const unsigned int&     x, BStream* stream);
  bool Write(const          BINT64&  x, BStream* stream);
  bool Write(const unsigned BINT64&  x, BStream* stream);
  bool Write(const          float&   x, BStream* stream);
  bool Write(const          double&  x, BStream* stream);
  bool Write(const BText& v, BStream* stream);
  bool Write(const BDat& v, BStream* stream);
  bool Write(const BComplex& v, BStream* stream);
  bool Write(const BDate& v, BStream* stream);
  bool Write(const BDat*  buf, int s, BStream* stream);
  bool Write(const BPol&   x, BStream* stream);
  bool Write(const BMemberOwner& owner, 
             const BMemberOwner::BClassByNameHash& x, 
             BStream* stream);
  bool Write(const BClass& cls, BStream* stream);
  bool Write(const BSet& x);

  bool WriteToken(const BToken& x, 
                  BArray<char>& streamBuf,
                  unsigned int& pos);
  bool WriteTree (const List* tre, 
                  BArray<char>& streamBuf,
                  unsigned int& pos);
  bool WriteTree (const List* tre, BStream* stream);

  bool WriteIndexElement(const BOis::BIndexElement& x, 
                         BArray<char>& streamBuf,
                         unsigned int& pos);
  bool WriteGlossaryElement(const BOis::BGlossaryElement& x, 
                            BArray<char>& streamBuf,
                            unsigned int& pos);
  bool WriteIndexSet(const BArray<BOis::BIndexElement>& x, 
                     BArray<char>& streamBuf,
                     unsigned int& pos);
  bool WriteGlossarySet(const BArray<BOis::BGlossaryElement>& x, 
                        BArray<char>& streamBuf,
                        unsigned int& pos);
  bool WriteHierarchy(const BArray<BOis::BIndexElement   >& idx, 
                      const BArray<BOis::BGlossaryElement>& glo, 
                      BStream* stream);

  bool Write    (BSyntaxObject*  v);
  bool WriteData(BSyntaxObject*  v, char tol_type);
  bool WriteData(BUserCode*      v);
  bool WriteData(BUserSet*       v);
  bool WriteData(BUserTimeSerie* v);
  bool WriteData(BUserTimeSet*   v);
  bool WriteData(BUserMat*       v);
  bool WriteData(BGraContensBase<BVMat>* v);
  bool WriteData(BUserPol*       v);
  bool WriteData(BUserRat*       v);
  bool WriteData(BUserNameBlock* v);
  bool WriteSource(int n, int size, const BText& path);

  void SetControl    ();
  void SetDoc        (const BSet* doc);
  void SetOptions    (const BSet* opt);
  void SetData       (BUserSet* uData);

  bool CreateDir     ();
  bool Open          ();
  bool WriteHeader   ();
  bool DoStat        ();
  bool SortHierarchy ();
  bool BuilTolEnv    ();
  bool Flush         ();
  bool Close         ();

public:
  BOisCreator();
 ~BOisCreator();

  void Create(const BText& root,
              BUserSet*    data,
              const BSet*  address=NULL,
              const BSet*  doc=NULL,
              const BSet*  opt=NULL);
  void CreateModule(const BText& tolFile, BUserSet* data);
  bool Build();
  bool RemoveSource();
};

#endif // TOL_OIS_CREATOR_H

