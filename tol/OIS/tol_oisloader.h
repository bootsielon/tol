/* tol_oisloader.h: Object's Indexed Serialization.
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

#ifndef TOL_OIS_LOADER_H
#define TOL_OIS_LOADER_H 1

#include <tol/tol_ois.h>
#include <tol/tol_btoken.h>
#include <tol/tol_bcmpgra.h>
#include <tol/tol_bpolgra.h>
#include <tol/tol_bclass.h>

//--------------------------------------------------------------------
// macros
//--------------------------------------------------------------------
#define ERead(x,stream) Ensure(Read(x,stream))


//--------------------------------------------------------------------
  class BOisLoader: public BOis
//--------------------------------------------------------------------
{
public:
  //------------------------------------------------------------------
     class BOffsetObject
  // TOL object's offset manager class
  //------------------------------------------------------------------
  {
  private:
    BINT64         offset_;
    BSyntaxObject* object_;
  public:  
    void PutOffset(BINT64 offset)
    {
      offset_ = offset;
    }
    void PutNullObject() { object_ = NULL; }
    BSyntaxObject* PutObject(BSyntaxObject* obj);
    BSyntaxObject* ReplaceObject(BSyntaxObject* obj);
    BINT64 Offset() { return(offset_); }
    BSyntaxObject* Object() { return(object_); }
  };

  //------------------------------------------------------------------
    class BTOLDependency
  //------------------------------------------------------------------
  {
   public:
    BText  path_;
    BDate  release_;
    BINT64 bytes_; 
    BTOLDependency()
    : path_(),
      release_(),
      bytes_(-1)
    { }
  };

 public:
  int fileSize_[8];
  BText currentXMLTag_;
  BArray<BOffsetObject> readed_;
  BArray<BTOLDependency> tolSources_;
  BList* localDependencies_;
  BSyntaxObject* data_;
  BText curHrchyPartialPath_;
  bool checkSourceExists_;
  bool checkSourceSize_;
  bool checkSourceDate_;
  bool obsoleteSource_;
  bool lostSource_;
  bool showCheckingTraces_;
  bool oisHasPriorityOnConflict_;

  bool ReadBlock(void* v, size_t size, size_t count, BStream* stream);
  bool Read(void* v, size_t size, size_t count, BStream* stream);
//bool Read(         bool&    x, BStream* stream) { return(stream->Read((void*)&x,1,1)==1); }
  bool Read(         char&    x, BStream* stream) { return(stream->Read((void*)&x,1,1)==1); }
  bool Read(unsigned char&    x, BStream* stream) { return(stream->Read((void*)&x,1,1)==1); }
  bool Read(         short&   x, BStream* stream) { return(stream->Read((void*)&x,2,1)==1); }
  bool Read(unsigned short&   x, BStream* stream) { return(stream->Read((void*)&x,2,1)==1); }
  bool Read(         int&     x, BStream* stream);
  bool Read(unsigned int&     x, BStream* stream);
  bool Read(         BINT64&  x, BStream* stream);
  bool Read(unsigned BINT64&  x, BStream* stream);
  // REVIEW: for compatibility size_t will be written/read as 4 bytes
#if defined(_LP64) || defined(_WIN64)
  bool Read(size_t &     x, BStream* stream)
  {
    unsigned int _x;
    bool ok = this->Read( _x, stream );
    if( ok )
      {
      x = _x;
      }
    return ok;
  }
#endif
  bool Read(         float&   x, BStream* stream);
  bool Read(         double&  x, BStream* stream);
  bool Read(BText& v, BStream* stream);
  bool Read(BDat& v, BStream* stream);
  bool Read(BComplex& v, BStream* stream);
  bool Read(BDate& v, BStream* stream);
  bool Read(BPol&  x, BStream* stream);
  bool Read(BDat*  buf, int s, BStream* stream);
  bool Read(BMemberOwner& cls, BMemberOwner::BClassByNameHash& x, BStream* stream);
  bool Read(BClass& cls, BStream* stream);

  BToken* ReadTokenFromFile(BStream* stream);
  List*   ReadTreeFromFile (BStream* stream);
  BToken* ReadTokenFromStream(BArray<char>& streamBuf, unsigned int& pos);
  List*   ReadTreeFromStream (BArray<char>& streamBuf, unsigned int& pos);
  List*   ReadTree (BStream* stream);
  bool    ReadIndexElement(BOis::BIndexElement& x, 
                           BArray<char>& streamBuf,
                           unsigned int& pos);
  bool    ReadGlossaryElement(BOis::BGlossaryElement& x, 
                              BArray<char>& streamBuf,
                              unsigned int& pos);
  bool    ReadIndexSet(BArray<BOis::BIndexElement>& x, 
                       BArray<char>& streamBuf,
                       unsigned int& pos);
  bool    ReadGlossarySet(BArray<BOis::BGlossaryElement>& x, 
                          BArray<char>& streamBuf,
                          unsigned int& pos);
  bool    ReadHierarchy(BArray<BOis::BIndexElement   >& idx, 
                        BArray<BOis::BGlossaryElement>& glo, 
                        BStream* stream);
  bool    ReadHierarchyIndex();
  void    ShowHierarchy(const BText& margin, 
                              int    n, 
                              int    depth, 
                              int    maxDepth, 
                              int    maxChilds,
                              FILE*  tmp);
  void   ShowHierarchy();
  bool   SearchOffsetInHierarchy(int         hrchyPos, 
                                 BINT64       objectOffset,  
                                 const BSet* partial,
                                 int         depth);
  bool   SearchOffsetInHierarchy(const BSet* partial);

  BSyntaxObject* ReadNextObject(bool addToLocalDependencies);
  BSyntaxObject* PutVariableName(BSyntaxObject* result, 
                                 const BText& name,
                                 char is_referenceable);

  bool BinError           (const char* fileName);
  bool XMLError           ();
  bool XMLGoToNextTag     ();
  bool XMLGetNextTagTitle (char* title, const char* ensure, bool goToNextTag=true);
  bool XMLEnsureEndTag    (const char* title, bool goToNextTag=true);
  bool XMLGetNextTagValue (char* title, char* value, const char* ensure=NULL);
  bool CheckFileSizes     ();
  bool Open               (bool errorWarning=true);
  bool XMLReadHeader      ();
  bool InitReaded         ();
  void GoTo               (const BText& child);
  bool CheckUpdatedSource  (int n, const BText& path);


public:
  BOisLoader();
 ~BOisLoader();

  bool Load(bool errorWarning, 
            bool doShowHierarchy,
            bool checkIsUpdate,
            bool doLoadData,
            int  showHrchyDepth,
            int  showHrchyMaxChilds,
            const BSet* partial);


  BSyntaxObject* GetData() { return(data_); }
  
  void SetOisHasPriorityOnConflict(bool pri) { oisHasPriorityOnConflict_  = pri; }

  void SetCheckSourceExists (bool chk) { checkSourceExists_  =chk; }
  void SetCheckSourceSize   (bool chk) { checkSourceSize_    =chk; }
  void SetCheckSourceDate   (bool chk) { checkSourceDate_    =chk; }
  void SetShowCheckingTraces(bool shw) { showCheckingTraces_ =shw; }

  bool  ObsoleteSource() const { return(obsoleteSource_); }
  bool  HasLostSource() const { return(lostSource_); }
  bool  Check  ();
  BDate Time   ();
  bool  Remove ();

  bool Command(const BText& command);
  static BOisLoader* Open(const BText& root, const BSet*  address=NULL);

  static void Interactive();
  bool Close             ();
  static BSyntaxObject* LoadFull(const BText& root);
};

//--------------------------------------------------------------------
  inline BInt CompareOffset(const BAny any0, const BAny any1)
// Compares the entry number of two received pointers to 
// BOisLoader::BOffsetObject
//--------------------------------------------------------------------
{
  BOisLoader::BOffsetObject* ofob0 = (BOisLoader::BOffsetObject*)(any0);
  BOisLoader::BOffsetObject* ofob1 = (BOisLoader::BOffsetObject*)(any1);
  BINT64 s1 = (BINT64)ofob0->Offset();
  BINT64 s2 = (BINT64)ofob1->Offset();
  assert((s1>=0)&&(s2>=0));
  return((s1<s2)?-1:(s1>s2)?1:0);
}


#endif // TOL_OIS_LOADER_H

